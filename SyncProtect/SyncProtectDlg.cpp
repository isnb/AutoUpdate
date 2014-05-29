
// SyncProtectDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "SyncProtect.h"
#include "SyncProtectDlg.h"
#include "afxdialogex.h"
#include "winsvc.h"
#include "Utility.h"
#include "time.h"
#include "ProcessExchangeData.h"
#include "../SyncProtectDll/Extern.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


extern "C" _declspec(dllimport) void DownloadFile(LPTSTR svrPath,LPTSTR savePath);
extern "C" _declspec(dllimport) void CheckVersion(int id,BOOL *pbFindVersion);
extern "C" _declspec(dllimport) void SetLogFn(LPVOID lpFn);
extern "C" _declspec(dllimport) void GetAppPath(LPTSTR lpAppPath);
extern "C" _declspec(dllimport) BOOL Execute(LPCTSTR lpszProcessName,LPCTSTR strCmdLine, BOOL bForceAdmin, BOOL bWaitProcess);
EXTERN_C _declspec(dllimport) int GetCrcInfo(BOOL *checkup,LPTSTR lptzHint,int size);
EXTERN_C _declspec(dllimport) bool DownloadDiffFile();

//SyncProtectDll
//EXTERN_C _declspec(dllexport) BOOL SetKeyboardHook();


const CString  sUpdateDir=_T("Update");
//服务名
const CString sServiceName=_T("YSSYNC");

//共享资源锁
HANDLE hLkShare=NULL;
;

//计时器事件ID
const int nEventIdGetShare=1;
const int nEventIdAu=2;

//计时器时间间隔
const int nTimespanGetShare=30000;
HANDLE hLkTimerGetShare=NULL;
const int nTimespanAu=1800000;
HANDLE hLkTimerAu=NULL;

CString sAppPath;

//提示次数
const int nHintCount=10;
	 
// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CSyncProtectDlg 对话框

CSyncProtectDlg::CSyncProtectDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CSyncProtectDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	hLkShare=CreateEvent(NULL,NULL,TRUE,NULL);
	hLkTimerGetShare=CreateEvent(NULL,TRUE,TRUE,_T("GetShareData"));
	hLkTimerAu=CreateEvent(NULL,TRUE,TRUE,_T("Au"));
}

void CSyncProtectDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_HINT, m_edtHint);
}

BEGIN_MESSAGE_MAP(CSyncProtectDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BTN_AUTO_UPDATE, &CSyncProtectDlg::OnBnClickedBtnAutoUpdate)
	ON_BN_CLICKED(IDC_BTN_START_SVC, &CSyncProtectDlg::OnBnClickedBtnStartSvc)
	ON_MESSAGE(WM_UPDATE_HINT, &CSyncProtectDlg::OnUpdateHint)
	ON_BN_CLICKED(IDC_BTN_STOP_SVC, &CSyncProtectDlg::OnBnClickedBtnStopSvc)
	ON_WM_NCPAINT()
	ON_WM_TIMER()
	ON_WM_GETMINMAXINFO()
END_MESSAGE_MAP()


// CSyncProtectDlg 消息处理程序

BOOL CSyncProtectDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标
	
	// TODO: 在此添加额外的初始化代码

	TCHAR tz1[MAX_PATH]={0};
	GetAppPath(tz1);
	sAppPath=tz1;
	Log=WriteLog;
	SetLogFn(WriteLog);
	SetShareData1();

	PProcessExchangeData lpData1=new ProcessExchangeData;
	int s=sizeof(ProcessExchangeData);
	//GetSvrUpdateVer();

	//CoInitialize(NULL);
	//CSyncServiceT<> cli;
	//double o=0;
	//cli.Add(1,2,&o);
	//CoUninitialize();

	//  add; 
	  //_ns1__AddResponse addResponse ; 
	  //add.x = &dx; 
	  //add.y = &dy; 
	  //BasicHttpBinding_USCORECalculatorService q;       //代理类 
	  //double result = q.__ns1__Add(&add,&addResponse); 
	  //if(result == SOAP_OK)    
	  //{ 
		 // dResult = *addResponse.AddResult; 
	  //} 
	  //else
		 // soap_print_fault(q.soap,stderr);

	SetTimer(nEventIdGetShare,nTimespanGetShare,NULL);
	SetTimer(nEventIdAu,nTimespanAu,NULL);
	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CSyncProtectDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CSyncProtectDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文
		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);
		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CSyncProtectDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CSyncProtectDlg::OnOK()
{
	// TODO: 在此添加专用代码和/或调用基类

	//CDialogEx::OnOK();
}


