
// d3dDemo.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// Cd3dDemoApp:
// �йش����ʵ�֣������ d3dDemo.cpp
//

class Cd3dDemoApp : public CWinApp
{
public:
	Cd3dDemoApp();

// ��д
public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern Cd3dDemoApp theApp;