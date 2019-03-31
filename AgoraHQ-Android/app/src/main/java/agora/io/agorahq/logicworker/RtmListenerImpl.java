package agora.io.agorahq.logicworker;

import android.content.Context;

import java.util.concurrent.ConcurrentHashMap;

import agora.io.agorahq.utils.Constants;
import io.agora.rtm.*;

public class RtmListenerImpl {
    private final static String TAG = Constants.TAG_PR + RtmListenerImpl.class.getSimpleName();

    private final Context mContext;
    private final ConcurrentHashMap<Integer, IRtmEventHandler> mHandler = new ConcurrentHashMap<>();

    public RtmListenerImpl(Context ctx) {
        mContext = ctx;
    }

    public void addSignalingEventHandler(IRtmEventHandler handler) {
        mHandler.put(0, handler);
    }

    public boolean removeSignalingEnventHandler(IRtmEventHandler handler) {
        return mHandler.remove(0).equals(handler);
    }

    ResultCallback<Void> channelJoinResult = new ResultCallback<Void>() {
        @Override
        public void onSuccess(Void responseInfo) {
            if (mHandler.size() != 0) {
                mHandler.values().iterator().next().onLoginSuccess();
            }
        }

        @Override
        public void onFailure(ErrorInfo errorInfo) {
            if (mHandler.size() != 0) {
                mHandler.values().iterator().next().onError(errorInfo);
            }
        }
    };

    RtmClientListener mRtmClientListener = new RtmClientListener() {
        @Override
        public void onConnectionStateChanged(int state, int reason) {

        }

        @Override
        public void onMessageReceived(RtmMessage message, String peerId) {
            if (mHandler.size() != 0) {
                mHandler.values().iterator().next().onMessageReceivedFrom(peerId, message.getText());
            }
        }
    };

    RtmChannelListener mRtmChannelListener = new RtmChannelListener() {
        @Override
        public void onMessageReceived(RtmMessage message, RtmChannelMember fromMember) {
            if (mHandler.size() != 0) {
                mHandler.values().iterator().next().onChannelMessageReceived(fromMember.getChannelId(), message.getText());
            }
        }

        @Override
        public void onMemberJoined(RtmChannelMember member) {

        }

        @Override
        public void onMemberLeft(RtmChannelMember member) {

        }
    };
        //        @Override
//        public void onLoginSuccess() {
//            if (mHandler.size() != 0) {
//                mHandler.values().iterator().next().onLoginSuccess();
//            }
//        }
//
//        @Override
//        public void onError(int error) {
//            if (mHandler.size() != 0) {
//                mHandler.values().iterator().next().onError(error);
//            }
//
//        }
//
//        @Override
//        public void onChannelMessageReceived(String channel, long msgId, String msg) {
//            if (mHandler.size() != 0) {
//                mHandler.values().iterator().next().onChannelMessageReceived(channel, msgId, msg);
//            }
//        }
//
//        @Override
//        public void onMessageReceivedFrom(String account, long msgId, String msg) {
//            if (mHandler.size() != 0) {
//                mHandler.values().iterator().next().onMessageReceivedFrom(account, msgId, msg);
//            }
//        }
}
