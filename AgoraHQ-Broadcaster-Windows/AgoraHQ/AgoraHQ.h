
// AgoraHQ.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������
class CUrlServiceCallback;

// CAgoraHQApp: 
// �йش����ʵ�֣������ AgoraHQ.cpp
//

class CAgoraHQApp : public CWinApp
{
public:
	CAgoraHQApp();

// ��д
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
// ʵ��

	DECLARE_MESSAGE_MAP()
public:
	CUrlServiceCallback* url_service_callback = nullptr;
};

extern CAgoraHQApp theApp;