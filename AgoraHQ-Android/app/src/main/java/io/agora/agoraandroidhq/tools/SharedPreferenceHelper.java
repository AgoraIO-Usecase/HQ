package io.agora.agoraandroidhq.tools;

import android.annotation.SuppressLint;
import android.content.Context;
import android.content.SharedPreferences;
import android.graphics.Bitmap;
import android.graphics.drawable.Drawable;
import android.util.Base64;

import java.io.ByteArrayInputStream;
import java.io.ByteArrayOutputStream;

/**
 * Created by zhangtao on 2018/1/30.
 */

public class SharedPreferenceHelper {

    public static SharedPreferenceHelper sharedPreferenceHelper;
    public Context applicationContext;


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


    public String getName(){

        String userName = getSharedPreferences().getString("userName","Label");
        GameControl.logD("getName = "+userName);

        return userName;
    }



    public  void saveDrawable(Bitmap bitmap) {
        SharedPreferences.Editor editor = sharedPreferenceHelper.getSharedPreferences().edit();
        Bitmap bit = bitmap;
        ByteArrayOutputStream baos = new ByteArrayOutputStream();
        bitmap.compress(Bitmap.CompressFormat.PNG, 50, baos);
        String imageBase64 = new String(Base64.encodeToString(baos.toByteArray(),Base64.DEFAULT));
        editor.putString("userImage",imageBase64 );
        GameControl.logD("saveDrawable  = "+ baos.toString());
        editor.commit();
    }


    public Drawable getDrawable() {


        SharedPreferences.Editor editor= sharedPreferenceHelper.getSharedPreferences().edit();
        String temp = getSharedPreferences().getString("userImage","1");

        if(temp.equals("1")){
            return null;
        }else{
            ByteArrayInputStream bais = new ByteArrayInputStream(Base64.decode(temp.getBytes(), Base64.DEFAULT));

            GameControl.logD("getDrawableForSharedPreference = "+ bais.toString());
            return Drawable.createFromStream(bais, "");
        }
    }

}
