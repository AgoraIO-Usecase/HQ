package io.agora.agoraandroidhq.module;

import java.util.ArrayList;

/**
 * Created by zhangtao on 2018/1/15.
 */

public class Question {

    private int id;
    private String question;
    private String chooseType;
    private ArrayList answerString;


    public Question(int id, String question, String chooseType , ArrayList answerString){
        this.id = id;
        this.question = question;
        this.chooseType = chooseType;
        this.answerString = answerString;
    }


    public ArrayList getAnswerString() {
        return answerString;
    }

    public String getChooseType() {
        return chooseType;
    }

    public int getId() {
        return id;
    }

    public String getQuestion() {
        return question;
    }
}
