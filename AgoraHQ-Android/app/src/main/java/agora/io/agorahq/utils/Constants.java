package agora.io.agorahq.utils;

import java.util.Locale;

public class Constants {
    public final static String TAG_PR = "Agora_WBS:\t";

    public static final int BASE_VALUE_PERMISSION = 0X0001;
    public static final int PERMISSION_REQ_ID_RECORD_AUDIO = BASE_VALUE_PERMISSION + 1;
    public static final int PERMISSION_REQ_ID_CAMERA = BASE_VALUE_PERMISSION + 2;
    public static final int PERMISSION_REQ_ID_WRITE_EXTERNAL_STORAGE = BASE_VALUE_PERMISSION + 3;

    public final static String PREFERENCE_CHANNEL_NAME_KEY = "preference_channel_name_key";
    public final static String PREFERENCE_PERSON_NAME_KEY = "preference_person_name_key";
    public final static String PREFERENCE_PERSON_IMAGE_KEY = "preference_image_name_key";
    public final static String PREFERENCE_RELIVE_COUNT_KEY = "preference_relive_count_key";

    public final static int DEFAULT_BROADCASTER_UID = 1;

    /**
     * Agora app key
     */
    public final static String AGORA_MEDIA_APPID = #YOUR_APPID;
    public final static String AGORA_SIGNALING_APPID = #YOUR_APPID;

    /**
     * for work thread
     */
    public final static int ACTION_WORKER_THREAD_QUIT = 0x1001;
    public final static int ACTION_WORKER_JOIN_CHANNEL = 0x1002;
    public final static int ACTION_WORKER_LEAVE_CHANNEL = 0x1003;
    public final static int ACTION_WORKER_DESTROY_ENGINE = 0x1004;


    /** server comm */
    public final static int HTTP_MESSAGE_SEND_ERROR = 0x2001;
    public final static int HTTP_MESSAGE_RECEIVE =0x2002;
    public final static int HTTP_MESSAGE_TOAST = 0x2010;

    public final static String HTTP_URL_RELIVE;
    public final static String HTTP_URL_ANSWER;
    public final static String HTTP_URL_LOCAL_JOIN;
//    public final static String HTTP_URL_CAN_PLAY;
    public final static String HTTP_URL_INVITE_RESPONSE;
    static {
        if (Locale.getDefault().getLanguage().equals(new Locale("zh").getLanguage())) {
            HTTP_URL_RELIVE = "http://123.155.153.87:10000/v1/relive";
            HTTP_URL_ANSWER = "http://123.155.153.87:10000/v1/answer";
            HTTP_URL_LOCAL_JOIN = "http://123.155.153.87:10000/v1/join";
//            HTTP_URL_CAN_PLAY = "http://123.155.153.87:10000/v1/canplay?gid=";
            HTTP_URL_INVITE_RESPONSE = "http://123.155.153.87:10000/v1/inviteResponse";
        } else {
            HTTP_URL_RELIVE = "http://76.231.168.66:10000/v1/relive";
            HTTP_URL_ANSWER = "http://76.231.168.66:10000/v1/answer";
            HTTP_URL_LOCAL_JOIN = "http://76.231.168.66:10000/v1/join";
//            HTTP_URL_CAN_PLAY = "http://76.231.168.66:10000/v1/canplay?gid=";
            HTTP_URL_INVITE_RESPONSE = "http://76.231.168.66:10000/v1/inviteResponse";
        }
    }

    /** signaling constants */
    public final static String AGORA_SIGNALING_CERF = null;

    public final static int SIGNALING_LOGIN = 0x3001;
    public final static int SIGNALING_SEND_CHANNEL_MESSAGE = 0x3002;
    public final static int SIGNALING_LOGOUT = 0x3003;


    /** for json key */
    public final static String JSON_KEY_TYPE = "type";

    public final static String JSON_KEY_CHAT = "chat";
    public final static String JSON_KEY_CHAT_NAME = "name";
    public final static String JSON_KEY_CHAT_CONTENT = "data";

    public final static String JSON_KEY_RESULT = "result";
    public final static String JSON_KEY_RESULT_DATA = "data";
    public final static String JSON_KEY_RESULT_CORRECT = "correct";//0, 1, 2, 3
    public final static String JSON_KEY_RESULT_TOTAL = "total";
    public final static String JSON_KEY_RESULT_SID = "sid";
    public final static String JSON_KEY_RESULT_RESULT = "result";

    public final static String JSON_KEY_QUIZ = "quiz";
    public final static String JSON_KEY_QUIZ_ENCRYPT = "encrypt";
    public final static String JSON_KEY_QUIZ_ID = "id";
    public final static String JSON_KEY_QUIZ_QUESTION = "question";
    public final static String JSON_KEY_QUIZ_TYPE = "type";
    public final static String JSON_KEY_QUIZ_TOTAL = "total";
    public final static String JSON_KEY_QUIZ_TIMEOUT = "timeout";
    public final static String JSON_KEY_QUIZ_OPTIONS = "options";
    public final static String JSON_KEY_RESET = "reset";

    public final static String JSON_KEY_PLAY_STATUS_RESULT_ERROR = "err";

    public final static String JSON_KEY_SEI_DATA = "data";
    public final static String JSON_KEY_SEI_DATA_SID = "questionId";
}
