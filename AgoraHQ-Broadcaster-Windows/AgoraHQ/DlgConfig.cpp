// DlgConfig.cpp : 实现文件
//

#include "stdafx.h"
#include "AgoraHQ.h"
#include "DlgConfig.h"
#include "afxdialogex.h"
#include "commonFun.h"


// CDlgConfig 对话框

IMPLEMENT_DYNAMIC(CDlgConfig, CDialogEx)

CDlgConfig::CDlgConfig(CWnd* pParent /*=NULL*/)
	: CDialogEx(CDlgConfig::IDD, pParent),
	pRtcEngine(NULL)
{

}

CDlgConfig::~CDlgConfig()
{
	CameraManager.TestCameraDevice(nullptr, FALSE);
	CameraManager.Close();
}

void CDlgConfig::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_APPID, m_edAppId);
	DDX_Control(pDX, IDC_CHECK_APPCERT, m_btnAppcertificateEnable);
	DDX_Control(pDX, IDC_EDIT_APPCERTIFICATID, m_edAppcertificatId);
	DDX_Control(pDX, IDC_COMBO_VideoSolution, m_comSolutionIndex);
	DDX_Control(pDX, IDC_COMBO_CAMERA, m_comCamera);
	DDX_Control(pDX, IDC_EDIT_MediaUID, m_edMediaUid);
	DDX_Control(pDX, IDC_EDIT_ChannelName, m_edChannelName);
	DDX_Control(pDX, IDC_BUTTON_VIDEOTEST, m_btnSatrtPreview);
	DDX_Control(pDX, IDC_STATIC_VIDEOTEST, m_trlTestVideo);
	DDX_Control(pDX, IDC_COMBO_LANGUAGE, m_comLanguage);
	DDX_Control(pDX, IDC_CHECK_EnableEncrypt, m_btnEnableEncrypt);
}


BEGIN_MESSAGE_MAP(CDlgConfig, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON_CONFIG_SAVE, &CDlgConfig::OnBnClickedButtonConfigSave)
	ON_BN_CLICKED(IDC_BUTTON_CONFIG_CANCLE, &CDlgConfig::OnBnClickedButtonConfigCancle)
	ON_BN_CLICKED(IDC_BUTTON_VIDEOTEST, &CDlgConfig::OnBnClickedButtonVideotest)
	ON_CBN_SELCHANGE(IDC_COMBO_CAMERA, &CDlgConfig::OnCbnSelchangeComboCamera)
END_MESSAGE_MAP()


// CDlgConfig 消息处理程序
BOOL CDlgConfig::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	//TO DO
	initData();
	initCtrl();

	return TRUE;
}

int CDlgConfig::getVideoIndex()
{
	int nVideoProfileIndex = m_comSolutionIndex.GetCurSel();
	if (nVideoProfileIndex == CB_ERR){
		return VIDEO_PROFILE_TYPE::VIDEO_PROFILE_DEFAULT;
	}
	return m_comSolutionIndex.GetItemData(nVideoProfileIndex);
}

void CDlgConfig::saveCameraID()
{
	//OnBnClickedButtonConfigSave();
}

void CDlgConfig::OnBnClickedButtonConfigSave()
{
	// TODO:  在此添加控件通知处理程序代码
	CString strParam;
	m_edAppId.GetWindowTextW(strParam);
	gHQConfig.setAppId(cs2s(strParam));
	m_edAppcertificatId.GetWindowTextW(strParam);
	gHQConfig.setAppCertificateId(cs2s(strParam));
	m_edMediaUid.GetWindowTextW(strParam);
	gHQConfig.setLoginUid(cs2s(strParam));
	std::string strCheck = int2str(m_btnAppcertificateEnable.GetCheck());
	gHQConfig.setAppCertEnable(strCheck);
	int nVideoIndexCurSel = m_comSolutionIndex.GetCurSel();
	gHQConfig.setVideoSolutinIndex(int2str(nVideoIndexCurSel));
	m_edChannelName.GetWindowTextW(strParam);
	gHQConfig.setChannelName(cs2s(strParam));
	gHQConfig.setSignalAccount(cs2s(strParam));
	int nLanguage = m_comLanguage.GetCurSel();
	gHQConfig.setLanguage(int2str(nLanguage));
	int nRegion = m_comRegion.GetCurSel();
	gHQConfig.setRegion(int2str(nRegion));
	bool bEnableEncrypt = m_btnEnableEncrypt.GetCheck();
	gHQConfig.setEnableEncrypt(int2str(bEnableEncrypt));
	
	int nCurSel = m_comCamera.GetCurSel();
	CAgoraCameraManager CameraManager;
	IRtcEngine* pRtcEngine = CAgoraObject::GetEngine();
	if (pRtcEngine && CB_ERR != nCurSel){

		CameraManager.Create(pRtcEngine);

		CString strDeviceName; CString strDeviceId;
		CameraManager.GetDevice(nCurSel, strDeviceName, strDeviceId);
		gHQConfig.setCameraDeviceName(cs2s(strDeviceName));
		gHQConfig.setCameraDeviceID(cs2s(strDeviceId));
		std::string strDeviceIdUtf8 = gbk2utf8(cs2s(strDeviceId));
		CameraManager.SetCurDevice(s2cs(strDeviceIdUtf8));
	}

	if (IDOK == AfxMessageBox(_T("是否重启HQ导播程序（Y-是；N-否）"), MB_OKCANCEL)){

		PostQuitMessage(0);
		WinExec("restart.bat", SW_HIDE);
	}

//	CameraManager.TestCameraDevice(nullptr, FALSE);

	CDlgConfig::OnOK();
}


