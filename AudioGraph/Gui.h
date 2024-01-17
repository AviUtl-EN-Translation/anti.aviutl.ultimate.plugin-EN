#pragma once
#include "Hive.h"

namespace fgo::audio_graph
{
	inline struct Gui
	{
		//
		// �N���C�A���g�v���Z�X�̃v���Z�X���ł��B
		//
		PROCESS_INFORMATION pi = {};

		//
		// ���������������s���܂��B
		//
		BOOL init()
		{
			MY_TRACE_FUNC("");

			TCHAR path[MAX_PATH] = {};
			::GetModuleFileName(hive.instance, path, std::size(path));
			::PathRemoveExtension(path);
			::StringCchCat(path, std::size(path), _T("Gui.exe"));
			MY_TRACE_TSTR(path);

			TCHAR args[MAX_PATH] = {};
			::StringCchPrintf(args, std::size(args), _T("0x%08p"), hive.mainWindow);
			MY_TRACE_TSTR(args);

			STARTUPINFO si = { sizeof(si) };
			if (!::CreateProcess(
				path,           // No module name (use command line)
				args,           // Command line
				NULL,           // Process handle not inheritable
				NULL,           // Thread handle not inheritable
				FALSE,          // Set handle inheritance to FALSE
				0,              // No creation flags
				NULL,           // Use parent's environment block
				NULL,           // Use parent's starting directory 
				&si,            // Pointer to STARTUPINFO structure
				&pi))         // Pointer to PROCESS_INFORMATION structur
			{
				MY_TRACE(_T("::CreateProcess() failed.\n"));

				return FALSE;
			}

			// �N���C�A���g�v���Z�X�̃��b�Z�[�W���[�v���J�n�����܂őҋ@���܂��B
			::WaitForInputIdle(pi.hProcess, INFINITE);

			return TRUE;
		}

		//
		// ��n�����������s���܂��B
		//
		BOOL exit()
		{
			MY_TRACE_FUNC("");
#if 1
			// �N���C�A���g�v���Z�X���I�������܂��B
			::PostThreadMessage(pi.dwThreadId, WM_QUIT, 0, 0);
#else
			// �N���C�A���g�v���Z�X�������I�ɏI�������܂��B
			::TerminateProcess(pi.hProcess, 0);
#endif
			return TRUE;
		}
	} gui;
}
