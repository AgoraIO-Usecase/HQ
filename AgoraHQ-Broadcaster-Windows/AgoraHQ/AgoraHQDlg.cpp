
// AgoraHQDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "AgoraHQ.h"
#include "AgoraHQDlg.h"
#include "afxdialogex.h"
#include "commonFun.h"
#include "DlgAnswer.h"
#include "DlgConfig.h"
#include "DlgInput.h"
#include "DlgSetTimeBonus.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define EVENT_TIMER_INVITEREMOTE 1

// ����Ӧ�ó��򡰹��ڡ��˵���� CAboutDlg �Ի���

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// �Ի�������
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

// ʵ��
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CAgoraHQDlg �Ի���


CAgoraHQDlg::CAgoraHQDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CAgoraHQDlg::IDD, pParent),
	m_pDlgAnswer(NULL),
	m_lpAgoraObject(NULL),
	m_pDlgConfig(nullptr),
	m_nLastmileQuality(-1),
	m_pDlgInputParam(nullptr),
	m_wndRemote(nullptr),
	m_pDlgSetTimeBonus(nullptr)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_agInviteRemoteAudience.isAccpet = false;
	m_agInviteRemoteAudience.remoteAccount = "";
	m_agInviteRemoteAudience.isValid = false;
}

void CAgoraHQDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STATIC_SHOWPIC, m_ctlShowPic);
	DDX_Control(pDX, IDC_STATIC_REMOTEVIDEO, m_ctlRemoteWnd);
	DDX_Control(pDX, IDC_STATIC_ANSWERAREA, m_ctlAnswerArea);
	DDX_Control(pDX, IDC_BUTTON_JOINCHANNEL, m_btnJoinChannel);
	DDX_Control(pDX, IDC_BUTTON_MEDIA_PARAM, m_btnMediaParam);
	DDX_Control(pDX, IDC_BUTTON_HQ_CONFIG, m_btnHQConfig);
	DDX_Control(pDX, IDC_BUTTON_INVITEMEDIA, m_btnInviteRemote);
	DDX_Control(pDX, IDC_STATIC_SDKVersion, m_ctlSdkVersion);
	DDX_Control(pDX, IDC_BUTTON_SetTimeBonus, m_btnSetTimeBonus);
}

BEGIN_MESSAGE_MAP(CAgoraHQDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_CLOSE()
	ON_WM_TIMER()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON_HQ_CONFIG, &CAgoraHQDlg::OnBnClickedButtonHqConfig)
	ON_BN_CLICKED(IDC_BUTTON_MEDIA_PARAM, &CAgoraHQDlg::OnBnClickedButtonMediaParam)
	ON_BN_CLICKED(IDC_BUTTON_JOINCHANNEL, &CAgoraHQDlg::OnBnClickedButtonJoinchannel)
	ON_MESSAGE(WM_MSGID(EID_JOINCHANNEL_SUCCESS), onJoinChannelSuccess)
	ON_MESSAGE(WM_MSGID(EID_WARNING), onWarning)
	ON_MESSAGE(WM_MSGID(EID_ERROR), onError)
	ON_MESSAGE(WM_MSGID(EID_LEAVE_CHANNEL), onLeaveChannel)
	ON_MESSAGE(WM_MSGID(EID_REQUEST_CHANNELKEY), onRequestChannelKey)
	ON_MESSAGE(WM_MSGID(EID_LASTMILE_QUALITY), onLastMileQuality)
	ON_MESSAGE(WM_MSGID(EID_FIRST_LOCAL_VIDEO_FRAME), onFirstLocalVideoFrame)
	ON_MESSAGE(WM_MSGID(EID_FIRST_REMOTE_VIDEO_DECODED), onFirstRemoteVideoDecoded)
	ON_MESSAGE(WM_MSGID(EID_FIRST_REMOTE_VIDEO_FRAME), onFirstRmoteVideoFrame)
	ON_MESSAGE(WM_MSGID(EID_USER_JOINED), onUserJoined)
	ON_MESSAGE(WM_MSGID(EID_USER_OFFLINE), onUserOff)
	ON_MESSAGE(WM_MSGID(EID_USER_MUTE_VIDEO), onUserMuteVideo)
	ON_MESSAGE(WM_MSGID(EID_CONNECTION_LOST), onConnectionLost)
	ON_MESSAGE((WM_NewChannelName), onNewChannelName)
	ON_MESSAGE(WM_InviteRemoteAudience, onInviteRemoteAudience)
	ON_MESSAGE(WM_InviteCallBackAccpet, onInviteCallBackAccept)
	ON_BN_CLICKED(IDC_BUTTON_INVITEMEDIA, &CAgoraHQDlg::OnBnClickedButtonInvitemedia)
	ON_BN_CLICKED(IDC_BUTTON_SetTimeBonus, &CAgoraHQDlg::OnBnClickedButtonSettimebonus)
