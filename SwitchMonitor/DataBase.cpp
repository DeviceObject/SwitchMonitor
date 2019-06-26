#include "stdafx.h"
#include "DataBase.h"


sqlite3 *OpenDataBase()
{
	BOOL bRet = FALSE;
	sqlite3 *db = NULL;
	char* errmsg;
	char **dbResult;
	int nRow, nColumn;
	int nRet = sqlite3_open("BeQuick.db",&db);
	if (nRet == SQLITE_OK)
	{
		OutputDebugString(TEXT("DataBasse open successfully!\n"));
		nRet = sqlite3_exec(db,"CREATE TABLE QuickKey(Id integer primary key autoincrement,UserDefine integer,KeyName varchar(32),KeyValue integer,SubKey varchar(32));",NULL,NULL,&errmsg); 
		if (nRet == SQLITE_OK)
		{
			OutputDebugString(TEXT("Create table successfully!\n"));
		}
		else
		{
			OutputDebugString(TEXT("Create table failed!\n"));
			sqlite3_get_table(db, "SELECT * FROM sqlite_master where type='table' and name='QuickKey'",&dbResult,&nRow,&nColumn,NULL);
			if(nRow > 0 )
			{
				OutputDebugString(TEXT("此表已经存在!\n"));
				// 查询数据表
			}
			sqlite3_free_table(dbResult);
		}
		return db;
	}
	OutputDebugString(TEXT("DataBasse open failed!\n"));
	return NULL;
}
void CloseDataBase(sqlite3 *db)
{
	sqlite3_close(db);
}
int callback(void* ,int nCount,char** pValue,char** pName)
{
	for (int i = 1;i <= nCount;i++)
	{
		OutputDebugStringA(pName[i]);
		OutputDebugStringA(" : ");
		OutputDebugStringA(pValue[i]);
		OutputDebugStringA("    ");
	}
	OutputDebugStringA("\n");
	return 0;
}
BOOL GetMaxCountDataBase(sqlite3 *db,int *nMaxCount)
{	
	int nRet = 0;
	char **dbResult;
	int nRow, nColumn;
	sqlite3_get_table(db,"select * from QuickKey",&dbResult,&nRow,&nColumn,NULL);
	if(nRow > 0 )
	{
		*nMaxCount = nRow;
		sqlite3_free_table(dbResult);
		return TRUE;
	}
	sqlite3_free_table(dbResult);
	nMaxCount = 0;
	return FALSE;
}
BOOL GetKeyToDataBase(sqlite3 *db,PQUICK_KEY_LIST pQuickList)
{
	char* errmsg;
	char **dbResult;
	int nRow, nColumn;
	int nRet = 0;
	int nNum = 1;
	nRet = sqlite3_get_table(db,"select * from QuickKey",&dbResult,&nRow,&nColumn,&errmsg);
	if(nRet == SQLITE_OK)
	{
		//dbResult1 前面第一行数据是字段名称，从 nColumn 索引开始才是真正的数据
		//dbResult1 的字段值是连续的，从第0索引到第 nColumn - 1索引都是字段名称，从第 nColumn 索引开始，
		//后面都是字段值，它把一个二维的表（传统的行列表示法）用一个扁平的形式来表示
		int index = nColumn;
		for(int i = 1; i <= nRow ; i++)
		{
			for(int j = 0 ; j < nColumn; j++)
			{
				OutputDebugStringA(dbResult[j]);
				OutputDebugStringA("    ");
			}
			OutputDebugStringA("\n");
		}
		//
		for(int i = 1; i <= nRow ; i++)
		{
			for(int j = nColumn; j < (nRow * nColumn + nColumn); j++)
			{
				OutputDebugStringA(dbResult[j]);
				OutputDebugStringA("    ");
			}
			OutputDebugStringA("\n");
		}
		for(int i = 0; i < nRow ; i++)
		{
			pQuickList->QuickKey[i].nId = atoi(dbResult[nNum * nColumn + 0]);

			pQuickList->QuickKey[i].nUserDefine = atoi(dbResult[nNum * nColumn + 1]);

			StringCchCopyA(pQuickList->QuickKey[i].KeyName,32,dbResult[nNum * nColumn + 2]);

			pQuickList->QuickKey[i].nKeyValue = atoi(dbResult[nNum * nColumn + 3]);

			pQuickList->QuickKey[i].SubKeyValue = *dbResult[nNum * nColumn + 4];

			pQuickList->nCurCount++;

			nNum++;
		}
		return TRUE;
	}
	sqlite3_free_table(dbResult);
	//不论数据库查询是否成功，都释放 char** 查询结果，使用 sqlite 提供的功能来释放
	return FALSE;
}
BOOL DeleteKeyToDataBase(sqlite3 *db,int nID)
{
	char* errmsg;
	int nRet = 0;
	char *szFormatSql = NULL;
	szFormatSql = (char*)malloc(MAX_PATH);
	if (szFormatSql == NULL)
	{
		OutputDebugString(TEXT("DeleteKeyToDataBase --> malloc failed!\n"));
		return FALSE;
	}
	RtlZeroMemory(szFormatSql,MAX_PATH);
	//"CREATE TABLE QuickKey(Id integer primary key autoincrement,UserDefine integer,KeyName varchar(32),KeyValue integer,SubKey varchar(32));"
	StringCchPrintfA(szFormatSql,MAX_PATH,"DELTE from QuickKey WHERE Id = %d;",nID);
	nRet = sqlite3_exec(db,szFormatSql,NULL,NULL,&errmsg);
	if (nRet == SQLITE_OK)
	{
		if (szFormatSql)
		{
			free(szFormatSql);
			szFormatSql = NULL;
		}
		sqlite3_free(errmsg);
		OutputDebugString(TEXT("Delete key successfully!\n"));
		return TRUE;
	}
	sqlite3_free(errmsg);
	if (szFormatSql)
	{
		free(szFormatSql);
		szFormatSql = NULL;
	}
	return FALSE;
}
BOOL UpdateKeyToDataBase(sqlite3 *db,int nID,int nUserDefine,char* szKeyName,int nKeyValue,char szSubKey)
{
	char* errmsg;
	int nRet = 0;
	char *szFormatSql = NULL;
	szFormatSql = (char*)malloc(MAX_PATH);
	if (szFormatSql == NULL)
	{
		OutputDebugString(TEXT("UpdateKeyToDataBase --> malloc failed!\n"));
		return FALSE;
	}
	RtlZeroMemory(szFormatSql,MAX_PATH);
	//"CREATE TABLE QuickKey(Id integer primary key autoincrement,UserDefine integer,KeyName varchar(32),KeyValue integer,SubKey varchar(32));"
	StringCchPrintfA(szFormatSql,MAX_PATH,"UPDATE QuickKey SET VALUES(%d,\'%s\',%d,\'%c\') WHERE Id = %d;",nUserDefine,szKeyName,nKeyValue,szSubKey,nID);
	nRet = sqlite3_exec(db,szFormatSql,NULL,NULL,&errmsg);
	if (nRet == SQLITE_OK)
	{
		if (szFormatSql)
		{
			free(szFormatSql);
			szFormatSql = NULL;
		}
		sqlite3_free(errmsg);
		OutputDebugString(TEXT("Update key successfully!\n"));
		return TRUE;
	}
	sqlite3_free(errmsg);
	if (szFormatSql)
	{
		free(szFormatSql);
		szFormatSql = NULL;
	}
	return FALSE;
}
BOOL SetKeyToDataBase(sqlite3 *db,int nUserDefine,char* szKeyName,int nKeyValue,char szSubKey)
{
	char* errmsg;
	int nRet = 0;
	char *szFormatSql = NULL;
	szFormatSql = (char*)malloc(MAX_PATH);
	if (szFormatSql == NULL)
	{
		OutputDebugString(TEXT("SetKeyToDataBase --> malloc failed!\n"));
		return FALSE;
	}
	RtlZeroMemory(szFormatSql,MAX_PATH);
	StringCchPrintfA(szFormatSql,MAX_PATH,"INSERT INTO QuickKey VALUES(1,%d,\'%s\',%d,\'%c\');",nUserDefine,szKeyName,nKeyValue,szSubKey);
	nRet = sqlite3_exec(db,szFormatSql,NULL,NULL,&errmsg);
	if (nRet == SQLITE_OK)
	{
		if (szFormatSql)
		{
			free(szFormatSql);
			szFormatSql = NULL;
		}
		sqlite3_free(errmsg);
		OutputDebugString(TEXT("Insert Key successfully!\n"));
		return TRUE;
	}
	sqlite3_free(errmsg);
	if (szFormatSql)
	{
		free(szFormatSql);
		szFormatSql = NULL;
	}
	return FALSE;
}