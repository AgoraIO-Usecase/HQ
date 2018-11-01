//
//  Agora Rtc Engine SDK
//
//  Copyright (c) 2018 Agora IO. All rights reserved.
//

#ifndef AGORA_RTC_LIVE_ENGINE_H
#define AGORA_RTC_LIVE_ENGINE_H

#include <stddef.h>
#include <stdio.h>
#include <stdarg.h>

#include "IAgoraRtcEngine.h"
#include "IAgoraLivePublisher.h"
#include "IAgoraLiveSubscriber.h"
#if defined(_WIN32)
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#define AGORA_CALL __cdecl
#if defined(AGORARTC_EXPORT)
#define AGORA_API extern "C" __declspec(dllexport)
#else
#define AGORA_API extern "C" __declspec(dllimport)
#endif
#elif defined(__APPLE__)
#define AGORA_API __attribute__((visibility("default"))) extern "C"
#define AGORA_CALL
#elif defined(__ANDROID__) || defined(__linux__)
#define AGORA_API extern "C" __attribute__((visibility("default")))
#define AGORA_CALL
#else
#define AGORA_API extern "C"
#define AGORA_CALL
#endif
namespace agora {

namespace rtc {

/*
* Media stream type.
*/
enum MEDIA_TYPE
{
	/*
	* No audio and video.
	*/
    MEDIA_TYPE_NONE = 0,
	/*
	* Audio only.
	*/
    MEDIA_TYPE_AUDIO_ONLY = 1,
	/*
	* Video only.
	*/
    MEDIA_TYPE_VIDEO_ONLY = 2,
	/*
	* Audio and video.
	*/
    MEDIA_TYPE_AUDIO_AND_VIDEO = 3
};

/*
* Channel configuration.
*/
struct LiveChannelConfig {
    bool videoEnabled;
};

/*
* Stream statitics.
*/
typedef RtcStats LiveStats;

/**
* Event callback interface.
*/
class ILiveEngineEventHandler
{
public:
    virtual ~ILiveEngineEventHandler() {}

    /** A warning message occurred.
     * @param warn Warning code: #WARN_CODE_TYPE

     */
    virtual void onWarning(int warn) {
        (void)warn;
    }

    /** An error message occurred.
     * @param err Error code: #ERROR_CODE_TYPE
     */
    virtual void onError(int err) {
        (void)err;
    }

    /** The user/host has joined the channel.
    * @param channel Channel name
    * @param uid User ID the user who joined the channel
    * @param elapsed Time elapsed (ms) from ILiveEngine::joinChannel until this callback function is triggered.
    */
    virtual void onJoinChannelSuccess(const char* channel, uid_t uid, int elapsed) {
        (void)channel;
        (void)uid;
        (void)elapsed;
    }

    /** A user rejoined the channel.
    * @param channel Channel name
    * @param uid User ID of the user who rejoined the channel
    * @param elapsed Time elapsed (ms) from ILiveEngine::joinChannel until this callback function is triggered.
    */
    virtual void onRejoinChannelSuccess(const char* channel, uid_t uid, int elapsed) {
        (void)channel;
        (void)uid;
        (void)elapsed;
    }

	/** A user left the channel.
	*/
    virtual void onLeaveChannel() {
    }

    /** The RtcEngine statistics reported once every two seconds.
     * @param stats RtcEngine statistics: RtcStats

    */
    virtual void onLiveStats(const LiveStats& stats) {
        (void)stats;
    }

    /** The network quality reported once every two seconds.
	* @param uid User ID of the remote user
	* @param txQuality The transmission network quality from 0 to 5.
	* @param rxQuality The receiving network quality from 0 to 5.
	*/
    virtual void onNetworkQuality(uid_t uid, int txQuality, int rxQuality) {
		(void)uid;
		(void)txQuality;
		(void)rxQuality;
    }

    /** The network connection is lost.
    */
    virtual void onConnectionLost() {}

    /** The local user is disconnected and the SDK will try to reconnect.
    */
    virtual void onConnectionInterrupted() {}

    /** The Token has expired.

     When a token is enabled and the specified token is invalid or expired, this function will be called.
    * The application should request a new token and call ILiveEngine::renewToken to renew the token.
    * @note #ERR_TOKEN_EXPIRED and #ERR_INVALID_TOKEN are also reported by the ILiveEngineEventHandler::onError callback.

    */
    virtual void onRequestToken() {
    }
/** The Token privilege will expire in 30 seconds.

 If the Token you specified when calling ILiveEngine::joinChannel expires, you will become offline. This callback is triggered 30 seconds before the Token expires to remind the application to renew the Token. Upon receiving this callback, the user needs to generate a new Token on your server and call renewToken to pass the new Token on to the SDK.

@param token The Token that will expire in 30 seconds.

@return

- 0: Success.
- < 0: Failure.
 */
    virtual void onTokenPrivilegeWillExpire(const char *token) {
        (void)token;
    }

};
/** ILiveEngine is the base class for managing a live broadcast. ILiveEngine manages the channel and all actions apart from publishing and subscribing.
 */
class ILiveEngine
{
public:

    /** Initializes the RtcEngine.

@param appId RTC engine context

@return

- 0: Success.
- < 0: Failure.
     */
    virtual int initialize(const char *appId) = 0;

    /** Gets the SDK version.

     * @param build Build number
     * @return The current SDK version in the sting format. For example, 2.3.0
     */
    virtual const char* getVersion(int* build) = 0;

	/*
	* Gets the IRtcEngine interface.
	*/
    virtual IRtcEngine* getRtcEngine() = 0;


    /**
    * Releases the engine resource.

     @param sync

- true: Releases the engine resources and returns after all resources have been destroyed.
- false: Notifies the engine to release its resources and returns without waiting for the resources to be  destroyed
    */
    virtual void release(bool sync=false) = 0;

	/*
	* Sets the event handler for ILiveEngine.
	*/
    virtual int setEventHandler(ILiveEngineEventHandler* eventHandler) = 0;

    /** Allows a useer to join the channel. If a channel has not been created, it will been created automatically.

@note A channel does not accept duplicate uids, such as two users with the same uid. If you set uid as 0, the system will automatically assign a uid.

@param token The token, if you have initialized the engine with an available App ID, it can set as null. If you enable the token in Dashboard, specify the token here.
@param channelId Channel Id
@param config Channel configuration: LiveChannelConfig
@param uid User ID, if set to 0, the system will automatically assign one.

@return

- 0: Success.
- < 0: Failure.
    */
    virtual int joinChannel(const char* token, const char* channelId, struct LiveChannelConfig *config, uid_t uid) = 0;

    /** Allows a user to leave the channel.

@return

- 0: Success.
- < 0: Failure.
    */
    virtual int leaveChannel() = 0;

    /** Renews the token.

@param token Token to be renewed.

@return

- 0: Success.
- < 0: Failure.
    */
    virtual int renewToken(const char* token) = 0;

    /** Starts the local video preview.

@return

- 0: Success.
- < 0: Failure.
    */
    virtual int startPreview(view_t view, RENDER_MODE_TYPE type) = 0;

    /** Stops the local video preview.

@return

- 0: Success.
- < 0: Failure.
    */
    virtual int stopPreview() = 0;
};


} //namespace rtc
} // namespace agora

/** Creates the RTC live engine object and returns the pointer.
* @return Returns the pointer of the RTC engine object.
*/
AGORA_API agora::rtc::ILiveEngine* AGORA_CALL createAgoraLiveEngine();

#endif