END_MESSAGE_MAP()


// CAgoraHQDlg ��Ϣ�������

BOOL CAgoraHQDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// ��������...���˵�����ӵ�ϵͳ�˵��С�

	// IDM_ABOUTBOX ������ϵͳ���Χ�ڡ�
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// ���ô˶Ի����ͼ�ꡣ  ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	// TODO:  �ڴ���Ӷ���ĳ�ʼ������
	SetBackgroundColor(RGB(0xff,0xff,0xff),TRUE);

	gHQConfig.setAppId("363f97d9690e4c0a9780499ab14a16c0");
	getChannelName();
	m_strAppId = gHQConfig.getAppId();
	if ("" == m_strAppId){
		AfxMessageBox(_T("APPID Ϊ��.����������"));
		std::string iniFilePath = gHQConfig.getFilePah();
		gHQConfig.setAppId("");
		ShellExecute(NULL, _T("open"), s2cs(iniFilePath), NULL, NULL, SW_SHOW);
		::PostQuitMessage(0);
		return FALSE;
	}

	initCtrl();
	initAgoraMediaRtc();

	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

void CAgoraHQDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ  ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CAgoraHQDlg::OnPaint()
{
	CPaintDC dc(this); // ���ڻ��Ƶ��豸������
	if (IsIconic())
	{
		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ����������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{	
		DrawClient(&dc);
		//CDialogEx::OnPaint();
	}
}

void CAgoraHQDlg::OnClose()
{
	if (m_lpAgoraObject){

		m_lpAgoraObject->LeaveCahnnel();
	}
	if (m_lpRtcEngine){

		m_lpRtcEngine->stopPreview();
	}

	uninitCtrl();
	uninitAgoraMediaRtc();

	CDialogEx::OnCancel();
}

void CAgoraHQDlg::OnTimer(UINT_PTR nIDEvent)
{
	if (EVENT_TIMER_INVITEREMOTE == nIDEvent){
		KillTimer(EVENT_TIMER_INVITEREMOTE);

		CString csStrFormat;
		csStrFormat.Format(_T("\t Invitees: %s\n\t EnableVideo : %d \n\t EnableAudio: %d\n\t nTimeout: %d\n\t TimeOut Rejected invitation default .."),
			s2cs(m_agInviteRemoteAudience.remoteAccount),m_agInviteRemoteAudience.enableVideo,m_agInviteRemoteAudience.enableAudio,m_agInviteRemoteAudience.nTimeOut);

#if 1
		m_agInviteRemoteAudience.isValid = false;
		m_agInviteRemoteAudience.isAccpet = false;
		m_agInviteRemoteAudience.remoteAccount = "";
#else
		::PostMessage(theApp.GetMainWnd()->m_hWnd, WM_InviteCallBackAccpet, WPARAM(TRUE), NULL);
#endif

		AfxMessageBox(csStrFormat);
	}
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
HCURSOR CAgoraHQDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CAgoraHQDlg::OnBnClickedButtonHqConfig()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	if (NULL == m_pDlgConfig){
		m_pDlgConfig = new CDlgConfig;
		m_pDlgConfig->Create(CDlgConfig::IDD);
	}
	m_pDlgConfig->CenterWindow();
	m_pDlgConfig->ShowWindow(SW_SHOW);
}


void CAgoraHQDlg::OnBnClickedButtonMediaParam()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
}


