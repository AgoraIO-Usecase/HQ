// AGVideoWnd.cpp : 实现文件
//

#include "stdafx.h"
#include "AgoraHQ.h"
#include "AgoraHQDlg.h"
#include "BasicPreviewWnd.h"
#include <graphics/vec2.h>
#include <graphics/matrix4.h>
#include <algorithm>
#include <cmath>
#include <graphics/vec4.h>

// CBasicPreviewWnd
#define HANDLE_RADIUS     4.0f
#define HANDLE_SEL_RADIUS (HANDLE_RADIUS * 1.5f)

struct HandleFindData {
	const vec2   &pos;
	const float  scale;

	OBSSceneItem item;
	ItemHandle   handle = ItemHandle::None;

	HandleFindData(const HandleFindData &) = delete;
	HandleFindData(HandleFindData &&) = delete;
	HandleFindData& operator=(const HandleFindData &) = delete;
	HandleFindData& operator=(HandleFindData &&) = delete;

	inline HandleFindData(const vec2 &pos_, float scale_)
		: pos(pos_),
		scale(scale_)
	{}
};

struct SceneFindData {
	const vec2   &pos;
	OBSSceneItem item;
	bool         selectBelow;

	SceneFindData(const SceneFindData &) = delete;
	SceneFindData(SceneFindData &&) = delete;
	SceneFindData& operator=(const SceneFindData &) = delete;
	SceneFindData& operator=(SceneFindData &&) = delete;

	inline SceneFindData(const vec2 &pos_, bool selectBelow_)
		: pos(pos_),
		selectBelow(selectBelow_)
	{}
};

static bool SceneItemHasVideo(obs_sceneitem_t *item)
{
	obs_source_t *source = obs_sceneitem_get_source(item);
	uint32_t flags = obs_source_get_output_flags(source);
	return (flags & OBS_SOURCE_VIDEO) != 0;
}

static bool CloseFloat(float a, float b, float epsilon = 0.01)
{
	using std::abs;
	return abs(a - b) <= epsilon;
}

static inline bool crop_enabled(const obs_sceneitem_crop *crop)
{
	return crop->left > 0 ||
		crop->top > 0 ||
		crop->right > 0 ||
		crop->bottom > 0;
}


static void DrawCircleAtPos(float x, float y, matrix4 &matrix,
	float previewScale)
{
	struct vec3 pos;
	vec3_set(&pos, x, y, 0.0f);
	vec3_transform(&pos, &pos, &matrix);
	vec3_mulf(&pos, &pos, previewScale);

	gs_matrix_push();
	gs_matrix_translate(&pos);
	gs_matrix_scale3f(HANDLE_RADIUS, HANDLE_RADIUS, 1.0f);
	gs_draw(GS_LINESTRIP, 0, 0);
	gs_matrix_pop();
}

static vec3 GetTransformedPosScaled(float x, float y, const matrix4 &mat,
	float scale)
{
	vec3 result;
	vec3_set(&result, x, y, 0.0f);
	vec3_transform(&result, &result, &mat);
	vec3_mulf(&result, &result, scale);
	return result;
}

static bool FindHandleAtPos(obs_scene_t *scene, obs_sceneitem_t *item,
	void *param)
{
	if (!obs_sceneitem_selected(item))
		return true;

	HandleFindData *data = reinterpret_cast<HandleFindData*>(param);
	matrix4        transform;
	vec3           pos3;
	float          closestHandle = HANDLE_SEL_RADIUS;

	vec3_set(&pos3, data->pos.x, data->pos.y, 0.0f);

	obs_sceneitem_get_box_transform(item, &transform);

	auto TestHandle = [&](float x, float y, ItemHandle handle)
	{
		vec3 handlePos = GetTransformedPosScaled(x, y, transform,
			data->scale);

		float dist = vec3_dist(&handlePos, &pos3);
		if (dist < HANDLE_SEL_RADIUS) {
			if (dist < closestHandle) {
				closestHandle = dist;
				data->handle = handle;
				data->item = item;
			}
		}
	};

	TestHandle(0.0f, 0.0f, ItemHandle::TopLeft);
	TestHandle(0.5f, 0.0f, ItemHandle::TopCenter);
	TestHandle(1.0f, 0.0f, ItemHandle::TopRight);
	TestHandle(0.0f, 0.5f, ItemHandle::CenterLeft);
	TestHandle(1.0f, 0.5f, ItemHandle::CenterRight);
	TestHandle(0.0f, 1.0f, ItemHandle::BottomLeft);
	TestHandle(0.5f, 1.0f, ItemHandle::BottomCenter);
	TestHandle(1.0f, 1.0f, ItemHandle::BottomRight);

	UNUSED_PARAMETER(scene);
	return true;
}


static bool FindItemAtPos(obs_scene_t *scene, obs_sceneitem_t *item,
	void *param)
{
	SceneFindData *data = reinterpret_cast<SceneFindData*>(param);
	matrix4       transform;
	matrix4       invTransform;
	vec3          transformedPos;
	vec3          pos3;
	vec3          pos3_;

	if (!SceneItemHasVideo(item))
		return true;
	if (obs_sceneitem_locked(item))
		return true;

	vec3_set(&pos3, data->pos.x, data->pos.y, 0.0f);

	obs_sceneitem_get_box_transform(item, &transform);

	matrix4_inv(&invTransform, &transform);
	vec3_transform(&transformedPos, &pos3, &invTransform);
	vec3_transform(&pos3_, &transformedPos, &transform);

	if (CloseFloat(pos3.x, pos3_.x) && CloseFloat(pos3.y, pos3_.y) &&
		transformedPos.x >= 0.0f && transformedPos.x <= 1.0f &&
		transformedPos.y >= 0.0f && transformedPos.y <= 1.0f) {
		if (data->selectBelow && obs_sceneitem_selected(item)) {
			if (data->item)
				return false;
			else
				data->selectBelow = false;
		}

		data->item = item;
	}

	UNUSED_PARAMETER(scene);
	return true;
}

