#include "stdafx.h"
#include "Extern.h"
#include "AutoUpdateCommon.h"
#include "common/utility.h"                /* for GetLeidianLogPath */
#include "Tlhelp32.h"

int DownloadFile(LPTSTR svrPath,LPTSTR savePath)
{
	return theApp.DownloadFile(svrPath,savePath,FALSE,0);
}

void CheckVersion(BOOL *pbFindVersion )
{
	theApp.CheckVersion(pbFindVersion);
}

void SetLogFn( LPVOID lpFn )
{
	Log1=(LogFn)lpFn;
}

void GetAppPath(LPTSTR lpAppPath)
{
	CString str;
	CAppPath::Instance().GetPath(str);
	_tcscpy_s(lpAppPath,MAX_PATH,str.GetBuffer());
}

void SetUpdateAddress( LPTSTR lpAppPath )
{
	theApp.SetUpdateAddress(lpAppPath);
}

void SetGetModuleFn( LPVOID lpFn )
{
	GetModule1=(GetModuleFn)lpFn;
}

BOOL Execute( LPCTSTR lpszProcessName,LPCTSTR strCmdLine, BOOL bForceAdmin, BOOL bWaitProcess)
{
	BOOL bRet = FALSE;
	PROCESS_INFORMATION processInfo;
	STARTUPINFO si = {sizeof(STARTUPINFO)};
	HANDLE hProcess = NULL;

	if (bForceAdmin)
	{
		SHELLEXECUTEINFO sei = { sizeof(SHELLEXECUTEINFO) };

		sei.fMask = SEE_MASK_NOCLOSEPROCESS;
		sei.hwnd = NULL;
		sei.lpVerb = _T("runas");
		sei.lpFile = lpszProcessName;
		sei.lpParameters = (LPWSTR)(LPCWSTR)strCmdLine;
		sei.nShow = SW_SHOWNORMAL;

		bRet = ::ShellExecuteEx(&sei);

		hProcess = sei.hProcess;
	}
	else
	{

		HANDLE   hToken; 
		if(!GetTokenByName(hToken, _T("EXPLORER.EXE"))) 
		{ 
			return   FALSE; 
		} 


		/*ZeroMemory(&si,   sizeof(STARTUPINFO)); 
		si.cb                 =   sizeof(STARTUPINFO); 
		si.lpDesktop   =   _T("winsta0\\default");*/
/*
		TCHAR   szParameter[10]   =   {0}; */

		bRet=CreateProcessAsUser(hToken,lpszProcessName,(LPWSTR)(LPCWSTR)strCmdLine,
			NULL, NULL,FALSE,0,NULL,NULL,&si,&processInfo); 
		


		//bRet = ::CreateProcess(
		//	lpszProcessName, 
		//	(LPWSTR)(LPCWSTR)strCmdLine, 
		//	NULL, NULL, FALSE, 0, NULL, NULL, &si, &processInfo
		//	);

		if (bRet)
		{
			::CloseHandle(processInfo.hThread);
			hProcess = processInfo.hProcess;
		}
	}

	if (bRet)
	{
		if (bWaitProcess)
		{
			::WaitForSingleObject(hProcess, INFINITE);
		}
		::CloseHandle(hProcess);
	}

	return bRet;
}


BOOL GetTokenByName(HANDLE   &hToken,LPTSTR   lpName) 
{ 
	if(!lpName) 
	{ 
		return   FALSE; 
	} 
	HANDLE                   hProcessSnap   =   NULL;   
	BOOL                       bRet             =   FALSE;   
	PROCESSENTRY32   pe32             =   {0};   

	hProcessSnap   =   CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS,   0); 
	if   (hProcessSnap   ==   INVALID_HANDLE_VALUE)   
		return   (FALSE);   

	pe32.dwSize   =  sizeof(PROCESSENTRY32);   
	if   (Process32First(hProcessSnap,   &pe32))   
	{ 
		do   
		{ 
			if(!_tcscmp(_tcsupr(pe32.szExeFile),_tcsupr(lpName))) 
			{ 
				HANDLE hProcess= OpenProcess(PROCESS_QUERY_INFORMATION, 
					FALSE,pe32.th32ProcessID); 
				bRet   =   OpenProcessToken(hProcess,TOKEN_ALL_ACCESS,&hToken); 
				CloseHandle   (hProcessSnap);   
				return   (bRet); 
			} 
		}   
		while   (Process32Next(hProcessSnap,   &pe32));   
		bRet   =   TRUE;   
	}   
	else   
		bRet   =   FALSE; 

	CloseHandle   (hProcessSnap);   
	return   (bRet); 
} 


int GetHashCode( LPTSTR lpStr )
{
	int ret;
	const char* chPtr = (char *)lpStr;
	int num = 0x15051505;
	int num2 = num;
	int* numPtr = (int*) chPtr;
	for (int i = _tcslen(lpStr); i > 0; i -= 4)
	{
		num = (((num << 5) + num) + (num >> 0x1b)) ^ numPtr[0];
		if (i <= 2)
		{
			break;
		}
		num2 = (((num2 << 5) + num2) + (num2 >> 0x1b)) ^ numPtr[1];
		numPtr += 2;
	}
	ret=num + (num2 * 0x5d588b65);
	return ret;
}

void GetCrcInfo( BOOL *checkup,LPTSTR lptzInfo,int size)
{
	return theApp.GetCrcInfo(checkup,lptzInfo,size);
}

bool DownloadDiffFile()
{
	HRESULT hr=theApp.DownloadDiffFile();
	if(hr==S_OK)
	{
		return TRUE;
	}
	else
		return FALSE;
}



