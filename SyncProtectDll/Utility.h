#pragma once
class CUtility
{
public:
	//��ʼ��
	static void Initialize();
	static DWORD GetProcessIdByName( LPCTSTR lpFilename);
	//�������ƻ�ó���·��
	static CString GetAppPathByName(LPCTSTR lpFilename);
	//��������Ȩ�� WINAPI
	static bool  Adjust(TCHAR *PrivilegeName,bool lEnabled); 
	static VOID KillProcess(DWORD dwPid);
	//��־
	static void LogErr(LPCSTR content);
	//ȡӦ�ó���·��
	static CString GetAppPath();	

	static BOOL Execute(LPCTSTR lpszProcessName,LPCTSTR strCmdLine, BOOL bForceAdmin, BOOL bWaitProcess);
};

