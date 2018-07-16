package io.agora.livequiz.module;

import android.graphics.drawable.Drawable;

import java.util.Random;

/**
 * Created by zhangtao on 2018/1/12.
 */

public class User {

    private Random random = new Random();
    //public static String uid = "232894";
    private String mediaUid = "2434" + random.nextInt(10000);
    private String userName;
    private String channelName;
    private Drawable drawable;
    private String signalAccount;

    public void setDrawable(Drawable drawable) {
        this.drawable = drawable;
    }

    public Drawable getDrawable() {
        return drawable;
    }

    public String getMediaUid() {
        return mediaUid;
    }

    public void setUserName(String userName) {
        this.userName = userName;
    }

    public String getUserName() {
        return userName;
    }

    public void setChannelName(String channelName) {
        this.channelName = channelName;
    }

    public String getChannelName() {
        return channelName;
    }

    public String getSignalAccount() {
        return signalAccount;
    }

    public void setSignalAccount(String signalAccount) {
        this.signalAccount = signalAccount;
    }
}
