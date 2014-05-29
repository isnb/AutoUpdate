// SyncProtectDll.cpp : 定义 DLL 的初始化例程。
//

#include "stdafx.h"
#include "SyncProtectDll.h"
#include "SyncService.h"
#include "json\json.h"
#include "comutil.h"
#include "tinyxml/tinyxml.h"
#include "Extern.h"
#include "Utility.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


//using namespace Json;

//
//TODO: 如果此 DLL 相对于 MFC DLL 是动态链接的，
//		则从此 DLL 导出的任何调入
//		MFC 的函数必须将 AFX_MANAGE_STATE 宏添加到
//		该函数的最前面。
//
//		例如:
//
//		extern "C" BOOL PASCAL EXPORT ExportedFunction()
//		{
//			AFX_MANAGE_STATE(AfxGetStaticModuleState());
//			// 此处为普通函数体
//		}
//
//		此宏先于任何 MFC 调用
//		出现在每个函数中十分重要。这意味着
//		它必须作为函数中的第一个语句
//		出现，甚至先于所有对象变量声明，
//		这是因为它们的构造函数可能生成 MFC
//		DLL 调用。
//
//		有关其他详细信息，
//		请参阅 MFC 技术说明 33 和 58。
//
using namespace SyncService;

//显示窗口密码
CString strShowPassword;
//钩子处理消息循环
HHOOK hKeyHook=NULL;

// CSyncProtectDllApp

BEGIN_MESSAGE_MAP(CSyncProtectDllApp, CWinApp)
END_MESSAGE_MAP()


// CSyncProtectDllApp 构造

CSyncProtectDllApp::CSyncProtectDllApp()
{
	// TODO: 在此处添加构造代码，
	// 将所有重要的初始化放置在 InitInstance 中
}


// 唯一的一个 CSyncProtectDllApp 对象

CSyncProtectDllApp theApp;

LRESULT FAR PASCAL GetMsgProc(int nCode, WPARAM wParam, LPARAM lParam)
{
	LPMSG lpMsg = (LPMSG) lParam;

	if ( nCode >= 0 && PM_REMOVE == wParam )
	{
		if(lpMsg->message==WM_KEYUP)
		{
			if(lpMsg->wParam==VK_RETURN)
			{
				strShowPassword.Empty();
				lpMsg->message = WM_NULL;
				lpMsg->lParam  = 0;
				lpMsg->wParam  = 0;
			}
		}
		if(lpMsg->message==WM_CHAR){
			strShowPassword.AppendChar((TCHAR)lpMsg->wParam);
			if(strShowPassword.MakeUpper().Compare(_T("SHOW_DUAN_DUAN"))==0)
			{
				lpMsg->message = WM_NULL;
				lpMsg->lParam  = 0;
				lpMsg->wParam  = 0;
			}
		}
		//if ( (lpMsg->message >= WM_KEYFIRST && lpMsg->message <= WM_KEYLAST))
		//{
		//	if ( IsDialogMessage(theApp.GetMainWnd()->m_hWnd, lpMsg) )
		//	{
		//		// The value returned from this hookproc is ignored, 
		//		// and it cannot be used to tell Windows the message has been handled.
		//		// To avoid further processing, convert the message to WM_NULL 
		//		// before returning.
		//		//theApp.GetMainWnd()->PreTranslateMessage(lpMsg);

		//		lpMsg->message = WM_NULL;
		//		lpMsg->lParam  = 0;
		//		lpMsg->wParam  = 0;
		//	}
		//}
		/*if ((lpMsg->message >= WM_MOUSEFIRST && lpMsg->message <= WM_MOUSELAST))
		{
			IsDialogMessage(theApp.GetMainWnd()->m_hWnd, lpMsg);
		}*/
	}
	return CallNextHookEx(hKeyHook, nCode, wParam, lParam);
}

LPVOID GetSyncSvcClient()
{
	CSyncService *cli=new CSyncService;
	if(!strSvcUrl.IsEmpty())
	{
		cli->SetUrl(strSvcUrl.GetString());
	}
	return cli;
}

CString GetAppPath()
{
	CString strCurPath = _T("");  
	GetModuleFileName( NULL, strCurPath.GetBuffer(MAX_PATH), MAX_PATH );  
	strCurPath.ReleaseBuffer();  
	CString strPath = strCurPath.Left( strCurPath.ReverseFind(_T('\\')) + 1 );  
	return strPath.GetString();
}

void ReadConfiguration()
{
	TiXmlDocument doc;
	CString str=GetAppPath();
	str.Append(_T("\\Sync.exe.config"));
	FILE *file=_tfopen(str.GetString(),_T("rb"));
	if(file==NULL) return;
	doc.LoadFile(file,TIXML_ENCODING_UTF8);
	TiXmlElement *root=doc.RootElement();
	TiXmlNode *node=(root->FirstChild("appSettings"))->FirstChild("add");
	while(node)
	{
		TiXmlElement *e1=node->ToElement();
		if(e1==NULL)
		{
			node=node->NextSibling();
			continue;
		}
		const char *lpszKey=e1->Attribute("key");
		if(lpszKey==NULL) 
		{
				node=node->NextSibling();
				continue;
		}

		if(stricmp(lpszKey,"WebSvcUrl")==0)
		{
			const char *lpszVal=e1->Attribute("value");
			if(strlen(lpszVal)>0)
			{
				strSvcUrl=lpszVal;
			}
			break;
		}
		node=node->NextSibling();
		TRACE("%s\r\n",lpszKey);
	}
}

BOOL CSyncProtectDllApp::SetKeyboardHook()
{
	hKeyHook = SetWindowsHookEx(WH_KEYBOARD, GetMsgProc, theApp.m_hInstance, 0);
	if(hKeyHook==NULL)
		return FALSE;
	else
		return TRUE;
}


int CSyncProtectDllApp::ExitInstance()
{
	// TODO: 在此添加专用代码和/或调用基类
	//UnhookWindowsHookEx(hKeyHook);
	return CWinApp::ExitInstance();
}

BOOL CSyncProtectDllApp::GetSvrUpdateVer(int *id,int *svrVer,int *cliVer )
{
	BOOL ret=FALSE;
	CoInitialize(NULL);
	CSyncService *cli=(CSyncService *)GetSyncSvcClient();
	CComBSTR bResult;
	
	cli->GetCanUpdate(*id,(BSTR *)&bResult.m_str);
	if(bResult==NULL) return ret;
	Json::Reader r;

	Json::Value obj;
	char *lszTmp=_com_util::ConvertBSTRToString(bResult.m_str);
	if(r.parse(lszTmp,obj))
	{
		*svrVer=obj["S"].asInt();
		*cliVer=obj["C"].asInt();
		ret=TRUE;
	}
	else
	{
		*svrVer=0;
		*cliVer=0;
		ret=FALSE;
	}
	delete cli;
	CoUninitialize();
	return ret;
}

BOOL CSyncProtectDllApp::SaveUpdateVer( int id)
{
	bool ret=FALSE;
	CoInitialize(NULL);
	CSyncService *cli=(CSyncService *)GetSyncSvcClient();
	cli->SaveCanUpdate(id,&ret);
	delete cli;
	CoUninitialize();
	return ret;
}


BOOL CSyncProtectDllApp::InitInstance()
{
	// TODO: 在此添加专用代码和/或调用基类
	ReadConfiguration();
	CUtility::Initialize();
	return CWinApp::InitInstance();
}


BOOL CSyncProtectDllApp::InitApplication()
{
	// TODO: 在此添加专用代码和/或调用基类
	return CWinApp::InitApplication();
}
