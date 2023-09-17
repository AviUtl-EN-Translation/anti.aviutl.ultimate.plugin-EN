#pragma once
#include "FilerDialog.h"

class ClientWindow : public CWnd
{
public:

	inline static const LPCTSTR ClassName = _T("FilerGui");

	std::shared_ptr<FileUpdateChecker> config_file_checker;

	//
	// �R���X�g���N�^�ł��B
	//
	ClientWindow(HWND hostWindow)
	{
		MY_TRACE_FUNC("0x%08X", hostWindow);

		// �N���C�A���g�E�B���h�E���쐬���܂��B
		if (!Create())
			throw _T("�N���C�A���g�E�B���h�E�̍쐬�Ɏ��s���܂���");

		// Dark�A�h�C����ǂݍ��݂܂��B
		if (!loadDark(hostWindow, *this))
			MY_TRACE(_T("Dark�A�h�C���̓ǂݍ��݂Ɏ��s���܂���\n"));

		// �z�X�g�E�B���h�E�ɃN���C�A���g�E�B���h�E�̃n���h����n���܂��B
		Share::Filer::HostWindow::setClientWindow(hostWindow, *this);

		// �N���C�A���g�E�B���h�E�����C���E�B���h�E�ɐݒ肵�܂��B
		hive->mainWindow = *this;

		// �N���C�A���g�E�B���h�E��MFC�̃��C���E�B���h�E�ɐݒ肵�܂��B
		AfxGetApp()->m_pMainWnd = this;

		// �z�X�g�E�B���h�E���Ď�����^�C�}�[���쐬���܂��B
		SetTimer(Hive::TimerID::CheckHostWindow, 1000, 0);

		// �R���t�B�O�t�@�C���`�F�b�J�[���쐬���܂��B
		config_file_checker = std::make_shared<FileUpdateChecker>(hive->configFileName);

		// �R���t�B�O�t�@�C�����Ď�����^�C�}�[���쐬���܂��B
		SetTimer(Hive::TimerID::CheckConfig, 1000, 0);
	}

	//
	// �f�X�g���N�^�ł��B
	//
	~ClientWindow()
	{
		MY_TRACE_FUNC("");
	}

	//
	// Dark�A�h�C�������݂���ꍇ�͓ǂݍ��݂܂��B
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

		void (WINAPI* DarkenWindow_init)(HWND hwnd) = 0;
		Tools::get_proc(dark, "DarkenWindow_init", DarkenWindow_init);
		MY_TRACE_HEX(DarkenWindow_init);
		if (!DarkenWindow_init) return FALSE;

		DarkenWindow_init(hwnd);

		return TRUE;
	}

	//
	// �E�B���h�E���쐬���܂��B
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
	// �R���t�B�O�t�@�C������ݒ��ǂݍ��݂܂��B
	//
	HRESULT loadConfig()
	{
		MY_TRACE(_T("ClientWindow::loadConfig()\n"));

		try
		{
			MSXML2::IXMLDOMDocumentPtr document(__uuidof(MSXML2::DOMDocument));

			if (document->load(hive->configFileName) == VARIANT_FALSE)
			{
				MY_TRACE(_T("%s �̓ǂݍ��݂Ɏ��s���܂���\n"), hive->configFileName);

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
	// �ݒ��ǂݍ��݂܂��B
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
	// �R���t�B�O�t�@�C������ݒ��ǂݍ��݂܂��B
	//
	HRESULT saveConfig()
	{
		MY_TRACE(_T("ClientWindow::saveConfig()\n"));

		try
		{
			MSXML2::IXMLDOMDocumentPtr document(__uuidof(MSXML2::DOMDocument));

			if (document->load(hive->configFileName) == VARIANT_FALSE)
			{
				MY_TRACE(_T("%s �̓ǂݍ��݂Ɏ��s���܂���\n"), hive->configFileName);

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
	// �ݒ��ۑ����܂��B
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
