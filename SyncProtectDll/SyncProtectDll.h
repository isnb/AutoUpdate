// SyncProtectDll.h : SyncProtectDll DLL ����ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CSyncProtectDllApp
// �йش���ʵ�ֵ���Ϣ������� SyncProtectDll.cpp
//

class CSyncProtectDllApp : public CWinApp
{
public:
	CSyncProtectDllApp();
	BOOL SetKeyboardHook();

public:
	//��������������İ汾
	BOOL GetSvrUpdateVer(int *id,int *svrVer,int *cliVer);
	BOOL SaveUpdateVer(int id);


// ��д
public:
	DECLARE_MESSAGE_MAP()
	virtual int ExitInstance();
	virtual BOOL InitInstance();
	virtual BOOL InitApplication();
};

extern CSyncProtectDllApp theApp;
LRESULT FAR PASCAL GetMsgProc(int nCode, WPARAM wParam, LPARAM lParam);

//����URL
__declspec(selectany) CString strSvcUrl; 
//����WebService����
LPVOID GetSyncSvcClient();
//��ȡ�����ļ�
void ReadConfiguration();

