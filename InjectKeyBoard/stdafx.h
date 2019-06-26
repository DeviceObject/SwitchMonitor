// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
// Windows Header Files:
#include <windows.h>



// TODO: reference additional headers your program requires here
typedef struct _MONITOR_STATUS
{
	WCHAR SavePicPath[MAX_PATH];
	BOOLEAN bIsMonitor;
	BOOLEAN g_bIsShowWindow;
	HANDLE g_hAttached;
	HANDLE hDrvDevice;
} MONITOR_STATUS,*PMONITOR_STATUS;

extern HHOOK g_hKeyboardHook;

extern HINSTANCE g_hInstDll;