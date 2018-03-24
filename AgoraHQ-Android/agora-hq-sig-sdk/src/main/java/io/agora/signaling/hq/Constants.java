package io.agora.signaling.hq;

public class Constants {

    public static final String RY_HTTP_GET_3RD_ = "http://hq.agoraio.cn:8000/v1/vendor";
    public static final String RY_HTTP_GET_TOKEN_ = "http://hq.agoraio.cn:8000/v1/token";
    public static final String URL_POST_STATISTICS_ = "http://hq-stats.agoraio.cn:8000/v1/stats";

    public static final int MESSAGE_SEND_ERROR = 1;
    public static final int MESSAGE_RECEIVE = 2;
    public static final int TOKEN_INVALID = 4;
    public static final int TOKEN_EXPIRED = 5;
    public static final int LOGIN_FAILED = 6;
    public static final int ON_BLACK_LIST = 7;
    public static final int NO_CONNECTION = 8;
    public static final int CONNECTION_FAILED = 9;
    public static final int KICKED_BY_SERVER = 10;
}
