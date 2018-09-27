#pragma once


// CDlgAnswer 对话框
class CAgoraSignalInstance;
class CSingleCallBack;
class CAgoraHQDlg;
#include "AGButton.h"
#include "commonFun.h"
#include <map>

#include "DlgAnswerResultStatics.h"
class CDlgAnswer : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgAnswer)

public:
	CDlgAnswer(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDlgAnswer();
	void updateStatusToPublish();

	void leaveChannel();
	void joinchannel();

// 对话框数据
	enum { IDD = IDD_DIALOG_ANSWER };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	virtual BOOL OnInitDialog();
	afx_msg void OnClose();
	//afx_msg void OnDestroy();

	DECLARE_MESSAGE_MAP()

	afx_msg void OnBnClickedCheckA();
	afx_msg void OnBnClickedCheckB();
	afx_msg void OnBnClickedCheckC();
	afx_msg void OnBnClickedCheckD();
	afx_msg void OnBnClickedButtonGetquestion();
	afx_msg void OnBnClickedButtonStartMark();
	afx_msg void OnBnClickedButtonStopanswer();
	afx_msg void OnBnClickedButtonSetbonuses();
	afx_msg void OnBnClickedButtonReset();
	afx_msg void OnTimer(UINT_PTR nIDEvent);

	HRESULT onLoginSuccess(WPARAM wParam, LPARAM lParam);
	HRESULT onLogout(WPARAM wParam, LPARAM lParam);
	HRESULT onLogFailed(WPARAM wParam, LPARAM lParam);
	LRESULT onError(WPARAM wParam, LPARAM lParam);
	LRESULT onLog(WPARAM wParam, LPARAM lParam);

	HRESULT onQueryUserStatusResult(WPARAM wParam, LPARAM lParam);
	HRESULT onMessageSendSuccess(WPARAM wParam, LPARAM lParam);
	HRESULT onMessageSendError(WPARAM wParam, LPARAM lParam);
	HRESULT onMessageInstantReceive(WPARAM wParam, LPARAM lParam);
	HRESULT onMessageChannelReceive(WPARAM wParam, LPARAM lParam);

	HRESULT onChannelJoined(WPARAM wParam, LPARAM lParam);
	HRESULT onChannelJoinFailed(WPARAM wParam, LPARAM lParam);
	HRESULT onChannelLeaved(WPARAM wParam, LPARAM lParam);

	LRESULT onInputParam(WPARAM wParam, LPARAM lParam);
	LRESULT onSetDataTimeBonus(WPARAM wParam, LPARAM lParam);
	LRESULT onHttpPublish(WPARAM wParam, LPARAM lParam);
	LRESULT onRequestChannel(WPARAM wParam, LPARAM lParam);
	LRESULT onHttpStopAns(WPARAM wParam, LPARAM lParam);
	LRESULT onHttpReset(WPARAM wParam, LPARAM lParam);
	LRESULT onHttpInvite(WPARAM wParam, LPARAM lParam);
	LRESULT onHttpInviteStatus(WPARAM wParam, LPARAM lParam);
protected:
	void initCtrl();
	void uninitCtrl();

	void initAgoraSignal();
	void uninitAgoraSignal();

	void DrawClient();

	int getChannelName();
	void switchNewQuestion(const tagQuestionAnswer &newQuestion);
	void notifyQuestionAnswerStatics(const tagQuestionStatics &QuestionStatics);
	void notifyRoundListOfWinners(const std::vector<tagListOfWinners> &vecListOfWinner);

	bool error_info(WPARAM wParam, LPARAM lParam);
	bool ParseResponseJson(rapidjson::Document& document, LPARAM lParam, CString function_name);

private:
	CString m_sQuestion;

private:
	std::string m_appId;
	std::string m_appCertificatId;
	std::string m_account;
	std::string m_serverAccount;
	bool m_isLogin;

	CAgoraSignalInstance* m_pSignalInstance;
	CSingleCallBack* m_pSignalCallBack;

	CStatic m_trlQuestion;
	CButton m_ckAnswerA;
	CButton m_ckAnswerB;
	CButton m_ckAnswerC;
	CButton m_ckAnswerD;

	CAGButton m_btnUpdateQuestion;
	CAGButton m_btnStartAnswer;
	CAGButton m_btnStopAnswer;
	CAGButton m_btnResetQuestion;
	CStatic m_ctlNoticeInfo;
	
	CDlgAnswerResultStatics m_DlgResult;
	int m_nQuestionId;
	CAGEngineEventHandler* m_pAgEngineEventHandle;

	CFileIO m_fileSigLog;


	int invite_status_count = 0;
	int invite_status_max = 30;
	int invite_status_interval = 1000;//1s 
	int invite_status_timer_event_id = 1001;
	std::map<std::string, CString> m_mapChn;
	//std::map<std::string, >
public:
	CAgoraHQDlg* m_pAgoraHQDlg;
};
