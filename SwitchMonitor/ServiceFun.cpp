
#include "stdafx.h"
#include <winsvc.h>
#include "ServiceFun.h"
//--------------------------------------------------------------------------- 
//判断系统中是否存在某个服务
//---------------------------------------------------------------------------
BOOL ChkHasServc(WCHAR* strServiceName)
{
	//打开服务控制管理器
	SC_HANDLE hSC = ::OpenSCManager(NULL,NULL,GENERIC_EXECUTE);
	if(hSC == NULL)
	{
		OutputDebugString(TEXT("open SCManager error\r\n"));
		return FALSE;
	}
	// 打开DC_ClientService服务。
	SC_HANDLE hSvc = ::OpenService(hSC,strServiceName,SERVICE_START | SERVICE_QUERY_STATUS | SERVICE_STOP);
	if(hSvc == NULL)
	{
		OutputDebugString(TEXT("Open eventlog erron.\r\n"));
		::CloseServiceHandle(hSC);
		return FALSE;
	}
	::CloseServiceHandle(hSvc);
	::CloseServiceHandle(hSC);
	return TRUE;
}
//--------------------------------------------------------------------------- 
//判断某服务是否在运行状态
//--------------------------------------------------------------------------- 
BOOL ChkServcRun(WCHAR* strServiceName) 
{ 
	BOOL bRet = FALSE;
	SC_HANDLE scm,svc; 
	SERVICE_STATUS ServiceStatus; 
	scm = OpenSCManager(NULL,NULL,SC_MANAGER_ALL_ACCESS); 
	if(scm!=NULL) 
	{ 
		svc = OpenService(scm, strServiceName,SERVICE_QUERY_STATUS); 
		if(svc!=NULL) 
		{ 
			QueryServiceStatus(svc,&ServiceStatus); 
			if(ServiceStatus.dwCurrentState == SERVICE_RUNNING) 
			{
				bRet = TRUE;
			}
			CloseServiceHandle(svc); 
		} 
		CloseServiceHandle(scm); 
	} 	
	return bRet;
} 
