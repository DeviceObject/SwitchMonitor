
typedef struct _MONITOR_STATUS
{
	WCHAR SavePicPath[MAX_PATH];
	BOOLEAN bIsMonitor;
	BOOLEAN g_bIsShowWindow;
	HANDLE g_hAttached;
	HANDLE hDrvDevice;
	HANDLE hThread;
	DWORD dwThreadId;
	HWND hCapWnd;
	BOOLEAN bIsCamera;
} MONITOR_STATUS,*PMONITOR_STATUS;

typedef BOOLEAN (*SETKEYBOARD)(BOOLEAN bInstall,PMONITOR_STATUS pMonitorStatus);
typedef BOOLEAN (*SETMOUSE)(BOOLEAN bInstall,PMONITOR_STATUS pMonitorStatus);

BOOLEAN StopKeyBoardMonitor();
BOOLEAN StartKeyBoardMonitor(PMONITOR_STATUS pMonitor);

BOOLEAN StartMouseMonitor(PMONITOR_STATUS pMonitor);
BOOLEAN StopMouseMonitor();

BOOLEAN InitializeHookData(WCHAR *KeyBoardDllName,CHAR *KeyCallName,WCHAR *MouseDllName,CHAR *MouseCallName);
void UnInitializeHookData();

extern SETKEYBOARD SetKeyboard;
extern SETMOUSE SetMouse;