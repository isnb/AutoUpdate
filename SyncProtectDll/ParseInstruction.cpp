#include "stdafx.h"
#include "json/json.h"
#include "Utility.h"
/*
ģ��
{"I":"Kill","P":[{"F":1}]}}
*/

//flag 2�ɹ�  3ʧ��
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
		/* Kill ָ�� 
		F:1ֻɱ�����̣�F:2ɱ������������ 
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
						 sResult.Append(_T("�Ҳ������̣�"));
					 }
					 else{
						 dw1=CUtility::GetProcessIdByName(str2);
						 if(nFlag==1)
						 {
							 CUtility::KillProcess(dw1);
							 sResult.Append(_T("�������̳ɹ���"));
						 }
						 else if(nFlag==2)
						 {
							 CUtility::KillProcess(dw1);
							 sResult.Append(_T("�������̳ɹ���"));
							 Sleep(1000);
							 CUtility::Execute(str1,NULL,FALSE,FALSE);
							 sResult.Append(_T("�������̳ɹ���"));
						 }
					 }
				}
			}
			*flag=2;
		}
		catch(...)
		{
			CUtility::LogErr("����ָ�����");
			sResult.Append(_T("����ָ�����"));
			*flag=3;

		}
	}
	return sResult;
}