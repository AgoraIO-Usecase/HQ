#ifndef __SINGLEWRAP_H__
#define __SINGLEWRAP_H__

class CSingleCallBack:public ICallBack
{
public:
	CSingleCallBack(HWND wnd);
	~CSingleCallBack();
	
	void setCallBackWnd(HWND wnd);

protected:

	void onReconnecting(uint32_t nretry) ;
	void onReconnected(int fd) ;
	void onLoginSuccess(uint32_t uid, int fd) ;
	void onLogout(int ecode) ;
	void onLoginFailed(int ecode) ;
	void onChannelJoined(char const * channelID, size_t channelID_size) ;
	void onChannelJoinFailed(char const * channelID, size_t channelID_size, int ecode) ;
	void onChannelLeaved(char const * channelID, size_t channelID_size, int ecode) ;
	void onChannelUserJoined(char const * account, size_t account_size, uint32_t uid) ;
	void onChannelUserLeaved(char const * account, size_t account_size, uint32_t uid) ;
	void onChannelUserList(int n, char** accounts, uint32_t* uids) ;
	void onChannelQueryUserNumResult(char const * channelID, size_t channelID_size, int ecode, int num) ;
	void onChannelQueryUserIsIn(char const * channelID, size_t channelID_size, char const * account, size_t account_size, int isIn) ;
	void onChannelAttrUpdated(char const * channelID, size_t channelID_size, char const * name, size_t name_size, char const * value, size_t value_size, char const * type, size_t type_size) ;
	void onInviteReceived(char const * channelID, size_t channelID_size, char const * account, size_t account_size, uint32_t uid, char const * extra, size_t extra_size) ;
	void onInviteReceivedByPeer(char const * channelID, size_t channelID_size, char const * account, size_t account_size, uint32_t uid) ;
	void onInviteAcceptedByPeer(char const * channelID, size_t channelID_size, char const * account, size_t account_size, uint32_t uid, char const * extra, size_t extra_size) ;
	void onInviteRefusedByPeer(char const * channelID, size_t channelID_size, char const * account, size_t account_size, uint32_t uid, char const * extra, size_t extra_size) ;
	void onInviteFailed(char const * channelID, size_t channelID_size, char const * account, size_t account_size, uint32_t uid, int ecode, char const * extra, size_t extra_size) ;
	void onInviteEndByPeer(char const * channelID, size_t channelID_size, char const * account, size_t account_size, uint32_t uid, char const * extra, size_t extra_size) ;
	void onInviteEndByMyself(char const * channelID, size_t channelID_size, char const * account, size_t account_size, uint32_t uid) ;
	void onInviteMsg(char const * channelID, size_t channelID_size, char const * account, size_t account_size, uint32_t uid, char const * msgType, size_t msgType_size, char const * msgData, size_t msgData_size, char const * extra, size_t extra_size) ;
	void onMessageSendError(char const * messageID, size_t messageID_size, int ecode) ;
	void onMessageSendProgress(char const * account, size_t account_size, char const * messageID, size_t messageID_size, char const * type, size_t type_size, char const * info, size_t info_size) ;
	void onMessageSendSuccess(char const * messageID, size_t messageID_size) ;
	void onMessageAppReceived(char const * msg, size_t msg_size) ;
	void onMessageInstantReceive(char const * account, size_t account_size, uint32_t uid, char const * msg, size_t msg_size) ;
	void onMessageChannelReceive(char const * channelID, size_t channelID_size, char const * account, size_t account_size, uint32_t uid, char const * msg, size_t msg_size) ;
	void onLog(char const * txt, size_t txt_size) ;
	void onInvokeRet(char const * callID, size_t callID_size, char const * err, size_t err_size, char const * resp, size_t resp_size) ;
	void onMsg(char const * from, size_t from_size, char const * t, size_t t_size, char const * msg, size_t msg_size) ;
	void onUserAttrResult(char const * account, size_t account_size, char const * name, size_t name_size, char const * value, size_t value_size) ;
	void onUserAttrAllResult(char const * account, size_t account_size, char const * value, size_t value_size) ;
	void onError(char const * name, size_t name_size, int ecode, char const * desc, size_t desc_size) ;
	void onQueryUserStatusResult(char const * name, size_t name_size, char const * status, size_t status_size) ;
	void onDbg(char const * a, size_t a_size, char const * b, size_t b_size) ;
	void onBCCall_result(char const * reason, size_t reason_size, char const * json_ret, size_t json_ret_size, char const * callID, size_t callID_size) ;

private: 
	inline void postMsg(UINT msg, WPARAM wParam = NULL ,LPARAM lParam = NULL);
	 
private:
	HWND m_MsgWnd;
};


#endif