#pragma once
#include "FilerDialog.h"

class ClientWindow : public CWnd
{
public:

	std::map<std::wstring, std::shared_ptr<FilerDialog>> filerDialogs;
	std::shared_ptr<FileUpdateChecker> config_file_checker;

	//
	// �R���X�g���N�^�ł��B
	//
	ClientWindow(HWND hostWindow)
	{
		MY_TRACE(_T("ClientWindow::ClientWindow(0x%08X)\n"), hostWindow);

		// DarkenWindow ��ǂݍ��݂܂��B
//		loadDarkenWindow();

		// �N���C�A���g�E�B���h�E���쐬���܂��B
		if (!Create())
			throw _T("�N���C�A���g�E�B���h�E�̍쐬�Ɏ��s���܂���");

		// �z�X�g�E�B���h�E�ɃN���C�A���g�E�B���h�E�̃n���h����n���܂��B
		Share::Filer::HostWindow::setClientWindow(hostWindow, GetSafeHwnd());

		// ���̃E�B���h�E�� MFC �̃��C���E�B���h�E�ɐݒ肵�܂��B
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
		MY_TRACE(_T("ClientWindow::~ClientWindow()\n"));
	}

	//
	// DarkenWindow �����݂���ꍇ�͓ǂݍ��݂܂��B
	//
	void loadDarkenWindow()
	{
		TCHAR fileName[MAX_PATH] = {};
		::GetModuleFileName(AfxGetInstanceHandle(), fileName, MAX_PATH);
		::PathRemoveFileSpec(fileName);
		::PathAppend(fileName, _T("..\\DarkenWindow.aul"));
		MY_TRACE_TSTR(fileName);

		HMODULE DarkenWindow = ::LoadLibrary(fileName);
		MY_TRACE_HEX(DarkenWindow);

		if (DarkenWindow)
		{
			typedef void (WINAPI* Type_DarkenWindow_init)(HWND hwnd);
			Type_DarkenWindow_init DarkenWindow_init =
				(Type_DarkenWindow_init)::GetProcAddress(DarkenWindow, "DarkenWindow_init");
			MY_TRACE_HEX(DarkenWindow_init);

			if (DarkenWindow_init)
				DarkenWindow_init(GetSafeHwnd());
		}
	}

	//
	// �E�B���h�E���쐬���܂��B
	//
	BOOL Create()
	{
		const static LPCTSTR className = _T("FilerGui");

		WNDCLASS wc = {};
		wc.lpszClassName = className;
		wc.lpfnWndProc = AfxWndProc;
		BOOL result = AfxRegisterClass(&wc);

		return __super::CreateEx(
			0,
			className,
			className,
			WS_VISIBLE | WS_CAPTION | WS_SYSMENU | WS_THICKFRAME,
			CRect(100, 100, 400, 400),
			0,
			0);
	}

	//
	// �t�@�C���_�C�A���O���쐬���ĕԂ��܂��B
	// ���łɍ쐬�ς݂̏ꍇ�͂��̃t�@�C���_�C�A���O��Ԃ��܂��B
	//
	std::shared_ptr<FilerDialog> createFilerDialog(HWND filerWindow)
	{
		MY_TRACE(_T("HostDialog::createFilerDialog(0x%08X)\n"), filerWindow);

		WCHAR name[MAX_PATH] = {};
		::GetWindowTextW(filerWindow, name, std::size(name));
		MY_TRACE_WSTR(name);

		auto it = filerDialogs.find(name);
		if (it != filerDialogs.end()) return it->second;
		return filerDialogs[name] = std::make_shared<FilerDialog>(filerWindow);
	}

protected:

	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg LRESULT OnFilerWindowCreated(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()
}; inline std::shared_ptr<ClientWindow> clientWindow;