static vec3 GetTransformedPos(float x, float y, const matrix4 &mat)
{
	vec3 result;
	vec3_set(&result, x, y, 0.0f);
	vec3_transform(&result, &result, &mat);
	return result;
}

static inline vec2 GetOBSScreenSize()
{
	obs_video_info ovi;
	vec2 size;
	vec2_zero(&size);

	if (obs_get_video_info(&ovi)) {
		size.x = float(ovi.base_width);
		size.y = float(ovi.base_height);
	}

	return size;
}


IMPLEMENT_DYNAMIC(CBasicPreviewWnd, CDisplaySourceWnd)

CBasicPreviewWnd::CBasicPreviewWnd()
: m_nUID(0)
, m_crBackColor(RGB(0x58, 0x58, 0x58))
, m_bShowVideoInfo(FALSE)
, m_bBigShow(FALSE)
, m_bBackground(FALSE)
{
	
}

CBasicPreviewWnd::~CBasicPreviewWnd()
{
	m_imgBackGround.DeleteImageList();
}


BEGIN_MESSAGE_MAP(CBasicPreviewWnd, CDisplaySourceWnd)
	ON_WM_ERASEBKGND()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_RBUTTONDOWN()
	ON_WM_CREATE()
	ON_WM_PAINT()
	ON_WM_SIZE()
	ON_WM_LBUTTONDBLCLK()
    ON_WM_PAINT()
	ON_WM_MOUSEMOVE()
	ON_WM_RBUTTONUP()
END_MESSAGE_MAP()



// CBasicPreviewWnd 消息处理程序
BOOL CBasicPreviewWnd::OnEraseBkgnd(CDC* pDC)
{
	return TRUE;
}

BOOL CBasicPreviewWnd::IsWndFree()
{
	return m_nUID == 0 ? TRUE : FALSE;
}

void CBasicPreviewWnd::ShowBackground(BOOL bBackground)
{
    m_bBackground = bBackground;

    Invalidate(TRUE);
}

void CBasicPreviewWnd::SetFaceColor(COLORREF crBackColor)
{
	m_crBackColor = crBackColor;

	Invalidate(TRUE);
}

static bool select_one(obs_scene_t *scene, obs_sceneitem_t *item, void *param)
{
	obs_sceneitem_t *selectedItem =
		reinterpret_cast<obs_sceneitem_t*>(param);
	obs_sceneitem_select(item, (selectedItem == item));

	UNUSED_PARAMETER(scene);
	return true;
}

void CBasicPreviewWnd::DoSelect(const POINT &cur_pos)
{
	CAgoraHQDlg *main = reinterpret_cast<CAgoraHQDlg*>(theApp.GetMainWindow());
	OBSScene     scene = main->GetCurrentScene();
	vec2 pos = { cur_pos.x, cur_pos.y };
	OBSSceneItem item = GetItemAtPos(pos, true);

	obs_scene_enum_items(scene, select_one, (obs_sceneitem_t*)item);
}

void CBasicPreviewWnd::DoCtrlSelect(const POINT &cur_pos)
{
	vec2 pos = { cur_pos.x, cur_pos.y };
	OBSSceneItem item = GetItemAtPos(pos, false);
	if (!item)
		return;

	bool selected = obs_sceneitem_selected(item);
	obs_sceneitem_select(item, !selected);
}

void CBasicPreviewWnd::ProcessClick(const POINT &pos, bool bCtrl)
{
	if (bCtrl)
		DoCtrlSelect(pos);
	else
		DoSelect(pos);
}

void CBasicPreviewWnd::OnLButtonUp(UINT nFlags, CPoint point)
{
	if (mouseDown) {
		
		POINT ptCursor;
		GetCursorPos(&ptCursor);
		ScreenToClient(&ptCursor);

		vec2 pos = GetMouseEventPos(ptCursor);
		if (!mouseMoved)
		{
			bool bCtrl = false;
			if (nFlags & MK_CONTROL)
				bCtrl = true;
			POINT pt = { ptCursor.x, ptCursor.y };
			ProcessClick(ptCursor, bCtrl);
		}
		stretchItem = nullptr;
		mouseDown = false;
		mouseMoved = false;
		cropping = false;
	}
	CWnd::OnLButtonUp(nFlags, point);
}

void CBasicPreviewWnd::OnLButtonDown(UINT nFlags, CPoint point)
{
	POINT ptCursor;
	GetCursorPos(&ptCursor);
	ScreenToClient(&ptCursor);
	if (scrollMode && IsFixedScaling()) {
		scrollingFrom.x = ptCursor.x;
		scrollingFrom.y = ptCursor.y;
		return;
	}

	CAgoraHQDlg *main = reinterpret_cast<CAgoraHQDlg*>(theApp.GetMainWindow());
//	float pixelRatio = main->devicePixelRatio();
	float pixelRatio = 1.0f;
	float x = float(ptCursor.x) - main->previewX / pixelRatio;
 	float y = float(ptCursor.y) - main->previewY / pixelRatio;
	bool altDown = MK_ALT&nFlags;//(modifiers & Qt::AltModifier);

	//OBSQTDisplay::mousePressEvent(event);

	
	mouseDown = true;

	if (altDown)
	    cropping = true;

	vec2_set(&startPos, x, y);
	GetStretchHandleData(startPos);

	vec2_divf(&startPos, &startPos, main->previewScale / pixelRatio);
	startPos.x = std::round(startPos.x);
	startPos.y = std::round(startPos.y);

	mouseOverItems = SelectedAtPos(startPos);
	vec2_zero(&lastMoveOffset);
	::SendMessage(GetParent()->GetSafeHwnd(), WM_SHOWBIG, (WPARAM)this, (LPARAM)m_nUID);

	CWnd::OnLButtonDown(nFlags, point);
}