void CAgoraHQDlg::OnBnClickedButtonJoinchannel()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	CString strParam;
	m_btnJoinChannel.GetWindowTextW(strParam);
	if (_T("JoinChannel") == strParam){
		if (NULL == m_pDlgConfig){
			m_pDlgConfig = new CDlgConfig;
			m_pDlgConfig->Create(CDlgConfig::IDD);
		}
		m_pDlgConfig->saveCameraID();

		m_lpAgoraObject->EnableLastmileTest(FALSE);
		m_lpAgoraObject->SetClientRole(CLIENT_ROLE_TYPE::CLIENT_ROLE_BROADCASTER);
		m_lpAgoraObject->SetChannelProfile(TRUE);

		m_uId = str2int(gHQConfig.getLoginUid());
		std::string strAppcertificatId = gHQConfig.getAppCertificateId();
		m_lpAgoraObject->SetSelfUID(m_uId);
		m_lpAgoraObject->SetAppCert(s2cs(strAppcertificatId));

		VideoCanvas vc;
		vc.renderMode = RENDER_MODE_FIT;
		vc.uid = m_uId;
		vc.view = m_ctlShowPic;
		m_lpRtcEngine->setupLocalVideo(vc);

		if (m_pDlgConfig){

			int strVideoIndex = m_pDlgConfig->getVideoIndex();
			m_lpAgoraObject->SetVideoProfile(strVideoIndex, FALSE);
		}

		m_lpRtcEngine->startPreview();
		
		//m_lpAgoraObject->MuteAllRemoteVideo(TRUE);
		//m_lpAgoraObject->MuteAllRemoteAudio(TRUE);

		m_strChannelName = gHQConfig.getChannelName();
		std::string strAppCertificateEnable = gHQConfig.getAppCertEnable();
		if ("0" == strAppCertificateEnable || "" == strAppCertificateEnable){

			m_lpAgoraObject->JoinChannel(s2cs(m_strChannelName), m_uId);
		}
		else if ("1" == strAppCertificateEnable){

			CStringA strMediaChannelKey = m_lpAgoraObject->getDynamicMediaChannelKey(s2cs(m_strChannelName));
			m_lpAgoraObject->JoinChannel(s2cs(m_strChannelName), m_uId, strMediaChannelKey);
		}

		SetWindowText(s2cs(m_strChannelName));
	}
	else if (_T("LeaveChannel") == strParam){
		
		m_lpAgoraObject->LeaveCahnnel();
		m_lpRtcEngine->stopPreview();
	}
}

void CAgoraHQDlg::initCtrl()
{
	std::string appcertEnable = gHQConfig.getAppCertEnable();
	if("1" == appcertEnable){
		m_strAppCertificatId = gHQConfig.getAppCertificateId();
		if ("" == m_strAppCertificatId){
			AfxMessageBox(_T("����ChannelKey ,appCertificatID ����Ϊ��"));
			::PostQuitMessage(0);
			return;
		}
	}

	m_nLastmileQuality = QUALITY_TYPE::QUALITY_DOWN;
	CBitmap	bmpNetQuality;
	bmpNetQuality.LoadBitmap(IDB_NETWORK_QUALITY);	
	m_imgNetQuality.Create(32, 32, ILC_COLOR24 | ILC_MASK, 6, 1);
	m_imgNetQuality.Add(&bmpNetQuality, RGB(0xFF, 0, 0xFF));	m_btnMediaParam.ShowWindow(SW_HIDE);

	m_btnJoinChannel.EnableWindow(FALSE);
	m_btnInviteRemote.EnableWindow(FALSE);

	if (nullptr == m_pDlgAnswer){
		m_pDlgAnswer = new CDlgAnswer(&m_ctlAnswerArea);
		m_pDlgAnswer->Create(CDlgAnswer::IDD, &m_ctlAnswerArea);
		m_pDlgAnswer->ShowWindow(SW_SHOW);
	}
}

