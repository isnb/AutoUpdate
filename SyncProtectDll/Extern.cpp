#include "stdafx.h"
#include "Extern.h"
#include "SyncProtectDll.h"
#include "../SyncProtect/ProcessExchangeData.h"
#include "SyncService.h"
#include "comutil.h"
#include "Utility.h"

using namespace SyncService;

extern CSyncProtectDllApp theApp;

#pragma data_seg("YishionSyncShare")
int BranchId=7297;
#pragma data_seg()
#pragma comment(linker,"/SECTION:YishionSyncShare,RWS")

//共享内存名称
const CString m_strMapFileName=_T("SyncProtectFileMap");
//共享
HANDLE hShareMemory;

CString ParseInstr(LPCSTR instr,OUT int *id,OUT short *flag);

BOOL SetKeyboardHook()
{
	return theApp.SetKeyboardHook();
}

CDWPI BOOL GetSvrUpdateVer(int *id,int *sv,int *cv)
{
	return theApp.GetSvrUpdateVer(id,sv,cv);
}

CDWPI BOOL SaveSvrUpdateVer( int id )
{
	return theApp.SaveUpdateVer(id);
}

CDWPI void GetShareData( LPVOID lpData )
{
	hShareMemory=OpenFileMapping(FILE_MAP_ALL_ACCESS,FALSE,m_strMapFileName);
	LPVOID lpAdr=NULL;

	if(lpData==NULL) 
	{
		return;
	}
	if(hShareMemory!=NULL)
	{
		lpAdr=MapViewOfFile(hShareMemory,FILE_MAP_ALL_ACCESS,0,0,0);
		memcpy_s(lpData,sizeof(ProcessExchangeData),lpAdr,sizeof(ProcessExchangeData));  
		UnmapViewOfFile(lpAdr);
		//_tcscpy_s((LPTSTR)lpData,30,_T("k3"));
	}
}

CDWPI void SetShareData( LPVOID lpData )
{
	hShareMemory=OpenFileMapping(FILE_MAP_ALL_ACCESS,FALSE,m_strMapFileName);
	LPVOID lpAdr=NULL;
	if(lpData==NULL) return;
	if(hShareMemory!=NULL)
	{
		lpAdr=MapViewOfFile(hShareMemory,FILE_MAP_ALL_ACCESS,0,0,0);
		memcpy_s(lpAdr,sizeof(ProcessExchangeData),lpData,sizeof(ProcessExchangeData));  
		UnmapViewOfFile(lpAdr);
	}
}


CDWPI void SetBranchId( int id )
{
	BranchId=id;
}

CDWPI int GetBranchId()
{
	return BranchId;
}

CDWPI void ParseInstruction()
{
	CComBSTR result;
	CString sResult;
	int id=0;
	short flag=1;

	CoInitialize(NULL);
	CSyncService *cli=(CSyncService *)GetSyncSvcClient();
	cli->GetInstruction(BranchId,&result);
	if(result ==NULL) return;

	char *lszTmp=_com_util::ConvertBSTRToString(result);
	sResult=ParseInstr(lszTmp,&id,&flag);
	result=sResult.GetString();
	cli->UpdateInstruction(id,flag,result);

	delete cli;
	CoUninitialize();
}

CDWPI void LogErr( LPCSTR msg )
{
	CUtility::LogErr(msg);
}

