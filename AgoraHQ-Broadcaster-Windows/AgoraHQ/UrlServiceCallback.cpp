#include "stdafx.h"
#include "UrlServiceCallback.h"
#include "CustomMsg.h"
#include <memory>
#include <string>

CUrlServiceCallback::CUrlServiceCallback(){}
CUrlServiceCallback::~CUrlServiceCallback(){}

void CUrlServiceCallback::SetMsgReceiver(HWND hWnd)
{
	m_hMainWnd = hWnd;
}

void PostCustomMessage(const HWND& hWnd, const UINT& uMsg, std::string js_res, std::string error)
{
	if (hWnd == NULL)
		return;

	bool bSuccess = true;
	std::string str = js_res;
	if (!error.empty())
	{
		bSuccess = false;
		str = error;
	}
	char* szJson = new char[str.length() + 1];
	szJson[str.length() + 1];
	memset(szJson, 0, str.length() + 1);
	memcpy(szJson, str.c_str(), str.length());
	LPARAM lParam = (LPARAM)szJson;
	::PostMessage(hWnd, uMsg, bSuccess, lParam);
}

void CUrlServiceCallback::onRequestChannel_Callback(std::string js_res, std::string error)
{
	PostCustomMessage(m_hMainWnd, WM_URL_MSG(URL_REQUEST_CHANNEL), js_res, error);
}

void CUrlServiceCallback::onSendQuestion_Callback(std::string js_res, std::string error)
{ 
	PostCustomMessage(m_hMainWnd, WM_URL_MSG(URL_PUBLISH), js_res, error);
}

void CUrlServiceCallback::onStopAnswer_Callback(std::string js_res, std::string error)
{
	PostCustomMessage(m_hMainWnd, WM_URL_MSG(URL_STOP_ANS), js_res, error);
}

void CUrlServiceCallback::onReset_Callback(std::string js_res, std::string error)
{
	PostCustomMessage(m_hMainWnd, WM_URL_MSG(URL_RESET), js_res, error);
}

void CUrlServiceCallback::onInviteRequest_Callback(std::string js_res, std::string error)
{
	PostCustomMessage(m_hMainWnd, WM_URL_MSG(URL_INVITE), js_res, error);
}


void CUrlServiceCallback::onInviteStatus_Callback(std::string js_res, std::string error)
{
	PostCustomMessage(m_hMainWnd, WM_URL_MSG(URL_INVITE_STATUS), js_res, error);
}
