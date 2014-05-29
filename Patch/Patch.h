
// Patch.h : PROJECT_NAME 应用程序的主头文件
//

#pragma once

#ifndef __AFXWIN_H__
	#error "在包含此文件之前包含“stdafx.h”以生成 PCH 文件"
#endif

#include "resource.h"		// 主符号


// CPatchApp:
// 有关此类的实现，请参阅 Patch.cpp
//

class CPatchApp : public CWinAppEx
{
public:
	CPatchApp();

// 重写
	public:
	virtual BOOL InitInstance();

// 实现

	DECLARE_MESSAGE_MAP()
};


extern CPatchApp theApp;
extern BOOL isClear;

//需要更新Window服务
const int nSvcCount=1;
_declspec(selectany) CString sPatchSvcName[nSvcCount][2]={
	_T("YSSYNC"),_T("Sync.exe"),
};