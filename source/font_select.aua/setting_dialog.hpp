#pragma once

namespace apn::font_select
{
	//
	// このクラスは設定ダイアログのサブクラスです。
	//
	inline struct SettingDialog : my::Window
	{
		//
		// 初期化処理を実行します。
		//
		BOOL init()
		{
			MY_TRACE_FUNC("");

			// 設定ダイアログをサブクラス化します。
			subclass(magi.exin.get_setting_dialog());

			return TRUE;
		}

		//
		// 後始末処理を実行します。
		//
		BOOL exit()
		{
			MY_TRACE_FUNC("");

			// 設定ダイアログのサブクラス化を解除します。
			unsubclass();

			return TRUE;
		}

		//
		// Displays a custom menu for font selection.
		//
		BOOL on_context_menu(HWND combobox)
		{
			MY_TRACE_FUNC("{:#010x}", combobox);

			// If it is not a font combo box, do nothing.
			if (combobox != magi.exin.get_font_combobox()) return FALSE;

			//
			// Returns TRUE if the specified key is pressed.
			//
			constexpr auto is_key_down = [](DWORD vk) { return ::GetKeyState(vk) < 0; };

			// Do nothing if the modifier key is pressed.
			if (is_key_down(VK_SHIFT) ||
				is_key_down(VK_CONTROL) ||
				is_key_down(VK_MENU) ||
				is_key_down(VK_LWIN) ||
				is_key_down(VK_RWIN))
			{
				return FALSE;
			}

			// If font data has not been read yet
			if (hive.menu_root.nodes.empty())
			{
				auto fonts = create_font_collection(combobox);
				TextReader text_reader(fonts);
				text_reader.read();
			}


			// Create a custom menu for font selection.
			my::menu::unique_ptr<> menu(create_menu(hive.menu_root));

			// Displays the menu.
			auto point = my::get_cursor_pos();
			auto id = ::TrackPopupMenuEx(menu.get(), TPM_NONOTIFY | TPM_RETURNCMD, point.x, point.y, hive.main_window, nullptr);
			if (id == 0) return FALSE;

			// Applies the selected font to the font combo box.
			if (CB_ERR == ::SendMessageW(combobox, CB_SETCURSEL, id - 1, 0)) return FALSE;

			// Notifies the configuration dialog that the selected font has changed.
			::SendMessageW(::GetParent(combobox), WM_COMMAND,
				MAKEWPARAM(::GetDlgCtrlID(combobox), CBN_SELCHANGE), (LPARAM)combobox);

			return TRUE;
		}

		//
		// This is a window procedure.
		//
		virtual LRESULT on_wnd_proc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) override
		{
			switch (message)
			{
			case WM_APPCOMMAND:
				{
					auto cmd  = GET_APPCOMMAND_LPARAM(lParam);

					switch (cmd)
					{
					case APPCOMMAND_BROWSER_BACKWARD:
						{
							// 「戻る」ボタンをハンドルします。
							if (hive.use_backward) on_context_menu((HWND)wParam);

							break;
						}
					case APPCOMMAND_BROWSER_FORWARD:
						{
							// 「進む」ボタンをハンドルします。
							if (hive.use_forward) on_context_menu((HWND)wParam);

							break;
						}
					}

					break;
				}
			case WM_CONTEXTMENU:
				{
					// コンテキストメニューをハンドルします。
				if (hive.use_context_menu) {
					on_context_menu((HWND)wParam); }

					break;
				}
			}

			return __super::on_wnd_proc(hwnd, message, wParam, lParam);
		}
	} setting_dialog;
}
