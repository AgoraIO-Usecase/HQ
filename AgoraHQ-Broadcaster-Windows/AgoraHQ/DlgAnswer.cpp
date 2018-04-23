// DlgAnswer.cpp : 实现文件
//

#include "stdafx.h"
#include "AgoraHQ.h"
#include "AgoraHQDlg.h"
#include "DlgAnswer.h"
#include "afxdialogex.h"
#include "SingleWrap.h"
#include "SignalInstance.h"
#include "generatorSignalToken.h"
#include "UrlService/CurlService.h"
#include "CustomMsg.h"
#include "UrlService/URLData.h"
using namespace HQ_LANG;
// CDlgAnswer 对话框

IMPLEMENT_DYNAMIC(CDlgAnswer, CDialogEx)

CDlgAnswer::CDlgAnswer(CWnd* pParent /*=NULL*/)
	: CDialogEx(CDlgAnswer::IDD, pParent),
	m_pSignalInstance(nullptr),
	m_pSignalCallBack(nullptr),
	m_nQuestionId(-1),
	m_pAgEngineEventHandle(nullptr)
{
	std::string strLogPath = getSigSdkLogPath();
	m_fileSigLog.openLog(strLogPath);
}

CDlgAnswer::~CDlgAnswer()
{
	AfxGetUrlService()->GetUrlCallback()->SetMsgReceiver(NULL);
	uninitAgoraSignal();
	uninitCtrl();
	m_fileSigLog.close();
}

void CDlgAnswer::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STATIC_QUESTION, m_trlQuestion);
	DDX_Control(pDX, IDC_CHECK_A, m_ckAnswerA);
	DDX_Control(pDX, IDC_CHECK_B, m_ckAnswerB);
	DDX_Control(pDX, IDC_CHECK_C, m_ckAnswerC);
	DDX_Control(pDX, IDC_CHECK_D, m_ckAnswerD);
	DDX_Control(pDX, IDC_BUTTON_GetQuestion, m_btnUpdateQuestion);
	DDX_Control(pDX, IDC_BUTTON_STARTANSWER, m_btnStartAnswer);
	DDX_Control(pDX, IDC_BUTTON_STOPANSWER, m_btnStopAnswer);
	DDX_Control(pDX, IDC_BUTTON_RESET, m_btnResetQuestion);
	DDX_Control(pDX, IDC_STATIC_NotifyInfo, m_ctlNoticeInfo);
}


BEGIN_MESSAGE_MAP(CDlgAnswer, CDialogEx)
	ON_WM_CLOSE()
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_CHECK_A, &CDlgAnswer::OnBnClickedCheckA)
	ON_BN_CLICKED(IDC_CHECK_B, &CDlgAnswer::OnBnClickedCheckB)
	ON_BN_CLICKED(IDC_CHECK_C, &CDlgAnswer::OnBnClickedCheckC)
	ON_BN_CLICKED(IDC_CHECK_D, &CDlgAnswer::OnBnClickedCheckD)
	ON_BN_CLICKED(IDC_BUTTON_GetQuestion, &CDlgAnswer::OnBnClickedButtonGetquestion)
	ON_BN_CLICKED(IDC_BUTTON_STARTANSWER, &CDlgAnswer::OnBnClickedButtonStartMark)
	ON_BN_CLICKED(IDC_BUTTON_STOPANSWER, &CDlgAnswer::OnBnClickedButtonStopanswer)
	ON_BN_CLICKED(IDC_BUTTON_SETBONUSES, &CDlgAnswer::OnBnClickedButtonSetbonuses)
	ON_MESSAGE(WM_LoginSuccess, onLoginSuccess)
	ON_MESSAGE(WM_LogOut, onLogout)
	ON_MESSAGE(WM_LoginFailed, onLogFailed)
	ON_MESSAGE(WM_Error, onError)
	ON_MESSAGE(WM_Log, onLog)
	ON_MESSAGE(WM_QueryUserStatusResult, onQueryUserStatusResult)
	ON_MESSAGE(WM_MessageSendSuccess, onMessageSendSuccess)
	ON_MESSAGE(WM_MessageSendError, onMessageSendError)
	ON_MESSAGE(WM_MessageInstantReceive, onMessageInstantReceive)
	ON_BN_CLICKED(IDC_BUTTON_RESET, &CDlgAnswer::OnBnClickedButtonReset)
	ON_MESSAGE(WM_UpdateInputParam, onInputParam)
	ON_MESSAGE(WM_SetDataTimeBonus, onSetDataTimeBonus)
	ON_MESSAGE(WM_URL_MSG(URL_PUBLISH), onHttpPublish)
	ON_MESSAGE(WM_URL_MSG(URL_REQUEST_CHANNEL), onRequestChannel)
	ON_MESSAGE(WM_URL_MSG(URL_RESET), onHttpReset)
	ON_MESSAGE(WM_URL_MSG(URL_STOP_ANS), onHttpStopAns)
	ON_MESSAGE(WM_URL_MSG(URL_INVITE), onHttpInvite)
	ON_MESSAGE(WM_URL_MSG(URL_INVITE_STATUS), onHttpInviteStatus)
END_MESSAGE_MAP()


// CDlgAnswer 消息处理程序
BOOL CDlgAnswer::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	
	AfxGetUrlService()->GetUrlCallback()->SetMsgReceiver(m_hWnd);

	initCtrl();
	initAgoraSignal();
	
	return TRUE;
}

