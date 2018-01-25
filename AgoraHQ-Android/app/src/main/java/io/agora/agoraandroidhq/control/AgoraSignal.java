package io.agora.agoraandroidhq.control;

import android.content.Context;
import android.util.Log;
import android.view.View;

import org.json.JSONException;

import java.security.NoSuchAlgorithmException;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.Map;

import io.agora.agoraandroidhq.tools.GameControl;
import io.agora.agoraandroidhq.tools.HttpUrlUtils;
import io.agora.signaling.hq.AgoraHQSigSDK;

/**
 * Created by zhangtao on 2018/1/23.
 */

public class AgoraSignal {

    public static AgoraSignal agoraSignal;
    private static ArrayList<android.os.Handler> messageHandler;

    private String appId;
    private AgoraHQSigSDK agoraHQSigSDK;
    private Context context;
    private String uid;
    private String channelName;


    private AgoraSignal(Context context, String appId, String uid, String channelName) {
        this.appId = appId;
        this.context = context;
        this.uid = uid;
        this.channelName = channelName;
        agoraHQSigSDK = new AgoraHQSigSDK(context, appId);
        messageHandler = new ArrayList<android.os.Handler>();


    }

    public static AgoraSignal newInstance(Context context, String appId, String uid, String channelName) {
        if (agoraSignal != null) {
            return agoraSignal;
        } else {
            agoraSignal = new AgoraSignal(context, appId, uid, channelName);
            return agoraSignal;
        }
    }


    public  void login() {


       loginAgoraSignal(uid);

    }

    public void addEventHandler(android.os.Handler handler){

        messageHandler.add(handler);
    }

    public void removeEnventHandler(){
        if (messageHandler.size() > 0) {
            for (android.os.Handler handler : messageHandler) {
                messageHandler.remove(handler);
            }
        }
    }


    private String token = "_no_need_token";;

    private  String getAgoraToken() {
        try {
            token = SignalingToken.getToken(appId, null, uid, (int) (System.currentTimeMillis() / 1000) + 100);
        } catch (NoSuchAlgorithmException e) {
            e.printStackTrace();
        }

        return token;
    }


    private void loginAgoraSignal(String account) {

        GameControl.logD("loginAgoraSignal");
        agoraHQSigSDK.login(account,channelName, token, new AgoraHQSigSDK.EventHandler() {
            @Override
            public void onLoginSuccess() {
              //  Log.e("MainActivity", "success");
                GameControl.logD("onLoginSuccess");
                handlerEvent(io.agora.agoraandroidhq.tools.Constants.LOGIN_AGORA_SIGNAL_SUCCESS,0);

            }

            @Override
            public void onError(int error) {
                //Log.e("MainActivity", "error:" + error);
                GameControl.logD("onError");
                handlerEvent(io.agora.agoraandroidhq.tools.Constants.LOGIN_AGORA_SIGNAL_FAIL,0);
            }

            @Override
            public void onChannelMessageReceived(final String channel, final long msgId, final String msg) {
               // Log.e("收到频道消息", "onChannelMessageReceived:[channel : " + channel + ", msgId : " + msgId + ", msg : " + msg + "]");
                GameControl.logD("onChannelMessageReceived  channel = "+ channel +"  msg =  " + msg);
                if(msg!=null) {
                    handlerEvent(io.agora.agoraandroidhq.tools.Constants.AGORA_SIGNAL_RECEIVE, msg);
                }
            }

            @Override
            public void onMessageReceivedFrom(final String account, final long msgId, final String msg) {
                //Log.e("收到点对点消息", "onMessageReceivedFrom:[account : " + account + ", msgId : " + msgId + ", msg : " + msg + "]");



            }
        });
    }


    public void sendChannelMessage(String message,String selfMessage){
        GameControl.logD("sendChannelMessage  = " +message);
        if(agoraHQSigSDK!=null){
            GameControl.logD("sendChannelMessage  =");
            agoraHQSigSDK.sendChannelMessage(GameControl.a++, message);
            if(selfMessage!=null) {
                handlerEvent(io.agora.agoraandroidhq.tools.Constants.AGORA_SIGNAL_SEND, selfMessage);
            }
        }
    }

    public void onLogoutSDKClick() {
        if (agoraHQSigSDK != null) {
            agoraHQSigSDK.logout();
        }
    }

    //        token = "_no_need_token";
    private void handlerEvent( int what,Object obj) {
        if (messageHandler.size() > 0) {
            for (android.os.Handler handler : messageHandler) {

                GameControl.logD("handlerEvent  =  "+messageHandler.size());

                android.os.Message m = android.os.Message.obtain();
                m.what = what;
                m.obj = obj;
                handler.sendMessage(m);
            }
        }
    }

    public static void checkRelive(HttpUrlUtils.OnResponse callback) throws JSONException {

        Map data = new HashMap();
        GameControl.logD("checkRelive   uid =  "+GameControl.currentUser.account +"   gid =  "+GameControl.currentUser.channelName);
        data.put("uid", GameControl.currentUser.account);
        data.put("gid", GameControl.currentUser.channelName);

        HttpUrlUtils utils = new HttpUrlUtils();
        utils.execHttpAsyncTask(io.agora.agoraandroidhq.tools.Constants.HTTP_RELIVE, true, callback, data);


    }


    public static void sendAnswerToserver(int sid, int result, HttpUrlUtils.OnResponse callback) throws JSONException {

        Map data = new HashMap();
        GameControl.logD("sendAnswerToserver   uid  =  "+GameControl.currentUser.account +"   gid =  "+GameControl.currentUser.channelName +"  sid = "+sid +"  result = "+result );
        data.put("uid", GameControl.currentUser.account);
        data.put("gid", GameControl.currentUser.channelName);
        data.put("sid", sid);
        data.put("result", result);

        HttpUrlUtils utils = new HttpUrlUtils();
        utils.execHttpAsyncTask(io.agora.agoraandroidhq.tools.Constants.HTTP_SEND_ANSWER_TO_SERVER, true, callback, data);
    }


    public static void checkWheatherCanPlay(HttpUrlUtils.OnResponse callback) throws JSONException {

       // String url = "http://123.155.153.87:8000/v1/canplay?gid=10001&uid=24324242";
        String url = "http://123.155.153.87:9000/v1/canplay?gid="+GameControl.currentUser.channelName+"&uid="+GameControl.currentUser.account;
        GameControl.logD("checkWheatherCanPlay  =  "+ url);
       /* HttpUrlUtils utils = new HttpUrlUtils();

        utils.execHttpAsyncTask(url, false, callback, null);
*/
    }
}
