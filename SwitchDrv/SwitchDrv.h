#pragma prefast(disable:__WARNING_ENCODE_MEMBER_FUNCTION_POINTER, "Not valid for kernel mode drivers")

extern ULONG gTraceFlags = 1;

#define PTDBG_TRACE_ROUTINES            0x00000001
#define PTDBG_TRACE_OPERATION_STATUS    0x00000002

#define PT_DBG_PRINT( _dbgLevel, _string )          \
	(FlagOn(gTraceFlags,(_dbgLevel)) ?              \
	DbgPrint _string :                          \
	((int)0))
/*************************************************************************
Prototypes
*************************************************************************/
NTSTATUS
	SwitchDrvInstanceSetup (
	_In_ PCFLT_RELATED_OBJECTS FltObjects,
	_In_ FLT_INSTANCE_SETUP_FLAGS Flags,
	_In_ DEVICE_TYPE VolumeDeviceType,
	_In_ FLT_FILESYSTEM_TYPE VolumeFilesystemType
	);

VOID
	SwitchDrvInstanceTeardownStart (
	_In_ PCFLT_RELATED_OBJECTS FltObjects,
	_In_ FLT_INSTANCE_TEARDOWN_FLAGS Flags
	);

VOID
	SwitchDrvInstanceTeardownComplete (
	_In_ PCFLT_RELATED_OBJECTS FltObjects,
	_In_ FLT_INSTANCE_TEARDOWN_FLAGS Flags
	);
NTSTATUS
	SwitchDrvInstanceQueryTeardown (
	_In_ PCFLT_RELATED_OBJECTS FltObjects,
	_In_ FLT_INSTANCE_QUERY_TEARDOWN_FLAGS Flags
	);

DRIVER_INITIALIZE DriverEntry;

NTSTATUS
	DriverEntry (
	_In_ PDRIVER_OBJECT DriverObject,
	_In_ PUNICODE_STRING RegistryPath
	);

NTSTATUS
	SwitchDrvUnload (
	_In_ FLT_FILTER_UNLOAD_FLAGS Flags
	);

FLT_PREOP_CALLBACK_STATUS SwitchDrvPre(
	_Inout_ PFLT_CALLBACK_DATA Data,
	_In_ PCFLT_RELATED_OBJECTS FltObjects,
	_Flt_CompletionContext_Outptr_ PVOID *CompletionContext);

VOID SwitchDrvOperationStatusCallback(
	_In_ PCFLT_RELATED_OBJECTS FltObjects,
	_In_ PFLT_IO_PARAMETER_BLOCK ParameterSnapshot,
	_In_ NTSTATUS OperationStatus,
	_In_ PVOID RequesterContext);

FLT_POSTOP_CALLBACK_STATUS SwitchDrvPost(
	_Inout_ PFLT_CALLBACK_DATA Data,
	_In_ PCFLT_RELATED_OBJECTS FltObjects,
	_In_opt_ PVOID CompletionContext,
	_In_ FLT_POST_OPERATION_FLAGS Flags);

FLT_PREOP_CALLBACK_STATUS
	SwitchDrvPreOperationNoPostOperation(
	_Inout_ PFLT_CALLBACK_DATA Data,
	_In_ PCFLT_RELATED_OBJECTS FltObjects,
	_Flt_CompletionContext_Outptr_ PVOID *CompletionContext
	);

BOOLEAN
	SwitchDrvDoRequestOperationStatus(
	_In_ PFLT_CALLBACK_DATA Data
	);


FLT_POSTOP_CALLBACK_STATUS
	SwitchDrvCreatePost(
	_Inout_ PFLT_CALLBACK_DATA Data,
	_In_ PCFLT_RELATED_OBJECTS FltObjects,
	_In_opt_ PVOID CompletionContext,
	_In_ FLT_POST_OPERATION_FLAGS Flags
	);

FLT_PREOP_CALLBACK_STATUS
	SwitchDrvCreatePre(
	_Inout_ PFLT_CALLBACK_DATA Data,
	_In_ PCFLT_RELATED_OBJECTS FltObjects,
	_Flt_CompletionContext_Outptr_ PVOID *CompletionContext
	);

#ifdef ALLOC_PRAGMA
#pragma alloc_text(INIT, DriverEntry)
#pragma alloc_text(PAGE, SwitchDrvUnload)
#endif

CONST FLT_OPERATION_REGISTRATION Callbacks[] = {
	{ 
		IRP_MJ_CREATE,
		0,
		IrpMjCreatePre,
		IrpMjCreatePost
	},
	{
		IRP_MJ_READ,
		0,
		IrpMjReadPre,
		IrpMjReadPost
	},
	{
		IRP_MJ_SET_INFORMATION,
		0,
		IrpMjSetInformationPre,
		IrpMjSetInformationPost
	},
	{ 
		IRP_MJ_OPERATION_END 
	}
};

CONST FLT_REGISTRATION FilterRegistration = {
	sizeof( FLT_REGISTRATION ),         //  Size
	FLT_REGISTRATION_VERSION,           //  Version
	0,                                  //  Flags
	NULL,                               //  Context
	Callbacks,                          //  Operation callbacks
	SwitchDrvUnload,                    //  MiniFilterUnload
	SwitchDrvInstanceSetup,								//  InstanceSetup
	SwitchDrvInstanceQueryTeardown,								//  InstanceQueryTeardown
	SwitchDrvInstanceTeardownStart,								//  InstanceTeardownStart
	SwitchDrvInstanceTeardownComplete,								//  InstanceTeardownComplete
	NULL,                               //  GenerateFileName
	NULL,                               //  GenerateDestinationFileName
	NULL                                //  NormalizeNameComponent

};


