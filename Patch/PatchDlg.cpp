
// PatchDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "Patch.h"
#include "PatchDlg.h"
#include "winsvc.h"
#include <tlhelp32.h>
#include "common/utility.h"                /* for GetLeidianLogPath */
#include "miniutil/bkini.h"                /* for BKIni::Document */
#include "ziputil/zip.h"  
#include "ziputil/unzip.h"
#include "miniutil/fileversion.h"
#include "bkupctl/bkupddownloader.h"
#include "bkupctl/bkupdwebfile.h"
#include "miniutil/szstr.h"
#include "common/RegistryKey.h"
#include "tinyxml/tinyxml.h"
#include "zlibcrc32/crc32.h"
#include "Psapi.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define  WM_UPDATE_HINT WM_USER+1

//注册表路径
#define  REGISTER_PATH _T("SOFTWARE\\Yishion\\")
//Update 目录名 下载后存放路径
#define  UPDATE_DIRECTORY _T("Update")


// CPatchDlg 对话框
//特殊文件 
const int nSpecialFileCount=1;
const CString sSpecialFile[nSpecialFileCount]={
	_T("PATCH.EXE")
};

//需要更新程序
const int nAppCount=4;
CString sPatchAppName[nAppCount][2]={
	_T("SyncProtect.exe"),_T("F"),
	_T("YS.Sync.WinUI.exe"),_T("F"),
	_T("YSPOS.exe"),_T("F"),
	_T("SyncUI.exe"),_T("F"),
};

//跳过删除的文件
const int nSkipFileCount=1;
CString arrSkipFileName[nSkipFileCount]={
	_T("ServiceSetting.xml"),
};

const int nSkipDirectoryCount=2;
CString arrSkipDirectoryName[nSkipDirectoryCount]={
	_T("Log"),
	_T("Update"),
};



//是否有新版本
bool bFindNewVersion=FALSE;
//下载基址
CString sBaseAddress;
//下载后存放路径
//const CString sSavePath=L"Update\\";
//客户端配置文件名
const CString sClientCfgFileName=L"updatelist.ini";
//服务端地址
CString sSvrPath=L"http://pos.yishion.com:8765/soft/(不能删)同步服务端更新程序/SyncUpdate.ini";
//CString sSvrPath=L"http://pos.yishion.com:8765/POS_UPDATE/SyncUpdate.ini";
//应用程序路径
CString sAppPath;
//更新压缩包信息
//压缩包文件名
CString sCompressFileName;
//压缩包CRC32
DWORD nCompressCRC32=0;
//压缩包解压并验证后写在注册表的标志
DWORD nCompressFlag=0;
//压缩标志的注册表 Key Name
#define COMPRESS_REGISTER_KEYNAME _T("UpdateCompressFlag")

CPatchDlg::CPatchDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CPatchDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	nCrc32Retry=5;
	bCrcChecekUp=TRUE;
	isCanClose=FALSE;
}

void CPatchDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_HINT, m_edtHint);
	DDX_Control(pDX, IDC_PGS_DOWNLOAD_FILE, m_pgsDownloadFile);
}

BEGIN_MESSAGE_MAP(CPatchDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_MESSAGE(WM_UPDATE_HINT, &CPatchDlg::OnUpdateHint)
	//}}AFX_MSG_MAP
	ON_WM_GETMINMAXINFO()
END_MESSAGE_MAP()


// CPatchDlg 消息处理程序

BOOL CPatchDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码
	Log1=WriteLog;
	CAppPath::Instance().GetPath(sAppPath);
	//读取配置文件
	ReadServiceSetting();
	AfxBeginThread(DoPatch,NULL);
	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CPatchDlg::OnPaint()
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
		CDialog::OnPaint();
	}
}

