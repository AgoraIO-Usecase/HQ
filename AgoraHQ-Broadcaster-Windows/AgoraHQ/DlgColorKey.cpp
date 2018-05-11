// DlgColorKey.cpp : implementation file
//

#include "stdafx.h"
#include "AgoraHQ.h"
#include "DlgColorKey.h"
#include "afxdialogex.h"


// CDlgColorKey dialog

IMPLEMENT_DYNAMIC(CDlgColorKey, CDialogEx)

CDlgColorKey::CDlgColorKey(CWnd* pParent /*=NULL*/)
	: CDialogEx(CDlgColorKey::IDD, pParent)
{

}

CDlgColorKey::~CDlgColorKey()
{
}

void CDlgColorKey::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDlgColorKey, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON_SELECT_COLOR, &CDlgColorKey::OnBnClickedButtonSelectColor)
END_MESSAGE_MAP()


// CDlgColorKey message handlers


BOOL CDlgColorKey::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  Add extra initialization here

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}


void CDlgColorKey::OnBnClickedButtonSelectColor()
{
	// TODO: Add your control notification handler code here
}
