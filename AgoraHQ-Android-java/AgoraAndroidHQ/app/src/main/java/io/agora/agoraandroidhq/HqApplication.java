package io.agora.agoraandroidhq;

import android.app.Application;

import io.agora.agoraandroidhq.control.AgoraLinkToCloud;
import io.agora.agoraandroidhq.tools.Constants;

/**
 * Created by zhangtao on 2018/1/12.
 */

public class HqApplication extends Application {

    @Override
    public void onCreate() {
        super.onCreate();
        //TODO
        AgoraLinkToCloud.init(getApplicationContext(), Constants.APP_KEY);

    }
}