afx_msg LRESULT CPatchDlg::OnUpdateHint(WPARAM wParam, LPARAM lParam)
{
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

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CPatchDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CPatchDlg::CheckVersion( BOOL *pbFind )
{
	CString strCfg;
	strCfg.AppendFormat(_T("%s\\%s\\%s"),sAppPath,UPDATE_DIRECTORY,sClientCfgFileName);

	bCrcChecekUp=TRUE;
	sCrcHint.Empty();
	// 下载服务端配置文件到Update
	if(DownloadFile(sSvrPath,strCfg,0,FALSE)==S_OK){
	//	读取配置文件中的文件列表和版本号.
	ParseServerCfg(strCfg);
	//下载补丁包,处理压缩
	DealCompress();
	//	读取运行中模块的文件名和版本号 比较CR2
	GetMoudleList();
    //	4>比较版本,有高版本,置升级标志.下载到Update
	//CompareVersion();
	//兼容旧的。。
	DownloadDiffFile();
	}
    *pbFind=bFindNewVersion;
}

HRESULT CPatchDlg::ParseServerCfg(CString cfgPath)
{
	BKIni::Document ini;
	int nFileCount=100;
	HRESULT hr=S_OK;
	BKIni::Section section;
	//压缩包文件.
	CString sCompressStr;
	CString sTmp;
	DWORD dwCrc=0;

	if (FAILED(hr))
		return hr;
    ini.LoadFile(cfgPath);
	if (!ini.HasSection(L"config"))
		return E_FAIL;
    section=ini[L"config"];
	//初始化
	nFileCount=section[L"maxFileCount"].GetValueDWORD();
	sBaseAddress=section[_T("baseAddress")].GetValueString();
	sCompressStr=section[_T("compressPackage")].GetValueString();
    AfxExtractSubString(sCompressFileName,sCompressStr,0,L',');
	AfxExtractSubString(sTmp,sCompressStr,1,L','); //CRC
	_stscanf_s(sTmp.GetString(),_T("%x"),&dwCrc);
	nCompressCRC32=dwCrc;
	//注册表标志
	AfxExtractSubString(sTmp,sCompressStr,2,L','); 
	_stscanf_s(sTmp.GetString(),_T("%x"),&dwCrc);
	nCompressFlag=dwCrc;
	section=ini[L"file"];
	m_mapSvrFileVersion.RemoveAll();
	Log(_T("分析服务端的文件列表,基址%s,最大文件数%d\r\n"),sBaseAddress,nFileCount);
	for (int i=1; i<nFileCount; ++i)
	{
		wchar_t szItem[10] = {0};
		wsprintf(szItem, L"f%d", i);
		CString 
			str,
			sFile,
			sVer,
			sCliPath;

		if (!section.HasKey(szItem))
			break;
        str=section[szItem].GetValueString();
		PFILEUPDATESVRDATA pData=new FILEUPDATESVRDATA();
	    AfxExtractSubString(sFile,str,0,L',');
		AfxExtractSubString(pData->sFilePath,str,1,L',');
        AfxExtractSubString(sVer,str,2,L',');
		AfxExtractSubString(sTmp,str,3,L',');
		AfxExtractSubString(sCliPath,str,4,L',');
		_stscanf_s(sTmp.GetString(),_T("%x"),&dwCrc);
		pData->sFileName=sFile;
		pData->sVersion=sVer;
		pData->nCrc32=dwCrc;
		pData->sClientPath=sCliPath;
		m_mapSvrFileVersion.SetAt(sFile.MakeUpper(),pData);
		//Log(_T("服务端文件:%-25s,版本:%10s\r\n"),pData->sFileName,pData->sVersion);
	}
	return S_OK;
}

//比较Update目录的文件CRC32是否与服务端一致.
//不一致或找不到都删除(视为旧压缩包,旧文件)
HRESULT CPatchDlg::CompareFile( CString path )
{
	CFileFind tempFind;
	TCHAR tempFileFind[MAX_PATH];

	_stprintf(tempFileFind,_T("%s\\*.*"),path);
	BOOL IsFinded=(BOOL)tempFind.FindFile(tempFileFind);
	while(IsFinded)
	{
		IsFinded=(BOOL)tempFind.FindNextFile();
		if(!tempFind.IsDots())
		{
			TCHAR foundFileName[MAX_PATH];
			_tcscpy(foundFileName,tempFind.GetFileName().GetBuffer(MAX_PATH));
			CString sKey=tempFind.GetFileName().MakeUpper();
			if(tempFind.IsDirectory())
			{
				TCHAR tempDir[MAX_PATH];
				_stprintf(tempDir,_T("%s\\%s"),path,foundFileName);
				CompareFile(tempDir);
			}
			else
			{
				TCHAR tempFileName[MAX_PATH];
				_stprintf(tempFileName,_T("%s\\%s"),path,foundFileName);
				PFILEUPDATESVRDATA pData=NULL;
				m_mapSvrFileVersion.Lookup(sKey,(void *&)pData);
				if(pData!=NULL)
				{
					DWORD dwCliCrc=CalcCrc(tempFileName);
					if(pData->nCrc32!=dwCliCrc)
						DeleteFile(tempFileName);
				}
				else
				{
					//忽略配置文件.
					if(_tcsicmp(foundFileName,sClientCfgFileName)==0)
						continue;
					DeleteFile(tempFileName);
				}
			}

		}

	}
	tempFind.Close();
	return S_OK;
}

HRESULT CPatchDlg::DealCompress()
{
	//查询注册表查是否需要下载更新包
	CString sTmp;
	DWORD  
		nTmp,
		//当前注册表标志
		nCompressReg;
	HRESULT hr;
	CRegistryKey reg(HKEY_LOCAL_MACHINE,REGISTER_PATH,FALSE);
	reg.Read(COMPRESS_REGISTER_KEYNAME,&nCompressReg);
	//_stscanf_s(sTmp.GetString(),_T("%d"),&nCompressReg);
	CString sUpdateDir;
	sUpdateDir.AppendFormat(_T("%s\\%s"),sAppPath,UPDATE_DIRECTORY);
	if(nCompressReg!=nCompressFlag)
	{
		//下载到Update目录 
		CString sPackage;
		sPackage.AppendFormat(_T("%s\\%s"),sUpdateDir,sCompressFileName);
		// 下载服务端配置文件到Update
		Log(_T("正在下载更新包,请稍候!\r\n"));
		if(DownloadFile(sBaseAddress+sCompressFileName,sPackage,TRUE,nCompressCRC32)==S_OK){
			//解压到Update目录
			HZIP hz;
			ZIPENTRY ze;
			hz = OpenZip(sPackage, 0 ); 
			GetZipItem(hz, -1, &ze);
			int nItems = ze.index;
			for (int i = 0; i < nItems; ++i)
			{
				CString strOldCruDir;
				GetZipItem(hz,i,&ze);
				strOldCruDir.Format(_T("%s\\%s"),sUpdateDir,ze.name);  
				Log(_T("正在解压:%s\r\n"),ze.name);
				UnzipItem(hz, i, strOldCruDir);
			}
			CloseZip(hz);
			CompareFile(sUpdateDir);
			reg.Write(COMPRESS_REGISTER_KEYNAME,nCompressFlag);
		}
	}

	

	return S_OK;

}

HRESULT CPatchDlg::DownloadFile(CString srvPath,CString savePath,BOOL checkCrc,DWORD crc)
{
	CBkUpdDownloader downloader(TRUE);
	HRESULT hr;
	CBkUpdWebFile file;
	CString sSaveDir=savePath;
	DWORD dwCliCrc=0;
	int i=0;
	CString sFileName;

	PathRemoveFileSpec(sSaveDir.GetBuffer());
	if(!PathIsDirectory(sSaveDir))
		hr=CAppPath::Instance().CreateFullPath(sSaveDir);
	//先比较下载文件是否存在
	if(PathFileExists(savePath))
	{
		dwCliCrc=CalcCrc(savePath);
		if(dwCliCrc!=crc) //校验不通过，重新下载
		{
			goto Download;
		}
		else
		{
			hr=S_OK;
			goto Done;
		}
	}

	Download:
	file.SetFileName(savePath);
	sFileName=PathFindFileName(savePath);
	GetDlgItem(IDC_STC_DOWNLOAD_FILE)->ShowWindow(SW_SHOW);
	GetDlgItem(IDC_PGS_DOWNLOAD_FILE)->ShowWindow(SW_SHOW);
	GetDlgItem(IDC_STC_DOWNLOAD_FILE)->SetWindowText(sFileName);
	do{
		hr = downloader.Download(srvPath, (IBkUpdWebFile*)&file, this);
		dwCliCrc=CalcCrc(savePath);
		i++;
	}
	while(hr==S_OK && checkCrc && i<=nCrc32Retry && dwCliCrc!=crc);
	if(dwCliCrc!=crc && checkCrc)
	{
		if(bCrcChecekUp) bCrcChecekUp=FALSE;
		sCrcHint.AppendFormat(_T("%-38s校验不通过,客户端:%08x,服务端:%08x.\r\n"),
			sFileName,dwCliCrc,crc);
		hr=S_FALSE;
	}

	Done:
	GetDlgItem(IDC_STC_DOWNLOAD_FILE)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_PGS_DOWNLOAD_FILE)->ShowWindow(SW_HIDE);
	return hr;
}

BOOL CPatchDlg::DownloadProcess( DWORD dwTotalSize, DWORD dwReadSize )
{
	float f1=(float)dwReadSize;
	m_pgsDownloadFile.SetPos((int)(f1/dwTotalSize*100));
	return TRUE;
}

