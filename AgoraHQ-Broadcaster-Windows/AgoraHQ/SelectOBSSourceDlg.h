#pragma once
#include "afxwin.h"

enum NAME_TYPE
{
	NAME_TYPE_SOURCE = 0,
	NAME_TYPE_FILTER,
};
// CSelectOBSSourceDlg dialog

class CSelectOBSSourceDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CSelectOBSSourceDlg)

public:
	CSelectOBSSourceDlg(const char* id, NAME_TYPE type, CString title, CString info, CWnd* pParent = NULL);   // standard constructor
	virtual ~CSelectOBSSourceDlg();

// Dialog Data
	enum { IDD = IDD_SELECT_OBS_SOURCE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CEdit m_edtOBSSourceName;
	afx_msg void OnBnClickedOk();
	//OBS for ExtCapture
	static bool EnumSources(void *data, obs_source_t *source);
private:
	std::string source_id;
	CString source_name = _T("");
	CString filter_name = _T("");
	NAME_TYPE name_type;
public:
	OBSSource newSource;
	virtual BOOL OnInitDialog();

	std::string utf8_source_name;

	CString m_title = _T("");
	CString m_info = _T("");
	CStatic m_staInfo;
};
