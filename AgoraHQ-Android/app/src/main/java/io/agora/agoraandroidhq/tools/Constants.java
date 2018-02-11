package io.agora.agoraandroidhq.tools;

import io.agora.agoraandroidhq.R;

/**
 * Created by zhangtao on 2018/1/12.
 */

public class Constants {


    public static final int broadCastUid = 1;

    // url to get toke
    //public static  final String HTTP_GET_TOKEN ="http://10.60.2.75:8000/v1/token";
    public static  final String HTTP_GET_TOKEN = "http://123.155.153.87:8000/v1/token";
    //url to send answer to server

    //url wheather can play
    //public static final String HTTP_WHEATHER_CAN_PLAY = "http://123.155.153.87:9000/v1/canplay";
    //url to relive
   /* public static final String HTTP_RELIVE = "http://123.155.153.87:9000/v1/relive";
    public static final String HTTP_SEND_ANSWER_TO_SERVER = "http://123.155.153.87:9000/v1/answer";
*/

    public static  String HTTP_RELIVE;
    public static  String HTTP_SEND_ANSWER_TO_SERVER;
    public static  String HTTP_CHECK_WHEATHER_CAN_PLAY;
    public static  String HTTP_REPLY_WHEATHER_ACCEPT_VIDEO;
    //rong cloud

    public static final String UID = "2942";
    //public static final String UID = "24324242";

    //public static final String APP_KEY = "mgb7ka1nmwzxg";
	//public static final String APP_KEY = "p5tvi9dsphuz4";

    public static final String APP_SECRET = "TKtTyIfVW8Wuva";
    // app_id

    //rongyun chatroom
    public static final String questionRoom = "10001";
    public static final String chatRoom = "chatroom";

    //agora live room
    public static final String agoraChannelName= "HQ_Demo";
    public static final int MESSAGE_SEND = 0;
    public static final int MESSAGE_SEND_ERROR = 1;
    public static final int MESSAGE_RECEIVE =2;
    public static final int MESSAGE_TOAST = 10;


    public static final String AGORA_APP_ID = #YOUR_APPID;
    public static final int LOGIN_AGORA_SIGNAL_SUCCESS = 100;
    public static final int LOGIN_AGORA_SIGNAL_FAIL = 101;
    public static final int AGORA_SIGNAL_RECEIVE = 102;
    public static final int AGORA_SIGNAL_SEND = 103;


}
