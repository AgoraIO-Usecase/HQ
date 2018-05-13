#pragma once
#include "UrlData.h"

class IUrlServiceCallback
{
public:
	IUrlServiceCallback(){}
	virtual ~IUrlServiceCallback(){}

	virtual void SetMsgReceiver(HWND hWnd)
	{
		(void)hWnd;
	}
	virtual HWND GetMsgReceiver()
	{
		return NULL;
	}

	virtual void onRequestChannel_Callback(std::string js_res, std::string error = "")
	{
	}

	virtual void onSendQuestion_Callback(std::string js_res, std::string error = "")
	{
	}

	virtual void onStopAnswer_Callback(std::string js_res, std::string error = "")
	{
	}

	virtual void onReset_Callback(std::string js_res, std::string error = "")
	{
		
	}

	virtual void onInviteRequest_Callback(std::string js_res, std::string error = "")
	{
		
	}

	virtual void onInviteStatus_Callback(std::string js_res, std::string error="")
	{

	}
};