#pragma once

namespace apn
{
	//
	// このクラスはコンフィグの入出力を担当します。
	//
	inline struct ConfigIO : StdConfigIO
	{
		//
		// アドイン情報をハイブに登録します。
		//
		void register_addin(
			std::wstring size,
			std::wstring name,
			std::wstring display_name,
			BOOL active,
			std::wstring args,
			std::vector<std::wstring> conflicts)
		{
			hive.addins.emplace_back(std::make_unique<Hive::Addin>(
				size, name, display_name, active, args, conflicts));
		}

		//
		// 初期化処理を実行します。
		//
		BOOL init()
		{
			MY_TRACE_FUNC("");

			hive.config_file_name = magi.get_config_file_name(hive.instance);

			// アドイン情報を登録します。

			register_addin(L"L", L"dark", L"Dark Mode", TRUE, L"", { L"DarkenWindow.aul" });
			register_addin(L"L", L"workspace", L"WorkSpace", TRUE, L"", { L"SplitWindow.aul" });
			register_addin(L"L", L"filer", L"Filer", TRUE, L"", { L"ObjectExplorer.auf" });
			register_addin(L"L", L"reboot", L"Reboot", TRUE, L"", {});
			register_addin(L"L", L"item_copy", L"Item Copy", TRUE, L"", {});
			register_addin(L"L", L"item_wave", L"Item Wave", TRUE, L"", { L"namecage.aua", L"NoScrollText.auf", L"ShowWaveform.auf" });
			register_addin(L"L", L"audio_visualizer", L"Audio Visualizer", TRUE, L"", { L"LevelMeter.auf" });
			register_addin(L"L", L"settings_browser", L"Settings Browser", TRUE, L"", {});

			register_addin(L"M", L"filter_copy", L"Filter Copy", TRUE, L"", { L"CopyFilter.auf" });
			register_addin(L"M", L"filter_drag", L"Filter Drag", TRUE, L"", { L"DragFilter.auf" });
			register_addin(L"M", L"dirty_check", L"Dirty Check", TRUE, L"", { L"auls_confirmclose.auf", L"DirtyCheck.auf" });
			register_addin(L"M", L"output_check", L"Output Check", TRUE, L"", { L"ConfigChecker.auf" });
			register_addin(L"M", L"text_drop", L"Text Drop", TRUE, L"", {});
			register_addin(L"M", L"text_split", L"Text Split", TRUE, L"", { L"SplitText.auf" });
			register_addin(L"M", L"item_align", L"Item Align", TRUE, L"", { L"BuildStairs.auf" });
			register_addin(L"M", L"image_export", L"Image Export", TRUE, L"", { L"SaveImage.auf" });
			register_addin(L"M", L"scene_select", L"Scene Select", TRUE, L"", { L"SelectScene.auf" });
			register_addin(L"M", L"ease_select", L"Easing Select", TRUE, L"", { L"SelectEasing.auf" });
			register_addin(L"M", L"zoom_select", L"Zoom Select", TRUE, L"", { L"SetZoom.auf" });
			register_addin(L"M", L"font_tree", L"Font Tree", TRUE, L"", { L"SelectFavoriteFont.auf" });
			register_addin(L"M", L"font_preview", L"Font Preview", TRUE, L"", {});
			register_addin(L"M", L"font_select", L"Font Select", TRUE, L"", {});
			register_addin(L"M", L"note", L"Note", TRUE, L"", {});
			register_addin(L"M", L"clipboard_viewer", L"Clipboard Viewer", TRUE, L"", {});
			register_addin(L"M", L"one_for_text", L"Edit Text Batch", TRUE, L"", {});

			register_addin(L"S", L"namecage", L"Always Display Item Name", FALSE, L"", { L"NoScrollText.auf", L"ShowWaveform.auf" });
			register_addin(L"S", L"last_frame", L"Audo-addjust Last Frame", TRUE, L"", { L"AdjustLastFrame.auf" });
			register_addin(L"S", L"color_code", L"Color Code", TRUE, L"", { L"AddColorCode.auf" });
			register_addin(L"S", L"editbox_tweaker", L"Editbox Tweaker", TRUE, L"", { L"OptimizeEditBox.auf" });
			register_addin(L"S", L"exedit_tweaker", L"Exedit Tweaker", FALSE, L"", { L"ExEditTweaker.auf" });
			register_addin(L"S", L"dialog_position", L"Dialog Position", TRUE, L"", { L"AdjustDialogPosition.auf" });
			register_addin(L"S", L"dialog_size", L"Dialog Position", TRUE, L"", { L"BigDialog.aul" });
			register_addin(L"S", L"optima", L"Optimization", TRUE, L"", { L"combobox_patch.auf" });

			read_file(hive.config_file_name, hive);

			return TRUE;
		}

		//
		// 後始末処理を実行します。
		//
		BOOL exit()
		{
			MY_TRACE_FUNC("");

			return write_file(hive.config_file_name, hive);
		}

		//
		// カラー配列を読み込みます。
		//
		inline static void read_color_array(const n_json& node, const std::string& name, auto& color_array)
		{
			read_child_nodes(node, name,
				[&](const n_json& color_node, size_t i)
			{
				if (i >= std::size(color_array))
					return FALSE;

				read_color(color_node, color_array[i]);

				return TRUE;
			});
		}

		//
		// カラー配列を書き込みます。
		//
		inline static void write_color_array(n_json& node, const std::string& name, const auto& color_array)
		{
			write_child_nodes(node, name, color_array,
				[&](n_json& color_node, const auto& color, size_t i)
			{
				write_color(color_node, color);

				return TRUE;
			});
		}

		//
		// コンフィグを読み込みます。
		//
		virtual BOOL read_node(n_json& root) override
		{
			MY_TRACE_FUNC("");

			read_string(root, "python_file_name", hive.python_file_name);
			read_color_array(root, "custom_color", magi.custom_colors);

			// アドイン情報を読み込みます。
			read_child_nodes(root, "addin",
				[&](const n_json& addin_node, size_t i)
			{
				// アドインの名前を取得します。
				std::wstring name;
				read_string(addin_node, "name", name);
				MY_TRACE_STR(name);

				// アドインの名前から登録されているアドインを取得します。
				auto it = std::find_if(hive.addins.begin(), hive.addins.end(),
					[&name](const auto& addin) { return addin->name == name; });
				if (it == hive.addins.end()) return TRUE;
				auto& addin = *it;

				// アドインの状態を取得し、登録されているアドインに格納します。
				read_bool(addin_node, "active", addin->active);
				MY_TRACE_INT(addin->active);

				// アドインの引数を取得し、登録されているアドインに格納します。
				read_string(addin_node, "args", addin->args);
				MY_TRACE_STR(addin->args);

				return TRUE;
			});

			return TRUE;
		}

		//
		// コンフィグを書き込みます。
		//
		virtual BOOL write_node(n_json& root) override
		{
			MY_TRACE_FUNC("");

			write_string(root, "python_file_name", hive.python_file_name);
			write_color_array(root, "custom_color", magi.custom_colors);

			write_child_nodes(root, "addin", hive.addins,
				[&](n_json& addin_node, const auto& addin, size_t i)
			{
				write_string(addin_node, "name", addin->name);
				write_bool(addin_node, "active", addin->active);
				write_string(addin_node, "args", addin->args);

				return TRUE;
			});

			return TRUE;
		}
	} config_io;
}
