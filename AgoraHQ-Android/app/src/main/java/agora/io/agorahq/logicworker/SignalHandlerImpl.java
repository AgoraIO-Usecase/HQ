package agora.io.agorahq.logicworker;

import android.content.Context;

import java.util.concurrent.ConcurrentHashMap;

import agora.io.agorahq.utils.Constants;
import io.agora.signaling.hq.AgoraHQSigSDK;

public class SignalHandlerImpl {
    private final static String TAG = Constants.TAG_PR + SignalHandlerImpl.class.getSimpleName();

    private final Context mContext;
    private final ConcurrentHashMap<Integer, ISignalEventHandler> mHandler = new ConcurrentHashMap<>();

    public SignalHandlerImpl(Context ctx) {
        mContext = ctx;
    }

    public void addSignalingEventHandler(ISignalEventHandler handler) {
        mHandler.put(0, handler);
    }

    public boolean removeSignalingEnventHandler(ISignalEventHandler handler) {
        return mHandler.remove(0).equals(handler);
    }

    AgoraHQSigSDK.EventHandler mSignalEventHandler = new AgoraHQSigSDK.EventHandler() {
        @Override
        public void onLoginSuccess() {
            if (mHandler.size() != 0) {
                mHandler.values().iterator().next().onLoginSuccess();
            }
        }

        @Override
        public void onError(int error) {
            if (mHandler.size() != 0) {
                mHandler.values().iterator().next().onError(error);
            }

        }

        @Override
        public void onChannelMessageReceived(String channel, long msgId, String msg) {
            if (mHandler.size() != 0) {
                mHandler.values().iterator().next().onChannelMessageReceived(channel, msgId, msg);
            }
        }

        @Override
        public void onMessageReceivedFrom(String account, long msgId, String msg) {
            if (mHandler.size() != 0) {
                mHandler.values().iterator().next().onMessageReceivedFrom(account, msgId, msg);
            }
        }
    };
}
