// DlgAnswer.cpp : 实现文件
//

#include "stdafx.h"
#include "AgoraHQ.h"
#include "DlgAnswer.h"
#include "afxdialogex.h"
#include "SingleWrap.h"
#include "SignalInstance.h"
#include "generatorSignalToken.h"
#include "commonFun.h"

// CDlgAnswer 对话框

IMPLEMENT_DYNAMIC(CDlgAnswer, CDialogEx)

CDlgAnswer::CDlgAnswer(CWnd* pParent /*=NULL*/)
	: CDialogEx(CDlgAnswer::IDD, pParent),
	m_pSignalInstance(nullptr),
	m_pSignalCallBack(nullptr),
	m_nQuestionId(-1),
	m_pAgEngineEventHandle(nullptr)
{

}

CDlgAnswer::~CDlgAnswer()
{
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
	ON_BN_CLICKED(IDC_CHECK_A, &CDlgAnswer::OnBnClickedCheckA)
	ON_BN_CLICKED(IDC_CHECK_B, &CDlgAnswer::OnBnClickedCheckB)
	ON_BN_CLICKED(IDC_CHECK_C, &CDlgAnswer::OnBnClickedCheckC)
	ON_BN_CLICKED(IDC_CHECK_D, &CDlgAnswer::OnBnClickedCheckD)
	ON_BN_CLICKED(IDC_BUTTON_GetQuestion, &CDlgAnswer::OnBnClickedButtonGetquestion)
	ON_BN_CLICKED(IDC_BUTTON_STARTANSWER, &CDlgAnswer::OnBnClickedButtonStartMark)
	ON_BN_CLICKED(IDC_BUTTON_STOPANSWER, &CDlgAnswer::OnBnClickedButtonStopanswer)
	ON_BN_CLICKED(IDC_BUTTON_SETBONUSES, &CDlgAnswer::OnBnClickedButtonSetbonuses)
	ON_MESSAGE(WM_LoginSuccess,onLoginSuccess)
	ON_MESSAGE(WM_LogOut,onLogout)
	ON_MESSAGE(WM_LoginFailed,onLogFailed)
	ON_MESSAGE(WM_Error,onError)
	ON_MESSAGE(WM_Log,onLog)
	ON_MESSAGE(WM_QueryUserStatusResult, onQueryUserStatusResult)
	ON_MESSAGE(WM_MessageSendSuccess, onMessageSendSuccess)
	ON_MESSAGE(WM_MessageSendError, onMessageSendError)
	ON_MESSAGE(WM_MessageInstantReceive, onMessageInstantReceive)
	ON_BN_CLICKED(IDC_BUTTON_RESET, &CDlgAnswer::OnBnClickedButtonReset)
	ON_MESSAGE(WM_UpdateInputParam, onInputParam)
END_MESSAGE_MAP()


// CDlgAnswer 消息处理程序
BOOL CDlgAnswer::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	
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

	m_pAgEngineEventHandle = CAgoraObject::getEngineEventHandle();
}

void CDlgAnswer::uninitCtrl()
{
	m_pAgEngineEventHandle = nullptr;
}

void CDlgAnswer::initAgoraSignal()
{
	m_appId = gHQConfig.getAppId();
	m_appCertificatId = gHQConfig.getAppCertificateId();

	m_pSignalInstance = CAgoraSignalInstance::getSignalInstance(m_appId);
	ASSERT(m_pSignalInstance);
	m_pSignalInstance->setLoginWnd(m_hWnd);
	m_pSignalInstance->setAppCertificateId(m_appCertificatId);
	
	m_pSignalCallBack = new CSingleCallBack(m_hWnd);
	m_pSignalInstance->getAgoraAPI()->callbackSet(m_pSignalCallBack);

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
}

