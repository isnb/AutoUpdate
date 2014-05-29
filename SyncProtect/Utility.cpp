#include "StdAfx.h"
#include "Utility.h"
#include "winsvc.h"


//特殊文件 
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

	// 打开服务管理对象
	SC_HANDLE hSC = ::OpenSCManager( NULL, 
		NULL, GENERIC_EXECUTE);
	if( hSC == NULL)
	{
		Log(_T("打开服务管理器错误！\r\n"));
		return FALSE;
	}

	SC_HANDLE hSvc = ::OpenService(hSC,sn,
		SERVICE_START | SERVICE_QUERY_STATUS | SERVICE_STOP);
	if(hSvc == NULL)
	{
		Log(_T("打开服务错误！\r\n"));
		::CloseServiceHandle( hSC);
		return FALSE;
	}

	SERVICE_STATUS status;
	if( ::QueryServiceStatus( hSvc, &status) == FALSE)
	{
		Log(_T("查询服务状态错误\r\n"));
		::CloseServiceHandle( hSvc);
		::CloseServiceHandle( hSC);
		return FALSE;
	}

	//如果处于停止状态则启动服务，否则停止服务。
	if( status.dwCurrentState == SERVICE_RUNNING)
	{
STOP:
		// 停止服务
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
				Log(_T("尝试停止服务时出错！\r\n"));
				::CloseServiceHandle( hSvc);
				::CloseServiceHandle( hSC);
				return FALSE;
			}
		}

		Log(_T("尝试停止服务,正等待服务停止！\r\n"));
		// 等待服务停止
		while( ::QueryServiceStatus( hSvc, &status) == TRUE)
		{
			::Sleep( status.dwWaitHint);
			if( status.dwCurrentState == SERVICE_STOPPED)
			{
				Log(_T("服务成功停止。\r\n"));
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
		Log(_T("服务没有运行!\r\n"));
	}

	::CloseServiceHandle( hSvc);
	::CloseServiceHandle( hSC);

	////移动文件 
	//Log(_T("正在更新文件。\r\n"));
	//GetAppPath(tz1);
	//sTmp.AppendFormat(_T("%s\\%s"),tz1,sUpdateDir);
	//MoveDirectory(sTmp.GetString(),tz1);
	//Log(_T("文件更新完毕，正在重启服务。\r\n"));

	return TRUE;
}

UINT StartService_dan( LPVOID lp1 )
{
	int nRetry=0;
	int nMaxRetry=5;
	CString sn=(LPTSTR)lp1;
	// 打开服务管理对象
	SC_HANDLE hSC = ::OpenSCManager( NULL, 
		NULL, GENERIC_EXECUTE);
	if( hSC == NULL)
	{
		Log(_T("打开服务管理器错误！\r\n"));
		return FALSE;
	}

	SC_HANDLE hSvc = ::OpenService(hSC,sn,
		SERVICE_START | SERVICE_QUERY_STATUS | SERVICE_STOP);
	if(hSvc == NULL)
	{
		Log(_T("打开服务错误！\r\n"));
		::CloseServiceHandle( hSC);
		return FALSE;
	}
	SERVICE_STATUS status;
	if( ::QueryServiceStatus( hSvc, &status) == FALSE)
	{
		Log(_T("查询服务状态错误\r\n"));
		::CloseServiceHandle( hSvc);
		::CloseServiceHandle( hSC);
		return FALSE;
	}
	
	if(status.dwCurrentState == SERVICE_STOP)
	{
		Log(_T("服务启动中。请稍等!\r\n"));
		// 启动服务
		if( ::StartService( hSvc, NULL, NULL) == FALSE)
		{
			Log(_T("启动服务发生错误。\r\n"));
			::CloseServiceHandle( hSvc);
			::CloseServiceHandle( hSC);
			return FALSE;
		}
		// 等待服务启动
		nRetry=0;
		while( ::QueryServiceStatus( hSvc, &status) == TRUE)
		{
			::Sleep( status.dwWaitHint);
			// 启动服务
			if(status.dwCurrentState == SERVICE_STOP)
			{
				::StartService( hSvc, NULL, NULL) ;
				nRetry++;
			}

			if( status.dwCurrentState == SERVICE_RUNNING)
			{
				Log(_T("服务成功启动。\r\n"));
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
			Log(_T("服务已经启动。\r\n"));
		}
	}
	::CloseServiceHandle( hSvc);
	::CloseServiceHandle( hSC);
	return TRUE;
}


// 将pc中的文件夹从一个目录拷贝到另外的一个目录
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

	BOOL bRet = FALSE; // 因为源目录不可能为空，所以该值一定会被修改
	CFileFind ff;  
	BOOL bFound = ff.FindFile(strSrcPath+_T("*"),   0);  
	CString strFile;
	BOOL bSpecialFile=FALSE;
	while(bFound)      // 递归拷贝
	{  
		bFound = ff.FindNextFile();  
		bSpecialFile=FALSE;
		if( ff.IsDots() )  
			continue;

		CString strSubSrcPath = ff.GetFilePath();
		CString strSubDespath = strSubSrcPath;
		strSubDespath.Replace(strSrcPath, strDesPath);

		if( ff.IsDirectory() )
			bRet = MoveDirectory(strSubSrcPath, strSubDespath);     // 递归拷贝文件夹
		else
		{
			strFile=PathFindFileName(strSubSrcPath);
			strFile.MakeUpper();
			for (int i=0;i<nSpecialFileCount;i++)
			{
				//找到特殊文件
				if(_tcscmp(strFile.GetString(),sSpecialFile[i])==0)
				{
					bSpecialFile=TRUE;
					break;
				}	
			}
			if(bSpecialFile)
				bRet=MoveFileEx( strSubSrcPath,strSubDespath,MOVEFILE_DELAY_UNTIL_REBOOT|MOVEFILE_REPLACE_EXISTING);
			else
				bRet = MoveFileEx(strSubSrcPath, strSubDespath,MOVEFILE_REPLACE_EXISTING);   // 移动文件
		}
		if ( !bRet )
			break;
	}  
	ff.Close();
	return bRet;
}



