#pragma once

#pragma once
// CFFmpegSourceDlg dialog
#include "BasicProperties.h"
#include "afxwin.h"
class CFFmpegSourceDlg : public CBasicProperties//CDialogEx
{
	DECLARE_DYNAMIC(CFFmpegSourceDlg)

public:
	CFFmpegSourceDlg(OBSSource _source, CWnd* pParent = NULL);   // standard constructor
	virtual ~CFFmpegSourceDlg();

// Dialog Data
	enum { IDD = IDD_DIALOG_FFMPEG_SOURCE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedButtonMoviePath();
private:
	void AddProperty(obs_property_t *property);
	CString m_strMoviePath = _T("");
	CAgoraHQDlg* m_pVideoDlg;
public:
	void SetVideoDialog(CAgoraHQDlg* pDlg){ m_pVideoDlg = pDlg; }
	virtual BOOL OnInitDialog();
	CEdit m_edtMovie;
	CButton m_btnMovie;
};
