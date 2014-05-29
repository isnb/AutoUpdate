#include "StdAfx.h"
#include "Utility.h"
#include "winsvc.h"


//�����ļ� 
const int nSpecialFileCount=2;
const TCHAR sSpecialFile[nSpecialFileCount][25]={
	_T("WINSERVICEUPDATE.EXE"),_T("AUTOUPDATECOMMON.DLL")
};

Utility::Utility(void)
{
}


Utility::~Utility(void)
{
}

UINT StopService_dan( LPVOID lp1 )
{
	int nRetry=0;
	int nMaxRetry=5;
	CString sn=(LPTSTR)lp1;

	// �򿪷���������
	SC_HANDLE hSC = ::OpenSCManager( NULL, 
		NULL, GENERIC_EXECUTE);
	if( hSC == NULL)
	{
		Log(_T("�򿪷������������\r\n"));
		return FALSE;
	}

	SC_HANDLE hSvc = ::OpenService(hSC,sn,
		SERVICE_START | SERVICE_QUERY_STATUS | SERVICE_STOP);
	if(hSvc == NULL)
	{
		Log(_T("�򿪷������\r\n"));
		::CloseServiceHandle( hSC);
		return FALSE;
	}

	SERVICE_STATUS status;
	if( ::QueryServiceStatus( hSvc, &status) == FALSE)
	{
		Log(_T("��ѯ����״̬����\r\n"));
		::CloseServiceHandle( hSvc);
		::CloseServiceHandle( hSC);
		return FALSE;
	}

	//�������ֹͣ״̬���������񣬷���ֹͣ����
	if( status.dwCurrentState == SERVICE_RUNNING)
	{
STOP:
		// ֹͣ����
		for (nRetry=0;nRetry<=nMaxRetry;nRetry++)
		{
			if(::ControlService( hSvc, 
				SERVICE_CONTROL_STOP, &status) == TRUE)
				break;
			if( status.dwCurrentState == SERVICE_STOPPED)
			{
				break;
			}
			::Sleep(100);
			if(nRetry==nMaxRetry)
			{
				Log(_T("����ֹͣ����ʱ����\r\n"));
				::CloseServiceHandle( hSvc);
				::CloseServiceHandle( hSC);
				return FALSE;
			}
		}

		Log(_T("����ֹͣ����,���ȴ�����ֹͣ��\r\n"));
		// �ȴ�����ֹͣ
		while( ::QueryServiceStatus( hSvc, &status) == TRUE)
		{
			::Sleep( status.dwWaitHint);
			if( status.dwCurrentState == SERVICE_STOPPED)
			{
				Log(_T("����ɹ�ֹͣ��\r\n"));
				break;
			}
			if( status.dwCurrentState == SERVICE_RUNNING)
			{
				goto STOP;
			}
		}
	}
	else
	{
		Log(_T("����û������!\r\n"));
	}

	::CloseServiceHandle( hSvc);
	::CloseServiceHandle( hSC);

	////�ƶ��ļ� 
	//Log(_T("���ڸ����ļ���\r\n"));
	//GetAppPath(tz1);
	//sTmp.AppendFormat(_T("%s\\%s"),tz1,sUpdateDir);
	//MoveDirectory(sTmp.GetString(),tz1);
	//Log(_T("�ļ�������ϣ�������������\r\n"));

	return TRUE;
}

