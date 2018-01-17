package io.agora.agoraandroidhq.module;

/**
 * Created by zhangtao on 2018/1/15.
 */

public class Result {


    public int correct;
    public String total;
    public String sid;
    public int result;


    public Result(int correct,String total,String sid,int result){
        this.correct = correct;
        this.total = total;
        this.sid = sid;
        this.result = result;
    }

    public int getCorrect() {
        return correct;
    }

    public int getResult() {
        return result;
    }

    public String getSid() {
        return sid;
    }

    public String getTotal() {
        return total;
    }
}
