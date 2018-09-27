package agora.io.agorahq.activity;

import android.Manifest;
import android.app.Activity;
import android.content.pm.ActivityInfo;
import android.content.pm.PackageManager;
import android.os.Bundle;
import android.support.annotation.NonNull;
import android.support.annotation.Nullable;
import android.support.v4.app.ActivityCompat;
import android.support.v4.content.ContextCompat;
import android.view.Window;
import android.view.WindowManager;
import android.widget.Toast;

import agora.io.agorahq.GlobalApplication;
import agora.io.agorahq.bean.GameInfo;
import agora.io.agorahq.logicworker.MediaHandlerImpl;
import agora.io.agorahq.logicworker.SignalHandlerImpl;
import agora.io.agorahq.logicworker.WorkThread;
import agora.io.agorahq.utils.Constants;
import io.agora.rtc.RtcEngine;

public abstract class BaseActivity extends Activity{

    @Override
    protected void onCreate(@Nullable Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        this.requestWindowFeature(Window.FEATURE_NO_TITLE);
        this.getWindow().setFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN, WindowManager.LayoutParams.FLAG_FULLSCREEN);
        setRequestedOrientation(ActivityInfo.SCREEN_ORIENTATION_PORTRAIT);
        checkSelfPermissions();
    }

    protected abstract void initUIandEvent();
    protected abstract void deInitUIandEvent();

    @Override
    protected void onPostCreate(@Nullable Bundle savedInstanceState) {
        super.onPostCreate(savedInstanceState);
        initUIandEvent();
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        deInitUIandEvent();
    }

    protected WorkThread worker() {
        return ((GlobalApplication) getApplication()).getWorkThread();
    }

    protected RtcEngine rtcEngine() {
        return ((GlobalApplication) getApplication()).getWorkThread().getRtcEngine();
    }

    protected MediaHandlerImpl mediaHanlder() {
        return ((GlobalApplication) getApplication()).getWorkThread().mediaHandler();
    }

    protected GameInfo gameInfo() {
        return ((GlobalApplication) getApplication()).getGlobalGameInfo();
    }

    protected SignalHandlerImpl signalHandler() {
        return ((GlobalApplication) getApplication()).getWorkThread().signalingHandler();
    }

    public final void showShortToast(final String msg) {
        this.runOnUiThread(new Runnable() {
            @Override
            public void run() {
                Toast.makeText(getApplicationContext(), msg, Toast.LENGTH_SHORT).show();
            }
        });
    }

    protected final boolean checkSelfPermissions() {
        return checkSelfPermission(Manifest.permission.WRITE_EXTERNAL_STORAGE, Constants.PERMISSION_REQ_ID_WRITE_EXTERNAL_STORAGE) &&
                checkSelfPermission(Manifest.permission.RECORD_AUDIO, Constants.PERMISSION_REQ_ID_RECORD_AUDIO) &&
                checkSelfPermission(Manifest.permission.CAMERA, Constants.PERMISSION_REQ_ID_CAMERA);
    }

    public boolean checkSelfPermission(String permission, int requestCode) {
        if (ContextCompat.checkSelfPermission(this,
                permission)
                != PackageManager.PERMISSION_GRANTED) {

            ActivityCompat.requestPermissions(this,
                    new String[]{permission},
                    requestCode);
            return false;
        }
        return true;
    }

    @Override
    public void onRequestPermissionsResult(int requestCode, @NonNull String permissions[], @NonNull int[] grantResults) {
        switch (requestCode) {
            case Constants.PERMISSION_REQ_ID_WRITE_EXTERNAL_STORAGE: {
                if (grantResults.length > 0
                        && grantResults[0] == PackageManager.PERMISSION_GRANTED) {
                } else {
                    showShortToast("No permission for " + Manifest.permission.WRITE_EXTERNAL_STORAGE);
                }
                break;
            }
            case Constants.PERMISSION_REQ_ID_RECORD_AUDIO: {
                if (grantResults.length > 0
                        && grantResults[0] == PackageManager.PERMISSION_GRANTED) {
                    checkSelfPermission(Manifest.permission.CAMERA, Constants.PERMISSION_REQ_ID_CAMERA);
                } else {
                    showShortToast("No permission for " + Manifest.permission.RECORD_AUDIO);
                }
                break;
            }
            case Constants.PERMISSION_REQ_ID_CAMERA: {
                if (grantResults.length > 0
                        && grantResults[0] == PackageManager.PERMISSION_GRANTED) {
                } else {
                    showShortToast("No permission for " + Manifest.permission.CAMERA);
                }
                break;
            }
        }
    }
}
