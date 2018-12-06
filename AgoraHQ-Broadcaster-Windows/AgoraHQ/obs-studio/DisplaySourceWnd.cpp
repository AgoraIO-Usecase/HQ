// AGVideoWnd.cpp : 实现文件
//

#include "stdafx.h"
#include "AgoraHQ.h"
#include "DisplaySourceWnd.h"
#include "obs.hpp"

// CDisplaySourceWnd

IMPLEMENT_DYNAMIC(CDisplaySourceWnd, CWnd)

CDisplaySourceWnd::CDisplaySourceWnd()
: m_nUID(0)
, m_crBackColor(RGB(0x58, 0x58, 0x58))
, m_bShowVideoInfo(FALSE)
, m_bBigShow(FALSE)
, m_bBackground(FALSE)
{

}

CDisplaySourceWnd::~CDisplaySourceWnd()
{
	
}


BEGIN_MESSAGE_MAP(CDisplaySourceWnd, CWnd)
	ON_WM_ERASEBKGND()
	ON_WM_LBUTTONDOWN()
	ON_WM_RBUTTONDOWN()
	ON_WM_CREATE()
	ON_WM_PAINT()
	ON_WM_SIZE()
	ON_WM_LBUTTONDBLCLK()
    ON_WM_PAINT()
END_MESSAGE_MAP()



// CDisplaySourceWnd 消息处理程序
BOOL CDisplaySourceWnd::OnEraseBkgnd(CDC* pDC)
{
	// TODO:  在此添加消息处理程序代码和/或调用默认值
	CRect		rcClient;
	CPoint		ptDraw;
	IMAGEINFO	imgInfo;

	GetClientRect(&rcClient);

	return TRUE;
}

void CDisplaySourceWnd::SetUID(UINT nUID)
{
	m_nUID = nUID;

// 	if (m_nUID == 0)
// 		m_wndInfo.ShowWindow(SW_HIDE);
// 	else
// 		m_wndInfo.ShowWindow(SW_SHOW);
}

UINT CDisplaySourceWnd::GetUID()
{
	return m_nUID;
}

BOOL CDisplaySourceWnd::IsWndFree()
{
	return m_nUID == 0 ? TRUE : FALSE;
}

BOOL CDisplaySourceWnd::SetBackImage(UINT nID, UINT nWidth, UINT nHeight, COLORREF crMask)
{
	CBitmap bmBackImage;

	if (!bmBackImage.LoadBitmap(nID))
		return FALSE;
	
	return TRUE;
}

void CDisplaySourceWnd::ShowBackground(BOOL bBackground)
{
    m_bBackground = bBackground;

    Invalidate(TRUE);
}

void CDisplaySourceWnd::SetFaceColor(COLORREF crBackColor)
{
	m_crBackColor = crBackColor;

	Invalidate(TRUE);
}

void CDisplaySourceWnd::SetVideoResolution(UINT nWidth, UINT nHeight)
{
	m_nWidth = nWidth;
	m_nHeight = nHeight;

	//m_wndInfo.SetVideoResolution(nWidth, nHeight);
}

void CDisplaySourceWnd::GetVideoResolution(UINT *nWidth, UINT *nHeight)
{
	*nWidth = m_nWidth;
	*nHeight = m_nHeight;
}

void CDisplaySourceWnd::SetBitrateInfo(int nReceivedBitrate)
{
	m_nBitRate = nReceivedBitrate;
	//m_wndInfo.SetBitrateInfo(nReceivedBitrate);
}

void CDisplaySourceWnd::SetFrameRateInfo(int nReceiveFrameRate)
{
	m_nFrameRate = nReceiveFrameRate;
	//m_wndInfo.SetFrameRateInfo(nReceiveFrameRate);
}

void CDisplaySourceWnd::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO:  在此添加消息处理程序代码和/或调用默认值

	::SendMessage(GetParent()->GetSafeHwnd(), WM_SHOWBIG, (WPARAM)this, (LPARAM)m_nUID);

	CWnd::OnLButtonDown(nFlags, point);
}


void CDisplaySourceWnd::OnRButtonDown(UINT nFlags, CPoint point)
{
	// TODO:  在此添加消息处理程序代码和/或调用默认值

	CWnd::OnRButtonDown(nFlags, point);
}


int CDisplaySourceWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  在此添加您专用的创建代码
//	m_wndInfo.Create(NULL, NULL, WS_CHILD | WS_VISIBLE, CRect(0, 0, 192, 28), this, IDC_STATIC);

	return 0;
}


void CDisplaySourceWnd::ShowVideoInfo(BOOL bShow)
{
	m_bShowVideoInfo = bShow;

	//m_wndInfo.ShowTips(bShow);
	Invalidate(TRUE);

/*	if (!bShow) {
		CRect rcTip;
		m_wndInfo.GetWindowRect(&rcTip);
		
	}
	*/

}

void CDisplaySourceWnd::SetBigShowFlag(BOOL bBigShow)
{
	CRect	rcClient;

	m_bBigShow = bBigShow;
	GetClientRect(&rcClient);

	int x = (rcClient.Width()- 192) / 2;
	int y = rcClient.Height() - 24;
	
// 	if (m_wndInfo.GetSafeHwnd() != NULL) {
// 		if (m_bBigShow)
// 			y -= 4;
// 
// 		m_wndInfo.MoveWindow(x, y, 192, 24);
// 	}
};


void CDisplaySourceWnd::OnSize(UINT nType, int cx, int cy)
{
	CWnd::OnSize(nType, cx, cy);

	CreateDisplay();

	if (::IsWindowVisible(m_hWnd) && display) {
		CRect		rcClient;
		GetClientRect(&rcClient);
		obs_display_resize(display, rcClient.right - rcClient.left, rcClient.bottom - rcClient.top);
	}

	//发信号给对应窗口
	if (receiveWnd != NULL)
		::PostMessage(receiveWnd, WM_DISPLAY_PREVIEW, 0, 0);
}


void CDisplaySourceWnd::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	CWnd::OnLButtonDblClk(nFlags, point);
}


void CDisplaySourceWnd::OnPaint()
{
    CPaintDC dc(this); // device context for painting
    // TODO:  在此处添加消息处理程序代码
    // 不为绘图消息调用 CWnd::OnPaint()


    return CWnd::OnPaint();
}

void CDisplaySourceWnd::CreateDisplay()
{
	if (display)
		return;

	RECT rcClient;
	GetWindowRect(&rcClient);
	//QSize size = GetPixelSize(this);

	gs_init_data info = {};
	info.cx = rcClient.right - rcClient.left;//size.width();
	info.cy = rcClient.bottom - rcClient.top;//size.height();
	info.format = GS_RGBA;
	info.zsformat = GS_ZS_NONE;

	//QTToGSWindow(winId(), info.window);
	info.window.hwnd = m_hWnd;
	display = obs_display_create(&info);
	//发消息创建

	if (display && receiveWnd != NULL)
		::PostMessage(receiveWnd, WM_CREATE_PREVIEW, 0, 0);
	//emit DisplayCreated(this);
}
