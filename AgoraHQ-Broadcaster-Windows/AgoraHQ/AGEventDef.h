#ifndef _AG_EVENT_DEFINE_H_ 
#define _AG_EVENT_DEFINE_H_

#define WM_MSGID(code) (WM_USER+0x200+code)
#define WM_EVTID(code) (code-0x200-WM_USER)

typedef struct _AGE_JOINCHANNEL_SUCCESS
{
	char	*channel;
	uid_t	uid;
	int		elapsed;

} AGE_JOINCHANNEL_SUCCESS, *PAGE_JOINCHANNEL_SUCCESS, *LPAGE_JOINCHANNEL_SUCCESS;

#define EID_JOINCHANNEL_SUCCESS			0x00000001

typedef AGE_JOINCHANNEL_SUCCESS AGE_REJOINCHANNEL_SUCCESS, *PAGE_REJOINCHANNEL_SUCCESS, *LPAGE_REJOINCHANNEL_SUCCESS ;

#define EID_REJOINCHANNEL_SUCCESS		0x00000002

typedef struct _AGE_ERROR
{
	int		err;
	char	*msg;

} AGE_ERROR, *PAGE_ERROR, *LPAGE_ERROR;

#define EID_ERROR						0x00000003

typedef struct _AGE_AUDIO_QUALITY {
	uid_t			uid;
	int				quality;
	unsigned short	delay;
	unsigned short	lost;

} AGE_AUDIO_QUALITY, *PAGE_AUDIO_QUALITY, *LPAGE_AUDIO_QUALITY;

#define EID_AUDIO_QUALITY				0x00000004

typedef struct _AGE_AUDIO_TRANSPORT_QUALITY
{
	uid_t			uid; 
	unsigned short	delay;
	unsigned short	lost;

} AGE_AUDIO_TRANSPORT_QUALITY, *PAGE_AUDIO_TRANSPORT_QUALITY, *LPAGE_AUDIO_TRANSPORT_QUALITY;

#define EID_AUDIO_TRANSPORT_QUALITY		0x00000005

typedef AGE_AUDIO_TRANSPORT_QUALITY AGE_VIDEO_TRANSPORT_QUALITY, *PAGE_VIDEO_TRANSPORT_QUALITY, *LPAGE_VIDEO_TRANSPORT_QUALITY;

#define EID_VUDIO_TRANSPORT_QUALITY		0x00000006

typedef struct _AGE_RECAP
{
	char*	recapData;
	int		length;

} AGE_RECAP, *PAGE_RECAP, *LPAGE_RECAP;

#define EID_RECAP						0x00000007

typedef struct _AGE_AUDIO_VOLUME_INDICATION 
{
	AudioVolumeInfo		*speakers;
	unsigned int		speakerNumber;
	int					totalVolume;

} AGE_AUDIO_VOLUME_INDICATION, *PAGE_AUDIO_VOLUME_INDICATION, *LPAGE_AUDIO_VOLUME_INDICATION;

#define EID_AUDIO_VOLUME_INDICATION			0x00000008

typedef struct _AGE_LEAVE_CHANNEL
{
	RtcStats	rtcStat;

} AGE_LEAVE_CHANNEL, *PAGE_LEAVE_CHANNEL, *LPAGE_LEAVE_CHANNEL;

#define EID_LEAVE_CHANNEL					0x00000009

typedef AGE_LEAVE_CHANNEL AGE_UPDATE_SESSION_STATS, *PAGE_UPDATE_SESSION_STATS, *LPAGE_UPDATE_SESSION_STATS;

#define EID_UPDATE_SESSION_STATS			0x0000000A

typedef struct _AGE_MEDIA_ENGINE_EVENT
{
	int evt;

} AGE_MEDIA_ENGINE_EVENT, *PAGE_MEDIA_ENGINE_EVENT, *LPAGE_MEDIA_ENGINE_EVENT;

#define EID_MEDIA_ENGINE_EVENT				0x0000000B

typedef struct _AGE_AUDIO_DEVICE_STATE_CHANGED
{
	char	*deviceId;
	int		deviceType;
	int		deviceState;

} AGE_AUDIO_DEVICE_STATE_CHANGED, *PAGE_AUDIO_DEVICE_STATE_CHANGED, *LPAGE_AUDIO_DEVICE_STATE_CHANGED;

