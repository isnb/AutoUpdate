#include "stdafx.h"
#include "manualUpdater.h"
#include "bkupctl/bkupddownloader.h"       /* for CBkUpdDownloader */
#include "bkupctl/bkupclterr.h"
#include "miniutil/fileversion.h"          /* for VersionLess() */
#include "miniutil/bkini.h"                /* for BKIni::Document */
#include "zlib/zlibimp.h"                  /* for CZlib */
dian#include "common/utility.h"                /* for GetLeidianLogPath */
#include "zlibcrc32/crc32.h"               /* for CRC32 */
#include "winmod/wintrustverifier.h"       /* for VerifyFile */
//#include "beikesafemsgbox.h"               /* for CBkSafeMsgBox */
#include <winhttp.h>
#include "define.h"

#pragma comment(lib,"Version.lib")

#define UPDATE_INI_URL      _T("http://172.16.1.136:8888/update/YsIMupdate.ini")


CManualUpdater::CManualUpdater() 
: m_hWnd(NULL), 
  m_hThread(NULL),
  m_fCompress(FALSE), 
  m_dwFileSize(0)
{
	m_BKSafeLog.SetLogForModule(BKSafeLog::enumBKSafeUpliveLog);
	m_hStopEvent = ::CreateEvent(NULL, TRUE, FALSE, NULL);
}

CManualUpdater::~CManualUpdater()
{
	if (m_hStopEvent)
		::CloseHandle(m_hStopEvent);

	if (m_hThread)
		::CloseHandle(m_hThread);
}

BOOL CManualUpdater::DownloadProcess(DWORD dwTotalSize, DWORD dwReadSize)
{
	if (dwTotalSize == 0)
		return TRUE;

	/* Stop the downlaod */
	if (WaitForSingleObject(m_hStopEvent, 0) == WAIT_OBJECT_0)
		return FALSE;

	if (m_hWnd)
	{
		/* Update progress */
		DWORD dwDegree = dwReadSize * 100 / dwTotalSize;
		::PostMessage(m_hWnd, WM_UPDATE_PROGRESS, 0, dwDegree);
	}
	return TRUE;
}

void CManualUpdater::Attach(HWND hWnd)
{
	m_hWnd = hWnd; 
}


void CManualUpdater::Start(UINT option)
{
	if (m_hWnd == NULL || m_hStopEvent == NULL)
		return;

	::ResetEvent(m_hStopEvent);

	if (m_hThread)
		::CloseHandle(m_hThread);

	m_uOption = option;
	m_hThread = (HANDLE)_beginthreadex(NULL, 0, WorkThread, this, 0, NULL);
}

void CManualUpdater::Stop()
{
	if (m_hStopEvent)
		::SetEvent(m_hStopEvent);

	if (m_hThread)
	{
		if (::WaitForSingleObject(m_hThread, 1000) != WAIT_OBJECT_0)
			::TerminateThread(m_hThread, 0);

		::CloseHandle(m_hThread);
		m_hThread = NULL;
	}
}

unsigned CManualUpdater::WorkThread(void* param)
{
	CManualUpdater* pSelf = (CManualUpdater*)param;
	return pSelf->DoWork();
}

unsigned CManualUpdater::DoWork()
{
	if (m_uOption == CHECKVER)
	{
		BOOL fFoundNew = FALSE;
		HRESULT hr = CheckNewVersion(&fFoundNew);
		/* Don't send the message if interrupt by user */
		if (hr != E_BKUPCTL_INTERRUPT)
		{
			m_BKSafeLog.WriteLog(L"CheckNewVersion: Error with 0x%x, bFount = %d", hr, fFoundNew);
			::SendMessage(m_hWnd, WM_UPDATE_CHECK_VERSION, hr, fFoundNew);
		}
	}
	else if (m_uOption == DOWNLAOD)
	{
		HRESULT hr = DownloadInstallPack();

		/* Don't send the message if interrupt by user */
		if (hr != E_BKUPCTL_INTERRUPT) 
			::SendMessage(m_hWnd, WM_UPDATE_DOWNLOAD_FINISH, hr, 0);
	}
	return 0;
}


