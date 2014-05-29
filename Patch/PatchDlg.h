
// PatchDlg.h : 头文件
//

#pragma once
#include "afxwin.h"
#include "bkupctl/bkupddownloader.h"  /* For IBkDownloadCallback interface */
#include "afxcmn.h"


typedef struct tagFILEUPDATECLIDATA
{
	CString sFileName; //比较的Key
	CString sVersion;
	DWORD nCrc32;
} FILEUPDATECLIDATA,*PFILEUPDATECLIDATA;


typedef struct tagFILEUPDATESVRDATA
{
	CString sFileName; //比较的Key
	CString sFilePath;
	CString sVersion;
	DWORD nCrc32;
	CString sClientPath; //客户端路径
	DWORD nCompress; //0不压缩,1 ZIP,2 RAR,3 7Z
} FILEUPDATESVRDATA,*PFILEUPDATESVRDATA;

// CPatchDlg 对话框
class CPatchDlg : public CDialog,IBkDownloadCallback
{
// 构造
public:
	CPatchDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_PATCH_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


	/* For IBkDownloadCallback interface method */
	virtual BOOL DownloadProcess(DWORD dwTotalSize, DWORD dwReadSize);

public:
	HRESULT ParseServerCfg(CString cfgPath); 
	//获得运行中的模块列表
	virtual HRESULT GetMoudleList();
	HRESULT DownloadFile(CString srvPath,CString savePath,BOOL chekcCrc,DWORD crc);
	//处理压缩文件.
	HRESULT DealCompress();
	//递归指定目录文件,与服务端文件比较,CRC不一致删除
	HRESULT CompareFile(CString path);
	//服务端版本与客户端版本比较,并下载
	HRESULT DownloadDiffFile();
	//只比较版本，不下载
	HRESULT CompareVersion();

public:
	//CRC32检验有问题，重试次数
	int nCrc32Retry;
	//CRC 验证不通过提示
	CString sCrcHint;
	//Crc通过标志
	BOOL bCrcChecekUp;
	//标志可以关闭窗口
	bool isCanClose;
	CMapStringToPtr m_mapSvrFileVersion;
	CMapStringToPtr m_mapCliFileVersion;

	//检查新版本
	void CheckVersion(BOOL *pbFind);
	void SetUpdateAddress(LPTSTR lpAppPath);
	//计算CRC
	int CalcCrc(CString fileName);
	//取CRC信息
	void GetCrcInfo(BOOL *checkup,LPTSTR lptzHint,int size);
	


private:
	
// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
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



//更新功能函数
UINT DoPatch(LPVOID lp1);

void CALLBACK WriteLog(LPCTSTR lpMsg);

VOID KillProcess(DWORD dwPid);
//根据名称取进程 取一个
DWORD GetProcessIdByName(LPCTSTR lpFilename,LPTSTR lpFilePath);

//重启服务
UINT StopService_dan(LPVOID lp1);
UINT StartService_dan(LPVOID lp1);
//查询服务状态
BOOL QueryServiceStatus_dan(LPCTSTR lpSvcName,UINT status);

// 将pc中的文件夹从一个目录拷贝到另外的一个目录
BOOL MoveDirectory(CString strSrcPath, CString strDesPath);

//当前目录所有文件和目录与服务配置文件比较,服务不存在就执行删除
void DelDiffFile();
//跳过不删除文件
bool IsSkipFile(CString fn);
//跳过不删除目录
bool IsSkipDirectory(CString fn);
//是否存在服务配置文件中
bool IsExistServer(CString fn);
bool DeleteDirectory(TCHAR* DirName); 



//typedef void (CALLBACK *LogFn)(LPCTSTR msg);
//_declspec(selectany) LogFn Log=NULL;
void GetAppPath(LPTSTR lpAppPath);

//是否是特殊文件
bool IsSpecialFile(CString fileName);

//Extern
BOOL Execute(LPCTSTR lpszProcessName,LPCTSTR strCmdLine, BOOL bForceAdmin, BOOL bWaitProcess);
void Log(LPCTSTR lpszFormat,... );
typedef void (CALLBACK *LogFn)(LPCTSTR lpszMsg); 
_declspec(selectany) LogFn Log1=NULL;
DWORD CalcFileCrc(FILE* lpFile);
bool   WINAPI Adjust(TCHAR * PrivilegeName,bool lEnabled=true);

//检查服务，停止重启
bool CheckSyncSvc();

//读取ServiceSetting.xml
void ReadServiceSetting();

//创建快捷方式
void CreateDesktopLink(CString sName,CString sPath);


