#pragma once


// CDlgColorKey dialog

class CDlgColorKey : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgColorKey)

public:
	CDlgColorKey(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgColorKey();

// Dialog Data
	enum { IDD = IDD_DIALOG_COLOR_KEY };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedButtonSelectColor();
};