HRESULT CPatchDlg::GetMoudleList()
{
	DWORD ret=0;
	//根据服务端文件列表.找到当前目录,文件版本
	HRESULT hr=S_OK;
	POSITION pos=m_mapSvrFileVersion.GetStartPosition();
	PFILEUPDATESVRDATA pData=NULL;
	PFILEUPDATECLIDATA pCliData=NULL;
	CString sKey;
	BOOL bDiff=FALSE;
	CString sFilePath;
	CString sVersion;
	CString sFileName;
	CFileFind ff;  
	BOOL bFound =FALSE;
	m_mapCliFileVersion.RemoveAll();

	//本来 要传给C# 因独立项目所以取消
	//sUpdataPath.AppendFormat(_T("%s\\%s"),sAppPath,sSavePath);
	//TiXmlDocument doc;
	//TiXmlElement *root=new TiXmlElement("dsFileVersion");
	//TiXmlDeclaration *d=new TiXmlDeclaration("1.0","utf-8","");
	//doc.LinkEndChild(d);
	//doc.LinkEndChild(root);
	//TiXmlElement 
	//	*nodeId,
	//	*nodeFn,
	//	*nodeFv;

	do{
		//bDiff=FALSE;
		pData=NULL;
		bFound=FALSE;
		sFilePath.Empty();
		sVersion.Empty();
		m_mapSvrFileVersion.GetNextAssoc(pos,sKey,(LPVOID&)pData);
		if(pData->sClientPath.IsEmpty())
			sFilePath.AppendFormat(_T("%s\\%s"),sAppPath,pData->sFileName);
		else
			sFilePath.AppendFormat(_T("%s\\%s"),sAppPath,pData->sClientPath);
		bFound=PathFileExists(sFilePath.GetString());
		if(bFound)
		{
			pCliData=new FILEUPDATECLIDATA();
			GetFileVersion(sFilePath.GetString(),sVersion);
			sFileName=pData->sFileName;
			pCliData->sFileName=sFileName;
			if(!sVersion.IsEmpty()) //有些文件没有版本号
				pCliData->sVersion=sVersion;
			pCliData->nCrc32=CalcCrc(sFilePath);
			m_mapCliFileVersion.SetAt(sFileName.MakeUpper(),pCliData);
			//本来 要传给C# 因独立项目所以取消
			//TiXmlElement *eTable=new TiXmlElement("sc_FileVersion");
			//root->LinkEndChild(eTable);
			//nodeId=new TiXmlElement("ID");
			//nodeFn=new TiXmlElement("FileName");
			//nodeFv=new TiXmlElement("FileVersion");
			//TiXmlText *fnContent =new TiXmlText(CT2A(pCliData->sFileName.GetString()));
			//TiXmlText *fvContent =new TiXmlText(CT2A(pCliData->sVersion.GetString()));
			//nodeFn->LinkEndChild(fnContent);
			//nodeFv->LinkEndChild(fvContent);
			//eTable->LinkEndChild(nodeId);
			//eTable->LinkEndChild(nodeFn);
			//eTable->LinkEndChild(nodeFv);
			//Log(_T("客户端文件:%-25s,版本:%10s\r\n"),pCliData->sFileName,pCliData->sVersion);
		}
	}while(pos!=NULL);
	//TiXmlPrinter printer;
	//doc.Accept(&printer);
	//CStringA str2=printer.CStr();
	//CString str=CA2T(str2);
	
	return ret;
}

HRESULT CPatchDlg::DownloadDiffFile()
{
	HRESULT hr=S_OK;
	POSITION pos=m_mapSvrFileVersion.GetStartPosition();
	PFILEUPDATESVRDATA pData=NULL;
    PFILEUPDATECLIDATA pCliData=NULL;
	CString sKey;
	BOOL bDiff=FALSE;
	CString sUpdataPath;
	sUpdataPath.AppendFormat(_T("%s\\%s\\"),sAppPath,UPDATE_DIRECTORY);	
	do{
		bDiff=FALSE;
		pCliData=NULL;
		m_mapSvrFileVersion.GetNextAssoc(pos,sKey,(LPVOID&)pData);
		m_mapCliFileVersion.Lookup(sKey,(LPVOID&)pCliData);
		if(pCliData==NULL)
		{
			bDiff=TRUE;
			Log(_T("S:%-25s, S:%8s,客户端没有,"),pData->sFileName,
				pData->sVersion);
		}
		else
		{
			if(pCliData->sVersion.IsEmpty())
			{
				if(pCliData->nCrc32!=pData->nCrc32)
					bDiff=TRUE;
			}
			else
			{
				bDiff=VersionLess(pCliData->sVersion,pData->sVersion);
				if(!bDiff) //版本一样比较Crc32
				{
					if(pCliData->nCrc32!=pData->nCrc32)
						bDiff=TRUE;
				}

			}
			if(bDiff)
			{
				Log(_T("S:%-25s, C:%-25s, S:%8s, C:%8s,"),pData->sFileName,pCliData->sFileName,
				pData->sVersion,pCliData->sVersion);
				//客户端存在的文件 才视为找到新版本..
				/*if(!bFindNewVersion && !IsSpecialFile(pCliData->sFileName))
					bFindNewVersion=TRUE;*/
			}
		}
		if(bDiff)
		{
			hr=DownloadFile(sBaseAddress+pData->sFilePath,sUpdataPath+pData->sFileName,TRUE,pData->nCrc32);
			if(hr==S_OK)
			{
				if(pCliData!=NULL)
				{
				if(!bFindNewVersion && !IsSpecialFile(pCliData->sFileName))
					bFindNewVersion=TRUE;
				}
				else
				{
					if(!bFindNewVersion)
						bFindNewVersion=TRUE;
				}
				Log(_T("下载成功。\r\n"));
			}
			else
				Log(_T("下载失败。\r\n"));
		}
	}while(pos!=NULL);
	return hr;
}

bool IsSpecialFile( CString fileName )
{
	bool ret=FALSE;
	for (int i=0;i<nSpecialFileCount;i++)
	{
		if(fileName.CompareNoCase(sSpecialFile[i])==0)
		{
			ret=TRUE;
			break;
		}
	}
	return ret;
}




int CPatchDlg::CalcCrc( CString fileName )
{
	FILE *pFile = NULL;
	char szFilePath[MAX_PATH] = {0};
	::WideCharToMultiByte( CP_ACP, 0, fileName, fileName.GetLength(), szFilePath, MAX_PATH-1, NULL, NULL );
	DWORD dwCrc = 0;
	if(PathFileExists(fileName))
	{
		pFile = fopen(szFilePath, "rb");
		if ( pFile )
		{
			dwCrc = CalcFileCrc( pFile );
			fclose( pFile );
		}
	}
	return dwCrc;
}

