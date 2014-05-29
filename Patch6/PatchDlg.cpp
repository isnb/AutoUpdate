
// PatchDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "Patch.h"
#include "PatchDlg.h"
#include "winsvc.h"
#include <tlhelp32.h>
#include "common/utility.h"                /* for GetLeidianLogPath */
#include "miniutil/bkini.h"                /* for BKIni::Document */
#include "miniutil/fileversion.h"
#include "bkupctl/bkupddownloader.h"
#include "bkupctl/bkupdwebfile.h"
#include "miniutil/szstr.h"
#include "tinyxml/tinyxml.h"
#include "zlibcrc32/crc32.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define  WM_UPDATE_HINT WM_USER+1

// CPatchDlg �Ի���

const CString  sUpdateDir=_T("Update");

//�����ļ� 
const int nSpecialFileCount=1;
const CString sSpecialFile[nSpecialFileCount]={
	_T("PATCH.EXE")
};

//��Ҫ���³���
const int nAppCount=2;
CString sPatchAppName[nAppCount][2]={
	_T("SyncProtect.exe"),_T("T"),
	_T("YS.Sync.WinUI.exe"),_T("F"),
};

//��Ҫ����Window����
const int nSvcCount=1;
const CString sPatchSvcName[nSvcCount][2]={
	_T("YSSYNC"),_T("Sync.exe"),
};


//�Ƿ����°汾
bool bFindNewVersion=FALSE;
//���ػ�ַ
CString sBaseAddress;
//���غ���·��
const CString sSavePath=L"Update\\";
//�ͻ��������ļ���
const CString sClientCfgFileName=L"updatelist.ini";
//����˵�ַ
CString sSvrPath=L"http://sync.yishion.com:8765/YSPOS/N_SYNC_V2/SyncUpdate.ini";
//Ӧ�ó���·��
CString sAppPath;


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


// CPatchDlg ��Ϣ�������

