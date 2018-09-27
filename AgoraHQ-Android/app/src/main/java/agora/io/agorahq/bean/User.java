package agora.io.agorahq.bean;

import android.graphics.drawable.Drawable;

public class User {
    private int mediaUid;
    private String userName;
    private String channelName;
    private Drawable drawable;
    private String signalAccount;

    public User(String userName, String signalAccount, String channelName, Drawable drawable) {
        this.userName = userName;
        this.signalAccount = signalAccount;
        this.channelName = channelName;
        this.drawable = drawable;
    }

    @Override
    public String toString() {
        return "MediaUid: " + mediaUid + "   user name: " + userName + "    channel name: " + channelName
                + " drawable: " + (drawable == null) + "    signal account: " + signalAccount;
    }

    public int getMediaUid() {
        return mediaUid;
    }

    public void setMediaUid(int mediaUid) {
        this.mediaUid = mediaUid;
    }

    public String getUserName() {
        return userName;
    }

    public void setUserName(String userName) {
        this.userName = userName;
    }

    public String getChannelName() {
        return channelName;
    }

    public void setChannelName(String channelName) {
        this.channelName = channelName;
    }

    public Drawable getDrawable() {
        return drawable;
    }

    public void setDrawable(Drawable drawable) {
        this.drawable = drawable;
    }

    public String getSignalAccount() {
        return signalAccount;
    }

    public void setSignalAccount(String signalAccount) {
        this.signalAccount = signalAccount;
    }
}
