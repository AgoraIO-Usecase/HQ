package agora.io.agorahq.utils;

import android.app.Activity;
import android.content.Context;
import android.text.TextUtils;

import org.json.JSONArray;
import org.json.JSONException;
import org.json.JSONObject;

import java.io.UnsupportedEncodingException;
import java.util.ArrayList;

import agora.io.agorahq.GlobalApplication;
import agora.io.agorahq.bean.QuestionAnswerResult;
import agora.io.agorahq.bean.ChatMessage;
import agora.io.agorahq.bean.Question;
import agora.io.agorahq.bean.Reset;

public class HQJson2StrUtil {
    public synchronized static Object json2Str(Context ctx, String jsonStr) throws JSONException {
        JSONObject jsonObj = new JSONObject(jsonStr);

        Object resObj = jsonObj;
        String strType = jsonObj.optString(Constants.JSON_KEY_TYPE);
        if (strType == null)
            return null;

        switch (strType) {
            case Constants.JSON_KEY_CHAT:
                String msgContent = jsonObj.optString(Constants.JSON_KEY_CHAT_CONTENT);
                String name = jsonObj.optString(Constants.JSON_KEY_CHAT_NAME);

                resObj = new ChatMessage(name, msgContent);
                break;
            case Constants.JSON_KEY_RESULT:
                JSONObject dataObj = jsonObj.optJSONObject(Constants.JSON_KEY_RESULT_DATA);
                if (null != dataObj) {
                    int corr = dataObj.optInt(Constants.JSON_KEY_RESULT_CORRECT);
                    String total = dataObj.optString(Constants.JSON_KEY_RESULT_TOTAL);
                    String sid = dataObj.optString(Constants.JSON_KEY_RESULT_SID);
                    int result = dataObj.optInt(Constants.JSON_KEY_RESULT_RESULT);

                    resObj = new QuestionAnswerResult(total, sid, result);
                } else {
                    resObj = null;
                }
                break;
            case Constants.JSON_KEY_QUIZ:
                String encrypt = jsonObj.optString(Constants.JSON_KEY_QUIZ_ENCRYPT);

                String nameKey = ((GlobalApplication) (((Activity) ctx).getApplication())).getGlobalGameInfo().getUser().getChannelName();
                String decryptKey = DecryptUtil.getKey(nameKey);
                String dataDecrypt = DecryptUtil.decryptData(encrypt, decryptKey, jsonObj);

                if (dataDecrypt == null)
                    return null;

                JSONObject jsonDataDecrypt = new JSONObject(dataDecrypt);
                int sid = jsonDataDecrypt.optInt(Constants.JSON_KEY_QUIZ_ID);
                String question = jsonDataDecrypt.optString(Constants.JSON_KEY_QUIZ_QUESTION);
                String type = jsonDataDecrypt.optString(Constants.JSON_KEY_QUIZ_TYPE);

                int totalQ = jsonDataDecrypt.optInt(Constants.JSON_KEY_QUIZ_TOTAL);
                int timeout = jsonDataDecrypt.optInt(Constants.JSON_KEY_QUIZ_TIMEOUT);

                JSONArray optionsArray = jsonDataDecrypt.optJSONArray(Constants.JSON_KEY_QUIZ_OPTIONS);
                ArrayList<String> options = new ArrayList<>();
                for (int i = 0; i < optionsArray.length(); i++) {
                    options.add(optionsArray.get(i).toString());
                }

                resObj = new Question(question, type, options, totalQ, timeout, encrypt, sid);
                break;
            case Constants.JSON_KEY_RESET:

                resObj = new Reset();
                break;
        }

        return resObj;
    }

    public static Object resolvePlayStatus(String data) {
        String errMessage = "";
        boolean canPlay = true;
        try {
            JSONObject object = new JSONObject(data);

            if (object.length() == 0) {
                return canPlay;
            } else {
                errMessage = object.optString(Constants.JSON_KEY_PLAY_STATUS_RESULT_ERROR, "");
            }
        } catch (JSONException e) {
        }
        return errMessage;
    }

    public static int resolveSEI(String data) {
        int sid = 0;
        try {
            JSONObject object = new JSONObject(data);
            JSONObject d = object.optJSONObject(Constants.JSON_KEY_SEI_DATA);
            sid = d.optInt(Constants.JSON_KEY_SEI_DATA_SID, 0);
        } catch (JSONException e) {
            e.printStackTrace();
        }

        return sid;
    }

    public static String sendMessageString(String message, String userName) {
        StringBuilder stringBuilder = new StringBuilder("{");
        stringBuilder.append("\\\"type\\\":").append("\\\"chat\\\"").append(",");
        stringBuilder.append("\\\"data\\\":").append("\\\"").append(message).append("\\\"").append(",");
        stringBuilder.append("\\\"name\\\":").append("\\\"").append(userName).append("\\\"");
        stringBuilder.append("}");
        try {
            return new String(stringBuilder.toString().getBytes("UTF-8"));
        } catch (UnsupportedEncodingException e) {
            e.printStackTrace();
            return null;
        }
    }
}
