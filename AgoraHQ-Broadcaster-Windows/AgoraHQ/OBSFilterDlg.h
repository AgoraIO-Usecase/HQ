#pragma once

#include "DisplaySourceWnd.h"
#include "afxcmn.h"

// COBSFilterDlg dialog
class CDlgColorKey;
class COBSFilterDlg : public CDialogEx
{
	DECLARE_DYNAMIC(COBSFilterDlg)
	struct FilterInfo {
		string type;
		string name;

		inline FilterInfo(const char *type_, const char *name_)
			: type(type_), name(name_)
		{}
	};
public:
	COBSFilterDlg(OBSSource source,CWnd* pParent = NULL);   // standard constructor
	virtual ~COBSFilterDlg();

// Dialog Data
	enum { IDD = IDD_DIALOG_OBS_FILTER };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
private:
	virtual BOOL OnWndMsg(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pResult);

	CAgoraHQDlg* m_pVideoDlg;
	CString filter_tile = _T("filter Name");
	CString filter_info = _T("Please input the name of filter");
	CDisplaySourceWnd* preview = NULL;
	CRect rcPreview;

	CDlgColorKey* m_pDlgColorKey;
	void AddNewFilter(const char *id);

	std::vector<FilterInfo> filter_types;

	OBSSignal addSignal;
	OBSSignal removeSignal;
	OBSSignal reorderSignal;

	OBSSignal removeSourceSignal;
	OBSSignal renameSourceSignal;
	OBSSignal updatePropertiesSignal;

	static void OBSSourceFilterAdded(void *param, calldata_t *data);
	static void OBSSourceFilterRemoved(void *param, calldata_t *data);
	static void OBSSourceReordered(void *param, calldata_t *data);
	static void SourceRemoved(void *param, calldata_t *data);
	static void SourceRenamed(void *param, calldata_t *data);
public:
	
	virtual BOOL OnInitDialog();

	OBSSource _source;
	CTreeCtrl m_effectFilterTree;
	static void DrawPreview(void *data, uint32_t cx, uint32_t cy);
	CStatic m_staPreview;
	afx_msg void OnBnClickedButtonAddFilter();
	afx_msg void OnBnClickedButtonDelFilter();
	afx_msg void OnBnClickedOk();
	CTreeCtrl m_filterTreeView;
	std::vector<OBSSource> m_vecFilters;
};
