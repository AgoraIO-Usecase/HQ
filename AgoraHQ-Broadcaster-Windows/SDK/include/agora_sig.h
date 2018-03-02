
#ifndef _AGORA_SDK_WIN_H
#define _AGORA_SDK_WIN_H

#define NOWINMEDIA
#include <string>

#if defined(_WIN32)
#include <cstdint>
#endif

#ifndef NOWINMEDIA
#include "IAgoraRtcEngine.h"
#endif

namespace agora_sdk_win
{
    typedef enum {
        SUCCESS = 0,
        LOGOUT_E_OTHER = 100,
        LOGOUT_E_USER = 101,
        LOGOUT_E_NET = 102,
        LOGOUT_E_KICKED = 103,
        LOGOUT_E_PACKET = 104,
        LOGOUT_E_TOKENEXPIRED = 105,
        LOGOUT_E_OLDVERSION = 106,
        LOGOUT_E_TOKENWRONG = 107,
        LOGOUT_E_ALREADY_LOGOUT = 108,
        LOGIN_E_OTHER = 200,
        LOGIN_E_NET = 201,
        LOGIN_E_FAILED = 202,
        LOGIN_E_CANCEL = 203,
        LOGIN_E_TOKENEXPIRED = 204,
        LOGIN_E_OLDVERSION = 205,
        LOGIN_E_TOKENWRONG = 206,
        LOGIN_E_TOKEN_KICKED = 207,
        LOGIN_E_ALREADY_LOGIN = 208,
        LOGIN_E_INVALID_USER = 209,
        JOINCHANNEL_E_OTHER = 300,
        SENDMESSAGE_E_OTHER = 400,
        SENDMESSAGE_E_TIMEOUT = 401,
        QUERYUSERNUM_E_OTHER = 500,
        QUERYUSERNUM_E_TIMEOUT = 501,
        QUERYUSERNUM_E_BYUSER = 502,
        LEAVECHANNEL_E_OTHER = 600,
        LEAVECHANNEL_E_KICKED = 601,
        LEAVECHANNEL_E_BYUSER = 602,
        LEAVECHANNEL_E_LOGOUT = 603,
        LEAVECHANNEL_E_DISCONN = 604,
        INVITE_E_OTHER = 700,
        INVITE_E_REINVITE = 701,
        INVITE_E_NET = 702,
        INVITE_E_PEEROFFLINE = 703,
        INVITE_E_TIMEOUT = 704,
        INVITE_E_CANTRECV = 705,
        GENERAL_E = 1000,
        GENERAL_E_FAILED = 1001,
        GENERAL_E_UNKNOWN = 1002,
        GENERAL_E_NOT_LOGIN = 1003,
        GENERAL_E_WRONG_PARAM = 1004,
        GENERAL_E_LARGE_PARAM = 1005
    }AGORA_E_CODE;

