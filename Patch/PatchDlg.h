
// PatchDlg.h : ͷ�ļ�
//

#pragma once
#include "afxwin.h"
#include "bkupctl/bkupddownloader.h"  /* For IBkDownloadCallback interface */
#include "afxcmn.h"


typedef struct tagFILEUPDATECLIDATA
{
	CString sFileName; //�Ƚϵ�Key
	CString sVersion;
	DWORD nCrc32;
} FILEUPDATECLIDATA,*PFILEUPDATECLIDATA;


typedef struct tagFILEUPDATESVRDATA
{
	CString sFileName; //�Ƚϵ�Key
	CString sFilePath;
	CString sVersion;
	DWORD nCrc32;
	CString sClientPath; //�ͻ���·��
	DWORD nCompress; //0��ѹ��,1 ZIP,2 RAR,3 7Z
} FILEUPDATESVRDATA,*PFILEUPDATESVRDATA;

// CPatchDlg �Ի���
class CPatchDlg : public CDialog,IBkDownloadCallback
{
// ����
public:
	CPatchDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_PATCH_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��


	/* For IBkDownloadCallback interface method */
	virtual BOOL DownloadProcess(DWORD dwTotalSize, DWORD dwReadSize);

public:
	HRESULT ParseServerCfg(CString cfgPath); 
	//��������е�ģ���б�
	virtual HRESULT GetMoudleList();
	HRESULT DownloadFile(CString srvPath,CString savePath,BOOL chekcCrc,DWORD crc);
	//����ѹ���ļ�.
	HRESULT DealCompress();
	//�ݹ�ָ��Ŀ¼�ļ�,�������ļ��Ƚ�,CRC��һ��ɾ��
	HRESULT CompareFile(CString path);
	//����˰汾��ͻ��˰汾�Ƚ�,������
	HRESULT DownloadDiffFile();
	//ֻ�Ƚϰ汾��������
	HRESULT CompareVersion();

public:
	//CRC32���������⣬���Դ���
	int nCrc32Retry;
	//CRC ��֤��ͨ����ʾ
	CString sCrcHint;
	//Crcͨ����־
	BOOL bCrcChecekUp;
	//��־���Թرմ���
	bool isCanClose;
	CMapStringToPtr m_mapSvrFileVersion;
	CMapStringToPtr m_mapCliFileVersion;

	//����°汾
	void CheckVersion(BOOL *pbFind);
	void SetUpdateAddress(LPTSTR lpAppPath);
	//����CRC
	int CalcCrc(CString fileName);
	//ȡCRC��Ϣ
	void GetCrcInfo(BOOL *checkup,LPTSTR lptzHint,int size);
	


private:
	
// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

protected:
	afx_msg LRESULT OnUpdateHint(WPARAM wParam, LPARAM lParam);
public:
	CEdit m_edtHint;
	CProgressCtrl m_pgsDownloadFile;
	afx_msg void OnGetMinMaxInfo(MINMAXINFO* lpMMI);
protected:
	virtual void OnOK();
	virtual void OnCancel();
};



//���¹��ܺ���
UINT DoPatch(LPVOID lp1);

void CALLBACK WriteLog(LPCTSTR lpMsg);

VOID KillProcess(DWORD dwPid);
//��������ȡ���� ȡһ��
DWORD GetProcessIdByName(LPCTSTR lpFilename,LPTSTR lpFilePath);

//��������
UINT StopService_dan(LPVOID lp1);
UINT StartService_dan(LPVOID lp1);
//��ѯ����״̬
BOOL QueryServiceStatus_dan(LPCTSTR lpSvcName,UINT status);

// ��pc�е��ļ��д�һ��Ŀ¼�����������һ��Ŀ¼
BOOL MoveDirectory(CString strSrcPath, CString strDesPath);

//��ǰĿ¼�����ļ���Ŀ¼����������ļ��Ƚ�,���񲻴��ھ�ִ��ɾ��
void DelDiffFile();
//������ɾ���ļ�
bool IsSkipFile(CString fn);
//������ɾ��Ŀ¼
bool IsSkipDirectory(CString fn);
//�Ƿ���ڷ��������ļ���
bool IsExistServer(CString fn);
bool DeleteDirectory(TCHAR* DirName); 



//typedef void (CALLBACK *LogFn)(LPCTSTR msg);
//_declspec(selectany) LogFn Log=NULL;
void GetAppPath(LPTSTR lpAppPath);

//�Ƿ��������ļ�
bool IsSpecialFile(CString fileName);

//Extern
BOOL Execute(LPCTSTR lpszProcessName,LPCTSTR strCmdLine, BOOL bForceAdmin, BOOL bWaitProcess);
void Log(LPCTSTR lpszFormat,... );
typedef void (CALLBACK *LogFn)(LPCTSTR lpszMsg); 
_declspec(selectany) LogFn Log1=NULL;
DWORD CalcFileCrc(FILE* lpFile);
bool   WINAPI Adjust(TCHAR * PrivilegeName,bool lEnabled=true);

//������ֹͣ����
bool CheckSyncSvc();

//��ȡServiceSetting.xml
void ReadServiceSetting();

//������ݷ�ʽ
void CreateDesktopLink(CString sName,CString sPath);


