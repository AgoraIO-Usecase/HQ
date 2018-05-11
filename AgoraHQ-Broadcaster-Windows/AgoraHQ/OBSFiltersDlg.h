#pragma once
#include "afxcmn.h"
#include "DisplaySourceWnd.h"
#include "afxwin.h"

// COBSFiltersDlg dialog
class CDlgColorKey;
class COBSFiltersDlg : public CDialogEx
{
	DECLARE_DYNAMIC(COBSFiltersDlg)

public:
	COBSFiltersDlg(OBSSource source, CWnd* pParent = NULL);   // standard constructor
	virtual ~COBSFiltersDlg();

// Dialog Data
	enum { IDD = IDD_DIALOG_OBS_FILTERS };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

private:
	virtual BOOL OnWndMsg(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pResult);

	CAgoraHQDlg* m_pVideoDlg;
	CString filter_tile = _T("filter Name");
	CString filter_info = _T("Please input the name of filter");
	CDisplaySourceWnd* preview = NULL;
	CRect rcPreview;

	CDlgColorKey* m_pDlgColorKey;
public:
	virtual BOOL OnInitDialog();

	OBSSource _source;
	CTreeCtrl m_effectFilterTree;
	afx_msg void OnBnClickedButtonAddFilter();
	afx_msg void OnBnClickedButtonDelFilter();

	static void DrawPreview(void *data, uint32_t cx, uint32_t cy);
	CStatic m_staPreview;
	afx_msg void OnBnClickedOk();
	
	CStatic m_staColorKey;
};
