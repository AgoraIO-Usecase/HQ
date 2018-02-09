package io.agora.agoraandroidhq.module;

/**
 * Created by zhangtao on 2018/1/13.
 */

public class Message {

    private String sender;
    private String content;
    private int type ;

    private boolean isMe;

    public Message(int type , String sender,String content){
        this.type = type;
        this.sender = sender;
        this.content = content;
        isMe = false;
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

    public boolean getIsMe(){
        return isMe;
    }

    public void setIsMe(boolean isMe){
        this.isMe = isMe;
    }
}
