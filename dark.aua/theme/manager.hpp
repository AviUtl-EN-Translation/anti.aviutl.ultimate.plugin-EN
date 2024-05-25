﻿#pragma once

namespace apn::dark::theme
{
	//
	// このクラスはテーマレンダラを管理します。
	//
	inline struct Manager
	{
		//
		// テーマレンダラのコレクションです。
		//
		std::unordered_map<HTHEME, Renderer*> collection;

		Renderer* get_renderer(HTHEME theme) const
		{
			// レンダラの使用が抑制されている場合は0を返します。
			if (hive.renderer_locked) return 0;

			auto it = collection.find(theme);
			if (it == collection.end()) return 0;
			return it->second;
		}

		//
		// テーマレンダラをコレクションに追加します。
		//
		void add_renderer(HTHEME theme, Renderer* renderer)
		{
			collection[theme] = renderer;
		}

		//
		// テーマレンダラをコレクションに追加します。
		//
		void add_renderer(HWND hwnd, LPCWSTR vsclass, Renderer* renderer)
		{
			auto theme = ::OpenThemeData(hwnd, vsclass);
			MY_TRACE("{} == {:#010x}\n", vsclass, theme);
			add_renderer(theme, renderer);
		}

		//
		// テーマレンダラをコレクションに追加します。
		//
		void add_renderer_nc(HWND hwnd, LPCWSTR vsclass, Renderer* renderer)
		{
			auto dpi = ::GetDpiForWindow(hwnd);
			auto theme = ::OpenThemeDataForDpi(hwnd, vsclass, dpi);
//			auto theme = ::OpenThemeDataEx(hwnd, vsclass, OTD_NONCLIENT);
			MY_TRACE("{} == {:#010x}\n", vsclass, theme);
			add_renderer(theme, renderer);
		}
	} manager;
}
