#pragma once


// CDlgSetTimeBonus �Ի���

class CDlgSetTimeBonus : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgSetTimeBonus)

public:
	CDlgSetTimeBonus(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CDlgSetTimeBonus();

// �Ի�������
	enum { IDD = IDD_DIALOG_SETTIMEBONUS };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��
	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	

	DECLARE_MESSAGE_MAP()

	afx_msg void OnPaint();

private:
	void initCtrl();
	void uninitCtrl();

private:

	CDateTimeCtrl m_DateItmeCtrl_Time;
	CEdit m_edBonus;
	CEdit m_edRoundId;
};
