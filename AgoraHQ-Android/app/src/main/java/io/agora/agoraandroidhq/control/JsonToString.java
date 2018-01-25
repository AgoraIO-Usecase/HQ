package io.agora.agoraandroidhq.control;

import org.json.JSONArray;
import org.json.JSONException;
import org.json.JSONObject;

import java.io.UnsupportedEncodingException;
import java.util.ArrayList;

import io.agora.agoraandroidhq.module.Message;
import io.agora.agoraandroidhq.module.Question;
import io.agora.agoraandroidhq.module.Result;
import io.agora.agoraandroidhq.tools.GameControl;

/**
 * Created by zhangtao on 2018/1/15.
 */

public class JsonToString {


    public static Object returnObject = new Object();
    public static String strinType = "none";

    public synchronized static Object jsonToString(String jsonData) throws JSONException {
        JSONObject jsonObject = new JSONObject(jsonData);

        strinType = jsonObject.getString("type");

        switch (strinType) {

            case "chat":
                //TODO
                String data = jsonObject.getString("data");
                String name = jsonObject.getString("name");

                returnObject = new Message(name, data);
                break;


            case "result":
                //TODO
                JSONObject dataObject = jsonObject.getJSONObject("data");
                int correct = dataObject.getInt("correct");
                String total = dataObject.getString("total");
                String sid = dataObject.getString("sid");
                int result = dataObject.getInt("result");

                returnObject = new Result(correct, total, sid, result);
                break;

            case "quiz":
                JSONObject dataObjects = jsonObject.getJSONObject("data");
                int id = dataObjects.getInt("id");
                String question = dataObjects.getString("question");
                String type = dataObjects.getString("type");
                JSONArray array = dataObjects.getJSONArray("options");

                ArrayList list = new ArrayList();
                for (int i = 0; i < array.length(); i++) {
                    list.add(array.get(i));
                }

                returnObject = new Question(id, question, type, list);

                break;

        }


        return returnObject;
    }


    public static String sendMessageString(String message) throws JSONException {

       /* JSONObject jsonObject = new JSONObject();
        jsonObject.put("\"type\"", "\"chat\"");
        jsonObject.put("\"data\"", "\""+message+"\"");
        jsonObject.put("\"name\"", "\""+GameControl.currentUser.getName()+"\"");*/

        StringBuilder stringBuilder = new StringBuilder("{");
        stringBuilder.append("\\\"type\\\":").append("\\\"chat\\\"").append(",");
        stringBuilder.append("\\\"data\\\":").append("\\\"").append(message).append("\\\"").append(",");
        stringBuilder.append("\\\"name\\\":").append("\\\"").append(GameControl.currentUser.getName()).append("\\\"");
        stringBuilder.append("}");
        GameControl.logD("sendString  =  "+stringBuilder.toString());

        try {
            return new String(stringBuilder.toString().getBytes("UTF-8"));
        } catch (UnsupportedEncodingException e) {
            e.printStackTrace();
            return null;
        }
       // return stringBuilder.toString();
    }

    public static String sendMessageSelf(String message) throws JSONException {

        JSONObject jsonObject = new JSONObject();
        jsonObject.put("type", "chat");
        jsonObject.put("data", message);
        jsonObject.put("name", GameControl.currentUser.getName());

       /* StringBuilder stringBuilder = new StringBuilder("{");
        stringBuilder.append("\\\"type\\\":").append("\\\"chat\\\"").append(",");
        stringBuilder.append("\\\"data\\\":").append("\\\"").append(message).append("\\\"").append(",");
        stringBuilder.append("\\\"name\\\":").append("\\\"").append(GameControl.currentUser.getName()).append("\\\"");
        stringBuilder.append("}");
        GameControl.logD("sendString  =  "+stringBuilder.toString());
*/
        /*try {
            return new String(stringBuilder.toString().getBytes("GBK"));
        } catch (UnsupportedEncodingException e) {
            e.printStackTrace();
            return null;
        }*/

        return  jsonObject.toString();
    }

}
