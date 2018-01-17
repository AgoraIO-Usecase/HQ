package io.agora.agoraandroidhq.control;

import android.annotation.TargetApi;
import android.content.Context;
import android.net.Uri;
import android.os.Build;
import android.os.Handler;
import android.util.ArrayMap;
import android.util.Log;


import org.json.JSONException;
import org.json.JSONObject;

import java.net.URL;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.Map;

import io.agora.agoraandroidhq.module.User;
import io.agora.agoraandroidhq.tools.Constants;
import io.agora.agoraandroidhq.tools.HttpUrlUtils;
import io.rong.imlib.RongIMClient;
import io.rong.imlib.model.Conversation;
import io.rong.imlib.model.Message;
import io.rong.imlib.model.MessageContent;
import io.rong.imlib.model.UserInfo;

import android.os.Handler;

/**
 * Created by zhangtao on 2018/1/12.
 */

@TargetApi(Build.VERSION_CODES.KITKAT)
public class AgoraLinkToCloud {


    //public static final ArrayMap<String, String> randomName;
    public static String currentRoomId;
    public static UserInfo currentUser;

    public static ArrayList<Handler> messageHandler = new ArrayList<Handler>();

    public static void init(Context context, String appkey) {
        RongIMClient.init(context, appkey);
        RongIMClient.setOnReceiveMessageListener(onReceiveMessageListener);
    }

    private static RongIMClient.OnReceiveMessageListener onReceiveMessageListener = new RongIMClient.OnReceiveMessageListener() {
        @Override
        public boolean onReceived(Message message, int i) {
            //TODO
            handlerEvent(Constants.MESSAGE_RECEIVE, message);

            return false;
        }
    };


    public static void addEventHandler(Handler handler) {

        messageHandler.add(handler);
    }

    public static void removeEventHandler(Handler handler) {
        messageHandler.remove(handler);
    }

    private static void handlerEvent(int what, Object obj) {
        for (Handler handler : messageHandler) {
            android.os.Message m = android.os.Message.obtain();
            m.what = what;
            m.obj = obj;
            handler.sendMessage(m);
        }
    }


    public static void getToken(UserInfo userInfo, HttpUrlUtils.OnResponse callback) throws JSONException {

        logD("getTokenAgora  " + userInfo.getName() + "   " + userInfo.getUserId() + "   " + userInfo.getPortraitUri());

        Map data = new HashMap();
        data.put("uid", Constants.UID);

        HttpUrlUtils utils = new HttpUrlUtils();

        utils.execHttpAsyncTask(Constants.HTTP_GET_TOKEN, true, callback, data);
    }


    public static void checkWheatherCanPlay(HttpUrlUtils.OnResponse callback) throws JSONException {

        String url = "http://123.155.153.87:8000/v1/canplay?gid=10001&uid=24324242";


       /* HttpUrlUtils utils = new HttpUrlUtils();

        utils.execHttpAsyncTask(url, false, callback, null);
*/
    }

    public static void checkRelive(HttpUrlUtils.OnResponse callback) throws JSONException {

        Map data = new HashMap();
        data.put("uid", Constants.UID);
        data.put("gid", 10001);

        HttpUrlUtils utils = new HttpUrlUtils();
        utils.execHttpAsyncTask(Constants.HTTP_RELIVE, true, callback, data);


    }


    public static void sendAnswerToserver(int sid, int result, HttpUrlUtils.OnResponse callback) throws JSONException {

        Map data = new HashMap();
        data.put("uid", Constants.UID);
        data.put("gid", "10001");
        data.put("sid", sid);
        data.put("result", result);

        HttpUrlUtils utils = new HttpUrlUtils();
        utils.execHttpAsyncTask(Constants.HTTP_SEND_ANSWER_TO_SERVER, true, callback, data);
    }

    public static void connect(String token, final RongIMClient.ConnectCallback callback) {
        RongIMClient.connect(token, callback);
    }

    public static void joinChatRoom(String roomId, int defMessageCount, final RongIMClient.OperationCallback callback) {
        currentRoomId = roomId;
        RongIMClient.getInstance().joinChatRoom(currentRoomId, defMessageCount, callback);
    }

    public static void quitChatRoom(final RongIMClient.OperationCallback callback, String roomId) {
        RongIMClient.getInstance().quitChatRoom(roomId, callback);
    }

    public static void sendMessage(final MessageContent messageContent, String roomId) {

        messageContent.setUserInfo(currentUser);
        final Message msg = Message.obtain(roomId, Conversation.ConversationType.CHATROOM, messageContent);
        RongIMClient.getInstance().sendMessage(msg, null, null, new RongIMClient.SendMessageCallback() {
            @Override
            public void onError(Integer integer, RongIMClient.ErrorCode errorCode) {
                //TODO
                logD(" sendMessage onError  errorCode = " + errorCode);
                // handlerEvent(Constants.MESSAGE_RECEIVE,errorCode);
            }

            @Override
            public void onSuccess(Integer integer) {
                //TODO

                logD("  onSendMessageSuccess");

                handlerEvent(Constants.MESSAGE_RECEIVE, msg);
            }
        });
    }

    public static void setCurrentUser(UserInfo user) {
        currentUser = user;
        logD("currentUser  = " + currentUser.getUserId() + "   " + currentUser.getName() + "   " + currentUser.getPortraitUri());
    }

    public static UserInfo getCurrentUser() {
        return currentUser;
    }


    public static UserInfo getLoginUser(String id, String password) {

        long time = System.currentTimeMillis();
        //Random ran = new Random(time);
        //int n = ran.nextInt(randomName.size());
        String uid = Long.toString(time);
        String name = User.name;
        Uri imageUri = User.portUri;

        return new UserInfo(uid, name, imageUri);
    }

    private static void logD(String message) {
        Log.d("zhang   ", message);
    }


}