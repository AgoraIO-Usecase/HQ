#pragma once

#include <string>
#include <vector>
#include <stdint.h>

const std::string server_url = "http://123.155.153.87:10000";
const std::string request_channel_method = "/v1/requestChannel";
const std::string reset_method = "/v1/reset";
const std::string publish_method = "/v1/publish";
const std::string stop_answer_method = "/v1/stopAnswer";
const std::string invite_request_method = "/v1/inviteRequest";
const std::string invite_status_method = "/v1/inviteStatus";
enum COURSE_TYPE
{
	MIN_COURSE_TYPE = 0,
	OPEN_COURSE_TYPE,
};
/*{gid: "房号", encrypt: "若加密则v1,不然不需要传", lang: "0为英文，1位中文"}*/
typedef struct tagGetChannelRequest
{
	tagGetChannelRequest()
	{
		gid = "";
		encrypt = false;
		lang = "0";
	}
	std::string gid;
	bool        encrypt;
	std::string lang; 
}GetChannelRequest, *PGetChannelRequest;

typedef struct tagInviteStatus
{
	bool        responded;
	bool        accept;
	std::string mediaUID;//连麦uid
	std::string uid; //remoteSigAccount,原来是account
}InviteStatus, *PInviteStatus, *LPInviteStatus;