void CAgoraHQDlg::uninitCtrl()
{
	if (m_pDlgConfig){
		delete m_pDlgConfig;
		m_pDlgConfig = nullptr;
	}

	if (m_pDlgAnswer){
		delete m_pDlgAnswer;
		m_pDlgAnswer = nullptr;
	}

	if (m_pDlgInputParam){
		delete m_pDlgInputParam;
		m_pDlgInputParam = nullptr;
	}

	if (m_pDlgSetTimeBonus){
		delete m_pDlgSetTimeBonus;
		m_pDlgSetTimeBonus = nullptr;
	}
}

void CAgoraHQDlg::initAgoraMediaRtc()
{
	if ("" == m_strAppId){
		return;
	}
	m_lpAgoraObject = CAgoraObject::GetAgoraObject(s2cs(m_strAppId));
	ASSERT(m_lpAgoraObject);
	m_lpAgoraObject->SetMsgHandlerWnd(m_hWnd);

	m_lpRtcEngine = CAgoraObject::GetEngine();
	ASSERT(m_lpRtcEngine);

	CString strSdkLogFilePath = getSdkLogPath();
	m_lpAgoraObject->SetLogFilePath(strSdkLogFilePath);
	m_lpAgoraObject->EnableLastmileTest(TRUE);
	m_lpAgoraObject->EnableLocalMirrorImage(FALSE);
	m_lpAgoraObject->EnableLoopBack(TRUE);

	m_lpAgoraObject->EnableVideo(TRUE);
}

void CAgoraHQDlg::uninitAgoraMediaRtc()
{
	if (nullptr == m_lpAgoraObject){
		return;
	}

	m_lpAgoraObject->EnableVideo(FALSE);
	m_lpAgoraObject->EnableLastmileTest(FALSE);
	if (m_lpAgoraObject){
		CAgoraObject::CloseAgoraObject();
		m_lpAgoraObject = nullptr;
		m_lpRtcEngine = nullptr;
	}
}

void CAgoraHQDlg::DrawClient(CDC *lpDC)
{
	if (0 <= m_nLastmileQuality){

		m_imgNetQuality.Draw(lpDC, m_nLastmileQuality, CPoint(16, 16), ILD_NORMAL);
	}
}

LRESULT CAgoraHQDlg::onJoinChannelSuccess(WPARAM wParam, LPARAM lParam)
{
	LPAGE_JOINCHANNEL_SUCCESS lpData = (LPAGE_JOINCHANNEL_SUCCESS)wParam;

	m_lpAgoraObject->SetSelfUID(lpData->uid);

	//fix title
	CString titleVideo;
	GetWindowText(titleVideo);
	CString newTitle;
	newTitle.Format(_T("[%s] [uid: %ld] %s"), _T("HQ_Windows"), lpData->uid, titleVideo);
	SetWindowText(newTitle);
	Invalidate();

	delete lpData;

	m_btnInviteRemote.EnableWindow(TRUE);
	CString lpStrSdkVersion = m_lpAgoraObject->GetSDKVersionEx();
	m_ctlSdkVersion.SetWindowTextW(lpStrSdkVersion);

	m_btnJoinChannel.SetWindowTextW(_T("LeaveChannel"));
	if (nullptr != m_pDlgAnswer){
		m_pDlgAnswer->updateStatusToPublish();
	}

	return 0;
}

LRESULT CAgoraHQDlg::onWarning(WPARAM wParam, LPARAM lParam)
{
	return TRUE;
}

LRESULT CAgoraHQDlg::onError(WPARAM wParam, LPARAM lParam)
{
	LPAGE_ERROR lpData = (LPAGE_ERROR)wParam;

	if (ERR_CHANNEL_KEY_EXPIRED == lpData->err){
		if (m_lpRtcEngine){
			CString strChannelName = m_lpAgoraObject->GetChanelName();
			CStringA strNewMediaChannelKey = m_lpAgoraObject->getDynamicMediaChannelKey(strChannelName);
			m_lpRtcEngine->renewChannelKey(strNewMediaChannelKey);
		}
	}
	delete lpData->msg; lpData->msg = nullptr;
	delete lpData; lpData = nullptr;

	return TRUE;
}