void CDlgAnswer::initCtrl()
{
	m_account = gHQConfig.getSignalAccount();
	m_serverAccount = gHQConfig.getServerAccount();
	if ("" == m_serverAccount){
		m_serverAccount = "agora_hq_cc_server_en";
		gHQConfig.setServerAccount(m_serverAccount);
	}

	m_btnUpdateQuestion.EnableWindow(FALSE);
	m_btnStartAnswer.EnableWindow(FALSE);
	m_btnStopAnswer.EnableWindow(FALSE);

	m_btnStartAnswer.ShowWindow(SW_HIDE);
	m_btnUpdateQuestion.ShowWindow(SW_HIDE);
	m_btnStopAnswer.ShowWindow(SW_HIDE);
	m_btnResetQuestion.ShowWindow(SW_HIDE);

	m_ckAnswerA.ShowWindow(SW_HIDE);
	m_ckAnswerB.ShowWindow(SW_HIDE);
	m_ckAnswerC.ShowWindow(SW_HIDE);
	m_ckAnswerD.ShowWindow(SW_HIDE);
	
	m_pAgEngineEventHandle = CAgoraObject::getEngineEventHandle();
	m_trlQuestion.SetWindowTextW(_T("Welcome To AgoraHQ . Join a Channel to Start !"));
}

void CDlgAnswer::uninitCtrl()
{
	m_pAgEngineEventHandle = nullptr;
}

void CDlgAnswer::initAgoraSignal()
{
	m_appId = gHQConfig.getAppId();
	m_appCertificatId = gHQConfig.getAppCertificateId();

#if 0
	m_pSignalInstance = CAgoraSignalInstance::getSignalInstance(m_appId);
	ASSERT(m_pSignalInstance);
	m_pSignalInstance->setLoginWnd(m_hWnd);
	m_pSignalInstance->setAppCertificateId(m_appCertificatId);
	
	m_pSignalCallBack = new CSingleCallBack(m_hWnd);
	m_pSignalInstance->getAgoraAPI()->callbackSet(m_pSignalCallBack);
	m_pSignalInstance->getAgoraAPI()->dbg("lbs_100", 7, "1", 1);//HQ environment

	std::string sdkVersion = m_pSignalInstance->getSDKVersion();

	time_t ltime;
	time(&ltime);
	int expiredSecond = ltime + 3600;
	std::string channelKey = CGeneSignalToken::generateSignallingToken(m_account,m_appId,m_appCertificatId,expiredSecond);
	bool bEnableAppCertificatId = str2int(gHQConfig.getAppCertEnable());
	if (!bEnableAppCertificatId)
	channelKey = "_no_need_token";
	m_pSignalInstance->setChannelKey(channelKey);
	m_pSignalInstance->Login(m_account);
#endif
}

void CDlgAnswer::uninitAgoraSignal()
{
	if (nullptr != m_pSignalInstance)
	{
		m_pSignalInstance->LeaveChannel();
		m_pSignalInstance->LogOut();
		m_pSignalInstance->getAgoraAPI()->callbackSet(nullptr);
		if (nullptr != m_pSignalCallBack)
		{
			delete m_pSignalCallBack;
			m_pSignalCallBack = nullptr;
		}
		delete m_pSignalInstance;
		m_pSignalInstance = nullptr;
	}
}


void CDlgAnswer::OnTimer(UINT_PTR nIDEvent)
{
	if (invite_status_timer_event_id == nIDEvent)
	{
		std::string gid = gHQConfig.getChannelName();
		AfxGetUrlService()->invite_status(gid);
		if (++invite_status_count <= invite_status_max)
			SetTimer(invite_status_timer_event_id, invite_status_interval, NULL);
		else
			KillTimer(invite_status_timer_event_id);
	}
}
void CDlgAnswer::OnBnClickedCheckA()
{
	// TODO:  在此添加控件通知处理程序代码
	m_ckAnswerA.SetCheck(TRUE);
	bool bStatusA = m_ckAnswerA.GetCheck();
	if (bStatusA){
		m_ckAnswerB.SetCheck(FALSE);
		m_ckAnswerC.SetCheck(FALSE);
		m_ckAnswerD.SetCheck(FALSE);
	}
}


void CDlgAnswer::OnBnClickedCheckB()
{
	// TODO:  在此添加控件通知处理程序代码	
	m_ckAnswerB.SetCheck(TRUE);
	bool bStatusB = m_ckAnswerB.GetCheck();
	if (bStatusB){
		m_ckAnswerA.SetCheck(FALSE);
		m_ckAnswerC.SetCheck(FALSE);
		m_ckAnswerD.SetCheck(FALSE);
	}
}


void CDlgAnswer::OnBnClickedCheckC()
{
	// TODO:  在此添加控件通知处理程序代码
	m_ckAnswerC.SetCheck(TRUE);
	bool bStatusC = m_ckAnswerC.GetCheck();
	if (bStatusC){
		m_ckAnswerA.SetCheck(FALSE);
		m_ckAnswerB.SetCheck(FALSE);
		m_ckAnswerD.SetCheck(FALSE);
	}
}


void CDlgAnswer::OnBnClickedCheckD()
{
	// TODO:  在此添加控件通知处理程序代码
	m_ckAnswerD.SetCheck(TRUE);
	bool bStatusD = m_ckAnswerD.GetCheck();
	if (bStatusD){
		m_ckAnswerA.SetCheck(FALSE);
		m_ckAnswerB.SetCheck(FALSE);
		m_ckAnswerC.SetCheck(FALSE);
	}
}

