#pragma once
class Utility
{
public:
	Utility(void);
	~Utility(void);
};


//��������
UINT StopService_dan(LPVOID lp1);
UINT StartService_dan(LPVOID lp1);

// ��pc�е��ļ��д�һ��Ŀ¼�����������һ��Ŀ¼
BOOL MoveDirectory(CString strSrcPath, CString strDesPath);

typedef void (CALLBACK *LogFn)(LPTSTR msg);
_declspec(selectany) LogFn Log=NULL;
	 