void CDlgConfig::OnBnClickedButtonConfigCancle()
{
	// TODO:  在此添加控件通知处理程序代码
	CameraManager.TestCameraDevice(nullptr, FALSE);
	CDlgConfig::OnCancel();
}

void CDlgConfig::initData()
{
	LPTSTR m_szProfileDes[32];
	int m_nProfileValue[32];
	m_szProfileDes[0] = _T("160x120 15fps 65kbps");
	m_nProfileValue[0] = 0;
	m_szProfileDes[1] = _T("120x120 15fps 50kbps");
	m_nProfileValue[1] = 2;

	m_szProfileDes[2] = _T("320x180 15fps 140kbps");
	m_nProfileValue[2] = 10;
	m_szProfileDes[3] = _T("180x180 15fps 100kbps");
	m_nProfileValue[3] = 12;
	m_szProfileDes[4] = _T("240x180 15fps 120kbps");
	m_nProfileValue[4] = 13;

	m_szProfileDes[5] = _T("320x240 15fps 200kbps");
	m_nProfileValue[5] = 20;
	m_szProfileDes[6] = _T("240x240 15fps 140kbps");
	m_nProfileValue[6] = 22;
	m_szProfileDes[7] = _T("424x240 15fps 220kbps");
	m_nProfileValue[7] = 23;

	m_szProfileDes[8] = _T("640x360 15fps 400kbps");
	m_nProfileValue[8] = 30;
	m_szProfileDes[9] = _T("360x360 15fps 260kbps");
	m_nProfileValue[9] = 32;
	m_szProfileDes[10] = _T("640x360 30fps 600kbps");
	m_nProfileValue[10] = 33;
	m_szProfileDes[11] = _T("360x360 30fps 400kbps");
	m_nProfileValue[11] = 35;
	m_szProfileDes[12] = _T("480x360 15fps 320kbps");
	m_nProfileValue[12] = 36;
	m_szProfileDes[13] = _T("480x360 30fps 490kbps");
	m_nProfileValue[13] = 37;
	m_szProfileDes[14] = _T("640x360 15fps 800kbps");
	m_nProfileValue[14] = 38;

	m_szProfileDes[15] = _T("640x480 15fps 500kbps");
	m_nProfileValue[15] = 40;
	m_szProfileDes[16] = _T("480x480 15fps 400kbps");
	m_nProfileValue[16] = 42;
	m_szProfileDes[17] = _T("640x480 30fps 750kbps");
	m_nProfileValue[17] = 43;
	m_szProfileDes[18] = _T("480x480 30fps 600kbps");
	m_nProfileValue[18] = 44;
	m_szProfileDes[19] = _T("848x480 15fps 610kbps");
	m_nProfileValue[19] = 47;
	m_szProfileDes[20] = _T("848x480 30fps 930kbps");
	m_nProfileValue[20] = 48;

	m_szProfileDes[21] = _T("1280x720 15fps 1130kbps");
	m_nProfileValue[21] = 50;
	m_szProfileDes[22] = _T("1280x720 30fps 1710kbps");
	m_nProfileValue[22] = 52;
	m_szProfileDes[23] = _T("960x720 15fps 910kbps");
	m_nProfileValue[23] = 54;
	m_szProfileDes[24] = _T("960x720 30fps 1380kbps");
	m_nProfileValue[24] = 55;

	m_szProfileDes[25] = _T("1920x1080 15fps 2080kbps");
	m_nProfileValue[25] = 60;
	m_szProfileDes[26] = _T("1920x1080 30fps 3150kbps");
	m_nProfileValue[26] = 62;
	m_szProfileDes[27] = _T("1920x1080 60fps 4780kbps");
	m_nProfileValue[27] = 64;
	m_szProfileDes[28] = _T("2560x1440 30fps 4850kbps");
	m_nProfileValue[28] = 66;
	m_szProfileDes[29] = _T("3560x1440 60fps 7350kbps");
	m_nProfileValue[29] = 67;

	m_szProfileDes[30] = _T("3840x2160 30fps 8190kbps");
	m_nProfileValue[30] = 70;
	m_szProfileDes[31] = _T("3840x2160 60fps 13500kbps");
	m_nProfileValue[31] = 72;

	for (int nIndex = 0; nIndex < 32; nIndex++){

		m_comSolutionIndex.AddString(m_szProfileDes[nIndex]);
		m_comSolutionIndex.SetItemData(nIndex,(DWORD_PTR)m_nProfileValue[nIndex]);
	}
}

