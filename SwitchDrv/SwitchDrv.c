
#include <strsafe.h>
#include <dontuse.h>
#include <suppress.h>

#include "DominateCommand.h"
#include "Irp_Mj_Read.h"
#include "Irp_Mj_Create.h"
#include "IoControlDispatch.h"
#include "FilterMsg.h"
#include "IrpMjSetInformation.h"
#include "MonitorProcessNotifyRoutine.h"
#include "OsVersion.h"
#include "SwitchDrv.h"

PDOMINATECMD g_pDominateCmd = NULL;
ULONG g_ulProcessNameOffset = 0;
ULONG g_ulProcessListEntryOffset = 0;
/*************************************************************************
MiniFilter initialization and unload routines.
*************************************************************************/

NTSTATUS DriverEntry(_In_ PDRIVER_OBJECT DriverObject,_In_ PUNICODE_STRING RegistryPath)
{
	NTSTATUS status;
	PSECURITY_DESCRIPTOR pSecurityDescriptor;
	UNICODE_STRING unStrPort;
	OBJECT_ATTRIBUTES ObjectAttributes;
	KIRQL OldIrql;

	pSecurityDescriptor = NULL;

	//UNREFERENCED_PARAMETER(RegistryPath);
	DbgPrint("SwitchDrv!DriverEntry: Entered\n");

	g_pDominateCmd = (PDOMINATECMD)ExAllocatePool(NonPagedPool,sizeof(DOMINATECMD));
	if (g_pDominateCmd == NULL)
	{
		DbgPrint("ExAllocatePool()-->g_pDominateCmd failed\n");
		return STATUS_UNSUCCESSFUL;
	}
	RtlZeroMemory(g_pDominateCmd,sizeof(DOMINATECMD));

	//ExInitializeNPagedLookasideList(g_Lookaside,NULL,NULL,0,sizeof(COMMAND_MESSAGE),L'W',0);
	OldIrql = KeGetCurrentIrql();
	if (OldIrql == PASSIVE_LEVEL)
	{
		DbgPrint("PASSIVE_LEVEL\r\n");
	}else if (OldIrql == DISPATCH_LEVEL)
	{
		DbgPrint("DISPATCH_LEVEL\r\n");
	}else
	{
		DbgPrint("UNKNOW_LEVEL\r\n");
	}
	WinVersion = GetKernelOsVersion();
	if (WinVersion == WINDOWS_VERSION_XP)
	{
		g_ulProcessListEntryOffset = 0x88;
	}
	else if (WinVersion == WINDOWS_VERSION_VISTA_2008)
	{
	}
	else if (WinVersion == WINDOWS_VERSION_7)
	{
		g_ulProcessListEntryOffset = 0xb8;
	}
	else
	{

	}
	g_ulProcessNameOffset = GetProcessNameOffset(NT_SYSTEM_NAME);
	if (g_ulProcessNameOffset)
	{
		DbgPrint("Process name offset\'s : %d\r\n",g_ulProcessNameOffset);
	}
	InitializeDriver(DriverObject,RegistryPath);

	status = FltRegisterFilter(DriverObject,&FilterRegistration,&g_pDominateCmd->gFilterHandle);
	FLT_ASSERT(NT_SUCCESS(status));
	if (NT_SUCCESS(status))
	{
		status = FltStartFiltering(g_pDominateCmd->gFilterHandle);
		if (!NT_SUCCESS(status))
		{
			DbgPrint("SwitchDrv!DriverEntry: FltStartFiltering failed,Status:%08x\n",status);
			FltUnregisterFilter(g_pDominateCmd->gFilterHandle);
			goto Final;
		}
	}
	// Create Security descriptor
	status = FltBuildDefaultSecurityDescriptor(&pSecurityDescriptor,FLT_PORT_ALL_ACCESS);
	if (!NT_SUCCESS(status))
	{
		DbgPrint("SwitchDrv!DriverEntry: FltBuildDefaultSecurityDescriptor failed,Status:%08x\n",status);
		goto Final;
	}
	RtlInitUnicodeString(&unStrPort,SWITCHDRV_PORT_NAME);
	InitializeObjectAttributes(&ObjectAttributes,&unStrPort,OBJ_KERNEL_HANDLE | OBJ_CASE_INSENSITIVE,NULL,pSecurityDescriptor);
	status = FltCreateCommunicationPort(g_pDominateCmd->gFilterHandle,&g_pDominateCmd->gServerPort,&ObjectAttributes,NULL,SwitchConnect,SwitchDisconnect,SwitchMessage,1);
	if (!NT_SUCCESS(status))
	{
		DbgPrint("SwitchDrv!DriverEntry: FltCreateCommunicationPort failed,Status:%08x\n",status);
		goto Final;
	}
	if (NULL != pSecurityDescriptor)
	{
		FltFreeSecurityDescriptor(pSecurityDescriptor);
		pSecurityDescriptor = NULL;
	}

	status = RegisterCreateProcessMonitor(TRUE);
	if (NT_SUCCESS(status))
	{
		DbgPrint("RegisterCreateProcessMonitor success\r\n");
	}

Final:

	if (!NT_SUCCESS(status))
	{
		if (NULL != g_pDominateCmd->gFilterHandle)
		{
			FltUnregisterFilter(g_pDominateCmd->gFilterHandle);
			g_pDominateCmd->gFilterHandle = NULL;
		}

		if (NULL != g_pDominateCmd->gServerPort)
		{
			FltCloseCommunicationPort(g_pDominateCmd->gServerPort);
		}
	}
	DbgPrint("SwitchDrv!DriverEntry: exit\n");
	return status;
}

