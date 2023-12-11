﻿#pragma once
#include "FilerDialog.h"

class ClientWindow : public CWnd
{
public:

	inline static const LPCTSTR ClassName = _T("FilerGui");

	std::shared_ptr<FileUpdateChecker> config_file_checker;

	//
	// コンストラクタです。
	//
	ClientWindow(HWND hostWindow)
	{
		MY_TRACE_FUNC("0x%08X", hostWindow);

		// クライアントウィンドウを作成します。
		if (!Create())
			throw _T("クライアントウィンドウの作成に失敗しました");

		// Darkアドインを読み込みます。
		if (!loadDark(hostWindow, *this))
			MY_TRACE(_T("Darkアドインの読み込みに失敗しました\n"));

		// ホストウィンドウにクライアントウィンドウのハンドルを渡します。
		Share::Filer::HostWindow::setClientWindow(hostWindow, *this);

		// クライアントウィンドウをメインウィンドウに設定します。
		hive->mainWindow = *this;

		// クライアントウィンドウをMFCのメインウィンドウに設定します。
		AfxGetApp()->m_pMainWnd = this;

		// ホストウィンドウを監視するタイマーを作成します。
		SetTimer(Hive::TimerID::CheckHostWindow, 1000, 0);

		// コンフィグファイルチェッカーを作成します。
		config_file_checker = std::make_shared<FileUpdateChecker>(hive->configFileName);

		// コンフィグファイルを監視するタイマーを作成します。
		SetTimer(Hive::TimerID::CheckConfig, 1000, 0);
	}

	//
	// デストラクタです。
	//
	~ClientWindow()
	{
		MY_TRACE_FUNC("");
	}

	//
	// Darkアドインが存在する場合は読み込みます。
	//
	inline static BOOL loadDark(HWND hostWindow, HWND hwnd)
	{
		MY_TRACE_FUNC("0x%08X, 0x%08X", hostWindow, hwnd);

		if (!Share::Filer::HostWindow::getDark(hostWindow))
			return FALSE;

		TCHAR fileName[MAX_PATH] = {};
		::GetModuleFileName(AfxGetInstanceHandle(), fileName, std::size(fileName));
		::PathRemoveFileSpec(fileName);
		::PathAppend(fileName, _T("Dark.aua"));
		MY_TRACE_TSTR(fileName);

		HMODULE dark = ::LoadLibrary(fileName);
		MY_TRACE_HEX(dark);
		if (!dark) return FALSE;

		void (WINAPI* dark_init)(HWND hwnd) = 0;
		Tools::get_proc(dark, "dark_init", dark_init);
		MY_TRACE_HEX(dark_init);
		if (!dark_init) return FALSE;

		dark_init(hwnd);

		return TRUE;
	}

	//
	// ウィンドウを作成します。
	//
	BOOL Create()
	{
		WNDCLASS wc = {};
		wc.lpszClassName = ClassName;
		wc.lpfnWndProc = AfxWndProc;
		BOOL result = AfxRegisterClass(&wc);

		return __super::CreateEx(
			0,
			ClassName,
			ClassName,
			WS_CAPTION | WS_SYSMENU | WS_THICKFRAME,
			CRect(100, 100, 400, 400),
			0,
			0);
	}

	//
	// コンフィグファイルから設定を読み込みます。
	//
	HRESULT loadConfig()
	{
		MY_TRACE(_T("ClientWindow::loadConfig()\n"));

		try
		{
			MSXML2::IXMLDOMDocumentPtr document(__uuidof(MSXML2::DOMDocument));

			if (document->load(hive->configFileName) == VARIANT_FALSE)
			{
				MY_TRACE(_T("%s の読み込みに失敗しました\n"), hive->configFileName);

				return S_FALSE;
			}

			MSXML2::IXMLDOMElementPtr element = document->documentElement;

			load(element);
		}
		catch (_com_error& e)
		{
			MY_TRACE(_T("%s\n"), e.ErrorMessage());
			return e.Error();
		}

		return S_OK;
	}

	//
	// 設定を読み込みます。
	//
	HRESULT load(const MSXML2::IXMLDOMElementPtr& element)
	{
		MY_TRACE(_T("ClientWindow::load()\n"));

		for (auto& filerDialog : FilerDialog::collection)
		{
			WCHAR name[MAX_PATH] = {};
			::GetWindowTextW(*filerDialog, name, std::size(name));
			MY_TRACE_WSTR(name);

			WCHAR xPath[MAX_PATH] = {};
			::StringCchPrintfW(xPath, std::size(xPath), L"filer[@name=\"%ws\"]", name);
			MY_TRACE_WSTR(xPath);

			MSXML2::IXMLDOMElementPtr filerElement = element->selectSingleNode(xPath);
			MY_TRACE_HEX(filerElement.GetInterfacePtr());
			if (!filerElement) continue;

			filerDialog->load(filerElement);
		}

		return S_OK;
	}

	//
	// コンフィグファイルから設定を読み込みます。
	//
	HRESULT saveConfig()
	{
		MY_TRACE(_T("ClientWindow::saveConfig()\n"));

		try
		{
			MSXML2::IXMLDOMDocumentPtr document(__uuidof(MSXML2::DOMDocument));

			if (document->load(hive->configFileName) == VARIANT_FALSE)
			{
				MY_TRACE(_T("%s の読み込みに失敗しました\n"), hive->configFileName);

				return S_FALSE;
			}

			MSXML2::IXMLDOMElementPtr element = document->documentElement;

			save(element);

			return saveXMLDocument(document, hive->configFileName, L"UTF-16");
		}
		catch (_com_error& e)
		{
			MY_TRACE(_T("%s\n"), e.ErrorMessage());
			return e.Error();
		}

		return S_OK;
	}

	//
	// 設定を保存します。
	//
	HRESULT save(const MSXML2::IXMLDOMElementPtr& element)
	{
		MY_TRACE(_T("ClientWindow::save()\n"));

		for (auto& filerDialog : FilerDialog::collection)
		{
			WCHAR name[MAX_PATH] = {};
			::GetWindowTextW(*filerDialog, name, std::size(name));
			MY_TRACE_WSTR(name);

			WCHAR xPath[MAX_PATH] = {};
			::StringCchPrintfW(xPath, std::size(xPath), L"filer[@name=\"%ws\"]", name);
			MY_TRACE_WSTR(xPath);

			MSXML2::IXMLDOMElementPtr filerElement = element->selectSingleNode(xPath);
			if (!filerElement) continue;

			filerDialog->save(filerElement);
		}

		return TRUE;
	}

protected:

	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg LRESULT OnPreInit(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnPostInit(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnPreExit(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnPostExit(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnPostInitFilerWindow(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()
}; inline std::shared_ptr<ClientWindow> clientWindow;
