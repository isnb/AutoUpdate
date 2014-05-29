
// Patch.cpp : ����Ӧ�ó��������Ϊ��
//

#include "stdafx.h"
#include "Patch.h"
#include "PatchDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CPatchApp

BEGIN_MESSAGE_MAP(CPatchApp, CWinAppEx)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


// CPatchApp ����

CPatchApp::CPatchApp()
{
	// TODO: �ڴ˴���ӹ�����룬
	// ��������Ҫ�ĳ�ʼ�������� InitInstance ��
}


// Ψһ��һ�� CPatchApp ����

CPatchApp theApp;
BOOL isClear=FALSE;


// CPatchApp ��ʼ��

BOOL CPatchApp::InitInstance()
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

	CWinAppEx::InitInstance();

	AfxEnableControlContainer();

	// ��׼��ʼ��
	// ���δʹ����Щ���ܲ�ϣ����С
	// ���տ�ִ���ļ��Ĵ�С����Ӧ�Ƴ�����
	// ����Ҫ���ض���ʼ������
	// �������ڴ洢���õ�ע�����
	// TODO: Ӧ�ʵ��޸ĸ��ַ�����
	// �����޸�Ϊ��˾����֯��
	SetRegistryKey(_T("Ӧ�ó��������ɵı���Ӧ�ó���"));

	LPTSTR lptzComm=GetCommandLine();
	int nComm=1;
	LPWSTR *comm=CommandLineToArgvW(lptzComm,&nComm);
	if(nComm>1 && wcsicmp(comm[1],L"-d")==0)
	{
		isClear=TRUE;
	}
	else if(nComm>1 && wcsicmp(comm[1],L"-s")==0)
	{
		if(nComm>=4){
		//������
		 sPatchSvcName[0][0]=comm[2];
		 //������
		 sPatchSvcName[0][1]=comm[3];
		}
	}

	CPatchDlg dlg;
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

	// ���ڶԻ����ѹرգ����Խ����� FALSE �Ա��˳�Ӧ�ó���
	//  ����������Ӧ�ó������Ϣ�á�
	return FALSE;
}
