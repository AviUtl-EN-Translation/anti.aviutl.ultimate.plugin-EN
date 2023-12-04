////////////////////////////////////////////////////////////////////////////////
// auls_confirmclose.cpp
// �ҏW���e���ύX����Ă��鎞�̂ݏI���m�F����悤�ɂ���v���O�C��
// -----------------------------------------------------------------------------
// �⑫���
// LoadLibrary �Ŏ������g�̎Q�ƃJ�E���g�𑝂₵�Ă����Ȃ���
// �I�����Ƀv���Z�X�������ԁA���邢�͂����Ǝc���Ă��܂��B
////////////////////////////////////////////////////////////////////////////////

#pragma once

namespace fgo::dirty_check
{
	LPCSTR FILTER_NAME = "�I���m�F";
	LPCSTR FILTER_INFORMATION = "Auls�I���m�F v1.2 forked by karoterra";

	LPCSTR EXEDIT_NAME = "�g���ҏW";
	LPCSTR EXEDIT_92 = "�g���ҏW(exedit) version 0.92 by �j�d�m����";

	inline struct DirtyFlag
	{
		const int* g_undo_id_ptr = 0;
		int  g_undo_id_prev = 0;
		bool g_dirty_flag = false;

		//
		// �g���ҏW�̃t�B���^��Ԃ��܂��B
		//
		static AviUtl::FilterPlugin* get_exedit(AviUtl::FilterPlugin* fp)
		{
			AviUtl::SysInfo si;
			fp->exfunc->get_sys_info(nullptr, &si);
			for (int i = 0; i < si.filter_n; i++) {
				auto fp1 = static_cast<AviUtl::FilterPlugin*>(fp->exfunc->get_filterp(i));
				if (strcmp(fp1->name, EXEDIT_NAME) == 0 && strcmp(fp1->information, EXEDIT_92) == 0) {
					return fp1;
				}
			}
			return nullptr;
		}

		//
		// �������������s���܂��B
		//
		BOOL init(AviUtl::FilterPlugin* fp)
		{
			// �g���ҏW���擾���܂��B
			auto exedit = get_exedit(fp);
			if (exedit == nullptr) {
				MessageBoxA(fp->hwnd, "�Ή�����g���ҏW��������܂���B", FILTER_NAME, MB_OK | MB_ICONERROR);
				return FALSE;
			}

			// �g���ҏW�̃A���h�DID�ւ̃|�C���^���擾���܂��B
			g_undo_id_ptr = reinterpret_cast<int*>(reinterpret_cast<size_t>(exedit->dll_hinst) + 0x244E08 + 12);

			return TRUE;
		}

		//
		// �w�肳�ꂽwp���g���K�[�Ȃ�true��Ԃ��܂��B
		//
		bool is_trigger_wp(WPARAM wp)
		{
			return wp == 5157	// ����
				|| wp == 5097	// �J��
				|| wp == 5118	// �ҏW�v���W�F�N�g���J��
				|| (5596 <= wp && wp <= 5603)	// �ŋߎg�����t�@�C��1-8
				;
		}

		//
		// �w�肳�ꂽ�E�B���h�E���b�Z�[�W���g���K�[�Ȃ�true��Ԃ��܂��B
		//
		bool is_trigger_message(UINT msg, WPARAM wp)
		{
			if (!get()) return false;
			if (msg == WM_CLOSE) return true;
			if (msg == WM_COMMAND && is_trigger_wp(wp)) return true;
			return false;
		}

		//
		// �_�[�e�B�[�t���O��Ԃ��܂��B
		//
		bool get()
		{
			return g_dirty_flag;
		}

		//
		// �_�[�e�B�[�t���O���`�F�b�N���܂��B
		// �_�[�e�B�[�ȏꍇ��true��Ԃ��܂��B
		//
		bool check()
		{
			if (*g_undo_id_ptr == g_undo_id_prev) return false;
			g_undo_id_prev = *g_undo_id_ptr;
			g_dirty_flag = true;
			return true;
		}

		//
		// �_�[�e�B�[�t���O�������l�ɖ߂��܂��B
		//
		void clear()
		{
			g_undo_id_prev = 0;
			g_dirty_flag = false;
		}
	} dirty_flag;

	inline struct AviUtlWindow
	{
		HWND hwnd = 0;
		WNDPROC orig = 0;

		//
		// AviUtl�E�B���h�E�̃E�B���h�E�v���V�[�W�����t�b�N���܂��B
		//
		static LRESULT CALLBACK hook(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp)
		{
			if (dirty_flag.is_trigger_message(msg, wp)) {
				int id = MessageBoxA(hwnd, "�ύX���ꂽ�ҏW�f�[�^������܂��B�ۑ����܂����H",
					FILTER_NAME, MB_YESNOCANCEL | MB_ICONQUESTION | MB_DEFBUTTON1);
				if (id == IDYES) {
					SendMessage(hwnd, WM_COMMAND, 1024, 0); // �ҏW�v���W�F�N�g�̏㏑��
					if (dirty_flag.get()) return 0; // �ۑ����L�����Z�����ꂽ��I�����Ȃ�
				}
				else if (id == IDCANCEL) return 0;
			}
			return CallWindowProc(aviutl_window.orig, hwnd, msg, wp, lp);
		}

		//
		// �������������s���܂��B
		//
		BOOL init(AviUtl::FilterPlugin* fp)
		{
			// AviUtl�E�B���h�E���擾���܂��B
			hwnd = GetWindow(fp->hwnd, GW_OWNER);

			// AviUtl�E�B���h�E���T�u�N���X�����܂��B
			orig = (WNDPROC)SetWindowLongPtr(hwnd, GWLP_WNDPROC, (LONG)hook);

			return TRUE;
		}
	} aviutl_window;

	BOOL func_init(AviUtl::FilterPlugin* fp)
	{
		dirty_flag.init(fp);
		aviutl_window.init(fp);

		return TRUE;
	}

	BOOL func_exit(AviUtl::FilterPlugin* fp)
	{
		return TRUE;
	}

	BOOL func_wndproc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp, AviUtl::EditHandle* editp, AviUtl::FilterPlugin* fp)
	{
		switch (msg) {
		case AviUtl::FilterPlugin::WindowMessage::Update:
			if (!fp->exfunc->is_editing(editp)) break;
			// �v���W�F�N�g���ύX���ꂽ��������Ȃ��̂Ń_�[�e�B�[�t���O���`�F�b�N���܂��B
			dirty_flag.check();
			break;
		case AviUtl::FilterPlugin::WindowMessage::FileClose:
			// �v���W�F�N�g������ꂽ�̂Ń_�[�e�B�[�t���O���N���A���܂��B
			dirty_flag.clear();
			break;
		}
		return FALSE;
	}

	BOOL func_project_save(AviUtl::FilterPlugin* fp, AviUtl::EditHandle* editp, void*, int*)
	{
		// �v���W�F�N�g���ۑ����ꂽ�̂Ń_�[�e�B�[�t���O���N���A���܂��B
		dirty_flag.clear();
		return FALSE;
	}

	BOOL func_modify_title(AviUtl::FilterPlugin* fp, AviUtl::EditHandle* editp, int frame, LPSTR title, int max_title)
	{
		// �v���W�F�N�g���ύX���ꂽ��������Ȃ��̂Ń_�[�e�B�[�t���O���`�F�b�N���܂��B
		if (!dirty_flag.check()) return FALSE;
		std::string str(title);
		sprintf_s(title, max_title, "* %s", str.c_str());
		return TRUE;
	}
}
