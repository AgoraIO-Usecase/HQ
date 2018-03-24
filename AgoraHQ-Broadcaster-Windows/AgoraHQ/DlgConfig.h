#pragma once


// CDlgConfig �Ի���

#include "AgoraCameraManager.h"
class CDlgConfig : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgConfig)

public:
	CDlgConfig(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CDlgConfig();
	int getVideoIndex();
	void saveCameraID();

// �Ի�������
	enum { IDD = IDD_DIALOG_CONFIG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��
	virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonConfigSave();
	afx_msg void OnBnClickedButtonConfigCancle();

protected:
	void initData();
	void initCtrl();
	void uninitCtrl();

private:
	CEdit m_edAppId;
	CEdit m_edAppcertificatId;
	CEdit m_edMediaUid;
	CEdit m_edChannelName;
	CEdit m_edLanguage;
	CComboBox m_comLanguage;
	CComboBox m_comServerAccount;

	CButton m_btnSatrtPreview;
	CButton m_btnAppcertificateEnable;
	CComboBox m_comSolutionIndex;
	CComboBox m_comCamera;
	CStatic m_trlTestVideo;
	CButton m_btnEnableEncrypt;

	CAgoraCameraManager CameraManager; 
	IRtcEngine* pRtcEngine;

public:
	afx_msg void OnBnClickedButtonVideotest();
	afx_msg void OnCbnSelchangeComboCamera();
};
