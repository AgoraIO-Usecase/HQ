#include "stdafx.h"
#include "signalinstance.h"
#include "commonFun.h"

CAgoraSignalInstance* CAgoraSignalInstance::m_pAgoraSignalInstance = NULL;

CAgoraSignalInstance* CAgoraSignalInstance::getSignalInstance(const std::string &appId /*= ""*/)
{
	if (NULL == m_pAgoraSignalInstance){
		
		m_pAgoraSignalInstance = new CAgoraSignalInstance(appId);
	}

	return m_pAgoraSignalInstance;
}

void CAgoraSignalInstance::ReleaseInstance()
{
	if (m_pAgoraSignalInstance){
		delete m_pAgoraSignalInstance;
		m_pAgoraSignalInstance = nullptr;
	}
}

CAgoraSignalInstance::CAgoraSignalInstance(const std::string appId):
m_ChatMsg(NULL),
m_loginWnd(NULL),
m_AgoraAPI(NULL),
m_isLoginPt(FALSE),
m_isJoinChannel(FALSE),
m_Account(""),
m_ChannelName(""),
m_channelKey(""),
m_AppId(appId),
m_AppCertificateId("")
{
	m_AgoraAPI = getAgoraSDKInstanceWin(m_AppId.data(), m_AppId.size());
}

CAgoraSignalInstance::~CAgoraSignalInstance()
{
	m_AgoraAPI = NULL;
}

void CAgoraSignalInstance::setLoginWnd(HWND wnd)
{
	m_loginWnd = wnd;
}

HWND CAgoraSignalInstance::getLoginWnd()
{
	return m_loginWnd;
}

void CAgoraSignalInstance::setChatMsgWnd(HWND wnd)
{
	m_ChatMsg = wnd;
}

HWND CAgoraSignalInstance::getChatMsgWnd()
{
	return m_ChatMsg;
}

bool CAgoraSignalInstance::getptStatus()
{
	return m_isLoginPt;
}

void CAgoraSignalInstance::setptStatus(bool status)
{
	m_isLoginPt = status;
}

bool CAgoraSignalInstance::getChannelStatus()
{
	return m_isJoinChannel;
}

void CAgoraSignalInstance::setChannleStatus(bool status)
{
	m_isJoinChannel = status;
}


void CAgoraSignalInstance::setAppId(const std::string &appId)
{
	m_AppId = appId;
}

std::string CAgoraSignalInstance::getAppId()
{
	return m_AppId;
}

void CAgoraSignalInstance::setAppCertificateId(const std::string &certificateId)
{
	m_AppCertificateId = certificateId;
}

std::string CAgoraSignalInstance::getCertificateId()
{
	return m_AppCertificateId;
}

void CAgoraSignalInstance::setChannelKey(const std::string &channelKey)
{
	m_channelKey = channelKey;
}

std::string CAgoraSignalInstance::getChannelKey()
{
	return m_channelKey;
}

std::string CAgoraSignalInstance::getLoginAccount()
{
	return m_Account;
}

IAgoraAPI* CAgoraSignalInstance::getAgoraAPI()
{
	return m_AgoraAPI;
}

bool CAgoraSignalInstance::Login(const std::string &account)
{
	if (m_AgoraAPI){
		m_Account = account;
		int retryCount = 3;
		int retryTime = 30;
		m_AgoraAPI->login2(gbk2utf8(m_AppId).data(), gbk2utf8(m_AppId).size(), gbk2utf8(m_Account).data(), gbk2utf8(m_Account).size(), gbk2utf8(m_channelKey).data(), gbk2utf8(m_channelKey).size(), 0, "", 0, retryTime, retryCount);
		return TRUE;
	}

	return FALSE;
}

bool CAgoraSignalInstance::LogOut()
{
	if (m_AgoraAPI){
		m_AgoraAPI->logout();
	}

	return FALSE;
}

bool CAgoraSignalInstance::QueryIsOnline(const std::string &account)
{
	if (m_AgoraAPI){
		m_AgoraAPI->queryUserStatus(gbk2utf8(account).data(), gbk2utf8(account).size());
		return TRUE;
	}

	return FALSE;
}

bool CAgoraSignalInstance::sendInstantMsg(const std::string &account, const std::string &instanmsg)
{
	if (m_AgoraAPI){
		std::string msgId = "p2pMsg";
		m_AgoraAPI->messageInstantSend(gbk2utf8(account).data(), gbk2utf8(account).size(), 0, gbk2utf8(instanmsg).data(), gbk2utf8(instanmsg).size(), gbk2utf8(msgId).data(), gbk2utf8(msgId).size());
	}

	return FALSE;
}

bool CAgoraSignalInstance::sendChannelMsg(const std::string &channel, const std::string &ChannelMsg)
{
	if (m_AgoraAPI){
		std::string msgId = "ChannelMsg";
		m_AgoraAPI->messageChannelSend(gbk2utf8(channel).data(), gbk2utf8(channel).size(), gbk2utf8(ChannelMsg).data(), gbk2utf8(ChannelMsg).size(), gbk2utf8(msgId).data(), gbk2utf8(msgId).size());
		return TRUE;
	}

	return FALSE;
}

bool CAgoraSignalInstance::JoinChannel(const std::string &channelID)
{
	if (m_AgoraAPI){
		m_AgoraAPI->channelJoin(gbk2utf8(channelID).data(), gbk2utf8(channelID).size());
		m_ChannelName = channelID;
		return TRUE;
	}

	return FALSE;
}

bool CAgoraSignalInstance::LeaveChannel()
{
	if (m_AgoraAPI){
		m_AgoraAPI->channelLeave(gbk2utf8(m_ChannelName).data(), gbk2utf8(m_ChannelName).size());
		return TRUE;
	}

	return FALSE;
}

std::string CAgoraSignalInstance::getSDKVersion()
{
	if (m_AgoraAPI){
		return  std::string("SDKVERSION: ") +int2str(m_AgoraAPI->getSdkVersion());
	}
	
	return "";
}