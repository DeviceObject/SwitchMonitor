// InjectMouse.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"

PMONITOR_STATUS g_pMonitorStatus = NULL;

HHOOK g_hMouseHook = NULL;

LRESULT CALLBACK MouseHook(int nCode,WPARAM wParam,LPARAM lParam)
{
	if (g_pMonitorStatus->bIsMonitor == TRUE && g_pMonitorStatus->g_bIsShowWindow == FALSE)
	{
		if(nCode < 0 || nCode == HC_NOREMOVE)
		{
			return CallNextHookEx(g_hMouseHook,nCode,wParam,lParam);
		}
		if(nCode == HC_ACTION)
		{
			if(wParam == WM_MOUSEWHEEL || wParam == WM_RBUTTONDOWN || wParam == WM_LBUTTONDOWN)
			{
				OutputDebugString(TEXT("Mouse Actived\r\n"));
				SetEvent(g_pMonitorStatus->g_hAttached);
			}
		}
	}
	return CallNextHookEx(g_hMouseHook,nCode,wParam,lParam);
}
BOOLEAN SetMouse(BOOLEAN bInstall,PMONITOR_STATUS pMonitorStatus)
{
	BOOLEAN bRet = FALSE;

	if (bInstall)
	{
		g_pMonitorStatus = pMonitorStatus;
		if (g_hMouseHook == NULL)
		{
			g_hMouseHook = SetWindowsHookEx(WH_MOUSE,MouseHook,g_hInstDll,0);
			if (g_hMouseHook != NULL)
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
		bRet = UnhookWindowsHookEx(g_hMouseHook);
		if (bRet == TRUE)
		{
			g_hMouseHook = NULL;
		}
	}
	return bRet;
}


