
// CalcCRC32.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CCalcCRC32App:
// �йش����ʵ�֣������ CalcCRC32.cpp
//

class CCalcCRC32App : public CWinApp
{
public:
	CCalcCRC32App();

// ��д
public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CCalcCRC32App theApp;