#define EID_AUDIO_DEVICE_STATE_CHANGED		0x0000000C


typedef struct _AGE_LASTMILE_QUALITY
{
	int quality;

} AGE_LASTMILE_QUALITY, *PAGE_LASTMILE_QUALITY, *LPAGE_LASTMILE_QUALITY;

#define EID_LASTMILE_QUALITY					0x0000000D

typedef struct _AGE_FIRST_LOCAL_VIDEO_FRAME
{
	int width;
	int height;
	int elapsed;

} AGE_FIRST_LOCAL_VIDEO_FRAME, *PAGE_FIRST_LOCAL_VIDEO_FRAME, *LPAGE_FIRST_LOCAL_VIDEO_FRAME;

#define EID_FIRST_LOCAL_VIDEO_FRAME			0x0000000E

typedef struct _AGE_FIRST_REMOTE_VIDEO_DECODED
{
	uid_t	uid;
	int		width;
	int		height;
	int		elapsed;

} AGE_FIRST_REMOTE_VIDEO_DECODED, *PAGE_FIRST_REMOTE_VIDEO_DECODED, *LPAGE_FIRST_REMOTE_VIDEO_DECODED;

#define EID_FIRST_REMOTE_VIDEO_DECODED		0x0000000F

typedef AGE_FIRST_REMOTE_VIDEO_DECODED AGE_FIRST_REMOTE_VIDEO_FRAME, *PAGE_FIRST_REMOTE_VIDEO_FRAME, *LPAGE_FIRST_REMOTE_VIDEO_FRAME;

#define EID_FIRST_REMOTE_VIDEO_FRAME		0x00000010

typedef struct _AGE_USER_JOINED
{
	uid_t	uid;
	int		elapsed;

} AGE_USER_JOINED, *PAGE_USER_JOINED, *LPAGE_USER_JOINED;

#define EID_USER_JOINED						0x00000011

typedef struct _AGE_USER_OFFLINE
{
	uid_t	uid;
	USER_OFFLINE_REASON_TYPE		reason;

} AGE_USER_OFFLINE, *PAGE_USER_OFFLINE, *LPAGE_USER_OFFLINE;

#define EID_USER_OFFLINE					0x00000012

typedef struct _AGE_USER_MUTE_AUDIO
{
	uid_t uid;
	bool  muted;

} AGE_USER_MUTE_AUDIO, *PAGE_USER_MUTE_AUDIO, *LPAGE_USER_MUTE_AUDIO;

#define EID_USER_MUTE_AUDIO					0x00000013

typedef AGE_USER_MUTE_AUDIO AGE_USER_MUTE_VIDEO, *PAGE_USER_MUTE_VIDEO, *LPAGE_USER_MUTE_VIDEO;

#define EID_USER_MUTE_VIDEO					0x00000014

typedef struct _AGE_AUDIO_RECORDER_EXCEPTION
{
	int elapsed;

} AGE_AUDIO_RECORDER_EXCEPTION, *PAGE_AUDIO_RECORDER_EXCEPTION, *LPAGE_AUDIO_RECORDER_EXCEPTION;

#define EID_AUDIO_RECORDER_EXCEPTION		0x00000015

typedef struct _AGE_LOCAL_VIDEO_STAT
{
	int sentBitrate;
	int sentFrameRate;

} AGE_LOCAL_VIDEO_STAT, *PAGE_LOCAL_VIDEO_STAT, *LPAGE_LOCAL_VIDEO_STAT;

#define EID_LOCAL_VIDEO_STAT				0x00000016

typedef struct _AGE_REMOTE_VIDEO_STAT
{
	uid_t uid;
	int delay;
	int width;
	int height;
	int receivedBitrate;
	int receivedFrameRate;

} AGE_REMOTE_VIDEO_STAT, *PAGE_REMOTE_VIDEO_STAT, *LPAGE_REMOTE_VIDEO_STAT;

#define EID_REMOTE_VIDEO_STAT				0x00000017

#define EID_CAMERA_READY					0x00000018
#define EID_CONNECTION_LOST					0x00000019

#define EID_VENDOR_MESSAGE					0x0000001A
typedef struct _AGE_VENDOR_MESSAGE
{
	uid_t uid;
	char* data;
	size_t length;

} AGE_VENDOR_MESSAGE, *PAGE_VENDOR_MESSAGE, *LPAGE_VENDOR_MESSAGE;


