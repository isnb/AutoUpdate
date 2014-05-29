
// SyncProtectDlg.h : ͷ�ļ�
//

#pragma once
#include "afxwin.h"

#define  WM_UPDATE_HINT WM_USER+1

// CSyncProtectDlg �Ի���
class CSyncProtectDlg : public CDialogEx
{
// ����
public:
	CSyncProtectDlg(CWnd* pParent = NULL);	// ��׼���캯��	

// �Ի�������
	enum { IDD = IDD_SYNCPROTECT_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��

private:
	CString strExitPassword;
	

public:
	//���ù�������
	void SetShareData1();
	void CloseDlg();
	//ȡ�������� 
	void GetShareData1();

// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
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


//�߳� 
UINT DoAutoUpdate(LPVOID lp1);
UINT DoParseInstruction(LPVOID lp);

