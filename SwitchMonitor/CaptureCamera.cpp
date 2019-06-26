#include "stdafx.h"

#include "KeyboardHook.h"
#include "CaptureCamera.h"

#include <Vfw.h>
#pragma comment(lib,"vfw32.lib")

CAPDRIVERCAPS m_CapDrvCap;
CAPSTATUS m_CapStatus;
CAPTUREPARMS m_Parms;

void GetCurTime(WCHAR* szCatPath,WCHAR* szOutTime,int SizeOutTime,WCHAR* szSuffix)
{
	BOOL bRet = FALSE;
	SYSTEMTIME SystemTime;
	WCHAR szFormat[MAX_PATH];

	RtlZeroMemory(szFormat,sizeof(WCHAR)*MAX_PATH);
	GetSystemTime(&SystemTime);
	if (szCatPath)
	{
		StringCchPrintf(szFormat,MAX_PATH,TEXT("%s\\%d-%d-%d-%d-%d-%d-%d.%s"), \
			szCatPath, \
			SystemTime.wYear, \
			SystemTime.wMonth, \
			SystemTime.wDay, \
			SystemTime.wHour, \
			SystemTime.wMinute, \
			SystemTime.wSecond, \
			SystemTime.wMilliseconds, \
			szSuffix);
	}
	else
	{
		StringCchPrintf(szFormat,MAX_PATH,TEXT("%d-%d-%d-%d-%d-%d-%d.%s"), \
			SystemTime.wYear, \
			SystemTime.wMonth, \
			SystemTime.wDay, \
			SystemTime.wHour, \
			SystemTime.wMinute, \
			SystemTime.wSecond, \
			SystemTime.wMilliseconds, \
			szSuffix);
	}
	StringCchCopy(szOutTime,SizeOutTime,szFormat);
}
void InitializeCaptureCamera(PMONITOR_STATUS pMonitorStatus)
{
	pMonitorStatus->hCapWnd = capCreateCaptureWindow((LPTSTR)TEXT("视频捕捉测试程序"),NULL,0,0,0,0,NULL,0); // 设置预示窗口
	ASSERT(pMonitorStatus->hCapWnd);
	::ShowWindow(pMonitorStatus->hCapWnd,SW_HIDE);
	if(capDriverConnect(pMonitorStatus->hCapWnd,0))
	{
		//连接第0 号驱动器
		//得到驱动器的性能
		capDriverGetCaps(pMonitorStatus->hCapWnd,sizeof(CAPDRIVERCAPS),&m_CapDrvCap);

		if(m_CapDrvCap.fCaptureInitialized)
		{
			//如果初始化成功
			capGetStatus(pMonitorStatus->hCapWnd, &m_CapStatus,sizeof(m_CapStatus)); // 得到驱动器状态
			capPreviewRate(pMonitorStatus->hCapWnd,30); // 设置预示帧频
			capPreview(pMonitorStatus->hCapWnd,TRUE); // 设置预示方式
			pMonitorStatus->bIsCamera = TRUE;
		}
		else
		{
			//初始化未成功
			pMonitorStatus->bIsCamera = FALSE;
			OutputDebugString(TEXT("视频捕捉卡初始化失败!\n"));
			PostMessage(pMonitorStatus->hCapWnd,WM_CLOSE,0,0);
		}
	}
	else
	{
		//未能连接到驱动器
		OutputDebugString(TEXT("与视频捕捉卡连接失败!\n"));
		pMonitorStatus->bIsCamera = FALSE;
	}
}
void Photo(PMONITOR_STATUS pMonitor,WCHAR* pSzSavePath)
{
	if (pMonitor->hCapWnd)
	{
		capCaptureSingleFrame(pMonitor->hCapWnd);
		capFileSaveDIB(pMonitor->hCapWnd,pSzSavePath);
	}
	return;
}
void CloseCapture(PMONITOR_STATUS pMonitor)
{
	if (pMonitor->hCapWnd)
	{
		capDriverDisconnect(pMonitor->hCapWnd);
		pMonitor->hCapWnd = NULL;
	}
}