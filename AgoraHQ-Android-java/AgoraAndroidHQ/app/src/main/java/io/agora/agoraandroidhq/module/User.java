package io.agora.agoraandroidhq.module;

import android.net.Uri;
import android.provider.Settings;

import java.net.URI;
import java.util.ArrayList;

import io.rong.imlib.model.UserInfo;

/**
 * Created by zhangtao on 2018/1/12.
 */

public class User {

    //public static String uid = "232894";
    public static String uid = "2424252";
    public static String name = "Test";

    // public static String name = "Agora";
    public static Uri portUri = Uri.parse("http://7xs9j5.com1.z0.glb.clouddn.com/liveapp/anlingrong.jpg");

    public String password;
    public String imageUrl;

    public void setUid(String uid) {
        this.uid = uid;
    }

    public void setPassword(String password) {
        this.password = password;
    }

    public void setImageUrl(String imageUrl) {
        this.imageUrl = imageUrl;
    }

}
