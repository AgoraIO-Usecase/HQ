package agora.io.agorahq.bean;

public class ChatMessage {
    private String account;
    private String content;
    private boolean isMe;

    public ChatMessage(String account, String content, boolean isMe) {
        this.account = account;
        this.content = content;
        this.isMe = isMe;
    }

    public ChatMessage(String account, String content) {
        this(account, content, false);
    }

    public String getSendAccount() {
        return account;
    }

    public String getContent() {
        return content;
    }

    public boolean getIsMe() {
        return isMe;
    }
}
