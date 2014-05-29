// SyncProtectDll.cpp : ���� DLL �ĳ�ʼ�����̡�
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
//TODO: ����� DLL ����� MFC DLL �Ƕ�̬���ӵģ�
//		��Ӵ� DLL �������κε���
//		MFC �ĺ������뽫 AFX_MANAGE_STATE ����ӵ�
//		�ú�������ǰ�档
//
//		����:
//
//		extern "C" BOOL PASCAL EXPORT ExportedFunction()
//		{
//			AFX_MANAGE_STATE(AfxGetStaticModuleState());
//			// �˴�Ϊ��ͨ������
//		}
//
//		�˺������κ� MFC ����
//		������ÿ��������ʮ����Ҫ������ζ��
//		��������Ϊ�����еĵ�һ�����
//		���֣������������ж������������
//		������Ϊ���ǵĹ��캯���������� MFC
//		DLL ���á�
//
//		�й�������ϸ��Ϣ��
//		����� MFC ����˵�� 33 �� 58��
//
using namespace SyncService;

//��ʾ��������
CString strShowPassword;
//���Ӵ�����Ϣѭ��
HHOOK hKeyHook=NULL;

// CSyncProtectDllApp

BEGIN_MESSAGE_MAP(CSyncProtectDllApp, CWinApp)
END_MESSAGE_MAP()


// CSyncProtectDllApp ����

CSyncProtectDllApp::CSyncProtectDllApp()
{
	// TODO: �ڴ˴���ӹ�����룬
	// ��������Ҫ�ĳ�ʼ�������� InitInstance ��
}


// Ψһ��һ�� CSyncProtectDllApp ����

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
	// TODO: �ڴ����ר�ô����/����û���
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
	// TODO: �ڴ����ר�ô����/����û���
	ReadConfiguration();
	CUtility::Initialize();
	return CWinApp::InitInstance();
}


BOOL CSyncProtectDllApp::InitApplication()
{
	// TODO: �ڴ����ר�ô����/����û���
	return CWinApp::InitApplication();
}
