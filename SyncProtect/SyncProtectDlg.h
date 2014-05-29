
// SyncProtectDlg.h : 头文件
//

#pragma once
#include "afxwin.h"

#define  WM_UPDATE_HINT WM_USER+1

// CSyncProtectDlg 对话框
class CSyncProtectDlg : public CDialogEx
{
// 构造
public:
	CSyncProtectDlg(CWnd* pParent = NULL);	// 标准构造函数	

// 对话框数据
	enum { IDD = IDD_SYNCPROTECT_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持

private:
	CString strExitPassword;
	

public:
	//设置共享数据
	void SetShareData1();
	void CloseDlg();
	//取共享数据 
	void GetShareData1();

// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	virtual void OnOK();
	virtual void OnCancel();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnBnClickedBtnAutoUpdate();
	afx_msg void OnBnClickedBtnStartSvc();
protected:
	afx_msg LRESULT OnUpdateHint(WPARAM wParam, LPARAM lParam);
public:
	afx_msg void OnBnClickedBtnStopSvc();
	CEdit m_edtHint;
	afx_msg void OnNcPaint();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnGetMinMaxInfo(MINMAXINFO* lpMMI);
};

void CALLBACK WriteLog(LPTSTR lpMsg);


//线程 
UINT DoAutoUpdate(LPVOID lp1);
UINT DoParseInstruction(LPVOID lp);

