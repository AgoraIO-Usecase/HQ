#pragma once
#include <memory>
#include <string>
#include "obs.h"
//#include "video_render_impl.h"
static const char *agora_signals[] = {
	"void firstRemoteVideoDecoded(ptr service, unsigned int uid, int width, int height, int elapsed)",
	"void userJoined(ptr service, unsigned int uid, int elapsed)",
	"void userOffline(ptr service, unsigned int uid, int reason)",
	NULL
};

class AgoraRtcEngine
{
   // Q_OBJECT
public:
    explicit AgoraRtcEngine();
    ~AgoraRtcEngine();
	static AgoraRtcEngine* GetInstance();
	static void ReleaseInstance();

	bool InitEngine(std::string appid);

	BOOL setClientRole(CLIENT_ROLE_TYPE role, LPCSTR lpPermissionKey = NULL);
	int  setChannelProfile(CHANNEL_PROFILE_TYPE profile);

	int enableVideo(bool enabled);
	bool enableLocalRender(bool bEnable);
	int muteLocalAudioStream(bool muted);
	int  setLocalVideoMirrorMode(VIDEO_MIRROR_MODE_TYPE mirrorMode);
	void startPreview();
	void stopPreview();

	int joinChannel(const std::string& key, const std::string& channel, unsigned uid);
	int leaveChannel();
	
	bool  keepPreRotation(bool bRotate);
	bool  setVideoProfileEx(int nWidth, int nHeight, int nFrameRate, int nBitRate);
	bool  enableLocalCameara(bool bEnable);

	void* AgoraVideoObserver_Create();
	void  AgoraVideoObserver_Destroy(void* data);
	bool  AgoraVideoObserver_Encode(void* data, struct encoder_frame* frame,
	    struct encoder_packet* packet, bool *receive_packet);

	bool setAudioProfile(int nSampleRate, int nChannels, int nSamplesPerCall);
	bool setExternalAudioSource(bool bEnabled, int nSampleRate, int nChannels);
	bool enableExtendPlayDevice(bool bEnable);

	void* AgoraAudioObserver_Create();
	void  AgoraAudioObserver_Destroy(void* data);
	bool  AgoraAudioObserver_Encode(void* data, struct encoder_frame* frame,
	    struct encoder_packet* packet, bool *receive_packet);
	
	int ConfigPublisher(const PublisherConfiguration& config);
	int SetVideoCompositingLayout(const VideoCompositingLayout& sei);
	int ClearVideoCompositingLayout();

	//…Ë±∏
	int getRecordingDeviceVolume();
	int getPalyoutDeviceVolume();
	int setRecordingDeviceVolume(int volume);
	int setPalyoutDeviceVolume(int volume);
	int testMicrophone(bool start, int interval);
	int testSpeaker(bool start);

	obs_service_t* agoraService;
//     QVariantMap getRecordingDeviceList();
//     QVariantMap getPlayoutDeviceList();
//     QVariantMap getVideoDeviceList();
//     int setRecordingDevice(const QString& guid);
//     int setPlayoutDevice(const QString& guid);
//     int setVideoDevice(const QString& guid);
//	int testCamera(bool start, QWidget* view);
//	int setupLocalVideo(QWidget* view);
	int setupRemoteVideo(unsigned int uid, void* view);
    agora::rtc::IRtcEngine* getRtcEngine() {return m_rtcEngine.get();}

	int agora_fps = 15;
	int agora_out_cx = 640;
	int agora_out_cy = 360;
	int agora_video_bitrate = 500;
//signals:
    void joinedChannelSuccess(const char* channel, unsigned int uid, int elapsed);
//      void userJoined(unsigned int uid, int elapsed);
//      void userOffline(unsigned int uid, int reason);
//      void firstRemoteVideoDecoded(unsigned int uid, int width, int height, int elapsed);
//      void firstRemoteVideoFrameDrawn(unsigned int uid, int width, int height, int elapsed);
//public slots:
private:
    friend class AgoraRtcEngineEvent;
private:
	static char* m_appid;
	static AgoraRtcEngine* m_agoraEngine;
};

