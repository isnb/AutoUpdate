
// SyncProtect.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CSyncProtectApp:
// �йش����ʵ�֣������ SyncProtect.cpp
//

class CSyncProtectApp : public CWinApp
{
public:
	CSyncProtectApp();

// ��д
public:
	virtual BOOL InitInstance();

public:
	//����
	HANDLE hShareMemory;
	bool bShowWindow;
	//���Ͻ�������
	bool bStartUpdate;
	//��һ��
	bool bFirst;
// ʵ��
	DECLARE_MESSAGE_MAP()
	virtual int ExitInstance();
};

extern CSyncProtectApp theApp;