#pragma once


// CDlgConfig �Ի���

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

	CButton m_btnAppcertificateEnable;
	CComboBox m_comSolutionIndex;
	CComboBox m_comCamera;

};