//publish_method
void CDlgAnswer::OnBnClickedButtonGetquestion()
{
	// TODO:  在此添加控件通知处理程序代码
	m_btnUpdateQuestion.EnableScalar(FALSE);

	m_ctlNoticeInfo.SetWindowTextW(_T(""));

	char cJsonStr[512] = { '\0' };
	sprintf_s(cJsonStr, "{\"type\": \"publish\",\"msgid\":%u}",GetTickCount());
	OutputDebugStringA(cJsonStr);
	OutputDebugStringA("\r\n");
	//m_pSignalInstance->sendInstantMsg(m_serverAccount, cJsonStr);
	std::string gid = gHQConfig.getChannelName();
	AfxGetUrlService()->publish(gid);

	m_ckAnswerA.SetCheck(FALSE);
	m_ckAnswerB.SetCheck(FALSE);
	m_ckAnswerC.SetCheck(FALSE);
	m_ckAnswerD.SetCheck(FALSE);
}

void CDlgAnswer::OnBnClickedButtonStartMark()
{
	// TODO:  在此添加控件通知处理程序代码
	m_btnStartAnswer.EnableScalar(FALSE);
	m_ctlNoticeInfo.SetWindowTextW(_T(""));

	char cJsonStr[512] = { '\0' };
	int timeStamp = ::time(NULL);
	sprintf_s(cJsonStr, "{\"type\":\"setSEI\",\"data\" :	{\"questionId\":%d,\"timeStamp\":%d}}", m_nQuestionId, timeStamp);
	OutputDebugStringA(cJsonStr);
	OutputDebugStringA("\r\n");
	if (m_pAgEngineEventHandle){
		m_pAgEngineEventHandle->setSEI(cJsonStr);
	}

	m_btnStopAnswer.EnableScalar(TRUE);

	// TO DO 开始标记本地画面.
}

//stop_answer_method
void CDlgAnswer::OnBnClickedButtonStopanswer()
{
	// TODO:  在此添加控件通知处理程序代码
	m_btnStopAnswer.EnableScalar(FALSE);
	m_btnUpdateQuestion.EnableScalar(TRUE);
	m_ctlNoticeInfo.SetWindowTextW(_T(""));

	char cJsonStr[512] = { '\0' };
	sprintf_s(cJsonStr, "{\"type\":\"stopAnswer\",\"msgid\":%u}",GetTickCount());
	OutputDebugStringA(cJsonStr);
	OutputDebugStringA("\r\n");
	//m_pSignalInstance->sendInstantMsg(m_serverAccount, cJsonStr);
	std::string gid = gHQConfig.getChannelName();
	AfxGetUrlService()->stop_answer(gid);
}

//reset_method
void CDlgAnswer::OnBnClickedButtonReset()
{
	// TODO:  在此添加控件通知处理程序代码
	m_ctlNoticeInfo.SetWindowTextW(_T(""));

	char cJsonStr[512] = { '\0' };
	sprintf_s(cJsonStr, "{\"type\":\"reset\",\"msgid\":%u}",GetTickCount());
	OutputDebugStringA(cJsonStr);
	OutputDebugStringA("\r\n");
	
	//m_pSignalInstance->sendInstantMsg(m_serverAccount, cJsonStr);
	std::string gid = gHQConfig.getChannelName();
	AfxGetUrlService()->reset(gid);
}

void CDlgAnswer::OnBnClickedButtonSetbonuses()
{
	// TODO:  在此添加控件通知处理程序代码
}

//
HRESULT CDlgAnswer::onLoginSuccess(WPARAM wParam, LPARAM lParam)
{
	PAG_SIGNAL_LOGINSUCCESS lpData = (PAG_SIGNAL_LOGINSUCCESS)wParam;

	if (lpData){

		delete lpData; lpData = nullptr;
	}

	m_btnUpdateQuestion.EnableWindow(TRUE);
	m_btnStartAnswer.EnableWindow(TRUE);
	m_btnStopAnswer.EnableWindow(TRUE);

	OutputDebugStringA(__FUNCTION__);
	OutputDebugStringA("\r\n");

// use http instead of agora signal
/*	std::string curLanguage = gHQConfig.getLanguage();
	if ("" == curLanguage){
		curLanguage = "0";
		gHQConfig.setLanguage(curLanguage);
	}

	char cJsonStr[512] = { '\0' };
	std::string strEncrypt = (gHQConfig.getEnableEncrypt());
	
	if (strEncrypt =="" || "0" == strEncrypt){

		sprintf_s(cJsonStr, "{\"type\":\"RequestChannelName\",\"QuestionLanguage\":\"%s\",\"msgid\":%u}", curLanguage.data(),GetTickCount());
	}
	else{

		sprintf_s(cJsonStr, "{\"type\":\"RequestChannelName\",\"QuestionLanguage\":\"%s\",\"encrypt\":\"v1\",\"msgid\":%u}", curLanguage.data(),GetTickCount());
	}
	OutputDebugStringA(cJsonStr);
	OutputDebugStringA("\r\n");

//	gFileApp.write(cJsonStr);
	m_pSignalInstance->sendInstantMsg(m_serverAccount, cJsonStr);
*/
	GetChannelRequest request;
	HQ_LANG::bLanguage = str2int(gHQConfig.getLanguage());
	request.encrypt = (gHQConfig.getEnableEncrypt() != "" && gHQConfig.getEnableEncrypt() != "0");
	request.gid = gHQConfig.getChannelName();
	request.lang = gHQConfig.getLanguage();
	AfxGetUrlService()->request_channel(request);
	
	return TRUE;
}

HRESULT CDlgAnswer::onLogout(WPARAM wParam, LPARAM lParam)
{
	PAG_SIGNAL_LOGOUT lpData = (PAG_SIGNAL_LOGOUT)wParam;
	delete lpData; lpData = nullptr;

	return TRUE;
	OutputDebugStringA(__FUNCTION__);
	OutputDebugStringA("\r\n");
}

HRESULT CDlgAnswer::onLogFailed(WPARAM wParam, LPARAM lParam)
{
	PAG_SIGNAL_LOGINFAILED lpData = (PAG_SIGNAL_LOGINFAILED)wParam;
	m_fileSigLog.write(int2str(lpData->ecode));
	delete lpData; lpData = nullptr;

	return TRUE;
	OutputDebugStringA(__FUNCTION__);
	OutputDebugStringA("\r\n");
}

