// keyboardHook.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include <Windowsx.h>

#define ID_NEWNOTEBUTTON 117
#define ID_NEWNOTEDIALOG 119
#define ID_NEWNOTECONTENT 120
#define EXPORTS __declspec(dllexport)

HWND hWnd = NULL;
HHOOK hKeyboardHook = NULL;
HINSTANCE hinstLib;

LRESULT CALLBACK KeyboardProc(int nCode, WPARAM wParam, WPARAM lParam)
{
	if (((DWORD)lParam & 0x40000000) && (HC_ACTION == nCode))
	{
		if (VK_CONTROL == wParam && GetAsyncKeyState(VK_SPACE) & 0x8000)
		{
			SendMessage(hWnd, WM_COMMAND, ID_NEWNOTEBUTTON, TRUE);
		}
		// just to more stable
		if (VK_CONTROL == wParam) 
		{
			if (GetAsyncKeyState(VK_SPACE) & 0x8000)
			{
				SendMessage(hWnd, WM_COMMAND, ID_NEWNOTEBUTTON, TRUE);
			}
		}
	}

	return CallNextHookEx(hKeyboardHook, nCode, wParam, lParam);
}

EXPORTS BOOL CALLBACK installKeyboardHook(HWND myhWnd)
{
	if (!hKeyboardHook)
	{
		hWnd = myhWnd;
		hKeyboardHook = SetWindowsHookEx(WH_KEYBOARD, (HOOKPROC)KeyboardProc, hinstLib, 0);
	}
	return hKeyboardHook ? TRUE : FALSE;
}

EXPORTS BOOL CALLBACK uninstallKeyboardHook(HWND myhWnd)
{
	if (hKeyboardHook)
	{
		UnhookWindowsHookEx(hKeyboardHook);
		hKeyboardHook = NULL;
	}
	return hKeyboardHook ? FALSE : TRUE;
}