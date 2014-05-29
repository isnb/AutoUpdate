
// SyncProtect.h : PROJECT_NAME 应用程序的主头文件
//

#pragma once

#ifndef __AFXWIN_H__
	#error "在包含此文件之前包含“stdafx.h”以生成 PCH 文件"
#endif

#include "resource.h"		// 主符号


// CSyncProtectApp:
// 有关此类的实现，请参阅 SyncProtect.cpp
//

class CSyncProtectApp : public CWinApp
{
public:
	CSyncProtectApp();

// 重写
public:
	virtual BOOL InitInstance();

public:
	//共享
	HANDLE hShareMemory;
	bool bShowWindow;
	//马上进行升级
	bool bStartUpdate;
	//第一次
	bool bFirst;
// 实现
	DECLARE_MESSAGE_MAP()
	virtual int ExitInstance();
};

extern CSyncProtectApp theApp;