void CPatchDlg::GetCrcInfo( BOOL *checkup,LPTSTR lptzHint,int size )
{
	*checkup=bCrcChecekUp;
	_tcscpy_s(lptzHint,size,sCrcHint.Left(size-1).GetString());
}


void CALLBACK WriteLog(LPCTSTR lpMsg)
{
	CPatchDlg *pDlg=NULL;
	pDlg=(CPatchDlg*)theApp.GetMainWnd();
	if(pDlg!=NULL)
	{
		pDlg->SendMessage(WM_UPDATE_HINT,(WPARAM)lpMsg,0);
	}
}

//更新功能函数
UINT DoPatch( LPVOID lp1 )
{
	//进程是否存在
	BOOL bIsExistApp=TRUE;
	BOOL bIsRunSvc=TRUE;
	int nTmp=0;
	bool bTmp=TRUE;
	//杀死和停止服务重试次数
	int nRetryKillCount=0;
	//移动文件重试次数
	int nRetryMoveCount=0;
	const int nRetryKillMaxCount=10;
	CString str;
	TCHAR tz1[800]={0};
	CString sTmp;
	BOOL bFind=FALSE;
	BOOL bCheck=FALSE;
	int nHintCount=3;
	UINT ret=S_OK;	
	TCHAR tzFp[MAX_PATH]={0};

	CPatchDlg *pDlg=NULL;
	pDlg=(CPatchDlg*)theApp.GetMainWnd();

	//检测新版本
	pDlg->CheckVersion(&bFind);
	if(bFind)
	{
		Log(_T("发现新版本.进行文件检验!\r\n"));
		pDlg->GetCrcInfo(&bCheck,tz1,800);
		if(bCheck)
		{
			Log(_T("检验通过!\r\n"));
			//检查是否有更新

			//检查要更新进程是否退出
			nTmp=0;
			Log(_T("正在检测需更新的程序是否已经退出!\r\n"));
			
			//重新结束进程并重试移动
RetryCopy:
			for (int i=0;i<nAppCount;i++)
			{
				nTmp=GetProcessIdByName(sPatchAppName[i][0].GetString(),tzFp);
				sPatchAppName[i][1]=tzFp;
				if(nTmp==0)  
				{
					//没有启动的，按默认。
					sPatchAppName[i][1]=_T("F");
					continue;
				}
				//else
				//{
				//	//有在启动的 改为T，
				//	sPatchAppName[i][1]=_T("T");
				//}
				nRetryKillCount=0;
				Sleep(1000);
				do 
				{
					Sleep(1000);
					KillProcess(nTmp);
					nTmp=GetProcessIdByName(sPatchAppName[i][0].GetString(),tzFp);
					sPatchAppName[i][1]=tzFp;
					nRetryKillCount++;
				} while (nTmp!=0 && nRetryKillCount<nRetryKillMaxCount);

				if(nRetryKillCount>=nRetryKillMaxCount)
				{
					Log(_T("尝试杀死进程%s失败.已经重试%d次.更新程序不能继续.\r\n"),sPatchAppName[i][0].GetString(),nRetryKillCount);
					ret=S_FALSE;
					goto  Done;
				}
			}
			Log(_T("需更新的程序已经退出!\r\n"));
			//停止服务
			Log(_T("正在检测需更新的服务是否已经停止!\r\n"));
			for(int i=0;i<nSvcCount;i++)
			{
				nRetryKillCount=0;
				while(QueryServiceStatus_dan(sPatchSvcName[i][0].GetString(),SERVICE_RUNNING)||
					nRetryKillCount>=nRetryKillMaxCount)
				{
					str.Empty();
					str.AppendFormat(_T("%s,%s"),sPatchSvcName[i][0],sPatchSvcName[i][1]);
					StopService_dan((LPVOID)str.GetString());
					nRetryKillCount++;
					Sleep(1000); //暂停一下。
					if(QueryServiceStatus_dan(sPatchSvcName[i][0].GetString(),SERVICE_RUNNING) ||
						QueryServiceStatus_dan(sPatchSvcName[i][0].GetString(),SERVICE_START_PENDING)||
						QueryServiceStatus_dan(sPatchSvcName[i][0].GetString(),SERVICE_STOP_PENDING))
					{
						nTmp=GetProcessIdByName(sPatchSvcName[i][1].GetString(),tzFp);
						sPatchAppName[i][1]=tzFp;
						if(nTmp>0)
							KillProcess(nTmp);
					}
				}

				if(nRetryKillCount>=nRetryKillMaxCount)
				{
					str.Empty();
					str.AppendFormat(_T("尝试停止服务%s失败.已经重试%d次.更新程序不能继续.\r\n"),sPatchSvcName[i][0].GetString(),nRetryKillCount);
					Log(str.GetBuffer());
					ret= S_FALSE;
					goto  Done;
				}
			}
			Log(_T("需更新的服务已经停止!\r\n"));
			//移动文件 
			Log(_T("进行更新文件操作.\r\n"));
			GetAppPath(tz1);
			sTmp.AppendFormat(_T("%s\\%s"),tz1,UPDATE_DIRECTORY);
			//nRetryKillCount=0;

			if(MoveDirectory(sTmp.GetString(),tz1))
			{
				Log(_T("更新文件完毕.\r\n"));
				//重新启动被更新程序
				for (int i=0;i<nAppCount;i++)
				{
					str.Empty();
					str.AppendFormat(_T("%s"),sPatchAppName[i][1]);
					//F不启动
					if(str==_T("F")) continue;
					if(Execute(str,NULL,FALSE,FALSE))
					{
						Log(_T("程序%s启动成功.\r\n"),sPatchAppName[i][0].GetString());
					}
				}
				//启动服务
				for(int i=0;i<nSvcCount;i++)
				{
					if(StartService_dan((LPVOID)sPatchSvcName[i][0].GetString()))
					{
						Log(_T("服务%s启动成功.\r\n"),sPatchSvcName[i][0].GetString());
					}

				}
				ret=S_OK;
			}
			else
			{
				nRetryMoveCount++;
				if(nRetryMoveCount<=nRetryKillMaxCount)
				{
					str.Empty();
					str.AppendFormat(_T("更新文件失败.1秒后进行第%d 次重试.\r\n"),nRetryMoveCount);
					Log(str.GetString());
					Sleep(1000); //暂停一下。
					goto RetryCopy;

				}
				str.Empty();
				str.AppendFormat(_T("更新文件失败.已经重试%d 次,更新程序不能继续.\r\n"),nRetryMoveCount);
				Log(str.GetString());

				//失败时也重新启动被更新程序
				for (int i=0;i<nAppCount;i++)
				{
					str.Empty();
					str.AppendFormat(_T("%s\\%s"),sAppPath,sPatchAppName[i][0].GetString());
					//F不启动
					if(sPatchAppName[i][1]==_T("F")) continue;
					if(Execute(str,NULL,FALSE,FALSE))
					{
						Log(_T("程序%s启动成功.\r\n"),sPatchAppName[i][0].GetString());
					}
				}
				//启动服务
				for(int i=0;i<nSvcCount;i++)
				{
					if(StartService_dan((LPVOID)sPatchSvcName[i][0].GetString()))
					{
						Log(_T("服务%s启动成功.\r\n"),sPatchSvcName[i][0].GetString());
					}

				}

				ret= S_FALSE;
			}
		}
		else
		{
			Log(_T("检验不通过!\r\n%s"),tz1);
		}
	}
	else
	{
		Log(_T("正在检查服务状态!\r\n"));
		CheckSyncSvc();
	}
Done:
//创建桌面快捷方式
CRegistryKey reg(HKEY_LOCAL_MACHINE,REGISTER_PATH,FALSE);
CString s1;
CString s2,s3;
int i=sAppPath.ReverseFind('\\');
CString s4=sAppPath.Left(i);
CString s5;

if(reg.GetValueByDataEx(sAppPath.GetString(),&s5))
{
	CString s6=s5.Right(s5.GetLength()-6);
    CString s7,
		s8;
	s7.Format(_T("BranchName_%s"),s6);
	reg.Read(s7,&s1);
	s3.Format(_T("同步桥-%s"),s1.GetString());
	s2.Format(_T("%s\\%s"),s4,sPatchAppName[3][0]);
	CreateDesktopLink(s3,s2);

	s3.Format(_T("同步修复-%s"),s1.GetString());
	s2.Format(_T("%s\\%s"),sAppPath,sSpecialFile[0]);
	CreateDesktopLink(s3,s2);
}

if(isClear){
	Log(_T("进行清理文件操作....\r\n"));
	DelDiffFile();
	}
	for(int i=nHintCount;i>0;i--)
	{
		Log(_T("%d 秒后关闭。\r\n"),i);
		Sleep(1000);
	}
	pDlg->isCanClose=TRUE;
	pDlg->SendMessage(WM_CLOSE);

	return ret;
}


