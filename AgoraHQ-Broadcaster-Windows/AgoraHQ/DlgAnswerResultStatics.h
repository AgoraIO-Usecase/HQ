#pragma once


// CDlgAnswerResultStatics �Ի���


class CDlgAnswerResultStatics : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgAnswerResultStatics)

public:
	CDlgAnswerResultStatics(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CDlgAnswerResultStatics();
	virtual BOOL OnInitDialog();
	void setContext(const tagQuestionStatics &questionStatics);
	void setContext(const std::vector<tagListOfWinners> &vecListOfWinners);

// �Ի�������
	enum { IDD = IDD_DIALOG_ANSWER_RESULT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��
	afx_msg void OnBnClickedButtonSave();
	afx_msg void OnBnClickedButtonCancle();

	DECLARE_MESSAGE_MAP()

private:
	CListCtrl m_ltrResult;
	CListCtrl m_ltrListofWinners;
};