HRESULT CManualUpdater::ParseCfg(LPCTSTR lpCfgName)
{
	BKIni::Document ini;

	HRESULT hr = ini.LoadFile(lpCfgName);

	if (FAILED(hr))
		return hr;

	if (!ini.HasSection(L"lastversion1"))
		return E_FAIL;

	BKIni::Section section = ini[L"lastversion1"];
	LPCTSTR lpCurChannel = L"";//hub 2011.0308

	if (section.HasKey(CT2W(lpCurChannel)))
		m_strNewVersion = section[CT2W(lpCurChannel)].GetValueString();
	else if (section.HasKey(L"h_home"))
		m_strNewVersion = section[L"h_home"].GetValueString();
	else
		return E_FAIL;

	if (!ini.HasSection(m_strNewVersion))
		return E_FAIL;

	section = ini[m_strNewVersion];
	
	if (!section.HasKey(L"url")      ||
		!section.HasKey(L"filename") ||
		!section.HasKey(L"crc")      ||
		!section.HasKey(L"filesize"))
		return E_FAIL;

	m_details.clear();

	m_strUrl     = section[L"url"].GetValueString();
	m_strFileName = section[L"filename"].GetValueString();
	m_strExecName = section[L"execname"].GetValueString();
	m_strCrc     = section[L"crc"].GetValueString();
	m_dwFileSize = section[L"filesize"].GetValueDWORD();

	for (int i=1; i<100; ++i)
	{
		wchar_t szItem[10] = {0};
		wsprintf(szItem, L"detail%d", i);

		if (!section.HasKey(szItem))
			break;

		m_details.push_back(section[szItem].GetValueString());
	}

	if (section.HasKey(L"compress"))
		m_fCompress = (section[L"compress"].GetValueDWORD() != 0);

	if (section.HasKey(L"paramete"))
		m_strParamete = section[L"paramete"].GetValueString();

	return S_OK;
}


/** 
* @brief Check new version
* @param pfFoundNew - TRUE if found new version
*                     FALSE if don't need update it
*/		
HRESULT CManualUpdater::CheckNewVersion(BOOL *pfFoundNew)
{
	CBkUpdDownloader downloader(TRUE);

	// 支持本地http代理 [1/12/2011 zhangbaoliang]
	downloader.UseBKSafeProxyInfo(PROXY_TYPE::PROXY_TYPE_HTTP);

	CBkUpdWebFile file;

	if (pfFoundNew == NULL)
		return E_INVALIDARG;

	*pfFoundNew = FALSE;
	
	/* Get path of download */

	CString strUpdatePath;
	HRESULT hr;
	//HRESULT hr = CAppPath::Instance().GetLeidianAppPath(strUpdatePath);

	//strUpdatePath=CW::CUtility::GetModulePath((HMODULE)&__ImageBase);

	hr = CAppPath::Instance().GetPath(strUpdatePath);

	if (FAILED(hr))
	{
		m_BKSafeLog.WriteLog(L"CheckNewVersion: 获取日志文件路径失败");
		return hr;
	}

	strUpdatePath += _T("\\update");

	if (::GetFileAttributes(strUpdatePath) == INVALID_FILE_ATTRIBUTES)
	{
		TCHAR szTempPath[MAX_PATH + 1] = {0};
		if (::GetTempPath(MAX_PATH, szTempPath) == 0)
		{
			m_BKSafeLog.WriteLog(L"CheckNewVersion: GetTempPath Error");
			return HRESULT_FROM_WIN32(::GetLastError());
		}
		strUpdatePath = szTempPath;
	}

	if(m_strIniName.IsEmpty())
		strUpdatePath += _T("\\YsIMupdate.ini");
	else
		strUpdatePath.Append(m_strIniName);

	//strUpdatePath += _T("\\YsIMupdate.ini");
	file.SetFileName(strUpdatePath);

	/* Download the configure */
	if(m_strSvrAddr.IsEmpty())
		hr = downloader.Download(UPDATE_INI_URL, (IBkUpdWebFile*)&file, this);
	else
	{
		hr = downloader.Download(m_strSvrAddr, (IBkUpdWebFile*)&file, this);
	}
	m_BKSafeLog.WriteLog(L"CheckNewVersion: DownloadIni hr = 0x%x", hr);

	if (SUCCEEDED(hr))
	{
		/* parse the cfg */
		hr = ParseCfg(strUpdatePath);

		if (FAILED(hr))
		{
			m_BKSafeLog.WriteLog(L"CheckNewVersion: ParseCfg hr = 0x%x", hr);
			return hr;
		}

		/* Get current version */
		TCHAR tzCurVer[255]={0};
		GetProcVersion(tzCurVer);
		/* Compare version */
		if (VersionLess(tzCurVer, m_strNewVersion))
		{
			*pfFoundNew = TRUE;
			return hr;
		}
		m_strNewVersion = tzCurVer;
	}
	return hr;
}


