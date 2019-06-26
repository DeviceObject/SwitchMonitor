
#include "DominateCommand.h"
#include "VolumeDeviceToDosName.h"
#include "Irp_Mj_Create.h"

FLT_POSTOP_CALLBACK_STATUS IrpMjCreatePost(
	_Inout_ PFLT_CALLBACK_DATA Data,
	_In_ PCFLT_RELATED_OBJECTS FltObjects,
	_In_opt_ PVOID CompletionContext,
	_In_ FLT_POST_OPERATION_FLAGS Flags)
{
	PFLT_FILE_NAME_INFORMATION NameInfo;
	NTSTATUS Status;
	PCOMMAND_MESSAGE pCmdMsg;
	REPLY_MSG ReplyMsg;
	ULONG ulRetSize;
	KIRQL CurIrql;
	UNICODE_STRING unStrDosName;
	BOOLEAN bIsDirectory;


	UNREFERENCED_PARAMETER(FltObjects);
	UNREFERENCED_PARAMETER(CompletionContext);
	UNREFERENCED_PARAMETER(Flags);

	pCmdMsg = NULL;
	ulRetSize = 0;
	bIsDirectory = FALSE;
	RtlZeroMemory(&ReplyMsg,sizeof(REPLY_MSG));

	Status = FltIsDirectory(FltObjects->FileObject,FltObjects->Instance,&bIsDirectory);
	if (NT_SUCCESS(Status))
	{
		if (bIsDirectory == TRUE)
		{
			DbgPrint("Is directory\r\n");
			return FLT_POSTOP_FINISHED_PROCESSING;
		}
	}

	if (!NT_SUCCESS(Data->IoStatus.Status) || (STATUS_REPARSE == Data->IoStatus.Status))
	{
		return FLT_POSTOP_FINISHED_PROCESSING;
	}
	if (IoThreadToProcess(Data->Thread) == g_pDominateCmd->UserProcess)
	{
		return FLT_POSTOP_FINISHED_PROCESSING;
	}
	
	if (IoThreadToProcess(Data->Thread) == g_pDominateCmd->ExplorerProcess)
	{
		if (g_pDominateCmd->bIs_Irp_Mj_Create == TRUE && g_pDominateCmd->bMonitorStarted == TRUE && g_pDominateCmd->bIsShowWindow == FALSE)
		{
			Status = FltGetFileNameInformation(Data,FLT_FILE_NAME_NORMALIZED | FLT_FILE_NAME_QUERY_DEFAULT,&NameInfo);
			if (!NT_SUCCESS(Status))
			{
				return FLT_POSTOP_FINISHED_PROCESSING;
			}
			FltParseFileNameInformation(NameInfo);

			pCmdMsg = (PCOMMAND_MESSAGE)ExAllocatePool(NonPagedPool,sizeof(COMMAND_MESSAGE));
			while (pCmdMsg == NULL)
			{
				pCmdMsg = (PCOMMAND_MESSAGE)ExAllocatePool(NonPagedPool,sizeof(COMMAND_MESSAGE));
			}
			RtlZeroMemory(pCmdMsg,sizeof(COMMAND_MESSAGE));

			if (Data->IoStatus.Information == FILE_CREATE)
			{
				StringCchCopyW(pCmdMsg->FileOpear,32,L"新建文件");
				StringCchCopyW(pCmdMsg->ShowMsg,MAX_SHOWMSG,L"新建文件:");

			}
			else if (Data->IoStatus.Information == FILE_OPEN)
			{
				StringCchCopyW(pCmdMsg->FileOpear,32,L"打开文件");
				StringCchCopyW(pCmdMsg->ShowMsg,MAX_SHOWMSG,L"打开文件:");

			}
			else
			{
				StringCchCopyW(pCmdMsg->FileOpear,32,L":");
				StringCchCopyW(pCmdMsg->ShowMsg,MAX_SHOWMSG,L":");
			}
			MyRtlVolumeDeviceToDosName(&NameInfo->Volume,&unStrDosName);
			if (NameInfo->Volume.Length > 0 && NameInfo->Volume.Length < 100)
			{
				StringCchCopyW(pCmdMsg->Volume,100,NameInfo->Volume.Buffer);
			}
			if (unStrDosName.Length > 0)
			{
				StringCchCatW(pCmdMsg->ShowMsg,MAX_SHOWMSG,unStrDosName.Buffer);
			}
			if (NameInfo->ParentDir.Length > 0 && NameInfo->ParentDir.Length < 256)
			{
				StringCchCopyW(pCmdMsg->ParentDir,256,NameInfo->ParentDir.Buffer);
				StringCchCatW(pCmdMsg->ShowMsg,MAX_SHOWMSG,NameInfo->ParentDir.Buffer);
			}
			if (NameInfo->FinalComponent.Length > 0 && NameInfo->FinalComponent.Length < 256)
			{
				StringCchCopyW(pCmdMsg->FinalComponent,256,NameInfo->FinalComponent.Buffer);
				//StringCchCatW(pCmdMsg->ShowMsg,MAX_SHOWMSG,NameInfo->FinalComponent.Buffer);
			}
			if (NameInfo->Extension.Length > 0 && NameInfo->Extension.Length < 32)
			{
				StringCchCopyW(pCmdMsg->Extension,32,NameInfo->Extension.Buffer);
			}
			if (NameInfo->Name.Length > 0 && NameInfo->Name.Length < 300)
			{
				StringCchCopyW(pCmdMsg->Name,300,NameInfo->Name.Buffer);
			}

			DbgPrint("%S\n",pCmdMsg->ShowMsg);

			ulRetSize = sizeof(REPLY_MSG);
			CurIrql = KeGetCurrentIrql();
			if (CurIrql <= APC_LEVEL)
			{
				Status = FltSendMessage(g_pDominateCmd->gFilterHandle,&g_pDominateCmd->gClientPort,pCmdMsg,sizeof(COMMAND_MESSAGE),pCmdMsg,&ulRetSize,NULL);
				if (STATUS_SUCCESS == Status)
				{
					DbgPrint("FltSendMessage Success:%08x\r\n",Status);
					ReplyMsg.bIsSuccess = ((PREPLY_MSG)pCmdMsg)->bIsSuccess;
					if (ReplyMsg.bIsSuccess == TRUE)
					{
						DbgPrint("TRUE\r\n");
					}
				}
				else
				{
					DbgPrint("FltSendMessage failed:%08x\r\n",Status);
				}
			}
			else
			{
				DbgPrint("Irql > APC_LEVEL\r\n");
			}
			if (pCmdMsg)
			{
				ExFreePool(pCmdMsg);
				pCmdMsg = NULL;
			}
			FltReleaseFileNameInformation(NameInfo);
		}
	}
	return FLT_POSTOP_FINISHED_PROCESSING;
}

