﻿#pragma once
#include "MainDialog.h"

namespace fgo::text_split
{
	//
	// このクラスはメインウィンドウです。
	// このウィンドウはプラグインウィンドウのように振る舞います。
	//
	struct MainWindow : Tools::AviUtl::PluginWindow
	{
		MainDialog mainDialog;

		//
		// ウィンドウを作成します。
		//
		BOOL create()
		{
			return createAsPlugin(
				hive.instance,
				magi.auin.GetAviUtlWindow(),
				Hive::DisplayName,
				WS_EX_NOPARENTNOTIFY,
				WS_CAPTION | WS_SYSMENU | WS_THICKFRAME |
				WS_CLIPSIBLINGS | WS_CLIPCHILDREN,
				100, 100, 300, 300);
		}

		//
		// ウィンドウプロシージャです。
		//
		LRESULT onWndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) override
		{
			switch (message)
			{
			case WM_CREATE:
				{
					MY_TRACE_FUNC("WM_CREATE, 0x%08X, 0x%08X", wParam, lParam);

					if (!mainDialog.create(hwnd))
					{
						MY_TRACE(_T("メインダイアログの作成に失敗しました\n"));

						return -1;
					}

					setTarget(mainDialog);
					resize();

					break;
				}
			}

			return __super::onWndProc(hwnd, message, wParam, lParam);
		}
	};
}
