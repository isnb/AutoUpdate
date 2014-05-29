#pragma once

#include "bkres/bkres.h"
#include "atlapp.h"
#include <vector>
#include "bkupctl/bkupddownloader.h"  /* For IBkDownloadCallback interface */
#include "define.h"
#include "ManualUpdater.h"



class CMainDlg 
	: public CBkDialogImpl<CMainDlg>,
	   public CWHRoundRectFrameHelper<CMainDlg>
{

	
protected:
	void ShowUpdateDetail();
	void ShowWarnning(LPCTSTR lpMsg);

public:

	CMainDlg();
	~CMainDlg();

	void OnClose();
	void OnBtnCancel();
	void OnBtnStart();

	void	OnSysCommand(UINT nID, CPoint point);
	BOOL OnInitDialog(CWindow wndFocus, LPARAM lInitParam);
	LRESULT OnCreate(LPCREATESTRUCT lParam);


	LRESULT OnProgress(UINT uMsg, WPARAM wParam, LPARAM lParam);
	LRESULT OnCheckVersion(UINT uMsg, WPARAM wParam, LPARAM lParam);
	LRESULT OnDownlaodFinish(UINT uMsg, WPARAM wParam, LPARAM lParam);

private:
//	
	CRichEditCtrl   m_edtDetail;

public:

	BEGIN_MSG_MAP_EX(CMainDlg)
	MSG_WM_INITDIALOG(OnInitDialog)
	MSG_WM_SYSCOMMAND(OnSysCommand)
	MSG_WM_CREATE(OnCreate);

	CHAIN_MSG_MAP(CBkDialogImpl<CMainDlg>)
	CHAIN_MSG_MAP(CWHRoundRectFrameHelper<CMainDlg>)
	MSG_BK_NOTIFY(IDC_RICHVIEW_WIN)

	
	REFLECT_NOTIFICATIONS_EX()

	MESSAGE_HANDLER_EX(WM_UPDATE_PROGRESS,     OnProgress)
	MESSAGE_HANDLER_EX(WM_UPDATE_CHECK_VERSION,  OnCheckVersion)
	MESSAGE_HANDLER_EX(WM_UPDATE_DOWNLOAD_FINISH,  OnDownlaodFinish)

	END_MSG_MAP()

	BK_NOTIFY_MAP(IDC_RICHVIEW_WIN)
	BK_NOTIFY_ID_COMMAND(IDC_BTN_CLOSE, OnClose)
	BK_NOTIFY_ID_COMMAND(IDC_BTN_UPDATE_START,OnBtnStart)
	BK_NOTIFY_ID_COMMAND(IDC_BTN_UPDATE_CANCEL,OnBtnCancel)
	BK_NOTIFY_MAP_END()

};

extern CAppModule _Module;
