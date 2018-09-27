package agora.io.agorahq.logicworker;

import io.agora.rtc.IRtcEngineEventHandler;

public interface IMediaEventHandler {
    /**
     * Remote video decoded
     * @param uid
     * @param width
     * @param height
     * @param elapsed
     */
    void onFirstRemoteVideoDecoded(int uid, int width, int height, int elapsed);

    /**
     * Other user joined
     * @param uid
     * @param elapsed
     */
    void onUserJoined(int uid, int elapsed);

    /**
     * Other user offline
     * @param uid
     * @param reason
     */
    void onUserOffline(int uid, int reason);

    /**
     * Local leave channel
     * @param stats
     */
    void onLeaveChannel(IRtcEngineEventHandler.RtcStats stats);

    /**
     * Local join channel
     * @param channel
     * @param uid
     * @param elapsed
     */
    void onJoinChannelSuccess(String channel, int uid, int elapsed);

    /**
     * Audio volume indication
     * @param speakers
     * @param totalVolume
     */
    void onAudioVolumeIndication(IRtcEngineEventHandler.AudioVolumeInfo[] speakers, int totalVolume);

    /**
     * Remote user mute himself
     * @param uid
     * @param muted
     */
    void onUserMuteVideo(int uid, boolean muted);

    /**
     * Got set form video stream
     * @param info
     */
    void onReceiveSEI(String info);

    void onError(int error);
}
