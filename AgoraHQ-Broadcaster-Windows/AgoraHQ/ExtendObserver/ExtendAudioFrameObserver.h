#pragma once
#include "../SDK/include/IAgoraMediaEngine.h"

//#include "XAudioPlayout.h"
#include "CicleBuffer.hpp"
class CExtendAudioFrameObserver :
	public agora::media::IAudioFrameObserver
{
public:
	CicleBuffer* pCircleBuffer;
	LPBYTE pPlayerData;
	int    nPlayerDataLen;

	CExtendAudioFrameObserver();
	~CExtendAudioFrameObserver();

	virtual bool onRecordAudioFrame(AudioFrame& audioFrame);
	virtual bool onPlaybackAudioFrame(AudioFrame& audioFrame);
	virtual bool onMixedAudioFrame(AudioFrame& audioFrame);
	virtual bool onPlaybackAudioFrameBeforeMixing(unsigned int uid, AudioFrame& audioFrame);
};