FLT_PREOP_CALLBACK_STATUS IrpMjCreatePre(
	_Inout_ PFLT_CALLBACK_DATA Data,
	_In_ PCFLT_RELATED_OBJECTS FltObjects,
	_Flt_CompletionContext_Outptr_ PVOID *CompletionContext)
{
	UCHAR create_option;
	BOOLEAN bIsDirectory;
	NTSTATUS Status;


	UNREFERENCED_PARAMETER(Data);
//	UNREFERENCED_PARAMETER(FltObjects);
	UNREFERENCED_PARAMETER(CompletionContext);

	bIsDirectory = FALSE;

	Status = FltIsDirectory(FltObjects->FileObject,FltObjects->Instance,&bIsDirectory);
	if (NT_SUCCESS(Status))
	{
		if (bIsDirectory == TRUE)
		{
			DbgPrint("Is directory\r\n");
			return FLT_PREOP_SUCCESS_NO_CALLBACK;
		}
	}
	if (IoThreadToProcess(Data->Thread) == g_pDominateCmd->ExplorerProcess)
	{
		if (g_pDominateCmd->bIs_Irp_Mj_Create == TRUE && g_pDominateCmd->bMonitorStarted == TRUE && g_pDominateCmd->bIsShowWindow == FALSE)
		{
			create_option = (UCHAR)(((Data->Iopb->Parameters.Create.Options)>>24) & 0xFF);
			switch (create_option)
			{
			case 0:    //FILE_SUPERSEDE
				KdPrint(("[IrpMjCreatePre]: PreCreate-<FILE_SUPERSEDE>->新建/覆盖文件\n"));
				break;
			case 1:    //FILE_OPEN 
				KdPrint(("[IrpMjCreatePre]: PreCreate-<FILE_OPEN>->打开已存在文件\n"));
				break;
			case 2:    //FILE_CREATE
				KdPrint(("[IrpMjCreatePre]: PreCreate-<FILE_CREATE>->新建文件\n"));
				break;
			case 3:    //FILE_OPEN_IF
				KdPrint(("[IrpMjCreatePre]: PreCreate-<FILE_OPEN_IF>->打开/创建文件\n"));
				break;
			case 4:    //FILE_OVERWRITE
				KdPrint(("[IrpMjCreatePre]: PreCreate-<FILE_OVERWRITE>->覆盖文件\n"));
				break;
			case 5:    //FILE_MAXIMUM_DISPOSITION
				KdPrint(("[IrpMjCreatePre]: PreCreate-<FILE_MAXIMUM_DISPOSITION>->打开并覆盖/创建文件\n"));
				break;
			default:
				break;
			}//*/
		}
	}
	else
	{
		return FLT_PREOP_SUCCESS_NO_CALLBACK;
	}
	return FLT_PREOP_SUCCESS_WITH_CALLBACK;
}