package io.agora.agoraandroidhq.tools;

import android.graphics.drawable.Drawable;
import android.util.Log;

import java.lang.ref.WeakReference;

import io.agora.agoraandroidhq.module.Question;
import io.agora.agoraandroidhq.module.User;
import io.agora.rtc.RtcEngine;

/**
 * Created by zhangtao on 2018/1/15.
 */

public class GameControl {

    public static boolean clientWheatherCanPlay = true;
    public static boolean serverWheatherCanPlay = true;
    public static Question currentQuestion;
    public static final boolean SHOW_LOG = true;
    public static boolean controlCheckThread = true;
    public static User currentUser;
    public static int MESSAGE_ID = 2;
    public static Drawable currentUserHeadImage;
    public static String currentUserName;
    public static String signalAccount;
    public static int timeOut = 10;
    public static int total = 10;
    public static int result = -1;
    public static void setCurrentQuestion(Question question) {
        currentQuestion = question;
    }
    public static void logD(String message) {
        if (SHOW_LOG) {
            Log.d("agora_signal", message);
        }
    }
}
