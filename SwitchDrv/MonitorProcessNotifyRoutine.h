
NTSTATUS RegisterCreateProcessMonitor(BOOLEAN bCreated);
int GetProcessNameOffset(char *szProcessName);
PEPROCESS GetProcEProcess(CHAR *szProcName,ULONG ulProcessListEntryOffset);