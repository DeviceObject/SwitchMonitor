// InjectKeyBoard.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"


HHOOK g_hKeyboardHook = NULL;
PMONITOR_STATUS g_pMonitorStatus = NULL;

BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM lParam) 
{ 
	DWORD dwCurProcessId = *((DWORD*)lParam); 
	DWORD dwProcessId = 0; 

	GetWindowThreadProcessId(hwnd, &dwProcessId); 
	if(dwProcessId == dwCurProcessId && GetParent(hwnd) == NULL)
	{ 
		*((HWND *)lParam) = hwnd;
		return FALSE; 
	} 
	return TRUE; 
} 
HWND GetMainWindow() 
{ 
	DWORD dwCurrentProcessId = GetCurrentProcessId();
	if(!EnumWindows(EnumWindowsProc, (LPARAM)&dwCurrentProcessId)) 
	{     
		return (HWND)dwCurrentProcessId; 
	} 
	return NULL; 
} 
LRESULT CALLBACK KeyBoardProc(int nCode,WPARAM wParam,LPARAM lParam)
{
	//char ch;
	
	if (g_pMonitorStatus->bIsMonitor == TRUE && g_pMonitorStatus->g_bIsShowWindow == FALSE)
	{
		if(((DWORD)lParam & 0x40000000) && nCode == HC_ACTION)
		{
			if (wParam == 'A' || wParam == 'a')
			{
				if (lParam >> 29 & 1)
				{
					OutputDebugString(TEXT("Alt + A"));
				}
			}
			else if (wParam == 'S' || wParam == 's')
			{
				if (lParam >> 29 & 1)
				{
					OutputDebugString(TEXT("Alt + S"));
				}
			}
			else
			{
				SetEvent(g_pMonitorStatus->g_hAttached);
			}
			OutputDebugString(TEXT("KeyBoard Actived\r\n"));
			//if ((wParam == VK_SPACE) || (wParam == VK_RETURN) || (wParam >= 0x2f ) && (wParam <= 0x100) || wParam == VK_MENU) 
			//{
			//	if (wParam == VK_RETURN)
			//		ch='\n';
			//	else
			//	{
			//		BYTE ks[256];
			//		//取得键盘所处的按键状态
			//		GetKeyboardState(ks);
			//		WORD w;
			//		UINT scan;
			//		scan = 0;
			//		ToAscii(wParam,scan,ks,&w,0);
			//		ch = char(w);
			//		if (ch == 'A' || ch == 'a')
			//		{
			//			//OutputDebugString(TEXT("A & a\r\n"));
			//		}
			//		else
			//		{
			//			OutputDebugString(TEXT("KeyBoard Actived\r\n"));
			//		}
			//	}

			//}
		}
	}
	return CallNextHookEx(g_hKeyboardHook,nCode,wParam,lParam);
}
BOOLEAN SetKeyBoard(BOOLEAN bInstall,PMONITOR_STATUS pMonitorStatus)
{
	BOOLEAN bRet = FALSE;

	if (bInstall)
	{
		g_pMonitorStatus = pMonitorStatus;
		if (g_hKeyboardHook == NULL)
		{
			g_hKeyboardHook = SetWindowsHookEx(WH_KEYBOARD,KeyBoardProc,g_hInstDll,0);
			if (g_hKeyboardHook != NULL)
			{
				bRet = TRUE;
			}
			else
			{
				bRet = FALSE;
			}
		}
		else
		{
			bRet = FALSE;
		}
	}
	else
	{
		g_pMonitorStatus = NULL;
		bRet = UnhookWindowsHookEx(g_hKeyboardHook);
		if (bRet == TRUE)
		{
			g_hKeyboardHook = NULL;
		}
	}
	return bRet;
}
