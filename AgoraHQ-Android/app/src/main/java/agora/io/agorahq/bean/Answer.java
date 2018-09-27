package agora.io.agorahq.bean;

public class Answer {
    private int sid;
    private int result;

    public Answer(int sid, int result){
        this.sid = sid;
        this.result = result;
    }

    public int getSid() {
        return sid;
    }

    public void setSid(int sid) {
        this.sid = sid;
    }

    public int getResult() {
        return result;
    }

    public void setResult(int result) {
        this.result = result;
    }

    public String toString(){
        return "sid:" + sid + ", result:" + result;
    }
}