typedef struct _AGE_VIDEO_DEVICE_STATE_CHANGED
{
	char	*deviceId;
	int		deviceType;
	int		deviceState;

} AGE_VIDEO_DEVICE_STATE_CHANGED, *PAGE_VIDEO_DEVICE_STATE_CHANGED, *LPAGE_VIDEO_DEVICE_STATE_CHANGED;

#define EID_VIDEO_DEVICE_STATE_CHANGED		0x0000001B

#define EID_VIDEO_STOPPED					0x0000001C

#define EID_START_RCDSRV					0x0000001D
#define EID_STOP_RCDSRV						0x0000001E

typedef struct _AGE_RCDSRV_STATUS
{
	int status;

}AGE_RCDSRV_STATUS, *PAGE_RCDSRV_STATUS, *LPAGE_RCDSRV_STATUS;

#define EID_REFREASH_RCDSRV					0x0000001F

typedef struct _AGE_APICALL_EXECUTED
{
	char	api[128];
	int		error;

}AGE_APICALL_EXECUTED, *PAGE_APICALL_EXECUTED, *LPAGE_APICALL_EXECUTED;

#define EID_APICALL_EXECUTED					0x00000020

typedef struct _AGE_PUBLISH_PARAM
{
	int width;
	int height;
	int fps;
	int bitrate;
	std::string rtmpUrl;
}AGE_PUBLISH_PARAM,*PAGE_PUBLISH_PARAM,*LPAGE_PUBLISH_PARAM;

#define EID_REQUEST_CHANNELKEY 0x00000021
#define EID_WARNING			0x00000022

namespace SingleDesc
{
	enum SingleMsg
	{
		WM_Default = WM_MSGID(EID_WARNING),
		WM_Reconnecting,
		WM_Reconnected,
		WM_LoginSuccess,
		WM_LogOut,
		WM_LoginFailed,
		WM_ChannelJoined,
		WM_ChannelJoinedFailed,
		WM_ChannelLeaved,
		WM_ChannelUserJoined,
		WM_ChannelUserLeaved,
		WM_ChannelUserList,
		WM_ChannelQueryUserNumResult,
		WM_ChannelQueryUserIsIn,
		WM_ChannelAttrUpdated,
		WM_InviteReceived,
		WM_InviteReceivedByPeer,
		WM_InviteAcceptedByPeer,
		WM_InviteRefusedByPeer,
		WM_InviteFailed,
		WM_InviteEndByPeer,
		WM_InviteEndByMyself,
		WM_InviteMsg,
		WM_MessageSendError,
		WM_MessageSendProgress,
		WM_MessageSendSuccess,
		WM_MessageAppReceived,
		WM_MessageInstantReceive,
		WM_MessageChannelReceive,
		WM_Log,
		WM_InvokeRet,
		WM_Msg,
		WM_UserAttrResult,
		WM_UserAttrAllResult,
		WM_Error,
		WM_QueryUserStatusResult,
		WM_Dbg,
		WM_BCCall_result,

		WM_UNKWOWN = WM_USER + 0xff
	};

	enum eTagMsgtype
	{
		eType_Instance,
		eType_Channel,
	};

	typedef struct InPutParam
	{
		eTagMsgtype type;
		std::string strParam;
	}AG_INPUTPARAM, *PAG_INPUTPARAM, *LPAG_INPUTPARAM;

	typedef struct AGTagNewChannelName
	{
		std::string account;
		std::string channelname;
	}AG_SIGNAL_NEWCHANNELNAME, *PAG_SIGNAL_NEWCHANNELNAME, *LPAG_SIGNAL_NEWCHANNELNAME;

#define 	WM_InviteReceived_ChannelName  (WM_BCCall_result + 1)
#define WM_UpdateInputParam (WM_BCCall_result + 2)
#define WM_NewChannelName (WM_BCCall_result + 3)

	typedef struct AGTagStructReconnecting
	{
		uint32_t nretry;
	}AG_SIGNAL_RECONNECTING, *PAG_SIGNAL_RECONNECTING, *LPAG_SIGNAL_RECONECTING;

	typedef struct AGTagReconnected
	{
		int fd;
	}AG_SIGNAL_RECONNECTED, *PAG_SIGNAL_RECONNECTED, *LPAG_SIGNAL_RECONNECTED;