LRESULT CAgoraHQDlg::onLeaveChannel(WPARAM wParam, LPARAM lParam)
{
	LPAGE_LEAVE_CHANNEL lpData = (LPAGE_LEAVE_CHANNEL)wParam;
	if (lpData){
		SetWindowText(s2cs(m_strChannelName));
	}

	delete lpData; lpData = nullptr;
	m_btnJoinChannel.SetWindowTextW(_T("JoinChannel"));

	return TRUE;
}

LRESULT CAgoraHQDlg::onRequestChannelKey(WPARAM wParam, LPARAM lParam)
{
	if (m_lpRtcEngine){
		CString strChannelName = m_lpAgoraObject->GetChanelName();
		CStringA strNewMediaChannelKey = m_lpAgoraObject->getDynamicMediaChannelKey(strChannelName);
		m_lpRtcEngine->renewChannelKey(strNewMediaChannelKey);
	}

	return TRUE;
}

LRESULT CAgoraHQDlg::onLastMileQuality(WPARAM wParam, LPARAM lParam)
{
	LPAGE_LASTMILE_QUALITY lpData = (LPAGE_LASTMILE_QUALITY)wParam;
	m_nLastmileQuality = lpData->quality;

	delete lpData; lpData = nullptr;
	Invalidate(TRUE);
	m_lpAgoraObject->EnableLastmileTest(FALSE);

	switch (m_nLastmileQuality)
	{
	case QUALITY_TYPE::QUALITY_EXCELLENT:
	case QUALITY_TYPE::QUALITY_GOOD:
	case QUALITY_TYPE::QUALITY_POOR:
		if ("" !=m_strChannelName)
		m_btnJoinChannel.EnableWindow(TRUE);
		break;
	default:
		if ("" != m_strChannelName)
		m_btnJoinChannel.EnableWindow(FALSE);
		break;
	}

	return TRUE;
}

LRESULT CAgoraHQDlg::onFirstLocalVideoFrame(WPARAM wParam, LPARAM lParam)
{
	return TRUE;
}

LRESULT CAgoraHQDlg::onFirstRemoteVideoDecoded(WPARAM wParam, LPARAM lParam)
{
	LPAGE_FIRST_REMOTE_VIDEO_DECODED lpData = (LPAGE_FIRST_REMOTE_VIDEO_DECODED)wParam;
	if (lpData){

		char logMsg[512] = { '\0' };
		sprintf_s(logMsg, "AgoraHQDlg::onFirstRemoteVideoDecoded : %d\n", lpData->uid);
		OutputDebugStringA(logMsg);

		if (lpData->uid == m_nInviteRemote){

			VideoCanvas vcRemote;
			vcRemote.renderMode = RENDER_MODE_HIDDEN;
			vcRemote.uid = lpData->uid;
			vcRemote.view = m_ctlRemoteWnd;

			CRect  crLocal;
			m_ctlShowPic.GetWindowRect(&crLocal);
			ScreenToClient(crLocal);
			m_ctlShowPic.SetWindowPos(&m_ctlRemoteWnd, crLocal.left, crLocal.top, crLocal.right - crLocal.left, crLocal.bottom - crLocal.top, SWP_SHOWWINDOW);

			if (m_lpRtcEngine){

				m_lpRtcEngine->setupRemoteVideo(vcRemote);
				OutputDebugStringA("firstRemoteVideoDecoded setRemoteVideo.\n");
			}
		}
		else{
			m_mapRemoteView[lpData->uid] = nullptr;
		}

		delete lpData; lpData = nullptr;
	}
	return TRUE;
}

