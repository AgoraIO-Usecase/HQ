// FFmpegSourceDlg.cpp : implementation file
//

#include "stdafx.h"
#include "AgoraHQ.h"
#include "FFmpegSourceDlg.h"
#include "afxdialogex.h"


// CFFmpegSourceDlg dialog

IMPLEMENT_DYNAMIC(CFFmpegSourceDlg, CBasicProperties)

CFFmpegSourceDlg::CFFmpegSourceDlg(OBSSource _source, CWnd* pParent /*=NULL*/)
: CBasicProperties(CFFmpegSourceDlg::IDD, _source, pParent)
{
	m_dist_vertical = 100;
}

CFFmpegSourceDlg::~CFFmpegSourceDlg()
{
}

void CFFmpegSourceDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_MOVIE_PATH, m_edtMovie);
	DDX_Control(pDX, IDC_BUTTON_MOVIE_PATH, m_btnMovie);
}


BEGIN_MESSAGE_MAP(CFFmpegSourceDlg, CBasicProperties)
	ON_BN_CLICKED(IDOK, &CFFmpegSourceDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDC_BUTTON_MOVIE_PATH, &CFFmpegSourceDlg::OnBnClickedButtonMoviePath)
END_MESSAGE_MAP()


// CFFmpegSourceDlg message handlers


BOOL CFFmpegSourceDlg::OnInitDialog()
{
	CBasicProperties::OnInitDialog();

	return TRUE;  // return TRUE unless you set the focus to a control
}

void CFFmpegSourceDlg::OnBnClickedOk()
{
	std::string propery_name = "local_file";
	obs_property_t* property = m_mapProperties[propery_name];
	char* utf8 = NULL;
	os_wcs_to_utf8_ptr(m_strMoviePath.GetBuffer(0), m_strMoviePath.GetLength(), &utf8);
	OBSData settings = obs_source_get_settings(source);
	obs_data_set_string(settings, propery_name.c_str(), utf8);

	if (obs_property_modified(property, settings))
	{
		RefreshProperties();
	}
	if (callback /*&& !deferUpdate*/)
	{
		callback((void*)source, settings);
	}
	
	CBasicProperties::OnOK();
}

void CFFmpegSourceDlg::OnBnClickedButtonMoviePath()
{
	TCHAR szFilters[] = _T("All Media Files (*.mp4;*.ts;*.mov;*.flv;*.mkv;*avi;*.mp3;*.ogg;*.aac;*.wav;*.gif;*.webm;)|*.mp4;*.ts;*.mov;*.flv;*.mkv;*avi;*.mp3;*.ogg;*.aac;*.wav;*.gif;*.webm;|\
						All Video Files(*.mp4;*.ts;*.mov;*.flv;*.mkv;*avi;)|*.mp4;*.ts;*.mov;*.flv;*.mkv;*avi;|\
						All Audio Files(*.mp3;*.ogg;*.aac;*.wav;*.gif;*.webm;)|*.mp3;*.ogg;*.aac;*.wav;*.gif;*.webm;||"); //All Files (*.*)|*.*
	TCHAR szOldCurrentDirector[MAX_PATH] = { 0 };
	GetCurrentDirectory(MAX_PATH, szOldCurrentDirector);

	CFileDialog fileDlg(TRUE, NULL, NULL,
		OFN_FILEMUSTEXIST | OFN_HIDEREADONLY, szFilters);

	if (fileDlg.DoModal() == IDOK)
	{
		CString pathName = fileDlg.GetPathName();

		if (!pathName.IsEmpty() && pathName.Compare(m_strMoviePath) != 0)
		{
			m_strMoviePath = pathName;
			m_edtMovie.SetWindowText(m_strMoviePath);
			SetCurrentDirectory(szOldCurrentDirector);
		}
	}
}

void CFFmpegSourceDlg::AddProperty(obs_property_t *property)
{
	CBasicProperties::AddProperty(property);

	if (!obs_property_enabled(property)) {
		m_edtMovie.EnableWindow(false);
		m_btnMovie.EnableWindow(false);
	}
}