
// AgoraHQDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "AgoraHQ.h"
#include "AgoraHQDlg.h"
#include "afxdialogex.h"
#include "commonFun.h"
#include "DlgAnswer.h"
#include "DlgConfig.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


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
	m_nLastmileQuality(-1)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CAgoraHQDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STATIC_SHOWPIC, m_ctlShowPic);
	DDX_Control(pDX, IDC_STATIC_ANSWERAREA, m_ctlAnswerArea);
	DDX_Control(pDX, IDC_BUTTON_JOINCHANNEL, m_btnJoinChannel);
	DDX_Control(pDX, IDC_BUTTON_MEDIA_PARAM, m_btnMediaParam);
	DDX_Control(pDX, IDC_BUTTON_HQ_CONFIG, m_btnHQConfig);
}

BEGIN_MESSAGE_MAP(CAgoraHQDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_CLOSE()
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
	ON_MESSAGE(WM_MSGID(EID_USER_JOINED), onUserJoined)
	ON_MESSAGE(WM_MSGID(EID_USER_OFFLINE), onUserOff)
	ON_MESSAGE(WM_MSGID(EID_CONNECTION_LOST), onConnectionLost)
	ON_MESSAGE((WM_NewChannelName), onNewChannelName)
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

	gHQConfig.setAppId("363f97d9690e4c0a9780499ab14a16c0");
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

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
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
		
		m_lpAgoraObject->MuteAllRemoteVideo(TRUE);
		m_lpAgoraObject->MuteAllRemoteAudio(TRUE);

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

	m_btnJoinChannel.SetWindowTextW(_T("LeaveChannel"));

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
	return TRUE;
}

LRESULT CAgoraHQDlg::onUserJoined(WPARAM wParam, LPARAM lParam)
{
	return TRUE;
}

LRESULT CAgoraHQDlg::onUserOff(WPARAM wParam, LPARAM lParam)
{
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