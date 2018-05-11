#pragma once
#include "afxwin.h"


// CDShowCameraDlg dialog
#include "BasicProperties.h"
class CDShowCameraDlg : public CBasicProperties
{
	DECLARE_DYNAMIC(CDShowCameraDlg)

public:
	CDShowCameraDlg(OBSSource _source, CWnd* pParent = NULL);   // standard constructor
	virtual ~CDShowCameraDlg();

// Dialog Data
	enum { IDD = IDD_DSHOW_INPUT_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
	void AddProperty(obs_property_t *property);
	CAgoraHQDlg* m_pVideoDlg;
	std::vector<std::string> m_cameraIds;
	std::string propery_name = "video_device_id";
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedOk();

	void SetVideoDialog(CAgoraHQDlg* pDlg);
	afx_msg void OnSelchangeCombo1();
	CComboBox m_cmbCameraDevice;
};
