#include "stdafx.h"
#include "commonFun.h"
#include "SingleWrap.h"
#include "SignalInstance.h"

CSingleCallBack::CSingleCallBack(HWND wnd) :m_MsgWnd(wnd)
{
}

CSingleCallBack::~CSingleCallBack()
{
	m_MsgWnd = nullptr;
}

void CSingleCallBack::setCallBackWnd(HWND wnd)
{
	m_MsgWnd = wnd;
}

void CSingleCallBack::onReconnecting(uint32_t nretry)
{
}

void CSingleCallBack::onReconnected(int fd)
{
}

void CSingleCallBack::onLoginSuccess(uint32_t uid, int fd)
{
	PAG_SIGNAL_LOGINSUCCESS lpData = new AG_SIGNAL_LOGINSUCCESS;
	lpData->fd = fd;
	lpData->uid = uid;

	postMsg(WM_LoginSuccess,WPARAM(lpData));
}

void CSingleCallBack::onLogout(int ecode)
{
	PAG_SIGNAL_LOGOUT lpData = new AG_SIGNAL_LOGOUT;
	lpData->ecode = ecode;

	postMsg(WM_LogOut,WPARAM(lpData),LPARAM(ecode));
}

void CSingleCallBack::onLoginFailed(int ecode)
{
	PAG_SIGNAL_LOGINFAILED lpData = new AG_SIGNAL_LOGINFAILED;
	lpData->ecode = ecode;

	postMsg(WM_LoginFailed, WPARAM(lpData), LPARAM(ecode));
}

void CSingleCallBack::onChannelJoined(char const * channelID, size_t channelID_size)
{
	PAG_SIGNAL_CHANNELJOIN lpData = new AG_SIGNAL_CHANNELJOIN;
	lpData->channelID = utf82gbk(channelID);

	postMsg(WM_ChannelJoined,WPARAM(lpData));
}

void CSingleCallBack::onChannelJoinFailed(char const * channelID, size_t channelID_size, int ecode)
{
	PAG_SIGNAL_CHANNELJOINFAILED lpData = new AG_SIGNAL_CHANNELJOINFAILED;
	lpData->channelID = utf82gbk(channelID);
	lpData->ecode = ecode;

	postMsg(WM_ChannelJoinedFailed,WPARAM(lpData));
}

void CSingleCallBack::onChannelLeaved(char const * channelID, size_t channelID_size, int ecode)
{
	PAG_SIGNAL_CHANNELLEAVE lpData = new AG_SIGNAL_CHANNELLEAVE;
	lpData->channelID = utf82gbk(channelID);
	lpData->ecode = ecode;

	postMsg(WM_ChannelLeaved,WPARAM(lpData));
}

void CSingleCallBack::onChannelUserJoined(char const * account, size_t account_size, uint32_t uid)
{
}

void CSingleCallBack::onChannelUserLeaved(char const * account, size_t account_size, uint32_t uid)
{
}

void CSingleCallBack::onChannelUserList(int n, char** accounts, uint32_t* uids)
{
}

void CSingleCallBack::onChannelQueryUserNumResult(char const * channelID, size_t channelID_size, int ecode, int num)
{
}

void CSingleCallBack::onChannelQueryUserIsIn(char const * channelID, size_t channelID_size, char const * account, size_t account_size, int isIn)
{

}

void CSingleCallBack::onChannelAttrUpdated(char const * channelID, size_t channelID_size, char const * name, size_t name_size, char const * value, size_t value_size, char const * type, size_t type_size)
{
}

void CSingleCallBack::onInviteReceived(char const * channelID, size_t channelID_size, char const * account, size_t account_size, uint32_t uid, char const * extra, size_t extra_size)
{
}

void CSingleCallBack::onInviteReceivedByPeer(char const * channelID, size_t channelID_size, char const * account, size_t account_size, uint32_t uid)
{
}

void CSingleCallBack::onInviteAcceptedByPeer(char const * channelID, size_t channelID_size, char const * account, size_t account_size, uint32_t uid, char const * extra, size_t extra_size)
{
}

void CSingleCallBack::onInviteRefusedByPeer(char const * channelID, size_t channelID_size, char const * account, size_t account_size, uint32_t uid, char const * extra, size_t extra_size)
{
}

void CSingleCallBack::onInviteFailed(char const * channelID, size_t channelID_size, char const * account, size_t account_size, uint32_t uid, int ecode, char const * extra, size_t extra_size)
{
}

void CSingleCallBack::onInviteEndByPeer(char const * channelID, size_t channelID_size, char const * account, size_t account_size, uint32_t uid, char const * extra, size_t extra_size)
{
}

void CSingleCallBack::onInviteEndByMyself(char const * channelID, size_t channelID_size, char const * account, size_t account_size, uint32_t uid)
{
}

