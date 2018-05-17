
// AgoraHQDlg.cpp : 实现文件
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
#include "UrlService/CurlService.h"
#include "CustomMsg.h"
// OBS for ExtCapture
#include "SelectOBSSourceDlg.h"
#include <SelectOBSSourceDlg.h>
#include "ImagePropertiesDlg.h"
#include "FFmpegSourceDlg.h"
#include "DShowCameraDlg.h"

#include <sstream>
#include <util/lexer.h>

#include "OBSFilterDlg.h"
#include "ChromaFilterDlg.h"
#define SERVICE_PATH "service.json"
//OBS end
#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define EVENT_TIMER_INVITEREMOTE 1

// 用于应用程序“关于”菜单项的 CAboutDlg 对话框
class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
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


// CAgoraHQDlg 对话框


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
	m_rcWndLocal = { 0, 0, 0, 0 };//OBS for ExtCapture
	m_rcNetQuality = CRect( 16, 16, 48, 48 );
}


CAgoraHQDlg::~CAgoraHQDlg()
{
//	outputHandler.reset();// del for test
	agoraService = nullptr;
	agoraOutputHandler.reset();
	obs_display_remove_draw_callback(m_wndLocal.GetDisplay(),
		CAgoraHQDlg::RenderMain, this);

	obs_enter_graphics();
	gs_vertexbuffer_destroy(box);
	gs_vertexbuffer_destroy(boxLeft);
	gs_vertexbuffer_destroy(boxTop);
	gs_vertexbuffer_destroy(boxRight);
	gs_vertexbuffer_destroy(boxBottom);
	gs_vertexbuffer_destroy(circle);
	obs_leave_graphics();
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
	// OBS for ExtCapture
	DDX_Control(pDX, IDC_CHECK_OBS, m_chkOBS);
	DDX_Control(pDX, IDC_BUTTON_OBS_IMAGE, m_btnOBSImage);
	DDX_Control(pDX, IDC_BUTTON_OBS_CAMERA, m_btnOBSCamera);
	DDX_Control(pDX, IDC_BUTTON_OBS_FFMPEG_MEDIA, m_btnOBSFFmpeg);
	// end
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
	ON_MESSAGE(WM_URL_MSG(URL_INVITE_STATS_SUCCESS), onHttpInviteStatusSuccess)
	// OBS for ExtCapture
	ON_BN_CLICKED(IDC_BUTTON_OBS_IMAGE, &CAgoraHQDlg::OnBnClickedButtonObsImage)
	ON_BN_CLICKED(IDC_BUTTON_OBS_CAMERA, &CAgoraHQDlg::OnBnClickedButtonObsCamera)
	ON_BN_CLICKED(IDC_BUTTON_OBS_FFMPEG_MEDIA, &CAgoraHQDlg::OnBnClickedButtonObsFfmpegMedia)
	ON_BN_CLICKED(IDC_CHECK_OBS, &CAgoraHQDlg::OnClickedCheckObs)
	// end
END_MESSAGE_MAP()


// CAgoraHQDlg 消息处理程序

BOOL CAgoraHQDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
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

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO:  在此添加额外的初始化代码
	SetBackgroundColor(RGB(0xff,0xff,0xff),TRUE);

	AfxGetUrlService()->GetUrlCallback()->SetMsgReceiver(m_hWnd);
	if ("" == gHQConfig.getAppId()){

		gHQConfig.setAppId("324f0da1e2284832a44fee5fcbec44c1");//319294c67d174c878cc7922551e6e773 319294c67d174c878cc7922551e6e773 5463902dc7254fdf8779989252e5e35
	}

	getChannelName();
	m_strAppId = gHQConfig.getAppId();
	if ("" == m_strAppId){
		AfxMessageBox(_T("APPID 为空.请重新配置"));
		std::string iniFilePath = gHQConfig.getFilePah();
		gHQConfig.setAppId("");
		ShellExecute(NULL, _T("open"), s2cs(iniFilePath), NULL, NULL, SW_SHOW);
		::PostQuitMessage(0);
		return FALSE;
	}
	
	initCtrl();
	initAgoraMediaRtc();
	InitOBSPreview();
	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

// OBS for ExtCapture
void CAgoraHQDlg::InitOBSPreview()
{
	RECT rcDesk;
	GetDesktopWindow()->GetWindowRect(&rcDesk);
	m_ctlShowPic.GetWindowRect(&m_rcWndLocal);
	previewScale = (float)(m_rcWndLocal.bottom - m_rcWndLocal.top) / (rcDesk.bottom - rcDesk.top);

	m_wndLocal.SetReceiveWnd(m_hWnd);
	m_wndLocal.Create(NULL, NULL, WS_CHILD | WS_VISIBLE | WS_BORDER, CRect(0, 0, 1, 1), this, IDC_BASEWND_VIDEO + 4);

	m_wndLocal.SetFaceColor(RGB(0x58, 0x58, 0x58));
	
	obs_display_set_enabled(m_wndLocal.GetDisplay(), m_chkOBS.GetCheck());
	EnableOBSCtrl(m_chkOBS.GetCheck());
}

void CAgoraHQDlg::EnableOBSCtrl(bool bEnable)
{
	m_btnOBSImage.EnableWindow(bEnable);
	m_btnOBSCamera.EnableWindow(bEnable);
	m_btnOBSFFmpeg.EnableWindow(bEnable);
}

BOOL CAgoraHQDlg::OnWndMsg(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pResult)
{
	if (WM_CREATE_PREVIEW == message)//创建preview
	{
		obs_display_add_draw_callback(m_wndLocal.GetDisplay(), CAgoraHQDlg::RenderMain, this);
	}
	else if (WM_INVALIDATE_PREVIEW == message)
	{
		m_wndLocal.ResizeDisplay();
	}
	else if (WM_OBS_SOURCE_DELETE == message)
	{
		RemoveSelectedSceneItem();
	}
	else if (WM_OBS_SOURCE_MOVEUP == message)
	{
		OBSSceneItem item = GetCurrentSceneItem();
		obs_sceneitem_set_order(item, OBS_ORDER_MOVE_UP);
	}
	else if (WM_OBS_SOURCE_MOVEDOWN == message)
	{
		OBSSceneItem item = GetCurrentSceneItem();
		obs_sceneitem_set_order(item, OBS_ORDER_MOVE_DOWN);
	}
	else if (WM_OBS_SOURCE_MOVETOP == message)
	{
		OBSSceneItem item = GetCurrentSceneItem();
		obs_sceneitem_set_order(item, OBS_ORDER_MOVE_TOP);
	}
	else if (WM_OBS_SOURCE_MOVEBOTTOM == message)
	{
		OBSSceneItem item = GetCurrentSceneItem();
		obs_sceneitem_set_order(item, OBS_ORDER_MOVE_BOTTOM);
	}
	else if (WM_OBS_SOURCE_FILTERS == message)
	{
		OBSSceneItem item = GetCurrentSceneItem();
		if (item)
		{
			OBSSource source = obs_sceneitem_get_source(item);
			//COBSFilterDlg dlg(source, this);
			CChromaFilterDlg dlg(source, this);
			dlg.DoModal();
		}
	
	}
	return CDialogEx::OnWndMsg(message, wParam, lParam, pResult);
}
// end

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

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CAgoraHQDlg::OnPaint()
{
	CPaintDC dc(this); // 用于绘制的设备上下文
	if (IsIconic())
	{
		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;
		// 绘制图标
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
	//CAgoraSignalInstance::getSignalInstance()->ReleaseInstance();
	if (agoraOutputHandler->AgoraActive()){
		m_lpAgoraObject->EnableExtendVideoCapture(FALSE, NULL);
		m_lpAgoraObject->EnableExtendAudioCapture(FALSE, NULL);
		agoraOutputHandler->StopAgora();
	}
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
		gFileApp.write(cs2s(csStrFormat));

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

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CAgoraHQDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CAgoraHQDlg::OnBnClickedButtonHqConfig()
{
	// TODO:  在此添加控件通知处理程序代码
	if (NULL == m_pDlgConfig){
		m_pDlgConfig = new CDlgConfig;
		m_pDlgConfig->Create(CDlgConfig::IDD);
	}
	m_pDlgConfig->CenterWindow();
	m_pDlgConfig->ShowWindow(SW_SHOW);
}


void CAgoraHQDlg::OnBnClickedButtonMediaParam()
{
	// TODO:  在此添加控件通知处理程序代码
}

void CAgoraHQDlg::OnBnClickedButtonJoinchannel()
{
	// TODO:  在此添加控件通知处理程序代码
	CString strParam;
	m_btnJoinChannel.GetWindowTextW(strParam);
	if (_T("JoinChannel") == strParam){
		if (m_chkOBS.GetCheck())
			JoinChannel_OBS();
		else
			JoinChannel_Agora();
	}
	else if (_T("LeaveChannel") == strParam){
		if (m_chkOBS.GetCheck())
			LeaveChannel_OBS();
		else
			LeaveChannel_Agora();
	}
}

void CAgoraHQDlg::JoinChannel_Agora()
{
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
	m_pDlgAnswer->joinchannel();
}

void CAgoraHQDlg::LeaveChannel_Agora()
{
	m_pDlgAnswer->leaveChannel();
	m_lpAgoraObject->LeaveCahnnel();
	m_lpRtcEngine->stopPreview();
}

#if 0
void CAgoraHQDlg::OnBnClickedButtonJoinchannel()
{
	// TODO:  在此添加控件通知处理程序代码
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

		m_pDlgAnswer->joinchannel();
	}
	else if (_T("LeaveChannel") == strParam){
		
		m_pDlgAnswer->leaveChannel();
		m_lpAgoraObject->LeaveCahnnel();
		m_lpRtcEngine->stopPreview();
	}
}
#endif

void CAgoraHQDlg::initCtrl()
{
	std::string appcertEnable = gHQConfig.getAppCertEnable();
	if("1" == appcertEnable){
		m_strAppCertificatId = gHQConfig.getAppCertificateId();
		if ("" == m_strAppCertificatId){
			AfxMessageBox(_T("启用ChannelKey ,appCertificatID 不能为空"));
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
		m_pDlgAnswer->m_pAgoraHQDlg = this;
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

	CString strSdkLogFilePath = s2cs(getMediaSdkLogPath());
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
	//Invalidate();

	if (lpData)
	{
		delete[] lpData->channel;
		lpData->channel = NULL;
		delete lpData;
		lpData = NULL;
	}
	m_btnInviteRemote.EnableWindow(TRUE);
	CString lpStrSdkVersion = m_lpAgoraObject->GetSDKVersionEx();
	m_ctlSdkVersion.SetWindowTextW(lpStrSdkVersion);

	m_btnJoinChannel.SetWindowTextW(_T("LeaveChannel"));
	if (nullptr != m_pDlgAnswer){
		m_pDlgAnswer->updateStatusToPublish();
	}
	m_chkOBS.EnableWindow(FALSE);
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
	m_chkOBS.EnableWindow(TRUE);
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
	char szBuffer[128] = { '\0' };
	sprintf_s(szBuffer, "LastMileQuality : %d\n", m_nLastmileQuality);
	gFileApp.write(szBuffer);
	OutputDebugStringA(szBuffer);

	delete lpData; lpData = nullptr;
	InvalidateRect(m_rcNetQuality, TRUE);
	//m_lpAgoraObject->EnableLastmileTest(FALSE);

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

	m_btnJoinChannel.EnableWindow(TRUE);

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
		sprintf_s(logMsg, "AgoraHQDlg::onFirstRemoteVideoDecoded : %u\n", lpData->uid);
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

				gFileApp.write("onFirstRemoteVideoDecoded setRemoteVideo");
				m_lpRtcEngine->setupRemoteVideo(vcRemote);
				m_ctlRemoteWnd.ShowWindow(SW_SHOW);
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

			m_ctlRemoteWnd.GetWindowRect(&m_rcRemote);
			ScreenToClient(&m_rcRemote);

			CRect  crLocal;
			m_ctlShowPic.GetWindowRect(&crLocal);
			ScreenToClient(crLocal);
			m_ctlShowPic.SetWindowPos(&m_ctlRemoteWnd, crLocal.left, crLocal.top, crLocal.right - crLocal.left, crLocal.bottom - crLocal.top, SWP_SHOWWINDOW);

			if (m_lpRtcEngine){

				gFileApp.write("onFirstRmoteVideoFrame setRemoteVideo");
				m_lpRtcEngine->setupRemoteVideo(vcRemote);
				m_ctlRemoteWnd.ShowWindow(SW_SHOW);
				OutputDebugStringA("onFirstRmoteVideoFrame setRemoteVideo.\n");
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
		sprintf_s(logMsg, "onUserMuteVideo: %u %d\n", lpData->uid, lpData->muted);
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
					gFileApp.write("onUserMuteVideo setRemoteVideo. HIDDEN");
				}

				m_mapRemoteView.erase(it);
				
				InvalidateRect(&m_rcRemote);
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
					gFileApp.write("onUserMuteVideo setRemoteVideo.SHOW");
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
							m_ctlRemoteWnd.ShowWindow(SW_SHOW);
							OutputDebugStringA("onInviteRemoteAudience setUpRemoteVideo\n");
							gFileApp.write("onInviteRemoteAudience setUpRemoteVideo");
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
			gFileApp.write("onInviteRemoteAudience now is not Accpet..");
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
						gFileApp.write("repeat signling Invite remote");
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

						gFileApp.write("callback accept setUpRemoteVideo");
						m_lpRtcEngine->setupRemoteVideo(vcRemote);
						m_ctlRemoteWnd.ShowWindow(SW_SHOW);
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
			gFileApp.write(cs2s(csStrFormat));

			AfxMessageBox(csStrFormat);
		}
	}

	if (lpData)
	{
		delete lpData;
		lpData = nullptr;
	}

	m_agInviteRemoteAudience.isValid = false;
	m_agInviteRemoteAudience.isAccpet = false;
	m_agInviteRemoteAudience.remoteAccount = "";

	return TRUE;
}

void CAgoraHQDlg::OnBnClickedButtonInvitemedia()
{
	// TODO:  在此添加控件通知处理程序代码
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
	// TODO:  在此添加控件通知处理程序代码
	if (nullptr == m_pDlgSetTimeBonus){
		m_pDlgSetTimeBonus = new CDlgSetTimeBonus();
		m_pDlgSetTimeBonus->Create(CDlgSetTimeBonus::IDD);
	}

	m_pDlgSetTimeBonus->CenterWindow();
	m_pDlgSetTimeBonus->ShowWindow(SW_SHOW);
}


LRESULT CAgoraHQDlg::onHttpInviteStatusSuccess(WPARAM wParam, LPARAM lParam)
{
	KillTimer(EVENT_TIMER_INVITEREMOTE);
	onInviteCallBackAccept(wParam, lParam);
	return true;
}



///////////////////////////////////////////////////////////////////////////////
//OBS for ExtCapture
///////////////////////////////////////////////////////////////////////////////


#define UNSUPPORTED_ERROR \
	"Failed to initialize video:\n\nRequired graphics API functionality " \
	"not found.  Your GPU may not be supported."

#define UNKNOWN_ERROR \
	"Failed to initialize video.  Your GPU may not be supported, " \
	"or your graphics drivers may need to be updated."

#define STARTUP_SEPARATOR \
	"==== Startup complete ==============================================="
#define SHUTDOWN_SEPARATOR \
	"==== Shutting down =================================================="

static const double scaled_vals[] =
{
	1.0,
	1.25,
	(1.0 / 0.75),
	1.5,
	(1.0 / 0.6),
	1.75,
	2.0,
	2.25,
	2.5,
	2.75,
	3.0,
	0.0
};


static inline int AttemptToResetVideo(struct obs_video_info *ovi)
{
	return obs_reset_video(ovi);
}

static inline enum obs_scale_type GetScaleType(ConfigFile &basicConfig)
{
	const char *scaleTypeStr = config_get_string(basicConfig,
		"Video", "ScaleType");

	if (astrcmpi(scaleTypeStr, "bilinear") == 0)
		return OBS_SCALE_BILINEAR;
	else if (astrcmpi(scaleTypeStr, "lanczos") == 0)
		return OBS_SCALE_LANCZOS;
	else
		return OBS_SCALE_BICUBIC;
}

static inline enum video_format GetVideoFormatFromName(const char *name)
{
	if (astrcmpi(name, "I420") == 0)
		return VIDEO_FORMAT_I420;
	else if (astrcmpi(name, "NV12") == 0)
		return VIDEO_FORMAT_NV12;
	else if (astrcmpi(name, "I444") == 0)
		return VIDEO_FORMAT_I444;
#if 0 //currently unsupported
	else if (astrcmpi(name, "YVYU") == 0)
		return VIDEO_FORMAT_YVYU;
	else if (astrcmpi(name, "YUY2") == 0)
		return VIDEO_FORMAT_YUY2;
	else if (astrcmpi(name, "UYVY") == 0)
		return VIDEO_FORMAT_UYVY;
#endif
	else
		return VIDEO_FORMAT_RGBA;
}



std::string GetDefaultVideoSavePath()
{
	wchar_t path_utf16[MAX_PATH];
	char    path_utf8[MAX_PATH] = {};

	SHGetFolderPathW(NULL, CSIDL_MYVIDEO, NULL, SHGFP_TYPE_CURRENT,
		path_utf16);

	os_wcs_to_utf8(path_utf16, wcslen(path_utf16), path_utf8, MAX_PATH);
	return std::string(path_utf8);
}


static void AddExtraModulePaths()
{
	char base_module_dir[512];
#if defined(_WIN32) || defined(__APPLE__)
	int ret = GetProgramDataPath(base_module_dir, sizeof(base_module_dir),
		"obs-studio/plugins/%module%");
#else
	int ret = GetConfigPath(base_module_dir, sizeof(base_module_dir),
		"obs-studio/plugins/%module%");
#endif

	if (ret <= 0)
		return;

	std::string path = (char*)base_module_dir;
#if defined(__APPLE__)
	obs_add_module_path((path + "/bin").c_str(), (path + "/data").c_str());

	BPtr<char> config_bin = os_get_config_path_ptr("obs-studio/plugins/%module%/bin");
	BPtr<char> config_data = os_get_config_path_ptr("obs-studio/plugins/%module%/data");
	obs_add_module_path(config_bin, config_data);

#elif ARCH_BITS == 64
	obs_add_module_path((path + "/bin/64bit").c_str(),
		(path + "/data").c_str());
#else
	obs_add_module_path((path + "/bin/32bit").c_str(),
		(path + "/data").c_str());
#endif
}

uint32_t GetWindowsVersion()
{
	static uint32_t ver = 0;

	if (ver == 0) {
		struct win_version_info ver_info;

		get_win_ver(&ver_info);
		ver = (ver_info.major << 8) | ver_info.minor;
	}

	return ver;
}

void SetAeroEnabled(bool enable)
{
	static HRESULT(WINAPI *func)(UINT) = nullptr;
	static bool failed = false;

	if (!func) {
		if (failed) {
			return;
		}

		HMODULE dwm = LoadLibraryW(L"dwmapi");
		if (!dwm) {
			failed = true;
			return;
		}

		func = reinterpret_cast<decltype(func)>(GetProcAddress(dwm,
			"DwmEnableComposition"));
		if (!func) {
			failed = true;
			return;
		}
	}

	func(enable ? DWM_EC_ENABLECOMPOSITION : DWM_EC_DISABLECOMPOSITION);
}

static inline bool HasAudioDevices(const char *source_id)
{
	const char *output_id = source_id;
	obs_properties_t *props = obs_get_source_properties(output_id);
	size_t count = 0;

	if (!props)
		return false;

	obs_property_t *devices = obs_properties_get(props, "device_id");
	if (devices)
		count = obs_property_list_item_count(devices);

	obs_properties_destroy(props);

	return count != 0;
}

bool CAgoraHQDlg::InitService()
{
	// 	if (LoadService())
	// 		return true;

	//初始化service sbd
	obs_data_t* settings = obs_data_create();
	obs_data_set_default_string(settings, "type", "rtmp_custom");//sbd
	obs_data_set_default_string(settings, "server", "rtmp://vid-218.push.fastweb.broadcastapp.agora.io/live/");
	obs_data_set_default_string(settings, "key", "abcijkmnldefzyx12344321");

	service = obs_service_create("rtmp_custom", "default_service", settings,
		nullptr);//rtmp_common
	if (!service)
		return false;
	obs_service_release(service);

	return true;
}

bool CAgoraHQDlg::LoadService()
{
	const char *type;

	char serviceJsonPath[512];
	int ret = GetProfilePath(serviceJsonPath, sizeof(serviceJsonPath),
		SERVICE_PATH);//
	if (ret <= 0)
		return false;

	obs_data_t *data = obs_data_create_from_json_file_safe(serviceJsonPath,
		"bak");
	//add by sbd 设置默认推流信息
	if (data == nullptr)
	{
		std::string servicePath = "./" + app_name + "/service.json";
		data = obs_data_create_from_json_file_safe(servicePath.c_str(), "bak");
	}
	//设置默认的Stream信息
	obs_data_set_default_string(data, "type", "rtmp_custom");//sbd
	obs_data_set_default_string(data, "server", "rtmp://vid-218.push.fastweb.broadcastapp.agora.io/live/");//设置推流地址
	obs_data_set_default_string(data, "key", "abcijkmnldefzyx12344321");
	type = obs_data_get_string(data, "type");
	// end

	obs_data_t *settings = obs_data_get_obj(data, "settings");
	obs_data_t *hotkey_data = obs_data_get_obj(data, "hotkeys");
	service = obs_service_create(type, "default_service", settings,
		hotkey_data);
	//obs_hotkeys_save_service(service);//add by sbd
	obs_service_release(service);

	obs_data_release(hotkey_data);
	obs_data_release(settings);
	obs_data_release(data);

	return !!service;//非0值转换成1,而0值还是0。
}


bool CAgoraHQDlg::InitBasicConfigDefaults()
{
	RECT rcDesktop;
	GetDesktopWindow()->GetWindowRect(&rcDesktop);

	uint32_t cx = rcDesktop.right - rcDesktop.left;  //primaryScreen->size().width();
	uint32_t cy = rcDesktop.bottom - rcDesktop.top;//primaryScreen->size().height();

	bool oldResolutionDefaults = config_get_bool(theApp.GlobalConfig(),
		"General", "Pre19Defaults");

	/* use 1920x1080 for new default base res if main monitor is above
	* 1920x1080, but don't apply for people from older bFPSCommonuilds -- only to
	* new users */
	if (!oldResolutionDefaults && (cx * cy) > (1920 * 1080)) {
		cx = 1920;
		cy = 1080;
	}

	/* ----------------------------------------------------- */
	/* move over mixer values in advanced if older config */
	if (config_has_user_value(basicConfig, "AdvOut", "RecTrackIndex") &&
		!config_has_user_value(basicConfig, "AdvOut", "RecTracks")) {

		uint64_t track = config_get_uint(basicConfig, "AdvOut",
			"RecTrackIndex");
		track = 1ULL << (track - 1);
		config_set_uint(basicConfig, "AdvOut", "RecTracks", track);
		config_remove_value(basicConfig, "AdvOut", "RecTrackIndex");
		config_save_safe(basicConfig, "tmp", nullptr);
	}

	/* ----------------------------------------------------- */

	config_set_default_string(basicConfig, "Output", "Mode", "Simple");

	config_set_default_string(basicConfig, "SimpleOutput", "FilePath",
		GetDefaultVideoSavePath().c_str());
	config_set_default_string(basicConfig, "SimpleOutput", "RecFormat",
		"flv");
	config_set_default_uint(basicConfig, "SimpleOutput", "VBitrate",
		obs_videoBitrate);//2500


	config_set_string(basicConfig, "SimpleOutput", "StreamEncoder",
		"x264");

	config_set_default_string(basicConfig, "SimpleOutput", "StreamEncoder",
		SIMPLE_ENCODER_X264);
	config_set_default_uint(basicConfig, "SimpleOutput", "ABitrate", 160);
	config_set_default_bool(basicConfig, "SimpleOutput", "UseAdvanced",
		false);
	config_set_default_bool(basicConfig, "SimpleOutput", "EnforceBitrate",
		true);
	config_set_default_string(basicConfig, "SimpleOutput", "Preset",
		"veryfast");
	config_set_default_string(basicConfig, "SimpleOutput", "RecQuality",
		"Stream");
	config_set_default_string(basicConfig, "SimpleOutput", "RecEncoder",
		SIMPLE_ENCODER_X264);
	config_set_default_bool(basicConfig, "SimpleOutput", "RecRB", false);
	config_set_default_int(basicConfig, "SimpleOutput", "RecRBTime", 20);
	config_set_default_int(basicConfig, "SimpleOutput", "RecRBSize", 512);
	config_set_default_string(basicConfig, "SimpleOutput", "RecRBPrefix",
		"Replay");

	config_set_default_bool(basicConfig, "AdvOut", "ApplyServiceSettings",
		true);
	config_set_default_bool(basicConfig, "AdvOut", "UseRescale", false);
	config_set_default_uint(basicConfig, "AdvOut", "TrackIndex", 1);
	config_set_default_string(basicConfig, "AdvOut", "Encoder", "obs_x264");

	config_set_default_string(basicConfig, "AdvOut", "RecType", "Standard");

	config_set_default_string(basicConfig, "AdvOut", "RecFilePath",
		GetDefaultVideoSavePath().c_str());
	config_set_default_string(basicConfig, "AdvOut", "RecFormat", "flv");
	config_set_default_bool(basicConfig, "AdvOut", "RecUseRescale",
		false);
	config_set_default_uint(basicConfig, "AdvOut", "RecTracks", (1 << 0));
	config_set_default_string(basicConfig, "AdvOut", "RecEncoder",
		"none");

	config_set_default_bool(basicConfig, "AdvOut", "FFOutputToFile",
		true);
	config_set_default_string(basicConfig, "AdvOut", "FFFilePath",
		GetDefaultVideoSavePath().c_str());
	config_set_default_string(basicConfig, "AdvOut", "FFExtension", "mp4");

	config_set_default_uint(basicConfig, "AdvOut", "FFVBitrate", obs_videoBitrate);//768
	config_set_default_uint(basicConfig, "AdvOut", "FFVGOPSize", 250);//
	config_set_default_bool(basicConfig, "AdvOut", "FFUseRescale",
		false);
	config_set_default_bool(basicConfig, "AdvOut", "FFIgnoreCompat",
		false);
	config_set_default_uint(basicConfig, "AdvOut", "FFABitrate", 160);
	config_set_default_uint(basicConfig, "AdvOut", "FFAudioTrack", 1);

	config_set_default_uint(basicConfig, "AdvOut", "Track1Bitrate", 160);
	config_set_default_uint(basicConfig, "AdvOut", "Track2Bitrate", 160);
	config_set_default_uint(basicConfig, "AdvOut", "Track3Bitrate", 160);
	config_set_default_uint(basicConfig, "AdvOut", "Track4Bitrate", 160);
	config_set_default_uint(basicConfig, "AdvOut", "Track5Bitrate", 160);
	config_set_default_uint(basicConfig, "AdvOut", "Track6Bitrate", 160);

	config_set_default_bool(basicConfig, "AdvOut", "RecRB", false);
	config_set_default_uint(basicConfig, "AdvOut", "RecRBTime", 20);
	config_set_default_int(basicConfig, "AdvOut", "RecRBSize", 512);

	config_set_default_uint(basicConfig, "Video", "BaseCX", cx);
	config_set_default_uint(basicConfig, "Video", "BaseCY", cy);

	/* don't allow BaseCX/BaseCY to be susceptible to defaults changing */
	if (!config_has_user_value(basicConfig, "Video", "BaseCX") ||
		!config_has_user_value(basicConfig, "Video", "BaseCY")) {
		config_set_uint(basicConfig, "Video", "BaseCX", cx);
		config_set_uint(basicConfig, "Video", "BaseCY", cy);
		config_save_safe(basicConfig, "tmp", nullptr);
	}

	config_set_default_string(basicConfig, "Output", "FilenameFormatting",
		"%CCYY-%MM-%DD %hh-%mm-%ss");

	config_set_default_bool(basicConfig, "Output", "DelayEnable", false);
	config_set_default_uint(basicConfig, "Output", "DelaySec", 20);
	config_set_default_bool(basicConfig, "Output", "DelayPreserve", true);

	config_set_default_bool(basicConfig, "Output", "Reconnect", true);
	config_set_default_uint(basicConfig, "Output", "RetryDelay", 10);
	config_set_default_uint(basicConfig, "Output", "MaxRetries", 20);

	config_set_default_string(basicConfig, "Output", "BindIP", "default");
	config_set_default_bool(basicConfig, "Output", "NewSocketLoopEnable",
		false);
	config_set_default_bool(basicConfig, "Output", "LowLatencyEnable",
		false);

	int i = 0;
	uint32_t scale_cx = cx;
	uint32_t scale_cy = cy;

	/* use a default scaled resolution that has a pixel count no higher
	* than 1280x720 */
	while (((scale_cx * scale_cy) > (1280 * 720)) && scaled_vals[i] > 0.0) {
		double scale = scaled_vals[i++];
		scale_cx = uint32_t(double(cx) / scale);
		scale_cy = uint32_t(double(cy) / scale);
	}

	//设置输出分辨率int obs_output_x = 640;
	scale_cx = obs_output_x;
	scale_cy = obs_output_y;
	config_set_default_uint(basicConfig, "Video", "OutputCX", scale_cx);
	config_set_default_uint(basicConfig, "Video", "OutputCY", scale_cy);

	config_set_uint(basicConfig, "Video", "OutputCX", scale_cx);
	config_set_uint(basicConfig, "Video", "OutputCY", scale_cy);

	/* don't allow OutputCX/OutputCY to be susceptible to defaults
	* changing */
	if (!config_has_user_value(basicConfig, "Video", "OutputCX") ||
		!config_has_user_value(basicConfig, "Video", "OutputCY")) {
		config_set_uint(basicConfig, "Video", "OutputCX", scale_cx);
		config_set_uint(basicConfig, "Video", "OutputCY", scale_cy);
		config_save_safe(basicConfig, "tmp", nullptr);
	}

	config_set_default_uint(basicConfig, "Video", "FPSType", 0);
	config_set_default_string(basicConfig, "Video", "FPSCommon", obs_video_fps.c_str());//30
	config_set_default_uint(basicConfig, "Video", "FPSInt", obs_fps);
	config_set_default_uint(basicConfig, "Video", "FPSNum", obs_fps);
	config_set_default_uint(basicConfig, "Video", "FPSDen", 1);
	config_set_default_string(basicConfig, "Video", "ScaleType", "bicubic");
	config_set_default_string(basicConfig, "Video", "ColorFormat", "I420");//"NV12"
	config_set_default_string(basicConfig, "Video", "ColorSpace", "601");
	config_set_default_string(basicConfig, "Video", "ColorRange",
		"Partial");

	config_set_default_string(basicConfig, "Audio", "MonitoringDeviceId",
		"default");
	config_set_default_string(basicConfig, "Audio", "MonitoringDeviceName",
		Str("Basic.Settings.Advanced.Audio.MonitoringDevice"
		".Default"));
	config_set_default_uint(basicConfig, "Audio", "SampleRate", 44100);
	config_set_default_string(basicConfig, "Audio", "ChannelSetup",
		"Stereo");

	config_set_string(basicConfig, "Video", "FPSCommon", obs_video_fps.c_str());//30

	// 	config_set_default_double(basicConfig, "Audio", "MeterDecayRate",
	// 		VOLUME_METER_DECAY_FAST);

	return true;
}

bool CAgoraHQDlg::InitBasicConfig()
{
	char configPath[512];

	int ret = GetProfilePath(configPath, sizeof(configPath), "");
	if (ret <= 0) {
		//	OBSErrorBox(nullptr, "Failed to get profile path");
		return false;
	}

	if (os_mkdir(configPath) == MKDIR_ERROR) {
		//	OBSErrorBox(nullptr, "Failed to create profile path");
		return false;
	}

	ret = GetProfilePath(configPath, sizeof(configPath), "basic.ini");
	if (ret <= 0) {
		//	OBSErrorBox(nullptr, "Failed to get base.ini path");
		return false;
	}

	int code = basicConfig.Open(configPath, CONFIG_OPEN_ALWAYS);
	if (code != CONFIG_SUCCESS) {
		//	OBSErrorBox(NULL, "Failed to open basic.ini: %d", code);
		return false;
	}

	if (config_get_string(basicConfig, "General", "Name") == nullptr) {
		const char *curName = config_get_string(theApp.GlobalConfig(),
			"Basic", "Profile");

		config_set_string(basicConfig, "General", "Name", curName);
		basicConfig.SaveSafe("tmp");
	}

	return InitBasicConfigDefaults();
}

void CAgoraHQDlg::ResetOutputs()
{
	ProfileScope("OBSBasic::ResetOutputs");

	const char *mode = config_get_string(basicConfig, "Output", "Mode");
	bool advOut = astrcmpi(mode, "Advanced") == 0;

	if (!outputHandler || !outputHandler->Active()) {
		outputHandler.reset();
		outputHandler.reset(advOut ?
			CreateAdvancedOutputHandler(this) :
			CreateSimpleOutputHandler(this));



		if (outputHandler->replayBuffer) {
			//replayBuffer 按钮
		}

		//sysTray处理
	}
	else {
		outputHandler->Update();
	}
}

void CAgoraHQDlg::InitPrimitives()
{
	ProfileScope("OBSBasic::InitPrimitives");

	obs_enter_graphics();

	gs_render_start(true);
	gs_vertex2f(0.0f, 0.0f);
	gs_vertex2f(0.0f, 1.0f);
	gs_vertex2f(1.0f, 1.0f);
	gs_vertex2f(1.0f, 0.0f);
	gs_vertex2f(0.0f, 0.0f);
	box = gs_render_save();

	gs_render_start(true);
	gs_vertex2f(0.0f, 0.0f);
	gs_vertex2f(0.0f, 1.0f);
	boxLeft = gs_render_save();

	gs_render_start(true);
	gs_vertex2f(0.0f, 0.0f);
	gs_vertex2f(1.0f, 0.0f);
	boxTop = gs_render_save();

	gs_render_start(true);
	gs_vertex2f(1.0f, 0.0f);
	gs_vertex2f(1.0f, 1.0f);
	boxRight = gs_render_save();

	gs_render_start(true);
	gs_vertex2f(0.0f, 1.0f);
	gs_vertex2f(1.0f, 1.0f);
	boxBottom = gs_render_save();

	gs_render_start(true);
	for (int i = 0; i <= 360; i += (360 / 20)) {
		float pos = RAD(float(i));
		gs_vertex2f(cosf(pos), sinf(pos));
	}
	circle = gs_render_save();

	obs_leave_graphics();
}

void  CAgoraHQDlg::ClearSceneData()
{
	disableSaving++;

	obs_set_output_source(0, nullptr);
	obs_set_output_source(1, nullptr);
	obs_set_output_source(2, nullptr);
	obs_set_output_source(3, nullptr);
	obs_set_output_source(4, nullptr);
	obs_set_output_source(5, nullptr);


	auto cb = [](void *unused, obs_source_t *source)
	{
		obs_source_remove(source);
		UNUSED_PARAMETER(unused);
		return true;
	};

	obs_enum_sources(cb, nullptr);

	disableSaving--;
	blog(LOG_INFO, "All scene data cleared");
	blog(LOG_INFO, "------------------------------------------------");
}
//添加音频源
void  CAgoraHQDlg::CreateFirstRunSources()
{
	bool hasDesktopAudio = HasAudioDevices(theApp.OutputAudioSource());
	bool hasInputAudio = HasAudioDevices(theApp.InputAudioSource());

	if (hasDesktopAudio)
		ResetAudioDevice(theApp.OutputAudioSource(), "default",
		Str("Basic.DesktopDevice1"), 1);
	if (hasInputAudio)
		ResetAudioDevice(theApp.InputAudioSource(), "default",
		Str("Basic.AuxDevice1"), 3);
}

void CAgoraHQDlg::AddScene(OBSSource source)
{
	const char *name = obs_source_get_name(source);
	obs_scene_t *scene = obs_scene_from_source(source);

	default_scene = OBSScene(scene);
	// 	QListWidgetItem *item = new QListWidgetItem(QT_UTF8(name));
	// 	SetOBSRef(item, OBSScene(scene));
	// 	ui->scenes->addItem(item);

	obs_hotkey_register_source(source, "OBSBasic.SelectScene",
		Str("Basic.Hotkeys.SelectScene"),
		[](void *data,
		obs_hotkey_id, obs_hotkey_t*, bool pressed)
	{
		CAgoraHQDlg *main =
			reinterpret_cast<CAgoraHQDlg*>(theApp.GetMainWindow());

		auto potential_source = static_cast<obs_source_t*>(data);
		auto source = obs_source_get_ref(potential_source);
		if (source && pressed)
			main->SetCurrentScene(source);
		obs_source_release(source);
	}, static_cast<obs_source_t*>(source));

	signal_handler_t *handler = obs_source_get_signal_handler(source);

	SignalContainer<OBSScene> container;
	container.ref = scene;
	container.handlers.assign({
		std::make_shared<OBSSignal>(handler, "item_add",
		CAgoraHQDlg::SceneItemAdded, this),
		std::make_shared<OBSSignal>(handler, "item_remove",
		CAgoraHQDlg::SceneItemRemoved, this),
		std::make_shared<OBSSignal>(handler, "item_select",
		CAgoraHQDlg::SceneItemSelected, this),
		std::make_shared<OBSSignal>(handler, "item_deselect",
		CAgoraHQDlg::SceneItemDeselected, this),
		std::make_shared<OBSSignal>(handler, "reorder",
		CAgoraHQDlg::SceneReordered, this),
	});

	// 	item->setData(static_cast<int>(QtDataRole::OBSSignals),
	// 		QVariant::fromValue(container));

	/* if the scene already has items (a duplicated scene) add them */
	auto addSceneItem = [this](obs_sceneitem_t *item)
	{
		//AddSceneItem(item);

	};

	using addSceneItem_t = decltype(addSceneItem);

	obs_scene_enum_items(scene,
		[](obs_scene_t*, obs_sceneitem_t *item, void *param)
	{
		addSceneItem_t *func;
		func = reinterpret_cast<addSceneItem_t*>(param);
		(*func)(item);
		obs_sceneitem_select(item, true);
		return true;
	}, &addSceneItem);


	//SaveProject();

	// 	if (!disableSaving) {
	// 		obs_source_t *source = obs_scene_get_source(scene);
	// 		blog(LOG_INFO, "User added scene '%s'",
	// 			obs_source_get_name(source));
	// 
	// 		OBSProjector::UpdateMultiviewProjectors();
	// 	}
	// 
	// 	if (api)
	// 		api->on_event(OBS_FRONTEND_EVENT_SCENE_LIST_CHANGED);
}

OBSScene CAgoraHQDlg::GetCurrentScene()
{
	return default_scene;
}

OBSSceneItem CAgoraHQDlg::GetCurrentSceneItem()
{
	OBSSceneItem itemSelected = nullptr;
	//for (int i = 0; i < m_vecOBSSources.size(); ++i)
	for (auto iter = m_lstOBSSources.begin(); iter != m_lstOBSSources.end(); ++iter)
	{
		obs_sceneitem_t* item = obs_scene_find_source(default_scene, (*iter).c_str());
		if (!item) continue;
		if (obs_sceneitem_selected(item))
		{
			itemSelected = item;
			break;
		}
	}

	return itemSelected;
}

void CAgoraHQDlg::AddSource(const char* id)
{
	CSelectOBSSourceDlg dlgSel(id,NAME_TYPE_SOURCE, source_tile, source_info, this);
	dlgSel.DoModal();

	if (dlgSel.newSource)
	{
		bool b = false;
		if (strcmp(id, "image_source") == 0)
		{
			CImagePropertiesDlg dlgImage(dlgSel.newSource, this);
			dlgImage.SetVideoDialog(this);
			if (IDOK == dlgImage.DoModal())
				b = true;
		}
		else if (strcmp(id, "ffmpeg_source") == 0)
		{
			CFFmpegSourceDlg dlg(dlgSel.newSource, this);
			dlg.SetVideoDialog(this);
			if (IDOK == dlg.DoModal())
			{
				b = true;
			}

			::PostMessage(m_hWnd, WM_INVALIDATE_PREVIEW, 0, 0);
		}
		else if (strcmp(id, "dshow_input") == 0)
		{
			CDShowCameraDlg dlg(dlgSel.newSource, this);
			dlg.SetVideoDialog(this);
			if (IDOK == dlg.DoModal())
				b = true;
		}

		if (b)
			m_lstOBSSources.push_back(dlgSel.utf8_source_name);
	}
}

void CAgoraHQDlg::SetCurrentScene(obs_scene_t *scene, bool force, bool direct)
{
	obs_source_t *source = obs_scene_get_source(scene);
	SetCurrentScene(source, force, direct);
}

void CAgoraHQDlg::SetCurrentScene(OBSSource scene, bool force, bool direct)
{
	//	obs_set_output_source(0, obs_scene_get_source(default_scene));
	if (!IsPreviewProgramMode() && !direct) {
		TransitionToScene(scene, force, false);

	}
	else if (IsPreviewProgramMode() && direct) {
		TransitionToScene(scene, force, true);

	}
	else {
		OBSSource actualLastScene = OBSGetStrongRef(lastScene);
		if (actualLastScene != scene) {
			if (scene)
				obs_source_inc_showing(scene);
			if (actualLastScene)
				obs_source_dec_showing(actualLastScene);
			lastScene = OBSGetWeakRef(scene);
		}
	}

	UpdateSceneSelection(scene);
}

void CAgoraHQDlg::UpdateSceneSelection(OBSSource source)
{
	if (source) {
		obs_scene_t *scene = obs_scene_from_source(source);
		const char *name = obs_source_get_name(source);

		if (!scene)
			return;
	}
}


void CAgoraHQDlg::ResizePreview(uint32_t cx, uint32_t cy)
{
	bool isFixedScaling;
	obs_video_info ovi;

	/* resize preview panel to fix to the top section of the window */
	SIZE sz;
	sz = GetPreviewRect();

	isFixedScaling = IsPreviewFixedScaling();//ui->preview->IsFixedScaling();
	obs_get_video_info(&ovi);

	if (isFixedScaling) {
		previewScale = GetPreviewScalingAmount();
		CAgoraHQApp::GetCenterPosFromFixedScale(int(cx), int(cy),
			sz.cx - PREVIEW_EDGE_SIZE * 2,
			sz.cy - PREVIEW_EDGE_SIZE * 2,
			previewX, previewY, previewScale);
	}
	else {
		CAgoraHQApp::GetScaleAndCenterPos(int(cx), int(cy),
			sz.cx - PREVIEW_EDGE_SIZE * 2,
			sz.cy - PREVIEW_EDGE_SIZE * 2,
			previewX, previewY, previewScale);
	}

	previewX += float(PREVIEW_EDGE_SIZE);
	previewY += float(PREVIEW_EDGE_SIZE);
}


void CAgoraHQDlg::CreateDefaultScene(bool firstStart)
{
	disableSaving++;

	ClearSceneData();


	InitDefaultTransitions();
	//	CreateDefaultQuickTransitions();
	//	ui->transitionDuration->setValue(300);
	SetTransition(fadeTransition);

	//创建scene 唯一一个
	/*obs_scene_t*/
	obs_scene_t* scene = obs_scene_create(Str("Basic.Scene"));

	if (firstStart)
		CreateFirstRunSources();

	AddScene(obs_scene_get_source(scene));
	SetCurrentScene(scene, true);

	obs_scene_release(scene);

	disableSaving--;
}

bool CAgoraHQDlg::IsStreaming()
{
	return outputHandler->StreamingActive();
}

bool CAgoraHQDlg::StartStreaming()
{
	if (outputHandler->StreamingActive())
		return false;

	if (!outputHandler->StartStreaming(service)) {

		AfxMessageBox(_T("obs推流失败"));
		return false;
	}

	return true;
	// 	bool recordWhenStreaming = config_get_bool(GetGlobalConfig(),
	// 		"BasicWindow", "RecordWhenStreaming");
	// 	if (recordWhenStreaming)
	// 		StartRecording();
	// 
	// 	bool replayBufferWhileStreaming = config_get_bool(GetGlobalConfig(),
	// 		"BasicWindow", "ReplayBufferWhileStreaming");
	// 	if (replayBufferWhileStreaming)
	// 		StartReplayBuffer();
}

void CAgoraHQDlg::StopStreaming()
{
	if (outputHandler->StreamingActive())
		outputHandler->StopStreaming(streamingStopping);

}

void CAgoraHQDlg::Load(const char *file)
{
	disableSaving++;

	obs_data_t *data = obs_data_create_from_json_file_safe(file, "bak");
	if (!data) {
		disableSaving--;
		blog(LOG_INFO, "No scene file found, creating default scene");
		CreateDefaultScene(true);
		return;
	}
}


void CAgoraHQDlg::OBSInit()
{
	const char *sceneCollection = config_get_string(theApp.GlobalConfig(),
		"Basic", "SceneCollectionFile");
	char savePath[512];
	char fileName[512];
	int ret;

	if (!sceneCollection)
		throw "Failed to get scene collection name";
	std::string basic_scenes = app_name + "/basic/scenes/%s.json";
	ret = snprintf(fileName, 512, basic_scenes.c_str(),
		sceneCollection);
	if (ret <= 0)
		throw "Failed to create scene collection file name";

	ret = GetConfigPath(savePath, sizeof(savePath), fileName);
	if (ret <= 0)
		throw "Failed to get scene collection json file path";

	if (!InitBasicConfig())
	{
		throw "Failed to load basic.ini";
	}

	if (!ResetAudio())
	{
		AfxMessageBox(_T("fuck"));
		//throw "Failed to initialize audio";
	}
	ret = ResetVideo();

	switch (ret) {
	case OBS_VIDEO_MODULE_NOT_FOUND:
		throw "Failed to initialize video:  Graphics module not found";
	case OBS_VIDEO_NOT_SUPPORTED:
		throw UNSUPPORTED_ERROR;
	case OBS_VIDEO_INVALID_PARAM:
		throw "Failed to initialize video:  Invalid parameters";
	default:
		if (ret != OBS_VIDEO_SUCCESS)
			throw UNKNOWN_ERROR;
	}

	/* load audio monitoring */
#if defined(_WIN32) || defined(__APPLE__) || HAVE_PULSEAUDIO
	const char *device_name = config_get_string(basicConfig, "Audio",
		"MonitoringDeviceName");
	const char *device_id = config_get_string(basicConfig, "Audio",
		"MonitoringDeviceId");

	obs_set_audio_monitoring_device(device_name, device_id);

	blog(LOG_INFO, "Audio monitoring device:\n\tname: %s\n\tid: %s",
		device_name, device_id);
#endif
	//InitOBSCallbacks()

	AddExtraModulePaths();
	blog(LOG_INFO, "---------------------------------");
	obs_load_all_modules();
	blog(LOG_INFO, "---------------------------------");
	obs_log_loaded_modules();
	blog(LOG_INFO, "---------------------------------");
	obs_post_load_modules();

	CheckForSimpleModeX264Fallback();

	blog(LOG_INFO, STARTUP_SEPARATOR);

	//ResetOutputs();//del for test

	if (!InitService())
	{
		//throw "Failed to initialize service";
	}
	InitPrimitives();
	ResetAgoraOutput();
	InitAgoraService();

	// 	sceneDuplicationMode = config_get_bool(App()->GlobalConfig(),
	// 		"BasicWindow", "SceneDuplicationMode");
	// 	swapScenesMode = config_get_bool(App()->GlobalConfig(),
	// 		"BasicWindow", "SwapScenesMode");
	// 	editPropertiesMode = config_get_bool(App()->GlobalConfig(),
	// 		"BasicWindow", "EditPropertiesMode");
	// 
	// 	if (!opt_studio_mode) {
	// 		SetPreviewProgramMode(config_get_bool(App()->GlobalConfig(),
	// 			"BasicWindow", "PreviewProgramMode"));
	// 	}
	// 	else {
	// 		SetPreviewProgramMode(true);
	// 		opt_studio_mode = false;
	// 	}

	// #define SET_VISIBILITY(name, control) \
		// 	do { \
		// 		if (config_has_user_value(App()->GlobalConfig(), \
		// 					"BasicWindow", name)) { \
		// 			bool visible = config_get_bool(App()->GlobalConfig(), \
		// 					"BasicWindow", name); \
		// 			ui->control->setChecked(visible); \
		// 						} \
		// 			} while (false)
	// 
	// 	SET_VISIBILITY("ShowListboxToolbars", toggleListboxToolbars);
	// 	SET_VISIBILITY("ShowStatusBar", toggleStatusBar);
	// #undef SET_VISIBILITY

	{
		disableSaving--;
		//Load(savePath);//没有加载的就创建默认的scene

		CreateDefaultScene(true);
		disableSaving++;
	}

	loaded = true;
#ifdef _WIN32
	uint32_t winVer = GetWindowsVersion();
	if (winVer > 0 && winVer < 0x602) {
		bool disableAero = config_get_bool(basicConfig, "Video",
			"DisableAero");
		if (disableAero) {
			SetAeroEnabled(true);
		}
	}
#endif

	//RefreshSceneCollections();
	//RefreshProfiles();


}

void CAgoraHQDlg::DrawBackdrop(float cx, float cy)
{
	if (!box)
		return;

	gs_effect_t    *solid = obs_get_base_effect(OBS_EFFECT_SOLID);
	gs_eparam_t    *color = gs_effect_get_param_by_name(solid, "color");
	gs_technique_t *tech = gs_effect_get_technique(solid, "Solid");

	vec4 colorVal;
	vec4_set(&colorVal, 0.0f, 0.0f, 0.0f, 1.0f);
	gs_effect_set_vec4(color, &colorVal);

	gs_technique_begin(tech);
	gs_technique_begin_pass(tech, 0);
	gs_matrix_push();
	gs_matrix_identity();
	gs_matrix_scale3f(float(cx), float(cy), 1.0f);

	gs_load_vertexbuffer(box);
	gs_draw(GS_TRISTRIP, 0, 0);

	gs_matrix_pop();
	gs_technique_end_pass(tech);
	gs_technique_end(tech);

	gs_load_vertexbuffer(nullptr);
}

config_t *CAgoraHQDlg::Config() const
{
	return basicConfig;
}

void CAgoraHQDlg::CheckForSimpleModeX264Fallback()
{
	const char *curStreamEncoder = config_get_string(basicConfig,
		"SimpleOutput", "StreamEncoder");
	const char *curRecEncoder = config_get_string(basicConfig,
		"SimpleOutput", "RecEncoder");
	bool qsv_supported = false;
	bool amd_supported = false;
	bool nve_supported = false;
	bool changed = false;
	size_t idx = 0;
	const char *id;

	while (obs_enum_encoder_types(idx++, &id)) {
		if (strcmp(id, "amd_amf_h264") == 0)
			amd_supported = true;
		else if (strcmp(id, "obs_qsv11") == 0)
			qsv_supported = true;
		else if (strcmp(id, "ffmpeg_nvenc") == 0)
			nve_supported = true;
	}

	auto CheckEncoder = [&](const char *&name)
	{
		if (strcmp(name, SIMPLE_ENCODER_QSV) == 0) {
			if (!qsv_supported) {
				changed = true;
				name = SIMPLE_ENCODER_X264;
				return false;
			}
		}
		else if (strcmp(name, SIMPLE_ENCODER_NVENC) == 0) {
			if (!nve_supported) {
				changed = true;
				name = SIMPLE_ENCODER_X264;
				return false;
			}
		}
		else if (strcmp(name, SIMPLE_ENCODER_AMD) == 0) {
			if (!amd_supported) {
				changed = true;
				name = SIMPLE_ENCODER_X264;
				return false;
			}
		}

		return true;
	};

	if (!CheckEncoder(curStreamEncoder))
		config_set_string(basicConfig,
		"SimpleOutput", "StreamEncoder",
		curStreamEncoder);
	if (!CheckEncoder(curRecEncoder))
		config_set_string(basicConfig,
		"SimpleOutput", "RecEncoder",
		curRecEncoder);
	if (changed)
		config_save_safe(basicConfig, "tmp", nullptr);
}


void CAgoraHQDlg::GetFPSCommon(uint32_t &num, uint32_t &den) const
{
	const char *val = config_get_string(basicConfig, "Video", "FPSCommon");

	if (strcmp(val, "10") == 0) {
		num = 10;
		den = 1;
	}
	else if (strcmp(val, "20") == 0) {//add by agora
		num = 15;
		den = 1;
	}
	else if (strcmp(val, "20") == 0) {
		num = 20;
		den = 1;
	}
	else if (strcmp(val, "24 NTSC") == 0) {
		num = 24000;
		den = 1001;
	}
	else if (strcmp(val, "25") == 0) {
		num = 25;
		den = 1;
	}
	else if (strcmp(val, "29.97") == 0) {
		num = 30000;
		den = 1001;
	}
	else if (strcmp(val, "48") == 0) {
		num = 48;
		den = 1;
	}
	else if (strcmp(val, "59.94") == 0) {
		num = 60000;
		den = 1001;
	}
	else if (strcmp(val, "60") == 0) {
		num = 60;
		den = 1;
	}
	else {
		num = 30;
		den = 1;
	}
}

void CAgoraHQDlg::GetFPSInteger(uint32_t &num, uint32_t &den) const
{
	num = (uint32_t)config_get_uint(basicConfig, "Video", "FPSInt");
	den = 1;
}

void CAgoraHQDlg::GetFPSFraction(uint32_t &num, uint32_t &den) const
{
	num = (uint32_t)config_get_uint(basicConfig, "Video", "FPSNum");
	den = (uint32_t)config_get_uint(basicConfig, "Video", "FPSDen");
}

void CAgoraHQDlg::GetFPSNanoseconds(uint32_t &num, uint32_t &den) const
{
	num = 1000000000;
	den = (uint32_t)config_get_uint(basicConfig, "Video", "FPSNS");
}

void CAgoraHQDlg::GetConfigFPS(uint32_t &num, uint32_t &den) const
{
	uint32_t type = config_get_uint(basicConfig, "Video", "FPSType");

	if (type == 1) //"Integer"
		GetFPSInteger(num, den);
	else if (type == 2) //"Fraction"
		GetFPSFraction(num, den);
	else if (false) //"Nanoseconds", currently not implemented
		GetFPSNanoseconds(num, den);
	else
		GetFPSCommon(num, den);
}


obs_service_t *CAgoraHQDlg::GetService()
{
	if (!service) {//rtmp_common mod by sbd
		service = obs_service_create("rtmp_custom", NULL, NULL,
			nullptr);
		obs_service_release(service);
	}
	return service;
}
void CAgoraHQDlg::SetService(obs_service_t *newService)
{
	if (newService)
		service = newService;
}
int CAgoraHQDlg::GetProfilePath(char *path, size_t size, const char *file) const
{
	char profiles_path[512];
	const char *profile = config_get_string(theApp.GlobalConfig(),
		"Basic", "ProfileDir");
	int ret;

	if (!profile)
		return -1;
	if (!path)
		return -1;
	if (!file)
		file = "";
	std::string basic_profiles = app_name + "/basic/profiles";
	ret = GetConfigPath(profiles_path, 512, basic_profiles.c_str());
	if (ret <= 0)
		return ret;

	if (!*file)
		return snprintf(path, size, "%s/%s", profiles_path, profile);

	return snprintf(path, size, "%s/%s/%s", profiles_path, profile, file);
}

int CAgoraHQDlg::ResetVideo()
{
	// del for test
	// 	if (outputHandler && outputHandler->Active())
	// 		return OBS_VIDEO_CURRENTLY_ACTIVE;

	if (agoraOutputHandler && agoraOutputHandler->Active())
		return OBS_VIDEO_CURRENTLY_ACTIVE;

	struct obs_video_info ovi;
	int ret;

	GetConfigFPS(ovi.fps_num, ovi.fps_den);

	const char *colorFormat = config_get_string(basicConfig, "Video",
		"ColorFormat");
	const char *colorSpace = config_get_string(basicConfig, "Video",
		"ColorSpace");
	const char *colorRange = config_get_string(basicConfig, "Video",
		"ColorRange");

	ovi.graphics_module = theApp.GetRenderModule();
	ovi.base_width = (uint32_t)config_get_uint(basicConfig,
		"Video", "BaseCX");
	ovi.base_height = (uint32_t)config_get_uint(basicConfig,
		"Video", "BaseCY");
	ovi.output_width = (uint32_t)config_get_uint(basicConfig,
		"Video", "OutputCX");
	ovi.output_height = (uint32_t)config_get_uint(basicConfig,
		"Video", "OutputCY");
	ovi.output_format = GetVideoFormatFromName(colorFormat);
	ovi.colorspace = astrcmpi(colorSpace, "601") == 0 ?
	VIDEO_CS_601 : VIDEO_CS_709;
	ovi.range = astrcmpi(colorRange, "Full") == 0 ?
	VIDEO_RANGE_FULL : VIDEO_RANGE_PARTIAL;
	ovi.adapter = config_get_uint(theApp.GlobalConfig(),
		"Video", "AdapterIdx");
	ovi.gpu_conversion = true;
	ovi.scale_type = GetScaleType(basicConfig);

	if (ovi.base_width == 0 || ovi.base_height == 0) {
		ovi.base_width = 1920;
		ovi.base_height = 1080;
		config_set_uint(basicConfig, "Video", "BaseCX", 1920);
		config_set_uint(basicConfig, "Video", "BaseCY", 1080);
	}

	if (ovi.output_width == 0 || ovi.output_height == 0) {
		ovi.output_width = ovi.base_width;
		ovi.output_height = ovi.base_height;
		config_set_uint(basicConfig, "Video", "OutputCX",
			ovi.base_width);
		config_set_uint(basicConfig, "Video", "OutputCY",
			ovi.base_height);
	}

	ret = AttemptToResetVideo(&ovi);
	if (ret != OBS_VIDEO_SUCCESS) {
		if (ret == OBS_VIDEO_CURRENTLY_ACTIVE) {
			blog(LOG_WARNING, "Tried to reset when "
				"already active");
			return ret;
		}

		/* Try OpenGL if DirectX fails on windows */
		if (astrcmpi(ovi.graphics_module, DL_OPENGL) != 0) {
			blog(LOG_WARNING, "Failed to initialize obs video (%d) "
				"with graphics_module='%s', retrying "
				"with graphics_module='%s'",
				ret, ovi.graphics_module,
				DL_OPENGL);
			ovi.graphics_module = DL_OPENGL;
			ret = AttemptToResetVideo(&ovi);
		}
	}
	return 0;
}
bool CAgoraHQDlg::ResetAudio()
{
	struct obs_audio_info ai;
	ai.samples_per_sec = config_get_uint(basicConfig, "Audio",
		"SampleRate");

	const char *channelSetupStr = config_get_string(basicConfig,
		"Audio", "ChannelSetup");

	if (strcmp(channelSetupStr, "Mono") == 0)
		ai.speakers = SPEAKERS_MONO;
	else if (strcmp(channelSetupStr, "2.1") == 0)
		ai.speakers = SPEAKERS_2POINT1;
	// 	else if (strcmp(channelSetupStr, "4.0") == 0)
	// 		ai.speakers = SPEAKERS_4POINT0;
	else if (strcmp(channelSetupStr, "4.1") == 0)
		ai.speakers = SPEAKERS_4POINT1;
	else if (strcmp(channelSetupStr, "5.1") == 0)
		ai.speakers = SPEAKERS_5POINT1;
	else if (strcmp(channelSetupStr, "7.1") == 0)
		ai.speakers = SPEAKERS_7POINT1;
	else
		ai.speakers = SPEAKERS_STEREO;

	return obs_reset_audio(&ai);
}

void CAgoraHQDlg::ResetAudioDevice(const char *sourceId, const char *deviceId,
	const char *deviceDesc, int channel)
{
	bool disable = deviceId && strcmp(deviceId, "disabled") == 0;
	obs_source_t *source;
	obs_data_t *settings;

	source = obs_get_output_source(channel);
	if (source) {
		if (disable) {
			obs_set_output_source(channel, nullptr);
		}
		else {
			settings = obs_source_get_settings(source);
			const char *oldId = obs_data_get_string(settings,
				"device_id");
			if (strcmp(oldId, deviceId) != 0) {
				obs_data_set_string(settings, "device_id",
					deviceId);
				obs_source_update(source, settings);
			}
			obs_data_release(settings);
		}

		obs_source_release(source);

	}
	else if (!disable) {
		settings = obs_data_create();
		obs_data_set_string(settings, "device_id", deviceId);
		source = obs_source_create(sourceId, deviceDesc, settings,
			nullptr);
		obs_data_release(settings);

		obs_set_output_source(channel, source);
		obs_source_release(source);
	}
}

void CAgoraHQDlg::SceneReordered(void *data, calldata_t *params)
{

}

void CAgoraHQDlg::SceneItemAdded(void *data, calldata_t *params)
{
	obs_sceneitem_t *item = (obs_sceneitem_t*)calldata_ptr(params, "item");
}

void CAgoraHQDlg::SceneItemRemoved(void *data, calldata_t *params)
{}

void CAgoraHQDlg::SceneItemSelected(void *data, calldata_t *params)
{}

void CAgoraHQDlg::SceneItemDeselected(void *data, calldata_t *params)
{

}

void CAgoraHQDlg::SourceLoaded(void *data, obs_source_t *source)
{}

void CAgoraHQDlg::SourceRemoved(void *data, calldata_t *params)
{}

void CAgoraHQDlg::SourceActivated(void *data, calldata_t *params)
{}

void CAgoraHQDlg::SourceDeactivated(void *data, calldata_t *params)
{}

void CAgoraHQDlg::SourceRenamed(void *data, calldata_t *params)
{

}

void CAgoraHQDlg::InitDefaultTransitions()
{
	std::vector<OBSSource> transitions;
	size_t idx = 0;
	const char *id;

	/* automatically add transitions that have no configuration (things
	* such as cut/fade/etc) */
	while (obs_enum_transition_types(idx++, &id)) {
		if (!obs_is_source_configurable(id)) {
			const char *name = obs_source_get_display_name(id);

			obs_source_t *tr = obs_source_create_private(
				id, name, NULL);
			InitTransition(tr);
			transitions.emplace_back(tr);

			if (strcmp(id, "fade_transition") == 0)
				fadeTransition = tr;

			obs_source_release(tr);
		}
	}

	for (OBSSource &tr : transitions) {
		// 		ui->transitions->addItem(QT_UTF8(obs_source_get_name(tr)),
		// 				QVariant::fromValue(OBSSource(tr)));
		SetTransition(tr);
	}
}

void CAgoraHQDlg::InitTransition(obs_source_t *transition)
{
	auto onTransitionStop = [](void *data, calldata_t*) {
		CAgoraHQDlg *window = (CAgoraHQDlg*)data;
		// 		QMetaObject::invokeMethod(window, "TransitionStopped",
		// 				Qt::QueuedConnection);
	};

	auto onTransitionFullStop = [](void *data, calldata_t*) {
		CAgoraHQDlg *window = (CAgoraHQDlg*)data;
		// 		QMetaObject::invokeMethod(window, "TransitionFullyStopped",
		// 				Qt::QueuedConnection);
	};

	signal_handler_t *handler = obs_source_get_signal_handler(transition);
	signal_handler_connect(handler, "transition_video_stop",
		onTransitionStop, this);
	signal_handler_connect(handler, "transition_stop",
		onTransitionFullStop, this);
}

void CAgoraHQDlg::TransitionToScene(OBSScene scene, bool force, bool direct)
{
	obs_source_t *source = obs_scene_get_source(scene);
	TransitionToScene(source, force, direct);
}


void CAgoraHQDlg::TransitionToScene(OBSSource source, bool force, bool direct,
	bool quickTransition)
{
	obs_scene_t *scene = obs_scene_from_source(source);
	//bool usingPreviewProgram = IsPreviewProgramMode();
	if (!scene)
		return;

	OBSSource transition = obs_get_output_source(0);
	obs_source_release(transition);

	if (force) {
		obs_transition_set(transition, source);
	}
}

static void OverrideTransition(OBSSource transition)
{
	obs_source_t *oldTransition = obs_get_output_source(0);

	if (transition != oldTransition) {
		obs_transition_swap_begin(transition, oldTransition);
		obs_set_output_source(0, transition);
		obs_transition_swap_end(transition, oldTransition);
	}

	obs_source_release(oldTransition);
}

void CAgoraHQDlg::TransitionFullyStopped()
{
}

//设置主窗口渲染
void CAgoraHQDlg::SetTransition(OBSSource transition)
{
	obs_source_t *oldTransition = obs_get_output_source(0);

	if (oldTransition && transition) {
		obs_transition_swap_begin(transition, oldTransition);
		obs_set_output_source(0, transition);
		obs_transition_swap_end(transition, oldTransition);
	}
	else
	{
		obs_set_output_source(0, transition);
	}

	if (oldTransition)
		obs_source_release(oldTransition);
}


/////////////////////////////////////////////////////////////////////////////////////////////////
//OBS for ExtCapture

// //开始推流
// void CAgoraHQDlg::OnBtnClickedStart()
// {
// 	CAgoraExternalCaptureDlg* dlgParent = static_cast<CAgoraExternalCaptureDlg*>(GetParent());
// 	if (!dlgParent->IsStreaming() && dlgParent->StartStreaming())
// 	{
// 		m_btnOBSStartStreaming.SetWindowText(_T("Stop"));
// 	}
// 	else if (dlgParent->IsStreaming())
// 	{
// 		m_btnOBSStartStreaming.SetWindowText(_T("Start"));
// 		dlgParent->StopStreaming();
// 	}
// }
// 
SIZE CAgoraHQDlg::GetPreviewRect()
{
	RECT rc;
	SIZE sz = { 0, 0 };
	if (m_wndLocal.GetSafeHwnd() != NULL)
	{
		m_wndLocal.GetWindowRect(&rc);
		sz.cx = rc.right - rc.left;
		sz.cy = rc.bottom - rc.top;
	}
	return sz;
}

// 
// void CAgoraHQDlg::OnDestroy()
// {
// 	obs_display_remove_draw_callback(m_wndLocal.GetDisplay(), RenderMain, this);
// 	CDialogEx::OnDestroy();
// 	// TODO: Add your message handler code here
// }
// 
void CAgoraHQDlg::RenderMain(void *data, uint32_t cx, uint32_t cy)
{
	CAgoraHQDlg* window = static_cast<CAgoraHQDlg*>(data);
	if (window == nullptr || window->m_hWnd == NULL || !window->IsWindowVisible())
		return;
	obs_video_info ovi;

	obs_get_video_info(&ovi);
	//window->previewScale = 1.0f;
	window->previewCX = int(window->previewScale * float(ovi.base_width));
	window->previewCY = int(window->previewScale * float(ovi.base_height));

	gs_viewport_push();
	gs_projection_push();

	/* --------------------------------------- */

	gs_ortho(0.0f, float(ovi.base_width), 0.0f, float(ovi.base_height),
		-100.0f, 100.0f);
	gs_set_viewport(window->previewX, window->previewY,
		window->previewCX, window->previewCY);

	//window->DrawBackdrop(float(ovi.base_width), float(ovi.base_height));
	//预览串流
	if (window->IsPreviewProgramMode()) {
		OBSScene scene = window->GetCurrentScene();
		obs_source_t *source = obs_scene_get_source(scene);
		if (source)
			obs_source_video_render(source);
	}
	else {
		obs_render_main_texture();//obs_render_main_view();20.0
	}
	gs_load_vertexbuffer(nullptr);

	/* --------------------------------------- */


	//QSize previewSize = GetPixelSize(window->ui->preview);
	SIZE previewSize = window->GetPreviewRect();
	float right = float(previewSize.cx) - window->previewX;
	float bottom = float(previewSize.cy) - window->previewY;

	gs_ortho(-window->previewX, right,
		-window->previewY, bottom,
		-100.0f, 100.0f);
	gs_reset_viewport();

	window->DrawSceneEditing();
	//window->ui->preview->DrawSceneEditing();

	/* --------------------------------------- */

	gs_projection_pop();
	gs_viewport_pop();

	UNUSED_PARAMETER(cx);
	UNUSED_PARAMETER(cy);
}

//end

void CAgoraHQDlg::OnBnClickedButtonObsImage()
{
	AddSource("image_source");
}


void CAgoraHQDlg::OnBnClickedButtonObsCamera()
{
	AddSource("dshow_input");
}

void CAgoraHQDlg::OnBnClickedButtonObsFfmpegMedia()
{
	AddSource("ffmpeg_source");
}


void CAgoraHQDlg::OnClickedCheckObs()
{
	bool bEnableOBS = m_chkOBS.GetCheck();
	if (!bEnableOBS && m_lstOBSSources.size() > 0 && 
		IDOK == MessageBox(_T("If unchecked OBS, the OBS source will be deleted. Are you sure you want to uncheck OBS."), NULL, MB_OKCANCEL)){
		RemoveAllSource();
	}
	EnableOBSCtrl(bEnableOBS);
	obs_display_set_enabled(m_wndLocal.GetDisplay(), bEnableOBS);
	m_ctlShowPic.ShowWindow(!bEnableOBS);
	CRect rc;
	m_ctlShowPic.GetWindowRect(&rc);
	ScreenToClient(rc);
	m_wndLocal.MoveWindow(&rc);
	m_wndLocal.ShowWindow(bEnableOBS);
	CAgoraObject::GetAgoraObject()->EnableExtendAudioCapture(bEnableOBS, &m_exCapAudioFrameObserver);
	CAgoraObject::GetAgoraObject()->EnableExtendVideoCapture(bEnableOBS, &m_exCapVideoFrameObserver);
}


void CAgoraHQDlg::RemoveAllSource()
{
	OBSScene scene = GetCurrentScene();
	if (scene) {
		obs_source_t *source = obs_scene_get_source(scene);

		//for (int i = 0; i < m_vecOBSSources.size(); ++i)
		for (auto iter = m_lstOBSSources.begin(); iter != m_lstOBSSources.end(); ++iter)
		{
			obs_sceneitem_t* item = obs_scene_find_source(scene, (*iter).c_str());
			if (!item) continue;
			obs_source_t *source = obs_sceneitem_get_source(item);
			if (!source) continue;
			obs_sceneitem_remove(item);
		}
		m_lstOBSSources.clear();
	}
}


void CAgoraHQDlg::RemoveSelectedSceneItem()
{
	//	for (int i = 0; i < m_vecOBSSources.size(); ++i)
	for (auto iter = m_lstOBSSources.begin(); iter != m_lstOBSSources.end(); ++iter)
	{
		std::string source_name = *iter;
		obs_sceneitem_t* item = obs_scene_find_source(default_scene, source_name.c_str());
		if (!item) continue;
		if (obs_sceneitem_selected(item))
		{
			wchar_t* wcs = NULL;
			os_utf8_to_wcs_ptr(source_name.c_str(), source_name.length(), &wcs);
			CString strMessage;
			strMessage.Format(_T("Are you sure you wish to remove \'%s\'"), wcs);
			if (IDOK == MessageBox(strMessage, NULL, MB_OKCANCEL))
			{
				obs_sceneitem_remove(item);
				m_lstOBSSources.remove(source_name);
				break;
			}
			else
				return;
		}
	}
}

void CAgoraHQDlg::JoinChannel_OBS()
{
	if (NULL == m_pDlgConfig){
		m_pDlgConfig = new CDlgConfig;
		m_pDlgConfig->Create(CDlgConfig::IDD);
	}
	m_lpAgoraObject->EnableLastmileTest(FALSE);
	m_lpAgoraObject->SetClientRole(CLIENT_ROLE_TYPE::CLIENT_ROLE_BROADCASTER);
	m_lpAgoraObject->SetChannelProfile(TRUE);
	m_uId = str2int(gHQConfig.getLoginUid());

	m_strChannelName = gHQConfig.getChannelName();
	std::string strAppCertificateEnable = gHQConfig.getAppCertEnable();
	std::string strAppcertificatId = gHQConfig.getAppCertificateId();
	m_lpAgoraObject->SetSelfUID(m_uId);
	m_lpAgoraObject->SetAppCert(s2cs(strAppcertificatId));

	CAgoraObject::GetAgoraObject()->SetAudioProfile(44100, 2, 2*1024);
	CAgoraObject::GetAgoraObject()->SetVideoProfileEx(obs_output_x, obs_output_y, obs_fps, obs_videoBitrate);
	BITMAPINFOHEADER bmi = { 0 };
	bmi.biHeight = obs_output_y;
	bmi.biWidth = obs_output_x;
	CVideoPackageQueue::GetInstance()->SetVideoFormat(&bmi);

	SetWindowText(s2cs(m_strChannelName));

	config_set_string(basicConfig, "Video", "ColorFormat", agoraColorFormat.c_str());
	ResetVideo();

	agoraOutputHandler->SetEncoderCallback(CAgoraHQDlg::obsVideoCallback, CAgoraHQDlg::obsAudioCallback);
	if (!agoraOutputHandler->StartAgora(agoraService)){

	}

	if ("0" == strAppCertificateEnable || "" == strAppCertificateEnable){

		m_lpAgoraObject->JoinChannel(s2cs(m_strChannelName), m_uId);
	}
	else if ("1" == strAppCertificateEnable){

		CStringA strMediaChannelKey = m_lpAgoraObject->getDynamicMediaChannelKey(s2cs(m_strChannelName));
		m_lpAgoraObject->JoinChannel(s2cs(m_strChannelName), m_uId, strMediaChannelKey);
	}
	m_pDlgAnswer->joinchannel();
}

void CAgoraHQDlg::LeaveChannel_OBS()
{
	if (agoraOutputHandler->AgoraActive()){
		agoraOutputHandler->StopAgora();
		ResetVideo();
	}
	m_pDlgAnswer->leaveChannel();
	m_lpAgoraObject->LeaveCahnnel();
}
////////////////////////////////////////////////////////////////////////////////
//agora output and agora service                                              //
////////////////////////////////////////////////////////////////////////////////
void CAgoraHQDlg::InitAgoraService()
{
	ProfileScope("OBSBasic::InitAgoraService");
	if (!GetAgoraService()){
		blog(LOG_WARNING, "Init agora service Failed");
		return;
	}
	InitAgoraServiceSettings();
}
obs_service_t* CAgoraHQDlg::GetAgoraService()
{
	if (!agoraService){
		agoraService = obs_service_create("agora_service", NULL, nullptr, nullptr);
		obs_service_release(agoraService);
	}
	return agoraService;
}

void CAgoraHQDlg::SetAgoraService(obs_service_t* service)
{
	if (service)
		agoraService = service;
}

void CAgoraHQDlg::ResetAgoraOutput()
{
	ProfileScope("CAgoraHQDlg::ResetAgoraOutput");
	if (!agoraOutputHandler || !agoraOutputHandler->Active()){
		agoraOutputHandler.reset();
		agoraOutputHandler.reset(CreateAgoraOutputHandler(this));
	}
	else{
		agoraOutputHandler->Update();
	}
}

void CAgoraHQDlg::InitAgoraServiceSettings()
{
	obs_data_t* settings = obs_service_get_settings(agoraService);
	std::string rtmpcustom = "rtmp_custonm";
	if (rtmpcustom.compare(obs_service_get_type(service)) == 0)
	{
		obs_data_set_string(settings, "agora_url", obs_service_get_url(service));
		obs_data_set_string(settings, "agora_key", obs_service_get_key(service));
	}

	obs_data_set_int(settings, "agora_video_bitrate", 2400);

	int out_cx = config_get_uint(basicConfig, "Video", "OutputCX");
	int out_cy = config_get_uint(basicConfig, "Video", "OutputCY");
	loacal_uid = 234;
	obs_data_set_int(settings, "agora_out_cx", out_cx);
	obs_data_set_int(settings, "agora_out_cy", out_cy);

	obs_data_set_int(settings, "fps", 15);
	obs_data_set_int(settings, "agora_uid", loacal_uid);
	obs_data_set_string(settings, "agora_channel", "lny321");

	obs_service_update(agoraService, settings);
}

void CAgoraHQDlg::obsVideoCallback(uint8_t* data, void* param)
{
	CAgoraHQDlg* pAgoraHQDlg = static_cast<CAgoraHQDlg*>(param);
	CVideoPackageQueue *lpPackageQueue = CVideoPackageQueue::GetInstance();
	
	int out_x = pAgoraHQDlg->obs_output_x;
	int out_y = pAgoraHQDlg->obs_output_y;

	if (lpPackageQueue->GetBufferSize() <  static_cast<SIZE_T>(out_x*out_y))
		return;

	lpPackageQueue->PushVideoPackage(data, out_y*out_x*3/2);
}

void CAgoraHQDlg::obsAudioCallback(struct encoder_frame* data, int planes, void* param)
{
	CAgoraHQDlg* pAgoraHQDlg = static_cast<CAgoraHQDlg*>(param);
	DWORD dwBytesToWrite = data->frames*planes;
	DWORD dwBytesWritten = 0;
	pAgoraHQDlg->m_exCapAudioFrameObserver.pCircleBuffer->writeBuffer(data->data[0], dwBytesToWrite);
}