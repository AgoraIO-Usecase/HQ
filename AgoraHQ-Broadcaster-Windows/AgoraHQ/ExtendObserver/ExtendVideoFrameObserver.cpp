#include "stdafx.h"
#include "ExtendVideoFrameObserver.h"

//FILE* fp = NULL;

CExtendVideoFrameObserver::CExtendVideoFrameObserver()
{
//	fp = fopen("D:\\HQ_onCaptureVideoFrame_yuv.i420", "ab+");
	m_lpImageBuffer = new BYTE[0x800000];
}


CExtendVideoFrameObserver::~CExtendVideoFrameObserver()
{
//	fclose(fp);
	delete[] m_lpImageBuffer;
}

bool CExtendVideoFrameObserver::onCaptureVideoFrame(VideoFrame& videoFrame)
{
	SIZE_T nBufferSize = 0x800000;

	BOOL bSuccess = CVideoPackageQueue::GetInstance()->PopVideoPackage(m_lpImageBuffer, &nBufferSize);
	if (!bSuccess)
		return false;

	m_lpY = m_lpImageBuffer;
	m_lpU = m_lpImageBuffer + videoFrame.height*videoFrame.width;
	m_lpV = m_lpImageBuffer + 5 * videoFrame.height*videoFrame.width / 4;

	memcpy_s(videoFrame.yBuffer, videoFrame.height*videoFrame.width, m_lpY, videoFrame.height*videoFrame.width);
	videoFrame.yStride = videoFrame.width;
	
	memcpy_s(videoFrame.uBuffer, videoFrame.height*videoFrame.width / 4, m_lpU, videoFrame.height*videoFrame.width / 4);
	videoFrame.uStride = videoFrame.width/2;

	memcpy_s(videoFrame.vBuffer, videoFrame.height*videoFrame.width / 4, m_lpV, videoFrame.height*videoFrame.width / 4);
	videoFrame.vStride = videoFrame.width/2;

	videoFrame.type = FRAME_TYPE_YUV420;
	videoFrame.rotation = 0;

	/*fwrite(m_lpImageBuffer, 1, videoFrame.width*videoFrame.height*3/2, fp);*/
	return true;
}

bool CExtendVideoFrameObserver::onRenderVideoFrame(unsigned int uid, VideoFrame& videoFrame)
{
	return true;
}
