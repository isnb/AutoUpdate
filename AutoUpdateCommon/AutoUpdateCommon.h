// AutoUpdateCommon.h : AutoUpdateCommon DLL ����ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������
#include "bkupctl/bkupddownloader.h"  /* For IBkDownloadCallback interface */

// CAutoUpdateCommonApp
// �йش���ʵ�ֵ���Ϣ������� AutoUpdateCommon.cpp
//



class CAutoUpdateCommonApp : public CWinApp,IBkDownloadCallback
{

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
} FILEUPDATESVRDATA,*PFILEUPDATESVRDATA;

public:
	CAutoUpdateCommonApp();

// ��д
public:
	virtual BOOL InitInstance();

	/* For IBkDownloadCallback interface method */
	virtual BOOL DownloadProcess(DWORD dwTotalSize, DWORD dwReadSize);

public:
	HRESULT ParseServerCfg(CString cfgPath); 
	//��������е�ģ���б�
	HRESULT GetMoudleList();
	HRESULT DownloadFile(CString srvPath,CString savePath,BOOL chekcCrc,DWORD crc);
	
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

	//����°汾
	//void CheckVersion(int id,BOOL *pbFind);
	void CheckVersion(BOOL *pbFind);
	//��������Ƿ���������
	void CheckAllowUpdate(int id,BOOL *allow);
	//����
	void SaveAllowUpdate(int id);
	void SetUpdateAddress(LPTSTR lpAppPath);
	//����CRC
	int CalcCrc(CString fileName);
	//ȡCRC��Ϣ
	void GetCrcInfo(BOOL *checkup,LPTSTR lptzHint,int size);


private:
	CMapStringToPtr m_mapSvrFileVersion;
	CMapStringToPtr m_mapCliFileVersion;
	DECLARE_MESSAGE_MAP()
};

extern CAutoUpdateCommonApp theApp;

//ULARGE_INTEGER GetDigitalVersionByString(LPCTSTR lpszVersion);
////�Ƚϰ汾
//BOOL VersionLess(LPCTSTR lpszVersionLeft, LPCTSTR lpszVersionRight);

typedef void (CALLBACK *LogFn)(LPCTSTR lpszMsg); 
typedef void (CALLBACK *GetModuleFn)(int nAddr);
_declspec(selectany) LogFn Log1=NULL;
_declspec(selectany) GetModuleFn GetModule1=NULL;
void Log(LPTSTR lpszFormat,... );
DWORD CalcFileCrc(FILE* lpFile);


