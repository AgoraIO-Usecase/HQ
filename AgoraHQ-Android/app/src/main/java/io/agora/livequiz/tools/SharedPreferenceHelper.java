package io.agora.livequiz.tools;

import android.annotation.SuppressLint;
import android.content.Context;
import android.content.SharedPreferences;
import android.graphics.Bitmap;
import android.graphics.drawable.Drawable;
import android.util.Base64;

import java.io.ByteArrayInputStream;
import java.io.ByteArrayOutputStream;
import java.io.IOException;
import java.util.Random;

/**
 * Created by zhangtao on 2018/1/30.
 */

public class SharedPreferenceHelper {

    public static SharedPreferenceHelper sharedPreferenceHelper;
    public Context applicationContext;
    private static String tag = "[SharedPreferenceHelper]  ";

    private SharedPreferenceHelper(Context context) {
        applicationContext = context;
    }

    public static SharedPreferenceHelper newInstance(Context context) {

        if (sharedPreferenceHelper == null) {
            sharedPreferenceHelper = new SharedPreferenceHelper(context);
            return sharedPreferenceHelper;
        } else {
            return sharedPreferenceHelper;
        }
    }

    private SharedPreferences getSharedPreferences() {
        @SuppressLint("WrongConstant")
        SharedPreferences sharedPreferences = applicationContext.getSharedPreferences("agora_hq", Context.MODE_APPEND);
        return sharedPreferences;
    }

    public void saveName(String userName) {
        SharedPreferences.Editor editor = sharedPreferenceHelper.getSharedPreferences().edit();
        editor.putString("userName", userName);
        boolean commit = editor.commit();
    }

    public String getName() {
        String userName = getSharedPreferences().getString("userName", getRandomString(7));
        GameControl.logD(tag + "getName = " + userName);
        return userName;
    }

    private String getRandomString(int length) {
        String str = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
        Random random = new Random();
        StringBuffer sb = new StringBuffer();
        for (int i = 0; i < length; ++i) {
            int number = random.nextInt(52);// [0,51)
            sb.append(str.charAt(number));
        }
        return sb.toString();
    }

    public void saveDrawable(Bitmap bitmap) {
        SharedPreferences.Editor editor = sharedPreferenceHelper.getSharedPreferences().edit();
        Bitmap bit = bitmap;
        ByteArrayOutputStream baos = new ByteArrayOutputStream();
        bitmap.compress(Bitmap.CompressFormat.PNG, 50, baos);
        String imageBase64 = new String(Base64.encodeToString(baos.toByteArray(), Base64.DEFAULT));
        editor.putString("userImage", imageBase64);
        GameControl.logD(tag + "saveDrawable  = ");
        editor.commit();
    }

    public Drawable getDrawable() {
        SharedPreferences.Editor editor = sharedPreferenceHelper.getSharedPreferences().edit();
        String temp = getSharedPreferences().getString("userImage", "1");
        if (temp.equals("1")) {
            return null;
        } else {
            ByteArrayInputStream bais = new ByteArrayInputStream(Base64.decode(temp.getBytes(), Base64.DEFAULT));
            Drawable drawable = Drawable.createFromStream(bais, "");
            try {
                bais.close();
            } catch (IOException e) {
                e.printStackTrace();
            }
            GameControl.logD(tag + "getDrawableForSharedPreference = " + bais.toString());
            return drawable;
        }
    }

}
