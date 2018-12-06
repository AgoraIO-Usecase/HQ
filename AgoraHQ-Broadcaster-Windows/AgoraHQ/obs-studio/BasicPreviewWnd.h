#pragma once
#include "DisplaySourceWnd.h"
#include <graphics/vec2.h>
#include <graphics/matrix4.h>

#define WM_SHOWMODECHANGED	WM_USER+300
#define WM_SHOWBIG			WM_USER+301


#define ITEM_LEFT   (1<<0)
#define ITEM_RIGHT  (1<<1)
#define ITEM_TOP    (1<<2)
#define ITEM_BOTTOM (1<<3)
enum class ItemHandle : uint32_t {
	None = 0,
	TopLeft = ITEM_TOP | ITEM_LEFT,
	TopCenter = ITEM_TOP,
	TopRight = ITEM_TOP | ITEM_RIGHT,
	CenterLeft = ITEM_LEFT,
	CenterRight = ITEM_RIGHT,
	BottomLeft = ITEM_BOTTOM | ITEM_LEFT,
	BottomCenter = ITEM_BOTTOM,
	BottomRight = ITEM_BOTTOM | ITEM_RIGHT
};

//主要是为响应所有的窗口消息处理
class CBasicPreviewWnd : public CDisplaySourceWnd //CWnd
{
	DECLARE_DYNAMIC(CBasicPreviewWnd)

public:
	CBasicPreviewWnd();
	virtual ~CBasicPreviewWnd();
	BOOL IsWndFree();

	void SetFaceColor(COLORREF crBackColor);
    void ShowBackground(BOOL bBackground);
    BOOL IsBackgroundMode() { return m_bBackground; }
	void DrawSceneEditing();
	static bool DrawSelectedItem(obs_scene_t *scene, obs_sceneitem_t *item,
		void *param);
	void ResizeDisplay();
	inline bool IsFixedScaling() const { return fixedScaling; }
	inline float GetScalingAmount() const { return scalingAmount; }
protected:

	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnPaint();
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	DECLARE_MESSAGE_MAP()

private:
	static OBSSceneItem GetItemAtPos(const vec2 &pos, bool selectBelow);
	static bool SelectedAtPos(const vec2 &pos);

	void GetStretchHandleData(const vec2 &pos);
	void ProcessClick(const POINT &pos, bool bCtrl);

	static void DoSelect(const POINT &pos);
	static void DoCtrlSelect(const POINT &pos);

	void SnapStretchingToScreen(vec3 &tl, vec3 &br);
	void ClampAspect(vec3 &tl, vec3 &br, vec2 &size, const vec2 &baseSize);
	vec3 CalculateStretchPos(const vec3 &tl, const vec3 &br);
	void CropItem(const vec2 &pos);
	void StretchItem(const vec2 &pos, UINT nFlags);
	void MoveItems(const vec2& pos, UINT nFlags);
	void SnapItemMovement(vec2 &offset);
	static vec3 GetSnapOffset(const vec3 &tl, const vec3 &br);
	static vec2 GetMouseEventPos(POINT pt);
	CImageList		m_imgBackGround;
	COLORREF		m_crBackColor;

	//CAGInfoWnd		m_wndInfo;

	UINT		m_nUID;

	UINT		m_nWidth;
	UINT		m_nHeight;
	int			m_nFrameRate;
	int			m_nBitRate;
	BOOL		m_bShowVideoInfo;
	BOOL		m_bBigShow;

    BOOL        m_bBackground;

	bool         fixedScaling = false;
	float        scalingAmount = 1.0f;


	obs_sceneitem_crop startCrop;
	vec2         startItemPos;
	vec2         cropSize;
	OBSSceneItem stretchItem;
	ItemHandle   stretchHandle = ItemHandle::None;
	vec2         stretchItemSize;
	matrix4      screenToItem;
	matrix4      itemToScreen;

	vec2         startPos;
	vec2         lastMoveOffset;
	vec2         scrollingFrom;
	vec2         scrollingOffset;
	bool         mouseDown = false;
	bool         mouseMoved = false;
	bool         mouseOverItems = false;
	bool         cropping = false;
	bool         locked = false;
	bool         scrollMode = false;

public:
   
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
};


