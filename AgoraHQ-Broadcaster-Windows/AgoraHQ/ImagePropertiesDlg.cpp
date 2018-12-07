// ImagePropertiesDlg.cpp : implementation file
//

#include "stdafx.h"
#include "AgoraHQ.h"
#include "AgoraHQDlg.h"
#include "ImagePropertiesDlg.h"
#include "afxdialogex.h"


// CImagePropertiesDlg dialog
#define MAX_CFileDialog_FILE_COUNT 99
#define FILE_LIST_BUFFER_SIZE ((MAX_CFileDialog_FILE_COUNT * (MAX_PATH + 1)) + 1)


IMPLEMENT_DYNAMIC(CImagePropertiesDlg, CBasicProperties)

CImagePropertiesDlg::CImagePropertiesDlg(OBSSource _source, CWnd* pParent /*=NULL*/)
: CBasicProperties(CImagePropertiesDlg::IDD, _source, pParent)
{
	m_dist_vertical = 150;
}

CImagePropertiesDlg::~CImagePropertiesDlg()
{
}

void CImagePropertiesDlg::DoDataExchange(CDataExchange* pDX)
{
	CBasicProperties::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_IMAGE_PATH, m_edtImagePath);
	DDX_Control(pDX, IDC_BUTTON_IMAGE_PATH, m_btnImagePath);
	DDX_Control(pDX, IDC_STATIC_IMAGE, m_staImage);
}

void CImagePropertiesDlg::SetVideoDialog(CAgoraHQDlg* pDlg)
{
	m_pVideoDlg = pDlg;
}


BEGIN_MESSAGE_MAP(CImagePropertiesDlg, CBasicProperties)
	ON_BN_CLICKED(IDC_BUTTON_IMAGE_PATH, &CImagePropertiesDlg::OnBnClickedButtonImagePath)
END_MESSAGE_MAP()


// CImagePropertiesDlg message handlers


void CImagePropertiesDlg::OnBnClickedButtonImagePath()
{
	// TODO: Add your control notification handler code here
	TCHAR szOldCurrentDirector[MAX_PATH] = {0};
	GetCurrentDirectory(MAX_PATH, szOldCurrentDirector);
	TCHAR szFilters[] = _T("All Image Files (*.jpg;*.jpeg;*.png;*.bmp;*.gif;)|*.jpg;*.jpeg;*.png;*.bmp;*.gif||"); //All Files (*.*)|*.*
	
	std::string propery_name = "file";
	obs_property_t* property = m_mapProperties[propery_name];
	const char    *desc = obs_property_description(property);
	obs_path_type type = obs_property_path_type(property);
	const char    *filter = obs_property_path_filter(property);
	const char    *default_path = obs_property_path_default_path(property);
	CFileDialog fileDlg(TRUE, NULL, NULL,
		OFN_FILEMUSTEXIST | OFN_HIDEREADONLY, szFilters);

	if (fileDlg.DoModal() == IDOK)
	{
		CString pathName = fileDlg.GetPathName();

		if (!pathName.IsEmpty() && pathName.Compare(m_strImagePath) != 0)
		{
			m_strImagePath = pathName;
			m_edtImagePath.SetWindowText(m_strImagePath);

			char* utf8 = NULL;
			os_wcs_to_utf8_ptr(m_strImagePath.GetBuffer(0), m_strImagePath.GetLength(), &utf8);
			OBSData settings = obs_source_get_settings(source);
			obs_data_set_string(settings, propery_name.c_str(), utf8);

			if (callback && !deferUpdate)
			{
				callback((void*)source, settings);
			}

			if(obs_property_modified(property, settings))
			{
				RefreshProperties();
			}
			SetCurrentDirectory(szOldCurrentDirector);
			::PostMessage(m_pVideoDlg->GetSafeHwnd(), WM_INVALIDATE_PREVIEW, 0, 0);
		}
	}
}


BOOL CImagePropertiesDlg::OnInitDialog()
{
	CBasicProperties::OnInitDialog();
	CRect rc;
	m_staImage.GetWindowRect(&rc);
	ScreenToClient(rc);
	preview->MoveWindow(&rc);
	return TRUE;  // return TRUE unless you set the focus to a control
}

void CImagePropertiesDlg::AddProperty(obs_property_t *property)
{
	CBasicProperties::AddProperty(property); 

	if (!obs_property_enabled(property)) {
		m_edtImagePath.EnableWindow(false);
		m_btnImagePath.EnableWindow(false);
	}
}
