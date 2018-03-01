package io.agora.agoraandroidhq.control;

import android.content.Context;
import android.os.Handler;
import android.os.Looper;
import android.os.Message;
import android.text.TextUtils;
import android.util.Log;
import android.view.SurfaceView;

import java.lang.ref.WeakReference;

import io.agora.agoraandroidhq.module.MyEngineEventHandler;
import io.agora.agoraandroidhq.tools.GameControl;
import io.agora.rtc.Constants;
import io.agora.rtc.RtcEngine;
import io.agora.rtc.video.VideoCanvas;

/**
 * Created by zhangtao on 2018/2/7.
 */

public class WorkerThread extends Thread {

    private final Context mContext;
    private static final int ACTION_WORKER_THREAD_QUIT = 0X1010; // quit this thread
    private static final int ACTION_WORKER_JOIN_CHANNEL = 0X2010;
    private static final int ACTION_WORKER_LEAVE_CHANNEL = 0X2011;
    private static final int ACTION_WORKER_CONFIG_ENGINE = 0X2012;
    private static final int ACTION_WORKER_PREVIEW = 0X2014;
    private static final int ACTION_WORKER_GANG_UP_JOIN_CHANNEL = 0x2015;
    private static final int ACTION_WORKER_GANG_UP_LEAVE_CHANNEL = 0x2016;
    private static final int ACTION_WORKER_DESTORY_ENGINE = 0x2017;
    private String tag = "[WorkerThread]  ";
    private final MyEngineEventHandler mEngineEventHandler;

    private static final class WorkerThreadHandler extends Handler {

        private WorkerThread mWorkerThread;

        WorkerThreadHandler(WorkerThread thread) {
            this.mWorkerThread = thread;
        }

        public void release() {
            mWorkerThread = null;
        }

        @Override
        public void handleMessage(Message msg) {
            if (this.mWorkerThread == null) {
                return;
            }
            switch (msg.what) {
                case ACTION_WORKER_THREAD_QUIT:
                    mWorkerThread.exit();
                    break;
                case ACTION_WORKER_JOIN_CHANNEL:
                    String[] data = (String[]) msg.obj;
                    mWorkerThread.joinChannel();
                    break;
                case ACTION_WORKER_LEAVE_CHANNEL:

                    mWorkerThread.leaveChannel();
                    break;
                case ACTION_WORKER_GANG_UP_JOIN_CHANNEL:
                    String[] datas = (String[]) msg.obj;
                    mWorkerThread.gangUpJoinChannel(datas[0], msg.arg1);
                    break;
                case ACTION_WORKER_GANG_UP_LEAVE_CHANNEL:
                    String gangUpChannel = (String) msg.obj;
                    mWorkerThread.gangUpLeaveChannel();
                    break;

                case ACTION_WORKER_DESTORY_ENGINE:
                    mWorkerThread.destoryEngine();

                    break;
            }
        }
    }

    private WorkerThreadHandler mWorkerHandler;
    private boolean mReady;

    public final void waitForReady() {
        while (!mReady) {
            try {
                Thread.sleep(20);
            } catch (InterruptedException e) {
                e.printStackTrace();
            }
        }
    }

    @Override
    public void run() {
        GameControl.logD(tag + " run ");
        Looper.prepare();
        mWorkerHandler = new WorkerThreadHandler(this);
        ensureRtcEngineReadyLock();
        mReady = true;
        // enter thread looper
        Looper.loop();
    }

    private RtcEngine mRtcEngine;

    public final void joinChannel() {
        if (Thread.currentThread() != this) {
            Message envelop = new Message();
            envelop.what = ACTION_WORKER_JOIN_CHANNEL;
            mWorkerHandler.sendMessage(envelop);
            return;
        }
        ensureRtcEngineReadyLock();
        mRtcEngine.setClientRole(io.agora.rtc.Constants.CLIENT_ROLE_BROADCASTER, null);
        //rtcEngine.setClientRole(io.agora.rtc.Constants.CLIENT_ROLE_BROADCASTER, null);
        mRtcEngine.setParameters("{\"rtc.hq_mode\": {\"hq\": true, \"broadcaster\":false, \"bitrate\":0}}");
        mRtcEngine.enableAudioVolumeIndication(1000, 3);
        // rtcEngine.setParameters("{\"rtc.log_filter\": 65535}");
        GameControl.logD(tag + "channelName   account  = " + GameControl.currentUser.getChannelName() + "   " + GameControl.currentUser.getMediaUid());
        mRtcEngine.enableVideo();
        mRtcEngine.enableLocalVideo(false);
        mRtcEngine.setVideoProfile(io.agora.rtc.Constants.VIDEO_PROFILE_360P, true);
        mRtcEngine.joinChannel(null, GameControl.currentUser.getChannelName(), "Extra Optional Data", Integer.parseInt(GameControl.currentUser.getMediaUid())); // if you do not specify the uid, we will generate the uid for you
    }

