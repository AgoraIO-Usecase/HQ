// DlgColorKey.cpp : implementation file
//

#include "stdafx.h"
#include "AgoraHQ.h"
#include "DlgColorKey.h"
#include "afxdialogex.h"

#define SETTING_OPACITY                "opacity"
#define SETTING_CONTRAST               "contrast"
#define SETTING_BRIGHTNESS             "brightness"
#define SETTING_GAMMA                  "gamma"
#define SETTING_COLOR_TYPE             "key_color_type"
#define SETTING_KEY_COLOR              "key_color"
#define SETTING_SIMILARITY             "similarity"
#define SETTING_SMOOTHNESS             "smoothness"
// CDlgColorKey dialog

IMPLEMENT_DYNAMIC(CDlgColorKey, CBasicProperties)
static inline COLORREF color_from_int(long long val, int alpha)
{
	alpha = (val >> 24) & 0xff;

	return RGB(val & 0xff, (val >> 8) & 0xff, (val >> 16) & 0xff);
}

static inline long long color_to_int(COLORREF color, int alpha)
{
	auto shift = [&](unsigned val, int shift)
	{
		return ((val & 0xff) << shift);
	};

	return color & 0xff | ((color & 0xff) << 8)
		| ((color & 0xff) << 16) | ((alpha & 0xff) << 24);
}
		//shift(alpha, 24);
CDlgColorKey::CDlgColorKey(OBSSource _filter, CWnd* pParent /*=NULL*/)
: CBasicProperties(CDlgColorKey::IDD, _filter, pParent, true)
{

}

CDlgColorKey::~CDlgColorKey()
{
}

void CDlgColorKey::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO1, m_cmbKeyColorType);
	DDX_Control(pDX, IDC_BUTTON_SELECT_COLOR, m_btnSelectColor);
	DDX_Control(pDX, IDC_SLIDER_SIMILARITY, m_sliderSimiliraty);
	DDX_Control(pDX, IDC_EDIT_SIMILARITY, m_edtSimilarity);
	DDX_Control(pDX, IDC_SPIN_SIMILARITY, m_spinSimilarity);
	DDX_Control(pDX, IDC_SLIDER_SMOOTH, m_sliderSmoothness);
	DDX_Control(pDX, IDC_SLIDER_CONTRAST, m_sliderContrast);
	DDX_Control(pDX, IDC_SLIDER_BRIGHT, m_sliderBrightness);
	DDX_Control(pDX, IDC_SLIDER_GAMA, m_sliderGama);
	DDX_Control(pDX, IDC_EDIT_GAMA, m_edtGama);
	DDX_Control(pDX, IDC_SPIN_GAMA, m_spinButtonGama);
	DDX_Control(pDX, IDC_SPIN_BRIGHT, m_spinButtonBright);
	DDX_Control(pDX, IDC_EDIT_BRIGHT, m_edtBrightness);
	DDX_Control(pDX, IDC_EDIT_CONTRAST, m_edtContrast);
	DDX_Control(pDX, IDC_SPIN_CONTRAST, m_spinContrast);
	DDX_Control(pDX, IDC_SPIN_OPACITY, m_spinOpacity);
	DDX_Control(pDX, IDC_SPIN_SMOOTH, m_spinSmoothness);
	DDX_Control(pDX, IDC_EDIT_SMOOTH, m_edtSmoothness);
	DDX_Control(pDX, IDC_EDIT_OPACITY, m_edtOpacity);
	DDX_Control(pDX, IDC_STATIC_SEL_COLOR, m_staKeyColor);
	DDX_Control(pDX, IDC_EDIT2, m_edtAlpha);
}


BEGIN_MESSAGE_MAP(CDlgColorKey, CBasicProperties)
	ON_BN_CLICKED(IDC_BUTTON_SELECT_COLOR, &CDlgColorKey::OnBnClickedButtonSelectColor)
	ON_EN_CHANGE(IDC_EDIT_OPACITY, &CDlgColorKey::OnChangeEditOpacity)
	ON_EN_CHANGE(IDC_EDIT_SIMILARITY, &CDlgColorKey::OnChangeEditSimilarity)
	ON_EN_CHANGE(IDC_EDIT_SMOOTH, &CDlgColorKey::OnChangeEditSmooth)
	ON_EN_CHANGE(IDC_EDIT_CONTRAST, &CDlgColorKey::OnChangeEditContrast)
	ON_EN_CHANGE(IDC_EDIT_GAMA, &CDlgColorKey::OnChangeEditGama)
	ON_EN_CHANGE(IDC_EDIT_BRIGHT, &CDlgColorKey::OnChangeEditBright)
	ON_CBN_SELCHANGE(IDC_COMBO1, &CDlgColorKey::OnSelchangeCombo1)
	ON_EN_CHANGE(IDC_EDIT2, &CDlgColorKey::OnChangeEdit2)
