// ChromaFilterDlg.cpp : implementation file
//

#include "stdafx.h"
#include "AgoraHQ.h"
#include "ChromaFilterDlg.h"
#include "afxdialogex.h"
#include "obs-filter-def.h"
// CChromaFilterDlg dialog

IMPLEMENT_DYNAMIC(CChromaFilterDlg, CDialogEx)

CChromaFilterDlg::CChromaFilterDlg(OBSSource source, CWnd* pParent /*=NULL*/)
	: CDialogEx(CChromaFilterDlg::IDD, pParent)
	, preview(new CDisplaySourceWnd)
	, _source(source)
	, addSignal(obs_source_get_signal_handler(source)
	, "filter_add"
	, CChromaFilterDlg::OBSSourceFilterAdded, this)
    , properties(nullptr, obs_properties_destroy)
	, oldSettings(obs_data_create())
{
	reloadCallback = (PropertiesReloadCallback)obs_source_properties;
	callback = (PropertiesUpdateCallback)obs_source_update;
	
}

CChromaFilterDlg::~CChromaFilterDlg()
{
	obs_display_remove_draw_callback(preview->GetDisplay(),
		CChromaFilterDlg::DrawPreview, this);
}

void CChromaFilterDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STATIC_PREVIEW, m_staPreview);
	DDX_Control(pDX, IDC_COMBO_KEY_COLOR_TYPE, m_cmbKeyColorType);
	DDX_Control(pDX, IDC_BUTTON_ADD_CHROMA_FILTER, m_btnAddFilter);
	DDX_Control(pDX, IDC_BUTTON_DEL_CHROMA_FILTER, m_btnDelFilter);
}


BEGIN_MESSAGE_MAP(CChromaFilterDlg, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON2, &CChromaFilterDlg::OnBnClickedButton2)
	ON_BN_CLICKED(IDC_BUTTON1, &CChromaFilterDlg::OnBnClickedButton1)
	ON_CBN_SELCHANGE(IDC_COMBO_KEY_COLOR_TYPE, &CChromaFilterDlg::OnSelchangeComboKeyColorType)
	ON_BN_CLICKED(IDC_BUTTON_DEL_CHROMA_FILTER, &CChromaFilterDlg::OnBnClickedButtonDelChromaFilter)
	ON_BN_CLICKED(IDC_BUTTON_ADD_CHROMA_FILTER, &CChromaFilterDlg::OnBnClickedButtonAddChromaFilter)
END_MESSAGE_MAP()


// CChromaFilterDlg message handlers


void CChromaFilterDlg::OnBnClickedButton2()
{
	settings = obs_source_get_settings(_filter);
	obs_data_set_int(settings, SETTING_OPACITY, 100);
	obs_data_set_double(settings, SETTING_CONTRAST, 0.0);
	obs_data_set_double(settings, SETTING_BRIGHTNESS, 0.0);
	obs_data_set_double(settings, SETTING_GAMMA, 0.0);
	obs_data_set_int(settings, SETTING_KEY_COLOR, 0x00FF00);
	obs_data_set_string(settings, SETTING_COLOR_TYPE, "green");
	obs_data_set_int(settings, SETTING_SIMILARITY, 400);
	obs_data_set_int(settings, SETTING_SMOOTHNESS, 80);
	obs_data_set_int(settings, SETTING_SPILL, 100);
	obs_source_update(_filter, settings);
	m_cmbKeyColorType.SetCurSel(m_mapChromaColorType["green"]);
}


BOOL CChromaFilterDlg::OnWndMsg(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pResult)
{
	if (WM_CREATE_PREVIEW == message){
		obs_display_add_draw_callback(preview->GetDisplay(), CChromaFilterDlg::DrawPreview, this);
	}
	else if (WM_OBS_ADD_FILTER == message){
		obs_source_t* filter = (obs_source_t*)wParam;
		obs_source_properties(filter);

		ReloadProperties(filter);
	 	settings = obs_source_get_settings(filter);
	}
	else if (WM_OBS_DEL_CHROM_FILTER == message){
		if (_filter && IDOK ==
			AfxMessageBox(_T("Are you sure you want to remove \"Chroma Key\""), MB_OKCANCEL))
		{
			obs_source_filter_remove(_source, _filter);
			settings = obs_source_get_settings(_filter);
			m_cmbKeyColorType.ResetContent();
			m_btnAddFilter.EnableWindow(TRUE);
		}
	}
	return CDialogEx::OnWndMsg(message, wParam, lParam, pResult);
}

void CChromaFilterDlg::ReloadProperties(OBSSource source)
{
	if (source) {
		properties.reset(reloadCallback(source));
	}
	else {
		obs_data_t *settings = obs_source_get_settings(source);
		properties.reset(reloadCallback((void*)""));
		obs_properties_apply_settings(properties.get(), settings);
	}

	uint32_t flags = obs_properties_get_flags(properties.get());
	deferUpdate = (flags & OBS_PROPERTIES_DEFER_UPDATE) != 0;

	RefreshProperties();
}

