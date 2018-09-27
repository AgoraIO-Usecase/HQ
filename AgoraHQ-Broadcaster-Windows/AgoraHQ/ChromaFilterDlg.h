#pragma once
#include "DisplaySourceWnd.h"
#include "afxwin.h"
#include "BasicProperties.h"
// CChromaFilterDlg dialog

class CChromaFilterDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CChromaFilterDlg)

public:
	CChromaFilterDlg(OBSSource source, CWnd* pParent = NULL);   // standard constructor
	virtual ~CChromaFilterDlg();

// Dialog Data
	enum { IDD = IDD_DIALOG_OBS_CHROMA_FILTER };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnWndMsg(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pResult);
	afx_msg void OnBnClickedButton2();

	//
private:
	OBSSource _source;
	OBSSource _filter;
	CRect rcPreview;
	static void DrawPreview(void *data, uint32_t cx, uint32_t cy);
	void AddNewFilter(const char *id);
	void AddFilter(OBSSource filter);
	CDisplaySourceWnd* preview = NULL;
	OBSSignal addSignal;
	static void OBSSourceFilterAdded(void *param, calldata_t *data);
public:
	virtual BOOL OnInitDialog();
	CStatic m_staPreview;
	afx_msg void OnBnClickedButton1();
	CComboBox m_cmbKeyColorType;

	properties_t                             properties;
	OBSData                                  settings;
	OBSData                                  oldSettings;
	bool                                     deferUpdate;
	PropertiesReloadCallback                 reloadCallback;
	PropertiesUpdateCallback                 callback = nullptr;
	void ReloadProperties(OBSSource source);
	void RefreshProperties();
	void _setChromaVal(obs_property_t *property);
	afx_msg void OnSelchangeComboKeyColorType();

	std::map<std::string, int> m_mapChromaColorType;
	std::vector<std::string>  m_vecChromColorType;
	int default_opacity = 100;
	double default_contrast = 0.0f;
	double default_brightness = 0.0f;
	double default_gama = 0.0f;
	long long default_key_color = 0x00FF00;

	std::string default_color_type = "green";
	int default_similarity = 400;
	int default_smoothness = 80;
	int default_spill = 100;
	afx_msg void OnBnClickedButtonDelChromaFilter();
	afx_msg void OnBnClickedButtonAddChromaFilter();
	void _UpdateFilter(std::string chromsetting, std::string val);
	CButton m_btnAddFilter;
	CButton m_btnDelFilter;
};
