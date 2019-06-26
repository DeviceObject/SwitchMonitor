
#include "stdafx.h"
#include "MiniDrvLoader.h"
//安装MiniFilter驱动
BOOL InstallMiniDriver(const WCHAR* lpszDriverName,const WCHAR* lpszDriverPath,const WCHAR* lpszAltitude)
{
	WCHAR    szTempStr[MAX_PATH * 2];
	HKEY    hKey;
	DWORD    dwData;
	WCHAR    szDriverImagePath[MAX_PATH];    

	if( NULL == lpszDriverName || NULL == lpszDriverPath )
	{
		return FALSE;
	}
	//得到完整的驱动路径
	GetFullPathName(lpszDriverPath,MAX_PATH,szDriverImagePath,NULL);

	SC_HANDLE hServiceMgr = NULL;// SCM管理器的句柄
	SC_HANDLE hService = NULL;// NT驱动程序的服务句柄

	//打开服务控制管理器
	hServiceMgr = OpenSCManager( NULL, NULL, SC_MANAGER_ALL_ACCESS );
	if( hServiceMgr == NULL ) 
	{
		// OpenSCManager失败
		CloseServiceHandle(hServiceMgr);
		return FALSE;        
	}

	// OpenSCManager成功  

	//创建驱动所对应的服务
	hService = CreateService( hServiceMgr,
		lpszDriverName,             // 驱动程序的在注册表中的名字
		lpszDriverName,             // 注册表驱动程序的DisplayName 值
		SERVICE_ALL_ACCESS,         // 加载驱动程序的访问权限
		SERVICE_FILE_SYSTEM_DRIVER, // 表示加载的服务是文件系统驱动程序
		SERVICE_DEMAND_START,       // 注册表驱动程序的Start 值
		SERVICE_ERROR_IGNORE,       // 注册表驱动程序的ErrorControl 值
		szDriverImagePath,          // 注册表驱动程序的ImagePath 值
		TEXT("FSFilter Activity Monitor"),// 注册表驱动程序的Group 值
		NULL, 
		TEXT("FltMgr"),                   // 注册表驱动程序的DependOnService 值
		NULL, 
		NULL);

	if( hService == NULL ) 
	{        
		if( GetLastError() == ERROR_SERVICE_EXISTS ) 
		{
			//服务创建失败，是由于服务已经创立过
			CloseServiceHandle(hService);       // 服务句柄
			CloseServiceHandle(hServiceMgr);    // SCM句柄
			return TRUE; 
		}
		else 
		{
			CloseServiceHandle(hService);       // 服务句柄
			CloseServiceHandle(hServiceMgr);    // SCM句柄
			return FALSE;
		}
	}
	CloseServiceHandle(hService);       // 服务句柄
	CloseServiceHandle(hServiceMgr);    // SCM句柄

	//-------------------------------------------------------------------------------------------------------
	// SYSTEM\\CurrentControlSet\\Services\\DriverName\\Instances子健下的键值项 
	//-------------------------------------------------------------------------------------------------------
	StringCchCopy(szTempStr,MAX_PATH * 2,TEXT("SYSTEM\\CurrentControlSet\\Services\\"));
	StringCchCat(szTempStr,MAX_PATH * 2,lpszDriverName);
	StringCchCat(szTempStr,MAX_PATH * 2,TEXT("\\Instances"));
	if(RegCreateKeyEx(HKEY_LOCAL_MACHINE,szTempStr,0,TEXT(""),REG_OPTION_NON_VOLATILE,KEY_ALL_ACCESS,NULL,&hKey,(LPDWORD)&dwData)!=ERROR_SUCCESS)
	{
		return FALSE;
	}
	// 注册表驱动程序的DefaultInstance 值 

	StringCchCopy(szTempStr,MAX_PATH * 2,lpszDriverName);
	StringCchCat(szTempStr,MAX_PATH * 2,TEXT(" Instance"));
	if(RegSetValueEx(hKey,TEXT("DefaultInstance"),0,REG_SZ,(CONST BYTE*)szTempStr,(DWORD)wcslen(szTempStr) * sizeof(WCHAR))!=ERROR_SUCCESS)
	{
		return FALSE;
	}
	RegFlushKey(hKey);//刷新注册表
	RegCloseKey(hKey);
	//-------------------------------------------------------------------------------------------------------

	//-------------------------------------------------------------------------------------------------------
	// SYSTEM\\CurrentControlSet\\Services\\DriverName\\Instances\\DriverName Instance子健下的键值项 
	//-------------------------------------------------------------------------------------------------------
	StringCchCopy(szTempStr,MAX_PATH * 2,TEXT("SYSTEM\\CurrentControlSet\\Services\\"));
	StringCchCat(szTempStr,MAX_PATH * 2,lpszDriverName);
	StringCchCat(szTempStr,MAX_PATH * 2,TEXT("\\Instances\\"));
	StringCchCat(szTempStr,MAX_PATH * 2,lpszDriverName);
	StringCchCat(szTempStr,MAX_PATH * 2,TEXT(" Instance"));

	if(RegCreateKeyEx(HKEY_LOCAL_MACHINE,szTempStr,0,TEXT(""),REG_OPTION_NON_VOLATILE,KEY_ALL_ACCESS,NULL,&hKey,(LPDWORD)&dwData)!=ERROR_SUCCESS)
	{
		return FALSE;
	}
	// 注册表驱动程序的Altitude 值
	StringCchCopy(szTempStr,MAX_PATH * 2,lpszAltitude);
	
	if(RegSetValueEx(hKey,TEXT("Altitude"),0,REG_SZ,(CONST BYTE*)szTempStr,(DWORD)wcslen(szTempStr) * sizeof(WCHAR))!=ERROR_SUCCESS)
	{
		return FALSE;
	}
	// 注册表驱动程序的Flags 值
	dwData=0x0;
	if(RegSetValueEx(hKey,TEXT("Flags"),0,REG_DWORD,(CONST BYTE*)&dwData,sizeof(DWORD))!=ERROR_SUCCESS)
	{
		return FALSE;
	}
	RegFlushKey(hKey);//刷新注册表
	RegCloseKey(hKey);
	//-------------------------------------------------------------------------------------------------------

	return TRUE;
}