void CBasicPreviewWnd::OnMouseMove(UINT nFlags, CPoint point)
{
	// 	if (scrollMode && nFlags&MK_LBUTTON) {
	// 		scrollingOffset.x += event->x() - scrollingFrom.x;
	// 		scrollingOffset.y += event->y() - scrollingFrom.y;
	// 		scrollingFrom.x = event->x();
	// 		scrollingFrom.y = event->y();
	// 		
	// 		//emit DisplayResized();
	// 		return;
	// 	}


	if (mouseDown) {
		POINT ptCursor;
		GetCursorPos(&ptCursor);
		ScreenToClient(&ptCursor);
	
		vec2 pos = GetMouseEventPos(ptCursor);
		if (!mouseMoved && !mouseOverItems &&
			stretchHandle == ItemHandle::None) {
			POINT pt = { startPos.x, startPos.y };
			ProcessClick(pt, nFlags&MK_CONTROL);
			mouseOverItems = SelectedAtPos(startPos);
		}

		pos.x = std::round(pos.x);
		pos.y = std::round(pos.y);

		if (stretchHandle != ItemHandle::None) {
			if (cropping)
				CropItem(pos);
			else
				StretchItem(pos, nFlags);

		}
		else if (mouseOverItems) {
			MoveItems(pos, nFlags);
		}

		mouseMoved = true;
		CDisplaySourceWnd::OnMouseMove(nFlags, point);
	}
}

void CBasicPreviewWnd::ClampAspect(vec3 &tl, vec3 &br, vec2 &size,
	const vec2 &baseSize)
{
	float    baseAspect = baseSize.x / baseSize.y;
	float    aspect = size.x / size.y;
	uint32_t stretchFlags = (uint32_t)stretchHandle;

	if (stretchHandle == ItemHandle::TopLeft ||
		stretchHandle == ItemHandle::TopRight ||
		stretchHandle == ItemHandle::BottomLeft ||
		stretchHandle == ItemHandle::BottomRight) {
		if (aspect < baseAspect) {
			if ((size.y >= 0.0f && size.x >= 0.0f) ||
				(size.y <= 0.0f && size.x <= 0.0f))
				size.x = size.y * baseAspect;
			else
				size.x = size.y * baseAspect * -1.0f;
		}
		else {
			if ((size.y >= 0.0f && size.x >= 0.0f) ||
				(size.y <= 0.0f && size.x <= 0.0f))
				size.y = size.x / baseAspect;
			else
				size.y = size.x / baseAspect * -1.0f;
		}

	}
	else if (stretchHandle == ItemHandle::TopCenter ||
		stretchHandle == ItemHandle::BottomCenter) {
		if ((size.y >= 0.0f && size.x >= 0.0f) ||
			(size.y <= 0.0f && size.x <= 0.0f))
			size.x = size.y * baseAspect;
		else
			size.x = size.y * baseAspect * -1.0f;

	}
	else if (stretchHandle == ItemHandle::CenterLeft ||
		stretchHandle == ItemHandle::CenterRight) {
		if ((size.y >= 0.0f && size.x >= 0.0f) ||
			(size.y <= 0.0f && size.x <= 0.0f))
			size.y = size.x / baseAspect;
		else
			size.y = size.x / baseAspect * -1.0f;
	}

	size.x = std::round(size.x);
	size.y = std::round(size.y);

	if (stretchFlags & ITEM_LEFT)
		tl.x = br.x - size.x;
	else if (stretchFlags & ITEM_RIGHT)
		br.x = tl.x + size.x;

	if (stretchFlags & ITEM_TOP)
		tl.y = br.y - size.y;
	else if (stretchFlags & ITEM_BOTTOM)
		br.y = tl.y + size.y;
}

void CBasicPreviewWnd::SnapStretchingToScreen(vec3 &tl, vec3 &br)
{
	uint32_t stretchFlags = (uint32_t)stretchHandle;
	vec3     newTL = GetTransformedPos(tl.x, tl.y, itemToScreen);
	vec3     newTR = GetTransformedPos(br.x, tl.y, itemToScreen);
	vec3     newBL = GetTransformedPos(tl.x, br.y, itemToScreen);
	vec3     newBR = GetTransformedPos(br.x, br.y, itemToScreen);
	vec3     boundingTL;
	vec3     boundingBR;

	vec3_copy(&boundingTL, &newTL);
	vec3_min(&boundingTL, &boundingTL, &newTR);
	vec3_min(&boundingTL, &boundingTL, &newBL);
	vec3_min(&boundingTL, &boundingTL, &newBR);

	vec3_copy(&boundingBR, &newTL);
	vec3_max(&boundingBR, &boundingBR, &newTR);
	vec3_max(&boundingBR, &boundingBR, &newBL);
	vec3_max(&boundingBR, &boundingBR, &newBR);

	vec3 offset = GetSnapOffset(boundingTL, boundingBR);
	vec3_add(&offset, &offset, &newTL);
	vec3_transform(&offset, &offset, &screenToItem);
	vec3_sub(&offset, &offset, &tl);

	if (stretchFlags & ITEM_LEFT)
		tl.x += offset.x;
	else if (stretchFlags & ITEM_RIGHT)
		br.x += offset.x;

	if (stretchFlags & ITEM_TOP)
		tl.y += offset.y;
	else if (stretchFlags & ITEM_BOTTOM)
		br.y += offset.y;
}


static float maxfunc(float x, float y)
{
	return x > y ? x : y;
}

static float minfunc(float x, float y)
{
	return x < y ? x : y;
}