LRESULT CDlgAnswer::onError(WPARAM wParam, LPARAM lParam)
{
	PAG_SIGNAL_ERROR lpData = (PAG_SIGNAL_ERROR)wParam;

	m_fileSigLog.openLog(lpData->desc);

	delete lpData; lpData = nullptr;

	return TRUE;
	OutputDebugStringA(__FUNCTION__);
	OutputDebugStringA("\r\n");
}

LRESULT CDlgAnswer::onLog(WPARAM wParam, LPARAM lParam)
{
	PAG_SIGNAL_LOG lpData = (PAG_SIGNAL_LOG)wParam;

	m_fileSigLog.write(lpData->txt);
	delete lpData; lpData = nullptr;

	return TRUE;
	OutputDebugStringA(__FUNCTION__);
	OutputDebugStringA("\r\n");
}


HRESULT CDlgAnswer::onQueryUserStatusResult(WPARAM wParam, LPARAM lParam)
{
	PAG_SIGNAL_QUERYUSERSTATUSRESULT lpData = (PAG_SIGNAL_QUERYUSERSTATUSRESULT)wParam;
	char logDesc[MAXPATHLEN] = { '\0' };
	sprintf_s(logDesc, "onQueryUserStatusResult(%s,%u,%s,%u)", utf82gbk(lpData->name).data(), utf82gbk(lpData->name).size(), utf82gbk(lpData->status).data(), utf82gbk(lpData->status).size());
	//LOG_MSG(logDesc, LogType_CallBack);
	OutputDebugStringA(logDesc);
	OutputDebugStringA("\r\n");

	delete lpData; lpData = nullptr;
	return TRUE;
}

HRESULT CDlgAnswer::onMessageSendSuccess(WPARAM wParam, LPARAM lParam)
{
	PAG_SIGNAL_MESSAGESENDSUCCESS lpData = (PAG_SIGNAL_MESSAGESENDSUCCESS)wParam;
	char logDesc[MAXPATHLEN] = { '\0' };
	sprintf_s(logDesc, "onMessageSendSuccess(%s,%u)", lpData->messageID.data(), lpData->messageID.size());
	//LOG_MSG(logDesc, LogType_CallBack);
	OutputDebugStringA(logDesc);
	OutputDebugStringA("\r\n");

	delete lpData; lpData = nullptr;

	return TRUE;
}

HRESULT CDlgAnswer::onMessageSendError(WPARAM wParam, LPARAM lParam)
{
	PAG_SIGNAL_MESSAGESENDERROR lpData = (PAG_SIGNAL_MESSAGESENDERROR)wParam;
	char logDesc[MAXPATHLEN] = { '\0' };
	sprintf_s(logDesc, "%s,%u,%d", lpData->message.data(), lpData->message.size(), lpData->ecode);
	//LOG_MSG(logDesc, LogType_CallBack);
	OutputDebugStringA(logDesc);
	OutputDebugStringA("\r\n");

	delete lpData; lpData = nullptr;

	return TRUE;
}

