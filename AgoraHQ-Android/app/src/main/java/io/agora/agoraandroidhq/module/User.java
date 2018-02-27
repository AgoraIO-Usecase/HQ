package io.agora.agoraandroidhq.module;

import android.graphics.drawable.Drawable;
import android.widget.ImageView;

import java.util.Random;


/**
 * Created by zhangtao on 2018/1/12.
 */

public class User {

    public Random random = new Random();
    //public static String uid = "232894";
    public String account = "2434" + random.nextInt(10000);
    public String name = "Test" + random.nextInt(1000);
    public String channelName;
    public boolean wheatherCanPlay;
    // public static String name = "Agora";
    // public static Uri portUri = Uri.parse("http://7xs9j5.com1.z0.glb.clouddn.com/liveapp/anlingrong.jpg");
    public String getAccount() {
        return account;
    }
    public String getName() {
        return name;
    }
    public void setChannelName(String channelName) {
        this.channelName = channelName;
    }
    public String getChannelName() {
        return channelName;
    }
    public Drawable drawable;
    public String signalAccount;
    public String getSignalAccount(){
        return signalAccount;
    }
}
