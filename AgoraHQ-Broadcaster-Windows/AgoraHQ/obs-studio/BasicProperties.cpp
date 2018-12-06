// BasicProperties.cpp : implementation file
//

#include "stdafx.h"
#include "AgoraHQ.h"
#include "AgoraHQDlg.h"
#include "BasicProperties.h"
#include "afxdialogex.h"



// CBasicProperties dialog

IMPLEMENT_DYNAMIC(CBasicProperties, CDialogEx)

CBasicProperties::CBasicProperties(UINT nIDTemplate, OBSSource _source, CWnd* pParent /*=NULL*/, bool _bFilter)
: CDialogEx(nIDTemplate, pParent)
, preview(new CDisplaySourceWnd)
, source(_source)
, removedSignal(obs_source_get_signal_handler(source), "remove", CBasicProperties::SourceRemoved, this)
, renamedSignal(obs_source_get_signal_handler(source), "rename", CBasicProperties::SourceRenamed, this)
, properties(nullptr, obs_properties_destroy)
, oldSettings(obs_data_create())
, bFilter(_bFilter)
{
	int cx = (int)config_get_int(theApp.GlobalConfig(), "PropertiesWindow",
		"cx");
	int cy = (int)config_get_int(theApp.GlobalConfig(), "PropertiesWindow",
		"cy");

	main = reinterpret_cast<CAgoraHQDlg*>(pParent);

	/* The OBSData constructor increments the reference once */
	obs_data_release(oldSettings);

	settings = obs_source_get_settings(source);
	obs_data_apply(oldSettings, settings);
	obs_data_release(settings);

	obs_source_inc_showing(source);

	reloadCallback = (PropertiesReloadCallback)obs_source_properties;
	callback = (PropertiesUpdateCallback)obs_source_update;
	obj = source;
}

CBasicProperties::~CBasicProperties()
{
	obs_source_dec_showing(source);
}

void CBasicProperties::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CBasicProperties, CDialogEx)
	ON_WM_DESTROY()
END_MESSAGE_MAP()

void CBasicProperties::OnDestroy()
{
	Cleanup();
	CDialogEx::OnDestroy();
}

BOOL CBasicProperties::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	const char *name = obs_source_get_name(source);
	wchar_t* wcs;
	os_utf8_to_wcs_ptr(name, strlen(name)*sizeof(wchar_t), &wcs);

	CString strTitle;
	strTitle.Format(_T("属性 %s"), wcs);
	SetWindowText(strTitle);

	CRect	rcClient;
	GetClientRect(&rcClient);

	int width = rcClient.right - rcClient.left;
	int height = rcClient.bottom - rcClient.top;
	if (!bFilter){
		preview->SetReceiveWnd(m_hWnd);
		preview->Create(NULL, NULL, WS_CHILD | WS_VISIBLE | WS_BORDER, 
			CRect(10, 10, width - 20, height - 10 - m_dist_vertical), this, IDC_BASEWND_VIDEO + 100);
		preview->ShowWindow(SW_SHOW);
	}


	ReloadProperties();
	return TRUE;
}

// CBasicProperties message handlers
void CBasicProperties::SourceRemoved(void *data, calldata_t *params)
{}

void CBasicProperties::SourceRenamed(void *data, calldata_t *params)
{
}

void CBasicProperties::UpdateProperties(void *data, calldata_t *params)
{
}

int CBasicProperties::CheckSettings()
{
	OBSData currentSettings = obs_source_get_settings(source);
	const char *oldSettingsJson = obs_data_get_json(oldSettings);
	const char *currentSettingsJson = obs_data_get_json(currentSettings);

	int ret = strcmp(currentSettingsJson, oldSettingsJson);

	obs_data_release(currentSettings);
	return ret;
}


void CBasicProperties::ReloadProperties()
{
	if (source) {
		properties.reset(reloadCallback(source));
	}
	else {
		obs_data_t *settings = obs_source_get_settings(source);
		properties.reset(reloadCallback((void*)type.c_str()));
		obs_properties_apply_settings(properties.get(), settings);
	}

	uint32_t flags = obs_properties_get_flags(properties.get());
	deferUpdate = (flags & OBS_PROPERTIES_DEFER_UPDATE) != 0;

	RefreshProperties();
}

void CBasicProperties::RefreshProperties()
{
	obs_property_t *property = obs_properties_first(properties.get());
	while (property) {
		AddProperty(property);
		obs_property_next(&property);
	}
}

void CBasicProperties::AddProperty(obs_property_t *property)
{
	const char        *name = obs_property_name(property);
	obs_property_type type = obs_property_get_type(property);
	m_mapProperties[name] = property;
	if (!obs_property_visible(property))
		return;
	OBSData settings = obs_source_get_settings(source);
	const char  *val = obs_data_get_string(settings, name);
}

void CBasicProperties::OnClickCancel()
{
	obs_data_t *settings = obs_source_get_settings(source);
	obs_data_clear(settings);
	obs_data_release(settings);

// 	if (view->DeferUpdate())
// 		obs_data_apply(settings, oldSettings);
// 	else
// 		obs_source_update(source, oldSettings);
}
void CBasicProperties::OnClickDefault()
{
	obs_data_t *settings = obs_source_get_settings(source);
	obs_data_clear(settings);
	obs_data_release(settings);
}

void CBasicProperties::DrawPreview(void *data, uint32_t cx, uint32_t cy)
{
	CBasicProperties *window = static_cast<CBasicProperties*>(data);
	if (!window->source)
		return;

	uint32_t sourceCX = max(obs_source_get_width(window->source), 1u);
	uint32_t sourceCY = max(obs_source_get_height(window->source), 1u);

	int   x, y;
	int   newCX, newCY;
	float scale;


	CAgoraHQApp::GetScaleAndCenterPos(sourceCX, sourceCY, cx, cy, x, y, scale);

	newCX = int(scale * float(sourceCX));
	newCY = int(scale * float(sourceCY));


	gs_viewport_push();
	gs_projection_push();
	gs_ortho(0.0f, float(sourceCX), 0.0f, float(sourceCY),
		-100.0f, 100.0f);
	gs_set_viewport(x, y, newCX, newCY);

	obs_source_video_render(window->source);

	gs_projection_pop();
	gs_viewport_pop();
}

void CBasicProperties::Cleanup()
{
	RECT rcClient;
	GetWindowRect(&rcClient);
	config_set_int(theApp.GlobalConfig(), "PropertiesWindow", "cx",
		rcClient.right - rcClient.left);
	config_set_int(theApp.GlobalConfig(), "PropertiesWindow", "cy",
		rcClient.bottom - rcClient.top);
	if (!bFilter)
		obs_display_remove_draw_callback(preview->GetDisplay(),
		CBasicProperties::DrawPreview, this);
}

BOOL CBasicProperties::OnWndMsg(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pResult)
{
	if (WM_CREATE_PREVIEW == message)//创建preview
	{ 
		obs_display_add_draw_callback(preview->GetDisplay(), 
			CBasicProperties::DrawPreview, this);
	}
	return CDialogEx::OnWndMsg(message, wParam, lParam, pResult);
}

void UpdateFilter()
{

}