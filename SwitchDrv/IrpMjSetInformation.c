#include "DominateCommand.h"
#include "VolumeDeviceToDosName.h"
#include "IrpMjSetInformation.h"

FLT_PREOP_CALLBACK_STATUS IrpMjSetInformationPre(
	_Inout_ PFLT_CALLBACK_DATA Data,
	_In_ PCFLT_RELATED_OBJECTS FltObjects,
	_Flt_CompletionContext_Outptr_ PVOID *CompletionContext)
{
	PIRP_SET_INFORMATION pIrpSetInfo;
	UNICODE_STRING unStrDosName;
	NTSTATUS ntStatus;
	PFLT_FILE_NAME_INFORMATION NameInfo;
	BOOLEAN bIsDirectory;
	NTSTATUS Status;

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
	if (!NT_SUCCESS(Data->IoStatus.Status) || (STATUS_REPARSE == Data->IoStatus.Status))
	{
		return FLT_PREOP_SUCCESS_NO_CALLBACK;
	}
	if (IoThreadToProcess(Data->Thread) == g_pDominateCmd->UserProcess)
	{
		return FLT_PREOP_SUCCESS_NO_CALLBACK;
	}
	if (IoThreadToProcess(Data->Thread) == g_pDominateCmd->ExplorerProcess)
	{
		if (g_pDominateCmd->bIs_Irp_Mj_SetInformation == TRUE && g_pDominateCmd->bMonitorStarted == TRUE && g_pDominateCmd->bIsShowWindow == FALSE)
		{
			ntStatus = FltGetFileNameInformation(Data,FLT_FILE_NAME_NORMALIZED | FLT_FILE_NAME_QUERY_DEFAULT,&NameInfo);
			if (!NT_SUCCESS(ntStatus))
			{
				return FLT_PREOP_SUCCESS_NO_CALLBACK;
			}
			FltParseFileNameInformation(NameInfo);
			if (FltObjects->FileObject->FileName.Length > 0)
			{
				DbgPrint("SwitchDrv!IrpMjSetInformationPre: Entered\n");
				pIrpSetInfo = (PIRP_SET_INFORMATION)ExAllocatePool(NonPagedPool,sizeof(IRP_SET_INFORMATION));
				if (pIrpSetInfo == NULL)
				{
					return FLT_PREOP_SUCCESS_NO_CALLBACK;
				}
				RtlZeroMemory(pIrpSetInfo,sizeof(IRP_SET_INFORMATION));
				MyRtlVolumeDeviceToDosName(&NameInfo->Volume,&unStrDosName);
				StringCchCopyW(pIrpSetInfo->RenName,MAX_SHOWMSG,unStrDosName.Buffer);
				StringCchCatW(pIrpSetInfo->RenName,MAX_SHOWMSG,L"\\");
				StringCchCatW(pIrpSetInfo->RenName,MAX_SHOWMSG,FltObjects->FileObject->FileName.Buffer);
				pIrpSetInfo->bSetd = TRUE;
				*CompletionContext = pIrpSetInfo;
				FltReleaseFileNameInformation(NameInfo);
				return FLT_PREOP_SYNCHRONIZE;
			}
			FltReleaseFileNameInformation(NameInfo);
		}
	}
	return FLT_PREOP_SUCCESS_WITH_CALLBACK;
}