HRESULT CDlgAnswer::onMessageInstantReceive(WPARAM wParam, LPARAM lParam)
{
	PAG_SIGNAL_MESSAGEINSTANCERECEIVE lpData = (PAG_SIGNAL_MESSAGEINSTANCERECEIVE)wParam;
	char logDesc[MAXPATHLEN] = { '\0' };
	sprintf_s(logDesc, "onMessageInstantReceive(%s,%u,%u,%s,%u)", lpData->account.data(), lpData->account.size(), lpData->uid, lpData->msg.data(), lpData->msg.size());
	//LOG_MSG(logDesc, LogType_CallBack);
	OutputDebugStringA(logDesc);
	OutputDebugStringA("\r\n");
	gFileApp.write(logDesc);
	//lpData->msg = "{\"type\":\"ListOfWinners\",\"data\":{\"num\":2,\"playerName\" : [\"play1\", \"play2\"]}}";

	rapidjson::Document document;
	if (document.Parse<0>(lpData->msg.data()).HasParseError()){
		return FALSE;
	}
	std::string msgType((document["type"].GetString()));
	if ("channel" == msgType){
		std::string channelName = document["data"].GetString();

		LPAG_SIGNAL_NEWCHANNELNAME lpData = new  AG_SIGNAL_NEWCHANNELNAME;
		lpData->account = m_account;
		lpData->channelname = channelName;
		::PostMessage(theApp.GetMainWnd()->m_hWnd, (WM_NewChannelName), (WPARAM)lpData, NULL);
		m_trlQuestion.SetWindowTextW(_T("Click the Join Channel button on the left to send the video to the Audience!"));
	}
	else if ("quiz" == msgType){
		tagQuestionAnswer answerTemp;
		int questionId(document["data"]["id"].GetInt());
		std::string strQuestion(document["data"]["question"].GetString());
		std::vector<std::string > vecQuestionChoose;
		rapidjson::Document::ValueIterator it = document["data"]["options"].Begin();
		for (; document["data"]["options"].End() != it; it++){
			vecQuestionChoose.push_back((*it).GetString());
		}
		answerTemp.questionId = questionId;
		answerTemp.strQuestion = strQuestion;
		answerTemp.vecQuestionAnswers = vecQuestionChoose;

		switchNewQuestion(answerTemp);
	}
	else if ("result" == msgType){

		tagQuestionStatics questionStaticsTemp;
		int nCorrectNum = document["data"]["correct"].GetInt();
		int nTotalNuum = document["data"]["total"].GetInt();
		int sid = document["data"]["sid"].GetInt();
		int nresult = document["data"]["result"].GetInt();

		std::map<std::string, int> mapSpreadTemp;
		std::string strAnswer; int nAnswerNum;
		strAnswer = "A";
		nAnswerNum = document["data"]["spread"]["0"].GetInt();
		mapSpreadTemp.insert(make_pair(strAnswer, nAnswerNum));
		strAnswer = "B";
		nAnswerNum = document["data"]["spread"]["1"].GetInt();
		mapSpreadTemp.insert(make_pair(strAnswer, nAnswerNum));
		strAnswer = "C";
		nAnswerNum = document["data"]["spread"]["2"].GetInt();
		mapSpreadTemp.insert(make_pair(strAnswer, nAnswerNum));
		strAnswer = "D";
		nAnswerNum = document["data"]["spread"]["3"].GetInt();
		mapSpreadTemp.insert(make_pair(strAnswer, nAnswerNum));

		questionStaticsTemp.ncorrectNum = nCorrectNum;
		questionStaticsTemp.nTotal = nTotalNuum;
		questionStaticsTemp.nsid = sid;
		questionStaticsTemp.nresult = nresult;
		questionStaticsTemp.mapSpread = mapSpreadTemp;

		notifyQuestionAnswerStatics(questionStaticsTemp);
	}
	else if ("inviteResponse" == msgType){

		std::string strRemoteMediaUid = document["data"]["mediaUid"].GetString();
		std::string strAccount = document["data"]["uid"].GetString(); 
		bool bAccept = document["data"]["accept"].GetBool();

		LPAG_INVITE_CALLBACKACCEPT lpData = new AG_INVITE_CALLBACKACCEPT;
		lpData->isAccept = bAccept;
		lpData->remoteMediaUid = strRemoteMediaUid;
		lpData->remoteSigAccount = strAccount;

		::PostMessage(theApp.GetMainWnd()->m_hWnd, WM_InviteCallBackAccpet, WPARAM(lpData),NULL);
	}
	else if ("ListOfWinners" == msgType){
		std::vector<tagListOfWinners> vecWinnersTemp;
		int nNum = document["data"]["num"].GetInt();
		rapidjson::Document::ValueIterator it = document["data"]["playerName"].Begin();
		for (int nIndex = 0; nNum > nIndex ; nIndex++){
			tagListOfWinners winnerTemp;
			winnerTemp.nPlayerId = nIndex;
			winnerTemp.strPlayerName = (*it).GetString();
			winnerTemp.fPlayerBonus = 0.0f;
			vecWinnersTemp.push_back(winnerTemp);
			it++;
		}

		notifyRoundListOfWinners(vecWinnersTemp);
	}
	else if ("info" == msgType){

		OutputDebugStringA(lpData->msg.data());
		OutputDebugStringA("\r\n");
		if (document["data"]["err"].IsString()){

			std::string errMsg = document["data"]["err"].GetString();
			//AfxMessageBox(s2cs(errMsg), MB_OK);
			m_ctlNoticeInfo.SetWindowTextW(s2cs("[NoticeInfo]: " + errMsg));
		}
	}
	
	delete lpData; lpData = nullptr;

	return TRUE;
}

HRESULT CDlgAnswer::onMessageChannelReceive(WPARAM wParam, LPARAM lParam)
{
	PAG_SIGNAL_MESSAGECHANNELRECEIVE lpData = (PAG_SIGNAL_MESSAGECHANNELRECEIVE)wParam;
	char logDesc[MAXPATHLEN] = { '\0' };
	sprintf_s(logDesc, "onMessageChannelReceive(%s,%u,%s,%u,%u,%s,%u)", lpData->channelID.data(), lpData->channelID.size(), lpData->account.data(), lpData->account.size(), lpData->uid, lpData->msg.data(), lpData->msg.size());
	//LOG_MSG(logDesc, LogType_CallBack);
	OutputDebugStringA(logDesc);
	OutputDebugStringA("\r\n");

	delete lpData; lpData = nullptr;
	return TRUE;
}

HRESULT CDlgAnswer::onChannelJoined(WPARAM wParam, LPARAM lParam)
{
	PAG_SIGNAL_CHANNELJOIN lpData = (PAG_SIGNAL_CHANNELJOIN)wParam;
	char logDesc[MAXPATHLEN] = { '\0' };
	sprintf_s(logDesc, "onChannelJoined(%s,%u)", lpData->channelID.data(), lpData->channelID.size());
	//LOG_MSG(logDesc, LogType_CallBack);
	OutputDebugStringA(logDesc);
	OutputDebugStringA("\r\n");

	delete lpData; lpData = nullptr;
	return TRUE;
}

HRESULT CDlgAnswer::onChannelJoinFailed(WPARAM wParam, LPARAM lParam)
{
	PAG_SIGNAL_CHANNELJOINFAILED lpData = (PAG_SIGNAL_CHANNELJOINFAILED)wParam;
	char logDesc[MAXPATHLEN] = { '\0' };
	sprintf_s(logDesc, "onChannelJoinFailed(%s,%u,%d)", lpData->channelID.data(), lpData->channelID.size(), lpData->ecode);
	//LOG_MSG(logDesc, LogType_CallBack);
	OutputDebugStringA(logDesc);
	OutputDebugStringA("\r\n");

	CString strChannel = s2cs(lpData->channelID);
	//AfxMessageBox(_T("join Channel Failed"));
	strChannel.ReleaseBuffer();

	delete lpData; lpData = nullptr;
	return TRUE;
}

