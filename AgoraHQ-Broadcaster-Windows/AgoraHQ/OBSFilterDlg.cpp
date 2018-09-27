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
	, addSignal(obs_source_get_signal_handler(source)
	, "filter_add"
	, COBSFilterDlg::OBSSourceFilterAdded,this)
	, removeSignal(obs_source_get_signal_handler(source)
	,"filter_remove"
	,COBSFilterDlg::OBSSourceFilterRemoved,this)
	,reorderSignal(obs_source_get_signal_handler(source)
	,"reorder_filters"
	,COBSFilterDlg::OBSSourceReordered,this)
	,removeSourceSignal(obs_source_get_signal_handler(source)
	,"remove"
	,COBSFilterDlg::SourceRemoved, this)
	,renameSourceSignal(obs_source_get_signal_handler(source)
	,"rename"
	,COBSFilterDlg::SourceRenamed, this)
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
	DDX_Control(pDX, IDC_TREE_FILTER_EFFECT, m_filterTreeView);
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

	m_staPreview.GetClientRect(rcPreview);
	m_staPreview.MapWindowPoints(this, rcPreview);

	preview->SetReceiveWnd(m_hWnd);
	preview->Create(NULL, NULL, WS_CHILD | WS_VISIBLE | WS_BORDER, rcPreview, this, IDC_BASEWND_VIDEO + 101);
	preview->ShowWindow(SW_SHOW);

	size_t idx = 0;
	const char *type_str;

	while (obs_enum_filter_types(idx++, &type_str)) {
		const char *name = obs_source_get_display_name(type_str);
		uint32_t caps = obs_get_source_output_flags(type_str);

		if ((caps & OBS_SOURCE_DEPRECATED) != 0)
			continue;
		if ((caps & OBS_SOURCE_CAP_DISABLED) != 0)
			continue;

		auto it = filter_types.begin();
		for (; it != filter_types.end(); ++it) {
			if (it->name >= name)
				break;
		}

		filter_types.emplace(it, type_str, name);
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
	if (WM_CREATE_PREVIEW == message){
		obs_display_add_draw_callback(preview->GetDisplay(), COBSFilterDlg::DrawPreview, this);
	}
	else if (WM_OBS_ADD_FILTER == message){
		obs_source_t* filter = (obs_source_t*)wParam;

		obs_source_properties(filter);
		if (nullptr == m_pDlgColorKey){
			m_pDlgColorKey = new CDlgColorKey(filter,this);
			m_pDlgColorKey->Create(CDlgColorKey::IDD, this);
			m_pDlgColorKey->MoveWindow(rcPreview.left, rcPreview.bottom + 5,
				rcPreview.right - rcPreview.left, rcPreview.bottom - rcPreview.top);
			//m_pDlgColorKey->ShowWindow(SW_SHOW);
		}
		else{

		}

	}
	return CDialogEx::OnWndMsg(message, wParam, lParam, pResult);
}

void COBSFilterDlg::OnBnClickedButtonAddFilter()
{
	AddNewFilter("color_key_filter");
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

void COBSFilterDlg::AddNewFilter(const char *id)
{
	if (id && *id) {
		obs_source_t *existing_filter;
		std::string name = obs_source_get_display_name(id);

		CSelectOBSSourceDlg dlg("Color Key", NAME_TYPE_FILTER, filter_tile, filter_info, GetParent(), name);
		if (IDOK != dlg.DoModal()) return;
		
		if (dlg.filter_name.IsEmpty()) {
			AfxMessageBox(_T("You can not use empty names"));
			AddNewFilter(id);
			return;
		}
		name = dlg.utf8_filter_name;
		existing_filter = obs_source_get_filter_by_name(_source,
			name.c_str());
		if (existing_filter) {
			AfxMessageBox(_T("The name is already in use"));
			obs_source_release(existing_filter);
			AddNewFilter(id);
			return;
		}
		name = dlg.utf8_filter_name;
		obs_source_t *filter = obs_source_create(id, name.c_str(),
			nullptr, nullptr);
		if (filter) {
			const char *sourceName = obs_source_get_name(_source);

			blog(LOG_INFO, "User added filter '%s' (%s) "
				"to source '%s'",
				name.c_str(), id, sourceName);

			obs_source_filter_add(_source, filter);
			DWORD dwErr = WSAGetLastError();
			obs_source_release(filter);


			TVINSERTSTRUCT tvInsert;
			tvInsert.hParent = NULL;
			tvInsert.hInsertAfter = NULL;
			tvInsert.item.mask = TVIF_TEXT;
			tvInsert.item.pszText = dlg.filter_name.GetBuffer(0);

			HTREEITEM hCountry = m_filterTreeView.InsertItem(&tvInsert);
			m_filterTreeView.SelectItem(hCountry);
			m_vecFilters.emplace_back(filter);
			if (!m_pDlgColorKey->IsWindowVisible()){
				m_pDlgColorKey->ShowWindow(SW_SHOW);
			}
		}
	}
}

/* OBS Signals */

void COBSFilterDlg::OBSSourceFilterAdded(void *param, calldata_t *data)
{
	COBSFilterDlg *window = reinterpret_cast<COBSFilterDlg*>(param);
	obs_source_t *filter = (obs_source_t*)calldata_ptr(data, "filter");

	::SendMessage(window->GetSafeHwnd(), WM_OBS_ADD_FILTER, (WPARAM)filter, 0);
// 	QMetaObject::invokeMethod(window, "AddFilter",
// 		Q_ARG(OBSSource, OBSSource(filter)));
}

void COBSFilterDlg::OBSSourceFilterRemoved(void *param, calldata_t *data)
{
	COBSFilterDlg *window = reinterpret_cast<COBSFilterDlg*>(param);
	obs_source_t *filter = (obs_source_t*)calldata_ptr(data, "filter");

// 	QMetaObject::invokeMethod(window, "RemoveFilter",
// 		Q_ARG(OBSSource, OBSSource(filter)));
}

void COBSFilterDlg::OBSSourceReordered(void *param, calldata_t *data)
{
// 	QMetaObject::invokeMethod(reinterpret_cast<OBSBasicFilters*>(param),
// 		"ReorderFilters");

	UNUSED_PARAMETER(data);
}

void COBSFilterDlg::SourceRemoved(void *data, calldata_t *params)
{
	UNUSED_PARAMETER(params);

// 	QMetaObject::invokeMethod(static_cast<OBSBasicFilters*>(data),
// 		"close");
}

void COBSFilterDlg::SourceRenamed(void *data, calldata_t *params)
{
	const char *name = calldata_string(params, "new_name");
//	QString title = QTStr("Basic.Filters.Title").arg(QT_UTF8(name));

// 	QMetaObject::invokeMethod(static_cast<OBSBasicFilters*>(data),
// 		"setWindowTitle", Q_ARG(QString, title));
}
