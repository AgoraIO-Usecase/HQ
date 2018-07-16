package io.agora.livequiz.tools;

import org.json.JSONException;
import org.json.JSONObject;

/**
 * Created by zhangtao on 2018/2/24.
 */

public class DecryptHelpter {

    public static final String Decyrpt_Empty = "null";
    public static final String Decrypt_AES128 = "v1";
    private static String tag = "DecryptHelpter";

    public static String decryptData(String encryptType, String decryptKey, JSONObject jsonObject) {
        GameControl.logD(tag + "decryptData");
        String decryptData = null;
        GameControl.logD(tag + encryptType);
        switch (encryptType) {
            case Decyrpt_Empty:
                JSONObject dataObjects = null;
                try {
                    dataObjects = jsonObject.getJSONObject("data");
                } catch (JSONException e) {
                    e.printStackTrace();
                }
                decryptData = dataObjects.toString();
                GameControl.logD(tag + "encryptType is null");
                break;
            case Decrypt_AES128:
                try {
                   // decryptData = DecryptData.aes128Decrypt(decryptKey, data);
                    String dataObject = null;
                    try {
                        dataObject = jsonObject.getString("data");
                    } catch (JSONException e) {
                        e.printStackTrace();
                    }
                    decryptData = DecryptData.decryptAES(dataObject,decryptKey);
                } catch (Exception e) {
                    e.printStackTrace();
                }
                break;
            default:
                break;
        }

        return decryptData;
    }
}