HRESULT CDlgAnswer::onChannelLeaved(WPARAM wParam, LPARAM lParam)
{
	PAG_SIGNAL_CHANNELLEAVE lpData = (PAG_SIGNAL_CHANNELLEAVE)wParam;
	char logDesc[MAXPATHLEN] = { '\0' };
	sprintf_s(logDesc, "onChannelLeaved(%s,%u,%d)", lpData->channelID.data(), lpData->channelID.size(), lpData->ecode);
	//LOG_MSG(logDesc, LogType_CallBack);
	OutputDebugStringA(logDesc);
	OutputDebugStringA("\r\n");

	delete lpData; lpData = nullptr;
	return TRUE;
}

// void CDlgAnswer::OnDestroy()
// {
// 	
// }

void CDlgAnswer::OnClose()
{
// 	AfxGetUrlService()->GetUrlCallback()->SetMsgReceiver(NULL);
// 	uninitAgoraSignal();
// 	uninitCtrl();
	
}

void CDlgAnswer::switchNewQuestion(const tagQuestionAnswer &newQuestion)
{
	m_btnStartAnswer.ShowWindow(SW_SHOW);
	m_btnStopAnswer.ShowWindow(SW_SHOW);
	m_btnResetQuestion.ShowWindow(SW_SHOW);
	m_ckAnswerA.ShowWindow(SW_SHOW);
	m_ckAnswerB.ShowWindow(SW_SHOW);
	m_ckAnswerC.ShowWindow(SW_SHOW);
	m_ckAnswerD.ShowWindow(SW_SHOW);

	m_nQuestionId = newQuestion.questionId;
	char chQuestionTitle[2048] = { '\0' };
	sprintf_s(chQuestionTitle, "%d: %s", m_nQuestionId + 1, newQuestion.strQuestion.data());
	m_trlQuestion.SetWindowTextW(s2cs(chQuestionTitle));
	int nAnswer = newQuestion.vecQuestionAnswers.size();
	if (1 <= nAnswer)
	m_ckAnswerA.SetWindowTextW(s2cs(newQuestion.vecQuestionAnswers[0]));
	if (2 <= nAnswer)
	m_ckAnswerB.SetWindowTextW(s2cs(newQuestion.vecQuestionAnswers[1])); 
	if (3 <= nAnswer)
	m_ckAnswerC.SetWindowTextW(s2cs(newQuestion.vecQuestionAnswers[2]));
	if (4 <= nAnswer)
	m_ckAnswerD.SetWindowTextW(s2cs(newQuestion.vecQuestionAnswers[3]));
}

void CDlgAnswer::notifyQuestionAnswerStatics(const tagQuestionStatics &QuestionStatics)
{
	CRect rect;
	CWnd* parentWnd = GetParent();
	GetClientRect(&rect);
	parentWnd->ClientToScreen(&rect);
	if (NULL == m_DlgResult.m_hWnd){

		m_DlgResult.Create(CDlgAnswerResultStatics::IDD);
	}
	rect.left = rect.left - 100;
	rect.bottom = rect.bottom - 50;
	//rect.right = rect.right + 100;
	switch (QuestionStatics.nresult)
	{
	case 0:m_ckAnswerA.SetCheck(TRUE); m_ckAnswerB.SetCheck(FALSE); m_ckAnswerC.SetCheck(FALSE); m_ckAnswerD.SetCheck(FALSE);
		break;
	case 1:m_ckAnswerA.SetCheck(FALSE); m_ckAnswerB.SetCheck(TRUE); m_ckAnswerC.SetCheck(FALSE); m_ckAnswerD.SetCheck(FALSE);
		break;
	case 2:m_ckAnswerA.SetCheck(FALSE); m_ckAnswerB.SetCheck(FALSE); m_ckAnswerC.SetCheck(TRUE); m_ckAnswerD.SetCheck(FALSE);
		break;;
	case 3:m_ckAnswerA.SetCheck(FALSE); m_ckAnswerB.SetCheck(FALSE); m_ckAnswerC.SetCheck(FALSE); m_ckAnswerD.SetCheck(TRUE);
		break;
	default:
		break;
	}

	m_DlgResult.ShowWindow(SW_SHOW);
	m_DlgResult.MoveWindow(&rect, TRUE);
	m_DlgResult.setContext(QuestionStatics);
}

void CDlgAnswer::notifyRoundListOfWinners(const std::vector<tagListOfWinners> &vecListOfWinner)
{
	CRect rect;
	CWnd* parentWnd = GetParent();
	GetClientRect(&rect);
	parentWnd->ClientToScreen(&rect);
	if (NULL == m_DlgResult.m_hWnd){

		m_DlgResult.Create(CDlgAnswerResultStatics::IDD);
	}
	rect.right = rect.right + 10;
	rect.bottom = rect.bottom - 50;

	m_DlgResult.ShowWindow(SW_SHOW);
	m_DlgResult.MoveWindow(&rect, TRUE);
	m_DlgResult.setContext(vecListOfWinner);
}

void CDlgAnswer::updateStatusToPublish()
{
	m_trlQuestion.SetWindowTextW(_T("Congratulations, you have joined the room channel, please click the SendQuestion button below to get the Question!"));
	m_btnUpdateQuestion.ShowWindow(SW_SHOW);
	m_btnStopAnswer.ShowWindow(SW_SHOW);
	//m_btnResetQuestion.ShowWindow(SW_SHOW);
}

void CDlgAnswer::leaveChannel()
{
	initCtrl();
}

void CDlgAnswer::joinchannel(){
	onLoginSuccess(NULL,NULL);
}

