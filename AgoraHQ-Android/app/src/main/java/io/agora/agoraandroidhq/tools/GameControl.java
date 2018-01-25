package io.agora.agoraandroidhq.tools;

import android.util.Log;

import io.agora.agoraandroidhq.module.Question;
import io.agora.agoraandroidhq.module.User;

/**
 * Created by zhangtao on 2018/1/15.
 */

public class GameControl {

    public static boolean clientWheatherCanPlay = true;

    public static boolean serverWheatherCanPlay = true;

    public static Question currentQuestion;

    public static void setCurrentQuestion(Question question) {
        currentQuestion = question;
    }

    public static boolean controlCheckThread = true;

    public static User currentUser;

    public static void logD(String message){
        Log.d("agora_signal",message);
    }
    public static int a =2;
}
