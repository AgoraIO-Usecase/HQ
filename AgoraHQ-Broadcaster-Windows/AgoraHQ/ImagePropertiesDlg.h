#pragma once

#include "BasicProperties.h"
#include "afxwin.h"
// CImagePropertiesDlg dialog
class CAgoraHQDlg;
class CImagePropertiesDlg : public CBasicProperties//public CDialogEx
{
	DECLARE_DYNAMIC(CImagePropertiesDlg)

public:
	CImagePropertiesDlg(OBSSource _source,CWnd* pParent = NULL);   // standard constructor
	virtual ~CImagePropertiesDlg();

// Dialog Data
	enum { IDD = IDD_DIALOG_ADD_IMAGE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedButtonImagePath();
	void AddProperty(obs_property_t *property);
	void SetVideoDialog(CAgoraHQDlg* pDlg);
private:
	CAgoraHQDlg* m_pVideoDlg;
public:
	CEdit m_edtImagePath;
	CString m_strImagePath = _T("");
	CButton m_btnImagePath;
};
