package io.agora.agoraandroidhq.tools;

import android.annotation.SuppressLint;
import android.util.ArrayMap;

import java.net.URL;
import java.util.ArrayList;
import java.util.HashMap;

/**
 * Created by zhangtao on 2018/1/12.
 */

public class Constants {

    // url to get toke
    //public static  final String HTTP_GET_TOKEN ="http://10.60.2.75:8000/v1/token";
    public static  final String HTTP_GET_TOKEN = "http://123.155.153.87:8000/v1/token";
    //url to send answer to server
    public static final String HTTP_SEND_ANSWER_TO_SERVER = "http://123.155.153.87:8000/v1/answer";
    //url wheather can play
    public static final String HTTP_WHEATHER_CAN_PLAY = "http://123.155.153.87:8000/v1/canplay";
    //url to relive
    public static final String HTTP_RELIVE = "http://123.155.153.87:8000/v1/relive";

    //rong cloud

    public static final String UID = "295042";
    //public static final String UID = "24324242";

    public static final String APP_KEY = "mgb7ka1nmwzxg";


    public static final String APP_SECRET = "TKtTyIfVW8Wuva";
    //app_id
    //public static final String APP_ID = "80e54398fed94ae8a010acf782f569b7";
    //public static final String APP_SECRET = "hLcD4dtADpPs4";
    public static final String AGORA_APP_ID = #Your_APPID;


    //public static final String AGORA_APP_ID = "b3f09afc7af3496f872e805f03bf262c";
    // rongyun chatroom
    public static final String questionRoom = "10001";
    public static final String chatRoom = "chatroom";

    //agora live room
    public static final String agoraChannelName= "HQ_Demo";
    public static final int MESSAGE_SEND = 0;
    public static final int MESSAGE_SEND_ERROR = 1;
    public static final int MESSAGE_RECEIVE =2;
    public static final int MESSAGE_TOAST = 10;

}