void CSyncProtectDlg::OnCancel()
{
	// TODO: 在此添加专用代码和/或调用基类
	theApp.bShowWindow=FALSE;
	SetShareData1();
	//CDialogEx::OnCancel();
}




void CSyncProtectDlg::CloseDlg()
{
	CDialogEx::OnCancel();
}


BOOL CSyncProtectDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 在此添加专用代码和/或调用基类
	if(pMsg->message==WM_KEYUP)
	{
		if(pMsg->wParam==VK_RETURN)
		{
			strExitPassword.Empty();
		}
	}
	if(pMsg->message==WM_CHAR){
		strExitPassword.AppendChar((TCHAR)pMsg->wParam);
		if(strExitPassword.MakeUpper().Compare(_T("DUAN"))==0)
		{
			CDialogEx::OnCancel();
		}
	}

	return CDialogEx::PreTranslateMessage(pMsg);
}

//自动更新
void CSyncProtectDlg::OnBnClickedBtnAutoUpdate()
{
	AfxBeginThread(DoAutoUpdate,NULL);
}


void CALLBACK WriteLog(LPTSTR lpMsg)
{
	CSyncProtectDlg *pDlg=NULL;
	pDlg=(CSyncProtectDlg*)theApp.GetMainWnd();
	if(pDlg!=NULL)
	{
		pDlg->SendMessage(WM_UPDATE_HINT,(WPARAM)lpMsg,0);
	}
}

UINT DoAutoUpdate(LPVOID lp1)
{
	BOOL bFind=FALSE;
	BOOL bCheck=FALSE;
	TCHAR *lptzPara=NULL;
	int nRetry=0;
	int nMaxRetry=5;
	TCHAR tz1[MAX_PATH]={0};
	CString sTmp;

	if(WaitForSingleObject(hLkTimerAu,0)==WAIT_OBJECT_0)
	{
		ResetEvent(hLkTimerAu);
		
		CSyncProtectDlg *pDlg=(CSyncProtectDlg*)theApp.GetMainWnd();
		int nB=GetBranchId();
		if(nB>0)
		{
			CheckVersion(nB,&bFind);
			if(bFind)
			{
				Execute(sAppPath+_T("\\Patch.exe"),NULL,FALSE,FALSE);
				pDlg->CloseDlg();
			}
		}
	
		SetEvent(hLkTimerAu);
	}
	return 1;
}

