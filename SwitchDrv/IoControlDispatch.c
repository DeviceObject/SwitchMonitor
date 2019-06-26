#include "DominateCommand.h"
#include "MonitorProcessNotifyRoutine.h"
#include "IoControlDispatch.h"

NTSTATUS CloseMajorFunction(PDEVICE_OBJECT pDeviceObject,PIRP pIrp)
{
	NTSTATUS ntStatus;
	BOOLEAN bRet;
	ULONG ulCurrentPid;

	UNREFERENCED_PARAMETER(pDeviceObject);

	ulCurrentPid = 0;
	ntStatus = STATUS_SUCCESS;
	bRet = FALSE;

	//if(pDeviceObject != NULL && pIrp != NULL)
	//{
	//	bRet = CheckClientFileName(DRIVER_REGISTRY_PATH);
	//	if(bRet)
	//	{
	//		g_ulMyClientProcessId = 0;
	//	}
	//	else
	//	{
	//		ulCurrentPid = PsGetCurrentProcessId();
	//		bRet = CheckIsMyClientFile(ulCurrentPid);
	//		if(bRet)
	//		{
	//			g_ulMyClientProcessId = 0;
	//			ntStatus = STATUS_SUCCESS;
	//		}
	//		else
	//		{
	//			ntStatus = STATUS_UNSUCCESSFUL;
	//		}
	//	}
	//}
	pIrp->IoStatus.Status = ntStatus;
	pIrp->IoStatus.Information = 0;
	IoCompleteRequest(pIrp, IO_NO_INCREMENT);
	return ntStatus;
}
NTSTATUS CreateMajorFunction(PDEVICE_OBJECT pDeviceObject,PIRP pIrp)
{
	NTSTATUS ntStatus;
	BOOLEAN bRet;
	ULONG ulCurrentPid;

	UNREFERENCED_PARAMETER(pDeviceObject);

	ulCurrentPid = 0;
	bRet = FALSE;
	ntStatus = STATUS_SUCCESS;

	//if(pDeviceObject != NULL && pIrp != NULL)
	//{
	//	bRet = CheckClientFileName(DRIVER_REGISTRY_PATH);
	//	if(bRet)
	//	{
	//		g_ulMyClientProcessId = PsGetCurrentProcessId();
	//	}
	//	else
	//	{
	//		ulCurrentPid = PsGetCurrentProcessId();
	//		bRet = CheckIsMyClientFile(ulCurrentPid);
	//		if(bRet)
	//		{
	//			g_ulMyClientProcessId = ulCurrentPid;
	//			ntStatus = STATUS_SUCCESS;
	//		}
	//		else
	//		{
	//			ntStatus = STATUS_UNSUCCESSFUL;
	//		}
	//	}
	//}
	pIrp->IoStatus.Status = ntStatus;
	pIrp->IoStatus.Information = 0;

	IoCompleteRequest(pIrp, IO_NO_INCREMENT);
	return ntStatus;
}
NTSTATUS IoControlDispatch(PDEVICE_OBJECT pDeviceObject, PIRP lpIrp)
{
	NTSTATUS Status;
	PIO_STACK_LOCATION lpIrpStack;
	ULONG ulInformation;
	PR3_DOMINATECMD pR3DominateCmd;
	POBJECT_HANDLE_INFORMATION HandleInformation;

	UNREFERENCED_PARAMETER(pDeviceObject);

	HandleInformation = NULL;
	ulInformation = 0;
	pR3DominateCmd = NULL;
	lpIrpStack = IoGetCurrentIrpStackLocation(lpIrp);
	Status = STATUS_SUCCESS;

	switch (lpIrpStack->Parameters.DeviceIoControl.IoControlCode)
	{
	case SWITCHDRV_INITIAL_GLOBAL:
		//if (lpIrpStack->Parameters.DeviceIoControl.InputBufferLength < sizeof(R3_DOMINATECMD))
		//{
		//	Status = STATUS_INVALID_PARAMETER;
		//	ulInformation = 0;
		//	break;
		//}
		//pR3DominateCmd = (PR3_DOMINATECMD)lpIrp->AssociatedIrp.SystemBuffer;

		//g_pDominateCmd->hIrp_Mj_Create_Event = pR3DominateCmd->hIrp_Mj_Create_Event;
		//Status = ObReferenceObjectByHandle(g_pDominateCmd->hIrp_Mj_Create_Event,SYNCHRONIZE,*ExEventObjectType,UserMode,&g_pDominateCmd->pMjCreateEventObject,HandleInformation);
		//if (!NT_SUCCESS(Status))
		//{
		//	ulInformation = 0;
		//	break;
		//}

		//g_pDominateCmd->hIrp_Mj_Read_Event = pR3DominateCmd->hIrp_Mj_Read_Event;
		//Status = ObReferenceObjectByHandle(g_pDominateCmd->hIrp_Mj_Read_Event,SYNCHRONIZE,*ExEventObjectType,UserMode,&g_pDominateCmd->pMjReadEventObject,HandleInformation);
		//if (!NT_SUCCESS(Status))
		//{
		//	ulInformation = 0;
		//	break;
		//}
		//
		//g_pDominateCmd->hIrp_Mj_Write_Event = pR3DominateCmd->hIrp_Mj_Write_Event;
		//Status = ObReferenceObjectByHandle(g_pDominateCmd->hIrp_Mj_Write_Event,SYNCHRONIZE,*ExEventObjectType,UserMode,&g_pDominateCmd->pMjWriteEventObject,HandleInformation);
		//if (!NT_SUCCESS(Status))
		//{
		//	ulInformation = 0;
		//	break;
		//}

		//g_pDominateCmd->hIrp_Mj_Close_Event = pR3DominateCmd->hIrp_Mj_Close_Event;
		//Status = ObReferenceObjectByHandle(g_pDominateCmd->hIrp_Mj_Close_Event,SYNCHRONIZE,*ExEventObjectType,UserMode,&g_pDominateCmd->pMjCloseEventObject,HandleInformation);
		//if (!NT_SUCCESS(Status))
		//{
		//	ulInformation = 0;
		//	break;
		//}


		g_pDominateCmd->bIs_Irp_Mj_Create = TRUE;
		g_pDominateCmd->bIs_Irp_Mj_Read = TRUE;
		g_pDominateCmd->bIs_Irp_Mj_Write = TRUE;
		g_pDominateCmd->bIs_Irp_Mj_Close = TRUE;
		g_pDominateCmd->bIs_Irp_Mj_SetInformation = TRUE;

		g_pDominateCmd->bMonitorStarted = FALSE;
		break;

	case SWITCHDRV_UNINIT_GLOBAL:
		if (g_pDominateCmd->bMonitorStarted == FALSE)
		{
			if (g_pDominateCmd->bIs_Irp_Mj_Close == FALSE && g_pDominateCmd->pMjCloseEventObject != NULL)
			{
				ObDereferenceObject(g_pDominateCmd->pMjCloseEventObject);
				g_pDominateCmd->pMjCloseEventObject = NULL;
			}

			if (g_pDominateCmd->bIs_Irp_Mj_Write == FALSE && g_pDominateCmd->pMjWriteEventObject != NULL)
			{
				ObDereferenceObject(g_pDominateCmd->pMjWriteEventObject);
				g_pDominateCmd->pMjWriteEventObject = NULL;
			}

			if (g_pDominateCmd->bIs_Irp_Mj_Read == FALSE && g_pDominateCmd->pMjReadEventObject != NULL)
			{
				ObDereferenceObject(g_pDominateCmd->pMjReadEventObject);
				g_pDominateCmd->pMjReadEventObject = NULL;
			}

			if (g_pDominateCmd->bIs_Irp_Mj_Create == FALSE && g_pDominateCmd->pMjCreateEventObject != NULL)
			{
				ObDereferenceObject(g_pDominateCmd->pMjCreateEventObject);
				g_pDominateCmd->pMjCreateEventObject = NULL;
			}

			if (g_pDominateCmd->bIs_Irp_Mj_SetInformation == FALSE && g_pDominateCmd->pMjSetInformationEventObject != NULL)
			{
				ObDereferenceObject(g_pDominateCmd->pMjSetInformationEventObject);
				g_pDominateCmd->pMjSetInformationEventObject = NULL;
			}
		}
		ulInformation = 0;
		Status = STATUS_SUCCESS;
		break;
	case SWITCHDRV_START_MONITOR:
		if (g_pDominateCmd->bMonitorStarted == FALSE)
		{
			g_pDominateCmd->bMonitorStarted = TRUE;

		}
		Status = STATUS_SUCCESS;
		ulInformation = 0;
		break;
	case SWITCHDRV_STOP_MONITOR:
		if (g_pDominateCmd->bMonitorStarted == TRUE)
		{
			g_pDominateCmd->bMonitorStarted = FALSE;

			g_pDominateCmd->bIs_Irp_Mj_Create = FALSE;
			g_pDominateCmd->bIs_Irp_Mj_Read = FALSE;
			g_pDominateCmd->bIs_Irp_Mj_Write = FALSE;
			g_pDominateCmd->bIs_Irp_Mj_Close = FALSE;
		}
		break;
	case SWITCHDRV_GET_EXPLORER:
		g_pDominateCmd->ExplorerProcess = GetProcEProcess(EXPLORER_NAME,g_ulProcessListEntryOffset);
		if (g_pDominateCmd->ExplorerProcess != NULL)
		{
			DbgPrint("Get explorer.exe process success,eprocess : %p\r\n",g_pDominateCmd->ExplorerProcess);
			Status = STATUS_SUCCESS;
			ulInformation = 0;
		}
		else
		{
			DbgPrint("get explorer.exe process failed.\r\n");
			Status = STATUS_UNSUCCESSFUL;
			ulInformation = 0;
		}
		break;
	case SWITCHDRV_SET_WINDOW_SHOW:
		DbgPrint("SWITCHDRV_SET_WINDOW_SHOW TRUE");
		g_pDominateCmd->bIsShowWindow = TRUE;
		Status = STATUS_SUCCESS;
		ulInformation = 0;
		break;
	case SWIRCHDRV_SET_WINDOW_HIDE:
		DbgPrint("SWITCHDRV_SET_WINDOW_SHOW FALSE");
		g_pDominateCmd->bIsShowWindow = FALSE;
		Status = STATUS_SUCCESS;
		ulInformation = 0;
		break;
	default:break;
	}
	lpIrp->IoStatus.Status = Status;
	lpIrp->IoStatus.Information = ulInformation;
	IoCompleteRequest(lpIrp,IO_NO_INCREMENT);
	return Status;
}
VOID InitializeDriverUnload(PDRIVER_OBJECT pDriverObject)
{
	UNICODE_STRING unStrSymLink;

	RtlInitUnicodeString(&unStrSymLink,SYMLINK_NAME);

	IoDeleteSymbolicLink(&unStrSymLink);

	IoDeleteDevice(pDriverObject->DeviceObject);
	return;
}
NTSTATUS InitializeDriver(PDRIVER_OBJECT pDriverObject,PUNICODE_STRING pUnStrRegPath)
{
	NTSTATUS ntStatus;
	UNICODE_STRING unStrDeviceName;
	UNICODE_STRING unStrSymName;
	PDEVICE_OBJECT pDeviceObject;

	UNREFERENCED_PARAMETER(pUnStrRegPath);

	ntStatus = STATUS_SUCCESS;

	RtlInitUnicodeString(&unStrDeviceName,DEIVCE_NAME);
	
	ntStatus = IoCreateDevice(pDriverObject, \
						    0, \
						    &unStrDeviceName, \
						    FILE_DEVICE_UNKNOWN, \
						    0, \
						    FALSE, \
						    &pDeviceObject);
	if(!NT_SUCCESS(ntStatus))
	{
		return ntStatus;
	}
	pDeviceObject->Flags |= DO_BUFFERED_IO;
	RtlInitUnicodeString(&unStrSymName,SYMLINK_NAME);
	ntStatus = IoCreateSymbolicLink(&unStrSymName,&unStrDeviceName);
	if(!NT_SUCCESS(ntStatus))
	{
		IoDeleteDevice(pDeviceObject);
		return ntStatus;
	}

	pDriverObject->MajorFunction[IRP_MJ_CREATE] =CreateMajorFunction;
	pDriverObject->MajorFunction[IRP_MJ_CLOSE] = CloseMajorFunction;
	pDriverObject->MajorFunction[IRP_MJ_READ] = NULL;
	pDriverObject->MajorFunction[IRP_MJ_WRITE] = NULL;
	pDriverObject->MajorFunction[IRP_MJ_DEVICE_CONTROL] = IoControlDispatch;

	pDriverObject->DriverUnload = InitializeDriverUnload;
	return ntStatus;
}