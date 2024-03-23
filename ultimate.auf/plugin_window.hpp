﻿#pragma once

namespace apn
{
	//
	// このクラスはこのプラグインのウィンドウです。
	//
	inline struct PluginWindow : my::Window
	{
		//
		// アドインの一覧を表示するためのリストボックスです。
		//
		my::Window listbox;

		//
		// 初期化を実行します。
		// 内部的に使用されます。
		//
		BOOL init(AviUtl::FilterPlugin* fp)
		{
			MY_TRACE_FUNC("");

			if (!subclass(fp->hwnd)) return FALSE;

			// クライアント矩形を取得します。
			auto rc = my::get_client_rect(*this);

			// アドイン一覧用のリストボックスを作成します。
			if (!listbox.create(
				WS_EX_CLIENTEDGE,
				WC_LISTBOX,
				_T("アドインの一覧"),
				WS_VISIBLE | WS_CHILD | WS_CLIPSIBLINGS | WS_CLIPCHILDREN |
				WS_HSCROLL | WS_VSCROLL |
				LBS_HASSTRINGS | LBS_NOTIFY | LBS_NOINTEGRALHEIGHT | LBS_MULTICOLUMN | LBS_SORT,
				rc.left, rc.top, my::get_width(rc), my::get_height(rc),
				*this, 0, 0, 0))
			{
				hive.message_box(_T("アドイン一覧リストボックスの作成に失敗しました"));

				return FALSE;
			}

			{
				// コントロールのフォントを設定します。

				AviUtl::SysInfo si = {};
				fp->exfunc->get_sys_info(0, &si);
				MY_TRACE_HEX(si.hfont);

				::SendMessageW(listbox, WM_SETFONT, (WPARAM)si.hfont, TRUE);
			}

			{
				// リストボックスにアドインの一覧を構築します。

				for (const auto& pair : addin_manager.addins.map)
				{
					const auto& addin = pair.second.addin;

					// リストボックスにアイテムを追加します。アイテム名はアドインの表示名です。
					auto index = (int)::SendMessageW(listbox, LB_ADDSTRING,
						0, (LPARAM)addin->get_addin_display_name());

					// アイテムデータにアドインのポインタを設定します。
					::SendMessageW(listbox, LB_SETITEMDATA, index, (LPARAM)addin);
				}
			}

			{
				// AviUtlにメニューアイテムを追加します。
				// リストボックスからソート済みのアドインを取得します。

				// アイテムの数を取得します。
				auto c = (int)::SendMessageW(listbox, LB_GETCOUNT, 0, 0);
				for (decltype(c) i = 0; i < c; i++)
				{
					// アイテムに関連付けられているアドインを取得します。
					auto addin = (Addin*)::SendMessageW(listbox, LB_GETITEMDATA, i, 0);
					MY_TRACE_HEX(addin);

					// アドインを取得できなかった場合は何もしません。
					if (!addin) continue;

					// メニューアイテム名を構築します。
					auto item_name = my::hs(addin->get_addin_display_name());
					MY_TRACE_STR(item_name);

					// AviUtlにメニュー項目を追加します。
					fp->exfunc->add_menu_item(fp, item_name.c_str(), *this,
						magi.c_command_id.c_addin.c_begin + i, 0, AviUtl::ExFunc::AddMenuItemFlag::None);
				}
			}

			return TRUE;
		}

		//
		// 後始末を実行します。
		// 内部的に使用されます。
		//
		BOOL exit()
		{
			MY_TRACE_FUNC("");

			return TRUE;
		}

		//
		// ウィンドウプロシージャです。
		//
		virtual LRESULT on_wnd_proc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) override
		{
			switch (message)
			{
			case WM_DESTROY:
				{
					MY_TRACE_FUNC("WM_DESTROY, {:#010x}, {:#010x}", wParam, lParam);

					break;
				}
			case WM_NCDESTROY:
				{
					MY_TRACE_FUNC("WM_NCDESTROY, {:#010x}, {:#010x}", wParam, lParam);

					MY_TRACE_HWND(hwnd);

					break;
				}
			case WM_SIZE:
				{
					MY_TRACE_FUNC("WM_SIZE, {:#010x}, {:#010x}", wParam, lParam);

					// クライアント矩形を取得します。
					auto rc = my::get_client_rect(*this);

					// リストボックスをリサイズします。
					::SetWindowPos(listbox, 0,
						rc.left, rc.top,
						my::get_width(rc), my::get_height(rc), SWP_NOZORDER);

					break;
				}
			}

			return __super::on_wnd_proc(hwnd, message, wParam, lParam);
		}

		BOOL func_wnd_proc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam, AviUtl::EditHandle* editp, AviUtl::FilterPlugin* fp)
		{
//			MY_TRACE_FUNC("{:#010x}, {:#010x}, {:#010x}, {:#010x}", hwnd, message, wParam, lParam);

			switch (message)
			{
			case AviUtl::FilterPlugin::WindowMessage::Command:
				{
					MY_TRACE_FUNC("Command, {:#010x}, {:#010x}", wParam, lParam);

					if (wParam == 0 && lParam == 0) return TRUE; // AviUtlの再描画のみ行います。

					if (wParam >= magi.c_command_id.c_addin.c_begin && wParam < magi.c_command_id.c_addin.c_end)
					{
						// コマンドからインデックスを取得します。
						auto index = wParam - magi.c_command_id.c_addin.c_begin;
						MY_TRACE_INT(index);

						// コマンドのインデックスからアドインを取得します。
						auto addin = (Addin*)::SendMessageW(listbox, LB_GETITEMDATA, index, 0);
						MY_TRACE_HEX(addin);

						// アドインを取得できなかった場合は何もしません。
						if (!addin) break;

						// アドインにコマンドを送ります。
						return addin->on_window_command(hwnd, message, magi.c_command_id.c_addin.c_command, lParam, editp, fp);
					}

					return addin_manager.fire_window_command(hwnd, message, wParam, lParam, editp, fp);
				}
			case WM_COMMAND:
				{
					auto code = HIWORD(wParam);
					auto control = (HWND)lParam;

					switch (code)
					{
					case LBN_DBLCLK: // リストボックスでダブルクリックが発生しました。
						{
							// 選択アイテムのインデックスを取得します。
							auto index = (int)::SendMessageW(listbox, LB_GETCURSEL, 0, 0);
							MY_TRACE_INT(index);

							// 選択アイテムのインデックスからアドインを取得します。
							auto addin = (Addin*)::SendMessageW(listbox, LB_GETITEMDATA, index, 0);
							MY_TRACE_HEX(addin);

							// アドインを取得できなかった場合は何もしません。
							if (!addin) break;

							// アドインにコマンドを送ります。
							return addin->on_window_command(hwnd, message, magi.c_command_id.c_addin.c_command, lParam, editp, fp);
						}
					}

					break;
				}
			}

			return FALSE;
		}
	} plugin_window;
}
