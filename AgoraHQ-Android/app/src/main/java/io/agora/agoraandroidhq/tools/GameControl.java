package io.agora.agoraandroidhq.tools;

import android.Manifest;
import android.content.Context;
import android.content.pm.PackageManager;
import android.graphics.drawable.Drawable;
import android.os.Environment;
import android.support.v4.app.ActivityCompat;
import android.support.v4.content.ContextCompat;
import android.util.Log;

import java.io.BufferedWriter;
import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.OutputStreamWriter;
import java.lang.ref.WeakReference;
import java.text.SimpleDateFormat;
import java.util.Date;
import java.util.Locale;

import io.agora.agoraandroidhq.module.Question;
import io.agora.agoraandroidhq.module.User;
import io.agora.rtc.RtcEngine;

/**
 * Created by zhangtao on 2018/1/15.
 */

public class GameControl {

    public static boolean clientWheatherCanPlay = true;
    public static boolean serverWheatherCanPlay = true;
    public static Question currentQuestion;
    public static final boolean SHOW_LOG = true;
    public static boolean controlCheckThread = true;
    public static User currentUser;
    public static int MESSAGE_ID = 2;
    public static Drawable currentUserHeadImage;
    public static String currentUserName;
    public static String signalAccount;
    public static int timeOut = 10;
    public static int total = 10;
    public static int result = -1;
    public static Context context;
    public static void setCurrentQuestion(Question question) {
        currentQuestion = question;
    }
    public static String logFileName = "HQ_Android_Log.txt";

    public static void logD(String message) {
        if (SHOW_LOG) {
            try {
                if (context != null) {
                    Log.d("agora_signal",message);
                    writeLogToFile(context, message);
                }
            } catch (IOException e) {
                e.printStackTrace();
            }
        }
    }

    public static void writeLogToFile(Context context, String msg) throws IOException {
        if (ContextCompat.checkSelfPermission(context, Manifest.permission.WRITE_EXTERNAL_STORAGE) != PackageManager.PERMISSION_GRANTED) {
            return;
        }
        SimpleDateFormat dateFormat = new SimpleDateFormat("yyyy-MM-dd HH:mm:ss ", Locale.CHINA);
        Date date = new Date();

      //  Log.d("GameControl : ", "WritePermission  =  " + ContextCompat.checkSelfPermission(context, Manifest.permission.WRITE_EXTERNAL_STORAGE));
        String log = dateFormat.format(date) + " " + msg + "\n";
        String filePath = getFilePath(context);
        File file = new File(filePath, logFileName);
        if (!file.exists()) {
            file.createNewFile();
            Log.d("GameControl :", "createNewFile ");
            Log.d("GameControl :", file.getAbsolutePath());
        }
        FileOutputStream fos = null;
        BufferedWriter bw = null;
        try {

            fos = new FileOutputStream(file, true);
            bw = new BufferedWriter(new OutputStreamWriter(fos));
            bw.write(log);

        } catch (FileNotFoundException e) {
            e.printStackTrace();
        } catch (IOException e) {
            e.printStackTrace();
        } finally {
            try {
                if (bw != null) {
                    bw.close();
                    fos.close();
                }
            } catch (IOException e) {
                e.printStackTrace();
            }
        }
    }

    private static String getFilePath(Context context) {
        if (Environment.getExternalStorageState().equals(Environment.MEDIA_MOUNTED)) {
            return Environment.getExternalStorageDirectory().getAbsolutePath();
        } else {
            return context.getFilesDir().getAbsolutePath();
        }
    }

    public static void deleteLogFile() {
        String filePath = getFilePath(context);
        File file = new File(filePath, logFileName);
        if (file.exists()) {
            Log.d("GameControl:  ", "deleteLogFIle");
            file.delete();
        }
    }
}
