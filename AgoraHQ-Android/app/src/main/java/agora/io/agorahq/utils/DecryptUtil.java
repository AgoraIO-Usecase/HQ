package agora.io.agorahq.utils;

import android.util.Base64;

import org.json.JSONException;
import org.json.JSONObject;

import java.io.UnsupportedEncodingException;
import java.security.InvalidAlgorithmParameterException;
import java.security.InvalidKeyException;
import java.security.NoSuchAlgorithmException;

import javax.crypto.BadPaddingException;
import javax.crypto.Cipher;
import javax.crypto.IllegalBlockSizeException;
import javax.crypto.NoSuchPaddingException;
import javax.crypto.spec.IvParameterSpec;
import javax.crypto.spec.SecretKeySpec;

public class DecryptUtil {

    public static final String Decyrpt_Empty = "null";
    public static final String Decrypt_AES128 = "v1";
    private static String tag = "DecryptUtil";

    private static final String IV_STRING = "00000000000agora";

    public static String aes128Decrypt(String sSrc, String sKey) throws Exception {
        try {
            // 判断Key是否正确
            if (sKey == null | sSrc == null) {
                return null;
            }
            // byte[] raw = sKey.getBytes("utf-8");
            byte[] raw = Base64.decode(sKey, Base64.DEFAULT);
            SecretKeySpec skeySpec = new SecretKeySpec(raw, "AES");
            Cipher cipher = Cipher.getInstance("AES/CBC/PKCS5Padding");
            cipher.init(Cipher.DECRYPT_MODE, skeySpec);
            byte[] encrypted = Base64.decode(sSrc, Base64.DEFAULT);

            try {
                byte[] original = cipher.doFinal(encrypted);
                String originalString = new String(original, "utf-8");
                return originalString;
            } catch (Exception e) {
                System.out.println(e.toString());
                return null;
            }
        } catch (Exception ex) {
            System.out.println(ex.toString());
            return null;
        }
    }

    public static String decryptAES(String content, String key) {
        // base64 解码
        byte[] encryptedBytes = Base64.decode(content, Base64.DEFAULT);

        byte[] enCodeFormat = key.getBytes();
        SecretKeySpec secretKey = new SecretKeySpec(enCodeFormat, "AES");

        byte[] initParam = IV_STRING.getBytes();
        IvParameterSpec ivParameterSpec = new IvParameterSpec(initParam);

        Cipher cipher = null;
        try {
            cipher = Cipher.getInstance("AES/CBC/PKCS5Padding");
        } catch (NoSuchAlgorithmException e) {
            e.printStackTrace();
        } catch (NoSuchPaddingException e) {
            e.printStackTrace();
        }
        try {
            cipher.init(Cipher.DECRYPT_MODE, secretKey, ivParameterSpec);
        } catch (InvalidKeyException e) {
            e.printStackTrace();
        } catch (InvalidAlgorithmParameterException e) {
            e.printStackTrace();
        }

        byte[] result = new byte[0];
        try {
            result = cipher.doFinal(encryptedBytes);
        } catch (IllegalBlockSizeException e) {
            e.printStackTrace();
        } catch (BadPaddingException e) {
            e.printStackTrace();
        }
        String string = null;
        try {
            string = new String(result, "UTF-8");
        } catch (UnsupportedEncodingException e) {
            e.printStackTrace();
        }
        return string;
    }

    public static String getKey(String channelName) {
        String key = null;
        int size = channelName.length();
        if (size >= 16) {
            int reduceSize = size - 16;
            key = channelName.substring(reduceSize);
        } else {
            int addSize = 16 - size;
            StringBuilder builder = new StringBuilder();
            for (int i = 0; i < addSize; i++) {
                builder.append(0);
            }
            key = builder.toString() + channelName;
        }
        return key;
    }

    public static String decryptData(String encryptType, String decryptKey, JSONObject jsonObject) {
        String decryptData = null;
        switch (encryptType) {
            case Decyrpt_Empty:
                JSONObject dataObjects = null;
                try {
                    dataObjects = jsonObject.getJSONObject("data");
                } catch (JSONException e) {
                    e.printStackTrace();
                }
                decryptData = dataObjects.toString();
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
                    decryptData = decryptAES(dataObject,decryptKey);
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