    public final void leaveChannel() {
        if (Thread.currentThread() != this) {
            Message envelop = new Message();
            envelop.what = ACTION_WORKER_LEAVE_CHANNEL;
            mWorkerHandler.sendMessage(envelop);
            return;
        }
        if (mRtcEngine != null) {
            mRtcEngine.leaveChannel();
        }
    }

    private RtcEngine ensureRtcEngineReadyLock() {
        if (mRtcEngine == null) {
            String appId = io.agora.agoraandroidhq.tools.Constants.AGORA_APP_ID;
            if (TextUtils.isEmpty(appId)) {
                throw new RuntimeException("NEED TO use your App ID, get your own ID at https://dashboard.agora.io/");
            }
            try {
                mRtcEngine = RtcEngine.create(mContext, appId, mEngineEventHandler.mRtcEventHandler);
            } catch (Exception e) {
                throw new RuntimeException("NEED TO check rtc sdk init fatal error\n" + Log.getStackTraceString(e));
            }
            mRtcEngine.setChannelProfile(Constants.CHANNEL_PROFILE_LIVE_BROADCASTING);
        }
        return mRtcEngine;
    }

    private RtcEngine gangUpRtcEngine;

    private RtcEngine ensureGangUpRtcEngineReadyLock() {
        if (gangUpRtcEngine == null) {
            String appId = io.agora.agoraandroidhq.tools.Constants.AGORA_APP_ID;
            if (TextUtils.isEmpty(appId)) {
                throw new RuntimeException("NEED TO use your App ID, get your own ID at https://dashboard.agora.io/");
            }
            try {
                gangUpRtcEngine = RtcEngine.create(mContext, appId, mEngineEventHandler.gangUpRtcEngineEventHandler);
                gangUpRtcEngine.setParameters("{\"rtc.hq_mode\": {\"hq\": true, \"broadcaster\":true, \"bitrate\":1000}}");
            } catch (Exception e) {
                throw new RuntimeException("NEED TO check rtc sdk init fatal error\n" + Log.getStackTraceString(e));
            }
        }
        return gangUpRtcEngine;
    }

    public void gangUpJoinChannel(final String channel, int uid) {
        if (Thread.currentThread() != this) {
            Message envelop = new Message();
            envelop.what = ACTION_WORKER_GANG_UP_JOIN_CHANNEL;
            envelop.obj = new String[]{channel};
            envelop.arg1 = uid;
            mWorkerHandler.sendMessage(envelop);
            return;
        }
        ensureGangUpRtcEngineReadyLock();
        gangUpRtcEngine.enableAudio();
        //rtcEngine.enableAudio();
        gangUpRtcEngine.setParameters("{\"rtc.hq_mode\": {\"hq\": true, \"broadcaster\":true, \"bitrate\":50}}");
        gangUpRtcEngine.joinChannel(null, channel, "Extra Optional Data", uid); // if you do not specify the uid, we will generate the uid for you
    }

    public void gangUpLeaveChannel() {
        if (Thread.currentThread() != this) {
            Message envelop = new Message();
            envelop.what = ACTION_WORKER_GANG_UP_LEAVE_CHANNEL;
            mWorkerHandler.sendMessage(envelop);
            return;
        }
        if (mRtcEngine != null && gangUpRtcEngine != null) {
            gangUpRtcEngine.setParameters("{\"rtc.hq_mode\": {\"hq\": true, \"broadcaster\":false, \"bitrate\":0}}");
            gangUpRtcEngine.leaveChannel();
        }
    }

    public MyEngineEventHandler eventHandler() {
        return mEngineEventHandler;
    }

    public RtcEngine getRtcEngine() {
        return mRtcEngine;
    }

    public RtcEngine getGangUpRtcEngine() {
        return gangUpRtcEngine;
    }

    public void destoryEngine() {
        GameControl.logD(tag + "destoryEngine");
        if (Thread.currentThread() != this) {
            Message envelop = new Message();
            envelop.what = ACTION_WORKER_DESTORY_ENGINE;
            mWorkerHandler.sendMessage(envelop);
            return;
        }
        if (gangUpRtcEngine != null) {
            gangUpRtcEngine.leaveChannel();
        }

        if (mRtcEngine != null) {
            mRtcEngine.leaveChannel();
        }

        if (gangUpRtcEngine != null) {
            RtcEngine.destroy(gangUpRtcEngine);
        }

        if (mRtcEngine != null) {
            RtcEngine.destroy(mRtcEngine);
        }
    }

    /**
     * call this method to exit
     * should ONLY call this method when this thread is running
     */
    public final void exit() {
        if (Thread.currentThread() != this) {
            mWorkerHandler.sendEmptyMessage(ACTION_WORKER_THREAD_QUIT);
            return;
        }
        mReady = false;
        // TODO should remove all pending(read) messages
        // exit thread looper
        Looper.myLooper().quit();
        mWorkerHandler.release();
    }

    public WorkerThread(Context context) {
        this.mContext = context;
        this.mEngineEventHandler = new MyEngineEventHandler(mContext);
    }
}