//invite_request_method http
LRESULT CDlgAnswer::onInputParam(WPARAM wParam, LPARAM lParam)
{
	PAG_INPUTPARAM lpData = (PAG_INPUTPARAM)wParam;
	if (lpData){
		//invite audience/ guest
		char cJsonStr[512] = { '\0' };
		int timeStamp = ::time(NULL);
		sprintf_s(cJsonStr, "{\"type\":\"inviteRequest\",\"data\":{\"uid\":\"%s\"},\"msgid\":%u}", lpData->strParam.data(),GetTickCount());
		OutputDebugStringA(cJsonStr);
		OutputDebugStringA("\r\n");

		std::string gid = gHQConfig.getChannelName();
		AfxGetUrlService()->invite_request(gid, lpData->strParam.data());
// 		if (m_pSignalInstance){
// 
// 			gFileApp.write(cJsonStr);
// 			m_pSignalInstance->sendInstantMsg(m_serverAccount, cJsonStr);
// 		}


		delete lpData; lpData = nullptr;
	}

	return TRUE;
}

LRESULT CDlgAnswer::onSetDataTimeBonus(WPARAM wParam, LPARAM lParam)
{
	LPAG_SetDataTimeBonus lpData = (LPAG_SetDataTimeBonus)wParam;
	if (lpData){

		char cJsonStr[512] = { '\0' };
		sprintf_s(cJsonStr, "{\"data\" : {\"DataTime\" : \"%s\",\"nBouns\" : %d,\"nRoundID\" : %d},	\"type\" : \"SetDataTimeAndBonus\",\"msgid\":%u}", lpData->strDataTime.data(), lpData->nBonus, lpData->nRoundId,GetTickCount());
		OutputDebugStringA(cJsonStr);
		OutputDebugStringA("\n");
		if (m_pSignalInstance){
			m_pSignalInstance->sendInstantMsg(m_serverAccount, cJsonStr);
		}

		delete lpData; lpData = nullptr;
	}

	return true;
}

bool CDlgAnswer::error_info(WPARAM wParam, LPARAM lParam)
{
	bool bSuccess = static_cast<bool>(wParam);
	if (!bSuccess) //错误
	{
		char* szError = (char*)lParam;
		if (szError)
		{
			//AfxMessageBox(s2cs(szError));
			m_ctlNoticeInfo.SetWindowTextW(s2cs(szError));

			delete[] szError;
			szError = NULL;
		}
		return false;
	}
	return true;
}

 bool CDlgAnswer::ParseResponseJson(rapidjson::Document& document, LPARAM lParam, CString function_name)
{
	char* js = (char*)lParam;
	std::string res = js;

	if (js)
	{
		delete[] js;
		js = NULL;
	}

	if (document.Parse<0>(res.data()).HasParseError())
	{
		CString strError;
		strError.Format(_T("%s reponse error: %s"), function_name.GetBuffer(), s2cs(res));
		//AfxMessageBox(strError);
		m_ctlNoticeInfo.SetWindowTextW(strError);
		return false;
	}

	if (!document["err"].IsNull())
	{
		std::string err = document["err"].GetString();
		if (err.length() > 0)
		{
			CString errorinfo;
			if ("room_not_found" == err){
				CString strChannel = s2cs(gHQConfig.getChannelName());
				errorinfo = CHQLANG::getError_Room_Not_Found(strChannel);

			}
			else if ("game_closed_already" == err){
				errorinfo = CHQLANG::getError_Game_Closed_Already();

				m_btnUpdateQuestion.EnableScalar(TRUE);
			}
			else if ("quiz_going_on" == err){
				errorinfo = CHQLANG::getError_Quiz_Going_On();
				
				m_btnStopAnswer.EnableScalar(TRUE);
			}
			else if ("no_more_quiz" == err){
				errorinfo = CHQLANG::getError_No_More_Quiz();

				m_btnResetQuestion.EnableScalar(TRUE);
			}

			m_ctlNoticeInfo.SetWindowTextW(errorinfo);
			//AfxMessageBox(s2cs(err));
			return false;
		}
	}
	return true;
}

LRESULT CDlgAnswer::onRequestChannel(WPARAM wParam, LPARAM lParam)
{
	if (!error_info(wParam, lParam))
		return false;

	rapidjson::Document document;
	if (!ParseResponseJson(document, lParam, _T("requestChannel")))
		return false;

	std::string msgType((document["type"].GetString()));
	if ("channel" == msgType){
		std::string channelName = document["data"].GetString();

		LPAG_SIGNAL_NEWCHANNELNAME lpData = new  AG_SIGNAL_NEWCHANNELNAME;
		lpData->account = m_account;
		lpData->channelname = channelName;
		::PostMessage(theApp.GetMainWnd()->m_hWnd, (WM_NewChannelName), (WPARAM)lpData, NULL);
		m_trlQuestion.SetWindowTextW(_T("Click the Join Channel button on the left to send the video to the Audience!"));
	}

	//stopbtn
	m_btnStopAnswer.EnableWindow(FALSE);
	m_btnUpdateQuestion.EnableScalar(TRUE);
	
	return true;
}

LRESULT CDlgAnswer::onHttpPublish(WPARAM wParam, LPARAM lParam)
{
	m_btnStartAnswer.EnableScalar(TRUE);

	if (!error_info(wParam, lParam))
		return false;

	rapidjson::Document doc;
	if (!ParseResponseJson(doc, lParam, _T("Send Question")))
		return false;
	
	std::string jsData = doc["data"].GetString();
	rapidjson::Document document;
	document.Parse<0>(jsData.data());
	std::string msgType = document["type"].GetString();
	if ("quiz" == msgType){
		tagQuestionAnswer answerTemp;
		int questionId(document["data"]["id"].GetInt());
		std::string strQuestion(document["data"]["question"].GetString());
		std::vector<std::string > vecQuestionChoose;
		rapidjson::Document::ValueIterator it = document["data"]["options"].Begin();
		for (; document["data"]["options"].End() != it; it++){
			vecQuestionChoose.push_back((*it).GetString());
		}
		answerTemp.questionId = questionId;
		answerTemp.strQuestion = strQuestion;
		answerTemp.vecQuestionAnswers = vecQuestionChoose;

		switchNewQuestion(answerTemp);
	}

	return true;
}

