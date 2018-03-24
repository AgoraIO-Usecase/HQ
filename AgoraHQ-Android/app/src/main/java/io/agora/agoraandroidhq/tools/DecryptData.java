package io.agora.agoraandroidhq.tools;

import android.os.Build;
import android.support.annotation.RequiresApi;
import android.util.Base64;

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

/**
 * Created by zhangtao on 2018/2/24.
 */

public class DecryptData {


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

        GameControl.logD("decryptAES  content =  " + content + "  key = " + key);
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
        GameControl.logD("decryptAES data  = " + string);
        return string;
    }

    public static String getKey(String channelName) {
        GameControl.logD("channelName Size = " + channelName.length());
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

}
