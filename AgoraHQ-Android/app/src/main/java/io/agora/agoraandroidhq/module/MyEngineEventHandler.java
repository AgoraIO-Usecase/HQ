package io.agora.agoraandroidhq.module;

import android.content.Context;

import io.agora.rtc.IRtcEngineEventHandler;

public class MyEngineEventHandler {

    public MyEngineEventHandler(Context ctx) {
        this.mContext = ctx;
    }

    private final Context mContext;

    private AGEventHandler agEventHandler;

    public void addEventHandler(AGEventHandler handler) {
        agEventHandler = handler;
    }

    public void removeEventHandler(AGEventHandler handler) {
        agEventHandler = null;
    }

    public final IRtcEngineEventHandler mRtcEventHandler = new IRtcEngineEventHandler() {

        @Override
        public void onFirstRemoteVideoDecoded(int uid, int width, int height, int elapsed) {
            if (agEventHandler != null) {
                agEventHandler.onFirstRemoteVideoDecoded(uid, width, height, elapsed);
            }
        }

        @Override
        public void onFirstLocalVideoFrame(int width, int height, int elapsed) {
        }

        @Override
        public void onUserMuteVideo(int uid, boolean muted) {
            super.onUserMuteVideo(uid, muted);
            if (agEventHandler != null) {
                agEventHandler.onUserMuteVideo(uid, muted);
            }
        }

        @Override
        public void onUserJoined(int uid, int elapsed) {

            if (agEventHandler != null) {
                agEventHandler.onUserJoined(uid, elapsed);
            }
        }

        @Override
        public void onUserOffline(int uid, int reason) {
            // FIXME this callback may return times

            if (agEventHandler != null) {
                agEventHandler.onUserOffline(uid, reason);
            }
        }


        @Override
        public void onLeaveChannel(RtcStats stats) {

            if (agEventHandler != null) {
                agEventHandler.onLeaveChannel(stats);
            }
        }

        @Override
        public void onLastmileQuality(int quality) {

        }

        @Override
        public void onError(int err) {
            super.onError(err);
        }

        @Override
        public void onJoinChannelSuccess(String channel, int uid, int elapsed) {
            if (agEventHandler != null) {
                agEventHandler.onJoinChannelSuccess(channel, uid, elapsed);
            }
        }

        @Override
        public void onAudioVolumeIndication(AudioVolumeInfo[] speakers, int totalVolume) {

            if (agEventHandler != null) {
                agEventHandler.onAudioVolumeIndication(speakers, totalVolume);
            }
        }

        @Override
        public void onReceiveSEI(String info) {
            super.onReceiveSEI(info);

            if (agEventHandler != null) {
                agEventHandler.onReceiveSEI(info);
            }
        }
    };

    public final IRtcEngineEventHandler gangUpRtcEngineEventHandler = new IRtcEngineEventHandler() {
        @Override
        public void onJoinChannelSuccess(String channel, int uid, int elapsed) {
            super.onJoinChannelSuccess(channel, uid, elapsed);
            if (agEventHandler != null) {
                agEventHandler.onGangUpEngineJoinChannelSuccess(channel, uid, elapsed);
            }
        }

        @Override
        public void onLeaveChannel(RtcStats stats) {
            super.onLeaveChannel(stats);
            if (agEventHandler != null) {
                agEventHandler.onGangUpLeaveChannel(stats);
            }
        }

        @Override
        public void onUserJoined(int uid, int elapsed) {
            super.onUserJoined(uid, elapsed);
            if (agEventHandler != null) {
                agEventHandler.onGangUpUserJoined(uid, elapsed);
            }
        }

        @Override
        public void onUserOffline(int uid, int reason) {
            super.onUserOffline(uid, reason);

            if (agEventHandler != null) {
                agEventHandler.onGangUponUserOffline(uid, reason);
            }
        }
    };
}
