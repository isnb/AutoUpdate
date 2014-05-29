#include "stdafx.h"
#include "AU.h"
#include "maindlg.h"
//#include "afxwin.h"


CManualUpdater  m_updater;
HMODULE	m_hModRichEdit2;

BOOL  ExtractSubString(CString& rString, LPCTSTR lpszFullString,
								int iSubString, TCHAR chSep)
{
	if (lpszFullString == NULL)
		return FALSE;

	while (iSubString--)
	{
		lpszFullString = _tcschr(lpszFullString, chSep);
		if (lpszFullString == NULL)
		{
			rString.Empty();        // return empty string as well
			return FALSE;
		}
		lpszFullString++;       // point past the separator
	}
	LPCTSTR lpchEnd = _tcschr(lpszFullString, chSep);
	int nLen = (lpchEnd == NULL) ?
		lstrlen(lpszFullString) : (int)(lpchEnd - lpszFullString);
	
	Checked::memcpy_s(rString.GetBufferSetLength(nLen), nLen*sizeof(TCHAR),
		lpszFullString, nLen*sizeof(TCHAR));
	rString.ReleaseBuffer();	// Need to call ReleaseBuffer 
	// after calling GetBufferSetLength
	return TRUE;
}


int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
	CMainDlg dlg;
	m_hModRichEdit2 = ::LoadLibrary(_T("RICHED20.DLL"));

	CString strComm="";

	ExtractSubString(strComm,lpCmdLine,0,' ');
	m_updater.m_strSvrAddr=strComm;

	ExtractSubString(strComm,lpCmdLine,1,' ');
	m_updater.m_strIniName=strComm;

	dlg.DoModal();
	return 0;
}
