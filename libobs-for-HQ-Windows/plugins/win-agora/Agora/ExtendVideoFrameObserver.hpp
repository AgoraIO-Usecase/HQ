#pragma once
#include "Agora/IAgoraMediaEngine.h"
#include <windows.h>

class CExtendVideoFrameObserver :
	public agora::media::IVideoFrameObserver
{
public:
	CExtendVideoFrameObserver();
	~CExtendVideoFrameObserver();

	virtual bool onCaptureVideoFrame(VideoFrame& videoFrame);
	virtual bool onRenderVideoFrame(unsigned int uid, VideoFrame& videoFrame);

	void setVideoResolution(int w, int h);
	LPBYTE				m_lpImageBuffer;
	LPBYTE				m_lpRenderBuffer;
	int					m_RenderWidth;
	int					m_RenderHeight;
    
private:
	LPBYTE				m_lpY;
	LPBYTE				m_lpU;
	LPBYTE				m_lpV;

	int                m_resolutionX;
	int                m_resolutionY;
};

