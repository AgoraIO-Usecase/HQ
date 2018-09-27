package agora.io.agorahq.bean;

public class QuestionAnswerResult {
    // total count
    public String total;
    // question id
    public String sid;
    // answer result
    public int result;

    private int timeout = 5; // 5 secs

    public QuestionAnswerResult(String total, String sid, int result) {
        this.total = total;
        this.sid = sid;
        this.result = result;
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

    public int getTimeout() {
        return timeout;
    }
}
