#pragma once
class Utility
{
public:
	Utility(void);
	~Utility(void);
};


//重启服务
UINT StopService_dan(LPVOID lp1);
UINT StartService_dan(LPVOID lp1);

// 将pc中的文件夹从一个目录拷贝到另外的一个目录
BOOL MoveDirectory(CString strSrcPath, CString strDesPath);

typedef void (CALLBACK *LogFn)(LPTSTR msg);
_declspec(selectany) LogFn Log=NULL;
	 