void CDlgConfig::initCtrl()
{
	std::string strAppId = gHQConfig.getAppId();
	m_edAppId.SetWindowTextW(s2cs(strAppId));
	std::string strAppCertificatId = gHQConfig.getAppCertificateId();
	m_edAppcertificatId.SetWindowTextW(s2cs(strAppCertificatId));
	bool res = str2int(gHQConfig.getAppCertEnable());
	m_btnAppcertificateEnable.SetCheck(res);
	std::string strMediaUid = gHQConfig.getLoginUid();
	strMediaUid = "1";
	m_edMediaUid.SetWindowTextW(s2cs(strMediaUid));
	m_edMediaUid.EnableWindow(FALSE);
	std::string strChannelName = gHQConfig.getChannelName();
	m_edChannelName.SetWindowTextW(s2cs(strChannelName));
	std::string strSignalAccount = gHQConfig.getSignalAccount();
	
	m_comLanguage.AddString(_T("English"));
	m_comLanguage.AddString(_T("Chinese"));
	std::string curLanguage = gHQConfig.getLanguage();
	if ("" == curLanguage || "0" == curLanguage){
		curLanguage = "0";
	}
	gHQConfig.setLanguage(curLanguage);
	m_comLanguage.SetCurSel(str2int(curLanguage));
	m_comLanguage.EnableWindow(FALSE);

#if 0
	m_comServerAccount.AddString(_T("agora_hq_cc_server_en"));
	m_comServerAccount.AddString(_T("agora_hq_cc_server"));
	std::string curServerAccount = gHQConfig.getServerAccount();
	int nCurSel = m_comServerAccount.FindStringExact(0, s2cs(curServerAccount));
	if (CB_ERR == nCurSel && "" != curServerAccount){
		nCurSel = m_comServerAccount.AddString(s2cs(curServerAccount));
	}
	m_comServerAccount.SetCurSel(nCurSel);
#endif

	int nRegion = str2int(gHQConfig.getRegion());
	m_comRegion.AddString(_T("Overseas Edition"));
	m_comRegion.AddString(_T("Chinese Edition"));
	m_comRegion.SetCurSel(nRegion);
	m_comLanguage.SetCurSel(nRegion);

	std::string strVideoIndex = gHQConfig.getVideoSolutinIndex();
	if ("" == strVideoIndex)
		strVideoIndex = "15";
	m_comSolutionIndex.SetCurSel(str2int(strVideoIndex));

	std::string strEnableEncrypt = gHQConfig.getEnableEncrypt();
	if ("" == strEnableEncrypt || "0" == strEnableEncrypt){

		m_btnEnableEncrypt.SetCheck(FALSE);
		gHQConfig.setEnableEncrypt("0");
	}
	else{

		m_btnEnableEncrypt.SetCheck(TRUE);
	}

	std::string strCameraID = gHQConfig.getCameraDeviceID();
	CString strDefCameraId = s2cs(strCameraID);

	int nDefSel = CB_ERR;
	pRtcEngine = CAgoraObject::GetEngine();
	if (pRtcEngine){

		if (CameraManager.Create(pRtcEngine)){
			CameraManager.TestCameraDevice(NULL, FALSE);

			CString strDeviceName; CString strDeviceId;
			int nCameraCount = CameraManager.GetDeviceCount();
			for (int nDeviceId = 0; nDeviceId < nCameraCount; nDeviceId++){

				CameraManager.GetDevice(nDeviceId, strDeviceName, strDeviceId);
				m_comCamera.AddString(strDeviceName);
				if (strDefCameraId == strDeviceId){
					nDefSel = nDeviceId;
					CameraManager.SetCurDevice(s2cs(gbk2utf8(cs2s(strDeviceId))));
				}
			}
		}
		
	}

	m_comCamera.SetCurSel(nDefSel);
}

void CDlgConfig::uninitCtrl()
{

}

void CDlgConfig::OnBnClickedButtonVideotest()
{
	// TODO:  在此添加控件通知处理程序代码

	if (pRtcEngine){
		if (CameraManager.IsTesting()){

			CameraManager.TestCameraDevice(m_trlTestVideo, FALSE);
			m_btnSatrtPreview.SetWindowTextW(_T("startPreview"));
		}
		else{

			CameraManager.TestCameraDevice(m_trlTestVideo, TRUE);
			m_btnSatrtPreview.SetWindowTextW(_T("stopPreview"));
		}
	}
}


void CDlgConfig::OnCbnSelchangeComboCamera()
{
	// TODO:  在此添加控件通知处理程序代码

	int nCurSel = m_comCamera.GetCurSel();
	if (CB_ERR != nCurSel){
		CString strDeviceName, strDeviceId;
		if (CameraManager.GetDevice(nCurSel, strDeviceName, strDeviceId)){
			if (_T("") != strDeviceId){

				CameraManager.SetCurDevice( s2cs(gbk2utf8(cs2s(strDeviceId))));
			}
		}
	}
}


void CDlgConfig::OnCbnSelchangeComboRegion()
{
	// TODO:  在此添加控件通知处理程序代码
	int nCursel = m_comRegion.GetCurSel();
	m_comLanguage.SetCurSel(nCursel);
}
