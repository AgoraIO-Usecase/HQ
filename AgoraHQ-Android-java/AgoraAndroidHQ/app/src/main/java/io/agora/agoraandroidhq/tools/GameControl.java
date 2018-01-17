package io.agora.agoraandroidhq.tools;

import io.agora.agoraandroidhq.module.Question;

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
}
