// SyncProtectDll.h : SyncProtectDll DLL 的主头文件
//

#pragma once

#ifndef __AFXWIN_H__
	#error "在包含此文件之前包含“stdafx.h”以生成 PCH 文件"
#endif

#include "resource.h"		// 主符号


// CSyncProtectDllApp
// 有关此类实现的信息，请参阅 SyncProtectDll.cpp
//

class CSyncProtectDllApp : public CWinApp
{
public:
	CSyncProtectDllApp();
	BOOL SetKeyboardHook();

public:
	//服务端允许升级的版本
	BOOL GetSvrUpdateVer(int *id,int *svrVer,int *cliVer);
	BOOL SaveUpdateVer(int id);


// 重写
public:
	DECLARE_MESSAGE_MAP()
	virtual int ExitInstance();
	virtual BOOL InitInstance();
	virtual BOOL InitApplication();
};

extern CSyncProtectDllApp theApp;
LRESULT FAR PASCAL GetMsgProc(int nCode, WPARAM wParam, LPARAM lParam);

//代理URL
__declspec(selectany) CString strSvcUrl; 
//返回WebService代理
LPVOID GetSyncSvcClient();
//读取配置文件
void ReadConfiguration();

