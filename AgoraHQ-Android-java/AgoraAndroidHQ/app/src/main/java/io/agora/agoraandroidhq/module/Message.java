package io.agora.agoraandroidhq.module;

/**
 * Created by zhangtao on 2018/1/13.
 */

public class Message {

    private String sender;
    private String content;
    private int type ;

    public Message(int type , String sender,String content){
        this.type = type;
        this.sender = sender;
        this.content = content;
    }

    public Message(String sender, String content){
        this(0,sender,content);
    }
    public String getSender(){
        return sender;
    }
    public int getType(){
        return type;
    }
    public String getContent(){
        return content;
    }

}
