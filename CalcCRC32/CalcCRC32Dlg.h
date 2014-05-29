
// CalcCRC32Dlg.h : ͷ�ļ�
//

#pragma once


// CCalcCRC32Dlg �Ի���
class CCalcCRC32Dlg : public CDialogEx
{
// ����
public:
	CCalcCRC32Dlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_CALCCRC32_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��

private:
	CList<LPTSTR,LPTSTR> m_liFileList;

private:
	int CalcCrc(CString fileName);

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
	afx_msg void OnBnClickedBtnBrowse();
	CString m_strHint;
};
