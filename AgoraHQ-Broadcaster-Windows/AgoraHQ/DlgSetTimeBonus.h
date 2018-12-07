#pragma once


// CDlgSetTimeBonus 对话框

class CDlgSetTimeBonus : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgSetTimeBonus)

public:
	CDlgSetTimeBonus(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDlgSetTimeBonus();

// 对话框数据
	enum { IDD = IDD_DIALOG_SETTIMEBONUS };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
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
