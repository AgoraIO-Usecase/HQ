// SelectOBSSourceDlg.cpp : implementation file
//

#include "stdafx.h"
#include "AgoraHQ.h"
#include "AgoraHQDlg.h"
#include "SelectOBSSourceDlg.h"
#include "afxdialogex.h"

struct AddSourceData {
	obs_source_t *source;
	bool visible;
};

static void AddSource(void *_data, obs_scene_t *scene)
{
	AddSourceData *data = (AddSourceData *)_data;
	obs_sceneitem_t *sceneitem;

	sceneitem = obs_scene_add(scene, data->source);
	//obs_sceneitem_select(sceneitem, true);//for test
	obs_sceneitem_set_visible(sceneitem, data->visible);
}

static char *get_new_source_name(const char *name)
{
	struct dstr new_name = { 0 };
	int inc = 0;

	dstr_copy(&new_name, name);

	for (;;) {
		obs_source_t *existing_source = obs_get_source_by_name(
			new_name.array);
		if (!existing_source)
			break;

		obs_source_release(existing_source);

		dstr_printf(&new_name, "%s %d", name, ++inc + 1);
	}

	return new_name.array;
}

static inline const char *GetSourceDisplayName(const char *id)
{
	if (strcmp(id, "scene") == 0)
		return Str("Basic.Scene");
	return obs_source_get_display_name(id);
}


bool AddNew(CAgoraHQDlg *parent, const char *id, const char *name,
	const bool visible, OBSSource &newSource)
{
	//OBSBasic     *main = reinterpret_cast<OBSBasic*>(App()->GetMainWindow());

	OBSScene     scene = parent->GetCurrentScene();
	bool         success = false;
	if (!scene)
		return false;

	obs_source_t *source = obs_get_source_by_name(name);
	if (source) {
		AfxMessageBox(_T("source name has existed."));
	}
	else {
		source = obs_source_create(id, name, NULL, nullptr);

		if (source) {
			AddSourceData data;
			data.source = source;
			data.visible = visible;

			obs_enter_graphics();
			obs_scene_atomic_update(scene, AddSource, &data);
			obs_leave_graphics();

			newSource = source;

			success = true;
		}
	}

	obs_source_release(source);
	return success;
}


bool CSelectOBSSourceDlg::EnumSources(void *data, obs_source_t *source)
{
	const char *name = obs_source_get_name(source);
	const char *id = obs_source_get_id(source);
	// 	if (strcmp(id, window->id) == 0)
	// 		window->ui->sourceList->addItem(QT_UTF8(name));

	return true;
}
// CSelectOBSSourceDlg dialog

IMPLEMENT_DYNAMIC(CSelectOBSSourceDlg, CDialogEx)

CSelectOBSSourceDlg::CSelectOBSSourceDlg(const char* id, NAME_TYPE type, CString title, CString info, CWnd* pParent /*=NULL*/, std::string name/*=""*/)
	: CDialogEx(CSelectOBSSourceDlg::IDD, pParent)
	, source_id(id)
	, m_title(title)
	, m_info(info)
	, name_type(type)
{
	if (name_type == NAME_TYPE_SOURCE){
		utf8_source_name = GetSourceDisplayName(id);//utf8
		obs_source_t *source = nullptr;

		char* szAnsi = NULL;
		wchar_t* wcs = NULL;
		os_utf8_to_wcs_ptr(utf8_source_name.c_str(), utf8_source_name.length(), &wcs);

		int i = 2;
		os_wcs_to_mbs_ptr(wcs, utf8_source_name.length(), &szAnsi);
		std::string text = utf8_source_name;
		while ((source = obs_get_source_by_name(text.c_str()))) {
			obs_source_release(source);
			char szutf8[1024] = { 0 };
			sprintf_s(szutf8, "%s%d", utf8_source_name.c_str(), i++);
			text = szutf8;
		}
		utf8_source_name = text;
		os_utf8_to_wcs_ptr(text.c_str(), text.length(), &wcs);
		source_name = wcs;
	}
	else{
		wchar_t* wcs = NULL;
		os_mbs_to_wcs_ptr(name.c_str(), name.length(), &wcs);
		filter_name = wcs;
	}
}

CSelectOBSSourceDlg::~CSelectOBSSourceDlg()
{
}

void CSelectOBSSourceDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_SOURCE_NAME, m_edtOBSSourceName);
	DDX_Control(pDX, IDC_STATIC_INFO, m_staInfo);
}


BEGIN_MESSAGE_MAP(CSelectOBSSourceDlg, CDialogEx)
	ON_BN_CLICKED(IDOK, &CSelectOBSSourceDlg::OnBnClickedOk)
END_MESSAGE_MAP()

// CSelectOBSSourceDlg message handlers

void CSelectOBSSourceDlg::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	if (name_type == NAME_TYPE_SOURCE)
		AddNew(reinterpret_cast<CAgoraHQDlg*>(GetParent()), source_id.c_str(), utf8_source_name.c_str(), true, newSource);
	else if (name_type == NAME_TYPE_FILTER){
		 m_edtOBSSourceName.GetWindowText(filter_name);
		 char* szAnsi = NULL;
		 os_wcs_to_utf8_ptr(filter_name.GetBuffer(), filter_name.GetLength(), &szAnsi);
		 utf8_filter_name = szAnsi;
	}
	CDialogEx::OnOK();
}


BOOL CSelectOBSSourceDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  Add extra initialization here
	if (name_type == NAME_TYPE_SOURCE)
	    m_edtOBSSourceName.SetWindowText(source_name);
	else
		m_edtOBSSourceName.SetWindowText(filter_name);
	SetWindowText(m_title);
	m_staInfo.SetWindowText(m_info);
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}
