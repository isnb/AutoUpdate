// Test1.h : Test1 DLL ����ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CTest1App
// �йش���ʵ�ֵ���Ϣ������� Test1.cpp
//

class CTest1App : public CWinApp
{
public:
	CTest1App();

// ��д
public:
	virtual BOOL InitInstance();

	DECLARE_MESSAGE_MAP()
};
