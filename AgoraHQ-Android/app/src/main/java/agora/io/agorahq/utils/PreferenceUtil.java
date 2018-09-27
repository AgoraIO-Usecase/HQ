package agora.io.agorahq.utils;

import android.annotation.SuppressLint;
import android.content.Context;
import android.content.SharedPreferences;
import android.graphics.Bitmap;
import android.graphics.drawable.Drawable;
import android.util.Base64;

import java.io.ByteArrayInputStream;
import java.io.ByteArrayOutputStream;
import java.io.IOException;
import java.util.Set;

public class PreferenceUtil {
    private static PreferenceUtil instance;

    private SharedPreferences mSharedPreferences;

    private SharedPreferences.Editor editor;

    @SuppressLint("CommitPrefEdits")
    private PreferenceUtil(Context mContext, String preferenceName) {
        mSharedPreferences = mContext.getSharedPreferences(preferenceName, Context.MODE_PRIVATE);
        editor = mSharedPreferences.edit();
    }

    public static PreferenceUtil getInstance(Context context, String preferenceName) {
        if (instance == null) {
            synchronized (PreferenceUtil.class) {
                if (instance == null) {
                    instance = new PreferenceUtil(context, preferenceName);
                }
            }
        }
        return instance;
    }

    public static PreferenceUtil getInstance(Context context) {
        return getInstance(context, context.getPackageName() + "_preferences");
    }

    /**
     * 得到SharedPreferences.Editor，如果之后的操作强烈依赖于存入的数据
     * 那么请用editor的commit方法进行提交。
     */
    public SharedPreferences.Editor getEditor() {
        return editor;
    }


    /**
     * put / get String
     *
     * @param key
     * @param value
     */
    public void putStringByApply(String key, String value) {
        editor.putString(key, value).apply();
    }

    public void putStringByCommit(String key, String value) {
        editor.putString(key, value).commit();
    }

    public String getString(String key, String defValue) {
        return mSharedPreferences.getString(key, defValue);
    }


    /**
     * put / get int
     *
     * @param key
     * @param value
     */
    public void putIntByApply(String key, int value) {
        editor.putInt(key, value).apply();
    }

    public void putIntByCommit(String key, int value) {
        editor.putInt(key, value).commit();
    }

    public int getInt(String key, int defValue) {
        return mSharedPreferences.getInt(key, defValue);
    }


    /**
     * put / get boolean
     *
     * @param key
     * @param value
     */
    public void putBooleanByApply(String key, boolean value) {
        editor.putBoolean(key, value).apply();
    }

    public void putBooleanByCommit(String key, boolean value) {
        editor.putBoolean(key, value).commit();
    }

    public boolean getBoolean(String key, boolean defValue) {
        return mSharedPreferences.getBoolean(key, defValue);
    }


    /**
     * put / get long
     *
     * @param key
     * @param value
     */
    public void putLongByApply(String key, long value) {
        editor.putLong(key, value).apply();
    }

    public void putLongByCommit(String key, long value) {
        editor.putLong(key, value).commit();
    }

    public long getLong(String key, long defValue) {
        return mSharedPreferences.getLong(key, defValue);
    }

    /**
     * drawable
     */
    public void putDrawableByApply(String key, Bitmap bitmap) {
        if (bitmap == null)
            return;

        ByteArrayOutputStream bos = new ByteArrayOutputStream();
        bitmap.compress(Bitmap.CompressFormat.PNG, 50, bos);
        String imageBase64 = Base64.encodeToString(bos.toByteArray(), Base64.DEFAULT);
        editor.putString(key, imageBase64).apply();
        try {
            bos.close();
        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    public void putDrawableByCommit(String key, Bitmap bitmap) {
        ByteArrayOutputStream bos = new ByteArrayOutputStream();
        bitmap.compress(Bitmap.CompressFormat.PNG, 50, bos);
        String imageBase64 = Base64.encodeToString(bos.toByteArray(), Base64.DEFAULT);
        editor.putString(key, imageBase64).commit();
        try {
            bos.close();
        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    public Drawable getDrawable(String key, Bitmap bitmap) {
        if (bitmap == null)
            return null;

        ByteArrayOutputStream bos = new ByteArrayOutputStream();
        bitmap.compress(Bitmap.CompressFormat.PNG, 50, bos);
        String imageBase64 = Base64.encodeToString(bos.toByteArray(), Base64.DEFAULT);

        String bitmapStr = mSharedPreferences.getString(key, imageBase64);
        if (bitmapStr == null)
            return null;

        ByteArrayInputStream ios = new ByteArrayInputStream(
                Base64.decode(bitmapStr.getBytes(), Base64.DEFAULT));
        Drawable drawable = Drawable.createFromStream(ios, "");
        try {
            bos.close();
            ios.close();
        } catch (IOException e) {
            e.printStackTrace();
        }

        return drawable;
    }

    /**
     * put / get Set<String>
     *
     * @param key
     * @param values
     */
    public void putStringSetByApply(String key, Set<String> values) {
        editor.putStringSet(key, values).apply();
    }

    public void putStringSetByCommit(String key, Set<String> values) {
        editor.putStringSet(key, values).commit();
    }

    public Set<String> getStringSet(String key, Set<String> defValue) {
        return mSharedPreferences.getStringSet(key, defValue);
    }

    /**
     * remove
     *
     * @param key
     */
    public void removeByApply(String key) {
        editor.remove(key).apply();
    }

    public void removeByCommit(String key) {
        editor.remove(key).commit();
    }


    /***
     * clear
     */
    public void clearByApply() {
        editor.clear().apply();
    }

    public void clearByCommit() {
        editor.clear().commit();
    }

}
