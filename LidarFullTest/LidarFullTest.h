// LidarFullTest.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CLidarFullTestApp:
// �йش����ʵ�֣������ LidarFullTest.cpp
//

class CLidarFullTestApp : public CWinApp
{
public:
	CLidarFullTestApp();

// ��д
	public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CLidarFullTestApp theApp;