UINT DoParseInstruction( LPVOID lp )
{
	if(WaitForSingleObject(hLkTimerGetShare,0)==WAIT_OBJECT_0)
	{
		ResetEvent(hLkTimerGetShare);
		CSyncProtectDlg *pDlg=(CSyncProtectDlg*)theApp.GetMainWnd();
		pDlg->GetShareData1();
		ParseInstruction();
		SetEvent(hLkTimerGetShare);
	}
	return 1;
}


 void CSyncProtectDlg::OnBnClickedBtnStartSvc()
 {
	 AfxBeginThread(StartService_dan,(LPVOID)sServiceName.GetString());
 }


 void CSyncProtectDlg::OnBnClickedBtnStopSvc()
 {
	 AfxBeginThread(StopService_dan,(LPVOID)sServiceName.GetString());
 }

 afx_msg LRESULT CSyncProtectDlg::OnUpdateHint(WPARAM wParam, LPARAM lParam)
 {
	/* if((int)wParam==0)
	 UpdateData(FALSE);
	 else
		 UpdateData(TRUE);
	 return 0;*/
	 LPCTSTR lptz1=(LPCTSTR)wParam;
	 if(lptz1!=NULL)
	 {
		 int nLen=m_edtHint.GetWindowTextLength();
		 if(nLen>2888)
		 {
			 m_edtHint.SetSel(0,-1,FALSE);
			 m_edtHint.Clear();
		 }
		 else
			m_edtHint.SetSel(nLen,nLen,FALSE);
		 m_edtHint.ReplaceSel(lptz1);
	 }
	 return S_OK;
 }

 void CSyncProtectDlg::OnNcPaint()
 {
	 if(theApp.bShowWindow)  
	 {  
		 CDialogEx::OnNcPaint(); 
	 }  
	 else  
		  ShowWindow(SW_HIDE);  
		
 }

 void CSyncProtectDlg::OnTimer(UINT_PTR nIDEvent)
 {
	 // TODO: 在此添加消息处理程序代码和/或调用默认值
	 if(nIDEvent==nEventIdGetShare)
	 {
		 AfxBeginThread(DoParseInstruction,NULL);
	 }
	 else if(nIDEvent==nEventIdAu)
	 {
		 AfxBeginThread(DoAutoUpdate,NULL);
	 }
	 CDialogEx::OnTimer(nIDEvent);
 }

 void CSyncProtectDlg::SetShareData1()
 {
	 //DWORD d=WaitForSingleObject(hLkShare,100);
	 if(WaitForSingleObject(hLkShare,0)==WAIT_OBJECT_0)
	 {
		 ResetEvent(hLkShare);
		 //拷贝数据到共享文件里。    
		 LPVOID lpMapAddr = MapViewOfFile(theApp.hShareMemory,FILE_MAP_ALL_ACCESS,   
			 0,0,0);   
		 ProcessExchangeData pData;
		pData.IsShowWindow=theApp.bShowWindow;
		pData.IsStartAutoUpdate=theApp.bStartUpdate;
		memcpy(lpMapAddr,&pData,sizeof(ProcessExchangeData));  
		if(pData.IsShowWindow)
			ShowWindow(SW_SHOW);
		else
			ShowWindow(SW_HIDE);
		FlushViewOfFile(lpMapAddr,sizeof(ProcessExchangeData));
		UnmapViewOfFile(lpMapAddr);
		SetEvent(hLkShare);
	 }
 }

 void CSyncProtectDlg::GetShareData1()
 {
	 if(WaitForSingleObject(hLkShare,0)==WAIT_OBJECT_0)
	 {
		 ResetEvent(hLkShare);
		 //拷贝数据到共享文件里。    
		 PProcessExchangeData pData=new ProcessExchangeData;
		 LPVOID lpMapAddr = MapViewOfFile(theApp.hShareMemory,FILE_MAP_ALL_ACCESS,   
			 0,0,0);   
		 memcpy_s(pData,sizeof(ProcessExchangeData),lpMapAddr,sizeof(ProcessExchangeData));   
		 //显示界面
		 theApp.bShowWindow=pData->IsShowWindow;
		 theApp.bStartUpdate=pData->IsStartAutoUpdate;
		 if(pData->IsShowWindow)
		 { 
		 ShowWindow(SW_SHOW);
		 }
		 else
		 ShowWindow(SW_HIDE);
		 if(pData->IsStartAutoUpdate)
		 {
		 AfxBeginThread(DoAutoUpdate,NULL);
		 pData->IsStartAutoUpdate=FALSE;
		 }
		 memcpy(lpMapAddr,pData,sizeof(ProcessExchangeData));   
		 FlushViewOfFile(lpMapAddr,sizeof(ProcessExchangeData));
		 UnmapViewOfFile(lpMapAddr);
		 delete pData;
		 SetEvent(hLkShare);
	 }
 }


 void CSyncProtectDlg::OnGetMinMaxInfo(MINMAXINFO* lpMMI)
 {
	 // TODO: 在此添加消息处理程序代码和/或调用默认值
	 lpMMI->ptMinTrackSize.x=568;
	 lpMMI->ptMinTrackSize.y=398;
	 lpMMI->ptMaxTrackSize.x=568;
	 lpMMI->ptMaxTrackSize.y=398;
	 CDialogEx::OnGetMinMaxInfo(lpMMI);
 }
