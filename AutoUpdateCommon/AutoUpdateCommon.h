// AutoUpdateCommon.h : AutoUpdateCommon DLL 的主头文件
//

#pragma once

#ifndef __AFXWIN_H__
	#error "在包含此文件之前包含“stdafx.h”以生成 PCH 文件"
#endif

#include "resource.h"		// 主符号
#include "bkupctl/bkupddownloader.h"  /* For IBkDownloadCallback interface */

// CAutoUpdateCommonApp
// 有关此类实现的信息，请参阅 AutoUpdateCommon.cpp
//



class CAutoUpdateCommonApp : public CWinApp,IBkDownloadCallback
{

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
} FILEUPDATESVRDATA,*PFILEUPDATESVRDATA;

public:
	CAutoUpdateCommonApp();

// 重写
public:
	virtual BOOL InitInstance();

	/* For IBkDownloadCallback interface method */
	virtual BOOL DownloadProcess(DWORD dwTotalSize, DWORD dwReadSize);

public:
	HRESULT ParseServerCfg(CString cfgPath); 
	//获得运行中的模块列表
	HRESULT GetMoudleList();
	HRESULT DownloadFile(CString srvPath,CString savePath,BOOL chekcCrc,DWORD crc);
	
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

	//检查新版本
	//void CheckVersion(int id,BOOL *pbFind);
	void CheckVersion(BOOL *pbFind);
	//检查服务端是否允许升级
	void CheckAllowUpdate(int id,BOOL *allow);
	//更新
	void SaveAllowUpdate(int id);
	void SetUpdateAddress(LPTSTR lpAppPath);
	//计算CRC
	int CalcCrc(CString fileName);
	//取CRC信息
	void GetCrcInfo(BOOL *checkup,LPTSTR lptzHint,int size);


private:
	CMapStringToPtr m_mapSvrFileVersion;
	CMapStringToPtr m_mapCliFileVersion;
	DECLARE_MESSAGE_MAP()
};

extern CAutoUpdateCommonApp theApp;

//ULARGE_INTEGER GetDigitalVersionByString(LPCTSTR lpszVersion);
////比较版本
//BOOL VersionLess(LPCTSTR lpszVersionLeft, LPCTSTR lpszVersionRight);

typedef void (CALLBACK *LogFn)(LPCTSTR lpszMsg); 
typedef void (CALLBACK *GetModuleFn)(int nAddr);
_declspec(selectany) LogFn Log1=NULL;
_declspec(selectany) GetModuleFn GetModule1=NULL;
void Log(LPTSTR lpszFormat,... );
DWORD CalcFileCrc(FILE* lpFile);