void CChromaFilterDlg::_setChromaVal(obs_property_t *property)
{
	const char        *name = obs_property_name(property);
	obs_property_type type = obs_property_get_type(property);

	if (strcmp(name, SETTING_COLOR_TYPE) == 0){
		size_t           count = obs_property_list_item_count(property);
		for (size_t i = 0; i < count; i++){
			const char *name = obs_property_list_item_name(property, i);
			if (strcmp(name, "Custom") != 0){
				wchar_t wcs[20] = { 0 };
				os_mbs_to_wcs(name, strlen(name), wcs, 20);
				m_cmbKeyColorType.AddString(wcs);
				m_mapChromaColorType[name] = m_mapChromaColorType.size();
				std::string key_color_type = name;
				std::transform(key_color_type.begin(), key_color_type.end(), key_color_type.begin(), ::tolower);
				m_vecChromColorType.emplace_back(key_color_type.c_str());
			}
		}
		if (count > 0) m_cmbKeyColorType.SetCurSel(0);
	}
	else if (strcmp(name, SETTING_KEY_COLOR) == 0){

		long long  val = obs_data_get_int(settings, SETTING_KEY_COLOR);
	}
	else if (strcmp(name, SETTING_SIMILARITY) == 0
		|| strcmp(name, SETTING_SMOOTHNESS) == 0
		|| strcmp(name, SETTING_OPACITY) == 0){
		int val = (int)obs_data_get_int(settings, name);
		obs_data_set_int(settings, name, val);
	}
	else if (strcmp(name, SETTING_CONTRAST) == 0
		|| strcmp(name, SETTING_BRIGHTNESS) == 0
		|| strcmp(name, SETTING_GAMMA) == 0){
		double val = (int)obs_data_get_double(settings, name);
		obs_data_set_double(settings, name, val);
	}
}	

void CChromaFilterDlg::RefreshProperties()
{
	obs_property_t *property = obs_properties_first(properties.get());
	while (property) {
		_setChromaVal(property);
		obs_property_next(&property);
	}
}
void CChromaFilterDlg::AddFilter(OBSSource filter)
{
	_filter = filter;
	if (_filter){
		m_btnAddFilter.EnableWindow(FALSE);
	}
}
void CChromaFilterDlg::AddNewFilter(const char *id)
{
	if (id && *id) {
		obs_source_t *existing_filter;
		std::string name = obs_source_get_display_name(id);
		obs_source_t *filter = obs_source_create(id, name.c_str(),
			nullptr, nullptr);
		if (filter) {
			const char *sourceName = obs_source_get_name(_source);

			blog(LOG_INFO, "User added filter '%s' (%s) "
				"to source '%s'",
				name.c_str(), id, sourceName);
			_filter = filter;
			if (_filter) m_btnAddFilter.EnableWindow(FALSE);
			obs_source_filter_add(_source, filter);
			obs_source_release(filter);
		}
	}
}

void CChromaFilterDlg::DrawPreview(void *data, uint32_t cx, uint32_t cy)
{
	CChromaFilterDlg *window = static_cast<CChromaFilterDlg*>(data);

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


void CChromaFilterDlg::OBSSourceFilterAdded(void *param, calldata_t *data)
{
	CChromaFilterDlg *window = reinterpret_cast<CChromaFilterDlg*>(param);
	obs_source_t *filter = (obs_source_t*)calldata_ptr(data, "filter");

	::SendMessage(window->GetSafeHwnd(), WM_OBS_ADD_FILTER, (WPARAM)filter, 0);
	// 	QMetaObject::invokeMethod(window, "AddFilter",
	// 		Q_ARG(OBSSource, OBSSource(filter)));
}


BOOL CChromaFilterDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	m_staPreview.GetClientRect(rcPreview);
	m_staPreview.MapWindowPoints(this, rcPreview);
	preview->SetReceiveWnd(m_hWnd);
	preview->Create(NULL, NULL, WS_CHILD | WS_VISIBLE | WS_BORDER, rcPreview, this, IDC_BASEWND_VIDEO + 102);
	preview->ShowWindow(SW_SHOW);

	obs_source_enum_filters(_source,
		[](obs_source_t*, obs_source_t *filter, void *p)
	{
		CChromaFilterDlg *window =
			reinterpret_cast<CChromaFilterDlg*>(p);

		window->AddFilter(filter);
		::SendMessage(window->GetSafeHwnd(), WM_OBS_ADD_FILTER, (WPARAM)filter, 0);
	}, this);
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}


void CChromaFilterDlg::OnBnClickedButton1()
{
	CDialogEx::OnOK();
}


void CChromaFilterDlg::OnSelchangeComboKeyColorType()
{
	int sel = m_cmbKeyColorType.GetCurSel();
	if (sel >=0 && sel < m_cmbKeyColorType.GetCount()){
		_UpdateFilter(SETTING_COLOR_TYPE, m_vecChromColorType[sel]);
	}
}

void CChromaFilterDlg::OnBnClickedButtonDelChromaFilter()
{
	obs_source_enum_filters(_source,
		[](obs_source_t*, obs_source_t *filter, void *p)
	{
		CChromaFilterDlg *window =
			reinterpret_cast<CChromaFilterDlg*>(p);
		::SendMessage(window->GetSafeHwnd(), WM_OBS_DEL_CHROM_FILTER, (WPARAM)filter, 0);
	}, this);

	
}


void CChromaFilterDlg::OnBnClickedButtonAddChromaFilter()
{
	// TODO: Add your control notification handler code here
	AddNewFilter("chroma_key_filter");
}

void CChromaFilterDlg::_UpdateFilter(std::string chromsetting, std::string val)
{
	obs_data_set_string(settings, chromsetting.c_str(), val.c_str());
	callback(_filter, settings);
	obs_property_t *property = obs_properties_get(properties.get(), chromsetting.c_str());
	obs_property_modified(property, settings);
}