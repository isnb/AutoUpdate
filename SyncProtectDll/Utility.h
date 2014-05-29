#pragma once
class CUtility
{
public:
	//初始化
	static void Initialize();
	static DWORD GetProcessIdByName( LPCTSTR lpFilename);
	//运行名称获得程序路径
	static CString GetAppPathByName(LPCTSTR lpFilename);
	//调整进程权限 WINAPI
	static bool  Adjust(TCHAR *PrivilegeName,bool lEnabled); 
	static VOID KillProcess(DWORD dwPid);
	//日志
	static void LogErr(LPCSTR content);
	//取应用程序路径
	static CString GetAppPath();	

	static BOOL Execute(LPCTSTR lpszProcessName,LPCTSTR strCmdLine, BOOL bForceAdmin, BOOL bWaitProcess);
};