DWORD GetProcessIdByName( LPCTSTR lpFilename,LPTSTR lpFilePath)
{
	DWORD dwCount = 0;
	HANDLE hProcessSnap;
	PROCESSENTRY32 pe32;
	pe32.dwSize=sizeof(PROCESSENTRY32);
	hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS,0);
	CString proName=lpFilename;
	proName.MakeUpper();
	if(hProcessSnap == INVALID_HANDLE_VALUE) return 0;
	if(!Process32First(hProcessSnap,&pe32))
	{
		CloseHandle(hProcessSnap);
		return 0;
	}
	do
	{
		_tcsupr_s(pe32.szExeFile);
		//查找szExeFile 中第一次出现proName的位置。作用=包含。
		if(_tcsstr(pe32.szExeFile,proName))
		{
			dwCount=pe32.th32ProcessID;
			HANDLE h=OpenProcess(PROCESS_ALL_ACCESS,FALSE,dwCount);
			DWORD dNeed=0;
			HMODULE hm=NULL;
			EnumProcessModules(h,&hm,sizeof(hm),&dNeed);
			GetModuleFileNameEx(h,hm,lpFilePath,MAX_PATH);
			break;
		}
	}
	while(Process32Next(hProcessSnap,&pe32));
	CloseHandle(hProcessSnap);
	return dwCount;
}




VOID KillProcess(DWORD dwPid)
{
	Adjust(_T("SeDebugPrivilege"),true);
	// When the all operation fail this function terminate the "winlogon" Process for force exit the system.
	HANDLE hTargetProcess = OpenProcess(PROCESS_QUERY_INFORMATION |   // Required by Alpha
		PROCESS_TERMINATE     |   // For CreateRemoteThread
		PROCESS_VM_OPERATION      |   // For VirtualAllocEx/VirtualFreeEx
		PROCESS_VM_WRITE,             // For WriteProcessMemory
		FALSE,dwPid);

	if(hTargetProcess == NULL)
	{
		return;
	}
	TerminateProcess(hTargetProcess, 0);
	return;
}

//调整进程权限
bool   WINAPI Adjust(TCHAR *PrivilegeName,bool lEnabled) 
{
	HANDLE hToken;
	TOKEN_PRIVILEGES tp;
	LUID luid;
	//打开进程令牌环
	if(!OpenProcessToken(GetCurrentProcess(),TOKEN_ADJUST_PRIVILEGES|TOKEN_QUERY,&hToken))
	{
		return false;
	}
	//获得进程本地唯一ID
	if(!LookupPrivilegeValue(NULL,PrivilegeName,&luid))
	{
		return false;
	}
	tp.PrivilegeCount=1;
	tp.Privileges[0].Attributes=lEnabled?SE_PRIVILEGE_ENABLED:0;
	tp.Privileges[0].Luid=luid;
	//调整进程权限
	if(!AdjustTokenPrivileges(hToken,0,&tp,sizeof(TOKEN_PRIVILEGES),NULL,NULL))
	{
		return false;
	}
	return true;
}


UINT StopService_dan( LPVOID lp1 )
{
	int nRetry=0;
	int nMaxRetry=5;
	CString str=(LPCTSTR)lp1;
	CString sn;
	CString sf;
	TCHAR tzp[MAX_PATH]={0};
	AfxExtractSubString(sn,str,0,L',');
	AfxExtractSubString(sf,str,1,L',');
	// 打开服务管理对象
	SC_HANDLE hSC = ::OpenSCManager( NULL, 
		NULL, GENERIC_EXECUTE);
	if( hSC == NULL)
	{
		Log(_T("打开服务管理器错误！\r\n"));
		return FALSE;
	}

	SC_HANDLE hSvc = ::OpenService(hSC,sn,
		SERVICE_START | SERVICE_QUERY_STATUS | SERVICE_STOP);
	if(hSvc == NULL)
	{
		Log(_T("打开服务错误！\r\n"));
		::CloseServiceHandle( hSC);
		return FALSE;
	}

	SERVICE_STATUS status;
	if( ::QueryServiceStatus( hSvc, &status) == FALSE)
	{
		Log(_T("查询服务状态错误\r\n"));
		::CloseServiceHandle( hSvc);
		::CloseServiceHandle( hSC);
		return FALSE;
	}

	//如果处于停止状态则启动服务，否则停止服务。
	if( status.dwCurrentState == SERVICE_RUNNING)
	{
STOP:
		if(::ControlService( hSvc, 
				SERVICE_CONTROL_STOP, &status) == TRUE)
			
		Log(_T("尝试停止服务,正等待服务停止！\r\n"));
		// 等待服务停止
		nRetry=0;
		while( ::QueryServiceStatus( hSvc, &status) == TRUE)
		{
			::Sleep( status.dwWaitHint);
			if( status.dwCurrentState == SERVICE_STOPPED)
			{
				Log(_T("服务成功停止。\r\n"));
				//停止也尝试结束进程
				KillProcess(GetProcessIdByName(sf,tzp));
				break;
			}
			if( status.dwCurrentState == SERVICE_RUNNING)
			{
				goto STOP;
			}
			
			if(nRetry==nMaxRetry)
			{
				Log(_T("尝试停止服务,等待太久,尝试结束进程.\r\n"));
				KillProcess(GetProcessIdByName(sf,tzp));
			}
			nRetry++;
		}
	}
	else
	{
		Log(_T("服务没有运行!\r\n"));
	}

	::CloseServiceHandle( hSvc);
	::CloseServiceHandle( hSC);

	return TRUE;
}

