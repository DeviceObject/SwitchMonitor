#include <winsvc.h>
#include <winioctl.h>

#define MAX_SHOWMSG				1024
#define MAX_THREAD_COUNT		64
#define SWITCHDRV_PORT_NAME		L"\\SwitchPort"
//#define	EVENT_NAME				L"\\BaseNamedObjects\\SwitchEvent"

#define DEIVCE_NAME	L"\\Device\\SwitchDrv"
#define SYMLINK_NAME L"\\DosDevices\\SwitchDrv"

#define  FILE_DEVICE_X_PROTECT 0x8000
#define  X_PROTECT_CTL_BASE  0x800
//IO«Î«Û∂®“Â
#define  CTL_CODE_SHIT(i)  CTL_CODE(FILE_DEVICE_X_PROTECT,X_PROTECT_CTL_BASE+i,METHOD_BUFFERED,FILE_ANY_ACCESS)
#define SWITCHDRV_INITIAL_GLOBAL					CTL_CODE_SHIT(0)
#define SWITCHDRV_UNINIT_GLOBAL						CTL_CODE_SHIT(1)
#define SWITCHDRV_START_MONITOR						CTL_CODE_SHIT(2)
#define SWITCHDRV_STOP_MONITOR						CTL_CODE_SHIT(3)
#define SWITCHDRV_GET_EXPLORER						CTL_CODE_SHIT(4)
#define SWITCHDRV_SET_WINDOW_SHOW					CTL_CODE_SHIT(5)
#define SWIRCHDRV_SET_WINDOW_HIDE					CTL_CODE_SHIT(6)

BOOL InstallMiniDriver(const WCHAR* lpszDriverName,const WCHAR* lpszDriverPath,const WCHAR* lpszAltitude);

BOOL StartMiniDriver(const WCHAR* lpszDriverName);

BOOL StopMiniDriver(const WCHAR* lpszDriverName);

BOOL DeleteMiniDriver(const WCHAR* lpszDriverName);

HANDLE OpenDevice(WCHAR* szDeviceName);