// AutoUpdateCommon.cpp : ���� DLL �ĳ�ʼ�����̡�
//

#include "stdafx.h"
#include "AutoUpdateCommon.h"
#include "Extern.h"
#include "miniutil/bkini.h"                /* for BKIni::Document */
#include "miniutil/fileversion.h"
#include "common/utility.h"                /* for GetLeidianLogPath */
#include "bkupctl/bkupddownloader.h"
#include "bkupctl/bkupdwebfile.h"
#include <tlhelp32.h>
#include "miniutil/szstr.h"
#include "tinyxml/tinyxml.h"
#include "zlibcrc32/crc32.h"
#include "../SyncProtectDll/Extern.h"

#pragma comment(lib,"Version.lib")

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

/*
1>���ط���������ļ���Update
2>��ȡ�����ļ��е��ļ��б�Ͱ汾��.
3>��ȡ������ģ����ļ����Ͱ汾��
4>�Ƚϰ汾,�и߰汾,��������־.���ص�Update
5>�ж�������־.��������,���˳�
*/

//�Ƿ����°汾
bool bFindNewVersion=FALSE;
//���ػ�ַ
CString sBaseAddress;
//���غ���·��
const CString sSavePath=L"Update\\";
//�ͻ��������ļ���
const CString sClientCfgFileName=L"updatelist.ini";
//����˵�ַ
CString sSvrPath=L"http://pos.yishion.com:8765/YSPOS/N_SYNC_V2/SyncUpdate.ini";
//Ӧ�ó���·��
CString sAppPath;




BEGIN_MESSAGE_MAP(CAutoUpdateCommonApp, CWinApp)
END_MESSAGE_MAP()


// CAutoUpdateCommonApp ����

CAutoUpdateCommonApp::CAutoUpdateCommonApp()
{
	// TODO: �ڴ˴���ӹ�����룬
	nCrc32Retry=5;
	bCrcChecekUp=TRUE;
	// ��������Ҫ�ĳ�ʼ�������� InitInstance ��
}


// Ψһ��һ�� CAutoUpdateCommonApp ����

CAutoUpdateCommonApp theApp;


// CAutoUpdateCommonApp ��ʼ��

BOOL CAutoUpdateCommonApp::InitInstance()
{
	CWinApp::InitInstance();
	CAppPath::Instance().GetPath(sAppPath);
	return TRUE;
}

//
//void CAutoUpdateCommonApp::CheckVersion(int id, BOOL *pbFind )
//{
//	CString strCfg;
//	strCfg.AppendFormat(_T("%s\\%s%s"),sAppPath,sSavePath,sClientCfgFileName);
//	bCrcChecekUp=TRUE;
//	sCrcHint.Empty();
//	BOOL bAllow=FALSE;
//
//	// 1>���ط���������ļ���Update
//	if(DownloadFile(sSvrPath,strCfg,0,FALSE)==S_OK){
//		CheckAllowUpdate(id,&bAllow);
//		if(bAllow)
//		{
//			//	2>��ȡ�����ļ��е��ļ��б�Ͱ汾��.
//			ParseServerCfg(strCfg);
//			//	3>��ȡ������ģ����ļ����Ͱ汾��
//			GetMoudleList();
//			//	4>�Ƚϰ汾,�и߰汾,��������־.���ص�Update
//			//CompareVersion();
//			//���ݾɵġ���
//			DownloadDiffFile();
//
//			if(!bFindNewVersion) //û�ҵ��������£����·����
//			{
//				SaveAllowUpdate(id);
//			}
//		}
//	}
//    *pbFind=bFindNewVersion;
//	
//}

void CAutoUpdateCommonApp::CheckVersion(BOOL *pbFind )
{
	CString strCfg;
	strCfg.AppendFormat(_T("%s\\%s%s"),sAppPath,sSavePath,sClientCfgFileName);
	bCrcChecekUp=TRUE;
	sCrcHint.Empty();
	BOOL bAllow=FALSE;

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

		//if(!bFindNewVersion) //û�ҵ��������£����·����
		//{
		//	SaveAllowUpdate(id);
		//}
		
	}
	*pbFind=bFindNewVersion;

}

HRESULT CAutoUpdateCommonApp::ParseServerCfg(CString cfgPath)
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

HRESULT CAutoUpdateCommonApp::DownloadFile(CString srvPath,CString savePath,BOOL checkCrc,DWORD crc)
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
	return hr;
}

BOOL CAutoUpdateCommonApp::DownloadProcess( DWORD dwTotalSize, DWORD dwReadSize )
{
	return TRUE;
}

