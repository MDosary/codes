
// epiminfo.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CepiminfoApp:
// �йش����ʵ�֣������ epiminfo.cpp
//

class CepiminfoApp : public CWinApp
{
public:
	CepiminfoApp();

// ��д
public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CepiminfoApp theApp;