FLT_POSTOP_CALLBACK_STATUS IrpMjSetInformationPost(
	_Inout_ PFLT_CALLBACK_DATA Data,
	_In_ PCFLT_RELATED_OBJECTS FltObjects,
	_In_opt_ PVOID CompletionContext,
	_In_ FLT_POST_OPERATION_FLAGS Flags)
{

	PFILE_RENAME_INFORMATION RenameInfo;
	PFILE_DISPOSITION_INFORMATION DispositionInfo;
	PIRP_SET_INFORMATION pIrpSetInfo;
	PCOMMAND_MESSAGE pCmdMsg;
	NTSTATUS Status;
	REPLY_MSG ReplyMsg;
	ULONG ulRetSize;
	KIRQL CurIrql;
	WCHAR SzTmp[512];
	PFLT_FILE_NAME_INFORMATION NameInfo;
	BOOLEAN bIsDirectory;

	UNREFERENCED_PARAMETER(Flags);
	bIsDirectory = FALSE;

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
		DbgPrint("SwitchDrv!IrpMjSetInformationPost: Entered\n");
		if (g_pDominateCmd->bIs_Irp_Mj_SetInformation == TRUE && g_pDominateCmd->bMonitorStarted == TRUE && g_pDominateCmd->bIsShowWindow == FALSE)
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

			if (Data->Iopb->Parameters.SetFileInformation.FileInformationClass == FileRenameInformation)
			{

				RenameInfo = (PFILE_RENAME_INFORMATION)Data->Iopb->Parameters.SetFileInformation.InfoBuffer;
				if (RenameInfo->FileName)
				{
					pIrpSetInfo = (PIRP_SET_INFORMATION)CompletionContext;
					DbgPrint("%S\t\t %d\n",pIrpSetInfo->RenName,pIrpSetInfo->bSetd);
					DbgPrint("%S\n",RenameInfo->FileName);
					RtlZeroMemory(SzTmp,512);
					StringCchCopyW(SzTmp,512,RenameInfo->FileName);

					StringCchCopyW(pCmdMsg->ShowMsg,MAX_SHOWMSG,pIrpSetInfo->RenName);
					StringCchCatW(pCmdMsg->ShowMsg,MAX_SHOWMSG,L" 重命名: ");
					StringCchCatW(pCmdMsg->ShowMsg,MAX_SHOWMSG,(WCHAR*)&SzTmp[4]);

					StringCchCopyW(pCmdMsg->FileOpear,MAX_SHOWMSG,L"重命名");

					if (NameInfo->Volume.Length > 0 && NameInfo->Volume.Length < 100)
					{
						StringCchCopyW(pCmdMsg->Volume,100,NameInfo->Volume.Buffer);
					}

					if (NameInfo->ParentDir.Length > 0 && NameInfo->ParentDir.Length < 256)
					{
						StringCchCopyW(pCmdMsg->ParentDir,256,NameInfo->ParentDir.Buffer);
					}

					if (NameInfo->FinalComponent.Length > 0 && NameInfo->FinalComponent.Length < 256)
					{
						StringCchCopyW(pCmdMsg->FinalComponent,256,NameInfo->FinalComponent.Buffer);
					}

					if (NameInfo->Extension.Length > 0 && NameInfo->Extension.Length < 32)
					{
						StringCchCopyW(pCmdMsg->Extension,32,NameInfo->Extension.Buffer);
					}

					if (NameInfo->Name.Length > 0 && NameInfo->Name.Length < 300)
					{
						StringCchCopyW(pCmdMsg->Name,300,NameInfo->Name.Buffer);
					}
					pCmdMsg->bIsSuccess = TRUE;
				}
				
			}
			else if (Data->Iopb->Parameters.SetFileInformation.FileInformationClass == FileDispositionInformation)
			{
				DispositionInfo = (PFILE_DISPOSITION_INFORMATION)Data->Iopb->Parameters.SetFileInformation.InfoBuffer;
				if (DispositionInfo->DeleteFile == TRUE)
				{
					StringCchCopyW(pCmdMsg->FileOpear,MAX_SHOWMSG,L"删除");

					if (NameInfo->Volume.Length > 0 && NameInfo->Volume.Length < 100)
					{
						StringCchCopyW(pCmdMsg->Volume,100,NameInfo->Volume.Buffer);
					}

					if (NameInfo->ParentDir.Length > 0 && NameInfo->ParentDir.Length < 256)
					{
						StringCchCopyW(pCmdMsg->ParentDir,256,NameInfo->ParentDir.Buffer);
					}

					if (NameInfo->FinalComponent.Length > 0 && NameInfo->FinalComponent.Length < 256)
					{
						StringCchCopyW(pCmdMsg->FinalComponent,256,NameInfo->FinalComponent.Buffer);
					}

					if (NameInfo->Extension.Length > 0 && NameInfo->Extension.Length < 32)
					{
						StringCchCopyW(pCmdMsg->Extension,32,NameInfo->Extension.Buffer);
					}

					if (NameInfo->Name.Length > 0 && NameInfo->Name.Length < 300)
					{
						StringCchCopyW(pCmdMsg->Name,300,NameInfo->Name.Buffer);
					}
					pCmdMsg->bIsSuccess = TRUE;
				}
			}
			else
			{
				DbgPrint("IrpMjSetInformationPost Unknow\r\n");
			}
			if (pCmdMsg->bIsSuccess == TRUE)
			{
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