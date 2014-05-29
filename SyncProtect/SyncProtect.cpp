
// SyncProtect.cpp : ����Ӧ�ó��������Ϊ��
//

#include "stdafx.h"
#include "SyncProtect.h"
#include "SyncProtectDlg.h"
#include "ProcessExchangeData.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CSyncProtectApp

BEGIN_MESSAGE_MAP(CSyncProtectApp, CWinApp)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


// CSyncProtectApp ����

CSyncProtectApp::CSyncProtectApp()
{
	// ֧����������������
	m_dwRestartManagerSupportFlags = AFX_RESTART_MANAGER_SUPPORT_RESTART;
	// TODO: �ڴ˴���ӹ�����룬
	bShowWindow=FALSE;
	bStartUpdate=FALSE;
	hShareMemory=NULL;
	// ��������Ҫ�ĳ�ʼ�������� InitInstance ��
}


// Ψһ��һ�� CSyncProtectApp ����

CSyncProtectApp theApp;

HANDLE hMutexMuth=NULL;

//�����ڴ�����
const CString m_strMapFileName=_T("SyncProtectFileMap");

// CSyncProtectApp ��ʼ��

BOOL CSyncProtectApp::InitInstance()
{
	// ���һ�������� Windows XP �ϵ�Ӧ�ó����嵥ָ��Ҫ
	// ʹ�� ComCtl32.dll �汾 6 ����߰汾�����ÿ��ӻ���ʽ��
	//����Ҫ InitCommonControlsEx()�����򣬽��޷��������ڡ�
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// ��������Ϊ��������Ҫ��Ӧ�ó�����ʹ�õ�
	// �����ؼ��ࡣ
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinApp::InitInstance();


	AfxEnableControlContainer();

	// ���� shell ���������Է��Ի������
	// �κ� shell ����ͼ�ؼ��� shell �б���ͼ�ؼ���
	CShellManager *pShellManager = new CShellManager;

	// ��׼��ʼ��
	// ���δʹ����Щ���ܲ�ϣ����С
	// ���տ�ִ���ļ��Ĵ�С����Ӧ�Ƴ�����
	// ����Ҫ���ض���ʼ������
	// �������ڴ洢���õ�ע�����
	// TODO: Ӧ�ʵ��޸ĸ��ַ�����
	// �����޸�Ϊ��˾����֯��
	SetRegistryKey(_T("Yishion"));

	//ͨ��Ȩ��
	SECURITY_ATTRIBUTES sa;
	SECURITY_DESCRIPTOR sd;
	InitializeSecurityDescriptor(&sd,SECURITY_DESCRIPTOR_REVISION);
	SetSecurityDescriptorDacl(&sd,TRUE,NULL,FALSE);
	sa.nLength = sizeof(SECURITY_ATTRIBUTES);
	sa.bInheritHandle = TRUE;
	sa.lpSecurityDescriptor = &sd;

	ProcessExchangeData pData;
	LPVOID lpAdr=NULL;
	hShareMemory=OpenFileMapping(FILE_MAP_ALL_ACCESS,FALSE,m_strMapFileName);
	if(hShareMemory==NULL)
	{
		// ���������ļ��� 
		hShareMemory=CreateFileMapping((HANDLE)0xFFFFFFFF,&sa, 
		PAGE_READWRITE,0,sizeof(ProcessExchangeData),m_strMapFileName);  	
	}
	else
	{
		lpAdr=MapViewOfFile(hShareMemory,FILE_MAP_ALL_ACCESS,0,0,0);
		memcpy(&pData,lpAdr,sizeof(ProcessExchangeData));
	}
	//������
	LPWSTR lpwzComm=GetCommandLine();
	int argc=0;
	LPWSTR * lpwzArgv=CommandLineToArgvW(lpwzComm,&argc);
	if (argc == 2) {
		if(_wcsicmp(lpwzArgv[1],_T("Hide"))==0)
			bShowWindow=FALSE;
		else if(_wcsicmp(lpwzArgv[1],_T("Au"))==0)
		{
			pData.IsStartAutoUpdate=TRUE;
			bStartUpdate=TRUE;
		}
	}

	//˫������
	hMutexMuth=OpenMutex(MUTEX_ALL_ACCESS,TRUE,_T("SyncProtect"));
	if(hMutexMuth==NULL)
		hMutexMuth=CreateMutex(&sa,TRUE,_T("SyncProtect"));
	else
	{
		goto Two;
	}
	DWORD err=GetLastError();
	if(err==ERROR_ALREADY_EXISTS || err==ERROR_ACCESS_DENIED)
	{
		Two:
		pData.IsShowWindow=TRUE;
		memcpy(lpAdr,&pData,sizeof(ProcessExchangeData));
		UnmapViewOfFile(lpAdr);
		CloseHandle(hMutexMuth);
		hMutexMuth=NULL;
		bFirst=FALSE;
		return FALSE;
	}
	
	bFirst=TRUE;
	CSyncProtectDlg dlg;
	m_pMainWnd = &dlg;

	INT_PTR nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO: �ڴ˷��ô����ʱ��
		//  ��ȷ�������رնԻ���Ĵ���
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: �ڴ˷��ô����ʱ��
		//  ��ȡ�������رնԻ���Ĵ���
	}

	// ɾ�����洴���� shell ��������
	if (pShellManager != NULL)
	{
		delete pShellManager;
	}

	// ���ڶԻ����ѹرգ����Խ����� FALSE �Ա��˳�Ӧ�ó���
	//  ����������Ӧ�ó������Ϣ�á�
	return FALSE;
}



int CSyncProtectApp::ExitInstance()
{
	// TODO: �ڴ����ר�ô����/����û���
	if(bFirst)
	{
		CloseHandle(hShareMemory);
		CloseHandle(hMutexMuth);
	}
	return CWinApp::ExitInstance();
}