UINT StartService_dan( LPVOID lp1 )
{
	int nRetry=0;
	int nMaxRetry=5;
	CString sn=(LPCTSTR)lp1;
	//CString sHint=_T("hint");
	// 打开服务管理对象
	SC_HANDLE hSC = ::OpenSCManager( NULL, 
		NULL, GENERIC_EXECUTE);
	if( hSC == NULL)
	{
		Log(_T("打开服务管理器错误！\r\n"));
		return FALSE;
	}

	SC_HANDLE hSvc = ::OpenService(hSC,sn,
		SERVICE_START | SERVICE_QUERY_STATUS | SERVICE_STOP);
	if(hSvc == NULL)
	{
		Log(_T("打开服务错误！\r\n"));
		::CloseServiceHandle( hSC);
		return FALSE;
	}
	SERVICE_STATUS status;
	if( ::QueryServiceStatus( hSvc, &status) == FALSE)
	{
		Log(_T("查询服务状态错误\r\n"));
		::CloseServiceHandle( hSvc);
		::CloseServiceHandle( hSC);
		return FALSE;
	}

	if(status.dwCurrentState == SERVICE_STOP ||
		status.dwCurrentState== SERVICE_STOPPED)
	{
		Log(_T("服务 %s 启动中。请稍等!\r\n"),sn);
		// 启动服务
		if( ::StartService( hSvc, NULL, NULL) == FALSE)
		{
			Log(_T("启动服务发生错误。\r\n"));
			::CloseServiceHandle( hSvc);
			::CloseServiceHandle( hSC);
			return FALSE;
		}
		// 等待服务启动
		nRetry=0;
		while( ::QueryServiceStatus( hSvc, &status) == TRUE)
		{
			::Sleep(1500);
			// 启动服务
			if(status.dwCurrentState == SERVICE_STOP)
			{
				::StartService( hSvc, NULL, NULL) ;
				nRetry++;
			}

			if( status.dwCurrentState == SERVICE_RUNNING)
			{
				//sHint.Empty();
				//sHint.AppendFormat(_T("服务成功启动。\r\n")),
				//Log(sHint);
				break;
			}
			if(nRetry==nMaxRetry)
				break;
		}
	}
	else
	{
		if( status.dwCurrentState == SERVICE_RUNNING)
		{
			Log(_T("服务已经启动。\r\n"));
		}
	}
	::CloseServiceHandle( hSvc);
	::CloseServiceHandle( hSC);
	return TRUE;
}


bool CheckSyncSvc()
{
	int nRetry=0;
	int nMaxRetry=15;
	CString strHint;

	SC_HANDLE hSC = ::OpenSCManager( NULL, 
		NULL, GENERIC_EXECUTE);
	if( hSC == NULL)
	{
		Log(_T("打开服务管理器错误！\r\n"));
		return FALSE;
	}

	for(int i=0;i<nSvcCount;i++)
	{
		if(sPatchSvcName[i][0].IsEmpty())
			continue;
	SC_HANDLE hSvc = ::OpenService(hSC,sPatchSvcName[i][0].GetString(),
		SERVICE_START | SERVICE_QUERY_STATUS | SERVICE_STOP);
	if(hSvc == NULL)
	{
		strHint.Empty();
		strHint.AppendFormat(_T("打开服务 %s 错误！\r\n"),sPatchSvcName[i][0]);
		Log(strHint);
		::CloseServiceHandle( hSC);
		return FALSE;
	}
	SERVICE_STATUS status;
	if( ::QueryServiceStatus( hSvc, &status) == FALSE)
	{
		strHint.Empty();
		strHint.AppendFormat(_T("查询服务%s状态错误\r\n"),sPatchSvcName[i][0]);
		Log(strHint);
		::CloseServiceHandle( hSvc);
		::CloseServiceHandle( hSC);
		return FALSE;
	}

	if(status.dwCurrentState == SERVICE_STOP ||
		status.dwCurrentState== SERVICE_STOPPED)
	{
		strHint.Empty();
		strHint.AppendFormat(_T("服务%s停止,重启中。请稍等!\r\n"),sPatchSvcName[i][0]);
		Log(strHint);
		// 启动服务
		if( ::StartService( hSvc, NULL, NULL) == FALSE)
		{
			Log(_T("启动服务发生错误。\r\n"));
			::CloseServiceHandle( hSvc);
			::CloseServiceHandle( hSC);
			return FALSE;
		}
		// 等待服务启动
		nRetry=0;
		while( ::QueryServiceStatus( hSvc, &status) == TRUE)
		{
			::Sleep( status.dwWaitHint);
			// 启动服务
			if(status.dwCurrentState == SERVICE_STOP ||
				status.dwCurrentState== SERVICE_STOPPED)
			{
				::StartService( hSvc, NULL, NULL) ;
				nRetry++;
			}

			if( status.dwCurrentState == SERVICE_RUNNING)
			{
				Log(_T("服务成功启动。\r\n"));
				break;
			}
			if(nRetry==nMaxRetry)
			{
                Log(_T("重试次数太多,服务不能启动!\r\n"));
				break;
			}
		}
	}
	else
	{
		if( status.dwCurrentState == SERVICE_RUNNING)
		{
			strHint.Empty();
			strHint.AppendFormat(_T("服务%s已经启动.\r\n"),sPatchSvcName[i][0]);
			Log(strHint);
		}
	}
	::CloseServiceHandle( hSvc);
	}
	
	::CloseServiceHandle( hSC);
	return TRUE;
}

