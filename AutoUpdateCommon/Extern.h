
EXTERN_C _declspec(dllexport) int DownloadFile(LPTSTR svrPath,LPTSTR savePath);
//EXTERN_C _declspec(dllexport) void CheckVersion(int id,BOOL *pbFindVersion);
EXTERN_C _declspec(dllexport) void CheckVersion(BOOL *pbFindVersion);
EXTERN_C _declspec(dllexport) void SetLogFn(LPVOID lpFn);
//设取到客户端DLL的回调函数
EXTERN_C _declspec(dllexport) void SetGetModuleFn(LPVOID lpFn);
EXTERN_C _declspec(dllexport) void GetAppPath(LPTSTR lpAppPath);
EXTERN_C _declspec(dllexport) void SetUpdateAddress(LPTSTR lpAppPath);
EXTERN_C _declspec(dllexport) BOOL Execute(LPCTSTR lpszProcessName,LPCTSTR strCmdLine, BOOL bForceAdmin, BOOL bWaitProcess);
EXTERN_C _declspec(dllexport) int GetHashCode(LPTSTR lpStr);
EXTERN_C _declspec(dllexport) void GetCrcInfo(BOOL *checkup,LPTSTR lptzInfo,int nSize);
//下载差异文件
EXTERN_C _declspec(dllexport) bool DownloadDiffFile();


BOOL GetTokenByName(HANDLE   &hToken,LPTSTR lpName); 