NTSTATUS SwitchDrvUnload(_In_ FLT_FILTER_UNLOAD_FLAGS Flags)
{
	UNREFERENCED_PARAMETER(Flags);
	PAGED_CODE();
	DbgPrint("SwitchDrv!SwitchDrvUnload: Entered\n");
	FltUnregisterFilter(g_pDominateCmd->gFilterHandle);
	FltCloseCommunicationPort(g_pDominateCmd->gServerPort);
	return STATUS_SUCCESS;
}


/*************************************************************************
MiniFilter callback routines.
*************************************************************************/
FLT_PREOP_CALLBACK_STATUS SwitchDrvPre(
	_Inout_ PFLT_CALLBACK_DATA Data,
	_In_ PCFLT_RELATED_OBJECTS FltObjects,
	_Flt_CompletionContext_Outptr_ PVOID *CompletionContext)
{
	NTSTATUS status;

	UNREFERENCED_PARAMETER(FltObjects);
	UNREFERENCED_PARAMETER(CompletionContext);

	DbgPrint("SwitchDrv!SwitchDrvPreOperation: Entered\n");

	if (SwitchDrvDoRequestOperationStatus(Data))
	{
		status = FltRequestOperationStatusCallback(Data,SwitchDrvOperationStatusCallback,NULL);
		if (!NT_SUCCESS(status))
		{
			DbgPrint("SwitchDrv!SwitchDrvPreOperation: FltRequestOperationStatusCallback Failed, status=%08x\n",status);
		}
	}
	return FLT_PREOP_SUCCESS_WITH_CALLBACK;
}

VOID SwitchDrvOperationStatusCallback(
	_In_ PCFLT_RELATED_OBJECTS FltObjects,
	_In_ PFLT_IO_PARAMETER_BLOCK ParameterSnapshot,
	_In_ NTSTATUS OperationStatus,
	_In_ PVOID RequesterContext)
{
	UNREFERENCED_PARAMETER(FltObjects);

	DbgPrint("SwitchDrv!SwitchDrvOperationStatusCallback: Entered\n");

	DbgPrint("SwitchDrv!SwitchDrvOperationStatusCallback: Status=%08x ctx=%p IrpMj=%02x.%02x \"%s\"\n",
		OperationStatus,
		RequesterContext,
		ParameterSnapshot->MajorFunction,
		ParameterSnapshot->MinorFunction,
		FltGetIrpName(ParameterSnapshot->MajorFunction));
}


FLT_POSTOP_CALLBACK_STATUS SwitchDrvPost(
	_Inout_ PFLT_CALLBACK_DATA Data,
	_In_ PCFLT_RELATED_OBJECTS FltObjects,
	_In_opt_ PVOID CompletionContext,
	_In_ FLT_POST_OPERATION_FLAGS Flags)
{
	UNREFERENCED_PARAMETER(Data);
	UNREFERENCED_PARAMETER(FltObjects);
	UNREFERENCED_PARAMETER(CompletionContext);
	UNREFERENCED_PARAMETER(Flags);
	DbgPrint("SwitchDrv!SwitchDrvPostOperation: Entered\n");
	return FLT_POSTOP_FINISHED_PROCESSING;
}


FLT_PREOP_CALLBACK_STATUS SwitchDrvPreOperationNoPostOperation(
	_Inout_ PFLT_CALLBACK_DATA Data,
	_In_ PCFLT_RELATED_OBJECTS FltObjects,
	_Flt_CompletionContext_Outptr_ PVOID *CompletionContext)
{
	UNREFERENCED_PARAMETER( Data );
	UNREFERENCED_PARAMETER( FltObjects );
	UNREFERENCED_PARAMETER( CompletionContext );
	DbgPrint("SwitchDrv!SwitchDrvPreOperationNoPostOperation: Entered\n");
	return FLT_PREOP_SUCCESS_NO_CALLBACK;
}