void CBasicPreviewWnd::CropItem(const vec2 &pos)
{
	obs_bounds_type boundsType = obs_sceneitem_get_bounds_type(stretchItem);
	uint32_t stretchFlags = (uint32_t)stretchHandle;
	uint32_t align = obs_sceneitem_get_alignment(stretchItem);
	vec3 tl, br, pos3;

	if (boundsType != OBS_BOUNDS_NONE) /* TODO */
		return;

	vec3_zero(&tl);
	vec3_set(&br, stretchItemSize.x, stretchItemSize.y, 0.0f);

	vec3_set(&pos3, pos.x, pos.y, 0.0f);
	vec3_transform(&pos3, &pos3, &screenToItem);

	obs_sceneitem_crop crop = startCrop;
	vec2 scale;

	obs_sceneitem_get_scale(stretchItem, &scale);

	vec2 max_tl;
	vec2 max_br;

	vec2_set(&max_tl,
		float(-crop.left) * scale.x,
		float(-crop.top) * scale.y);
	vec2_set(&max_br,
		stretchItemSize.x + crop.right * scale.x,
		stretchItemSize.y + crop.bottom * scale.y);

	typedef std::function<float(float, float)> minmax_func_t;

	minmax_func_t min_x = scale.x < 0.0f ? maxfunc : minfunc;
	minmax_func_t min_y = scale.y < 0.0f ? maxfunc : minfunc;
	minmax_func_t max_x = scale.x < 0.0f ? minfunc : maxfunc;
	minmax_func_t max_y = scale.y < 0.0f ? minfunc : maxfunc;

	pos3.x = min_x(pos3.x, max_br.x);
	pos3.x = max_x(pos3.x, max_tl.x);
	pos3.y = min_y(pos3.y, max_br.y);
	pos3.y = max_y(pos3.y, max_tl.y);

	if (stretchFlags & ITEM_LEFT) {
		float maxX = stretchItemSize.x - (2.0 * scale.x);
		pos3.x = tl.x = min_x(pos3.x, maxX);

	}
	else if (stretchFlags & ITEM_RIGHT) {
		float minX = (2.0 * scale.x);
		pos3.x = br.x = max_x(pos3.x, minX);
	}

	if (stretchFlags & ITEM_TOP) {
		float maxY = stretchItemSize.y - (2.0 * scale.y);
		pos3.y = tl.y = min_y(pos3.y, maxY);

	}
	else if (stretchFlags & ITEM_BOTTOM) {
		float minY = (2.0 * scale.y);
		pos3.y = br.y = max_y(pos3.y, minY);
	}

#define ALIGN_X (ITEM_LEFT|ITEM_RIGHT)
#define ALIGN_Y (ITEM_TOP|ITEM_BOTTOM)
	vec3 newPos;
	vec3_zero(&newPos);

	uint32_t align_x = (align & ALIGN_X);
	uint32_t align_y = (align & ALIGN_Y);
	if (align_x == (stretchFlags & ALIGN_X) && align_x != 0)
		newPos.x = pos3.x;
	else if (align & ITEM_RIGHT)
		newPos.x = stretchItemSize.x;
	else if (!(align & ITEM_LEFT))
		newPos.x = stretchItemSize.x * 0.5f;

	if (align_y == (stretchFlags & ALIGN_Y) && align_y != 0)
		newPos.y = pos3.y;
	else if (align & ITEM_BOTTOM)
		newPos.y = stretchItemSize.y;
	else if (!(align & ITEM_TOP))
		newPos.y = stretchItemSize.y * 0.5f;
#undef ALIGN_X
#undef ALIGN_Y

	crop = startCrop;

	if (stretchFlags & ITEM_LEFT)
		crop.left += int(std::round(tl.x / scale.x));
	else if (stretchFlags & ITEM_RIGHT)
		crop.right += int(std::round((stretchItemSize.x - br.x) / scale.x));

	if (stretchFlags & ITEM_TOP)
		crop.top += int(std::round(tl.y / scale.y));
	else if (stretchFlags & ITEM_BOTTOM)
		crop.bottom += int(std::round((stretchItemSize.y - br.y) / scale.y));

	vec3_transform(&newPos, &newPos, &itemToScreen);
	newPos.x = std::round(newPos.x);
	newPos.y = std::round(newPos.y);

#if 0
	vec3 curPos;
	vec3_zero(&curPos);
	obs_sceneitem_get_pos(stretchItem, (vec2*)&curPos);
	blog(LOG_DEBUG, "curPos {%d, %d} - newPos {%d, %d}",
		int(curPos.x), int(curPos.y),
		int(newPos.x), int(newPos.y));
	blog(LOG_DEBUG, "crop {%d, %d, %d, %d}",
		crop.left, crop.top,
		crop.right, crop.bottom);
#endif

	obs_sceneitem_defer_update_begin(stretchItem);
	obs_sceneitem_set_crop(stretchItem, &crop);
	obs_sceneitem_set_pos(stretchItem, (vec2*)&newPos);
	obs_sceneitem_defer_update_end(stretchItem);
}


void CBasicPreviewWnd::StretchItem(const vec2 &pos, UINT nFlags)
{
	obs_bounds_type boundsType = obs_sceneitem_get_bounds_type(stretchItem);
	uint32_t stretchFlags = (uint32_t)stretchHandle;
	bool shiftDown = MK_SHIFT&nFlags;
	vec3 tl, br, pos3;

	vec3_zero(&tl);
	vec3_set(&br, stretchItemSize.x, stretchItemSize.y, 0.0f);

	vec3_set(&pos3, pos.x, pos.y, 0.0f);
	vec3_transform(&pos3, &pos3, &screenToItem);

	if (stretchFlags & ITEM_LEFT)
		tl.x = pos3.x;
	else if (stretchFlags & ITEM_RIGHT)
		br.x = pos3.x;

	if (stretchFlags & ITEM_TOP)
		tl.y = pos3.y;
	else if (stretchFlags & ITEM_BOTTOM)
		br.y = pos3.y;

	if (!(MK_CONTROL & nFlags))
		SnapStretchingToScreen(tl, br);

	obs_source_t *source = obs_sceneitem_get_source(stretchItem);

	vec2 baseSize;
	vec2_set(&baseSize,
		float(obs_source_get_width(source)),
		float(obs_source_get_height(source)));

	vec2 size;
	vec2_set(&size, br.x - tl.x, br.y - tl.y);

	if (boundsType != OBS_BOUNDS_NONE) {
		if (shiftDown)
			ClampAspect(tl, br, size, baseSize);

		if (tl.x > br.x) std::swap(tl.x, br.x);
		if (tl.y > br.y) std::swap(tl.y, br.y);

		vec2_abs(&size, &size);

		obs_sceneitem_set_bounds(stretchItem, &size);
	}
	else {
		obs_sceneitem_crop crop;
		obs_sceneitem_get_crop(stretchItem, &crop);

		baseSize.x -= float(crop.left + crop.right);
		baseSize.y -= float(crop.top + crop.bottom);

		if (!shiftDown)
			ClampAspect(tl, br, size, baseSize);

		vec2_div(&size, &size, &baseSize);
		obs_sceneitem_set_scale(stretchItem, &size);
	}

	pos3 = CalculateStretchPos(tl, br);
	vec3_transform(&pos3, &pos3, &itemToScreen);

	vec2 newPos;
	vec2_set(&newPos, std::round(pos3.x), std::round(pos3.y));
	obs_sceneitem_set_pos(stretchItem, &newPos);
}