	typedef struct AGTagLoginSuccess
	{
		uint32_t uid;
		int fd;
	}AG_SIGNAL_LOGINSUCCESS, *PAG_SIGNAL_LOGINSUCCESS, *LPAG_SIGNAL_LOGINSUCCESS;

	typedef struct AGTagLogout
	{
		int ecode;
	}AG_SIGNAL_LOGOUT, *PAG_SIGNAL_LOGOUT, *LPAG_SIGNAL_LOGOUT;

	typedef struct AGTagLoginFailed
	{
		int ecode;
	}AG_SIGNAL_LOGINFAILED, *PAG_SIGNAL_LOGINFAILED, *LPAG_SIGNAL_LOGINFAILED;

	typedef struct AGTagChannelJoin
	{
		std::string channelID;
	}AG_SIGNAL_CHANNELJOIN, *PAG_SIGNAL_CHANNELJOIN, *LPAG_SIGNAL_CHANNELJOIN;

	typedef struct AGTagChannelJoinFailed
	{
		std::string channelID;
		int ecode;
	}AG_SIGNAL_CHANNELJOINFAILED, *PAG_SIGNAL_CHANNELJOINFAILED, *LPAG_SIGNAL_CHANNELJOINFAILED;

	typedef struct AGTagChannelLeave
	{
		std::string channelID;
		int ecode;
	}AG_SIGNAL_CHANNELLEAVE, *PAG_SIGNAL_CHANNELLEAVE, *LPAG_SIGNAL_CHANNELLEAVE;

	typedef struct  AGTagChannelUserJoin
	{
		std::string account;
		uint32_t uid;
	}AG_SIGNAL_CHANNELUSERJOIN, *PAG_SIGNAL_CHANNELUSERJOIN, *LPAG_SIGNAL_CHANNELUSERJOIN;

	typedef struct AGTagChannelUserLeave
	{
		std::string account;
		uint32_t uid;
	}AG_SIGNAL_CHANNELUSERLEAVE, *PAG_SIGNAL_CHANNELUSERLEAVE, *LPAG_SIGNAL_CHANNELUSERLEAVE;

	typedef struct AGTagChannelUserList
	{
		int nUserNum;
		char** ppAccount;
		uint32_t uids;
	}AG_SIGNAL_CHANNELUSERLIST, *PAG_SIGNAL_CHANNELUSERLIST, *LPAG_SIGNAL_CHANNELUSERLIST;

	typedef struct AGTagChannelQueryUserNumResult
	{
		std::string channelID;
		int ecode;
		int num;
	}AG_SIGNAL_CHNNELQUERYUSERNUMRESULT, *PAG_SIGNAL_CHNNELQUERYUSERNUMRESULT, *LPAG_SIGNAL_CHNNELQUERYUSERNUMRESULT;

	typedef struct AGTagChannelQueryUserIsln
	{
		std::string channelID;
		std::string account;
		int isIn;
	}AG_SIGNAL_CHANNELQUERYUSERISIN, *PAG_SIGNAL_CHANNELQUERYUSERISIN, *LPAG_SIGNAL_CHANNELQUERYUSERISIN;

	typedef struct AGTagChannelAttrUpdated
	{
		std::string channelD;
		std::string name;
		std::string value;
		std::string type;
	}AG_SIGNAL_CHANNELATTRUPDATED, *PAG_SIGNAL_CHANNELATTRUPDATED, *LPAG_SIGNAL_CHANNELATTRUPDATED;

	typedef struct AGTagInviteReceived
	{
		std::string channelID;
		std::string account;
		uint32_t uid;
		std::string extra;
	}AG_SIGNAL_INVITERECEIVED, *PAG_SIGNAL_INVITERECEIVED, *LPAG_SIGNAL_INVITERECEIVED;

	typedef struct AGTagInviteReceivedByPeer
	{
		std::string channelID;
		std::string account;
		uint32_t uid;
	}AG_SIGNAL_INVITERECEIVEDBYPEER, *PAG_SIGNAL_INVITERECEIVEDBYPEER, *LPAG_SIGNAL_INVITERECEIVEDBYPEER;

