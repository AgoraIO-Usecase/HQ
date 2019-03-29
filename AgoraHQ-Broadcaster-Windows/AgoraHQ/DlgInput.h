#pragma once


// CDlgInput �Ի���

class CDlgInput : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgInput)

public:
	CDlgInput(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CDlgInput();

	void showWindow(eTagMsgtype type);
// �Ի�������
	enum { IDD = IDD_DIALOG_INPUT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��
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
