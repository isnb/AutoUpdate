// Test1.cpp : ���� DLL �ĳ�ʼ�����̡�
//

#include "stdafx.h"
#include "Test1.h"
#include "bdx.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

using namespace BkJson;

//
//TODO: ����� DLL ����� MFC DLL �Ƕ�̬���ӵģ�
//		��Ӵ� DLL �������κε���
//		MFC �ĺ������뽫 AFX_MANAGE_STATE ����ӵ�
//		�ú�������ǰ�档
//
//		����:
//
//		extern "C" BOOL PASCAL EXPORT ExportedFunction()
//		{
//			AFX_MANAGE_STATE(AfxGetStaticModuleState());
//			// �˴�Ϊ��ͨ������
//		}
//
//		�˺������κ� MFC ����
//		������ÿ��������ʮ����Ҫ������ζ��
//		��������Ϊ�����еĵ�һ�����
//		���֣������������ж������������
//		������Ϊ���ǵĹ��캯���������� MFC
//		DLL ���á�
//
//		�й�������ϸ��Ϣ��
//		����� MFC ����˵�� 33 �� 58��
//

// CTest1App

BEGIN_MESSAGE_MAP(CTest1App, CWinApp)
END_MESSAGE_MAP()


// CTest1App ����

CTest1App::CTest1App()
{
	// TODO: �ڴ˴���ӹ�����룬
	// ��������Ҫ�ĳ�ʼ�������� InitInstance ��
}


// Ψһ��һ�� CTest1App ����

CTest1App theApp;


// CTest1App ��ʼ��

BOOL CTest1App::InitInstance()
{
	CWinApp::InitInstance();

	/*BkJson::Document jsonDoc;*/

	return TRUE;
}


void test()
{
	//BkJson::Document jsonDoc;
	//CString str1=_T("afdafa");
	//CString &str=str1;
	//BDX_ToJson<CString>(str, str, FALSE);
	BkJson::Document doc;
}