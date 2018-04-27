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
    private int totalQuestion;
    private int timeOut;
    private String encrypt = "";
    private int sid = -1;

    public Question(int id, String question, String chooseType, ArrayList answerString, int totalQuestion, int timeOut, String encrypt, int sid) {
        this.id = id;
        this.question = question;
        this.chooseType = chooseType;
        this.answerString = answerString;
        this.totalQuestion = totalQuestion;
        this.timeOut = timeOut;
        this.encrypt = encrypt;
        this.sid = sid;
    }

    public Question(int id, String question, String chooseType, ArrayList answerString, int sid) {
        this.id = id;
        this.question = question;
        this.chooseType = chooseType;
        this.answerString = answerString;
        this.sid = sid;
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

    public void setSid(int currentId) {
        sid = currentId;
    }

    public int getSid() {

        return sid;
    }

    public String getQuestion() {
        return question;
    }

    public int getTotalQuestion() {
        return totalQuestion;
    }

    public int getTimeOut() {
        return timeOut;
    }

    public String getEncrypt() {
        return encrypt;
    }
}
