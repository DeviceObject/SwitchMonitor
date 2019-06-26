
typedef struct _IRP_SET_INFORMATION
{
	BOOLEAN bSetd;
	WCHAR RenName[MAX_SHOWMSG];
} IRP_SET_INFORMATION,*PIRP_SET_INFORMATION;




FLT_POSTOP_CALLBACK_STATUS IrpMjSetInformationPost(
	_Inout_ PFLT_CALLBACK_DATA Data,
	_In_ PCFLT_RELATED_OBJECTS FltObjects,
	_In_opt_ PVOID CompletionContext,
	_In_ FLT_POST_OPERATION_FLAGS Flags);

FLT_PREOP_CALLBACK_STATUS IrpMjSetInformationPre(
	_Inout_ PFLT_CALLBACK_DATA Data,
	_In_ PCFLT_RELATED_OBJECTS FltObjects,
	_Flt_CompletionContext_Outptr_ PVOID *CompletionContext);