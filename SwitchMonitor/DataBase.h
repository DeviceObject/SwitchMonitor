

typedef struct _QUICK_KEY
{
	int nId;
	int nUserDefine;
	CHAR KeyName[32];
	int nKeyValue;
	CHAR SubKeyValue;
} QUICK_KEY,*PQUICK_KEY;

typedef struct _QUICK_KEY_LIST
{
	PQUICK_KEY QuickKey;
	int nMaxCount;
	int nCurCount;
} QUICK_KEY_LIST,*PQUICK_KEY_LIST;

sqlite3 *OpenDataBase();
void CloseDataBase(sqlite3 *db);
int callback(void* ,int nCount,char** pValue,char** pName);
BOOL GetMaxCountDataBase(sqlite3 *db,int *nMaxCount);
BOOL GetKeyToDataBase(sqlite3 *db,PQUICK_KEY_LIST pQuickList);
BOOL SetKeyToDataBase(sqlite3 *db,int nUserDefine,char* szKeyName,int nKeyValue,char szSubKey);
BOOL UpdateKeyToDataBase(sqlite3 *db,int nID,int nUserDefine,char* szKeyName,int nKeyValue,char szSubKey);
BOOL DeleteKeyToDataBase(sqlite3 *db,int nID);