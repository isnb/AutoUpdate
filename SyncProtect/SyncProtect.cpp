
// SyncProtect.cpp : 定义应用程序的类行为。
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


// CSyncProtectApp 构造

CSyncProtectApp::CSyncProtectApp()
{
	// 支持重新启动管理器
	m_dwRestartManagerSupportFlags = AFX_RESTART_MANAGER_SUPPORT_RESTART;
	// TODO: 在此处添加构造代码，
	bShowWindow=FALSE;
	bStartUpdate=FALSE;
	hShareMemory=NULL;
	// 将所有重要的初始化放置在 InitInstance 中
}


// 唯一的一个 CSyncProtectApp 对象

CSyncProtectApp theApp;

HANDLE hMutexMuth=NULL;

//共享内存名称
const CString m_strMapFileName=_T("SyncProtectFileMap");

// CSyncProtectApp 初始化

BOOL CSyncProtectApp::InitInstance()
{
	// 如果一个运行在 Windows XP 上的应用程序清单指定要
	// 使用 ComCtl32.dll 版本 6 或更高版本来启用可视化方式，
	//则需要 InitCommonControlsEx()。否则，将无法创建窗口。
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// 将它设置为包括所有要在应用程序中使用的
	// 公共控件类。
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinApp::InitInstance();


	AfxEnableControlContainer();

	// 创建 shell 管理器，以防对话框包含
	// 任何 shell 树视图控件或 shell 列表视图控件。
	CShellManager *pShellManager = new CShellManager;

	// 标准初始化
	// 如果未使用这些功能并希望减小
	// 最终可执行文件的大小，则应移除下列
	// 不需要的特定初始化例程
	// 更改用于存储设置的注册表项
	// TODO: 应适当修改该字符串，
	// 例如修改为公司或组织名
	SetRegistryKey(_T("Yishion"));

	//通用权限
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
		// 创建共享文件。 
		hShareMemory=CreateFileMapping((HANDLE)0xFFFFFFFF,&sa, 
		PAGE_READWRITE,0,sizeof(ProcessExchangeData),m_strMapFileName);  	
	}
	else
	{
		lpAdr=MapViewOfFile(hShareMemory,FILE_MAP_ALL_ACCESS,0,0,0);
		memcpy(&pData,lpAdr,sizeof(ProcessExchangeData));
	}
	//命令行
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

	//双开限制
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
		// TODO: 在此放置处理何时用
		//  “确定”来关闭对话框的代码
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: 在此放置处理何时用
		//  “取消”来关闭对话框的代码
	}

	// 删除上面创建的 shell 管理器。
	if (pShellManager != NULL)
	{
		delete pShellManager;
	}

	// 由于对话框已关闭，所以将返回 FALSE 以便退出应用程序，
	//  而不是启动应用程序的消息泵。
	return FALSE;
}



int CSyncProtectApp::ExitInstance()
{
	// TODO: 在此添加专用代码和/或调用基类
	if(bFirst)
	{
		CloseHandle(hShareMemory);
		CloseHandle(hMutexMuth);
	}
	return CWinApp::ExitInstance();
}