LRESULT CAgoraHQDlg::onFirstRmoteVideoFrame(WPARAM wParam, LPARAM lParam)
{
	LPAGE_FIRST_REMOTE_VIDEO_FRAME lpData = (LPAGE_FIRST_REMOTE_VIDEO_FRAME)wParam;
	if (lpData){

		if (lpData->uid == m_nInviteRemote && m_agInviteRemoteAudience.isValid && m_agInviteRemoteAudience.isAccpet){

			VideoCanvas vcRemote;
			vcRemote.renderMode = RENDER_MODE_HIDDEN;
			vcRemote.uid = lpData->uid;
			vcRemote.view = m_ctlRemoteWnd;
			m_mapRemoteView[lpData->uid] = m_ctlRemoteWnd;

			CRect  crLocal;
			m_ctlShowPic.GetWindowRect(&crLocal);
			ScreenToClient(crLocal);
			m_ctlShowPic.SetWindowPos(&m_ctlRemoteWnd, crLocal.left, crLocal.top, crLocal.right - crLocal.left, crLocal.bottom - crLocal.top, SWP_SHOWWINDOW);

			if (m_lpRtcEngine){

				m_lpRtcEngine->setupRemoteVideo(vcRemote);
			}
		}
		else{
			m_mapRemoteView[lpData->uid] = nullptr;
		}

		delete lpData; lpData = nullptr;
	}

	return TRUE;
}

LRESULT CAgoraHQDlg::onUserJoined(WPARAM wParam, LPARAM lParam)
{
	LPAGE_USER_JOINED lpData = (LPAGE_USER_JOINED)wParam;
	if (lpData){

		if (0 < lpData->uid ){

		}

		delete lpData; lpData = nullptr;
	}

	return TRUE;
}

LRESULT CAgoraHQDlg::onUserOff(WPARAM wParam, LPARAM lParam)
{
	LPAGE_USER_OFFLINE lpData = (LPAGE_USER_OFFLINE)wParam;
	if (lpData){

		delete lpData; lpData = nullptr;
	}

	return TRUE;
}

LRESULT CAgoraHQDlg::onUserMuteVideo(WPARAM wParam, LPARAM lParam)
{
	LPAGE_USER_MUTE_VIDEO lpData = (LPAGE_USER_MUTE_VIDEO)wParam;
	if (lpData){

		char logMsg[512] = { '\0' };
		sprintf_s(logMsg, "onUserMuteVideo: %d %d\n", lpData->uid, lpData->muted);
		OutputDebugStringA(logMsg);

		if (lpData->muted){

			m_ctlRemoteWnd.ShowWindow(SW_HIDE);
			std::map<uid_t, HWND>::iterator it = m_mapRemoteView.find(lpData->uid);
			if (m_mapRemoteView.end() != it){

				VideoCanvas vcRemote;
				vcRemote.renderMode = RENDER_MODE_HIDDEN;
				vcRemote.uid = lpData->uid;
				vcRemote.view = nullptr;
				if (m_lpRtcEngine){

					m_lpRtcEngine->setupRemoteVideo(vcRemote);
					m_mapRemoteView[it->first] = nullptr;
					OutputDebugStringA("onUserMuteVideo setRemoteVideo. HIDDEN\n");
				}

				m_mapRemoteView.erase(it);
				Invalidate();
			}
		}
		else{

			m_mapRemoteView[lpData->uid] = m_wndRemote;
			if (m_nInviteRemote == lpData->uid){

				VideoCanvas vcRemote;
				vcRemote.renderMode = RENDER_MODE_HIDDEN;
				vcRemote.uid = lpData->uid;
				vcRemote.view = m_ctlRemoteWnd;

				CRect  crLocal;
				m_ctlShowPic.GetWindowRect(&crLocal);
				ScreenToClient(crLocal);
				m_ctlShowPic.SetWindowPos(&m_ctlRemoteWnd, crLocal.left, crLocal.top, crLocal.right - crLocal.left, crLocal.bottom - crLocal.top, SWP_SHOWWINDOW);

				if (m_lpRtcEngine){

					m_lpRtcEngine->setupRemoteVideo(vcRemote);
					m_ctlRemoteWnd.ShowWindow(SW_SHOW);
					OutputDebugStringA("onUserMuteVideo setRemoteVideo.SHOW\n");
				}
			}
		}

		delete lpData; lpData = nullptr;
	}

	return TRUE;
}

