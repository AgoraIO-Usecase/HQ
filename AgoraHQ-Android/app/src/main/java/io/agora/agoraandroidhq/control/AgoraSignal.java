package io.agora.agoraandroidhq.control;

import android.content.Context;
import android.os.Handler;
import android.util.Log;
import android.view.View;

import org.json.JSONException;

import java.lang.ref.WeakReference;
import java.security.NoSuchAlgorithmException;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.Map;
import java.util.concurrent.CopyOnWriteArrayList;

import io.agora.agoraandroidhq.R;
import io.agora.agoraandroidhq.tools.Constants;
import io.agora.agoraandroidhq.tools.GameControl;
import io.agora.agoraandroidhq.tools.HttpUrlUtils;
import io.agora.signaling.hq.AgoraHQSigSDK;

/**
 * Created by zhangtao on 2018/1/23.
 */

public class AgoraSignal {

    public static AgoraSignal agoraSignal;
    private  CopyOnWriteArrayList<Handler> messageHandler;
    private static String tag = "[AgoraSignal]  ";
    private String appId;
    private AgoraHQSigSDK agoraHQSigSDK;
    private Context context;
    private String signalAccount;
    private String channelName;
    private SignalEvent signalEvent;

    private AgoraSignal(Context context, String appId, String signalAccount, String channelName) {
        this.appId = appId;
        this.context = context;
        this.signalAccount = signalAccount;
        this.channelName = channelName;
        agoraHQSigSDK = new AgoraHQSigSDK(context, appId);
        messageHandler = new CopyOnWriteArrayList<Handler>();
        signalEvent = new SignalEvent();
    }

    public static AgoraSignal newInstance(Context context, String appId, String signalAccount, String channelName) {
        if (agoraSignal != null) {
            return agoraSignal;
        } else {

            WeakReference<Context> weakReference = new WeakReference<Context>(context);
            agoraSignal = new AgoraSignal(weakReference.get(), appId, signalAccount, channelName);
            return agoraSignal;
        }
    }


    public void login() {
        loginAgoraSignal(signalAccount);
    }

    public void addEventHandler(android.os.Handler handler) {

        messageHandler.add(handler);
    }

    public void removeEnventHandler() {
        if (messageHandler.size() > 0) {
            for (android.os.Handler handler : messageHandler) {
                messageHandler.remove(handler);
            }
        }
    }

    private String token = "_no_need_token";

    private void loginAgoraSignal(String account) {
        GameControl.logD(tag + "loginAgoraSignal =  " + account + "  channelName  " + channelName);
        agoraHQSigSDK.login(account, channelName, token, signalEvent);
    }


    public void sendChannelMessage(String message, String selfMessage) {
        GameControl.logD(tag + "sendChannelMessage  = " + message);
        if (agoraHQSigSDK != null) {
            GameControl.logD(tag + "sendChannelMessage  =");
            GameControl.logD(tag + "agoraHQSigSDK  =  " + agoraHQSigSDK);
            agoraHQSigSDK.sendChannelMessage(GameControl.MESSAGE_ID++, message);
            if (selfMessage != null) {
                handlerEvent(io.agora.agoraandroidhq.tools.Constants.AGORA_SIGNAL_SEND, selfMessage);
            }
        }
    }

    public void onLogoutSDKClick() {
        GameControl.logD(tag + "onLogoutSDKClick  11 ");
        if (agoraHQSigSDK != null) {
            agoraHQSigSDK.logout();
            AgoraSignal.agoraSignal = null;
            agoraHQSigSDK = null;
            agoraSignal = null;
            signalEvent = null;
            GameControl.logD(tag + "onLogoutSDKClick  22");
        }
    }

    //        token = "_no_need_token";
    private void handlerEvent(int what, Object obj) {
        if (messageHandler.size() > 0) {
            for (android.os.Handler handler : messageHandler) {
                GameControl.logD(tag + "handlerEvent  =  " + messageHandler.size());
                android.os.Message m = android.os.Message.obtain();
                m.what = what;
                m.obj = obj;
                handler.sendMessage(m);
            }
        }
    }

