// DShowCameraDlg.cpp : implementation file
//

#include "stdafx.h"
#include "AgoraHQ.h"
#include "DShowCameraDlg.h"
#include "afxdialogex.h"
#include "AgoraHQDlg.h"

// CDShowCameraDlg dialog

IMPLEMENT_DYNAMIC(CDShowCameraDlg, CBasicProperties)

CDShowCameraDlg::CDShowCameraDlg(OBSSource _source, CWnd* pParent /*=NULL*/)
: CBasicProperties(CDShowCameraDlg::IDD, _source, pParent)
{
	m_dist_vertical = 250;
}

CDShowCameraDlg::~CDShowCameraDlg()
{
}

void CDShowCameraDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_CAMERA, m_cmbCameraDevice);
}


BEGIN_MESSAGE_MAP(CDShowCameraDlg, CBasicProperties)
	ON_BN_CLICKED(IDOK, &CDShowCameraDlg::OnBnClickedOk)
	ON_CBN_SELCHANGE(IDC_COMBO1, &CDShowCameraDlg::OnSelchangeCombo1)
END_MESSAGE_MAP()


// CDShowCameraDlg message handlers


BOOL CDShowCameraDlg::OnInitDialog()
{
	CBasicProperties::OnInitDialog();
	
	obs_property_t* prop = m_mapProperties[propery_name];
	size_t          count = obs_property_list_item_count(prop);

	for (int i = 0; i < count; ++i)
	{
		const char *name = obs_property_list_item_name(prop, i);

		char* utf8 = NULL;
		os_mbs_to_utf8_ptr(name, strlen(name), &utf8);

		 settings = obs_source_get_settings(source);
		/*obs_data_set_string(settings, propery_name.c_str(), utf8);*/

		wchar_t* wcs = NULL;
		os_utf8_to_wcs_ptr(name, strlen(name), &wcs);
		m_cmbCameraDevice.AddString(wcs);
		std::string cameraId = obs_property_list_item_string(prop, i);

		char* id = NULL;
		os_mbs_to_utf8_ptr(cameraId.c_str(), strlen(name), &id);
		m_cameraIds.push_back(id);
	}

	if (m_cmbCameraDevice.GetCount() > 0)
	{
		m_cmbCameraDevice.SetCurSel(0);
		obs_data_set_string(settings, propery_name.c_str(), m_cameraIds[0].c_str());
	}

	if (callback && !deferUpdate)
	{
		callback((void*)source, settings);
	}

	if (obs_property_modified(prop, settings))
	{
		RefreshProperties();
	}

	::PostMessage(m_pVideoDlg->GetSafeHwnd(), WM_INVALIDATE_PREVIEW, 0, 0);
	return TRUE;  // return TRUE unless you set the focus to a control
}

void CDShowCameraDlg::SetVideoDialog(CAgoraHQDlg* pDlg){ m_pVideoDlg = pDlg; }

void CDShowCameraDlg::OnBnClickedOk()
{
	CBasicProperties::OnOK();
}


void CDShowCameraDlg::AddProperty(obs_property_t *property)
{	CBasicProperties::AddProperty(property);

	if (!obs_property_enabled(property)) {
		
	}
}


void CDShowCameraDlg::OnSelchangeCombo1()
{
	int cur_sel = m_cmbCameraDevice.GetCurSel();
	if (cur_sel >= 0)
	{

		obs_property_t* prop = m_mapProperties[propery_name];
		obs_data_set_string(settings, propery_name.c_str(), m_cameraIds[cur_sel].c_str());
		settings = obs_source_get_settings(source);

		if (callback && !deferUpdate)
		{
			callback((void*)source, settings);
		}

		if (obs_property_modified(prop, settings))
		{
			RefreshProperties();
		}
	}
}