BOOLEAN SwitchDrvDoRequestOperationStatus(_In_ PFLT_CALLBACK_DATA Data)
{
	PFLT_IO_PARAMETER_BLOCK iopb = Data->Iopb;
	return (BOOLEAN)

		//
		//  Check for oplock operations
		//

		(((iopb->MajorFunction == IRP_MJ_FILE_SYSTEM_CONTROL) &&
		((iopb->Parameters.FileSystemControl.Common.FsControlCode == FSCTL_REQUEST_FILTER_OPLOCK)  ||
		(iopb->Parameters.FileSystemControl.Common.FsControlCode == FSCTL_REQUEST_BATCH_OPLOCK)   ||
		(iopb->Parameters.FileSystemControl.Common.FsControlCode == FSCTL_REQUEST_OPLOCK_LEVEL_1) ||
		(iopb->Parameters.FileSystemControl.Common.FsControlCode == FSCTL_REQUEST_OPLOCK_LEVEL_2)))

		||

		//
		//    Check for directy change notification
		//

		((iopb->MajorFunction == IRP_MJ_DIRECTORY_CONTROL) &&
		(iopb->MinorFunction == IRP_MN_NOTIFY_CHANGE_DIRECTORY))
		);
}
NTSTATUS
	SwitchDrvInstanceSetup (
	_In_ PCFLT_RELATED_OBJECTS FltObjects,
	_In_ FLT_INSTANCE_SETUP_FLAGS Flags,
	_In_ DEVICE_TYPE VolumeDeviceType,
	_In_ FLT_FILESYSTEM_TYPE VolumeFilesystemType
	)
	/*++

	Routine Description:

	This routine is called whenever a new instance is created on a volume. This
	gives us a chance to decide if we need to attach to this volume or not.

	If this routine is not defined in the registration structure, automatic
	instances are always created.

	Arguments:

	FltObjects - Pointer to the FLT_RELATED_OBJECTS data structure containing
	opaque handles to this filter, instance and its associated volume.

	Flags - Flags describing the reason for this attach request.

	Return Value:

	STATUS_SUCCESS - attach
	STATUS_FLT_DO_NOT_ATTACH - do not attach

	--*/
{
	UNREFERENCED_PARAMETER( FltObjects );
	UNREFERENCED_PARAMETER( Flags );
	UNREFERENCED_PARAMETER( VolumeDeviceType );
	UNREFERENCED_PARAMETER( VolumeFilesystemType );

	PAGED_CODE();

	DbgPrint("SwitchDrv!SwitchDrvInstanceSetup: Entered\n") ;

	return STATUS_SUCCESS;
}


NTSTATUS
	SwitchDrvInstanceQueryTeardown (
	_In_ PCFLT_RELATED_OBJECTS FltObjects,
	_In_ FLT_INSTANCE_QUERY_TEARDOWN_FLAGS Flags
	)
	/*++

	Routine Description:

	This is called when an instance is being manually deleted by a
	call to FltDetachVolume or FilterDetach thereby giving us a
	chance to fail that detach request.

	If this routine is not defined in the registration structure, explicit
	detach requests via FltDetachVolume or FilterDetach will always be
	failed.

	Arguments:

	FltObjects - Pointer to the FLT_RELATED_OBJECTS data structure containing
	opaque handles to this filter, instance and its associated volume.

	Flags - Indicating where this detach request came from.

	Return Value:

	Returns the status of this operation.

	--*/
{
	UNREFERENCED_PARAMETER( FltObjects );
	UNREFERENCED_PARAMETER( Flags );

	PAGED_CODE();

	DbgPrint("SwitchDrv!SwitchDrvInstanceQueryTeardown: Entered\n");

	return STATUS_SUCCESS;
}


VOID
	SwitchDrvInstanceTeardownStart (
	_In_ PCFLT_RELATED_OBJECTS FltObjects,
	_In_ FLT_INSTANCE_TEARDOWN_FLAGS Flags
	)
	/*++

	Routine Description:

	This routine is called at the start of instance teardown.

	Arguments:

	FltObjects - Pointer to the FLT_RELATED_OBJECTS data structure containing
	opaque handles to this filter, instance and its associated volume.

	Flags - Reason why this instance is being deleted.

	Return Value:

	None.

	--*/
{
	UNREFERENCED_PARAMETER( FltObjects );
	UNREFERENCED_PARAMETER( Flags );

	PAGED_CODE();

	DbgPrint("SwitchDrv!SwitchDrvInstanceTeardownStart: Entered\n");
}


VOID
	SwitchDrvInstanceTeardownComplete (
	_In_ PCFLT_RELATED_OBJECTS FltObjects,
	_In_ FLT_INSTANCE_TEARDOWN_FLAGS Flags
	)
	/*++

	Routine Description:

	This routine is called at the end of instance teardown.

	Arguments:

	FltObjects - Pointer to the FLT_RELATED_OBJECTS data structure containing
	opaque handles to this filter, instance and its associated volume.

	Flags - Reason why this instance is being deleted.

	Return Value:

	None.

	--*/
{
	UNREFERENCED_PARAMETER( FltObjects );
	UNREFERENCED_PARAMETER( Flags );

	PAGED_CODE();

	DbgPrint("SwitchDrv!SwitchDrvInstanceTeardownComplete: Entered\n");
}