// 将pc中的文件夹从一个目录拷贝到另外的一个目录
BOOL MoveDirectory(CString strSrcPath, CString strDesPath)
{
	CPatchDlg *pDlg=NULL;
	pDlg=(CPatchDlg*)theApp.GetMainWnd();

	if( strSrcPath.IsEmpty() )
	{       
		return FALSE;
	}

	if ( !PathIsDirectory(strDesPath) )
	{
		if ( !CreateDirectory(strDesPath,NULL))
			return FALSE;
	}

	if ( strSrcPath.GetAt(strSrcPath.GetLength()-1) != '\\' )
		strSrcPath += '\\';
	if ( strDesPath.GetAt(strDesPath.GetLength()-1) != '\\' )
		strDesPath += '\\';

	BOOL bRet = FALSE; // 因为源目录不可能为空，所以该值一定会被修改
	CFileFind ff;  
	BOOL bFound = ff.FindFile(strSrcPath+_T("*"),   0);  
	CString strFile;
	BOOL bSpecialFile=FALSE;
	while(bFound)      // 递归拷贝
	{  
		bFound = ff.FindNextFile();  
		bSpecialFile=FALSE;
		if( ff.IsDots() )  
			continue;

		CString strSubSrcPath = ff.GetFilePath();
		CString strSubDespath = strSubSrcPath;
		CString strSubDespath1=strSubSrcPath;
		strFile=PathFindFileName(strSubSrcPath);
		strFile.MakeUpper();
		PFILEUPDATESVRDATA pData=NULL;
		pDlg->m_mapSvrFileVersion.Lookup(strFile,(LPVOID&)pData);
		if(pData!=NULL){
		if(!pData->sClientPath.IsEmpty())
			//路径不是Update目录的上一个目录时
			strSubDespath1=strDesPath+pData->sClientPath;
			strSubDespath1.Replace(strSrcPath, strDesPath);
		}
		strSubDespath.Replace(strSrcPath, strDesPath);
		if( ff.IsDirectory() )
			bRet = MoveDirectory(strSubSrcPath, strSubDespath);     // 递归拷贝文件夹
		else
		{
			for (int i=0;i<nSpecialFileCount;i++)
			{
				//找到特殊文件
				if(_tcscmp(strFile.GetString(),sSpecialFile[i])==0)
				{
					bSpecialFile=TRUE;
					break;
				}	
			}
			if(bSpecialFile)
				bRet=MoveFileEx( strSubSrcPath,strSubDespath,MOVEFILE_DELAY_UNTIL_REBOOT|MOVEFILE_REPLACE_EXISTING);
			else
			{
				if(!strSubDespath1.IsEmpty())  //路径不是Update目录的上一个目录时
				{
					//bRet=CopyFile(strSubSrcPath,strSubDespath1,FALSE); 
					bRet=MoveFileEx(strSubSrcPath,strSubDespath1,MOVEFILE_REPLACE_EXISTING); 
				}
				else
				{
					bRet = MoveFileEx(strSubSrcPath, strSubDespath,MOVEFILE_REPLACE_EXISTING);   // 移动文件
				}
			}
		}
		if ( !bRet )
			break;
	}  
	ff.Close();
	return bRet;
}



BOOL QueryServiceStatus_dan( LPCTSTR lpSvcName,UINT status)
{
	int nRetry=0;
	int nMaxRetry=5;
	CString sn=lpSvcName;
	// 打开服务管理对象
	SC_HANDLE hSC = ::OpenSCManager( NULL, 
		NULL, GENERIC_EXECUTE);
	if( hSC == NULL)
	{
		Log(_T("打开服务管理器错误！\r\n"));
		return FALSE;
	}

	SC_HANDLE hSvc = ::OpenService(hSC,sn,
		SERVICE_START | SERVICE_QUERY_STATUS | SERVICE_STOP);
	if(hSvc == NULL)
	{
		Log(_T("打开服务错误！\r\n"));
		::CloseServiceHandle( hSC);
		return FALSE;
	}
	SERVICE_STATUS status1;
	if( ::QueryServiceStatus( hSvc, &status1) == FALSE)
	{
		Log(_T("查询服务状态错误\r\n"));
		::CloseServiceHandle( hSvc);
		::CloseServiceHandle( hSC);
		return FALSE;
	}
	::CloseServiceHandle( hSvc);
	::CloseServiceHandle( hSC);
	return status1.dwCurrentState == status;
}

void GetAppPath( LPTSTR lpAppPath )
{
	CString str;
	CAppPath::Instance().GetPath(str);
	_tcscpy_s(lpAppPath,MAX_PATH,str.GetBuffer());
}

BOOL Execute( LPCTSTR lpszProcessName,LPCTSTR strCmdLine, BOOL bForceAdmin, BOOL bWaitProcess)
{
	BOOL bRet = FALSE;
	PROCESS_INFORMATION processInfo;
	STARTUPINFO si = {sizeof(STARTUPINFO)};
	HANDLE hProcess = NULL;

	if (bForceAdmin)
	{
		SHELLEXECUTEINFO sei = { sizeof(SHELLEXECUTEINFO) };

		sei.fMask = SEE_MASK_NOCLOSEPROCESS;
		sei.hwnd = NULL;
		sei.lpVerb = _T("runas");
		sei.lpFile = lpszProcessName;
		sei.lpParameters = (LPWSTR)(LPCWSTR)strCmdLine;
		sei.nShow = SW_SHOWNORMAL;

		bRet = ::ShellExecuteEx(&sei);

		hProcess = sei.hProcess;
	}
	else
	{
		bRet = ::CreateProcess(
			lpszProcessName, 
			(LPWSTR)(LPCWSTR)strCmdLine, 
			NULL, NULL, FALSE, 0, NULL, NULL, &si, &processInfo
			);

		if (bRet)
		{
			::CloseHandle(processInfo.hThread);
			hProcess = processInfo.hProcess;
		}
	}

	if (bRet)
	{
		if (bWaitProcess)
		{
			::WaitForSingleObject(hProcess, INFINITE);
		}
		::CloseHandle(hProcess);
	}

	return bRet;
}


void Log( LPCTSTR lpszFormat,... )
{
	va_list args;
	CString str;
	LPCTSTR lp1;
	//TCHAR tz1[255]={0};
	if(Log1!=NULL)
	{
		va_start(args,lpszFormat);
		str.FormatV(lpszFormat,args);
		va_end(args);
		lp1=str.GetString();
		try{
			Log1(lp1);
		}
		catch(...)
		{

		}
	}
}

DWORD CalcFileCrc(FILE* lpFile)
{
	const int nBuffLen = 500;
	BYTE cBuff[nBuffLen];
	DWORD dwCRC = 0;

	while ( TRUE )
	{
		size_t dwRead = fread(cBuff, 1, nBuffLen, lpFile);
		if ( dwRead == 0 )
		{
			break;
		}

		dwCRC = CRC32(dwCRC, cBuff, (DWORD)dwRead);
	}

	return dwCRC;
}

