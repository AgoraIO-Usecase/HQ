# Agora Game Show Broadcaster SDK Integration (Host)

This page explains how to integrate the Agora Live Quiz SDK for the broadcaster (host).

## Prerequisites

Contact [sales_us@agora.io](mailto:sales_us@agora.io) for the Agora HQ SDK.

- Microsoft Visual C++ 2013+
- Microsoft Visual Studio (recommended)
- Microsoft Windows 7+
- Agora App ID and an App Certificate

## Integrating the SDK

### Import the SDK

Save the `libs` folder into your project folder:

- Under the `Agora_programe/AgoraIO/HQ/AgoraHQ-Broadcaster-Windows/SDK/dll` directory:
	- Add the `agora_rtc_sdk.dll` file
	- Add the `agora_sig_sdk.dll` file

- Under the `Agora_programe/AgoraIO/HQ/AgoraHQ-Broadcaster-Windows/AgoraHQ/openssl/dll` directory, add the `libcrypto-1_1.dll` file

### Apply Agora SDK Methods

1. Create an IRtcEngine object, see [Create an agora::IRtcEngine Object (create)](https://docs.agora.io/en/2.3.1/product/Interactive%20Broadcast/API%20Reference/live_video_windows#live-windows-create-en)
	`IRtcEngine*createAgoraRtcEngine(agora::IRtcEngineEventHandler* pEventHandler)`

2. Set the channel profile to live broadcast, see [Set a Channel Profile (setChannelProfile)](https://docs.agora.io/en/2.3.1/product/Interactive%20Broadcast/API%20Reference/live_video_windows#live-windows-setchannelprofile-en)

	`int setChannelProfile (CHANNEL_PROFILE_TYPE profile)`

3. Enable video mode, see [Enable the Video Mode (enableVideo)](https://docs.agora.io/en/2.3.1/product/Interactive%20Broadcast/API%20Reference/live_video_windows#live-windows-enablevideo-en)
	`int enableVideo()`

4. Set the local video, see [Set the Local Video View (setupLocalVideo)](https://docs.agora.io/en/2.3.1/product/Interactive%20Broadcast/API%20Reference/live_video_windows#live-windows-setuplocalvideo-en)

	`int setupLocalVideo(const VideoCanvas& canvas)`

5. Set the remote video, see [Set the Remote Video View (setupRemoteVideo)](https://docs.agora.io/en/2.3.1/product/Interactive%20Broadcast/API%20Reference/live_video_windows#live-windows-setupremotevideo-en)

	`int setupRemoteVideo(const VideoCanvas& canvas)`
	
6. Set the video resolution, see [Set the Video Profile (setVideoProfile)](https://docs.agora.io/en/2.3.1/product/Interactive%20Broadcast/API%20Reference/live_video_windows#live-windows-setvideoprofile-en)

	`int setVideoProfile(VIDEO_PROFILE_TYPE profile)`

7. Set the client role, see [Set the User Role (setClientRole)](https://docs.agora.io/en/2.3.1/product/Interactive%20Broadcast/API%20Reference/live_video_windows#live-windows-setclientrole-en)

	`int setClientRole(CLIENT_ROLE_TYPE role);`

8. Create and join a channel, see [Join a Channel (joinChannel)](https://docs.agora.io/en/2.3.1/product/Interactive%20Broadcast/API%20Reference/live_video_windows#live-windows-joinchannel-en)

	`int joinChannel(const char* channelToken, const char* channel, const char* info, uid_t uid)`

	**Note:** Once you have joined the channel, the SDK enables the camera by default. The broadcaster can use Start Video Preview (startPreview) for the video preview and Disable the Video Mode (disableVideo) to switch to the voice mode.

For more information on the Agora HQ API methods, see [Agora HQ API](https://docs.agora.io/en/2.3.1/product/Interactive%20Broadcast/API%20Reference/live_contest).