void CDlgAnswer::uninitAgoraSignal()
{
	m_pSignalInstance->LeaveChannel();
	m_pSignalInstance->LogOut();
	m_pSignalInstance->getAgoraAPI()->callbackSet(nullptr);
	delete m_pSignalInstance;
	m_pSignalInstance = nullptr;
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

void CDlgAnswer::OnBnClickedButtonGetquestion()
{
	// TODO:  在此添加控件通知处理程序代码
	char cJsonStr[512] = { '\0' };
	sprintf_s(cJsonStr, "{\"type\": \"publish\"}");
	OutputDebugStringA(cJsonStr);
	OutputDebugStringA("\r\n");
	m_pSignalInstance->sendInstantMsg(m_serverAccount, cJsonStr);

	m_ckAnswerA.SetCheck(FALSE);
	m_ckAnswerB.SetCheck(FALSE);
	m_ckAnswerC.SetCheck(FALSE);
	m_ckAnswerD.SetCheck(FALSE);
}

void CDlgAnswer::OnBnClickedButtonStartMark()
{
	// TODO:  在此添加控件通知处理程序代码
	char cJsonStr[512] = { '\0' };
	int timeStamp = ::time(NULL);
	sprintf_s(cJsonStr, "{\"type\":\"setSEI\",\"data\" :	{\"questionId\":%d,\"timeStamp\":%d}}", m_nQuestionId, timeStamp);
	OutputDebugStringA(cJsonStr);
	OutputDebugStringA("\r\n");
	if (m_pAgEngineEventHandle){
		m_pAgEngineEventHandle->setSEI(cJsonStr);
	}

	// TO DO 开始标记本地画面.
}


void CDlgAnswer::OnBnClickedButtonStopanswer()
{
	// TODO:  在此添加控件通知处理程序代码
	char cJsonStr[512] = { '\0' };
	sprintf_s(cJsonStr, "{\"type\":\"stopAnswer\"}");
	OutputDebugStringA(cJsonStr);
	OutputDebugStringA("\r\n");
	m_pSignalInstance->sendInstantMsg(m_serverAccount, cJsonStr);
}

void CDlgAnswer::OnBnClickedButtonReset()
{
	// TODO:  在此添加控件通知处理程序代码
	char cJsonStr[512] = { '\0' };
	sprintf_s(cJsonStr, "{\"type\":\"reset\"}");
	OutputDebugStringA(cJsonStr);
	OutputDebugStringA("\r\n");
	m_pSignalInstance->sendInstantMsg(m_serverAccount, cJsonStr);
}

void CDlgAnswer::OnBnClickedButtonSetbonuses()
{
	// TODO:  在此添加控件通知处理程序代码
}


HRESULT CDlgAnswer::onLoginSuccess(WPARAM wParam, LPARAM lParam)
{
	PAG_SIGNAL_LOGINSUCCESS lpData = (PAG_SIGNAL_LOGINSUCCESS)wParam;

	delete lpData; lpData = nullptr;

	m_btnUpdateQuestion.EnableWindow(TRUE);
	m_btnStartAnswer.EnableWindow(TRUE);
	m_btnStopAnswer.EnableWindow(TRUE);

	OutputDebugStringA(__FUNCTION__);
	OutputDebugStringA("\r\n");

	std::string curLanguage = gHQConfig.getLanguage();
	if ("" == curLanguage){
		curLanguage = "0";
		gHQConfig.setLanguage(curLanguage);
	}

	char cJsonStr[512] = { '\0' };
	sprintf_s(cJsonStr, "{\"type\":\"RequestChannelName\",\"QuestionLanguage\":\"%s\"}",curLanguage.data());
	OutputDebugStringA(cJsonStr);
	OutputDebugStringA("\r\n");
	m_pSignalInstance->sendInstantMsg(m_serverAccount, cJsonStr);

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
	delete lpData; lpData = nullptr;

	return TRUE;
	OutputDebugStringA(__FUNCTION__);
	OutputDebugStringA("\r\n");
}

LRESULT CDlgAnswer::onError(WPARAM wParam, LPARAM lParam)
{
	PAG_SIGNAL_ERROR lpData = (PAG_SIGNAL_ERROR)wParam;
	delete lpData; lpData = nullptr;

	return TRUE;
	OutputDebugStringA(__FUNCTION__);
	OutputDebugStringA("\r\n");
}

LRESULT CDlgAnswer::onLog(WPARAM wParam, LPARAM lParam)
{
	PAG_SIGNAL_LOG lpData = (PAG_SIGNAL_LOG)wParam;
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

		bool bAccept = document["data"]["accept"].GetBool();
		::PostMessage(theApp.GetMainWnd()->m_hWnd, WM_InviteCallBackAccpet, WPARAM(bAccept),NULL);
	}
	else if ("ListOfWinners" == msgType){
		std::vector<tagListOfWinners> vecWinnersTemp;
		int nNum = document["data"]["num"].GetInt();
		rapidjson::Document::ValueIterator it = document["data"]["playerName"].Begin();
		for (int nIndex = 0; nNum > nIndex; nIndex++){
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
			m_ctlNoticeInfo.SetWindowTextW(s2cs("NoticeInfo" + errMsg));
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

void CDlgAnswer::OnClose()
{
	uninitAgoraSignal();
	uninitCtrl();
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
	m_ckAnswerA.SetWindowTextW(s2cs(newQuestion.vecQuestionAnswers[0]));
	m_ckAnswerB.SetWindowTextW(s2cs(newQuestion.vecQuestionAnswers[1])); 
	m_ckAnswerC.SetWindowTextW(s2cs(newQuestion.vecQuestionAnswers[2]));
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
}

LRESULT CDlgAnswer::onInputParam(WPARAM wParam, LPARAM lParam)
{
	PAG_INPUTPARAM lpData = (PAG_INPUTPARAM)wParam;
	if (lpData){
		//invite audience/ guest
		char cJsonStr[512] = { '\0' };
		int timeStamp = ::time(NULL);
		sprintf_s(cJsonStr, "{\"type\":\"inviteRequest\",\"data\":{\"uid\":\"%s\"}}", lpData->strParam.data());
		OutputDebugStringA(cJsonStr);
		OutputDebugStringA("\r\n");
		if (m_pSignalInstance){
			m_pSignalInstance->sendInstantMsg(m_serverAccount, cJsonStr);
		}

		delete lpData; lpData = nullptr;
	}

	return TRUE;
}