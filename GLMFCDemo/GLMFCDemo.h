
// GLMFCDemo.h : GLMFCDemo Ӧ�ó������ͷ�ļ�
//
#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"       // ������


// CGLMFCDemoApp:
// �йش����ʵ�֣������ GLMFCDemo.cpp
//

class CGLMFCDemoApp : public CWinAppEx
{
public:
	CGLMFCDemoApp();


// ��д
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

// ʵ��
	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
};

extern CGLMFCDemoApp theApp;
