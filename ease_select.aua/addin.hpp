﻿#pragma once

namespace apn::ease_select
{
	//
	// このクラスは画像からイージング番号を選択できるようにします。
	//
	inline struct EaseSelect : Addin
	{
		//
		// この仮想関数は、このアドインの識別名が必要なときに呼ばれます。
		//
		virtual LPCWSTR get_addin_name() override
		{
			return hive.c_name;
		}

		//
		// この仮想関数は、このアドインの表示名が必要なときに呼ばれます。
		//
		virtual LPCWSTR get_addin_display_name() override
		{
			return hive.c_display_name;
		}

		//
		// この仮想関数は、初期化のタイミングで呼ばれます。
		//
		virtual BOOL on_init() override
		{
			return TRUE;
		}

		//
		// この仮想関数は、後始末のタイミングで呼ばれます。
		//
		virtual BOOL on_exit() override
		{
			return TRUE;
		}

		//
		// この仮想関数は、ウィンドウの初期化を実行するときに呼ばれます。
		//
		BOOL on_window_init(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam, AviUtl::EditHandle* editp, AviUtl::FilterPlugin* fp) override
		{
			if (!host_window.init()) return FALSE;
			if (!hook_manager.init()) return FALSE;
			if (!gui.init()) return FALSE;

			return FALSE;
		}

		//
		// この仮想関数は、ウィンドウの後始末を実行するときに呼ばれます。
		//
		BOOL on_window_exit(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam, AviUtl::EditHandle* editp, AviUtl::FilterPlugin* fp) override
		{
			gui.exit();
			hook_manager.exit();
			host_window.exit();

			return FALSE;
		}
	} addin;
}