	typedef struct AGTagInviteAcceptedByPeer
	{
		std::string channelID;
		std::string account;
		int uid;
		std::string extra;
	}AG_SIGNAL_INVITEACCEPTEDBYPEER, *PAG_SIGNAL_INVITEACCEPTEDBYPEER, *LPAG_SIGNAL_INVITEACCEPTEDBYPEER;

	typedef struct AGTagInviteRefusedByPeer
	{
		std::string channelID;
		std::string account;
		uint32_t uid;
		std::string extra;
	}AG_SIGNAL_INVITEREFUSEDBYPEER, *PAG_SIGNAL_INVITEREFUSEDBYPEER, *LPAG_SIGNAL_INVITEREFUSEDBYPEER;

	typedef struct AGTagInviteFailed
	{
		std::string channelID;
		std::string account;
		uint32_t uid;
		int ecode;
		std::string extra;
	}AG_SIGNAL_INVITEFAILED, *PAG_SIGNAL_INVITEFAILED, *LPAG_SIGNAL_INVITEFAILED;

	typedef struct AGTagInviteEndByPeer
	{
		std::string channelID;
		std::string account;
		uint32_t uid;
		std::string extra;
	}AG_SIGNAL_INVITEENDBYPEER, *PAG_SIGNAL_INVITEENDBYPEER, *LPAG_SIGNAL_INVITEENDBYPEER;

	typedef struct AGTagInviteEndByMyself
	{
		std::string channelID;
		std::string account;
		uint32_t uid;
	}AG_SIGNAL_INVITEENDBYMYSELF, *PAG_SIGNAL_INVITEENDBYMYSELF, *LPAG_SIGNAL_INVITEENDBYMYSELF;

	typedef struct AGTagInviteMsg
	{
		std::string channelID;
		std::string account;
		uint32_t uid;
		std::string msgType;
		std::string msgData;
		std::string extra;
	}AG_SIGNAL_INVIDEMSG, *PAG_SIGNAL_INVIDEMSG, *LPAG_SIGNAL_INVIDEMSG;

	typedef struct AGTagMessageSendError
	{
		std::string message;
		int ecode;
	}AG_SIGNAL_MESSAGESENDERROR, *PAG_SIGNAL_MESSAGESENDERROR, *LPAG_SIGNAL_MESSAGESENDERROR;

	typedef struct AGTagMessageSendProgress
	{
		std::string account;
		std::string messageID;
		std::string type;
		std::string info;
	}AG_SIGNAL_MESSAGESENDPROGRESS, *PAG_SIGNAL_MESSAGESENDPROGRESS, *LPAG_SIGNAL_MESSAGESENDPROGRESS;

	typedef struct AGTagMessageSendSuccess
	{
		std::string messageID;
	}AG_SIGNAL_MESSAGESENDSUCCESS, *PAG_SIGNAL_MESSAGESENDSUCCESS, *LPAG_SIGNAL_MESSAGESENDSUCCESS;

	typedef struct AGTagMessageInstantReceive
	{
		std::string account;
		uint32_t uid;
		std::string msg;
	}AG_SIGNAL_MESSAGEINSTANCERECEIVE, *PAG_SIGNAL_MESSAGEINSTANCERECEIVE, *LPAG_SIGNAL_MESSAGEINSTANCERECEIVE;

	typedef struct AGTagMessageChannelReceive
	{
		std::string channelID;
		std::string account;
		uint32_t uid;
		std::string msg;
	}AG_SIGNAL_MESSAGECHANNELRECEIVE, *PAG_SIGNAL_MESSAGECHANNELRECEIVE, *LPAG_SIGNAL_MESSAGECHANNELRECEIVE;

	typedef struct AGTagLog
	{
		std::string txt;
	}AG_SIGNAL_LOG, *PAG_SIGNAL_LOG, *LPAG_SIGNAL_LOG;

	typedef struct AGTagError
	{
		std::string name;
		int ecode;
		std::string desc;
	}AG_SIGNAL_ERROR, *PAG_SIGNAL_ERROR, *LPAG_SIGNAL_ERROR;

	typedef struct AGTagQueryUserStatusResult
	{
		std::string name;
		std::string status;
	}AG_SIGNAL_QUERYUSERSTATUSRESULT, *PAG_SIGNAL_QUERYUSERSTATUSRESULT, *LPAG_SIGNAL_QUERYUSERSTATUSRESULT;
}

using namespace SingleDesc;

#endif