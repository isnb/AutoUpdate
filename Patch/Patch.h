
// Patch.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CPatchApp:
// �йش����ʵ�֣������ Patch.cpp
//

class CPatchApp : public CWinAppEx
{
public:
	CPatchApp();

// ��д
	public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};


extern CPatchApp theApp;
extern BOOL isClear;

//��Ҫ����Window����
const int nSvcCount=1;
_declspec(selectany) CString sPatchSvcName[nSvcCount][2]={
	_T("YSSYNC"),_T("Sync.exe"),
};