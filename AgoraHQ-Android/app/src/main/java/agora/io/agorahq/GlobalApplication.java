package agora.io.agorahq;

import android.app.Application;

import com.tencent.bugly.crashreport.CrashReport;

import agora.io.agorahq.bean.GameInfo;
import agora.io.agorahq.logicworker.WorkThread;

public class GlobalApplication extends Application {
    private WorkThread mWorkThread;
    private GameInfo mGlobalGameInfo;

    @Override
    public void onCreate() {
        super.onCreate();

        if (AppBuildConfig.DEBUG_ON) {
            CrashReport.UserStrategy strategy = new CrashReport.UserStrategy(getApplicationContext());
            strategy.setAppVersion(BuildConfig.VERSION_NAME + "(" + BuildConfig.VERSION_CODE + ")");
            // if you want publish the app , turn it to false
            CrashReport.initCrashReport(getApplicationContext(), AppBuildConfig.APPLICATION_BUGLY_APPID, false, strategy);
        }

        mGlobalGameInfo = GameInfo.newInstance();

        VMExceptionHandler.install();
    }

    public synchronized void initWorkThread(){
        if (mWorkThread == null) {
            mWorkThread = new WorkThread(getApplicationContext());

            mWorkThread.start();
            mWorkThread.waitForReady();
        }
    }

    public synchronized void deinitWorkThread() {
        if (mWorkThread != null) {
            mWorkThread.exit();
            try {
                mWorkThread.join();
            } catch (InterruptedException e) {
                e.printStackTrace();
            }

            mWorkThread = null;
        }
    }

    public WorkThread getWorkThread() {
        return mWorkThread;
    }

    public GameInfo getGlobalGameInfo() {
        return mGlobalGameInfo;
    }
}
