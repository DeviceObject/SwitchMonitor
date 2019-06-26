#include "DominateCommand.h"
#include "MonitorProcessNotifyRoutine.h"

PEPROCESS GetProcEProcess(CHAR *szProcName,ULONG ulProcessListEntryOffset)
{
	PEPROCESS pEProcess,pCurEProcess;
	LIST_ENTRY *pListEntry;
	pCurEProcess = pEProcess = NULL;
	pEProcess = PsGetCurrentProcess();
	pListEntry = (LIST_ENTRY*)((ULONG)pEProcess + ulProcessListEntryOffset);
	pCurEProcess = (PEPROCESS)((ULONG)pListEntry->Flink - ulProcessListEntryOffset);
	while(pEProcess != pCurEProcess)
	{
		if(strcmp(szProcName,(char*)((ULONG)pCurEProcess + g_ulProcessNameOffset)) == 0)
		{
			return pCurEProcess;
		}
		pListEntry = (LIST_ENTRY*)((ULONG)pCurEProcess + ulProcessListEntryOffset);
		pCurEProcess = (PEPROCESS)((ULONG)pListEntry->Flink - ulProcessListEntryOffset);
	}
	return 0;
}
int GetProcessNameOffset(char *szProcessName)
{
	PEPROCESS pEProcess;
	ULONG uli;

	pEProcess = PsGetCurrentProcess();

	for(uli = 0;uli < 3*PAGE_SIZE;uli++)
	{
		if(0 == strncmp(NT_SYSTEM_NAME,(PCHAR)((ULONG)pEProcess + uli),strlen(szProcessName)))
		{
			return uli;
		}
	}
	return 0;
}
NTSTATUS GetProcessName(HANDLE hPid,char *szProcessName)
{
	NTSTATUS ntStatus;
	PVOID pObject;

	ntStatus = STATUS_SUCCESS;

	ntStatus = PsLookupProcessByProcessId(hPid,&(PEPROCESS)pObject);
	if(!NT_SUCCESS(ntStatus))
	{
		return ntStatus;
	}
	strncpy(szProcessName,(char*)((ULONG)pObject + g_ulProcessNameOffset),16);
	if (strncmp(szProcessName,EXPLORER_NAME,strlen(EXPLORER_NAME)) == 0)
	{
		g_pDominateCmd->ExplorerProcess = (PEPROCESS)pObject;
		DbgPrint("Set explorer.exe process success. process name: %s process : %p\r\n",szProcessName,g_pDominateCmd->ExplorerProcess);
	}
	ObDereferenceObject(pObject);
	return ntStatus;
}
VOID ProcessCreateMon(IN HANDLE hParentId,IN HANDLE PId,IN BOOLEAN bCreate)
{
	NTSTATUS ntStatus;
	char szProcessName[100];

	UNREFERENCED_PARAMETER(hParentId);

	ntStatus = STATUS_SUCCESS;
	RtlZeroMemory(szProcessName,100);

	if (bCreate)
	{
		ntStatus = GetProcessName(PId,szProcessName);
		if (NT_SUCCESS(ntStatus))
		{
			DbgPrint("process name: %s\r\n",szProcessName);
		}
	}
}
NTSTATUS RegisterCreateProcessMonitor(BOOLEAN bCreated)
{
	NTSTATUS ntStatus;

	ntStatus = STATUS_SUCCESS;

	if (bCreated)
	{
		ntStatus = PsSetCreateProcessNotifyRoutine(ProcessCreateMon,FALSE);
		if (!NT_SUCCESS(ntStatus))
		{
			DbgPrint("PsSetCreateProcessNotifyRoutine() FALSE\n");
		}
	}
	else
	{
		ntStatus = PsSetCreateProcessNotifyRoutine(ProcessCreateMon,TRUE);
		if (!NT_SUCCESS(ntStatus))
		{
			DbgPrint("PsSetCreateProcessNotifyRoutine() TRUE\n");
		}
	}
	return ntStatus;
}