static bool move_items(obs_scene_t *scene, obs_sceneitem_t *item, void *param)
{
	if (obs_sceneitem_locked(item))
		return true;

	vec2 *offset = reinterpret_cast<vec2*>(param);

	if (obs_sceneitem_selected(item)) {
		vec2 pos;
		obs_sceneitem_get_pos(item, &pos);
		vec2_add(&pos, &pos, offset);
		obs_sceneitem_set_pos(item, &pos);
	}

	UNUSED_PARAMETER(scene);
	return true;
}

void CBasicPreviewWnd::MoveItems(const vec2 &pos, UINT nFlags)
{	
	CAgoraHQDlg *main = reinterpret_cast<CAgoraHQDlg*>(theApp.GetMainWindow());
	OBSScene scene = main->GetCurrentScene();

	vec2 offset, moveOffset;
	vec2_sub(&offset, &pos, &startPos);
	vec2_sub(&moveOffset, &offset, &lastMoveOffset);

	if (!(nFlags & MK_CONTROL))
		SnapItemMovement(moveOffset);

	vec2_add(&lastMoveOffset, &lastMoveOffset, &moveOffset);

	obs_scene_enum_items(scene, move_items, &moveOffset);
}


struct SelectedItemBounds {
	bool first = true;
	vec3 tl, br;
};

static bool AddItemBounds(obs_scene_t *scene, obs_sceneitem_t *item,
	void *param)
{
	SelectedItemBounds *data = reinterpret_cast<SelectedItemBounds*>(param);

	if (!obs_sceneitem_selected(item))
		return true;

	matrix4 boxTransform;
	obs_sceneitem_get_box_transform(item, &boxTransform);

	vec3 t[4] = {
		GetTransformedPos(0.0f, 0.0f, boxTransform),
		GetTransformedPos(1.0f, 0.0f, boxTransform),
		GetTransformedPos(0.0f, 1.0f, boxTransform),
		GetTransformedPos(1.0f, 1.0f, boxTransform)
	};

	for (const vec3 &v : t) {
		if (data->first) {
			vec3_copy(&data->tl, &v);
			vec3_copy(&data->br, &v);
			data->first = false;
		}
		else {
			vec3_min(&data->tl, &data->tl, &v);
			vec3_max(&data->br, &data->br, &v);
		}
	}

	UNUSED_PARAMETER(scene);
	return true;
}

struct OffsetData {
	float clampDist;
	vec3 tl, br, offset;
};


static bool GetSourceSnapOffset(obs_scene_t *scene, obs_sceneitem_t *item,
	void *param)
{
	OffsetData *data = reinterpret_cast<OffsetData*>(param);

	if (obs_sceneitem_selected(item))
		return true;

	matrix4 boxTransform;
	obs_sceneitem_get_box_transform(item, &boxTransform);

	vec3 t[4] = {
		GetTransformedPos(0.0f, 0.0f, boxTransform),
		GetTransformedPos(1.0f, 0.0f, boxTransform),
		GetTransformedPos(0.0f, 1.0f, boxTransform),
		GetTransformedPos(1.0f, 1.0f, boxTransform)
	};

	bool first = true;
	vec3 tl, br;
	vec3_zero(&tl);
	vec3_zero(&br);
	for (const vec3 &v : t) {
		if (first) {
			vec3_copy(&tl, &v);
			vec3_copy(&br, &v);
			first = false;
		}
		else {
			vec3_min(&tl, &tl, &v);
			vec3_max(&br, &br, &v);
		}
	}

	// Snap to other source edges
#define EDGE_SNAP(l, r, x, y) \
	do { \
		double dist = fabsf(l.x - data->r.x); \
		if (dist < data->clampDist && \
		    fabsf(data->offset.x) < EPSILON && \
		    data->tl.y < br.y && \
		    data->br.y > tl.y && \
		    (fabsf(data->offset.x) > dist || data->offset.x < EPSILON)) \
			data->offset.x = l.x - data->r.x; \
			} while (false)

	EDGE_SNAP(tl, br, x, y);
	EDGE_SNAP(tl, br, y, x);
	EDGE_SNAP(br, tl, x, y);
	EDGE_SNAP(br, tl, y, x);
#undef EDGE_SNAP

	UNUSED_PARAMETER(scene);
	return true;
}

vec2 CBasicPreviewWnd::GetMouseEventPos(POINT pt)
{
	CAgoraHQDlg *main = reinterpret_cast<CAgoraHQDlg*>(theApp.GetMainWindow());
	float pixelRatio = 1.0f;//main->devicePixelRatio();
	float scale = pixelRatio / main->previewScale;
	vec2 pos;
	vec2_set(&pos,
		(float(pt.x) - main->previewX / pixelRatio) * scale,
		(float(pt.y) - main->previewY / pixelRatio) * scale);

	return pos;
}

