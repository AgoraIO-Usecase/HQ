#pragma once


// CDlgAnswer 对话框
class CAgoraSignalInstance;
class CSingleCallBack;
#include "AGButton.h"

#include "DlgAnswerResultStatics.h"
class CDlgAnswer : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgAnswer)

public:
	CDlgAnswer(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDlgAnswer();
	void updateStatusToPublish();

// 对话框数据
	enum { IDD = IDD_DIALOG_ANSWER };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	virtual BOOL OnInitDialog();
	afx_msg void OnClose();

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
	CButton m_btnResetQuestion;
	CStatic m_ctlNoticeInfo;
	
	CDlgAnswerResultStatics m_DlgResult;
	int m_nQuestionId;
	CAGEngineEventHandler* m_pAgEngineEventHandle;
};
