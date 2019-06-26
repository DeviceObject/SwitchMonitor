#include "DominateCommand.h"
#include "VolumeDeviceToDosName.h"

////输入\\??\\c:-->\\device\\\harddiskvolume1
NTSTATUS QuerySymbolicLink(IN PUNICODE_STRING SymbolicLinkName,OUT PUNICODE_STRING LinkTarget)                                  
{
	OBJECT_ATTRIBUTES	oa		= {0};
	NTSTATUS			status	= 0;
	HANDLE				handle	= NULL;

	InitializeObjectAttributes(&oa,SymbolicLinkName,OBJ_CASE_INSENSITIVE,0,0);
	status = ZwOpenSymbolicLinkObject(&handle, GENERIC_READ, &oa);
	if (!NT_SUCCESS(status))
	{
		return status;
	}
	LinkTarget->MaximumLength = 200*sizeof(WCHAR);
	LinkTarget->Length = 0;
	LinkTarget->Buffer = ExAllocatePoolWithTag(PagedPool, LinkTarget->MaximumLength,'SOD');
	if (!LinkTarget->Buffer)
	{
		ZwClose(handle);
		return STATUS_INSUFFICIENT_RESOURCES;
	}
	RtlZeroMemory(LinkTarget->Buffer, LinkTarget->MaximumLength);
	status = ZwQuerySymbolicLinkObject(handle, LinkTarget, NULL);
	ZwClose(handle);
	if (!NT_SUCCESS(status))
	{
		ExFreePool(LinkTarget->Buffer);
	}
	return status;
}
////输入\\Device\\harddiskvolume1
//输出C:
//DosName.Buffer的内存记得释放
NTSTATUS MyRtlVolumeDeviceToDosName(IN PUNICODE_STRING DeviceName,OUT PUNICODE_STRING DosName)
	/*++
	Routine Description:
	This routine returns a valid DOS path for the given device object.
	This caller of this routine must call ExFreePool on DosName->Buffer
	when it is no longer needed.

	Arguments:

	VolumeDeviceObject - Supplies the volume device object.
	DosName - Returns the DOS name for the volume
	Return Value:

	NTSTATUS

	--*/

{
	NTSTATUS				status					= 0;
	UNICODE_STRING			driveLetterName			= {0};
	WCHAR					driveLetterNameBuf[128] = {0};
	WCHAR					c						= L'\0';
	WCHAR					DriLetter[3]			= {0};
	UNICODE_STRING			linkTarget				= {0};

	for (c = L'A'; c <= L'Z'; c++)
	{
		RtlInitEmptyUnicodeString(&driveLetterName,driveLetterNameBuf,sizeof(driveLetterNameBuf));
		RtlAppendUnicodeToString(&driveLetterName, L"\\??\\");
		DriLetter[0] = c;
		DriLetter[1] = L':';
		DriLetter[2] = 0;
		RtlAppendUnicodeToString(&driveLetterName,DriLetter);

		status = QuerySymbolicLink(&driveLetterName, &linkTarget);
		if (!NT_SUCCESS(status))
		{
			continue;
		}

		if (RtlEqualUnicodeString(&linkTarget, DeviceName, TRUE))
		{
			ExFreePool(linkTarget.Buffer);
			break;
		}

		ExFreePool(linkTarget.Buffer);
	}

	if (c <= L'Z')
	{
		DosName->Buffer = ExAllocatePoolWithTag(PagedPool, 3*sizeof(WCHAR), 'SOD');
		if (!DosName->Buffer)
		{
			return STATUS_INSUFFICIENT_RESOURCES;
		}

		DosName->MaximumLength = 6;
		DosName->Length   = 4;
		*DosName->Buffer  = c;
		*(DosName->Buffer+ 1) = ':';
		*(DosName->Buffer+ 2) = 0;

		return STATUS_SUCCESS;
	}
	return status;
} 