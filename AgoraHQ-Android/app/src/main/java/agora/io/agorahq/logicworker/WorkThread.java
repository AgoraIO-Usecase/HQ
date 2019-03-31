package agora.io.agorahq.logicworker;

import android.content.Context;
import android.os.Handler;
import android.os.Looper;
import android.os.Message;
import android.support.annotation.NonNull;
import android.text.TextUtils;
import android.util.Log;

import agora.io.agorahq.utils.Constants;
import agora.io.agorahq.utils.LogUtil;
import agora.io.agorahq.utils.SignalTokenGeneraterUtil;
import io.agora.rtc.RtcEngine;
import io.agora.rtm.*;

public class WorkThread extends Thread {
    private final static String TAG = Constants.TAG_PR + WorkThread.class.getSimpleName();

    private WorkerHandler mWorkerHandler;

    private Context mContext;
    private boolean mIsReady = false;

    private RtcEngine mRtcEngine;
    private MediaHandlerImpl mMediaHandler;

    private RtmClient mRtmClient;
    private RtmListenerImpl mRtmHandler;
    private RtmChannel mRtmChannel;

    private LogUtil mLogger;

    public MediaHandlerImpl mediaHandler() {
        return mMediaHandler;
    }

    public RtmListenerImpl rtmHandler() {
        return mRtmHandler;
    }

    public WorkThread(Context ctx) {
        this.mContext = ctx;
        this.mMediaHandler = new MediaHandlerImpl(ctx);
        this.mRtmHandler = new RtmListenerImpl(ctx);
        this.mLogger = LogUtil.newInstance();
    }

    private final static class WorkerHandler extends Handler {
        private WorkThread workThread;

        public WorkerHandler(WorkThread thread) {
            this.workThread = thread;
        }

        public void release() {
            this.workThread = null;
        }

        @Override
        public void handleMessage(Message msg) {
            super.handleMessage(msg);
            switch (msg.what) {
                case Constants.ACTION_WORKER_JOIN_CHANNEL:
                    String channelName = msg.obj.toString();
                    workThread.joinChannel(channelName);
                    break;
                case Constants.ACTION_WORKER_LEAVE_CHANNEL:
                    workThread.leaveChannel();
                    break;
                case Constants.ACTION_WORKER_THREAD_QUIT:
                    workThread.exit();
                    break;
                case Constants.ACTION_WORKER_DESTROY_ENGINE:
                    workThread.destoryEngine();
                    break;
                case Constants.SIGNALING_LOGIN:
                    String[] signalS = (String[]) msg.obj;
                    workThread.loginSignaling(signalS[0], signalS[1]);
                    break;
                case Constants.SIGNALING_SEND_CHANNEL_MESSAGE:
                    String content = msg.obj.toString();
                    workThread.sendSignalChannelMessage(content);
                    break;
                case Constants.SIGNALING_LOGOUT:
                    workThread.signalLogout();
                    break;
                default:
                    break;
            }
        }
    }

    @Override
    public void run() {
        super.run();
        Looper.prepare();
        mWorkerHandler = new WorkerHandler(this);
        mIsReady = true;
        Looper.loop();
    }

    public final void joinChannel(@NonNull String channelName) {
        if (Thread.currentThread() != this) {
            Message envelop = new Message();
            envelop.what = Constants.ACTION_WORKER_JOIN_CHANNEL;
            envelop.obj = channelName;
            mWorkerHandler.sendMessage(envelop);
            return;
        }
        ensureRtcEngineReady();
        mRtcEngine.setClientRole(io.agora.rtc.Constants.CLIENT_ROLE_BROADCASTER, null);

        /** set sdk to hq mode*/
        mRtcEngine.setParameters("{\"rtc.hq_mode\": {\"hq\": true, \"broadcaster\":false, \"bitrate\":0}}");
        mRtcEngine.enableAudioVolumeIndication(1000, 3);

        mLogger.log("channelName  account = " + channelName + ",uid = " + 0);
        mRtcEngine.enableVideo();
        mRtcEngine.enableLocalVideo(false);
        mRtcEngine.setVideoProfile(io.agora.rtc.Constants.VIDEO_PROFILE_360P, true);
        mRtcEngine.joinChannel(null, channelName, "", 0); // if you do not specify the uid, we will generate the uid for you

        mLogger.log("SDKVersion:  " + RtcEngine.getSdkVersion());
    }

    public final void leaveChannel() {
        if (Thread.currentThread() != this) {
            Message envelop = new Message();
            envelop.what = Constants.ACTION_WORKER_LEAVE_CHANNEL;
            mWorkerHandler.sendMessage(envelop);
            return;
        }

        ensureRtcEngineReady();
        mRtcEngine.leaveChannel();
        mLogger.log("leaveChannel!");
    }

    public void destoryEngine() {
        if (Thread.currentThread() != this) {
            Message envelop = new Message();
            envelop.what = Constants.ACTION_WORKER_DESTROY_ENGINE;
            mWorkerHandler.sendMessage(envelop);
            return;
        }

        ensureRtcEngineReady();
        mRtcEngine.leaveChannel();

        RtcEngine.destroy(mRtcEngine);
        mLogger.log("destoryEngine");
    }