UINT StartService_dan( LPVOID lp1 )
{
	int nRetry=0;
	int nMaxRetry=5;
	CString sn=(LPTSTR)lp1;
	// �򿪷���������
	SC_HANDLE hSC = ::OpenSCManager( NULL, 
		NULL, GENERIC_EXECUTE);
	if( hSC == NULL)
	{
		Log(_T("�򿪷������������\r\n"));
		return FALSE;
	}

	SC_HANDLE hSvc = ::OpenService(hSC,sn,
		SERVICE_START | SERVICE_QUERY_STATUS | SERVICE_STOP);
	if(hSvc == NULL)
	{
		Log(_T("�򿪷������\r\n"));
		::CloseServiceHandle( hSC);
		return FALSE;
	}
	SERVICE_STATUS status;
	if( ::QueryServiceStatus( hSvc, &status) == FALSE)
	{
		Log(_T("��ѯ����״̬����\r\n"));
		::CloseServiceHandle( hSvc);
		::CloseServiceHandle( hSC);
		return FALSE;
	}
	
	if(status.dwCurrentState == SERVICE_STOP)
	{
		Log(_T("���������С����Ե�!\r\n"));
		// ��������
		if( ::StartService( hSvc, NULL, NULL) == FALSE)
		{
			Log(_T("��������������\r\n"));
			::CloseServiceHandle( hSvc);
			::CloseServiceHandle( hSC);
			return FALSE;
		}
		// �ȴ���������
		nRetry=0;
		while( ::QueryServiceStatus( hSvc, &status) == TRUE)
		{
			::Sleep( status.dwWaitHint);
			// ��������
			if(status.dwCurrentState == SERVICE_STOP)
			{
				::StartService( hSvc, NULL, NULL) ;
				nRetry++;
			}

			if( status.dwCurrentState == SERVICE_RUNNING)
			{
				Log(_T("����ɹ�������\r\n"));
				break;
			}
			if(nRetry==nMaxRetry)
				break;
		}
	}
	else
	{
		if( status.dwCurrentState == SERVICE_RUNNING)
		{
			Log(_T("�����Ѿ�������\r\n"));
		}
	}
	::CloseServiceHandle( hSvc);
	::CloseServiceHandle( hSC);
	return TRUE;
}


// ��pc�е��ļ��д�һ��Ŀ¼�����������һ��Ŀ¼
BOOL MoveDirectory(CString strSrcPath, CString strDesPath)
{
	if( strSrcPath.IsEmpty() )
	{       
		return FALSE;
	}

	if ( !PathIsDirectory(strDesPath) )
	{
		if ( !CreateDirectory(strDesPath,NULL))
			return FALSE;
	}

	if ( strSrcPath.GetAt(strSrcPath.GetLength()-1) != '\\' )
		strSrcPath += '\\';
	if ( strDesPath.GetAt(strDesPath.GetLength()-1) != '\\' )
		strDesPath += '\\';

	BOOL bRet = FALSE; // ��ΪԴĿ¼������Ϊ�գ����Ը�ֵһ���ᱻ�޸�
	CFileFind ff;  
	BOOL bFound = ff.FindFile(strSrcPath+_T("*"),   0);  
	CString strFile;
	BOOL bSpecialFile=FALSE;
	while(bFound)      // �ݹ鿽��
	{  
		bFound = ff.FindNextFile();  
		bSpecialFile=FALSE;
		if( ff.IsDots() )  
			continue;

		CString strSubSrcPath = ff.GetFilePath();
		CString strSubDespath = strSubSrcPath;
		strSubDespath.Replace(strSrcPath, strDesPath);

		if( ff.IsDirectory() )
			bRet = MoveDirectory(strSubSrcPath, strSubDespath);     // �ݹ鿽���ļ���
		else
		{
			strFile=PathFindFileName(strSubSrcPath);
			strFile.MakeUpper();
			for (int i=0;i<nSpecialFileCount;i++)
			{
				//�ҵ������ļ�
				if(_tcscmp(strFile.GetString(),sSpecialFile[i])==0)
				{
					bSpecialFile=TRUE;
					break;
				}	
			}
			if(bSpecialFile)
				bRet=MoveFileEx( strSubSrcPath,strSubDespath,MOVEFILE_DELAY_UNTIL_REBOOT|MOVEFILE_REPLACE_EXISTING);
			else
				bRet = MoveFileEx(strSubSrcPath, strSubDespath,MOVEFILE_REPLACE_EXISTING);   // �ƶ��ļ�
		}
		if ( !bRet )
			break;
	}  
	ff.Close();
	return bRet;
}