vec3 CBasicPreviewWnd::GetSnapOffset(const vec3 &tl, const vec3 &br)
{
	CAgoraHQDlg *main = reinterpret_cast<CAgoraHQDlg*>(theApp.GetMainWindow());
	vec2 screenSize = GetOBSScreenSize();
	vec3 clampOffset;

	vec3_zero(&clampOffset);

	const bool snap = config_get_bool(GetGlobalConfig(),
		"BasicWindow", "SnappingEnabled");
	if (snap == false)
		return clampOffset;

	const bool screenSnap = config_get_bool(GetGlobalConfig(),
		"BasicWindow", "ScreenSnapping");
	const bool centerSnap = config_get_bool(GetGlobalConfig(),
		"BasicWindow", "CenterSnapping");

	const float clampDist = config_get_double(GetGlobalConfig(),
		"BasicWindow", "SnapDistance") / main->previewScale;
	const float centerX = br.x - (br.x - tl.x) / 2.0f;
	const float centerY = br.y - (br.y - tl.y) / 2.0f;

	// Left screen edge.
	if (screenSnap &&
		fabsf(tl.x) < clampDist)
		clampOffset.x = -tl.x;
	// Right screen edge.
	if (screenSnap &&
		fabsf(clampOffset.x) < EPSILON &&
		fabsf(screenSize.x - br.x) < clampDist)
		clampOffset.x = screenSize.x - br.x;
	// Horizontal center.
	if (centerSnap &&
		fabsf(screenSize.x - (br.x - tl.x)) > clampDist &&
		fabsf(screenSize.x / 2.0f - centerX) < clampDist)
		clampOffset.x = screenSize.x / 2.0f - centerX;

	// Top screen edge.
	if (screenSnap &&
		fabsf(tl.y) < clampDist)
		clampOffset.y = -tl.y;
	// Bottom screen edge.
	if (screenSnap &&
		fabsf(clampOffset.y) < EPSILON &&
		fabsf(screenSize.y - br.y) < clampDist)
		clampOffset.y = screenSize.y - br.y;
	// Vertical center.
	if (centerSnap &&
		fabsf(screenSize.y - (br.y - tl.y)) > clampDist &&
		fabsf(screenSize.y / 2.0f - centerY) < clampDist)
		clampOffset.y = screenSize.y / 2.0f - centerY;

	return clampOffset;
}

void CBasicPreviewWnd::SnapItemMovement(vec2 &offset)
{
	CAgoraHQDlg *main = reinterpret_cast<CAgoraHQDlg*>(theApp.GetMainWindow());
	OBSScene scene = main->GetCurrentScene();

	SelectedItemBounds data;
	obs_scene_enum_items(scene, AddItemBounds, &data);

	data.tl.x += offset.x;
	data.tl.y += offset.y;
	data.br.x += offset.x;
	data.br.y += offset.y;

	vec3 snapOffset = GetSnapOffset(data.tl, data.br);

	const bool snap = config_get_bool(GetGlobalConfig(),
		"BasicWindow", "SnappingEnabled");
	const bool sourcesSnap = config_get_bool(GetGlobalConfig(),
		"BasicWindow", "SourceSnapping");
	if (snap == false)
		return;
	if (sourcesSnap == false) {
		offset.x += snapOffset.x;
		offset.y += snapOffset.y;
		return;
	}

	const float clampDist = config_get_double(GetGlobalConfig(),
		"BasicWindow", "SnapDistance") / main->previewScale;

	OffsetData offsetData;
	offsetData.clampDist = clampDist;
	offsetData.tl = data.tl;
	offsetData.br = data.br;
	vec3_copy(&offsetData.offset, &snapOffset);

	obs_scene_enum_items(scene, GetSourceSnapOffset, &offsetData);

	if (fabsf(offsetData.offset.x) > EPSILON ||
		fabsf(offsetData.offset.y) > EPSILON) {
		offset.x += offsetData.offset.x;
		offset.y += offsetData.offset.y;
	}
	else {
		offset.x += snapOffset.x;
		offset.y += snapOffset.y;
	}
}

vec3 CBasicPreviewWnd::CalculateStretchPos(const vec3 &tl, const vec3 &br)
{
	uint32_t alignment = obs_sceneitem_get_alignment(stretchItem);
	vec3 pos;

	vec3_zero(&pos);

	if (alignment & OBS_ALIGN_LEFT)
		pos.x = tl.x;
	else if (alignment & OBS_ALIGN_RIGHT)
		pos.x = br.x;
	else
		pos.x = (br.x - tl.x) * 0.5f + tl.x;

	if (alignment & OBS_ALIGN_TOP)
		pos.y = tl.y;
	else if (alignment & OBS_ALIGN_BOTTOM)
		pos.y = br.y;
	else
		pos.y = (br.y - tl.y) * 0.5f + tl.y;

	return pos;
}


static bool CheckItemSelected(obs_scene_t *scene, obs_sceneitem_t *item,
	void *param)
{
	SceneFindData *data = reinterpret_cast<SceneFindData*>(param);
	matrix4       transform;
	vec3          transformedPos;
	vec3          pos3;

	if (!SceneItemHasVideo(item))
		return true;

	vec3_set(&pos3, data->pos.x, data->pos.y, 0.0f);

	obs_sceneitem_get_box_transform(item, &transform);

	matrix4_inv(&transform, &transform);
	vec3_transform(&transformedPos, &pos3, &transform);

	if (transformedPos.x >= 0.0f && transformedPos.x <= 1.0f &&
		transformedPos.y >= 0.0f && transformedPos.y <= 1.0f) {
		if (obs_sceneitem_selected(item)) {
			data->item = item;
			return false;
		}
	}

	UNUSED_PARAMETER(scene);
	return true;
}

bool CBasicPreviewWnd::SelectedAtPos(const vec2 &pos)
{
	CAgoraHQDlg *main = reinterpret_cast<CAgoraHQDlg*>(theApp.GetMainWindow());

	OBSScene scene = main->GetCurrentScene();
	if (!scene)
		return false;

	SceneFindData data(pos, false);
	obs_scene_enum_items(scene, CheckItemSelected, &data);
	return !!data.item;
}

OBSSceneItem CBasicPreviewWnd::GetItemAtPos(const vec2 &pos, bool selectBelow)
{
	CAgoraHQDlg *main = reinterpret_cast<CAgoraHQDlg*>(theApp.GetMainWindow());

	OBSScene scene = main->GetCurrentScene();
	if (!scene)
		return OBSSceneItem();

	SceneFindData data(pos, selectBelow);
	obs_scene_enum_items(scene, FindItemAtPos, &data);
	return data.item;
}


