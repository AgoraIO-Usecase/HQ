package agora.io.agorahq.logicworker;

import android.content.Context;

import java.util.concurrent.ConcurrentHashMap;

import agora.io.agorahq.utils.Constants;
import io.agora.rtc.IRtcEngineEventHandler;

public class MediaHandlerImpl {
    private final static String TAG = Constants.TAG_PR + MediaHandlerImpl.class.getSimpleName();

    private final Context mContext;
    private final ConcurrentHashMap<Integer, IMediaEventHandler> mHandler = new ConcurrentHashMap<>();

    public MediaHandlerImpl(Context ctx) {
        mContext = ctx;
    }

    /**
     * for this solution, we just hold only one engine
     *
     * @param handler
     */
    public void addMediaEventHandler(IMediaEventHandler handler) {
        mHandler.put(0, handler);
    }

    public boolean removeMediaEnventHandler(IMediaEventHandler handler) {
        return mHandler.remove(0).equals(handler);
    }

    public final IRtcEngineEventHandler mRtcHandler = new IRtcEngineEventHandler() {

        @Override
        public void onFirstRemoteVideoDecoded(int uid, int width, int height, int elapsed) {
            if (mHandler.size() != 0) {
                mHandler.values().iterator().next().onFirstRemoteVideoDecoded(uid, width, height, elapsed);
            }
        }

        @Override
        public void onFirstLocalVideoFrame(int width, int height, int elapsed) {
        }

        @Override
        public void onUserMuteVideo(int uid, boolean muted) {
            super.onUserMuteVideo(uid, muted);
            if (mHandler.size() != 0) {
                mHandler.values().iterator().next().onUserMuteVideo(uid, muted);
            }
        }

        @Override
        public void onUserJoined(int uid, int elapsed) {

            if (mHandler.size() != 0) {
                mHandler.values().iterator().next().onUserJoined(uid, elapsed);
            }
        }

        @Override
        public void onUserOffline(int uid, int reason) {
            // FIXME this callback may return times

            if (mHandler.size() != 0) {
                mHandler.values().iterator().next().onUserOffline(uid, reason);
            }
        }


        @Override
        public void onLeaveChannel(RtcStats stats) {

            if (mHandler.size() != 0) {
                mHandler.values().iterator().next().onLeaveChannel(stats);
            }
        }

        @Override
        public void onLastmileQuality(int quality) {

        }

        @Override
        public void onError(int err) {
            super.onError(err);

            if (mHandler.size() != 0)
                mHandler.values().iterator().next().onError(err);
        }

        @Override
        public void onJoinChannelSuccess(String channel, int uid, int elapsed) {

            if (mHandler.size() != 0) {
                mHandler.values().iterator().next().onJoinChannelSuccess(channel, uid, elapsed);
            }
        }

        @Override
        public void onAudioVolumeIndication(AudioVolumeInfo[] speakers, int totalVolume) {

            if (mHandler.size() != 0) {
                mHandler.values().iterator().next().onAudioVolumeIndication(speakers, totalVolume);
            }
        }

        @Override
        public void onReceiveSEI(String info) {
            super.onReceiveSEI(info);

            if (mHandler.size() != 0) {
                mHandler.values().iterator().next().onReceiveSEI(info);
            }
        }
    };
}