  class ICallBack
  {
  public:
    virtual void onReconnecting(uint32_t nretry) {}
    virtual void onReconnected(int fd) {}
    virtual void onLoginSuccess(uint32_t uid,int fd) {}
    virtual void onLogout(int ecode) {}
    virtual void onLoginFailed(int ecode) {}
    virtual void onChannelJoined(char const * channelID, size_t channelID_size) {}
    virtual void onChannelJoinFailed(char const * channelID, size_t channelID_size,int ecode) {}
    virtual void onChannelLeaved(char const * channelID, size_t channelID_size,int ecode) {}
    virtual void onChannelUserJoined(char const * account, size_t account_size,uint32_t uid) {}
    virtual void onChannelUserLeaved(char const * account, size_t account_size,uint32_t uid) {}
    virtual void onChannelUserList(int n,char** accounts,uint32_t* uids) {}
    virtual void onChannelQueryUserNumResult(char const * channelID, size_t channelID_size,int ecode,int num) {}
    virtual void onChannelQueryUserIsIn(char const * channelID, size_t channelID_size,char const * account, size_t account_size,int isIn) {}
    virtual void onChannelAttrUpdated(char const * channelID, size_t channelID_size,char const * name, size_t name_size,char const * value, size_t value_size,char const * type, size_t type_size) {}
    virtual void onInviteReceived(char const * channelID, size_t channelID_size,char const * account, size_t account_size,uint32_t uid,char const * extra, size_t extra_size) {}
    virtual void onInviteReceivedByPeer(char const * channelID, size_t channelID_size,char const * account, size_t account_size,uint32_t uid) {}
    virtual void onInviteAcceptedByPeer(char const * channelID, size_t channelID_size,char const * account, size_t account_size,uint32_t uid,char const * extra, size_t extra_size) {}
    virtual void onInviteRefusedByPeer(char const * channelID, size_t channelID_size,char const * account, size_t account_size,uint32_t uid,char const * extra, size_t extra_size) {}
    virtual void onInviteFailed(char const * channelID, size_t channelID_size,char const * account, size_t account_size,uint32_t uid,int ecode,char const * extra, size_t extra_size) {}
    virtual void onInviteEndByPeer(char const * channelID, size_t channelID_size,char const * account, size_t account_size,uint32_t uid,char const * extra, size_t extra_size) {}
    virtual void onInviteEndByMyself(char const * channelID, size_t channelID_size,char const * account, size_t account_size,uint32_t uid) {}
    virtual void onInviteMsg(char const * channelID, size_t channelID_size,char const * account, size_t account_size,uint32_t uid,char const * msgType, size_t msgType_size,char const * msgData, size_t msgData_size,char const * extra, size_t extra_size) {}
    virtual void onMessageSendError(char const * messageID, size_t messageID_size,int ecode) {}
    virtual void onMessageSendProgress(char const * account, size_t account_size,char const * messageID, size_t messageID_size,char const * type, size_t type_size,char const * info, size_t info_size) {}
    virtual void onMessageSendSuccess(char const * messageID, size_t messageID_size) {}
    virtual void onMessageAppReceived(char const * msg, size_t msg_size) {}
    virtual void onMessageInstantReceive(char const * account, size_t account_size,uint32_t uid,char const * msg, size_t msg_size) {}
    virtual void onMessageChannelReceive(char const * channelID, size_t channelID_size,char const * account, size_t account_size,uint32_t uid,char const * msg, size_t msg_size) {}
    virtual void onLog(char const * txt, size_t txt_size) {}
    virtual void onInvokeRet(char const * callID, size_t callID_size,char const * err, size_t err_size,char const * resp, size_t resp_size) {}
    virtual void onMsg(char const * from, size_t from_size,char const * t, size_t t_size,char const * msg, size_t msg_size) {}
    virtual void onUserAttrResult(char const * account, size_t account_size,char const * name, size_t name_size,char const * value, size_t value_size) {}
    virtual void onUserAttrAllResult(char const * account, size_t account_size,char const * value, size_t value_size) {}
    virtual void onError(char const * name, size_t name_size,int ecode,char const * desc, size_t desc_size) {}
    virtual void onQueryUserStatusResult(char const * name, size_t name_size,char const * status, size_t status_size) {}
    virtual void onDbg(char const * a, size_t a_size,char const * b, size_t b_size) {}
    virtual void onBCCall_result(char const * reason, size_t reason_size,char const * json_ret, size_t json_ret_size,char const * callID, size_t callID_size) {}
  };

