#include "stdafx.h"
#include "json/json.h"
#include "Utility.h"
/*
模板
{"I":"Kill","P":[{"F":1}]}}
*/

//flag 2成功  3失败
CString ParseInstr(LPCSTR instr,OUT int *id,OUT short *flag)
{
	Json::Reader read;
	Json::Value val;
	const char *instruct;
	int nFlag=0;
	const char 
		*lpszP1,
		*lpszP2;
	CString 
		str1,
		str2;
	CString sResult;
	DWORD dw1=0;
	
	if(read.parse(instr,val))
	{
		instruct=val["i"].asCString();
		/* Kill 指令 
		F:1只杀死进程，F:2杀死后重启进程 
		//{"i":"Kill","p":[{"f"},{"pe"}]}
		*/
		try{
			sResult.Empty();
			if(_stricmp(instruct,"Kill")==0)
			{
				if(val.size()>=2)
				{
					 nFlag=val["f"].asInt();
					 lpszP1=val["p1"].asCString();
					 *id=val["id"].asInt();
					 str2=CA2T(lpszP1);
					 str1=CUtility::GetAppPathByName(str2);
					 if(str1.IsEmpty())
					 {
						 sResult.Append(_T("找不到进程！"));
					 }
					 else{
						 dw1=CUtility::GetProcessIdByName(str2);
						 if(nFlag==1)
						 {
							 CUtility::KillProcess(dw1);
							 sResult.Append(_T("结束进程成功。"));
						 }
						 else if(nFlag==2)
						 {
							 CUtility::KillProcess(dw1);
							 sResult.Append(_T("结束进程成功。"));
							 Sleep(1000);
							 CUtility::Execute(str1,NULL,FALSE,FALSE);
							 sResult.Append(_T("启动进程成功。"));
						 }
					 }
				}
			}
			*flag=2;
		}
		catch(...)
		{
			CUtility::LogErr("分析指令出错。");
			sResult.Append(_T("分析指令出错。"));
			*flag=3;

		}
	}
	return sResult;
}