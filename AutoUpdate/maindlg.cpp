#include "stdafx.h"
#include "maindlg.h"
#include "bkupctl\bkupddownloader.h"
#include "bkupctl\bkupdwebfile.h"
#include "miniutil/bkini.h"                /* for BKIni::Document */
#include "winhttp.h"
#include "kispublic/kopksafereg.h"

#include "AU.h"


#define UPDATE_HIT_CHECK    _T("正在检测升级助手是否为最新版本, 请等待...")
#define UPDATE_HIT_FOUND    _T("检测到有更新版本, 请点击“立即升级”升级到最新版本。")
#define UPDATE_HIT_FINISH   _T("您的升级助手已经是最新版本。")
#define UPDATE_HIT_BUY      _T("网络正忙，请稍后再试。")
#define UPDATE_HIT_FAILED1  _T("升级失败，请稍后再试。")
#define UPDATE_HIT_FAILED2  _T("版本检测失败，请稍后再试。")  
#define UPDATE_HIT_DOWN     _T("正在下载升级包，请等待...")
#define UPDATE_EDIT_STYLE  (WS_CHILD | WS_VSCROLL | ES_LEFT | ES_AUTOHSCROLL | ES_MULTILINE | ES_AUTOVSCROLL | ES_READONLY)


CMainDlg::CMainDlg():CBkDialogImpl<CMainDlg>(IDR_MAIN_DIALOG) // 在这里加载界面框架
{
	BkString::Load(IDR_STRING_DEF); // 加载字符串
	BkSkin::LoadSkins(IDR_SKIN_DEF); // 加载皮肤
	BkStyle::LoadStyles(IDR_STYLE_DEF); // 加载风格
}

CMainDlg::~CMainDlg()
{

}

void CMainDlg::OnSysCommand(UINT nID, CPoint point)
{
	SetMsgHandled(FALSE);

	switch (nID & 0xFFF0)
	{
	case SC_CLOSE:
		SetMsgHandled(TRUE);
		OnClose();
		break;
	case SC_RESTORE:
		{
			DWORD dwStyle = GetStyle();
			if (WS_MINIMIZE == (dwStyle & WS_MINIMIZE))
				break;

			if (WS_MAXIMIZE == (dwStyle & WS_MAXIMIZE))
			{
				SetItemAttribute(IDC_BTN_MAX, "skin", "dlg_btn_max");
				break;
			}
		}
	case SC_MAXIMIZE:
		SetItemAttribute(IDC_BTN_MAX, "skin", "dlg_btn_normal");
		break;
	default:
		break;
	}
}

BOOL CMainDlg::OnInitDialog( CWindow wndFocus, LPARAM lInitParam )
{
	HRESULT hr;
	SetIcon(::LoadIcon((HMODULE)&__ImageBase, MAKEINTRESOURCE(IDI_MAIN)));

	//把路径写入注册表
	WCHAR wzFilePath[MAX_PATH]={0};
	int nRet=GetModuleFileName(NULL,wzFilePath,MAX_PATH);
	if(nRet!=0)
	{
		PathRemoveFileSpec(wzFilePath);
		KOpKsafeReg::CreateKsafeReg(wzFilePath);
	}

	SetItemVisible(IDC_DLG_UPDATE_DETAIL,  TRUE);
	SetItemVisible(IDC_BTN_UPDATE_START,   FALSE);
	SetItemText(IDC_TXT_UPDATE_HIT, UPDATE_HIT_CHECK); 
	SetItemVisible(IDC_TXT_UPDATE_HIT,     TRUE);
	SetItemVisible(IDC_BTN_UPDATE_CANCEL,  TRUE);
	SetItemVisible(IDC_PROGRESS_UPDATE,    FALSE);
	
	HWND h=this->m_hWnd;
	m_edtDetail.Create(GetViewHWND(), NULL, NULL, 
		UPDATE_EDIT_STYLE, 0, IDC_EDIT_UPDATE_DETAIL);	
	DWORD d=GetLastError();

	//m_edtDetail.m_hWnd=this->GetViewHWND();

	PARAFORMAT2 pf2;
	pf2.cbSize = sizeof(PARAFORMAT2);
	pf2.dwMask = PFM_LINESPACING|PFM_BORDER;
	pf2.dyLineSpacing     = 330;
	pf2.bLineSpacingRule  = 4;

	m_edtDetail.SetParaFormat(pf2);
	m_edtDetail.SetFont(BkFontPool::GetFont(FALSE, FALSE, FALSE, 2));
	m_edtDetail.SetBackgroundColor(RGB(240, 248, 255));

	m_updater.Attach(m_hWnd);
	m_updater.Start(CManualUpdater::CHECKVER);
	return TRUE;

}



LRESULT CMainDlg::OnProgress( UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	SetItemIntAttribute(IDC_PROGRESS_UPDATE, "value", (int)lParam);
	return TRUE;
}

LRESULT CMainDlg::OnCheckVersion( UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	if (lParam) /* found new version */
	{
		SetItemText(IDC_TXT_UPDATE_HIT, UPDATE_HIT_FOUND);
		SetItemVisible(IDC_BTN_UPDATE_CANCEL, TRUE);
		SetItemVisible(IDC_BTN_UPDATE_START,  TRUE);
		SetItemVisible(IDC_PROGRESS_UPDATE,   FALSE);

		ShowUpdateDetail(); /* show details */
	}
	else 
	{
		HRESULT hr = (HRESULT)wParam;

		if (SUCCEEDED(hr)) /* have been new version */
		{
			SetItemText(IDC_TXT_UPDATE_HIT, UPDATE_HIT_FINISH);
			SetItemVisible(IDC_PROGRESS_UPDATE,   FALSE);
			SetItemVisible(IDC_BTN_UPDATE_CANCEL, FALSE);

			CString detail = _T("当前版本号:");
			detail += m_updater.GetVersion();

			m_edtDetail.SetWindowText(detail);
			SetItemVisible(IDC_DLG_UPDATE_DETAIL, TRUE);
		}
		else  /* maybe timeout */
		{
			if (hr == ERROR_WINHTTP_TIMEOUT)
				ShowWarnning(UPDATE_HIT_BUY);
			else
				ShowWarnning(UPDATE_HIT_FAILED2);
		}
	}
	return TRUE;
}