END_MESSAGE_MAP()


// CDlgColorKey message handlers
void CDlgColorKey::SetKeyColorProperty()
{
}

BOOL CDlgColorKey::OnInitDialog()
{
	CBasicProperties::OnInitDialog();
	m_btnSelectColor.EnableWindow(FALSE);
	m_edtAlpha.EnableWindow(FALSE);
	obs_property_t *property = obs_properties_first(properties.get());
	bool hasNoProperties = !property;
	// TODO:  Add extra initialization here


	m_spinContrast.ShowWindow(SW_HIDE);
	m_sliderContrast.ShowWindow(SW_HIDE);
	m_edtContrast.ShowWindow(SW_HIDE);

	m_spinSmoothness.ShowWindow(SW_HIDE);
	m_sliderSmoothness.ShowWindow(SW_HIDE);
	m_edtSmoothness.ShowWindow(SW_HIDE);

	m_spinSimilarity.ShowWindow(SW_HIDE);
	m_sliderSimiliraty.ShowWindow(SW_HIDE);
	m_edtSimilarity.ShowWindow(SW_HIDE);

	m_spinOpacity.ShowWindow(SW_HIDE);
	m_edtOpacity.ShowWindow(SW_HIDE);

	m_sliderBrightness.ShowWindow(SW_HIDE);
	m_spinButtonBright.ShowWindow(SW_HIDE);
	m_edtBrightness.ShowWindow(SW_HIDE);

	m_spinButtonGama.ShowWindow(SW_HIDE);
	m_sliderGama.ShowWindow(SW_HIDE);
	m_edtGama.ShowWindow(SW_HIDE);
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}


void CDlgColorKey::OnBnClickedButtonSelectColor()
{
	CColorDialog colorDlg;
	if (IDOK == colorDlg.DoModal()){
		m_customColor = colorDlg.GetColor();
		CString strClr;
		strClr.Format(_T("#%02x%02x%02x%02x"),
			m_alpha, GetRValue(m_customColor), 
			GetGValue(m_customColor), GetBValue(m_customColor));
		m_staKeyColor.SetWindowText(strClr);

		obs_data_set_int(settings, SETTING_KEY_COLOR, color_to_int(m_customColor, m_alpha));
	}
}

void CDlgColorKey::UpdateFilter(obs_source_t* filter)
{

}

void CDlgColorKey::SetContrlValue(obs_property_t* prop, 
	CSliderCtrl* slider, CSpinButtonCtrl* spin, CEdit* edt)
{
	const char        *name = obs_property_name(prop);
	obs_property_type type = obs_property_get_type(prop);
	int minVal = obs_property_int_min(prop);
	int	maxVal = obs_property_int_max(prop);
	int stepVal = obs_property_int_step(prop);
	int val = (int)obs_data_get_int(settings, name);

	if (spin){
		spin->SetPos(val);
		spin->SetRange(minVal, maxVal);
	}
	
	if (slider){
		slider->SetRangeMax(maxVal);
		slider->SetRangeMin(minVal);
		slider->SetPos(val);
		slider->SetLineSize(stepVal);
	}
	CString s;
	s.Format(_T("%d"), val);
	edt->SetWindowText(s);
}

void CDlgColorKey::AddProperty(obs_property_t *property)
{
	CBasicProperties::AddProperty(property);

	const char        *name = obs_property_name(property);
	obs_property_type type = obs_property_get_type(property);

	if (strcmp(name, SETTING_COLOR_TYPE) == 0){
		size_t           count = obs_property_list_item_count(property);
		for (size_t i = 0; i < count; i++){
			const char *name = obs_property_list_item_name(property, i);
			wchar_t wcs[20] = { 0 };
			os_mbs_to_wcs(name, strlen(name), wcs, 20);
			m_cmbKeyColorType.AddString(wcs);

			m_mapKeyColorType[name] = m_mapKeyColorType.size();
		}
		if (count > 0) m_cmbKeyColorType.SetCurSel(0);
	}
	else if (strcmp(name, SETTING_KEY_COLOR) == 0){
		//obs_property_get_type()
		long long  val = obs_data_get_int(settings, SETTING_KEY_COLOR);
		m_customColor = color_from_int(val, m_alpha);
	}
	else if (strcmp(name, SETTING_SIMILARITY) == 0){
		SetContrlValue(property, &m_sliderSimiliraty, &m_spinSimilarity, &m_edtSimilarity);
	}
	else if (strcmp(name, SETTING_SMOOTHNESS) == 0){
		SetContrlValue(property, &m_sliderSmoothness, &m_spinSmoothness, &m_edtSmoothness);
	}
	else if (strcmp(name, SETTING_OPACITY) == 0){
		SetContrlValue(property, nullptr, &m_spinOpacity, &m_edtOpacity);
	}
	else if (strcmp(name, SETTING_CONTRAST) == 0){
		CString s;
		double val = (double)obs_data_get_double(settings, name);
		s.Format(_T("%.2lf"), val);
		m_edtContrast.SetWindowText(s);
		//SetContrlValue(property, NULL, NULL, &m_edtContrast);
	}
	else if (strcmp(name, SETTING_BRIGHTNESS) == 0){
		CString s;
		double val = (double)obs_data_get_double(settings, name);
		s.Format(_T("%.2lf"), val);
		m_edtBrightness.SetWindowText(s);
	}
	else if (strcmp(name, SETTING_GAMMA) == 0){
		CString s;
		double val = (double)obs_data_get_double(settings, name);
		s.Format(_T("%.2lf"), val);
		m_edtGama.SetWindowText(s);
	}
}