HRESULT CAutoUpdateCommonApp::GetMoudleList()
{
//	HANDLE hPSnap;
	/*MODULEENTRY32 mo32;*/
	DWORD ret=0;
	//DWORD dwProcessId=0;
	//TCHAR szFullPath[MAX_PATH];
	//DWORD dwVerInfoSize=0;
//	DWORD dwVerHnd;
	//CString strVersion;
	//CString sFileName;
//	VS_FIXEDFILEINFO * pFileInfo;
	//mo32.dwSize=sizeof(MODULEENTRY32);	

	/* �������й� ��ģ�鼯
	dwProcessId=GetCurrentProcessId();
	hPSnap=CreateToolhelp32Snapshot(TH32CS_SNAPMODULE,dwProcessId);
	if(hPSnap == INVALID_HANDLE_VALUE) return 0;
	if(!Module32First(hPSnap,&mo32))
	{
		CloseHandle(hPSnap);
		return 0;
	}
	do 
	{
		if(mo32.th32ProcessID==dwProcessId)
		{
			//mo32.hModule
			GetModuleFileName(mo32.hModule, szFullPath, sizeof(szFullPath));
			//ȡ�汾��С
			dwVerInfoSize = GetFileVersionInfoSize(szFullPath, &dwVerHnd);
			if (dwVerInfoSize)
			{
				// If we were able to get the information, process it:
				HANDLE  hMem;
				LPVOID  lpvMem;
				unsigned int uInfoSize = sizeof(VS_FIXEDFILEINFO);

				hMem = GlobalAlloc(GMEM_MOVEABLE, dwVerInfoSize);
				lpvMem = GlobalLock(hMem);
				GetFileVersionInfo(szFullPath, dwVerHnd, dwVerInfoSize, lpvMem);
				::VerQueryValue(lpvMem, (LPTSTR)_T("\\"), (void**)&pFileInfo, &uInfoSize);
				WORD m_nProdVersion[4];
				// Product version from the FILEVERSION of the version info resource 
				m_nProdVersion[0] = HIWORD(pFileInfo->dwProductVersionMS); 
				m_nProdVersion[1] = LOWORD(pFileInfo->dwProductVersionMS);
				m_nProdVersion[2] = HIWORD(pFileInfo->dwProductVersionLS);
				m_nProdVersion[3] = LOWORD(pFileInfo->dwProductVersionLS); 
				
				strVersion.Format(_T("%d.%d.%d.%d"),m_nProdVersion[0],
					m_nProdVersion[1],m_nProdVersion[2],m_nProdVersion[3]);
				GlobalUnlock(hMem);
				GlobalFree(hMem);
				sFileName=PathFindFileName(szFullPath);
				PFILEUPDATECLIDATA pData=new FILEUPDATECLIDATA();
				pData->sFileName=sFileName.MakeUpper();
				pData->sVersion=strVersion;
				m_mapCliFileVersion.SetAt(pData->sFileName,pData);
				Log(_T("�ͻ����ļ�:%s,�汾:%s\r\n"),pData->sFileName,pData->sVersion);
			}
		}
	} while (Module32Next(hPSnap,&mo32));
	CloseHandle(hPSnap);
	*/

	//����йܴ������. ����ص��йܴ���..����ע��
	//int nAdd=(int)szFullPath;
	//if(GetModule1!=NULL)
	//{
	//	GetModule1((int)&nAdd);
	//	//Log(_T("%s"),(LPTSTR)nAdd);
	//	CString str=(LPTSTR)nAdd;
	//

	///*CString str=L"<?xml version=\"1.0\" encoding=\"utf-16\"?> \
	//<Modules> \
	//<add name=\"Test.exe\" version=\"1.0.0.0\" />\
	//<add name=\"Ys.Sync.Model.dll\" version=\"1.0.1.0\" />\
	//<add name=\"Common.dll\" version=\"1.0.1.0\" />\
	//</Modules>";*/
	//
	//TiXmlDocument doc;
	//doc.Parse(CT2A(str.GetString()),NULL,TIXML_ENCODING_UTF8);
	//TiXmlElement *RootElement = doc.RootElement();
	//TiXmlElement *node=RootElement->FirstChildElement();

	//while(node!=NULL)
	//{
	//	PFILEUPDATECLIDATA pData=new FILEUPDATECLIDATA();
	//	TiXmlAttribute *NameAttribute = node->FirstAttribute();
	//	TiXmlAttribute *VerAttribute=NameAttribute->Next();

	//	strVersion=CA2T(VerAttribute->Value());
	//	sFileName=CA2T(NameAttribute->Value());
	//	pData->sFileName=sFileName;
	//	pData->sVersion=strVersion;
	//	m_mapCliFileVersion.SetAt(sFileName.MakeUpper(),pData);
	//	//Log(_T("�ͻ����ļ�:%-25s,�汾:%10s\r\n"),pData->sFileName,pData->sVersion);
	//	node=node->NextSiblingElement();
	//}
	//}

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
			//eTable->LinkEndChild(nodeId);
			eTable->LinkEndChild(nodeFn);
			eTable->LinkEndChild(nodeFv);
			//Log(_T("�ͻ����ļ�:%-25s,�汾:%10s\r\n"),pCliData->sFileName,pCliData->sVersion);
		}
	    
	}while(pos!=NULL);
	TiXmlPrinter printer;
	doc.Accept(&printer);
	CStringA str2=printer.CStr();
	CString str=CA2T(str2);
	if(GetModule1!=NULL)
	{
	   GetModule1((int)str.GetString());
	}
	return ret;
}