bool IsSkipDirectory(CString fn)
{
	bool ret=FALSE;
	for (int i=0;i<nSkipDirectoryCount;i++)
	{
		if(_tcsicmp(fn,arrSkipDirectoryName[i])==0)
		{
			ret=TRUE;
			break;
		}
	}
	return ret;
}

bool IsSkipFile(CString fn)
{
	bool ret=FALSE;
	for (int i=0;i<nSkipFileCount;i++)
	{
		if(_tcsicmp(fn,arrSkipFileName[i])==0)
		{
			ret=TRUE;
			break;
		}
	}
	return ret;
}

void DelDiffFile()
{
	CFileFind ff;  
	TCHAR tz1[MAX_PATH]={0};
	GetAppPath(tz1);
	if(tz1==NULL) return;
	CString sTmp=tz1;
	sTmp+=_T("\\*");
	BOOL bFound = ff.FindFile(sTmp,0);  
	CString strFile;
	CString sFilePath;
	BOOL bSpecialFile=FALSE;
	CString sHint;
	while(bFound)      // 递归拷贝
	{  
		bFound = ff.FindNextFile();  
		if( ff.IsDots() )  
			continue;
		strFile=ff.GetFileName();
		sFilePath=ff.GetFilePath();

		if(IsExistServer(strFile))
			continue;

		if( ff.IsDirectory() )
		{
			if(!IsSkipDirectory(strFile))
			{
				Log(_T("目录 %s 已经没有用,进行删除.\r\n"),strFile);
				DeleteDirectory(sFilePath.GetBuffer());
			}
		}
		else
		{
			if(!IsSkipFile(strFile))
			{
				Log(_T("文件 %s 已经没有用,进行删除.\r\n"),strFile);
				DeleteFile(sFilePath);
			}
		}
	}  
	ff.Close();
}

bool DeleteDirectory(TCHAR* DirName) 
{ 
	CFileFind tempFind;
	TCHAR tempFileFind[MAX_PATH];
	
	_stprintf(tempFileFind,_T("%s\\*.*"),DirName);
	BOOL IsFinded=(BOOL)tempFind.FindFile(tempFileFind);
	while(IsFinded)
	{
		IsFinded=(BOOL)tempFind.FindNextFile();
		if(!tempFind.IsDots())
		{
			TCHAR foundFileName[MAX_PATH];
			_tcscpy(foundFileName,tempFind.GetFileName().GetBuffer(MAX_PATH));
			if(tempFind.IsDirectory())
			{
				TCHAR tempDir[MAX_PATH];
				_stprintf(tempDir,_T("%s\\%s"),DirName,foundFileName);
				DeleteDirectory(tempDir);
			}
			else
			{
				TCHAR tempFileName[MAX_PATH];
				_stprintf(tempFileName,_T("%s\\%s"),DirName,foundFileName);
				DeleteFile(tempFileName);
			}

		}

	}
	tempFind.Close();
	if(!RemoveDirectory(DirName))
	{
		return FALSE;
	}
	return TRUE;
} 



bool IsExistServer( CString fn )
{
	bool ret=TRUE;
	PFILEUPDATESVRDATA pData=NULL;
	CPatchDlg *pDlg=(CPatchDlg*)theApp.GetMainWnd();
	fn.MakeUpper();
	pDlg->m_mapSvrFileVersion.Lookup(fn,(LPVOID&)pData);
	if(pData==NULL) 
		ret=false;
	return ret;
}

void ReadServiceSetting()
{
	TiXmlDocument doc;
	CString s1;
	CStringA s2;
    
	s1.AppendFormat(_T("%s\\ServiceSetting.xml"),sAppPath);
    s2=CT2A(s1);
	if(PathFileExists(s1))
	{
		doc.LoadFile(s2);
		TiXmlNode* pParent= doc.FirstChild("Settings");
        TiXmlNode* pChild= NULL;
		for ( pChild = pParent->FirstChild(); pChild != 0; pChild = pChild->NextSibling())   
		{  
			int t = pChild->Type();//获取此节点的类型   
			//搜索元素值
			if (TiXmlNode::ELEMENT == t && (stricmp("ServiceName",pChild->Value()) == 0))
			{   
				sPatchSvcName[0][0]=pChild->ToElement()->GetText();
			}  
			if (TiXmlNode::ELEMENT == t && (stricmp("ServiceExe",pChild->Value()) == 0))
			{   
				sPatchSvcName[0][1]=pChild->ToElement()->GetText();
			}  
		}  
	}
}

void CPatchDlg::OnGetMinMaxInfo(MINMAXINFO* lpMMI)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	lpMMI->ptMinTrackSize.x=568;
	lpMMI->ptMinTrackSize.y=398;
	lpMMI->ptMaxTrackSize.x=568;
	lpMMI->ptMaxTrackSize.y=398;
	__super::OnGetMinMaxInfo(lpMMI);
}

void CPatchDlg::OnOK()
{
	// TODO: 在此添加专用代码和/或调用基类

	//__super::OnOK();
}

void CPatchDlg::OnCancel()
{
	// TODO: 在此添加专用代码和/或调用基类
	if(isCanClose)
		__super::OnCancel();
    else
		MessageBox(_T("程序执行完毕会自动关闭,不能手工关闭."),_T("提示"));
	//
}

void CreateDesktopLink(CString sName,CString sPath )
{
	LPCTSTR LINK_FILE_NAME = sName;
	BOOL succeeded = FALSE;
	CoInitialize(0);
	do
	{
		CComPtr<IShellLinkW> pShellLink;
		HRESULT hr = CoCreateInstance(CLSID_ShellLink, NULL, CLSCTX_INPROC_SERVER, IID_IShellLink, (void**)&pShellLink);
		if(!SUCCEEDED(hr)) break;

		CComPtr<IPersistFile> pPersistFile;
		hr = pShellLink->QueryInterface(IID_IPersistFile, (void**)&pPersistFile);
		if(!SUCCEEDED(hr)) break;


		pShellLink->SetPath(sPath);

		TCHAR dstPath[MAX_PATH] = {0};
		{
			::SHGetSpecialFolderPath(NULL, dstPath, CSIDL_DESKTOPDIRECTORY, FALSE);
			::PathAddBackslash(dstPath);
			wcscat_s(dstPath, MAX_PATH, LINK_FILE_NAME);
			wcscat_s(dstPath, MAX_PATH, L".lnk");
		}

		hr = pPersistFile->Save(dstPath, FALSE);
		succeeded = SUCCEEDED(hr);
	}
	while(FALSE);
	CoUninitialize();
}