void CDlgColorKey::OnChangeEditOpacity()
{
	CString opacity;
	m_edtOpacity.GetWindowText(opacity);

	int iopacity = _ttoi(opacity.GetBuffer(0));
	if (iopacity > 100){
		m_edtOpacity.SetWindowText(_T("100"));
		iopacity = 100;
	}
	obs_data_set_int(settings, SETTING_OPACITY, iopacity);
	callback(obj, settings);
}


void CDlgColorKey::OnChangeEditSimilarity()
{
	CString similarity;
	m_edtSimilarity.GetWindowText(similarity);

	int isimilarity = _ttoi(similarity.GetBuffer(0));
	if (isimilarity > 1000){
		m_edtSimilarity.SetWindowText(_T("1000"));
		isimilarity = 1000;
	}
	
	obs_data_set_int(settings, SETTING_SIMILARITY, isimilarity);
	callback(obj, settings);
}


void CDlgColorKey::OnChangeEditSmooth()
{
	CString smooth;
	m_edtSmoothness.GetWindowText(smooth);

	int ismooth = _ttoi(smooth.GetBuffer(0));
	if (ismooth > 1000){
		m_edtSmoothness.SetWindowText(_T("1000"));
		ismooth = 1000;
	}
	obs_data_set_int(settings, SETTING_SMOOTHNESS, ismooth);
	callback(obj, settings);
}

void CDlgColorKey::OnChangeEditContrast()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CBasicProperties::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	CString contrast;
	m_edtContrast.GetWindowText(contrast);

	float f = _ttof(contrast);

	obs_data_set_int(settings, SETTING_CONTRAST, f);
	callback(obj, settings);
}


void CDlgColorKey::OnChangeEditGama()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CBasicProperties::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	CString gama;
	m_edtGama.GetWindowText(gama);

	float f = _ttof(gama);
	obs_data_set_int(settings, SETTING_GAMMA, f);
	callback(obj, settings);
}


void CDlgColorKey::OnChangeEditBright()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CBasicProperties::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	CString bright;
	m_edtBrightness.GetWindowText(bright);

	float f = _ttof(bright);

	obs_data_set_int(settings, SETTING_BRIGHTNESS, f);
	callback(obj, settings);
}


BOOL CDlgColorKey::PreTranslateMessage(MSG* pMsg)
{
	switch(pMsg->message)
	{
	case WM_KEYDOWN:
		if (pMsg->wParam == VK_RETURN)
			return TRUE;
		break;
	default:
		break;

	}
	return CBasicProperties::PreTranslateMessage(pMsg);
}


void CDlgColorKey::OnSelchangeCombo1()
{
	CString keyColorType;
	m_cmbKeyColorType.GetWindowText(keyColorType);
	char* utf8 = nullptr;
	os_wcs_to_utf8_ptr(keyColorType.GetBuffer(0), keyColorType.GetLength(), &utf8);
	obs_data_set_string(settings, SETTING_KEY_COLOR, utf8);
	bool bCustom = m_cmbKeyColorType.GetCurSel() == m_mapKeyColorType["Custom Color"];
	m_edtAlpha.EnableWindow(bCustom);
	m_btnSelectColor.EnableWindow(bCustom);
}


void CDlgColorKey::OnChangeEdit2()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CBasicProperties::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	CString strAlpha;
	m_edtAlpha.GetWindowText(strAlpha);

	m_alpha = _ttol (strAlpha.GetBuffer(0));
	if (m_alpha > 255)
		m_edtAlpha.SetWindowText(_T("255"));

	long long v = color_to_int(m_customColor, m_alpha);
	long long alpha = (m_alpha & 0xff) << 24;
	obs_data_set_int(settings, SETTING_KEY_COLOR, color_to_int(m_customColor, m_alpha));
}
