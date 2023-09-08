﻿#include "pch.h"
#include "Hive.h"
#include "Servant.h"
#include "Hook/Api.h"
#include "Hook/GetMessage.h"
#include "Hook/LowLevelMouse.h" // テスト用です。
#include "Hook/WinEvent.h" // テスト用です。

using namespace fgo::nest;

fgo::Servant* get_servant()
{
	return &servant;
}

//
// エントリポイントです。
//
BOOL APIENTRY DllMain(HINSTANCE instance, DWORD reason, LPVOID reserved)
{
	switch (reason)
	{
	case DLL_PROCESS_ATTACH:
		{
			::DisableThreadLibraryCalls(instance);

			hive.instance = instance;

			hook::api.init();
			hook::get_message.init();
//			hook::low_level_mouse.init();
//			hook::win_event.init();

			break;
		}
	case DLL_PROCESS_DETACH:
		{
//			hook::win_event.exit();
//			hook::low_level_mouse.exit();
			hook::get_message.exit();
			hook::api.exit();

			break;
		}
	}

	return TRUE;
}
