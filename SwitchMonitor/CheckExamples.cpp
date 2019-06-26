#include "stdafx.h"
#include "CheckExamples.h"
#include <Sddl.h>


HANDLE   g_hTryMonitor = NULL;
HANDLE   g_hBoundary = NULL;
HANDLE   g_hNamespace = NULL;
// Keep track whether or not the namespace was created or open for clean-up
BOOL     g_bNamespaceOpened = FALSE;
// Names of boundary and private namespace
PCTSTR   g_szBoundary = TEXT("TryMonitorBoundary");
PCTSTR   g_szNamespace = TEXT("TryMonitorNamespace");

void ReleaseExamples()
{
	if (g_hTryMonitor != NULL)
	{
		CloseHandle(g_hTryMonitor);
	}
	if (g_hNamespace != NULL)
	{
		if (g_bNamespaceOpened)
		{
			ClosePrivateNamespace(g_hNamespace,0);
		}
		else
		{
			ClosePrivateNamespace(g_hNamespace,PRIVATE_NAMESPACE_FLAG_DESTROY);
		}
	}
	if (g_hBoundary != NULL)
	{
		DeleteBoundaryDescriptor(g_hBoundary);
	}
}
DWORD CheckExamples()
{
	TCHAR szMutexName[64];
	BYTE localAdminSID[SECURITY_MAX_SID_SIZE];
	SECURITY_ATTRIBUTES sa;
	PSID pLocalAdminSID = &localAdminSID;
	DWORD cbSID;
	cbSID = sizeof(localAdminSID);
	g_hBoundary = CreateBoundaryDescriptor(g_szBoundary,0);
	if (!CreateWellKnownSid(WinBuiltinAdministratorsSid,NULL,pLocalAdminSID,&cbSID))
	{
		OutputDebugString(TEXT("CreateWellKnownSid failed.\n"));
		return 0;
	}
	if (!AddSIDToBoundaryDescriptor(&g_hBoundary,pLocalAdminSID))
	{
		OutputDebugString(TEXT("AddSIDToBoundaryDescriptor failed.\n"));
		return 0;
	}
	sa.nLength = sizeof(sa);
	sa.bInheritHandle = FALSE;
	if (!ConvertStringSecurityDescriptorToSecurityDescriptor(L"D:(A;;GA;;;BA)",SDDL_REVISION_1,&sa.lpSecurityDescriptor,NULL))
	{
		OutputDebugString(TEXT("ConvertStringSecurityDescriptorToSecurityDescriptor failed.\n"));
		return 0;
	}
	g_hNamespace = CreatePrivateNamespace(&sa,g_hBoundary,g_szNamespace);
	LocalFree(sa.lpSecurityDescriptor);
	if (g_hNamespace == NULL)
	{
		DWORD dwError = GetLastError();
		if (dwError == ERROR_ACCESS_DENIED)
		{
			OutputDebugString(TEXT("CreatePrivateNamespace failed:ERROR_ACCESS_DENIED.\n"));
			return 0;
		}
		else
		{
			if ((dwError == ERROR_ALREADY_EXISTS))
			{
				g_hNamespace = OpenPrivateNamespace(g_hBoundary,g_szNamespace);
				if (g_szNamespace == NULL)
				{
					OutputDebugString(TEXT("OpenPrivateNamespace failed:ERROR_ALREADY_EXISTS.\n"));
					return 0;
				}
				else
				{
					g_bNamespaceOpened = TRUE;
				}
			}
			else
			{
				OutputDebugString(TEXT("Unexpected error occured.\n"));
				return 0;
			}
		}
	}
	StringCchPrintf(szMutexName,_countof(szMutexName),TEXT("%s\\%s"),g_szNamespace,TEXT("TryMonitor"));
	g_hTryMonitor = CreateMutex(NULL,FALSE,szMutexName);
	if (GetLastError() == ERROR_ALREADY_EXISTS)
	{
		OutputDebugString(TEXT("Mutex is Exists!.\n"));
		return 2;
	}
	else
	{
		OutputDebugString(TEXT("Examples First run!.\n"));
		return 1;
	}
	return 0;
}