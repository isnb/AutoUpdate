#ifdef CD_EXPORT
#define  CDWPI EXTERN_C _declspec(dllexport)
#else
#define  CDWPI EXTERN_C _declspec(dllimport)
#endif


CDWPI BOOL SetKeyboardHook();
CDWPI BOOL GetSvrUpdateVer(int *id,int *sv,int *cv);
CDWPI BOOL SaveSvrUpdateVer(int id);
CDWPI void GetShareData(LPVOID lpData);
CDWPI void SetShareData(LPVOID lpData);
CDWPI void SetBranchId(int id);
CDWPI int  GetBranchId();
CDWPI void ParseInstruction();
CDWPI void LogErr(LPCSTR msg);
