package io.agora.livequiz.module;

import io.agora.rtc.IRtcEngineEventHandler;

/**
 * Created by zhangtao on 2018/2/7.
 */

public interface AGEventHandler {

    void onFirstRemoteVideoDecoded(int uid, int width, int height, int elapsed);

    void onUserJoined(int uid, int elapsed);

    void onUserOffline(int uid, int reason);

    void onLeaveChannel(IRtcEngineEventHandler.RtcStats stats);

    void onJoinChannelSuccess(String channel, int uid, int elapsed);

    void onAudioVolumeIndication(IRtcEngineEventHandler.AudioVolumeInfo[] speakers, int totalVolume);

    void onUserMuteVideo(int uid, boolean muted);

    void onReceiveSEI(String info);

    void onGangUpEngineJoinChannelSuccess(String channel, int uid, int elapsed);

    void onGangUpLeaveChannel(IRtcEngineEventHandler.RtcStats stats);

    void onGangUpUserJoined(int uid, int elapsed);

    void onGangUponUserOffline(final int uid, int reason);
}