void CSingleCallBack::onInviteMsg(char const * channelID, size_t channelID_size, char const * account, size_t account_size, uint32_t uid, char const * msgType, size_t msgType_size, char const * msgData, size_t msgData_size, char const * extra, size_t extra_size)
{
}

void CSingleCallBack::onMessageSendError(char const * messageID, size_t messageID_size, int ecode)
{
	PAG_SIGNAL_MESSAGESENDERROR lpData = new AG_SIGNAL_MESSAGESENDERROR;
	lpData->message = utf82gbk(messageID);
	lpData->ecode = ecode;

	postMsg(WM_MessageSendError,WPARAM(lpData));
}

void CSingleCallBack::onMessageSendProgress(char const * account, size_t account_size, char const * messageID, size_t messageID_size, char const * type, size_t type_size, char const * info, size_t info_size)
{
}

void CSingleCallBack::onMessageSendSuccess(char const * messageID, size_t messageID_size)
{
	PAG_SIGNAL_MESSAGESENDSUCCESS  lpData = new AG_SIGNAL_MESSAGESENDSUCCESS;
	lpData->messageID = utf82gbk(messageID);

	postMsg(WM_MessageSendSuccess,WPARAM(lpData));
}

void CSingleCallBack::onMessageAppReceived(char const * msg, size_t msg_size)
{
}

void CSingleCallBack::onMessageInstantReceive(char const * account, size_t account_size, uint32_t uid, char const * msg, size_t msg_size)
{
	PAG_SIGNAL_MESSAGEINSTANCERECEIVE lpData = new AG_SIGNAL_MESSAGEINSTANCERECEIVE;
	lpData->account = utf82gbk(account);
	lpData->uid = uid;
	lpData->msg = utf82gbk(msg);

	postMsg(WM_MessageInstantReceive,WPARAM(lpData));
}

void CSingleCallBack::onMessageChannelReceive(char const * channelID, size_t channelID_size, char const * account, size_t account_size, uint32_t uid, char const * msg, size_t msg_size)
{
	PAG_SIGNAL_MESSAGECHANNELRECEIVE lpData = new AG_SIGNAL_MESSAGECHANNELRECEIVE;
	lpData->account = utf82gbk(account);
	lpData->channelID = utf82gbk(channelID);
	lpData->uid = uid;
	lpData->msg = utf82gbk(msg);

	postMsg(WM_MessageChannelReceive,WPARAM(lpData));
}

void CSingleCallBack::onLog(char const * txt, size_t txt_size)
{
	PAG_SIGNAL_LOG lpData = new AG_SIGNAL_LOG;
	lpData->txt = utf82gbk(txt);

	HWND MainWnd = CAgoraSignalInstance::getSignalInstance()->getLoginWnd();
	SendMessage(MainWnd, WM_Log, WPARAM(lpData), NULL);
}

void CSingleCallBack::onInvokeRet(char const * callID, size_t callID_size, char const * err, size_t err_size, char const * resp, size_t resp_size)
{
}

void CSingleCallBack::onMsg(char const * from, size_t from_size, char const * t, size_t t_size, char const * msg, size_t msg_size)
{
}

void CSingleCallBack::onUserAttrResult(char const * account, size_t account_size, char const * name, size_t name_size, char const * value, size_t value_size)
{
}

void CSingleCallBack::onUserAttrAllResult(char const * account, size_t account_size, char const * value, size_t value_size)
{
}

void CSingleCallBack::onError(char const * name, size_t name_size, int ecode, char const * desc, size_t desc_size)
{
	PAG_SIGNAL_ERROR lpData = new AG_SIGNAL_ERROR;
	lpData->name = utf82gbk(name);
	lpData->ecode = ecode;
	lpData->desc = utf82gbk(desc);

	HWND MainWnd = CAgoraSignalInstance::getSignalInstance()->getLoginWnd();
	::PostMessage(MainWnd,WM_Error, WPARAM(lpData),NULL);
}

void CSingleCallBack::onQueryUserStatusResult(char const * name, size_t name_size, char const * status, size_t status_size)
{
	PAG_SIGNAL_QUERYUSERSTATUSRESULT lpData = new AG_SIGNAL_QUERYUSERSTATUSRESULT;
	lpData->name = utf82gbk(name);
	lpData->status = utf82gbk(status);

	postMsg(WM_QueryUserStatusResult,WPARAM(lpData));
}

void CSingleCallBack::onDbg(char const * a, size_t a_size, char const * b, size_t b_size)
{
}

void CSingleCallBack::onBCCall_result(char const * reason, size_t reason_size, char const * json_ret, size_t json_ret_size, char const * callID, size_t callID_size)
{
}

void CSingleCallBack::postMsg(UINT msg, WPARAM wParam /*= NULL*/, LPARAM lParam /*= NULL*/)
{
	::PostMessage(m_MsgWnd, msg, wParam, lParam);
}