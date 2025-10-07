#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
#include <stdexcept>

#include "core/interfaces.hpp"
#include "core/hooks.hpp"
#include "core/menu.hpp"



DWORD WINAPI CheatThread(LPVOID instance) {
	try {
		interfaces::create();
		menu::create();
		hooks::create();
	} catch (const std::exception& e) {
		hooks::destroy();
		menu::destroy();
		interfaces::destroy();
		MessageBoxA(nullptr, e.what(), "Error", MB_ICONERROR);
		FreeLibraryAndExitThread((HMODULE)instance, 1);
	}

	while (!(GetAsyncKeyState(VK_END) & 1)) {
		Sleep(100);
	}
	
	hooks::destroy();
	menu::destroy();
	interfaces::destroy();

	MessageBoxA(nullptr, "Cheat is exiting.", "Info", MB_ICONINFORMATION);

	FreeLibraryAndExitThread((HMODULE)instance, 0);

	return 0;
}

DWORD APIENTRY DllMain(HINSTANCE instance, DWORD reason, LPVOID) {
	if (reason == DLL_PROCESS_ATTACH) {
		DisableThreadLibraryCalls(instance);


		HANDLE thread = CreateThread(NULL, 0, CheatThread, instance, 0, nullptr);
		if (thread != NULL && thread != INVALID_HANDLE_VALUE) {
			CloseHandle(thread);
			return TRUE;
		}
	}

	return FALSE;
}