//启动Mini驱动
BOOL StartMiniDriver(const WCHAR* lpszDriverName)
{
	SC_HANDLE        schManager;
	SC_HANDLE        schService;

	if(NULL==lpszDriverName)
	{
		return FALSE;
	}

	schManager=OpenSCManager(NULL,NULL,SC_MANAGER_ALL_ACCESS);
	if(NULL==schManager)
	{
		CloseServiceHandle(schManager);
		return FALSE;
	}
	schService=OpenService(schManager,lpszDriverName,SERVICE_ALL_ACCESS);
	if(NULL==schService)
	{
		CloseServiceHandle(schService);
		CloseServiceHandle(schManager);
		return FALSE;
	}

	if(!StartService(schService,0,NULL))
	{
		CloseServiceHandle(schService);
		CloseServiceHandle(schManager);
		if( GetLastError() == ERROR_SERVICE_ALREADY_RUNNING ) 
		{             
			// 服务已经开启
			return TRUE;
		} 
		return FALSE;
	}

	CloseServiceHandle(schService);
	CloseServiceHandle(schManager);

	return TRUE;
}

//停止Mini驱动
BOOL StopMiniDriver(const WCHAR* lpszDriverName)
{
	SC_HANDLE        schManager;
	SC_HANDLE        schService;
	SERVICE_STATUS    svcStatus;

	schManager = OpenSCManager(NULL,NULL,SC_MANAGER_ALL_ACCESS);
	if(NULL == schManager)
	{
		return FALSE;
	}
	schService = OpenService(schManager,lpszDriverName,SERVICE_ALL_ACCESS);
	if(NULL == schService)
	{
		CloseServiceHandle(schManager);
		return FALSE;
	}    
	if(!ControlService(schService,SERVICE_CONTROL_STOP,&svcStatus) && (svcStatus.dwCurrentState!=SERVICE_STOPPED))
	{
		CloseServiceHandle(schService);
		CloseServiceHandle(schManager);
		return FALSE;
	}

	CloseServiceHandle(schService);
	CloseServiceHandle(schManager);

	return TRUE;
}

//卸载Mini驱动
BOOL DeleteMiniDriver(const WCHAR* lpszDriverName)
{
	SC_HANDLE        schManager;
	SC_HANDLE        schService;
	SERVICE_STATUS    svcStatus;

	schManager=OpenSCManager(NULL,NULL,SC_MANAGER_ALL_ACCESS);
	if(NULL==schManager)
	{
		return FALSE;
	}
	schService=OpenService(schManager,lpszDriverName,SERVICE_ALL_ACCESS);
	if(NULL==schService)
	{
		CloseServiceHandle(schManager);
		return FALSE;
	}
	ControlService(schService,SERVICE_CONTROL_STOP,&svcStatus);
	if(!DeleteService(schService))
	{
		CloseServiceHandle(schService);
		CloseServiceHandle(schManager);
		return FALSE;
	}
	CloseServiceHandle(schService);
	CloseServiceHandle(schManager);

	return TRUE;
}
HANDLE OpenDevice(WCHAR* szDeviceName)
{
	//测试驱动程序  
	HANDLE hDevice = CreateFile(szDeviceName,  
		GENERIC_WRITE | GENERIC_READ,  
		0,  
		NULL,  
		OPEN_EXISTING,  
		0,  
		NULL);  
	if( hDevice != INVALID_HANDLE_VALUE )  
	{
		OutputDebugString(TEXT("Create Device ok ! \n"));  
	}
	else  
	{
		OutputDebugString(TEXT("Create Device faild! \n")); 
		return NULL;
	}

	return hDevice;
}