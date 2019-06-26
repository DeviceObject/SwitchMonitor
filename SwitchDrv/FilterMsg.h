
NTSTATUS SwitchConnect(__in PFLT_PORT ClientPort, \
					   __in PVOID ServerPortCookie, \
					   __in_bcount(SizeOfContext) PVOID ConnectionContext, \
					   __in ULONG SizeOfContext, \
					   __deref_out_opt PVOID *ConnectionCookie);

VOID SwitchDisconnect(__in_opt PVOID ConnectionCookie);

NTSTATUS SwitchMessage(__in PVOID ConnectionCookie,
					   __in_bcount_opt(InputBufferSize) PVOID InputBuffer,
					   __in ULONG InputBufferSize,
					   __out_bcount_part_opt(OutputBufferSize,*ReturnOutputBufferLength) PVOID OutputBuffer,
					   __in ULONG OutputBufferSize,
					   __out PULONG ReturnOutputBufferLength);