  class IAgoraAPI
  {
  public:
    virtual void callbackSet (ICallBack* handler) = 0;
    virtual ICallBack* callbackGet () = 0;
    virtual void login (char const * vendorID, size_t vendorID_size,char const * account, size_t account_size,char const * token, size_t token_size,uint32_t uid,char const * deviceID, size_t deviceID_size) = 0;
    virtual void login2 (char const * vendorID, size_t vendorID_size,char const * account, size_t account_size,char const * token, size_t token_size,uint32_t uid,char const * deviceID, size_t deviceID_size,int retry_time_in_s,int retry_count) = 0;
    virtual void logout () = 0;
    virtual void channelJoin (char const * channelID, size_t channelID_size) = 0;
    virtual void channelLeave (char const * channelID, size_t channelID_size) = 0;
    virtual void channelQueryUserNum (char const * channelID, size_t channelID_size) = 0;
    virtual void channelQueryUserIsIn (char const * channelID, size_t channelID_size,char const * account, size_t account_size) = 0;
    virtual void channelSetAttr (char const * channelID, size_t channelID_size,char const * name, size_t name_size,char const * value, size_t value_size) = 0;
    virtual void channelDelAttr (char const * channelID, size_t channelID_size,char const * name, size_t name_size) = 0;
    virtual void channelClearAttr (char const * channelID, size_t channelID_size) = 0;
    virtual void channelInviteUser (char const * channelID, size_t channelID_size,char const * account, size_t account_size,uint32_t uid=0) = 0;
    virtual void channelInviteUser2 (char const * channelID, size_t channelID_size,char const * account, size_t account_size,char const * extra, size_t extra_size) = 0;
    virtual void channelInvitePhone (char const * channelID, size_t channelID_size,char const * phoneNum, size_t phoneNum_size,uint32_t uid=0) = 0;
    virtual void channelInvitePhone2 (char const * channelID, size_t channelID_size,char const * phoneNum, size_t phoneNum_size,char const * sourcesNum, size_t sourcesNum_size) = 0;
    virtual void channelInvitePhone3 (char const * channelID, size_t channelID_size,char const * phoneNum, size_t phoneNum_size,char const * sourcesNum, size_t sourcesNum_size,char const * extra, size_t extra_size) = 0;
    virtual void channelInviteDTMF (char const * channelID, size_t channelID_size,char const * phoneNum, size_t phoneNum_size,char const * dtmf, size_t dtmf_size) = 0;
    virtual void channelInviteAccept (char const * channelID, size_t channelID_size,char const * account, size_t account_size,uint32_t uid) = 0;
    virtual void channelInviteRefuse (char const * channelID, size_t channelID_size,char const * account, size_t account_size,uint32_t uid,char const * extra, size_t extra_size) = 0;
    virtual void channelInviteEnd (char const * channelID, size_t channelID_size,char const * account, size_t account_size,uint32_t uid) = 0;
    virtual void messageAppSend (char const * msg, size_t msg_size,char const * msgID, size_t msgID_size) = 0;
    virtual void messageInstantSend (char const * account, size_t account_size,uint32_t uid,char const * msg, size_t msg_size,char const * msgID, size_t msgID_size) = 0;
    virtual void messageInstantSend2 (char const * account, size_t account_size,uint32_t uid,char const * msg, size_t msg_size,char const * msgID, size_t msgID_size,char const * options, size_t options_size) = 0;
    virtual void messageChannelSend (char const * channelID, size_t channelID_size,char const * msg, size_t msg_size,char const * msgID, size_t msgID_size) = 0;
    virtual void messageChannelSendForce (char const * channelID, size_t channelID_size,char const * msg, size_t msg_size,char const * msgID, size_t msgID_size) = 0;
    virtual void messagePushSend (char const * account, size_t account_size,uint32_t uid,char const * msg, size_t msg_size,char const * msgID, size_t msgID_size) = 0;
    virtual void messageChatSend (char const * account, size_t account_size,uint32_t uid,char const * msg, size_t msg_size,char const * msgID, size_t msgID_size) = 0;
    virtual void messageDTMFSend (uint32_t uid,char const * msg, size_t msg_size,char const * msgID, size_t msgID_size) = 0;
    virtual void setBackground (uint32_t bOut) = 0;
    virtual void setNetworkStatus (uint32_t bOut) = 0;
    virtual void ping () = 0;
    virtual void setAttr (char const * name, size_t name_size,char const * value, size_t value_size) = 0;
    virtual void getAttr (char const * name, size_t name_size) = 0;
    virtual void getAttrAll () = 0;
    virtual void getUserAttr (char const * account, size_t account_size,char const * name, size_t name_size) = 0;
    virtual void getUserAttrAll (char const * account, size_t account_size) = 0;
    virtual void queryUserStatus (char const * account, size_t account_size) = 0;
    virtual void invoke (char const * name, size_t name_size,char const * req, size_t req_size,char const * callID, size_t callID_size) = 0;
    virtual void start () = 0;
    virtual void stop () = 0;
    virtual bool isOnline () = 0;
    virtual int getStatus () = 0;
    virtual int getSdkVersion () = 0;
    virtual void bc_call (char const * func, size_t func_size,char const * json_args, size_t json_args_size,char const * callID, size_t callID_size) = 0;
    virtual void dbg (char const * a, size_t a_size,char const * b, size_t b_size) = 0;
    virtual void destory () = 0;
    
#ifndef NOWINMEDIA
    virtual agora::rtc::IRtcEngine* getMedia() = 0;
#endif
  };
}

#define LIB_PRE __declspec(dllexport)
extern "C" LIB_PRE agora_sdk_win::IAgoraAPI* getAgoraSDKInstanceWin(char const * vendorID, size_t vendorID_size);
extern "C" LIB_PRE agora_sdk_win::IAgoraAPI* createAgoraSDKInstanceWin(char const * vendorID, size_t vendorID_size);

#ifndef NOWINMEDIA
extern "C" LIB_PRE agora_sdk_win::IAgoraAPI* getAgoraSDKInstanceWinWithMedia(agora::rtc::IRtcEngine*, char const * vendorID, size_t vendorID_size);
#endif

#endif
