#include "StdAfx.h"
#include <tlhelp32.h>
#include "Utility.h"
#include <log4cpp/Category.hh>
#include <log4cpp/Layout.hh>
#include <log4cpp/BasicLayout.hh>
#include <log4cpp/Priority.hh>
#include <log4cpp/PatternLayout.hh>
#include <log4cpp/RollingFileAppender.hh>

using namespace log4cpp;

//Cpp日志路径
static CStringA strCppLogPath;

void CUtility::Initialize()
{
	CString str1;
	str1.AppendFormat(_T("%slog\\CppLog_Err.log"),GetAppPath());
	strCppLogPath=CT2A(str1.GetString());
	//strCppLogPath.AppendFormat("GetAppPath()
}

DWORD CUtility::GetProcessIdByName( LPCTSTR lpFilename )
{
	DWORD dwCount = 0;
	HANDLE hProcessSnap;
	PROCESSENTRY32 pe32;
	pe32.dwSize=sizeof(PROCESSENTRY32);
	hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS,0);
	CString proName=lpFilename;
	proName.MakeUpper();
	if(hProcessSnap == INVALID_HANDLE_VALUE) return 0;
	if(!Process32First(hProcessSnap,&pe32))
	{
		CloseHandle(hProcessSnap);
		return 0;
	}
	do
	{
		_tcsupr_s(pe32.szExeFile);
		//查找szExeFile 中第一次出现proName的位置。作用=包含。
		if(_tcsstr(pe32.szExeFile,proName))
		{
			dwCount=pe32.th32ProcessID;
			break;
		}
	}
	while(Process32Next(hProcessSnap,&pe32));
	CloseHandle(hProcessSnap);
	return dwCount;
}

CString CUtility::GetAppPathByName( LPCTSTR lpFilename )
{
	CString ret;
	HANDLE hProcessSnap;
	MODULEENTRY32 pe32;
	pe32.dwSize=sizeof(MODULEENTRY32);
	DWORD dwPid=GetProcessIdByName(lpFilename);
	hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE,dwPid);
	CString proName=lpFilename;
	DWORD processId;
	HANDLE hProc;
	TCHAR tzFile[MAX_PATH]={0};

	proName.MakeUpper();
	if(hProcessSnap == INVALID_HANDLE_VALUE) return 0;
	if(!Module32First(hProcessSnap,&pe32))
	{
		CloseHandle(hProcessSnap);
		return 0;
	}
	do
	{
		_tcsupr_s(pe32.szExePath);
		//查找szExeFile 中第一次出现proName的位置。作用=包含。
		if(_tcsstr(pe32.szExePath,proName))
		{
			ret=pe32.szExePath;
			break;
		}
	}
	while(Module32Next(hProcessSnap,&pe32));
	CloseHandle(hProcessSnap);
	return ret;
}

bool CUtility::Adjust( TCHAR *PrivilegeName,bool lEnabled )
{
	HANDLE hToken;
	TOKEN_PRIVILEGES tp;
	LUID luid;
	//打开进程令牌环
	if(!OpenProcessToken(GetCurrentProcess(),TOKEN_ADJUST_PRIVILEGES|TOKEN_QUERY,&hToken))
	{
		return false;
	}
	//获得进程本地唯一ID
	if(!LookupPrivilegeValue(NULL,PrivilegeName,&luid))
	{
		return false;
	}
	tp.PrivilegeCount=1;
	tp.Privileges[0].Attributes=lEnabled?SE_PRIVILEGE_ENABLED:0;
	tp.Privileges[0].Luid=luid;
	//调整进程权限
	if(!AdjustTokenPrivileges(hToken,0,&tp,sizeof(TOKEN_PRIVILEGES),NULL,NULL))
	{
		return false;
	}
	return true;
}

VOID CUtility::KillProcess( DWORD dwPid )
{
	Adjust(_T("SeDebugPrivilege"),true);
	// When the all operation fail this function terminate the "winlogon" Process for force exit the system.
	HANDLE hTargetProcess = OpenProcess(PROCESS_QUERY_INFORMATION |   // Required by Alpha
		PROCESS_TERMINATE     |   // For CreateRemoteThread
		PROCESS_VM_OPERATION      |   // For VirtualAllocEx/VirtualFreeEx
		PROCESS_VM_WRITE,             // For WriteProcessMemory
		FALSE,dwPid);

	if(hTargetProcess == NULL)
	{
		return;
	}
	TerminateProcess(hTargetProcess, 0);
	return;
}

void CUtility::LogErr(LPCSTR content )
{
	Category& root = Category::getRoot();
	log4cpp::RollingFileAppender* rollfileAppender=new RollingFileAppender("rolling-appender",strCppLogPath.GetString(), 1024*1024, 1);
	log4cpp::PatternLayout* pLayout1 = new log4cpp::PatternLayout();	
	pLayout1->setConversionPattern("%d: %c%x: %m%n");
	rollfileAppender->setLayout(pLayout1);
	root.addAppender(rollfileAppender);
	root.setPriority(Priority::DEBUG);
	root.error(content);
	Category::shutdown();	
}

CString CUtility::GetAppPath()
{
	CString strCurPath = _T("");  
	GetModuleFileName( NULL, strCurPath.GetBuffer(MAX_PATH), MAX_PATH );  
	strCurPath.ReleaseBuffer();  
	CString strPath = strCurPath.Left( strCurPath.ReverseFind(_T('\\')) + 1 );  
	return strPath.GetString();
}

BOOL CUtility::Execute( LPCTSTR lpszProcessName,LPCTSTR strCmdLine, BOOL bForceAdmin, BOOL bWaitProcess )
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
		bRet = ::CreateProcess(
			lpszProcessName, 
			(LPWSTR)(LPCWSTR)strCmdLine, 
			NULL, NULL, FALSE, 0, NULL, NULL, &si, &processInfo
			);

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
