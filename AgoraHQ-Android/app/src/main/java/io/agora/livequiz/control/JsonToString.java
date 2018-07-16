package io.agora.livequiz.control;

import org.json.JSONArray;
import org.json.JSONException;
import org.json.JSONObject;

import java.util.ArrayList;

import io.agora.livequiz.module.Message;
import io.agora.livequiz.module.Question;
import io.agora.livequiz.module.Result;
import io.agora.livequiz.tools.DecryptData;
import io.agora.livequiz.tools.DecryptHelpter;
import io.agora.livequiz.tools.GameControl;

/**
 * Created by zhangtao on 2018/1/15.
 */

public class JsonToString {


    public static Object returnObject = new Object();
    public static String strinType = "none";
    private  static String tag = "[JsonToString]  ";
    public synchronized static Object jsonToString(String jsonData) throws JSONException {
        JSONObject jsonObject = new JSONObject(jsonData);
        GameControl.logD(tag+"jsonToString");
        strinType = jsonObject.getString("type");
        returnObject = jsonObject;
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
                GameControl.logD("quiz");
                String encrypt = jsonObject.getString("encrypt");
                GameControl.logD("quiz2222");
                //String encryptData = dataObjects.toString();
                String key = GameControl.currentUser.getChannelName();
                String decryptKey = DecryptData.getKey(key);
                String dataDecrypt = DecryptHelpter.decryptData(encrypt, decryptKey,jsonObject);
                GameControl.logD("quiz333333");
                JSONObject jsonDataDecrypt = null;
                if (dataDecrypt == null) {
                    return null;
                } else {
                    jsonDataDecrypt = new JSONObject(dataDecrypt);
                }

                int id = jsonDataDecrypt.getInt("id");
                String question = jsonDataDecrypt.getString("question");
                String type = jsonDataDecrypt.getString("type");
                int total_question = jsonDataDecrypt.getInt("total");
                int timeOut = jsonDataDecrypt.getInt("timeout");
                JSONArray array = jsonDataDecrypt.getJSONArray("options");
                GameControl.logD(tag+"jsonToString  =  id=" + id + " ");
                ArrayList list = new ArrayList();
                for (int i = 0; i < array.length(); i++) {
                    list.add(array.get(i));
                }
                returnObject = new Question(id, question, type, list, total_question, timeOut, encrypt);
                //  returnObject = new Question(id, question, type, list);
                break;
        }
        return returnObject;
    }
}
