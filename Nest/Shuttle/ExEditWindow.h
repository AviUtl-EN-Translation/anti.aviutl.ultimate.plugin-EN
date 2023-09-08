﻿#pragma once
#include "Shuttle.h"

namespace fgo::nest
{
	//
	// このクラスは拡張編集ウィンドウ用のシャトルです。
	//
	struct ExEditWindow : Shuttle
	{
		HWND dummy = 0;

		//
		// ターゲットにアタッチします。
		//
		void init(const _bstr_t& name, HWND hwnd)
		{
			MY_TRACE(_T("ExEditWindow::init(%s, 0x%08X)\n"), (LPCTSTR)name, hwnd);

			__super::init(name, hwnd);

			// フローティングコンテナのアイコンを設定します。
			HICON icon = (HICON)::GetClassLong(*this, GCL_HICON);
			::SendMessage(*floatContainer, WM_SETICON, ICON_SMALL, (LPARAM)icon);
			::SendMessage(*floatContainer, WM_SETICON, ICON_BIG, (LPARAM)icon);

			{
				// 「VoiceroidUtil」用の処理です。ダミーの拡張編集ウィンドウを作成します。
				// ::FindWindow()で見つけられるように、オーバーラップウィンドウとして作成します。

				WNDCLASS wc = {};
				wc.style = 0;
				wc.hCursor = 0;
				wc.lpfnWndProc = ::DefWindowProc;
				wc.hInstance = ::GetModuleHandle(_T("exedit.auf"));
				wc.lpszClassName = _T("AviUtl");
				ATOM atom = ::RegisterClass(&wc);

				dummy = hive.true_CreateWindowExA(
					WS_EX_LAYERED | WS_EX_TRANSPARENT | WS_EX_TOOLWINDOW,
					"AviUtl",
					"拡張編集",
					WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN,
					0, 0, 0, 0,
					hive.mainWindow, 0, wc.hInstance, 0);
			}
		}

		DWORD onGetTargetNewStyle() override
		{
			// ※ WS_CAPTION を外すとウィンドウの高さ調節にズレが生じるので外さないようにします。
			DWORD style = ::GetWindowLong(*this, GWL_STYLE);
			style &= ~(WS_POPUP | WS_THICKFRAME | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_SYSMENU);
			return style;
		}

		void onSetTargetWindowPos(LPRECT rc) override
		{
			// ターゲットウィンドウのサイズを微調整します。
			rc->bottom -= magi.auin.GetLayerHeight() / 2;
			::SendMessage(*this, WM_SIZING, WMSZ_BOTTOM, (LPARAM)rc);
		}

		//
		// ターゲットのウィンドウプロシージャです。
		//
		LRESULT onWndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) override
		{
			switch (message)
			{
			case WM_SHOWWINDOW:
				{
					// ダミーウィンドウの表示状態を更新します。
					::ShowWindow(dummy, wParam ? SW_SHOW : SW_HIDE);

					break;
				}
			case WM_SETTEXT:
				{
					// ダミーウィンドウのテキストを更新します。
					::SetWindowText(dummy, (LPCTSTR)lParam);

					break;
				}
			}

			return __super::onWndProc(hwnd, message, wParam, lParam);
		}
	}; inline auto exeditWindow = std::make_shared<ExEditWindow>();
}