DWORD CManualUpdater::CalcFileCRC(LPCTSTR lpFileName)
{
	HANDLE hFile = ::CreateFile(lpFileName, GENERIC_READ, 
		FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

    if (hFile == INVALID_HANDLE_VALUE)
		return 0;

	BYTE byBuffer[1024] = {0};
	DWORD dwCRC = 0;
	DWORD dwFileSize = ::GetFileSize(hFile, NULL);

	if (dwFileSize == m_dwFileSize) /* check size of file */
	{
		while (dwFileSize > 0)
		{
			DWORD dwRead = 0;

			if (!::ReadFile(hFile, byBuffer, sizeof(byBuffer), &dwRead, NULL))
				break;

			dwFileSize -= dwRead;
			dwCRC = CRC32(dwCRC, byBuffer, dwRead);
		}
	}

	::CloseHandle(hFile);
	return dwCRC;
}

BOOL CManualUpdater::VerifyFileSign(LPCTSTR lpFileName)
{
	CAtlArray<CString> singer;

	singer.Add(_T("Zhuhai  Kingsoft Software Co.,Ltd"));
	singer.Add(_T("Kingsoft Security Co.,Ltd"));

	WinMod::CWinTrustVerifier verifier;
	verifier.TryLoadDll();

	HRESULT hr;
	WinMod::CWinTrustSignerInfoChain SingerInfo;
	DWORD dwWinTrust = verifier.VerifyFile(CT2W(lpFileName), &SingerInfo);

	if (dwWinTrust <= 0x0000FFFF)
		hr = AtlHresultFromWin32(dwWinTrust);
	else
		hr = dwWinTrust;

	if (SUCCEEDED(hr))
	{
		if (!SingerInfo.m_chain.IsEmpty())
		{
			for (int i=0; i<singer.GetCount(); i++)
			{
				if (0 == SingerInfo.m_chain.GetHead().m_strIsserName.Compare(singer[i]))
					return TRUE;
			}
		}
	}
	return FALSE;
}

HRESULT CManualUpdater::DownloadInstallPack()
{
	/* get path of download */

	CString strUpdatePath;
	HRESULT hr = CAppPath::Instance().GetLeidianLogPath(strUpdatePath);
	 hr =CAppPath::Instance().GetPath(strUpdatePath);
	if (FAILED(hr))
		return hr;

	TCHAR szPackPath[MAX_PATH + 1] = {0};
	if (::GetTempFileName(strUpdatePath, _T("upd"), 0, szPackPath) == 0)
		return  HRESULT_FROM_WIN32(GetLastError());

	/* dowlaod a pack */

	CBkUpdDownloader downloader(TRUE);

	// 支持本地http代理 [1/12/2011 zhangbaoliang]
	downloader.UseBKSafeProxyInfo(PROXY_TYPE::PROXY_TYPE_HTTP);

	CBkUpdWebFile pack;

	hr = pack.SetFileName(szPackPath);
	hr = downloader.Download(m_strUrl, (IBkUpdWebFile*)&pack, this);

	if (SUCCEEDED(hr))
	{
		/* get file name of exectue */
		m_strCmd.Format(_T("%s"),m_strExecName);
		m_strFilePath.Format(_T("%s\\%s"), strUpdatePath, m_strFileName);

		if (m_fCompress)
		{
			/* uncompress */

			hr = CZlib::Instance().InitZlib();

			if (SUCCEEDED(hr))
			{
				hr = CZlib::Instance().DecodeFile(szPackPath, m_strFilePath, m_dwFileSize);
				CZlib::Instance().UninitZlib();
				::DeleteFile(szPackPath);
			}
		}
		else
		{		
			/* rename */
			if (!::MoveFileEx(szPackPath, m_strFilePath, MOVEFILE_REPLACE_EXISTING))
				hr = HRESULT_FROM_WIN32(GetLastError());
		}

		//不检查文件
		//if (SUCCEEDED(hr))
		//{
		//	/* verify this file's crc and sign */

		//	DWORD dwCRC = 0;
		//	_stscanf(m_strCrc, _T("%x"), &dwCRC);

		//	if (CalcFileCRC(m_strCmd) != dwCRC || 
		//		!VerifyFileSign(m_strCmd)) 
		//	{
		//		::DeleteFile(m_strCmd);
		//		hr = E_FAIL;
		//	}
		//}
	}

	if (FAILED(hr))
	{
		/* delete this pack if failed */
		::DeleteFile(szPackPath);
	}
	return hr;	
}

void CManualUpdater::Execute()
{
	if (m_strCmd.IsEmpty())
		return;

	if (!m_strParamete.IsEmpty())
		m_strCmd.Format(_T("%s %s"), m_strExecName, m_strParamete);

	Execute(m_strCmd.GetBuffer());

	/*STARTUPINFO si;
    PROCESS_INFORMATION pi;

    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    ZeroMemory(&pi, sizeof(pi));

	if (::CreateProcess(NULL, m_strCmd.GetBuffer(), NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi))
	{
		CloseHandle(pi.hProcess);
		CloseHandle(pi.hThread);
	}*/
}

void CManualUpdater::Execute( LPTSTR lpstr )
{

	STARTUPINFO si;
	PROCESS_INFORMATION pi;

	ZeroMemory(&si, sizeof(si));
	si.cb = sizeof(si);
	ZeroMemory(&pi, sizeof(pi));

	CString strTmp=lpstr;
	if (::CreateProcess(NULL, strTmp.GetBuffer(), NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi))
	{
		CloseHandle(pi.hProcess);
		CloseHandle(pi.hThread);
	}
}

void CManualUpdater::ExecuteUpdate()
{
	Execute(_T("Update.dat"));
}

int CManualUpdater::GetNumberOfDetail()
{
	return (int)m_details.size();
}

const CString& CManualUpdater::GetDetailItem(int index)
{
	if (index < m_details.size())
		return m_details[index];

	return _T("");
}

const CString& CManualUpdater::GetVersion()
{
	return m_strNewVersion;
}

void CManualUpdater::GetProcVersion(LPTSTR lpCurVer)
{
	DWORD dwVerInfoSize = 0;
	DWORD dwVerHnd;
	VS_FIXEDFILEINFO * pFileInfo;
	TCHAR szFullPath[MAX_PATH];

	GetModuleFileName(NULL, szFullPath, sizeof(szFullPath));
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

		CString strVersion ;
		strVersion.Format(_T("%d.%d.%d.%d"),m_nProdVersion[0],
			m_nProdVersion[1],m_nProdVersion[2],m_nProdVersion[3]);

		GlobalUnlock(hMem);
		GlobalFree(hMem);
		_tcscpy_s(lpCurVer,255,strVersion.GetBuffer());
	}
}
