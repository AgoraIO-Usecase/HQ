#pragma once
#include <Mmreg.h>

#include "BufferQueue.h"


class CAudioCapturePackageQueue
{
protected:
	CAudioCapturePackageQueue();
	~CAudioCapturePackageQueue();

public:
	static CAudioCapturePackageQueue *GetInstance();
	static void CloseInstance();

	void SetAudioPackageSize(SIZE_T nPackageSize);
	SIZE_T GetAudioPackageSize() const { return m_nPackageSize; };

	void SetAudioFormat(const WAVEFORMATEX *lpWaveInfo);
	void GetAudioFormat(WAVEFORMATEX *lpWaveInfo);

	BOOL PushAudioPackage(LPCVOID lpAudioPackage, SIZE_T nPackageSize);
	BOOL PopAudioPackage(LPVOID lpAudioPackage, SIZE_T *nPackageSize);

private:
	CBufferQueue	m_bufQueue;
	WAVEFORMATEX	m_waveFormat;
	SIZE_T			m_nPackageSize;

	static CAudioCapturePackageQueue	*m_lpAudioPackageQueue;
};

