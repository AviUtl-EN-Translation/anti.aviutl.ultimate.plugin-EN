#pragma once

namespace apn::dialog_size
{
	//
	// このクラスは他クラスから共通して使用される変数を保持します。
	//
	inline struct Hive
	{
		inline static constexpr auto c_name = L"dialog_size";
		inline static constexpr auto c_display_name = L"Dialog Size";

		//
		// このアドインのインスタンスハンドルです。
		//
		HINSTANCE instance = nullptr;

		//
		// コンフィグのファイル名です。
		//
		std::wstring config_file_name;

		//
		// このアドインのメインウィンドウです。
		//
		HWND main_window = nullptr;

		//
		// カスタムテンプレートの配列です。
		//
		struct CustomTemplate {
			BOOL active;
			std::string target_template_name;
			std::wstring target_display_name;
		} custom_templates[9] = {
			{ TRUE, "SYSTEM", L"SYSTEM SETTINGS" },
			{ TRUE, "ENV_CONFIG", L"Preferences" },
			{ TRUE, "INPUTLIST", L"Import Plugins Priority" },
			{ TRUE, "PRIORITYLIST", L"Filter Order" },
			{ TRUE, "NEW_FILE", L"Create a New Project" },
			{ TRUE, "SAVE_OBJECT", L"New Alias" },
			{ TRUE, "GRID_CONFIG", L"Grid Setting" },
			{ TRUE, "SCENE_CONFIG", L"Scene Settings" },
			{ TRUE, "SCRIPT_PARAM", L"Parameter Settings" },
		};

		//
		// 映像サイズのプリセットです。
		//
		struct VideoSize {
			std::wstring display_name;
			std::wstring width;
			std::wstring height;
		};
		std::vector<VideoSize> video_size_collection;

		//
		// 映像レートのプリセットです。
		//
		struct VideoRate {
			std::wstring display_name;
			std::wstring rate;
		};
		std::vector<VideoRate> video_rate_collection;

		//
		// 音声レートのプリセットです。
		//
		struct AudioRate {
			std::wstring display_name;
			std::wstring rate;
		};
		std::vector<AudioRate> audio_rate_collection;

		//
		// メッセージボックスを表示します。
		//
		int32_t message_box(const std::wstring& text, HWND hwnd = nullptr, int32_t type = MB_OK | MB_ICONWARNING) {
			return magi.message_box(text, c_name, hwnd, type);
		}
	} hive;
}
