package io.agora.agoraandroidhq.control;

import org.json.JSONException;
import org.json.JSONObject;

import java.io.UnsupportedEncodingException;

import io.agora.agoraandroidhq.tools.GameControl;

/**
 * Created by zhangtao on 2018/2/24.
 */

public class SendMessageHelper {

    private static String tag = "[SendMessageHelper]  ";
    public static String sendMessageString(String message) throws JSONException {
        StringBuilder stringBuilder = new StringBuilder("{");
        stringBuilder.append("\\\"type\\\":").append("\\\"chat\\\"").append(",");
        stringBuilder.append("\\\"data\\\":").append("\\\"").append(message).append("\\\"").append(",");
        stringBuilder.append("\\\"name\\\":").append("\\\"").append(GameControl.currentUser.getUserName()).append("\\\"");
        stringBuilder.append("}");
        GameControl.logD(tag + "sendMessageString  =  " + stringBuilder.toString());
        try {
            return new String(stringBuilder.toString().getBytes("UTF-8"));
        } catch (UnsupportedEncodingException e) {
            e.printStackTrace();
            return null;
        }
    }

    public static String sendMessageSelf(String message) throws JSONException {
        GameControl.logD(tag + "sendMessageSelf  =  " + message.toString());
        JSONObject jsonObject = new JSONObject();
        jsonObject.put("type", "chat");
        jsonObject.put("data", message);
        jsonObject.put("name", GameControl.currentUser.getUserName());
        return jsonObject.toString();
    }
}