    /**
     * wait for thread run(not already)
     */
    public void waitForReady() {
        while (!mIsReady) {
            try {
                Thread.sleep(20);
            } catch (InterruptedException e) {
                e.printStackTrace();
            }
        }
    }

    public RtcEngine ensureRtcEngineReady() {
        if (mRtcEngine == null) {
            if (TextUtils.isEmpty(Constants.AGORA_MEDIA_APPID)) {
                throw new RuntimeException("NEED TO use your App ID, get your own ID at https://dashboard.agora.io/");
            }
            try {
                mRtcEngine = RtcEngine.create(mContext, Constants.AGORA_MEDIA_APPID, mMediaHandler.mRtcHandler);
            } catch (Exception e) {
                throw new RuntimeException("NEED TO check rtc sdk init fatal error\n" + Log.getStackTraceString(e));
            }
            mRtcEngine.setChannelProfile(io.agora.rtc.Constants.CHANNEL_PROFILE_LIVE_BROADCASTING);
            // mRtcEngine.setParameters(String.format(Locale.US, "{\"rtc.log_filter\": 65535}"));
        }
        return mRtcEngine;
    }

    public RtmClient ensureRtmReady() {
        if (mRtmClient == null) {
            if (TextUtils.isEmpty(Constants.AGORA_SIGNALING_APPID)) {
                throw new RuntimeException("NEED TO use your App ID, get your own ID at https://dashboard.agora.io/");
            }
            try {
//                mSignalingEngine = new AgoraHQSigSDK(mContext, Constants.AGORA_SIGNALING_APPID);
                mRtmClient = RtmClient.createInstance(Constants.AGORA_SIGNALING_APPID, mRtmHandler.mRtmClientListener);
            } catch (Exception e) {
                throw new RuntimeException("NEED TO check rtc sdk init fatal error\n" + Log.getStackTraceString(e));
            }
        }

        return mRtmClient;
    }

    public void exit() {
        if (Thread.currentThread() != this) {
            mWorkerHandler.sendEmptyMessage(Constants.ACTION_WORKER_THREAD_QUIT);
            return;
        }

        mIsReady = false;

        if (Looper.myLooper() != null)
            Looper.myLooper().quit();

        mWorkerHandler.release();
    }

    public RtcEngine getRtcEngine() {
        return mRtcEngine;
    }

    public void loginSignaling(String account, final String channelName) {
        if (Thread.currentThread() != this) {
            Message msg = Message.obtain();
            msg.obj = new String[]{account, channelName};
            msg.what = Constants.SIGNALING_LOGIN;
            mWorkerHandler.sendMessage(msg);
            return;
        }

        ensureRtmReady();
        mRtmClient.login(null, account, new ResultCallback<Void>() {
            @Override
            public void onSuccess(Void responseInfo) {
                mRtmChannel = mRtmClient.createChannel(channelName, mRtmHandler.mRtmChannelListener);
                mRtmChannel.join(mRtmHandler.channelJoinResult);
            }

            @Override
            public void onFailure(ErrorInfo errorInfo) {
                mLogger.log( "login failed: " + errorInfo);
            }
        });
//        mSignalingEngine.login(account, channelName, getToken(account), mSignalHandler.mSignalEventHandler);
    }

    private String getToken(String account) {
        if (TextUtils.isEmpty(Constants.AGORA_SIGNALING_CERF))
            return "_no_need_token";

        return SignalTokenGeneraterUtil.calcToken(Constants.AGORA_SIGNALING_APPID, Constants.AGORA_SIGNALING_CERF, account,
                SignalTokenGeneraterUtil.getExpiredTime());
    }

    public void sendSignalChannelMessage(final String message) {
        if (Thread.currentThread() != this) {
            Message msg = Message.obtain();
            msg.obj = message;
            msg.what = Constants.SIGNALING_SEND_CHANNEL_MESSAGE;

            mWorkerHandler.sendMessage(msg);

            return;
        }

        ensureRtmReady();

        long messageId = System.currentTimeMillis();
        if(mRtmChannel != null) {
//        mSignalingEngine.sendChannelMessage(messageId, message);
            RtmMessage msg = mRtmClient.createMessage();
            msg.setText(message);
            mRtmChannel.sendMessage(msg, new ResultCallback<Void>() {
                @Override
                public void onSuccess(Void responseInfo) {
                    mLogger.log( "sent channel message ---" + message);
                }

                @Override
                public void onFailure(ErrorInfo errorInfo) {
                    mLogger.log( "sent channel message failed: ---" + errorInfo);
                }
            });
        }
    }

    public void signalLogout() {
        if (Thread.currentThread() != this) {
            mWorkerHandler.sendEmptyMessage(Constants.SIGNALING_LOGOUT);
            return;
        }

        ensureRtmReady();
        mRtmClient.logout(null);
    }
}
