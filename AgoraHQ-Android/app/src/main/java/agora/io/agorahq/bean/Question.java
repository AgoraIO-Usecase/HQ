package agora.io.agorahq.bean;

import java.util.ArrayList;

public class Question {
    /**
     * question title
     */
    private String question;

    // not use now
    private String chooseType;
    /**
     * answer options
     */
    private ArrayList<String> answerString;

    /**
     * number of questions
     */
    private int totalQuestionCount;

    private int timeout;

    /**
     * encrypt type
     */
    private String encrpyt = "";

    /**
     * question index
     */
    private int sid = -1;

    public Question(String question, String chooseType, ArrayList<String> answerString, int totalQuestion, int timeOut, String encrypt, int sid) {
        this.question = question;
        this.chooseType = chooseType;
        this.answerString = answerString;
        this.totalQuestionCount = totalQuestion;
        this.timeout = timeOut;
        this.encrpyt = encrypt;
        this.sid = sid;
    }

    public Question(String question, String chooseType, ArrayList<String> answerString, int sid) {
        this.question = question;
        this.chooseType = chooseType;
        this.answerString = answerString;
        this.sid = sid;
    }

    public ArrayList<String> getAnswerString() {
        return answerString;
    }

    public String getChooseType() {
        return chooseType;
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
        return totalQuestionCount;
    }

    public int getTimeOut() {
        return timeout;
    }

    public String getEncrypt() {
        return encrpyt;
    }
}
