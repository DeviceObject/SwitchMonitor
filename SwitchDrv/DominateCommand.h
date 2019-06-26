#include <fltKernel.h>
#include <strsafe.h>

#define MAX_SHOWMSG				1024
#define SWITCHDRV_PORT_NAME		L"\\SwitchPort"
#define	EVENT_NAME				L"\\BaseNamedObjects\\SwitchEvent"

#define DEIVCE_NAME	L"\\Device\\SwitchDrv"
#define SYMLINK_NAME L"\\DosDevices\\SwitchDrv"


#define EXPLORER_NAME	"explorer.exe"
#define NT_SYSTEM_NAME	"System"


#define  FILE_DEVICE_X_PROTECT 0x8000
#define  X_PROTECT_CTL_BASE  0x800
//IOÇëÇó¶¨Òå
#define  CTL_CODE_SHIT(i)  CTL_CODE(FILE_DEVICE_X_PROTECT,X_PROTECT_CTL_BASE+i,METHOD_BUFFERED,FILE_ANY_ACCESS)

#define SWITCHDRV_INITIAL_GLOBAL					CTL_CODE_SHIT(0)
#define SWITCHDRV_UNINIT_GLOBAL						CTL_CODE_SHIT(1)
#define SWITCHDRV_START_MONITOR						CTL_CODE_SHIT(2)
#define SWITCHDRV_STOP_MONITOR						CTL_CODE_SHIT(3)
#define SWITCHDRV_GET_EXPLORER						CTL_CODE_SHIT(4)
#define SWITCHDRV_SET_WINDOW_SHOW					CTL_CODE_SHIT(5)
#define SWIRCHDRV_SET_WINDOW_HIDE					CTL_CODE_SHIT(6)

typedef struct _DOMINATECMD 
{
	PFLT_FILTER gFilterHandle;
	PFLT_PORT gServerPort;
	PFLT_PORT gClientPort;

	PEPROCESS UserProcess;
	PEPROCESS ExplorerProcess;

	BOOLEAN bMonitorStarted;
	BOOLEAN bIsShowWindow;

	BOOLEAN bIs_Irp_Mj_Create;
	HANDLE hIrp_Mj_Create_Event;
	PKEVENT pMjCreateEventObject;

	BOOLEAN bIs_Irp_Mj_Read;
	HANDLE hIrp_Mj_Read_Event;
	PKEVENT pMjReadEventObject;

	BOOLEAN bIs_Irp_Mj_Write;
	HANDLE hIrp_Mj_Write_Event;
	PKEVENT pMjWriteEventObject;

	BOOLEAN bIs_Irp_Mj_Close;
	HANDLE hIrp_Mj_Close_Event;
	PKEVENT pMjCloseEventObject;

	BOOLEAN bIs_Irp_Mj_SetInformation;
	HANDLE hIrp_Mj_SetInformation_Event;
	PKEVENT pMjSetInformationEventObject;
} DOMINATECMD,*PDOMINATECMD;

typedef struct _R3_DOMINATECMD
{
	//BOOLEAN bIs_Irp_Mj_Create;
	HANDLE hIrp_Mj_Create_Event;
	//PKEVENT pMjCreateEventObject;

	//BOOLEAN bIs_Irp_Mj_Read;
	HANDLE hIrp_Mj_Read_Event;
	//PKEVENT pMjReadEventObject;

	//BOOLEAN bIs_Irp_Mj_Write;
	HANDLE hIrp_Mj_Write_Event;
	//PKEVENT pMjWriteEventObject;

	//BOOLEAN bIs_Irp_Mj_Close;
	HANDLE hIrp_Mj_Close_Event;
	//PKEVENT pMjCloseEventObject;

} R3_DOMINATECMD,*PR3_DOMINATECMD;

//typedef struct _LIST_FILTER_DATA
//{
//	LIST_ENTRY pList;
//	UNICODE_STRING unStrData;
//} LIST_FILTER_DATA,*PLIST_FILTER_DATA;

//StringCchCatW(pCmdMsg->ShowMsg,MAX_SHOWMSG,unStrDosName.Buffer);
//
//StringCchCatW(pCmdMsg->ShowMsg,MAX_SHOWMSG,NameInfo->ParentDir.Buffer);
//
//StringCchCatW(pCmdMsg->ShowMsg,MAX_SHOWMSG,NameInfo->FinalComponent.Buffer);

typedef struct _COMMAND_MESSAGE
{
	BOOLEAN bIsSuccess;
	WCHAR Volume[100];
	WCHAR Name[300];
	WCHAR Extension[32];
	WCHAR FinalComponent[256];
	WCHAR ParentDir[256];
	WCHAR FileOpear[32];
	WCHAR ShowMsg[MAX_SHOWMSG];
} COMMAND_MESSAGE,*PCOMMAND_MESSAGE;

typedef struct _REPLY_MSG
{
	BOOLEAN bIsSuccess;
} REPLY_MSG,*PREPLY_MSG;

extern PDOMINATECMD g_pDominateCmd;
extern ULONG g_ulProcessNameOffset;
extern ULONG g_ulProcessListEntryOffset;