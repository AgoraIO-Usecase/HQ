// DlgInput.cpp : 实现文件
//

#include "stdafx.h"
#include "AgoraHQ.h"
#include "DlgInput.h"
#include "afxdialogex.h"
#include "commonFun.h"
#include "SignalInstance.h"


// CDlgInput 对话框

IMPLEMENT_DYNAMIC(CDlgInput, CDialogEx)

CDlgInput::CDlgInput(CWnd* pParent /*=NULL*/)
	: CDialogEx(CDlgInput::IDD, pParent)
{

}

CDlgInput::~CDlgInput()
{
}

void CDlgInput::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_INPUT_PARAM, m_edInviteAccount);
	DDX_Control(pDX, IDC_CHECK_EnableVideo, m_btckEnableVideo);
	DDX_Control(pDX, IDC_CHECK_EnableAudio, m_btckEnableAudio);
	DDX_Control(pDX, IDC_EDIT_TimeOut, m_edTimeOut);
}

void CDlgInput::showWindow(eTagMsgtype type)
{
	m_type = type;
	CenterWindow();
	ShowWindow(SW_SHOW);
}


BEGIN_MESSAGE_MAP(CDlgInput, CDialogEx)
END_MESSAGE_MAP()


// CDlgInput 消息处理程序
BOOL CDlgInput::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	
	m_btckEnableAudio.SetCheck(TRUE);
	m_btckEnableVideo.SetCheck(TRUE);

	m_edTimeOut.SetWindowTextW(_T("30000"));

	return TRUE;
}


BOOL CDlgInput::PreTranslateMessage(MSG* pMsg)
{	
	if ( VK_RETURN == pMsg->wParam && WM_KEYDOWN == pMsg->message){

		CString InputParam;
		m_edInviteAccount.GetWindowTextW(InputParam);
		if (_T("") == InputParam){

			AfxMessageBox(_T("输入为空"));
		}
		else{

			PAG_INPUTPARAM lpData = new AG_INPUTPARAM;
			lpData->strParam = cs2s(InputParam);
			lpData->type = m_type;
			HWND MsgWnd = CAgoraSignalInstance::getSignalInstance()->getLoginWnd();
			::PostMessage(MsgWnd, WM_UpdateInputParam, (WPARAM)lpData, NULL);

			PAG_INVITE_REMOTEAUDIENCE lpData1= new AG_INVITE_REMOTEAUDIENCE;
			lpData1->enableAudio = m_btckEnableAudio.GetCheck();
			lpData1->enableVideo = m_btckEnableVideo.GetCheck();
			lpData1->remoteAccount = cs2s(InputParam);
			m_edTimeOut.GetWindowTextW(InputParam);
			lpData1->nTimeOut = str2int(cs2s(InputParam));
			lpData1->isAccpet = false;
			MsgWnd = CAgoraObject::GetAgoraObject()->GetMsgHandlerWnd();
			::PostMessage(MsgWnd,WM_InviteRemoteAudience,(WPARAM)lpData1,NULL);
		}
	}
	else if (pMsg->hwnd == m_hWnd && VK_ESCAPE == pMsg->wParam){
		ShowWindow(SW_HIDE);
	}

	return CDialogEx::PreTranslateMessage(pMsg);
}