BOOL CPatchDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// ���ô˶Ի����ͼ�ꡣ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	// TODO: �ڴ���Ӷ���ĳ�ʼ������
	Log1=WriteLog;
	CAppPath::Instance().GetPath(sAppPath);
	AfxBeginThread(DoPatch,NULL);
	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CPatchDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ����������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
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

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
HCURSOR CPatchDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CPatchDlg::CheckVersion( BOOL *pbFind )
{
	CString strCfg;
	strCfg.AppendFormat(_T("%s\\%s%s"),sAppPath,sSavePath,sClientCfgFileName);

	bCrcChecekUp=TRUE;
	sCrcHint.Empty();
	// 1>���ط���������ļ���Update
	if(DownloadFile(sSvrPath,strCfg,0,FALSE)==S_OK){
	//	2>��ȡ�����ļ��е��ļ��б�Ͱ汾��.
	ParseServerCfg(strCfg);
	//	3>��ȡ������ģ����ļ����Ͱ汾��
	GetMoudleList();
    //	4>�Ƚϰ汾,�и߰汾,��������־.���ص�Update
	//CompareVersion();
	//���ݾɵġ���
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

	if (FAILED(hr))
		return hr;
    ini.LoadFile(cfgPath);
	if (!ini.HasSection(L"config"))
		return E_FAIL;
    section=ini[L"config"];
	//��ʼ��
	nFileCount=section[L"maxFileCount"].GetValueDWORD();
	sBaseAddress=section[_T("baseAddress")].GetValueString();
	section=ini[L"file"];
	m_mapSvrFileVersion.RemoveAll();
	Log(_T("��������˵��ļ��б�,��ַ%s,����ļ���%d\r\n"),sBaseAddress,nFileCount);
	for (int i=1; i<nFileCount; ++i)
	{
		wchar_t szItem[10] = {0};
		wsprintf(szItem, L"f%d", i);
		CString 
			str,
			sFile,
			sVer,
			sTmp;
		DWORD dwCrc=0;
		if (!section.HasKey(szItem))
			break;
        str=section[szItem].GetValueString();
		PFILEUPDATESVRDATA pData=new FILEUPDATESVRDATA();
	    AfxExtractSubString(sFile,str,0,L',');
		AfxExtractSubString(pData->sFilePath,str,1,L',');
        AfxExtractSubString(sVer,str,2,L',');
		AfxExtractSubString(sTmp,str,3,L',');
		_stscanf_s(sTmp.GetString(),_T("%x"),&dwCrc);
		pData->sFileName=sFile;
		pData->sVersion=sVer;
		pData->nCrc32=dwCrc;
		m_mapSvrFileVersion.SetAt(sFile.MakeUpper(),pData);
		//Log(_T("������ļ�:%-25s,�汾:%10s\r\n"),pData->sFileName,pData->sVersion);
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
	//�ȱȽ������ļ��Ƿ����
	if(PathFileExists(savePath))
	{
		dwCliCrc=CalcCrc(savePath);
		if(dwCliCrc!=crc) //У�鲻ͨ������������
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
		sCrcHint.AppendFormat(_T("%-38sУ�鲻ͨ��,�ͻ���:%08x,�����:%08x.\r\n"),
			sFileName,dwCliCrc,crc);
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
	//���ݷ�����ļ��б�.�ҵ���ǰĿ¼,�ļ��汾
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
	//sUpdataPath.AppendFormat(_T("%s\\%s"),sAppPath,sSavePath);
	TiXmlDocument doc;
	TiXmlElement *root=new TiXmlElement("dsFileVersion");
	TiXmlDeclaration *d=new TiXmlDeclaration("1.0","utf-8","");
	
	doc.LinkEndChild(d);
	doc.LinkEndChild(root);
	
	TiXmlElement 
		*nodeId,
		*nodeFn,
		*nodeFv;
	do{
		//bDiff=FALSE;
		pData=NULL;
		bFound=FALSE;
		sFilePath.Empty();
		sVersion.Empty();
		m_mapSvrFileVersion.GetNextAssoc(pos,sKey,(LPVOID&)pData);
		sFilePath.AppendFormat(_T("%s\\%s"),sAppPath,pData->sFileName);
		bFound=PathFileExists(sFilePath.GetString());
		if(bFound)
		{
			pCliData=new FILEUPDATECLIDATA();
			GetFileVersion(sFilePath.GetString(),sVersion);
			sFileName=pData->sFileName;
			pCliData->sFileName=sFileName;
			if(!sVersion.IsEmpty()) //��Щ�ļ�û�а汾��
				pCliData->sVersion=sVersion;
			pCliData->nCrc32=CalcCrc(sFilePath);
			m_mapCliFileVersion.SetAt(sFileName.MakeUpper(),pCliData);
			TiXmlElement *eTable=new TiXmlElement("sc_FileVersion");
			root->LinkEndChild(eTable);
			nodeId=new TiXmlElement("ID");
			nodeFn=new TiXmlElement("FileName");
			nodeFv=new TiXmlElement("FileVersion");
			TiXmlText *fnContent =new TiXmlText(CT2A(pCliData->sFileName.GetString()));
			TiXmlText *fvContent =new TiXmlText(CT2A(pCliData->sVersion.GetString()));
			nodeFn->LinkEndChild(fnContent);
			nodeFv->LinkEndChild(fvContent);
			eTable->LinkEndChild(nodeId);
			eTable->LinkEndChild(nodeFn);
			eTable->LinkEndChild(nodeFv);
			//Log(_T("�ͻ����ļ�:%-25s,�汾:%10s\r\n"),pCliData->sFileName,pCliData->sVersion);
		}
	    
	}while(pos!=NULL);
	TiXmlPrinter printer;
	doc.Accept(&printer);
	CStringA str2=printer.CStr();
	CString str=CA2T(str2);
	
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
	sUpdataPath.AppendFormat(_T("%s\\%s"),sAppPath,sSavePath);	
	do{
		bDiff=FALSE;
		pCliData=NULL;
		m_mapSvrFileVersion.GetNextAssoc(pos,sKey,(LPVOID&)pData);
		m_mapCliFileVersion.Lookup(sKey,(LPVOID&)pCliData);
		if(pCliData==NULL)
		{
			bDiff=TRUE;
			Log(_T("S:%-25s, S:%8s,�ͻ���û��,"),pData->sFileName,
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
				if(!bDiff) //�汾һ���Ƚ�Crc32
				{
					if(pCliData->nCrc32!=pData->nCrc32)
						bDiff=TRUE;
				}

			}
			if(bDiff)
			{
				Log(_T("S:%-25s, C:%-25s, S:%8s, C:%8s,"),pData->sFileName,pCliData->sFileName,
				pData->sVersion,pCliData->sVersion);
				//�ͻ��˴��ڵ��ļ� ����Ϊ�ҵ��°汾..
				if(!bFindNewVersion && !IsSpecialFile(pCliData->sFileName))
					bFindNewVersion=TRUE;
			}
		}
		if(bDiff)
		{
			hr=DownloadFile(sBaseAddress+pData->sFilePath,sUpdataPath+pData->sFileName,TRUE,pData->nCrc32);
			if(hr==S_OK)
				Log(_T("���سɹ���\r\n"));
			else
				Log(_T("����ʧ�ܡ�\r\n"));
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
	_tcscpy_s(lptzHint,size,sCrcHint.GetString());
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

//���¹��ܺ���
UINT DoPatch( LPVOID lp1 )
{
	//�����Ƿ����
	BOOL bIsExistApp=TRUE;
	BOOL bIsRunSvc=TRUE;
	int nTmp=0;
	bool bTmp=TRUE;
	//ɱ����ֹͣ�������Դ���
	int nRetryKillCount=0;
	int nRetryKillMaxCount=5;
	CString str;
	TCHAR tz1[MAX_PATH]={0};
	CString sTmp;
	BOOL bFind=FALSE;
	BOOL bCheck=FALSE;
	int nHintCount=10;
	UINT ret=S_OK;

	CPatchDlg *pDlg=NULL;
	pDlg=(CPatchDlg*)theApp.GetMainWnd();

	//����°汾
	pDlg->CheckVersion(&bFind);
	if(bFind)
	{
		Log(_T("�����°汾.�����ļ�����!\r\n"));
		pDlg->GetCrcInfo(&bCheck,tz1,MAX_PATH);
		if(bCheck)
		{
			Log(_T("����ͨ��!\r\n"));
			//����Ƿ��и���

			//���Ҫ���½����Ƿ��˳�
			nTmp=0;
			Log(_T("���ڼ������µĳ����Ƿ��Ѿ��˳�!\r\n"));
			for (int i=0;i<nAppCount;i++)
			{
				nTmp=GetProcessIdByName(sPatchAppName[i][0].GetString());
				if(nTmp==0)  
				{
					//û�������ģ���Ĭ�ϡ�
					continue;
				}
				else
				{
					//���������� ��ΪT��
					sPatchAppName[i][1]=_T("T");
				}
				nRetryKillCount=0;
				Sleep(1000);
				do 
				{
					Sleep(1000);
					KillProcess(nTmp);
					nTmp=GetProcessIdByName(sPatchAppName[i][0].GetString());
					nRetryKillCount++;
				} while (nTmp!=0 && nRetryKillCount<nRetryKillMaxCount);

				if(nRetryKillCount>=nRetryKillMaxCount)
				{
					Log(_T("����ɱ������%sʧ��.�Ѿ�����%d��.���³����ܼ���.\r\n"),sPatchAppName[i][0].GetString(),nRetryKillCount);
					ret=S_FALSE;
					goto  Done;
				}
			}
			Log(_T("����µĳ����Ѿ��˳�!\r\n"));
			//ֹͣ����
			Log(_T("���ڼ������µķ����Ƿ��Ѿ�ֹͣ!\r\n"));
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
					Sleep(1000); //��ͣһ�¡�
					if(QueryServiceStatus_dan(sPatchSvcName[i][0].GetString(),SERVICE_RUNNING) ||
						QueryServiceStatus_dan(sPatchSvcName[i][0].GetString(),SERVICE_START_PENDING)||
						QueryServiceStatus_dan(sPatchSvcName[i][0].GetString(),SERVICE_STOP_PENDING))
					{
						nTmp=GetProcessIdByName(sPatchSvcName[i][1].GetString());
						if(nTmp>0)
							KillProcess(nTmp);
					}
				}

				if(nRetryKillCount>=nRetryKillMaxCount)
				{
					str.Empty();
					str.AppendFormat(_T("����ֹͣ����%sʧ��.�Ѿ�����%d��.���³����ܼ���.\r\n"),sPatchSvcName[i][0].GetString(),nRetryKillCount);
					Log(str.GetBuffer());
					ret= S_FALSE;
					goto  Done;
				}
			}
			Log(_T("����µķ����Ѿ�ֹͣ!\r\n"));
			//�ƶ��ļ� 
			Log(_T("���и����ļ�����.\r\n"));
			GetAppPath(tz1);
			sTmp.AppendFormat(_T("%s\\%s"),tz1,sUpdateDir);
			if(MoveDirectory(sTmp.GetString(),tz1))
			{
				Log(_T("�����ļ����.\r\n"));
				//�������������³���
				for (int i=0;i<nAppCount;i++)
				{
					str.Empty();
					str.AppendFormat(_T("%s\\%s"),sAppPath,sPatchAppName[i][0].GetString());
					//F������
					if(sPatchAppName[i][1]==_T("F")) continue;
					if(Execute(str,NULL,FALSE,FALSE))
					{
						Log(_T("����%s�����ɹ�.\r\n"),sPatchAppName[i][0].GetString());
					}
				}
				//��������
				for(int i=0;i<nSvcCount;i++)
				{
					if(StartService_dan((LPVOID)sPatchSvcName[i][0].GetString()))
					{
						Log(_T("����%s�����ɹ�.\r\n"),sPatchSvcName[i][0].GetString());
					}

				}
				ret=S_OK;
			}
			else
			{
				Log(_T("�����ļ�ʧ��.���³����ܼ���.\r\n"));
				ret= S_FALSE;
			}
		}
		else
		{
			Log(_T("���鲻ͨ��!\r\n%s"),tz1);
		}
	}
	else
	{
		Log(_T("���ڼ�����״̬!\r\n"));
		CheckSyncSvc();
	}

Done:
	for(int i=nHintCount;i>0;i--)
	{
		Log(_T("%d ���رա�\r\n"),i);
		Sleep(1000);
	}
	pDlg->isCanClose=TRUE;
	pDlg->SendMessage(WM_CLOSE);
	return ret;
}




DWORD GetProcessIdByName( LPCTSTR lpFilename)
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
		//����szExeFile �е�һ�γ���proName��λ�á�����=������
		if(_tcsstr(pe32.szExeFile,proName))
		{
			dwCount=pe32.th32ProcessID;
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

//��������Ȩ��
bool   WINAPI Adjust(TCHAR *PrivilegeName,bool lEnabled) 
{
	HANDLE hToken;
	TOKEN_PRIVILEGES tp;
	LUID luid;
	//�򿪽������ƻ�
	if(!OpenProcessToken(GetCurrentProcess(),TOKEN_ADJUST_PRIVILEGES|TOKEN_QUERY,&hToken))
	{
		return false;
	}
	//��ý��̱���ΨһID
	if(!LookupPrivilegeValue(NULL,PrivilegeName,&luid))
	{
		return false;
	}
	tp.PrivilegeCount=1;
	tp.Privileges[0].Attributes=lEnabled?SE_PRIVILEGE_ENABLED:0;
	tp.Privileges[0].Luid=luid;
	//��������Ȩ��
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
	AfxExtractSubString(sn,str,0,L',');
	AfxExtractSubString(sf,str,1,L',');
	// �򿪷���������
	SC_HANDLE hSC = ::OpenSCManager( NULL, 
		NULL, GENERIC_EXECUTE);
	if( hSC == NULL)
	{
		Log(_T("�򿪷������������\r\n"));
		return FALSE;
	}

	SC_HANDLE hSvc = ::OpenService(hSC,sn,
		SERVICE_START | SERVICE_QUERY_STATUS | SERVICE_STOP);
	if(hSvc == NULL)
	{
		Log(_T("�򿪷������\r\n"));
		::CloseServiceHandle( hSC);
		return FALSE;
	}

	SERVICE_STATUS status;
	if( ::QueryServiceStatus( hSvc, &status) == FALSE)
	{
		Log(_T("��ѯ����״̬����\r\n"));
		::CloseServiceHandle( hSvc);
		::CloseServiceHandle( hSC);
		return FALSE;
	}

	//�������ֹͣ״̬���������񣬷���ֹͣ����
	if( status.dwCurrentState == SERVICE_RUNNING)
	{
STOP:
		if(::ControlService( hSvc, 
				SERVICE_CONTROL_STOP, &status) == TRUE)
			
		Log(_T("����ֹͣ����,���ȴ�����ֹͣ��\r\n"));
		// �ȴ�����ֹͣ
		nRetry=0;
		while( ::QueryServiceStatus( hSvc, &status) == TRUE)
		{
			::Sleep( status.dwWaitHint);
			if( status.dwCurrentState == SERVICE_STOPPED)
			{
				Log(_T("����ɹ�ֹͣ��\r\n"));
				break;
			}
			if( status.dwCurrentState == SERVICE_RUNNING)
			{
				goto STOP;
			}
			
			if(nRetry==nMaxRetry)
			{
				Log(_T("����ֹͣ����,�ȴ�̫��,���Խ�������.\r\n"));
				KillProcess(GetProcessIdByName(sf));
			}
			nRetry++;
		}
	}
	else
	{
		Log(_T("����û������!\r\n"));
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
	// �򿪷���������
	SC_HANDLE hSC = ::OpenSCManager( NULL, 
		NULL, GENERIC_EXECUTE);
	if( hSC == NULL)
	{
		Log(_T("�򿪷������������\r\n"));
		return FALSE;
	}

	SC_HANDLE hSvc = ::OpenService(hSC,sn,
		SERVICE_START | SERVICE_QUERY_STATUS | SERVICE_STOP);
	if(hSvc == NULL)
	{
		Log(_T("�򿪷������\r\n"));
		::CloseServiceHandle( hSC);
		return FALSE;
	}
	SERVICE_STATUS status;
	if( ::QueryServiceStatus( hSvc, &status) == FALSE)
	{
		Log(_T("��ѯ����״̬����\r\n"));
		::CloseServiceHandle( hSvc);
		::CloseServiceHandle( hSC);
		return FALSE;
	}

	if(status.dwCurrentState == SERVICE_STOP ||
		status.dwCurrentState== SERVICE_STOPPED)
	{
		Log(_T("���������С����Ե�!\r\n"));
		// ��������
		if( ::StartService( hSvc, NULL, NULL) == FALSE)
		{
			Log(_T("��������������\r\n"));
			::CloseServiceHandle( hSvc);
			::CloseServiceHandle( hSC);
			return FALSE;
		}
		// �ȴ���������
		nRetry=0;
		while( ::QueryServiceStatus( hSvc, &status) == TRUE)
		{
			::Sleep( status.dwWaitHint);
			// ��������
			if(status.dwCurrentState == SERVICE_STOP)
			{
				::StartService( hSvc, NULL, NULL) ;
				nRetry++;
			}

			if( status.dwCurrentState == SERVICE_RUNNING)
			{
				Log(_T("����ɹ�������\r\n"));
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
			Log(_T("�����Ѿ�������\r\n"));
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
	

	SC_HANDLE hSC = ::OpenSCManager( NULL, 
		NULL, GENERIC_EXECUTE);
	if( hSC == NULL)
	{
		Log(_T("�򿪷������������\r\n"));
		return FALSE;
	}

	SC_HANDLE hSvc = ::OpenService(hSC,sPatchSvcName[0][0].GetString(),
		SERVICE_START | SERVICE_QUERY_STATUS | SERVICE_STOP);
	if(hSvc == NULL)
	{
		Log(_T("�򿪷������\r\n"));
		::CloseServiceHandle( hSC);
		return FALSE;
	}
	SERVICE_STATUS status;
	if( ::QueryServiceStatus( hSvc, &status) == FALSE)
	{
		Log(_T("��ѯ����״̬����\r\n"));
		::CloseServiceHandle( hSvc);
		::CloseServiceHandle( hSC);
		return FALSE;
	}

	if(status.dwCurrentState == SERVICE_STOP ||
		status.dwCurrentState== SERVICE_STOPPED)
	{
		Log(_T("����ֹͣ,�����С����Ե�!\r\n"));
		// ��������
		if( ::StartService( hSvc, NULL, NULL) == FALSE)
		{
			Log(_T("��������������\r\n"));
			::CloseServiceHandle( hSvc);
			::CloseServiceHandle( hSC);
			return FALSE;
		}
		// �ȴ���������
		nRetry=0;
		while( ::QueryServiceStatus( hSvc, &status) == TRUE)
		{
			::Sleep( status.dwWaitHint);
			// ��������
			if(status.dwCurrentState == SERVICE_STOP ||
				status.dwCurrentState== SERVICE_STOPPED)
			{
				::StartService( hSvc, NULL, NULL) ;
				nRetry++;
			}

			if( status.dwCurrentState == SERVICE_RUNNING)
			{
				Log(_T("����ɹ�������\r\n"));
				break;
			}
			if(nRetry==nMaxRetry)
			{
                Log(_T("���Դ���̫��,����������!\r\n"));
				break;
			}
		}
	}
	else
	{
		if( status.dwCurrentState == SERVICE_RUNNING)
		{
			Log(_T("�����Ѿ�������\r\n"));
		}
	}
	::CloseServiceHandle( hSvc);
	::CloseServiceHandle( hSC);
	return TRUE;
}

// ��pc�е��ļ��д�һ��Ŀ¼�����������һ��Ŀ¼
BOOL MoveDirectory(CString strSrcPath, CString strDesPath)
{
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

	BOOL bRet = FALSE; // ��ΪԴĿ¼������Ϊ�գ����Ը�ֵһ���ᱻ�޸�
	CFileFind ff;  
	BOOL bFound = ff.FindFile(strSrcPath+_T("*"),   0);  
	CString strFile;
	BOOL bSpecialFile=FALSE;
	while(bFound)      // �ݹ鿽��
	{  
		bFound = ff.FindNextFile();  
		bSpecialFile=FALSE;
		if( ff.IsDots() )  
			continue;

		CString strSubSrcPath = ff.GetFilePath();
		CString strSubDespath = strSubSrcPath;
		strSubDespath.Replace(strSrcPath, strDesPath);

		if( ff.IsDirectory() )
			bRet = MoveDirectory(strSubSrcPath, strSubDespath);     // �ݹ鿽���ļ���
		else
		{
			strFile=PathFindFileName(strSubSrcPath);
			strFile.MakeUpper();
			for (int i=0;i<nSpecialFileCount;i++)
			{
				//�ҵ������ļ�
				if(_tcscmp(strFile.GetString(),sSpecialFile[i])==0)
				{
					bSpecialFile=TRUE;
					break;
				}	
			}
			if(bSpecialFile)
				bRet=MoveFileEx( strSubSrcPath,strSubDespath,MOVEFILE_DELAY_UNTIL_REBOOT|MOVEFILE_REPLACE_EXISTING);
			else
				bRet = MoveFileEx(strSubSrcPath, strSubDespath,MOVEFILE_REPLACE_EXISTING);   // �ƶ��ļ�
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
	// �򿪷���������
	SC_HANDLE hSC = ::OpenSCManager( NULL, 
		NULL, GENERIC_EXECUTE);
	if( hSC == NULL)
	{
		Log(_T("�򿪷������������\r\n"));
		return FALSE;
	}

	SC_HANDLE hSvc = ::OpenService(hSC,sn,
		SERVICE_START | SERVICE_QUERY_STATUS | SERVICE_STOP);
	if(hSvc == NULL)
	{
		Log(_T("�򿪷������\r\n"));
		::CloseServiceHandle( hSC);
		return FALSE;
	}
	SERVICE_STATUS status1;
	if( ::QueryServiceStatus( hSvc, &status1) == FALSE)
	{
		Log(_T("��ѯ����״̬����\r\n"));
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

void CPatchDlg::OnGetMinMaxInfo(MINMAXINFO* lpMMI)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
	lpMMI->ptMinTrackSize.x=568;
	lpMMI->ptMinTrackSize.y=398;
	lpMMI->ptMaxTrackSize.x=568;
	lpMMI->ptMaxTrackSize.y=398;
	__super::OnGetMinMaxInfo(lpMMI);
}

void CPatchDlg::OnOK()
{
	// TODO: �ڴ����ר�ô����/����û���

	//__super::OnOK();
}

void CPatchDlg::OnCancel()
{
	// TODO: �ڴ����ר�ô����/����û���
	if(isCanClose)
		__super::OnCancel();
    else
		MessageBox(_T("����ִ����ϻ��Զ��ر�,�����ֹ��ر�."),_T("��ʾ"));
	//
}
