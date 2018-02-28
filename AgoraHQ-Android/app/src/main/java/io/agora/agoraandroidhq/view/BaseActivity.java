package io.agora.agoraandroidhq.view;

import android.app.Activity;
import android.content.pm.ActivityInfo;
import android.os.Bundle;
import android.support.annotation.Nullable;
import android.view.Window;
import android.view.WindowManager;
import android.widget.Toast;

import io.agora.agoraandroidhq.HqApplication;
import io.agora.agoraandroidhq.R;
import io.agora.agoraandroidhq.control.WorkerThread;
import io.agora.agoraandroidhq.module.MyEngineEventHandler;
import io.agora.agoraandroidhq.tools.Constants;
import io.agora.agoraandroidhq.tools.GameControl;
import io.agora.rtc.RtcEngine;

/**
 * Created by zhangtao on 2018/2/7.
 */

public abstract class BaseActivity extends Activity {

    private String tag = "[BaseActivity]  ";
    @Override
    protected void onCreate(@Nullable Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        this.requestWindowFeature(Window.FEATURE_NO_TITLE);
        this.getWindow().setFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN, WindowManager.LayoutParams.FLAG_FULLSCREEN);
        setRequestedOrientation(ActivityInfo.SCREEN_ORIENTATION_PORTRAIT);
    }

    protected abstract void initUIandEvent();
    protected abstract void deInitUIandEvent();
    protected abstract void setUiListener();
    @Override
    protected void onPostCreate(@Nullable Bundle savedInstanceState) {
        super.onPostCreate(savedInstanceState);
        ((HqApplication) getApplication()).initWorkerThread();
    }

    @Override
    protected void onDestroy() {
        deInitUIandEvent();
        super.onDestroy();
    }

    protected RtcEngine rtcEngine() {
        return ((HqApplication) getApplication()).getWorkerThread().getRtcEngine();
    }

    protected final WorkerThread worker() {
        return ((HqApplication) getApplication()).getWorkerThread();
    }

    protected final MyEngineEventHandler event() {
        return ((HqApplication) getApplication()).getWorkerThread().eventHandler();
    }

    public final void showShortToast(final String msg) {
        this.runOnUiThread(new Runnable() {
            @Override
            public void run() {
                Toast.makeText(getApplicationContext(), msg, Toast.LENGTH_SHORT).show();
            }
        });
    }

    protected void initConstantsHttp() {
        //Can not change this
        Constants.HTTP_RELIVE = getResources().getString(R.string.http_relive);
        Constants.HTTP_SEND_ANSWER_TO_SERVER = getResources().getString(R.string.http_send_answer_to_server);
        Constants.HTTP_CHECK_WHEATHER_CAN_PLAY = getResources().getString(R.string.http_check_wheather_can_play);
        Constants.HTTP_REPLY_WHEATHER_ACCEPT_VIDEO = getString(R.string.http_reply_wheather_accept_video);
        GameControl.logD(tag + "Http_relive = " + Constants.HTTP_RELIVE);
        GameControl.logD(tag + "Http_send_answer = " + Constants.HTTP_SEND_ANSWER_TO_SERVER);
        GameControl.logD(tag + "Http_Check_Wheather_can_paly = " + Constants.HTTP_CHECK_WHEATHER_CAN_PLAY);
        GameControl.logD(tag + "Http_Check_Wheather_can_paly = " + Constants.HTTP_CHECK_WHEATHER_CAN_PLAY);
    }
}