static vec2 GetItemSize(obs_sceneitem_t *item)
{
	obs_bounds_type boundsType = obs_sceneitem_get_bounds_type(item);
	vec2 size;

	if (boundsType != OBS_BOUNDS_NONE) {
		obs_sceneitem_get_bounds(item, &size);
	}
	else {
		obs_source_t *source = obs_sceneitem_get_source(item);
		obs_sceneitem_crop crop;
		vec2 scale;

		obs_sceneitem_get_scale(item, &scale);
		obs_sceneitem_get_crop(item, &crop);
		size.x = float(obs_source_get_width(source) -
			crop.left - crop.right) * scale.x;
		size.y = float(obs_source_get_height(source) -
			crop.top - crop.bottom) * scale.y;
	}

	return size;
}
void CBasicPreviewWnd::GetStretchHandleData(const vec2 &pos)
{
	CAgoraHQDlg *main = reinterpret_cast<CAgoraHQDlg*>(theApp.GetMainWindow());

	OBSScene scene = main->GetCurrentScene();
	if (!scene)
		return;

	HandleFindData data(pos, main->previewScale /1.0f/* main->devicePixelRatio()*/);
	obs_scene_enum_items(scene, FindHandleAtPos, &data);

	stretchItem = std::move(data.item);
	stretchHandle = data.handle;

	if (stretchHandle != ItemHandle::None) {
		matrix4 boxTransform;
		vec3    itemUL;
		float   itemRot;

		stretchItemSize = GetItemSize(stretchItem);

		obs_sceneitem_get_box_transform(stretchItem, &boxTransform);
		itemRot = obs_sceneitem_get_rot(stretchItem);
		vec3_from_vec4(&itemUL, &boxTransform.t);

		/* build the item space conversion matrices */
		matrix4_identity(&itemToScreen);
		matrix4_rotate_aa4f(&itemToScreen, &itemToScreen,
			0.0f, 0.0f, 1.0f, RAD(itemRot));
		matrix4_translate3f(&itemToScreen, &itemToScreen,
			itemUL.x, itemUL.y, 0.0f);

		matrix4_identity(&screenToItem);
		matrix4_translate3f(&screenToItem, &screenToItem,
			-itemUL.x, -itemUL.y, 0.0f);
		matrix4_rotate_aa4f(&screenToItem, &screenToItem,
			0.0f, 0.0f, 1.0f, RAD(-itemRot));

		obs_sceneitem_get_crop(stretchItem, &startCrop);
		obs_sceneitem_get_pos(stretchItem, &startItemPos);

		obs_source_t *source = obs_sceneitem_get_source(stretchItem);
		cropSize.x = float(obs_source_get_width(source) -
			startCrop.left - startCrop.right);
		cropSize.y = float(obs_source_get_height(source) -
			startCrop.top - startCrop.bottom);
	}
}


void CBasicPreviewWnd::OnRButtonDown(UINT nFlags, CPoint point)
{
	// TODO:  在此添加消息处理程序代码和/或调用默认值
	::SendMessage(GetParent()->GetSafeHwnd(), WM_SHOWMODECHANGED, (WPARAM)this, (LPARAM)m_nUID);

	CMenu menu;
	menu.CreatePopupMenu();
	menu.AppendMenu(MF_STRING, WM_OBS_SOURCE_DELETE, _T("Delete"));
	menu.AppendMenu(MF_STRING, WM_OBS_SOURCE_MOVEUP, _T("MoveUp"));
	menu.AppendMenu(MF_STRING, WM_OBS_SOURCE_MOVEDOWN, _T("MoveDown"));
	menu.AppendMenu(MF_STRING, WM_OBS_SOURCE_MOVETOP, _T("MoveTop"));
	menu.AppendMenu(MF_STRING, WM_OBS_SOURCE_MOVEBOTTOM, _T("MoveBottom"));
	menu.AppendMenu(MF_STRING, WM_OBS_SOURCE_FILTERS, _T("filters"));
	POINT tpoint;
	tpoint.x = point.x;
	tpoint.y = point.y;
	ClientToScreen(&tpoint);
	menu.TrackPopupMenu(TPM_LEFTALIGN, tpoint.x, tpoint.y, this);

	CWnd::OnRButtonDown(nFlags, point);
}

void CBasicPreviewWnd::OnRButtonUp(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default

	CDisplaySourceWnd::OnRButtonUp(nFlags, point);
}

int CBasicPreviewWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	CDisplaySourceWnd::SetParent(GetParent());//display创建成功通知父窗口
	// TODO:  在此添加您专用的创建代码
//	m_wndInfo.Create(NULL, NULL, WS_CHILD | WS_VISIBLE, CRect(0, 0, 192, 28), this, IDC_STATIC);

	return 0;
}

void CBasicPreviewWnd::OnSize(UINT nType, int cx, int cy)
{
	CDisplaySourceWnd::OnSize(nType, cx, cy);
}


void CBasicPreviewWnd::OnLButtonDblClk(UINT nFlags, CPoint point)
{

	::SendMessage(GetParent()->GetSafeHwnd(), WM_SHOWMODECHANGED, (WPARAM)this, (LPARAM)m_nUID);
	CWnd::OnLButtonDblClk(nFlags, point);
}


void CBasicPreviewWnd::OnPaint()
{
    CPaintDC dc(this); // device context for painting
    // TODO:  在此处添加消息处理程序代码
    // 不为绘图消息调用 CWnd::OnPaint()

    if (m_bBackground) {
        CRect		rcClient;
        CPoint		ptDraw;
        IMAGEINFO	imgInfo;

        GetClientRect(&rcClient);

        dc.FillSolidRect(&rcClient, m_crBackColor);
        if (!m_imgBackGround.GetImageInfo(0, &imgInfo))
            return;

        ptDraw.SetPoint((rcClient.Width() - imgInfo.rcImage.right) / 2, (rcClient.Height() - imgInfo.rcImage.bottom) / 2);
        if (ptDraw.x < 0)
            ptDraw.x = 0;
        if (ptDraw.y <= 0)
            ptDraw.y = 0;

        m_imgBackGround.Draw(&dc, 0, ptDraw, ILD_NORMAL);
    }
    else
        return CWnd::OnPaint();
}


