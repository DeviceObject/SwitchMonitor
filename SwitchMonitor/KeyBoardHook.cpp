#include "stdafx.h"
#include "KeyboardHook.h"

SETKEYBOARD SetKeyboard = NULL;
SETMOUSE SetMouse = NULL;

HMODULE hModKeyBoardDll = NULL;
HMODULE hModMouseDll = NULL;

BOOLEAN InitializeHookData(WCHAR *KeyBoardDllName,CHAR *KeyCallName,WCHAR *MouseDllName,CHAR *MouseCallName)
{
	hModKeyBoardDll = LoadLibrary(KeyBoardDllName);
	if (hModKeyBoardDll == NULL)
	{
		return FALSE;
	}
	SetKeyboard = (SETKEYBOARD)GetProcAddress(hModKeyBoardDll,KeyCallName);
	if (SetKeyboard == NULL)
	{
		return FALSE;
	}

	hModMouseDll = LoadLibrary(MouseDllName);
	if (hModMouseDll == NULL)
	{
		return FALSE;
	}
	SetMouse = (SETMOUSE)GetProcAddress(hModMouseDll,MouseCallName);
	if (SetMouse == NULL)
	{
		return FALSE;
	}
	return TRUE;
}
void UnInitializeHookData()
{
	if (hModKeyBoardDll)
	{
		FreeLibrary(hModKeyBoardDll);
		hModKeyBoardDll = NULL;
	}
	if (hModMouseDll)
	{
		FreeLibrary(hModMouseDll);
		hModMouseDll = NULL;
	}
}
BOOLEAN StartKeyBoardMonitor(PMONITOR_STATUS pMonitor)
{
	BOOLEAN bRet;

	bRet = FALSE;

	bRet = SetKeyboard(TRUE,pMonitor);
	if (bRet == TRUE)
	{
		OutputDebugString(TEXT("安装键盘钩子成功\r\n"));
		bRet = TRUE;
	}
	return bRet;
}
BOOLEAN StopKeyBoardMonitor()
{
	BOOLEAN bRet;

	bRet = FALSE;

	bRet = SetKeyboard(FALSE,NULL);
	if (bRet == TRUE)
	{
		OutputDebugString(TEXT("卸载键盘钩子成功\r\n"));
		bRet = TRUE;
	}
	return bRet;
}

BOOLEAN StartMouseMonitor(PMONITOR_STATUS pMonitor)
{
	BOOLEAN bRet;

	bRet = FALSE;

	bRet = SetMouse(TRUE,pMonitor);
	if (bRet == TRUE)
	{
		OutputDebugString(TEXT("安装鼠标钩子成功\r\n"));
		bRet = TRUE;
	}
	return bRet;
}
BOOLEAN StopMouseMonitor()
{
	BOOLEAN bRet;

	bRet = FALSE;

	bRet = SetMouse(FALSE,NULL);
	if (bRet == TRUE)
	{
		OutputDebugString(TEXT("卸载鼠标钩子成功\r\n"));
		bRet = TRUE;
	}
	return bRet;
}