HRESULT CAutoUpdateCommonApp::DownloadDiffFile()
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
				bDiff=VersionLess(pCliData->sVersion,pData->sVersion);
			if(bDiff)
			{
				Log(_T("S:%-25s, C:%-25s, S:%8s, C:%8s,"),pData->sFileName,pCliData->sFileName,
				pData->sVersion,pCliData->sVersion);
				//�ͻ��˴��ڵ��ļ� ����Ϊ�ҵ��°汾..
				if(!bFindNewVersion)
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

HRESULT CAutoUpdateCommonApp::CompareVersion()
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
			Log(_T("S:%-25s, S:%10s,�ͻ���û��,��Ҫ����!\r\n"),pData->sFileName,
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
				bDiff=VersionLess(pCliData->sVersion,pData->sVersion);
			if(bDiff)
			{
				Log(_T("S:%-25s, C:%-25s, S:%10s, C:%10s,��Ҫ����!\r\n"),pData->sFileName,pCliData->sFileName,
					pData->sVersion,pCliData->sVersion);
				//�ͻ��˴��ڵ��ļ� ����Ϊ�ҵ��°汾..
				if(!bFindNewVersion)
				{
					bFindNewVersion=TRUE;
					break;
				}
			}
		}
	}while(pos!=NULL);
	return hr;
}



void CAutoUpdateCommonApp::SetUpdateAddress( LPTSTR lpAppPath )
{
	sSvrPath=lpAppPath;
}

int CAutoUpdateCommonApp::CalcCrc( CString fileName )
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

void CAutoUpdateCommonApp::GetCrcInfo( BOOL *checkup,LPTSTR lptzHint,int size )
{
	*checkup=bCrcChecekUp;
	_tcscpy_s(lptzHint,size,sCrcHint.GetString());
}

void CAutoUpdateCommonApp::CheckAllowUpdate(int id,BOOL *allow )
{
	int nc=0;
	int ns=0;
	*allow=FALSE;

	BOOL r=GetSvrUpdateVer(&id,&ns,&nc);
	if(r)
	{
		if(ns>nc)
			*allow=TRUE;
	}
	else
	{
		*allow=FALSE;
	}
}

void CAutoUpdateCommonApp::SaveAllowUpdate(int id)
{
	if(!bFindNewVersion)
		SaveSvrUpdateVer(id);
}

//�Ƚϰ汾.
//BOOL VersionLess(LPCTSTR lpszVersionLeft, LPCTSTR lpszVersionRight)
//{
//	ULARGE_INTEGER ullVerLeft  = ::GetDigitalVersionByString(lpszVersionLeft);
//	ULARGE_INTEGER ullVerRight = ::GetDigitalVersionByString(lpszVersionRight);
//	return (ullVerLeft.QuadPart < ullVerRight.QuadPart);
//}

//ULARGE_INTEGER GetDigitalVersionByString(LPCTSTR lpszVersion)
//{
//	ULARGE_INTEGER ullRet = {0};
//	CString strVersion = lpszVersion;
//	int nPos[4] = {0, 0, 0, 0};
//
//	for (int i = 1; i < 4; i ++)
//	{
//		nPos[i] = strVersion.Find(_T('.'), nPos[i - 1]) + 1;
//	}
//
//	ullRet.HighPart = (DWORD)MAKELONG(_szttoi(strVersion.Mid(nPos[1])), _szttoi(strVersion));
//	ullRet.LowPart = (DWORD)MAKELONG(_szttoi(strVersion.Mid(nPos[3])), _szttoi(strVersion.Mid(nPos[2])));
//
//	return ullRet;
//}

void Log( LPTSTR lpszFormat,... )
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
	/*if(Log1!=NULL)
	{
		Log1(1);
	}*/
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

//ȡ�ļ��汾

