package io.agora.signaling.hq;

import android.content.Context;
import android.util.Log;

import org.json.JSONException;
import org.json.JSONObject;


import java.util.ArrayList;
import java.util.List;

import io.rong.imlib.RongIMClient;
import io.rong.imlib.model.Message;

import static io.agora.signaling.hq.StatisticsData.statisticsItem;

public class AgoraLinkToCloud {

    private static final String LOG_TAG = "AgoraLinkToCloud";

    public static void init(Context context, String appkey) {
        RongIMClient.init(context, appkey);
    }

    public static void setConnectionStatusListener(RongIMClient.ConnectionStatusListener connectionStatusListener) {
        RongIMClient.setConnectionStatusListener(connectionStatusListener);
    }

    public static void setOnReceiveMessageListener(final String account, final AgoraHQSigSDK.InternalMessageEventHandler handler) {
        RongIMClient.setOnReceiveMessageListener(new RongIMClient.OnReceiveMessageListener() {
            @Override
            public boolean onReceived(Message message, int i) {
                Log.d(LOG_TAG, message.getConversationType().getName());
                if ("private".equals(message.getConversationType().getName())) {
                    String msg = null;
                    try {
                        JSONObject jsonObject = new JSONObject(new String(message.getContent().encode()));
                        msg = jsonObject.getString("content");
                    } catch (JSONException e) {
                        Log.w(LOG_TAG, Log.getStackTraceString(e));
                    }

                    JSONObject jsonObject;
                    long msgId = 0;
                    String m = null;
                    try {
                        jsonObject = new JSONObject(msg);
                        msgId = jsonObject.getLong("id");
                        m = jsonObject.getString("m");
                    } catch (JSONException e) {
                        Log.w(LOG_TAG, Log.getStackTraceString(e));
                    }

                    handler.onMessageReceivedFrom(message.getSenderUserId(), msgId, m);

                    List<Object> data = new ArrayList<>();
                    data.add(System.currentTimeMillis());
                    data.add("r");
                    data.add("i");
                    data.add(msgId);
                    data.add(message.getSenderUserId());
                    data.add(account);
                    if (statisticsItem != null) {
                        statisticsItem.addMsgsItem(data);
                    }
                } else if ("chatroom".equals(message.getConversationType().getName())) {
                    String msg = null;
                    try {
                        JSONObject jsonObject = new JSONObject(new String(message.getContent().encode()));
                        msg = jsonObject.getString("content");
                    } catch (JSONException e) {
                        Log.w(LOG_TAG, Log.getStackTraceString(e));
                    }

                    JSONObject jsonObject;
                    long msgId = 0;
                    String m = null;
                    try {
                        jsonObject = new JSONObject(msg);
                        msgId = jsonObject.getLong("id");
                        m = jsonObject.getString("m");
                    } catch (JSONException e) {
                        Log.w(LOG_TAG, Log.getStackTraceString(e));
                    }

                    handler.onChannelMessageReceived(message.getTargetId(), msgId, m);

                    List<Object> data = new ArrayList<>();
                    data.add(System.currentTimeMillis());
                    data.add("r");
                    data.add("c");
                    data.add(msgId);
                    data.add(message.getSenderUserId());
                    if (statisticsItem != null) {
                        statisticsItem.addMsgsItem(data);
                    }
                }
                return false;
            }
        });
    }

    public static void getRCToken(String account, String channel, String vid, HttpUrlUtils.OnResponse callback) {
        logD("getRCToken " + account + " " + channel + " " + vid);

        StringBuilder rawJson = new StringBuilder("{");
        rawJson.append("\"uid\":").append("\"").append(account).append("\", ");
        rawJson.append("\"name\":").append("\"").append(channel).append("\", ");
        rawJson.append("\"vid\":").append("\"").append(vid).append("\"");
        rawJson.append("}");

        HttpUrlUtils utils = new HttpUrlUtils();

        utils.execHttpAsyncTask(Constants.RY_HTTP_GET_TOKEN_, true, callback, rawJson.toString());
    }

    public static void connect(String token, final RongIMClient.ConnectCallback callback) {
        RongIMClient.connect(token, callback);
    }

    public static void joinChatRoom(String roomId, int defMessageCount, final RongIMClient.OperationCallback callback) {
        RongIMClient.getInstance().joinChatRoom(roomId, defMessageCount, callback);
    }

    public static void quitChatRoom(final RongIMClient.OperationCallback callback, String roomId) {
        RongIMClient.getInstance().quitChatRoom(roomId, callback);
    }

    private static void logD(String message) {
        Log.d(AgoraHQSigSDK.LOG_TAG, message);
    }


}
