// OBSFilterDlg.cpp : implementation file
//

#include "stdafx.h"
#include "AgoraHQ.h"
#include "OBSFilterDlg.h"
#include "afxdialogex.h"
#include "SelectOBSSourceDlg.h"
#include <stdlib.h>
#include "DlgColorKey.h"

// COBSFilterDlg dialog

IMPLEMENT_DYNAMIC(COBSFilterDlg, CDialogEx)

COBSFilterDlg::COBSFilterDlg(OBSSource source,CWnd* pParent /*=NULL*/)
	: CDialogEx(COBSFilterDlg::IDD, pParent)
	, _source(source)
	, preview(new CDisplaySourceWnd)
{
	
}

COBSFilterDlg::~COBSFilterDlg()
{
	obs_display_remove_draw_callback(preview->GetDisplay(),
		COBSFilterDlg::DrawPreview, this);
}

void COBSFilterDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STATIC_SOURCE2, m_staPreview);
}


BEGIN_MESSAGE_MAP(COBSFilterDlg, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON_ADD_FILTER, &COBSFilterDlg::OnBnClickedButtonAddFilter)
	ON_BN_CLICKED(IDC_BUTTON_DEL_FILTER, &COBSFilterDlg::OnBnClickedButtonDelFilter)
	ON_BN_CLICKED(IDOK, &COBSFilterDlg::OnBnClickedOk)
END_MESSAGE_MAP()


// COBSFilterDlg message handlers

BOOL COBSFilterDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  Add extra initialization here
	enum obs_source_type type = obs_source_get_type(_source);
	uint32_t caps = obs_source_get_output_flags(_source);
	bool drawable_type = type == OBS_SOURCE_TYPE_INPUT ||
		type == OBS_SOURCE_TYPE_SCENE;
	CRect rcPreview;
	m_staPreview.GetClientRect(rcPreview);
	m_staPreview.MapWindowPoints(this, rcPreview);

	preview->SetReceiveWnd(m_hWnd);
	preview->Create(NULL, NULL, WS_CHILD | WS_VISIBLE | WS_BORDER, rcPreview, this, IDC_BASEWND_VIDEO + 101);
	preview->ShowWindow(SW_SHOW);

	if (nullptr == m_pDlgColorKey){
		m_pDlgColorKey = new CDlgColorKey(this);
		//m_pDlgColorKey->m_pAgoraHQDlg = this;
		//CRect rc = { 200, 340, 800, 600 };
		m_pDlgColorKey->Create(CDlgColorKey::IDD, this);
		m_pDlgColorKey->MoveWindow(rcPreview.left, rcPreview.bottom+5,
			rcPreview.right-rcPreview.left, rcPreview.bottom-rcPreview.top);//CRect(257, 340, 930, 300)
		m_pDlgColorKey->ShowWindow(SW_SHOW);
	}
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}


void COBSFilterDlg::DrawPreview(void *data, uint32_t cx, uint32_t cy)
{
	COBSFilterDlg *window = static_cast<COBSFilterDlg*>(data);

	if (!window->_source)
		return;
	
	uint32_t sourceCX = max(obs_source_get_width(window->_source), 1u);
	uint32_t sourceCY = max(obs_source_get_height(window->_source), 1u);

	int   x, y;
	int   newCX, newCY;
	float scale;

	CAgoraHQApp::GetScaleAndCenterPos(sourceCX, sourceCY, cx, cy, x, y, scale);

	newCX = int(scale * float(sourceCX));
	newCY = int(scale * float(sourceCY));

	gs_viewport_push();
	gs_projection_push();
	gs_ortho(0.0f, float(sourceCX), 0.0f, float(sourceCY), -100.0f, 100.0f);
	gs_set_viewport(x, y, newCX, newCY);

	obs_source_video_render(window->_source);

	gs_projection_pop();
	gs_viewport_pop();
}

BOOL COBSFilterDlg::OnWndMsg(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pResult)
{
	if (WM_CREATE_PREVIEW == message)//´´½¨preview
	{
		obs_display_add_draw_callback(preview->GetDisplay(), COBSFilterDlg::DrawPreview, this);
	}
	return CDialogEx::OnWndMsg(message, wParam, lParam, pResult);
}

void COBSFilterDlg::OnBnClickedButtonAddFilter()
{
	CSelectOBSSourceDlg dlg("Color Key", NAME_TYPE_FILTER, filter_tile, filter_info, GetParent());
	dlg.DoModal();
	
}


void COBSFilterDlg::OnBnClickedButtonDelFilter()
{
	// TODO: Add your control notification handler code here
}


void COBSFilterDlg::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	CDialogEx::OnOK();
}