LRESULT CMainDlg::OnDownlaodFinish( UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	HRESULT hr = (HRESULT)wParam;

	if (SUCCEEDED(hr))
	{
		m_updater.Execute(); /* do install */
		EndDialog(0);
	}
	else 
	{
		if (hr == ERROR_WINHTTP_TIMEOUT)
			ShowWarnning(UPDATE_HIT_BUY);
		else
			ShowWarnning(UPDATE_HIT_FAILED1);

		
	}
	return TRUE;
}

void CMainDlg::ShowUpdateDetail()
{
	int count =  m_updater.GetNumberOfDetail();
	CString detail;
	CString version;
	const int rowwide = 30;

	version.Format(_T("新版本号:\n  %s\n"), m_updater.GetVersion());

	for (int i=0; i<count; ++i)
	{
		CString item = m_updater.GetDetailItem(i);
		item.Format(_T("  %d.%s\n"), i+1, item);
		int line =  item.GetLength() / rowwide;

		for (int j=0; j<line; ++j)
		{
			if (item[(j+1)*(rowwide-1)] != _T('\n'))
				item.Insert((j+1)*(rowwide-1), _T("\n"));
		}

		detail += item;
	}

	detail.Insert(0, _T("更新内容:\n"));
	m_edtDetail.SetWindowText((version + detail));
	SetItemVisible(IDC_DLG_UPDATE_DETAIL, TRUE);
}

void CMainDlg::ShowWarnning( LPCTSTR lpMsg )
{
	SetItemVisible(IDC_TXT_UPDATE_HIT,     FALSE);
	SetItemVisible(IDC_PROGRESS_UPDATE,    FALSE);
	SetItemVisible(IDC_BTN_UPDATE_CANCEL,  TRUE);
	SetItemText(IDC_TXT_UPDATE_WARNING, lpMsg);
	SetItemVisible(IDC_DLG_UPDATE_WARNING, TRUE);

	m_edtDetail.SetWindowText(_T(""));
	m_edtDetail.ShowWindow(SW_HIDE);
}

LRESULT CMainDlg::OnCreate( LPCREATESTRUCT lParam )
{
	return TRUE;
}

void CMainDlg::OnBtnStart()
{
	SetItemVisible(IDC_BTN_UPDATE_START,  FALSE);
	SetItemVisible(IDC_DLG_UPDATE_DETAIL, FALSE);
	SetItemText(IDC_TXT_UPDATE_HIT, UPDATE_HIT_DOWN);
	SetItemVisible(IDC_PROGRESS_UPDATE, TRUE);
	SetItemIntAttribute(IDC_PROGRESS_UPDATE, "value", (int)0);
	m_updater.Start(CManualUpdater::DOWNLAOD);
}

//HRESULT CMainDlg::ParseCfg( LPCTSTR lpCfgName )
//{
//	BKIni::Document ini;
//
//	HRESULT hr = ini.LoadFile(lpCfgName);
//
//	if (FAILED(hr))
//		return hr;
//
//	if (!ini.HasSection(L"lastversion1"))
//		return E_FAIL;
//
//	BKIni::Section section = ini[L"lastversion1"];
//	LPCTSTR lpCurChannel = L"";//hub 2011.0308
//
//	if (section.HasKey(CT2W(lpCurChannel)))
//		m_strNewVersion = section[CT2W(lpCurChannel)].GetValueString();
//	else if (section.HasKey(L"h_home"))
//		m_strNewVersion = section[L"h_home"].GetValueString();
//	else
//		return E_FAIL;
//
//	if (!ini.HasSection(m_strNewVersion))
//		return E_FAIL;
//
//	section = ini[m_strNewVersion];
//
//	if (!section.HasKey(L"url")      ||
//		!section.HasKey(L"filename") ||
//		!section.HasKey(L"crc")      ||
//		!section.HasKey(L"filesize"))
//		return E_FAIL;
//
//	m_details.clear();
//
//	m_strUrl     = section[L"url"].GetValueString();
//	m_strExeName = section[L"filename"].GetValueString();
//	m_strCrc     = section[L"crc"].GetValueString();
//	m_dwFileSize = section[L"filesize"].GetValueDWORD();
//
//	for (int i=1; i<100; ++i)
//	{
//		wchar_t szItem[10] = {0};
//		wsprintf(szItem, L"detail%d", i);
//
//		if (!section.HasKey(szItem))
//			break;
//
//		m_details.push_back(section[szItem].GetValueString());
//	}
//
//	if (section.HasKey(L"compress"))
//		m_fCompress = (section[L"compress"].GetValueDWORD() != 0);
//
//	if (section.HasKey(L"paramete"))
//		m_strParamete = section[L"paramete"].GetValueString();
//
//	return S_OK;
//}

void CMainDlg::OnClose()
{
	m_updater.Stop();
	EndDialog(0);
}


void CMainDlg::OnBtnCancel()
{
	m_updater.Stop();
	EndDialog(0);
}