LRESULT CAgoraHQDlg::onConnectionLost(WPARAM wParam, LPARAM lParam)
{
	return TRUE;
}

LRESULT CAgoraHQDlg::onNewChannelName(WPARAM wParam, LPARAM lParam)
{
	LPAG_SIGNAL_NEWCHANNELNAME lpData = (LPAG_SIGNAL_NEWCHANNELNAME)wParam;
	if (lpData){

		m_strChannelName = lpData->channelname;
		if ("" != m_strChannelName){

			gHQConfig.setChannelName(m_strChannelName);
			gHQConfig.setSignalAccount(lpData->account);
			if (m_nLastmileQuality == QUALITY_EXCELLENT ||
				QUALITY_GOOD == m_nLastmileQuality ||
				QUALITY_POOR == m_nLastmileQuality){

				m_btnJoinChannel.EnableWindow(TRUE);
			}
		}
	}

	delete lpData; lpData = nullptr;

	return TRUE;
}

LRESULT CAgoraHQDlg::onInviteRemoteAudience(WPARAM wParam, LPARAM lParam)
{
	LPAG_INVITE_REMOTEAUDEINCE lpData = (PAG_INVITE_REMOTEAUDIENCE)wParam;
	OutputDebugStringA("onInviteRemoteAudience\n");
	if (lpData){

		if (lpData->isAccpet){
			if (m_lpAgoraObject){
				m_lpAgoraObject->MuteRemoteAudio(str2int(lpData->remoteAccount), !lpData->enableAudio);
				m_lpAgoraObject->MuteRemoteVideo(str2int(lpData->remoteAccount), !lpData->enableVideo);

				for (std::map<UINT, HWND>::iterator it = m_mapRemoteView.begin(); m_mapRemoteView.end() != it; it++){
					if (m_nInviteRemote == it->first){
						if (nullptr != it->second){
							OutputDebugStringA("repeat signling Invite remote \n");
							break;
						}

						VideoCanvas vcRemote;
						vcRemote.renderMode = RENDER_MODE_HIDDEN;
						vcRemote.uid = it->first;
						vcRemote.view = m_ctlRemoteWnd;

						CRect  crLocal;
						m_ctlShowPic.GetWindowRect(&crLocal);
						ScreenToClient(crLocal);
						m_ctlShowPic.SetWindowPos(&m_ctlRemoteWnd, crLocal.left, crLocal.top, crLocal.right - crLocal.left, crLocal.bottom - crLocal.top, SWP_SHOWWINDOW);

						if (m_lpRtcEngine){

							m_lpRtcEngine->setupRemoteVideo(vcRemote);

						}
					}
					else{

						m_lpAgoraObject->MuteRemoteAudio(it->first);
						m_lpAgoraObject->MuteRemoteVideo(it->first);
						it->second = nullptr;
					}
				}
			}
		}
		else{
			m_agInviteRemoteAudience.enableAudio = lpData->enableAudio;
			m_agInviteRemoteAudience.enableVideo = lpData->enableVideo;
			m_agInviteRemoteAudience.nTimeOut = lpData->nTimeOut;
			m_agInviteRemoteAudience.isAccpet = false;
			m_agInviteRemoteAudience.remoteAccount = lpData->remoteAccount;
			m_agInviteRemoteAudience.isValid = true;

			KillTimer(EVENT_TIMER_INVITEREMOTE);
			SetTimer(EVENT_TIMER_INVITEREMOTE, m_agInviteRemoteAudience.nTimeOut, NULL);

			delete lpData; lpData = nullptr;
		}
	}

	return TRUE;
}

