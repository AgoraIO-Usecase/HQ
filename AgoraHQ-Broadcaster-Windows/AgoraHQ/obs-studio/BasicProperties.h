#pragma once

#include "obs.hpp"
#include "DisplaySourceWnd.h"
// CBasicProperties dialog
class CAgoraHQDlg;

typedef obs_properties_t *(*PropertiesReloadCallback)(void *obj);
typedef void(*PropertiesUpdateCallback)(void *obj,
	obs_data_t *settings);
using properties_delete_t = decltype(&obs_properties_destroy);
using properties_t =
std::unique_ptr<obs_properties_t, properties_delete_t>;
class CBasicProperties : public CDialogEx
{
	
	DECLARE_DYNAMIC(CBasicProperties)

public:
	CBasicProperties(UINT nIDTemplate, OBSSource _source, CWnd* pParent = NULL);   // standard constructor
	virtual ~CBasicProperties();

// Dialog Data
	//enum { IDD = IDD_BASICPROPERTIES };
	
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

	afx_msg void OnDestroy();
protected:
//	void OnClickOK();
	void OnClickCancel();
	void OnClickDefault();
	virtual BOOL OnInitDialog();
	CDisplaySourceWnd* preview = NULL;
	CAgoraHQDlg* main;
	OBSSource  source;
	OBSSignal  removedSignal;
	OBSSignal  renamedSignal;
	OBSSignal  updatePropertiesSignal;
	OBSData    oldSettings;
	void                                     *obj = nullptr;
	std::map<std::string, obs_property_t*> m_mapProperties;
	properties_t                             properties;
	OBSData                                  settings;
	std::string                              type = "";
	bool                                     deferUpdate;
	static void DrawPreview(void *data, uint32_t cx, uint32_t cy);

	PropertiesReloadCallback                 reloadCallback;
	PropertiesUpdateCallback                 callback = nullptr;

	static void SourceRemoved(void *data, calldata_t *params);
	static void SourceRenamed(void *data, calldata_t *params);
	static void UpdateProperties(void *data, calldata_t *params);
//	static void DrawPreview(void *data, uint32_t cx, uint32_t cy);

	int CheckSettings();

	void ReloadProperties();
	void RefreshProperties();
	virtual void AddProperty(obs_property_t *property);
	void Cleanup();
	void AddDisplay();

	virtual BOOL OnWndMsg(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pResult);

	int m_dist_vertical = 0;
};
