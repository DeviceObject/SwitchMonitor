
typedef enum _WINDOWS_VERSION
{
	WINDOWS_VERSION_NONE,       //  0
	WINDOWS_VERSION_2K,
	WINDOWS_VERSION_XP,
	WINDOWS_VERSION_2K3,
	WINDOWS_VERSION_2K3_SP1_SP2,
	WINDOWS_VERSION_VISTA_2008,
	WINDOWS_VERSION_7
} WINDOWS_VERSION,*PWINDOWS_VERSION;

WINDOWS_VERSION WinVersion;



#define PROCESS_NAME_OFFSET_2K               0x01FC//NT5.0.2195.7133
#define PROCESS_NAME_OFFSET_XP               0x0174//NT5.1.2600.3093
#define PROCESS_NAME_OFFSET_2K3              0x0154//nt5.2.3790.0
#define PROCESS_NAME_OFFSET_2K3_SP1			 0x0164//nt5.2.3790.1830
#define PROCESS_NAME_OFFSET_VISTA			 0x014c //nt6.0 6.1
#define PROCESS_NAME_OFFSET_WIN7	         0x016c

typedef NTSTATUS (NTAPI * PFN_RtlGetVersion)(OUT PRTL_OSVERSIONINFOW lpVersionInformation);
WINDOWS_VERSION GetKernelOsVersion();