LRESULT CDlgAnswer::onHttpStopAns(WPARAM wParam, LPARAM lParam)
{
	if (!error_info(wParam, lParam))
		return false;

	rapidjson::Document document;
	
	if (!ParseResponseJson(document, lParam, _T("Stop Answer")))
		return false;

	std::string msgType((document["type"].GetString()));
	if ("result" == msgType){

		tagQuestionStatics questionStaticsTemp;
		int nCorrectNum = document["data"]["correct"].GetInt();
		int nTotalNuum = document["data"]["total"].GetInt();
		int sid = document["data"]["sid"].GetInt();
		int nresult = document["data"]["result"].GetInt();

		std::map<std::string, int> mapSpreadTemp;
		std::string strAnswer; int nAnswerNum;
		if (!document["data"]["spread"]["0"].IsNull()){

			strAnswer = "A";
			nAnswerNum = document["data"]["spread"]["0"].GetInt();
			mapSpreadTemp.insert(make_pair(strAnswer, nAnswerNum));
		}
		if (!document["data"]["spread"]["1"].IsNull()){

			strAnswer = "B";
			nAnswerNum = document["data"]["spread"]["1"].GetInt();
			mapSpreadTemp.insert(make_pair(strAnswer, nAnswerNum));
		}
		if (!document["data"]["spread"]["2"].IsNull()){

			strAnswer = "C";
			nAnswerNum = document["data"]["spread"]["2"].GetInt();
			mapSpreadTemp.insert(make_pair(strAnswer, nAnswerNum));
		}
		if (!document["data"]["spread"]["3"].IsNull()){

			strAnswer = "D";
			nAnswerNum = document["data"]["spread"]["3"].GetInt();
			mapSpreadTemp.insert(make_pair(strAnswer, nAnswerNum));
		}

		questionStaticsTemp.ncorrectNum = nCorrectNum;
		questionStaticsTemp.nTotal = nTotalNuum;
		questionStaticsTemp.nsid = sid;
		questionStaticsTemp.nresult = nresult;
		questionStaticsTemp.mapSpread = mapSpreadTemp;

		notifyQuestionAnswerStatics(questionStaticsTemp);

		if (document["data"]["winners"].IsArray())
		{
			rapidjson::Value& winners = document["data"]["winners"];
			std::vector<tagListOfWinners> vecWinnersTemp;
			int nIndex = 0;

			for (rapidjson::SizeType i = 0; i < winners.Size(); ++i)
			{
				tagListOfWinners winnerTemp;
				winnerTemp.nPlayerId = nIndex++;
				winnerTemp.fPlayerBonus = 0.0f;				
				rapidjson::Value& players = winners[i];
				if (players.IsString())
				{
					winnerTemp.strPlayerName = players.GetString();
				}
				vecWinnersTemp.push_back(winnerTemp);
			}
			notifyRoundListOfWinners(vecWinnersTemp);
		}
		
	}
	return true;
}

LRESULT CDlgAnswer::onHttpReset(WPARAM wParam, LPARAM lParam)
{
	if (!error_info(wParam, lParam))
		return false;

	rapidjson::Document doc;
	if (!ParseResponseJson(doc, lParam, _T("Reset")))
		return false;

	//TO DO . reset Success
	CString strInfo = CHQLANG::getInfo_ResetInfo();
	AfxMessageBox(strInfo);
	m_btnUpdateQuestion.EnableScalar(TRUE);
	
	return true;
}

LRESULT CDlgAnswer::onHttpInvite(WPARAM wParam, LPARAM lParam)
{
	if (!error_info(wParam, lParam))
		return false;

	rapidjson::Document doc;
	if (!ParseResponseJson(doc, lParam, _T("Reset")))
		return false;

	//invite request success
	invite_status_count = 0;
	
	std::string gid = gHQConfig.getChannelName();
	AfxGetUrlService()->invite_status(gid);
	invite_status_count++;
	SetTimer(invite_status_timer_event_id, invite_status_interval, NULL);
	return true;
}
//{"data":{"responded":false,"uid":"12321312"}}
LRESULT CDlgAnswer::onHttpInviteStatus(WPARAM wParam, LPARAM lParam)
{
	if (!error_info(wParam, lParam))
		return false;

	char* js = (char*)lParam;
	std::string res = js;

	if (js)
	{
		delete[] js;
		js = NULL;
	}

	rapidjson::Document doc;
	if (doc.Parse<0>(res.data()).HasParseError())
	{
		return false;
	}

	if (!doc["err"].IsNull())
	{

		std::string err = doc["err"].GetString();
		if (err.length() > 0)
		{
			AfxMessageBox(s2cs(err));
			return false;
		}
	}
	//如果请求成功
	if (doc["data"]["responded"].GetBool())
	{
		KillTimer(invite_status_timer_event_id);
		if (m_pAgoraHQDlg)
		{
			LPAG_INVITE_CALLBACKACCEPT invite_accept = new AG_INVITE_CALLBACKACCEPT;
			invite_accept->isAccept = doc["data"]["accept"].GetBool();
			invite_accept->remoteMediaUid = doc["data"]["mediaUid"].GetString();
			invite_accept->remoteSigAccount = doc["data"]["uid"].GetString();
			
			::SendMessage(m_pAgoraHQDlg->m_hWnd, WM_URL_MSG(URL_INVITE_STATS_SUCCESS), (WPARAM)invite_accept, 0);
		}
	}
	

	return true;
}