LRESULT CAgoraHQDlg::onInviteCallBackAccept(WPARAM wParam, LPARAM lParam)
{
	LPAG_INVITE_CALLBACKACCEPT lpData = (LPAG_INVITE_CALLBACKACCEPT)wParam;
	OutputDebugStringA("onInviteCallBackAccept\n");
	if (m_agInviteRemoteAudience.isValid){
		KillTimer(EVENT_TIMER_INVITEREMOTE);

		m_nInviteRemote = str2long(lpData->remoteMediaUid);
		if (m_lpAgoraObject && lpData->isAccept && m_agInviteRemoteAudience.remoteAccount == lpData->remoteSigAccount){
			m_lpAgoraObject->MuteRemoteAudio(str2long(lpData->remoteMediaUid), !m_agInviteRemoteAudience.enableAudio);
			m_lpAgoraObject->MuteRemoteVideo(str2long(lpData->remoteMediaUid), !m_agInviteRemoteAudience.enableVideo);

			for (std::map<UINT, HWND>::iterator it = m_mapRemoteView.begin(); m_mapRemoteView.end() != it; it++){
				m_nInviteRemote = str2long(lpData->remoteMediaUid);
				if (m_nInviteRemote == it->first){
					if (nullptr != it->second){
						OutputDebugStringA("repeat signling Invite remote \n");
						break;
					}

					VideoCanvas vcRemote;
					vcRemote.renderMode = RENDER_MODE_HIDDEN;
					vcRemote.uid = str2long(lpData->remoteMediaUid);
					vcRemote.view = m_ctlRemoteWnd;

					CRect  crLocal;
					m_ctlShowPic.GetWindowRect(&crLocal);
					ScreenToClient(crLocal);
					m_ctlShowPic.SetWindowPos(&m_ctlRemoteWnd, crLocal.left, crLocal.top, crLocal.right - crLocal.left, crLocal.bottom - crLocal.top, SWP_SHOWWINDOW);

					if (m_lpRtcEngine){

						m_lpRtcEngine->setupRemoteVideo(vcRemote);
						OutputDebugStringA("callback accept setUpRemoteVideo\n");
					}
				}
				else{

					m_lpAgoraObject->MuteRemoteAudio(it->first);
					m_lpAgoraObject->MuteRemoteVideo(it->first);
					it->second = nullptr;
				}
			}
		}
		else{
			CString csStrFormat;
			csStrFormat.Format(_T("\t Invitees: %s\n\t EnableVideo : %d \n\t EnableAudio: %d\n\t nTimeout: %d\n\t Rejected invitation.."),
				s2cs(m_agInviteRemoteAudience.remoteAccount), m_agInviteRemoteAudience.enableVideo, m_agInviteRemoteAudience.enableAudio, m_agInviteRemoteAudience.nTimeOut);

			AfxMessageBox(csStrFormat);
		}
	}

	m_agInviteRemoteAudience.isValid = false;
	m_agInviteRemoteAudience.isAccpet = false;
	m_agInviteRemoteAudience.remoteAccount = "";

	return TRUE;
}

void CAgoraHQDlg::OnBnClickedButtonInvitemedia()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	if (nullptr == m_pDlgInputParam){
		m_pDlgInputParam = new CDlgInput();
		m_pDlgInputParam->Create(CDlgInput::IDD);
	}

	m_pDlgInputParam->showWindow(eTagMsgtype::eType_Instance);
	m_agInviteRemoteAudience.isAccpet = false;
	m_agInviteRemoteAudience.remoteAccount = "";
	m_agInviteRemoteAudience.enableAudio = false;
	m_agInviteRemoteAudience.enableVideo = false;
	m_agInviteRemoteAudience.isValid = false;
}

void CAgoraHQDlg::OnBnClickedButtonSettimebonus()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	if (nullptr == m_pDlgSetTimeBonus){
		m_pDlgSetTimeBonus = new CDlgSetTimeBonus();
		m_pDlgSetTimeBonus->Create(CDlgSetTimeBonus::IDD);
	}

	m_pDlgSetTimeBonus->CenterWindow();
	m_pDlgSetTimeBonus->ShowWindow(SW_SHOW);
}
