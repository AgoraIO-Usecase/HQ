#pragma once

#include <string>
#include <vector>
#include <stdint.h>

const std::string server_url = "http://123.155.153.87:10000";
const std::string server_url_en = "http://76.231.168.66:10000";
const std::string request_channel_method = "/v1/requestChannel";
const std::string reset_method = "/v1/reset";
const std::string publish_method = "/v1/publish";
const std::string stop_answer_method = "/v1/stopAnswer";
const std::string invite_request_method = "/v1/inviteRequest";
const std::string invite_status_method = "/v1/inviteStatus";

namespace HQ_LANG{

	static bool bLanguage = false;
	namespace ZH_CN{

		const TCHAR* const KError_Room_Not_Found = _T("游戏房间创建失败");
		const TCHAR* const KError_Game_closed_already = _T("本轮次答题已停止，勿重复提交“停止答题”操作");
		const TCHAR* const KError_Incorrect_quiz_id = _T("");
		const TCHAR* const KError_Game_closed = _T("");
		const TCHAR* const KError_Cannot_play = _T("");
		const TCHAR* const KError_Answer_given = _T("");
		const TCHAR* const KError_Quiz_going_on = _T("本轮次答题未结束，请先点击“停止答题”后，再开始下一轮次答题");
		const TCHAR* const KError_No_more_quiz = _T("本局游戏所有题目均已下发作答");
		const TCHAR* const KInfo_Reset = _T("重置题库成功,下一步选择SendQuestion");
		const TCHAR* const KError_NetBad = _T("网络未连接,请检查网络设置");
	}

	namespace UK{

		const TCHAR* const KError_Room_Not_Found = _T("Failed to create channel");
		const TCHAR* const KError_Game_closed_already = _T("This question has closed");
		const TCHAR* const KError_Incorrect_quiz_id = _T("");
		const TCHAR* const KError_Game_closed = _T("");
		const TCHAR* const KError_Cannot_play = _T("");
		const TCHAR* const KError_Answer_given = _T("");
		const TCHAR* const KError_Quiz_going_on = _T("The current round of question has not completed yet. Please click “Stop questions”to complete the current round, before send new questions");
		const TCHAR* const KError_No_more_quiz = _T("All questions completed");
		const TCHAR* const KInfo_Reset = _T("Reset Question Success.,the Next step is SendQuestion");
		const TCHAR* const KError_NetBad = _T("The network does not work properly. Please check the network setting.");
	}

	struct CHQLANG
	{
		static CString getError_Room_Not_Found(const CString &strChanel){

			TCHAR tszBuffer[MAX_PATH] = { _T('\0') };
			if (bLanguage)
				swprintf_s(tszBuffer, _T("%s \"%s\""), ZH_CN::KError_Room_Not_Found, strChanel);
			else 
				swprintf_s(tszBuffer, _T("%s \"%s\""), UK::KError_Room_Not_Found, strChanel);

			return tszBuffer;
		}

		static CString getError_Game_Closed_Already(){

			TCHAR tszBuffer[MAX_PATH] = { _T('\0') };
			if (bLanguage)
				swprintf_s(tszBuffer, _T("%s "), ZH_CN::KError_Game_closed_already);
			else
				swprintf_s(tszBuffer, _T("%s "), UK::KError_Game_closed_already);

			return tszBuffer;

		}

		static CString getError_Quiz_Going_On(){

			TCHAR tszBuffer[MAX_PATH] = { _T('\0') };
			if (bLanguage)
				swprintf_s(tszBuffer, _T("%s "), ZH_CN::KError_Quiz_going_on);
			else
				swprintf_s(tszBuffer, _T("%s "), UK::KError_Quiz_going_on);

			return tszBuffer;

		}

		static CString getError_No_More_Quiz(){

			TCHAR tszBuffer[MAX_PATH] = { _T('\0') };
			if (bLanguage)
				swprintf_s(tszBuffer, _T("%s "), ZH_CN::KError_No_more_quiz);
			else
				swprintf_s(tszBuffer, _T("%s "), UK::KError_No_more_quiz);

			return tszBuffer;
		}

		static CString getInfo_ResetInfo(){

			TCHAR tszBuffer[MAX_PATH] = { _T('\0') };
			if (bLanguage)
				swprintf_s(tszBuffer, _T("%s "), ZH_CN::KInfo_Reset);
			else
				swprintf_s(tszBuffer, _T("%s "), UK::KInfo_Reset);

			return tszBuffer;
		}

		static CString getError_NetBad(){

			TCHAR tszBuffer[MAX_PATH] = { _T('\0') };
			if (bLanguage)
				swprintf_s(tszBuffer, _T("%s "), ZH_CN::KError_NetBad);
			else
				swprintf_s(tszBuffer, _T("%s "), UK::KError_NetBad);

			return tszBuffer;
		}

		static std::string getResfAPIAddressIP(){

			return bLanguage == true ? server_url : server_url_en;
		}
	};
}

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
