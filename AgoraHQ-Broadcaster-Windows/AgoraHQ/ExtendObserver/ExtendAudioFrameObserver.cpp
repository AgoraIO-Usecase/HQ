#include "stdafx.h"
#include "ExtendAudioFrameObserver.h"

CExtendAudioFrameObserver::CExtendAudioFrameObserver()
{
	this->pCircleBuffer = new CicleBuffer(44100 * 2 * 2, 0);
	pPlayerData = new BYTE[0x800000];
}


CExtendAudioFrameObserver::~CExtendAudioFrameObserver()
{
	delete[] pPlayerData;
}

bool CExtendAudioFrameObserver::onRecordAudioFrame(AudioFrame& audioFrame)
{
	SIZE_T nSize = audioFrame.channels*audioFrame.samples * 2;
	unsigned int datalen = 0;
	pCircleBuffer->readBuffer(this->pPlayerData, nSize, &datalen);
	if (nSize > 0 && datalen > 0)
	{
		int nMixLen = datalen > nSize ? nSize : datalen;
		memcpy((int16_t*)audioFrame.buffer, (int16_t*)pPlayerData, nMixLen);
	}
	return true;
}

bool CExtendAudioFrameObserver::onPlaybackAudioFrame(AudioFrame& audioFrame)
{
	//SIZE_T nSize = audioFrame.channels*audioFrame.samples * 2;
	//CAudioPlayPackageQueue::GetInstance()->PushAudioPackage(audioFrame.buffer, nSize);
	
	return true;
}

bool CExtendAudioFrameObserver::onMixedAudioFrame(AudioFrame& audioFrame)
{
	return true;
}

bool CExtendAudioFrameObserver::onPlaybackAudioFrameBeforeMixing(unsigned int uid, AudioFrame& audioFrame)
{
	return true;
}

