#pragma once

class CAgoraSignalInstance
{
public:
	static CAgoraSignalInstance* getSignalInstance(const std::string &appId = "");
	void ReleaseInstance();
	~CAgoraSignalInstance();

	void setLoginWnd(HWND wnd);
	HWND getLoginWnd();
	void setChatMsgWnd(HWND wnd);
	HWND getChatMsgWnd();

	bool getptStatus();
	void setptStatus(bool status);
	bool getChannelStatus();
	void setChannleStatus(bool status);

	void setAppId(const std::string &appId);
	std::string getAppId();
	void setAppCertificateId(const std::string &certificateId);
	std::string getCertificateId();
	void setChannelKey(const std::string &channelKey);
	std::string getChannelKey();
	std::string getLoginAccount();

	IAgoraAPI* getAgoraAPI();

	bool Login(const std::string &account);
	bool LogOut();

	bool QueryIsOnline(const std::string &account);
	bool sendInstantMsg(const std::string &account, const std::string &instanmsg);
	bool sendChannelMsg(const std::string &channel, const std::string &ChannelMsg);

	bool JoinChannel(const std::string &channelID);
	bool LeaveChannel();

	std::string getSDKVersion();

protected:
	CAgoraSignalInstance(const std::string appId);

private:

	static CAgoraSignalInstance* m_pAgoraSignalInstance;

	bool m_isLoginPt;
	bool m_isJoinChannel;
	
	std::string m_Account;
	std::string m_ChannelName;

	std::string m_AppId;
	std::string m_AppCertificateId;
	std::string m_channelKey;

	IAgoraAPI* m_AgoraAPI;
	HWND m_loginWnd;
	HWND m_ChatMsg;
};