void CBasicPreviewWnd::DrawSceneEditing()
{
	CAgoraHQDlg *main = reinterpret_cast<CAgoraHQDlg*>(theApp.GetMainWindow());

	gs_effect_t    *solid = obs_get_base_effect(OBS_EFFECT_SOLID);
	gs_technique_t *tech = gs_effect_get_technique(solid, "Solid");

	vec4 color;
	vec4_set(&color, 1.0f, 0.0f, 0.0f, 1.0f);
	gs_effect_set_vec4(gs_effect_get_param_by_name(solid, "color"), &color);

	gs_technique_begin(tech);
	gs_technique_begin_pass(tech, 0);

	OBSScene scene = main->GetCurrentScene();

	if (scene)
		obs_scene_enum_items(scene, DrawSelectedItem, this);

	gs_load_vertexbuffer(nullptr);

	gs_technique_end_pass(tech);
	gs_technique_end(tech);
}

bool CBasicPreviewWnd::DrawSelectedItem(obs_scene_t *scene,
	obs_sceneitem_t *item, void *param)
{
	if (obs_sceneitem_locked(item))
		return true;

	if (!obs_sceneitem_selected(item))
		return true;
	CAgoraHQDlg *main = reinterpret_cast<CAgoraHQDlg*>(theApp.GetMainWindow());

	matrix4 boxTransform;
	matrix4 invBoxTransform;
	obs_sceneitem_get_box_transform(item, &boxTransform);
	matrix4_inv(&invBoxTransform, &boxTransform);

	vec3 bounds[] = {
		{ { { 0.f, 0.f, 0.f } } },
		{ { { 1.f, 0.f, 0.f } } },
		{ { { 0.f, 1.f, 0.f } } },
		{ { { 1.f, 1.f, 0.f } } },
	};

	bool visible = std::all_of(std::begin(bounds), std::end(bounds),
		[&](const vec3 &b)
	{
		vec3 pos;
		vec3_transform(&pos, &b, &boxTransform);
		vec3_transform(&pos, &pos, &invBoxTransform);
		return CloseFloat(pos.x, b.x) && CloseFloat(pos.y, b.y);
	});

	if (!visible)
		return true;

	obs_transform_info info;
	obs_sceneitem_get_info(item, &info);

	gs_load_vertexbuffer(main->circle);

	DrawCircleAtPos(0.0f, 0.0f, boxTransform, main->previewScale);
	DrawCircleAtPos(0.0f, 1.0f, boxTransform, main->previewScale);
	DrawCircleAtPos(1.0f, 0.0f, boxTransform, main->previewScale);
	DrawCircleAtPos(1.0f, 1.0f, boxTransform, main->previewScale);
	DrawCircleAtPos(0.5f, 0.0f, boxTransform, main->previewScale);
	DrawCircleAtPos(0.0f, 0.5f, boxTransform, main->previewScale);
	DrawCircleAtPos(0.5f, 1.0f, boxTransform, main->previewScale);
	DrawCircleAtPos(1.0f, 0.5f, boxTransform, main->previewScale);

	gs_matrix_push();
	gs_matrix_scale3f(main->previewScale, main->previewScale, 1.0f);
	gs_matrix_mul(&boxTransform);

	obs_sceneitem_crop crop;
	obs_sceneitem_get_crop(item, &crop);

	if (info.bounds_type == OBS_BOUNDS_NONE && crop_enabled(&crop)) {
		vec4 color;
		gs_effect_t *eff = gs_get_effect();
		gs_eparam_t *param = gs_effect_get_param_by_name(eff, "color");

#define DRAW_SIDE(side, vb) \
		if (crop.side > 0) \
			vec4_set(&color, 0.0f, 1.0f, 0.0f, 1.0f); \
				else \
			vec4_set(&color, 1.0f, 0.0f, 0.0f, 1.0f); \
		gs_effect_set_vec4(param, &color); \
		gs_load_vertexbuffer(main->vb); \
		gs_draw(GS_LINESTRIP, 0, 0);

		DRAW_SIDE(left, boxLeft);
		DRAW_SIDE(top, boxTop);
		DRAW_SIDE(right, boxRight);
		DRAW_SIDE(bottom, boxBottom);
#undef DRAW_SIDE
	}
	else {
		gs_load_vertexbuffer(main->box);
		gs_draw(GS_LINESTRIP, 0, 0);
	}

	gs_matrix_pop();

	UNUSED_PARAMETER(scene);
	UNUSED_PARAMETER(param);
	return true;
}

void CBasicPreviewWnd::ResizeDisplay()
{
	if (::IsWindowVisible(m_hWnd) && display) 
	{
		CRect		rcClient;
		GetClientRect(&rcClient);
		obs_display_resize(display, rcClient.right - rcClient.left, rcClient.bottom - rcClient.top);
	}
}

BOOL CBasicPreviewWnd::OnCommand(WPARAM wParam, LPARAM lParam)
{
	switch (LOWORD(wParam))
	{
	case WM_OBS_SOURCE_DELETE:
		::SendMessage(GetParent()->GetSafeHwnd(), WM_OBS_SOURCE_DELETE, 0, 0);
		break;
	case WM_OBS_SOURCE_MOVEUP:
		::SendMessage(GetParent()->GetSafeHwnd(), WM_OBS_SOURCE_MOVEUP, 0, 0);
		break;
	case WM_OBS_SOURCE_MOVEDOWN:
		::SendMessage(GetParent()->GetSafeHwnd(), WM_OBS_SOURCE_MOVEDOWN, 0, 0);
		break;
	case WM_OBS_SOURCE_MOVETOP:
		::SendMessage(GetParent()->GetSafeHwnd(), WM_OBS_SOURCE_MOVETOP, 0, 0);
		break;
	case WM_OBS_SOURCE_MOVEBOTTOM:
		::SendMessage(GetParent()->GetSafeHwnd(), WM_OBS_SOURCE_MOVEBOTTOM, 0, 0);
		break;
	case WM_OBS_SOURCE_FILTERS:
		::SendMessage(GetParent()->GetSafeHwnd(), WM_OBS_SOURCE_FILTERS, 0, 0);
		break;
	}

	return CDisplaySourceWnd::OnCommand(wParam, lParam);
}
