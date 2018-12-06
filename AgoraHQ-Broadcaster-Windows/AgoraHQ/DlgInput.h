#pragma once


// CDlgInput 对话框

class CDlgInput : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgInput)

public:
	CDlgInput(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDlgInput();

	void showWindow(eTagMsgtype type);
// 对话框数据
	enum { IDD = IDD_DIALOG_INPUT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);

	DECLARE_MESSAGE_MAP()

private:
	eTagMsgtype m_type;
	
	CButton m_btckEnableVideo;
	CButton m_btckEnableAudio;
	CEdit m_edTimeOut;
	CEdit m_edInviteAccount;
};