    public static void checkRelive(HttpUrlUtils.OnResponse callback) throws JSONException {
        Map data = new HashMap();
        GameControl.logD(tag + "checkRelive   uid =  " + GameControl.currentUser.getMediaUid() + "   gid =  " + GameControl.currentUser.getChannelName());
        data.put("uid", GameControl.currentUser.getSignalAccount());
        data.put("gid", GameControl.currentUser.getChannelName());
        HttpUrlUtils utils = new HttpUrlUtils();
        utils.execHttpAsyncTask(io.agora.agoraandroidhq.tools.Constants.HTTP_RELIVE, true, callback, data);
    }

    public static void wheatherAcceptVideoWithBroadCast(boolean wheatherAccept, String account, String mediaUid, String channelName) {
        Map data = new HashMap();
        data.put("accept", wheatherAccept);
        data.put("account", account);
        data.put("mediaUid", mediaUid);
        data.put("gid", channelName);

        HttpUrlUtils urlUtils = new HttpUrlUtils();
        urlUtils.execHttpAsyncTask(Constants.HTTP_REPLY_WHEATHER_ACCEPT_VIDEO, true, null, data);
        GameControl.logD(tag + "wheatherAcceptVideoWithBroadCast  wheather  map =  " + data.toString());
    }

    public static void sendAnswerToserver(int sid, int result, HttpUrlUtils.OnResponse callback) throws JSONException {
        if (GameControl.serverWheatherCanPlay && GameControl.clientWheatherCanPlay) {
            Map data = new HashMap();
            GameControl.logD(tag + "sendAnswerToserver   uid  =  " + GameControl.currentUser.getMediaUid() + "   gid =  " + GameControl.currentUser.getChannelName() + "  sid = " + sid + "  result = " + result);
            data.put("uid", GameControl.currentUser.getSignalAccount());
            data.put("gid", GameControl.currentUser.getChannelName());
            data.put("sid", sid);
            data.put("result", result);
            HttpUrlUtils utils = new HttpUrlUtils();
            utils.execHttpAsyncTask(io.agora.agoraandroidhq.tools.Constants.HTTP_SEND_ANSWER_TO_SERVER, true, callback, data);
        }
    }

    public static void checkWheatherCanPlay(HttpUrlUtils.OnResponse callback) throws JSONException {
        // String url = "http://123.155.153.87:8000/v1/canplay?gid=10001&uid=24324242";
        String url = Constants.HTTP_CHECK_WHEATHER_CAN_PLAY + GameControl.currentUser.getChannelName() + "&uid=" + GameControl.currentUser.getSignalAccount();
        GameControl.logD(tag + "checkWheatherCanPlay  =  " + url);
        HttpUrlUtils utils = new HttpUrlUtils();
        utils.execHttpAsyncTask(url, false, callback, null);
    }

    class SignalEvent implements AgoraHQSigSDK.EventHandler {
        @Override
        public void onLoginSuccess() {
            //  Log.e("MainActivity", "success");
            GameControl.logD(tag + "onLoginSuccess");
            handlerEvent(io.agora.agoraandroidhq.tools.Constants.LOGIN_AGORA_SIGNAL_SUCCESS, 0);
        }

        @Override
        public void onError(int error) {
            //Log.e("MainActivity", "error:" + error);
            GameControl.logD(tag + "onError");
            handlerEvent(io.agora.agoraandroidhq.tools.Constants.LOGIN_AGORA_SIGNAL_FAIL, error);
        }

        @Override
        public void onChannelMessageReceived(final String channel, final long msgId, final String msg) {
            // Log.e("收到频道消息", "onChannelMessageReceived:[channel : " + channel + ", msgId : " + msgId + ", msg : " + msg + "]");
            GameControl.logD(tag + "onChannelMessageReceived  channel = " + channel + "  msg =  " + msg);
            if (msg != null) {
                handlerEvent(io.agora.agoraandroidhq.tools.Constants.AGORA_SIGNAL_RECEIVE, msg);
            }
        }

        @Override
        public void onMessageReceivedFrom(final String account, final long msgId, final String msg) {
            //Log.e("收到点对点消息", "onMessageReceivedFrom:[account : " + account + ", msgId : " + msgId + ", msg : " + msg + "]");
            GameControl.logD(tag + "onChannelMessageReceived  account = " + account + "  msg =  " + msg);
            if (msg != null) {
                handlerEvent(io.agora.agoraandroidhq.tools.Constants.AGORA_SIGNAL_RECEIVE, msg);
            }
        }
    }
}
