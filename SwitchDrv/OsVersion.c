#include "DominateCommand.h"
#include "OsVersion.h"

WINDOWS_VERSION GetKernelOsVersion()
{
	NTSTATUS ntStatus;
	UNICODE_STRING ustrFuncName; 
	RTL_OSVERSIONINFOEXW OsVersionInfo;
	PFN_RtlGetVersion pfnRtlGetVersion;

	ntStatus = STATUS_SUCCESS;
	OsVersionInfo.dwOSVersionInfoSize = sizeof(OsVersionInfo);
	pfnRtlGetVersion = NULL;

	if (WinVersion)
		return WinVersion;
	RtlInitUnicodeString(&ustrFuncName,L"RtlGetVersion"); 
#pragma warning(push) 
#pragma warning(disable:4055) // type cast from data pointer to function pointer 
	pfnRtlGetVersion = (PFN_RtlGetVersion)MmGetSystemRoutineAddress(&ustrFuncName); 
#pragma warning(pop)
	if (pfnRtlGetVersion)
	{ 
		pfnRtlGetVersion((PRTL_OSVERSIONINFOW)&OsVersionInfo); 
	} 
	else 
	{
		PsGetVersion(&OsVersionInfo.dwMajorVersion,&OsVersionInfo.dwMinorVersion,&OsVersionInfo.dwBuildNumber,NULL);
	}
	if (NT_SUCCESS(ntStatus))
	{

		if (OsVersionInfo.dwMajorVersion == 6)
		{
			if (OsVersionInfo.dwMinorVersion == 0)
			{
				DbgPrint("GetCurOsVersion : Windows Vista or Server 2008\r\n");
				WinVersion = WINDOWS_VERSION_VISTA_2008;
			}
			else if (OsVersionInfo.dwMinorVersion == 1)
			{
				DbgPrint("GetCurOsVersion : Windows 7 or 2008 R2\r\n");
				WinVersion = WINDOWS_VERSION_7;
			}
			else if (OsVersionInfo.dwMinorVersion == 2)
			{
				DbgPrint("GetCurOsVersion : Windows 8 or 2012 R2\r\n");
				//WinVersion = 
			}
			else if (OsVersionInfo.dwMinorVersion == 3)
			{
				DbgPrint("GetCurOsVersion : Windows 8 or 2012 R2\r\n");
				//WinVersion = 
			}
			else
			{
				DbgPrint("GetCurOsVersion : Unknow Windows Version\r\n");
				//WinVersion = 
			}
		}
		else if (OsVersionInfo.dwMajorVersion == 5)
		{
			if (OsVersionInfo.dwMinorVersion == 0)
			{
				DbgPrint("GetCurOsVersion : Windows 2000\r\n");
				WinVersion = WINDOWS_VERSION_2K;
			}
			else if (OsVersionInfo.dwMinorVersion == 1)
			{
				DbgPrint("GetCurOsVersion : Windows XP\r\n");
				WinVersion = WINDOWS_VERSION_XP;
			}
			else if (OsVersionInfo.dwMinorVersion == 2)
			{
				if (OsVersionInfo.wServicePackMajor == 0)
				{
					DbgPrint("GetCurOsVersion : Windows Server 2003\r\n");
					WinVersion = WINDOWS_VERSION_2K3;
				}
				else
				{
					DbgPrint("GetCurOsVersion : Windows Server 2003 Sp1 Sp2\r\n");
					WinVersion = WINDOWS_VERSION_2K3_SP1_SP2;
				}
			}
			else
			{
				DbgPrint("GetCurOsVersion : Unknow Windows Version\r\n");
				//WinVersion = ;
			}
		}
		else
		{
			DbgPrint("GetCurOsVersion : Unknow Windows Version\r\n");
			//WinVersion = 
		}
	}
	return WinVersion;
}