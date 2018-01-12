#pragma once


// CDlgAnswerResultStatics 对话框

class CDlgAnswerResultStatics : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgAnswerResultStatics)

public:
	CDlgAnswerResultStatics(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDlgAnswerResultStatics();
	virtual BOOL OnInitDialog();
	void setContext(const tagQuestionStatics &questionStatics);

// 对话框数据
	enum { IDD = IDD_DIALOG_ANSWER_RESULT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()

private:
	CListCtrl m_ltrResult;
public:
	afx_msg void OnBnClickedButtonSave();
	afx_msg void OnBnClickedButtonCancle();
};
