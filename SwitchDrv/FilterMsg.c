
#include "DominateCommand.h"
#include "FilterMsg.h"

//user application Conect
NTSTATUS SwitchConnect(__in PFLT_PORT ClientPort,__in PVOID ServerPortCookie,__in_bcount(SizeOfContext) PVOID ConnectionContext,__in ULONG SizeOfContext,__deref_out_opt PVOID *ConnectionCookie)
{
	PAGED_CODE();
	UNREFERENCED_PARAMETER(ServerPortCookie);
	UNREFERENCED_PARAMETER(ConnectionContext);
	UNREFERENCED_PARAMETER(SizeOfContext);
	UNREFERENCED_PARAMETER(ConnectionCookie);

	FLT_ASSERT(g_pDominateCmd->gClientPort == NULL);

	if (g_pDominateCmd->gClientPort == NULL)
	{
		g_pDominateCmd->gClientPort = ClientPort;
	}

	if (g_pDominateCmd->UserProcess == NULL)
	{
		g_pDominateCmd->UserProcess = IoGetCurrentProcess();
	}
	return STATUS_SUCCESS;
}

//user application Disconect
VOID SwitchDisconnect(__in_opt PVOID ConnectionCookie)
{		
	PAGED_CODE();
	UNREFERENCED_PARAMETER(ConnectionCookie);
	FltCloseClientPort(g_pDominateCmd->gFilterHandle,&g_pDominateCmd->gClientPort);
	g_pDominateCmd->gClientPort = NULL;
}

NTSTATUS SwitchMessage(__in PVOID ConnectionCookie,
					   __in_bcount_opt(InputBufferSize) PVOID InputBuffer,
					   __in ULONG InputBufferSize,
					   __out_bcount_part_opt(OutputBufferSize,*ReturnOutputBufferLength) PVOID OutputBuffer,
					   __in ULONG OutputBufferSize,
					   __out PULONG ReturnOutputBufferLength)
{		
	PAGED_CODE();

	UNREFERENCED_PARAMETER(ConnectionCookie);
	UNREFERENCED_PARAMETER(OutputBufferSize);
	UNREFERENCED_PARAMETER(InputBuffer);
	UNREFERENCED_PARAMETER(InputBufferSize);
	UNREFERENCED_PARAMETER(ReturnOutputBufferLength);
	UNREFERENCED_PARAMETER(OutputBuffer);
	_try{
		//memcpy(OutputBuffer,strPath,wcslen(strPath));
	}
	_except(EXCEPTION_EXECUTE_HANDLER)
	{
		KdPrint(("catch error\r\n"));
	}

	//KeClearEvent(pEvent);
	return STATUS_SUCCESS;
}
