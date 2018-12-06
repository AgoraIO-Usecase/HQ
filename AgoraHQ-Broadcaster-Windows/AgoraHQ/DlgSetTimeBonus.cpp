// DlgSetTimeBonus.cpp : 实现文件
//

#include "stdafx.h"
#include "AgoraHQ.h"
#include "DlgSetTimeBonus.h"
#include "afxdialogex.h"
#include "commonFun.h"
#include "SignalInstance.h"

// CDlgSetTimeBonus 对话框

IMPLEMENT_DYNAMIC(CDlgSetTimeBonus, CDialogEx)

CDlgSetTimeBonus::CDlgSetTimeBonus(CWnd* pParent /*=NULL*/)
	: CDialogEx(CDlgSetTimeBonus::IDD, pParent)
{

}

CDlgSetTimeBonus::~CDlgSetTimeBonus()
{
}

void CDlgSetTimeBonus::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_DATETIMEPICKER_NextGameTime, m_DateItmeCtrl_Time);
	DDX_Control(pDX, IDC_EDIT_Bonus, m_edBonus);
	DDX_Control(pDX, IDC_EDIT_RoundID, m_edRoundId);
}


BEGIN_MESSAGE_MAP(CDlgSetTimeBonus, CDialogEx)
	ON_WM_PAINT()
END_MESSAGE_MAP()


// CDlgSetTimeBonus 消息处理程序
BOOL CDlgSetTimeBonus::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	//TO DO
	initCtrl();

	return true;
}

void CDlgSetTimeBonus::initCtrl()
{
	m_DateItmeCtrl_Time.SetFormat(_T("yyyy-MM-dd HH:mm:ss"));
	CTime timeTemp;
	m_DateItmeCtrl_Time.SetTime(&CTime::GetCurrentTime());

	m_edRoundId.SetWindowTextW(_T("1"));
	m_edBonus.SetWindowTextW(_T("1000"));
}

void CDlgSetTimeBonus::uninitCtrl()
{

}

BOOL CDlgSetTimeBonus::PreTranslateMessage(MSG* pMsg)
{
	if (VK_RETURN == pMsg->wParam && WM_KEYDOWN == pMsg->message){
		
		LPAG_SetDataTimeBonus lpData = new AG_SetDataTimeBonus;
		if (lpData){
			CString strDataTime, strData, strTime;
			CTime tmTemp;
			m_DateItmeCtrl_Time.GetTime(tmTemp);
			strData = tmTemp.Format(_T("%Y-%m-%d"));
			strTime = tmTemp.Format(_T("%H:%M:%S"));
			strDataTime.Format(_T("%s %s"),strData,strTime);

			lpData->strDataTime = cs2s(strDataTime);
			CString strParam;
			m_edRoundId.GetWindowTextW(strParam);
			lpData->nRoundId = str2int(cs2s(strParam));
			m_edBonus.GetWindowTextW(strParam);
			lpData->nBonus = str2int(cs2s(strParam));

			HWND hDestWnd = CAgoraSignalInstance::getSignalInstance()->getLoginWnd();
			::PostMessage(hDestWnd, WM_SetDataTimeBonus, (WPARAM)lpData, NULL);
		}
	}
	else if (VK_ESCAPE == pMsg->wParam && WM_KEYDOWN == pMsg->message){
		ShowWindow(SW_HIDE);
	}

	return CDialogEx::PreTranslateMessage(pMsg);
}

void CDlgSetTimeBonus::OnPaint()
{
	CDialogEx::OnPaint();
}