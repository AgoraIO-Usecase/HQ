#pragma once
#include "afxwin.h"
#include "afxcmn.h"
#include "BasicProperties.h"

// CDlgColorKey dialog

class CDlgColorKey : public CBasicProperties
{
	DECLARE_DYNAMIC(CDlgColorKey)

public:
	CDlgColorKey(OBSSource _filter, CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgColorKey();

// Dialog Data
	enum { IDD = IDD_DIALOG_COLOR_KEY };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	void UpdateFilter(obs_source_t* filter);
	void AddProperty(obs_property_t *property);
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedButtonSelectColor();
	CComboBox m_cmbKeyColorType;
	CButton m_btnSelectColor;
	CSliderCtrl m_sliderSimiliraty;
	CEdit m_edtSimilarity;
	CSpinButtonCtrl m_spinSimilarity;
	CSliderCtrl m_sliderSmoothness;
	CSliderCtrl m_sliderContrast;
	CSliderCtrl m_sliderBrightness;
	CSliderCtrl m_sliderGama;
	CEdit m_edtGama;
	CSpinButtonCtrl m_spinButtonGama;
	CSpinButtonCtrl m_spinButtonBright;
	CEdit m_edtBrightness;
	CEdit m_edtContrast;
	CSpinButtonCtrl m_spinContrast;
	CSpinButtonCtrl m_spinOpacity;
	CSpinButtonCtrl m_spinSmoothness;
	CEdit m_edtSmoothness;
	void SetKeyColorProperty();
	void SetContrlValue(obs_property_t* prop,
		CSliderCtrl* slider, CSpinButtonCtrl* spin, CEdit* edt);
	CEdit m_edtOpacity;
	afx_msg void OnChangeEditOpacity();
	afx_msg void OnChangeEditSimilarity();
	afx_msg void OnChangeEditSmooth();

	int default_similarity = 80;
	int default_smooth = 50;
	int default_opacity = 100;
	float default_gama = 0.0;
	float default_bright = 0.0;
	std::string default_color_type = "green";

	CString str_default_similarity = _T("80");
	CString str_default_smooth = _T("50");
	CString str_default_opacity = _T("100");
	CString str_default_gama = _T("0.0");
	CString str_default_bright = _T("0.0");
	CString str_default_color_type = _T("green");
	
	afx_msg void OnChangeEditContrast();
	afx_msg void OnChangeEditGama();
	afx_msg void OnChangeEditBright();

	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnSelchangeCombo1();

	std::map<std::string, int> m_mapKeyColorType;
	CStatic m_staKeyColor;
	CEdit m_edtAlpha;
	long long m_alpha = 0;
	COLORREF m_customColor = RGB(255,0,0);
	//COLORREF color()
	afx_msg void OnChangeEdit2();
};
