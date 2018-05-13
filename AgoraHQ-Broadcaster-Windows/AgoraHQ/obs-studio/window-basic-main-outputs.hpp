#pragma once

#include <string>

class CAgoraHQDlg;

struct BasicOutputHandler {
	OBSOutput              fileOutput;
	OBSOutput              streamOutput;
	OBSOutput              replayBuffer;
	bool                   streamingActive = false;
	bool                   recordingActive = false;
	bool                   delayActive = false;
	bool                   replayBufferActive = false;
	CAgoraHQDlg               *main;

	std::string            outputType;

	OBSSignal              startRecording;
	OBSSignal              stopRecording;
	OBSSignal              startReplayBuffer;
	OBSSignal              stopReplayBuffer;
	OBSSignal              startStreaming;
	OBSSignal              stopStreaming;
	OBSSignal              streamDelayStarting;
	OBSSignal              streamStopping;
	OBSSignal              recordStopping;
	OBSSignal              replayBufferStopping;

	inline BasicOutputHandler(CAgoraHQDlg *main_) : main(main_) {}

	virtual ~BasicOutputHandler() {};

	virtual bool StartStreaming(obs_service_t *service) = 0;
	virtual bool StartRecording() = 0;
	virtual bool StartReplayBuffer() {return false;}
	virtual void StopStreaming(bool force = false) = 0;
	virtual void StopRecording(bool force = false) = 0;
	virtual void StopReplayBuffer(bool force = false) {(void)force;}
	virtual bool StreamingActive() const = 0;
	virtual bool RecordingActive() const = 0;
	virtual bool ReplayBufferActive() const {return false;}

	virtual void Update() = 0;

	inline bool Active() const
	{
		return streamingActive || recordingActive || delayActive ||
			replayBufferActive;
	}
};
std::string GenerateSpecifiedFilename(const char *extension, bool noSpace,
	const char *format);
BasicOutputHandler *CreateSimpleOutputHandler(CAgoraHQDlg *main);
BasicOutputHandler *CreateAdvancedOutputHandler(CAgoraHQDlg *main);
