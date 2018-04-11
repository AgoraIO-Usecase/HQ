#include "UrlService/IServiceCallback.h"

class CUrlServiceCallback : public IUrlServiceCallback
{
public:
	CUrlServiceCallback();
	~CUrlServiceCallback();

	virtual void SetMsgReceiver(HWND hWnd);
	virtual HWND GetMsgReceiver() { return m_hMainWnd; };
	virtual void onRequestChannel_Callback(std::string js_res, std::string error = "");
	virtual void onSendQuestion_Callback(std::string js_res, std::string error = "");
	virtual void onStopAnswer_Callback(std::string js_res, std::string error = "");
	virtual void onReset_Callback(std::string js_res, std::string error = "");
	virtual void onInviteRequest_Callback(std::string js_res, std::string error = "");
	virtual void onInviteStatus_Callback(std::string js_res, std::string error = "");
private:
	HWND		m_hMainWnd;
};