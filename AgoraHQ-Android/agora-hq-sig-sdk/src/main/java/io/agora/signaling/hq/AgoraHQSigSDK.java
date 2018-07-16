package io.agora.signaling.hq;

import android.content.Context;
import android.os.Build;
import android.text.TextUtils;
import android.util.Log;

import org.json.JSONException;
import org.json.JSONObject;

import java.lang.ref.WeakReference;
import java.util.ArrayList;
import java.util.List;
import java.util.Timer;
import java.util.TimerTask;
import java.util.TreeSet;
import java.util.UUID;

import io.agora.AgoraAPI;
import io.agora.AgoraAPIOnlySignal;
import io.agora.IAgoraAPI;
import io.rong.imlib.IRongCallback;
import io.rong.imlib.RongIMClient;
import io.rong.imlib.common.BuildVar;
import io.rong.imlib.model.Conversation;
import io.rong.imlib.model.Message;
import io.rong.message.TextMessage;

import static io.agora.signaling.hq.StatisticsData.statisticsItem;

public class AgoraHQSigSDK {

    public static final String LOG_TAG = "AgoraHQSigSDK";

    private WeakReference<Context> mCtx;
    private String mAppId;
    private AgoraAPIOnlySignal mAgoraSigSDK;
    private EventHandler mEventHandler;

    private String mChannel = null;
    private String mAccount = null;
    private String mToken = null;

    private String mRcAppId;

    private Timer mStatisticsTimer;

    // if it's RC failed, do not notify users
    enum AgoraHQSigLoginStatus {
        LoginFailed(-1, "AgoraHQSigLoginStatus_LoginFailed"),
        Initial(0, "AgoraHQSigLoginStatus_Initial"),
        Logout(1, "AgoraHQSigLoginStatus_Logout"),
        Logining(2, "AgoraHQSigLoginStatus_Logining"),
        AgoraLoggedIn(3, "AgoraHQSigLoginStatus_AgoraLoggedIn"),
        RCLoggedIn(4, "AgoraHQSigLoginStatus_RCLoggedIn"),
        AllLoggedIn(5, "AgoraHQSigLoginStatus_AllLoggedIn");

        public final int value;
        public final String desc;

        AgoraHQSigLoginStatus(int value, String desc) {
            this.value = value;
            this.desc = desc;
        }
    }

    private volatile AgoraHQSigLoginStatus mLoginStatus;

    public AgoraHQSigSDK(Context context, String appId) {
        this.mCtx = new WeakReference<>(context);
        this.mAppId = appId;
        mAgoraSigSDK = AgoraAPIOnlySignal.getInstance(context, appId);
        mAgoraSigSDK.dbg("lbs_100", "1");
       // mAgoraSigSDK.dbg("lbss", "125.88.159.173");

        initRCIfPossible(context, appId, null);

        mLoginStatus = AgoraHQSigLoginStatus.Initial;
    }

    private void initRCIfPossible(final Context context, String appId, final Internal3rdDataHandler callback) {
        if (!TextUtils.equals(mRcAppId, "_disabled") && !TextUtils.isEmpty(mRcAppId)) {
            if (callback != null) {
                callback.onRCReady(mRcAppId);
            }
            return;
        }

        HttpUrlUtils utils = new HttpUrlUtils();
        Log.d(LOG_TAG, "start get 3rd " + mChannel + " " + appId);
        utils.execHttpAsyncTask(Constants.RY_HTTP_GET_3RD_ + "?vid=" + appId, false, new HttpUrlUtils.OnResponse() {
            @Override
            public void onResponse(String data) {

                if (TextUtils.isEmpty(data)) {
                    Log.e(LOG_TAG, "getVendorInfo result " + data);
                    return;
                }

                try {
                    JSONObject result = new JSONObject(data);
                    mRcAppId = result.getString("ry_appid");
                } catch (JSONException e) {
                }

                if (TextUtils.isEmpty(mRcAppId) || TextUtils.equals(mRcAppId, "_disabled")) {
                    return;
                }

                Log.d(LOG_TAG, "get 3rd done " + mChannel + " token[length]" + mRcAppId.length());

                AgoraLinkToCloud.init(context, mRcAppId);

                if (callback != null) {
                    callback.onRCReady(mRcAppId);
                }
            }
        }, null);

        Log.d(LOG_TAG, "init fully done");
    }

    public void login(final String account, String channel, String token, EventHandler handler) {
        // send the first statistic msg
        statisticsItem = new StatisticsItem();
        statisticsItem.setAppid(mAppId);
        statisticsItem.setAccount(account);
        statisticsItem.setChannel(channel);
        statisticsItem.setSid(UUID.randomUUID().toString());
        statisticsItem.setV(BuildConfig.VERSION_NAME);
        statisticsItem.setVa(mAgoraSigSDK.getSdkVersion() + "");
        statisticsItem.setVr(BuildVar.SDK_VERSION);
        statisticsItem.setP("Android");
        statisticsItem.setD(Build.MODEL);
        statisticsItem.setVos(Build.VERSION.RELEASE);

        List<Object> data = new ArrayList<>();
        data.add(System.currentTimeMillis());
        data.add("login");
        data.add(account);
        statisticsItem.addEvtsAItem(data);

        startStatisticsTimer();

        this.mEventHandler = handler;
        this.mAccount = account;
        this.mToken = token;
        this.mChannel = channel;

        if (TextUtils.isEmpty(mAccount) || TextUtils.isEmpty(mChannel) || TextUtils.isEmpty(mToken)) {
            throw new IllegalArgumentException("account and channel should not be null " + mAccount + " " + mChannel);
        }

        Log.d(LOG_TAG, "login " + mAccount + " " + mChannel + " " + mToken.length() + " " + handler);

        mLoginStatus = AgoraHQSigLoginStatus.Logining;

        mAgoraSigSDK.callbackSet(new ICallBack2() {
            @Override
            public void onLoginSuccess(int uid, int fd) {
                List<Object> data0 = new ArrayList<>();
                data0.add(System.currentTimeMillis());
                data0.add("onLoginSuccess");
                if (statisticsItem != null) {
                    statisticsItem.addEvtsAItem(data0);
                }

                Log.d(LOG_TAG, "onLoginSuccess " + (uid & 0xFFFFFFFFL) + " " + fd);
                mAgoraSigSDK.channelJoin(mChannel);

                List<Object> data = new ArrayList<>();
                data.add(System.currentTimeMillis());
                data.add("join");
                data.add(mChannel);
                if (statisticsItem != null) {
                    statisticsItem.addEvtsAItem(data);
                }
            }

            @Override
            public void onLoginFailed(int reason) {
                Log.e(LOG_TAG, "onLoginFailed " + reason);

                List<Object> data = new ArrayList<>();
                data.add(System.currentTimeMillis());
                data.add("onLoginFailed");
                data.add(reason);
                if (statisticsItem != null) {
                    statisticsItem.addEvtsAItem(data);
                }

                if (reason == IAgoraAPI.ECODE_LOGIN_E_ALREADY_LOGIN) {
                    return;
                }

                if (reason == IAgoraAPI.ECODE_LOGIN_E_NET) {
                    onConnectionFatalError(Constants.LOGIN_FAILED, "Login failed");
                    return;
                }

                if (reason == IAgoraAPI.ECODE_LOGIN_E_TOKENEXPIRED) {
                    onConnectionFatalError(Constants.TOKEN_EXPIRED, "Agora Token Login");
                    return;
                }

                if (reason == IAgoraAPI.ECODE_LOGIN_E_TOKENWRONG) {
                    onConnectionFatalError(Constants.TOKEN_INVALID, "Agora Token Login");
                    return;
                }
            }

            @Override
            public void onLogout(int reason) {
                Log.d(LOG_TAG, "onLogout " + reason + " " + mAgoraSigSDK.getStatus());

                List<Object> data = new ArrayList<>();
                data.add(System.currentTimeMillis());
                data.add("onLogout");
                data.add(reason);
                if (statisticsItem != null) {
                    statisticsItem.addEvtsAItem(data);
                }

                if (reason == IAgoraAPI.ECODE_LOGOUT_E_ALREADY_LOGOUT) {
                    return;
                }

                if (reason == AgoraAPI.ECODE_LOGOUT_E_NET) {
                    mAgoraSigSDK.login2(mAppId, mAccount, mToken, 0, null, 30, 3);
                    onConnectionFatalError(Constants.CONNECTION_FAILED, "Connection Failed");
                    return;
                }

                if (reason == IAgoraAPI.ECODE_LOGOUT_E_TOKENEXPIRED) {
                    onConnectionFatalError(Constants.TOKEN_EXPIRED, "Agora Token Logout");
                    return;
                }

                if (reason == IAgoraAPI.ECODE_LOGOUT_E_TOKENWRONG) {
                    onConnectionFatalError(Constants.TOKEN_INVALID, "Agora Token Logout");
                    return;
                }

                if (reason == IAgoraAPI.ECODE_LOGOUT_E_KICKED) {
                    onConnectionFatalError(Constants.KICKED_BY_SERVER, "Kicked by Server");
                    return;
                }
            }

            @Override
            public void onReconnecting(int nretry) {
                List<Object> data = new ArrayList<>();
                data.add(System.currentTimeMillis());
                data.add("onReconnecting");
                data.add(nretry);
                if (statisticsItem != null) {
                    statisticsItem.addEvtsAItem(data);
                }
            }

            @Override
            public void onReconnected(int fd) {
                List<Object> data = new ArrayList<>();
                data.add(System.currentTimeMillis());
                data.add("onReconnected");
                data.add(fd);
                if (statisticsItem != null) {
                    statisticsItem.addEvtsAItem(data);
                }
            }

            public void onChannelJoined(String channel) {
                Log.d(LOG_TAG, "onChannelJoined " + channel + " " + mLoginStatus);

                List<Object> data = new ArrayList<>();
                data.add(System.currentTimeMillis());
                data.add("onJoined");
                if (statisticsItem != null) {
                    statisticsItem.addEvtsAItem(data);
                }

                AgoraHQSigSDK.this.onLoginSuccess(AgoraHQSigLoginStatus.AgoraLoggedIn);
            }

            @Override
            public void onChannelLeaved(String s, int i) {
                List<Object> data = new ArrayList<>();
                data.add(System.currentTimeMillis());
                data.add("onLeaved");
                data.add(i);
                if (statisticsItem != null) {
                    statisticsItem.addEvtsAItem(data);
                }
            }

            @Override
            public void onChannelJoinFailed(String channel, int error) {
                Log.e(LOG_TAG, "onChannelJoinFailed " + channel + " " + error);

                AgoraHQSigSDK.this.onLoginFailed(Constants.LOGIN_FAILED, "Agora Channel Join " + error);
            }

            @Override
            public void onError(String name, int error, String desc) {
                Log.e(LOG_TAG, "onError " + name + " " + error + " " + desc);
            }

            @Override
            public void onMessageChannelReceive(String channel, String account, int uid, String msg) {
                if (TextUtils.equals(mAccount, account)) {
                    return;
                }

                JSONObject jsonObject;
                long msgId = 0;
                String m = null;
                try {
                    jsonObject = new JSONObject(msg);
                    msgId = jsonObject.getLong("id");
                    m = jsonObject.getString("m");
                } catch (JSONException e) {
                    Log.w(LOG_TAG, Log.getStackTraceString(e));
                }

                checkAndReturnChannelMessage(channel, msgId, m);

                List<Object> data = new ArrayList<>();
                data.add(System.currentTimeMillis());
                data.add("a");
                data.add("c");
                data.add(msgId);
                data.add(account);
                if (statisticsItem != null) {
                    statisticsItem.addMsgsItem(data);
                }
            }

            @Override
            public void onMessageInstantReceive(String account, int uid, String msg) {
                JSONObject jsonObject;
                long msgId = 0;
                String m = null;
                try {
                    jsonObject = new JSONObject(msg);
                    msgId = jsonObject.getLong("id");
                    m = jsonObject.getString("m");
                } catch (JSONException e) {
                    Log.w(LOG_TAG, Log.getStackTraceString(e));
                }

                checkAndReturnInstantMessage(account, msgId, m);

                List<Object> data = new ArrayList<>();
                data.add(System.currentTimeMillis());
                data.add("a");
                data.add("i");
                data.add(msgId);
                data.add(account);
                data.add(mAccount);
                if (statisticsItem != null) {
                    statisticsItem.addMsgsItem(data);
                }
            }

        });

        mAgoraSigSDK.login2(mAppId, mAccount, token, 0, null, 30, 3);

        AgoraLinkToCloud.setConnectionStatusListener(new RongIMClient.ConnectionStatusListener() {
            @Override
            public void onChanged(ConnectionStatus connectionStatus) {
                Log.d(LOG_TAG, "onChanged " + connectionStatus + " " + mLoginStatus);

                if (mAgoraSigSDK != null && connectionStatus == ConnectionStatus.CONNECTING) {
                    mAgoraSigSDK.setNetworkStatus(0);
                    if (mAgoraSigSDK.getStatus() == 0) { // LOGIN_STATE_NOT_LOGIN
                        mAgoraSigSDK.login2(mAppId, mAccount, mToken, 0, null, 30, 3);
                    }
                }

                if (connectionStatus == ConnectionStatus.NETWORK_UNAVAILABLE) {
                    onConnectionFatalError(Constants.NO_CONNECTION, "No connection");
                }

                List<Object> data = new ArrayList<>();
                data.add(System.currentTimeMillis());
                data.add("onConnectionStatus");
                data.add(connectionStatus.getValue());
                if (statisticsItem != null) {
                    statisticsItem.addEvtsRItem(data);
                }
            }
        });

        RongIMClient.setChatRoomActionListener(new RongIMClient.ChatRoomActionListener() {
            @Override
            public void onJoining(String channel) {
                Log.d(LOG_TAG, "onJoining " + channel + " " + mAgoraSigSDK.getStatus() + " " + mLoginStatus);
            }

            @Override
            public void onJoined(String channel) {
                Log.d(LOG_TAG, "onJoined " + channel + " " + mAgoraSigSDK.getStatus() + " " + mLoginStatus);
            }

            @Override
            public void onQuited(String channel) {
                Log.d(LOG_TAG, "onQuited " + channel);
            }

            @Override
            public void onError(String channel, RongIMClient.ErrorCode errorCode) {
                Log.d(LOG_TAG, "onError " + channel + " " + errorCode);
            }
        });

        AgoraLinkToCloud.setOnReceiveMessageListener(mAccount, new InternalMessageEventHandler() {

            @Override
            public void onChannelMessageReceived(String channel, long msgId, String msg) {

                Log.d("zhangtao  iCloud  = ",msg);

                checkAndReturnChannelMessage(channel, msgId, msg);
            }

            @Override
            public void onMessageReceivedFrom(String account, long msgId, String msg) {
                Log.d("zhangtao","account = "+account + " msgID = "+msgId+" msg = "+msg);
                checkAndReturnInstantMessage(account, msgId, msg);
            }
        });

        initRCIfPossible(mCtx.get(), mAppId, new Internal3rdDataHandler() {
            @Override
            public void onRCReady(String appId) {
                getTokenAndLoginRC();
            }
        });

        Log.d(LOG_TAG, "login " + mAccount + ":" + account + " " + mChannel + " " + handler + " done");
    }

    private void getTokenAndLoginRC() {

        List<Object> tokenData = new ArrayList<>();
        tokenData.add(System.currentTimeMillis());
        tokenData.add("token");
        if (statisticsItem != null) {
            statisticsItem.addEvtsRItem(tokenData);
        }

        AgoraLinkToCloud.getRCToken(mAccount, mChannel, mAppId, new HttpUrlUtils.OnResponse() {
            @Override
            public void onResponse(String data) {

                if (TextUtils.isEmpty(data)) {
                    Log.e(LOG_TAG, "getRCToken result " + data);
                    return;
                }

                JSONObject jsonObject;
                String token = null;
                String error = null;
                String uid = null;
                try {
                    jsonObject = new JSONObject(data);
                    uid = jsonObject.getString("uid");
                    token = jsonObject.getString("token");
                    error = jsonObject.getString("err");
                } catch (JSONException e) {
                    Log.w(LOG_TAG, Log.getStackTraceString(e));
                }

                if (!TextUtils.isEmpty(error) || TextUtils.isEmpty(token)) {
                    Log.e(LOG_TAG, "getRCToken failed " + error);
                    return;
                }

                Log.d(LOG_TAG, "login rc with token(length) " + (token.length()) + " " + error);

                List<Object> tokenData = new ArrayList<>();
                tokenData.add(System.currentTimeMillis());
                tokenData.add("onToken");
                if (statisticsItem != null) {
                    statisticsItem.addEvtsRItem(tokenData);
                }

                List<Object> loginData = new ArrayList<>();
                loginData.add(System.currentTimeMillis());
                loginData.add("login");
                loginData.add(mAccount);
                if (statisticsItem != null) {
                    statisticsItem.addEvtsRItem(loginData);
                }

                AgoraLinkToCloud.connect(token, new RongIMClient.ConnectCallback() {
                    @Override
                    public void onTokenIncorrect() {
                        Log.e(LOG_TAG, "onTokenIncorrect");
//                        AgoraHQSigSDK.this.onLoginFailed(Constants.TOKEN_INVALID, "RC Token Invalid");
                    }

                    @Override
                    public void onSuccess(String s) {
                        Log.e(LOG_TAG, "onSuccess " + s);

                        List<Object> data0 = new ArrayList<>();
                        data0.add(System.currentTimeMillis());
                        data0.add("onLoginSuccess");
                        if (statisticsItem != null) {
                            statisticsItem.addEvtsRItem(data0);
                        }

                        List<Object> data = new ArrayList<>();
                        data.add(System.currentTimeMillis());
                        data.add("join");
                        data.add(mChannel);
                        if (statisticsItem != null) {
                            statisticsItem.addEvtsRItem(data);
                        }

                        AgoraLinkToCloud.joinChatRoom(mChannel, -1, new RongIMClient.OperationCallback() {
                            @Override
                            public void onSuccess() {
                                Log.d(LOG_TAG, "onSuccess");
                                List<Object> data = new ArrayList<>();
                                data.add(System.currentTimeMillis());
                                data.add("onJoined");
                                if (statisticsItem != null) {
                                    statisticsItem.addEvtsRItem(data);
                                }

                                AgoraHQSigSDK.this.onLoginSuccess(AgoraHQSigLoginStatus.RCLoggedIn);
                            }

                            @Override
                            public void onError(RongIMClient.ErrorCode errorCode) {
                                Log.e(LOG_TAG, "onError " + errorCode);
                                AgoraHQSigSDK.this.onLoginFailed(errorCode.getValue(), errorCode.getMessage());

                                List<Object> data = new ArrayList<>();
                                data.add(System.currentTimeMillis());
                                data.add("onLeaved");
                                data.add(errorCode.getValue());
                                if (statisticsItem != null) {
                                    statisticsItem.addEvtsRItem(data);
                                }
                            }
                        });
                    }

                    @Override
                    public void onError(RongIMClient.ErrorCode errorCode) {
                        Log.e(LOG_TAG, "onError " + errorCode);

                        List<Object> data = new ArrayList<>();
                        data.add(System.currentTimeMillis());
                        data.add("onLoginFailed");
                        data.add(errorCode.getValue());
                        if (statisticsItem != null) {
                            statisticsItem.addEvtsRItem(data);
                        }

//                        AgoraHQSigSDK.this.onLoginFailed(errorCode.getValue(), errorCode.getMessage());
                    }
                });

            }
        });
    }

    public void sendChannelMessage(long msgId, String msg) {
        String rawMsg = "{\"id\": " + msgId + ", \"m\": \"" + msg + "\"}";


        mAgoraSigSDK.messageChannelSend(mChannel, rawMsg, null);

        Log.d("zhangtao  ","sdk - sendChannelMessage = "+rawMsg);
        TextMessage myTextMessage = TextMessage.obtain(rawMsg);
        Message myMessage = Message.obtain(mChannel, Conversation.ConversationType.CHATROOM, myTextMessage);
        RongIMClient.getInstance().sendMessage(myMessage, null, null, new IRongCallback.ISendMessageCallback() {
            @Override
            public void onAttached(Message message) {
            }

            @Override
            public void onSuccess(Message message) {
            }

            @Override
            public void onError(Message message, RongIMClient.ErrorCode errorCode) {
            }
        });
    }

    public void sendMessageTo(String account, long msgId, String msg) {
        String rawMsg = "{\"id\": " + msgId + ", \"m\": \"" + msg + "\"}";

        mAgoraSigSDK.messageInstantSend(account, 0, rawMsg, null);
        TextMessage myTextMessage = TextMessage.obtain(rawMsg);
        Message myMessage = Message.obtain(account, Conversation.ConversationType.PRIVATE, myTextMessage);
        RongIMClient.getInstance().sendMessage(myMessage, null, null, new IRongCallback.ISendMessageCallback() {
            @Override
            public void onAttached(Message message) {
            }

            @Override
            public void onSuccess(Message message) {
            }

            @Override
            public void onError(Message message, RongIMClient.ErrorCode errorCode) {
            }
        });
    }

    public void logout() {

        mAgoraSigSDK.channelLeave(mChannel);

        List<Object> data0 = new ArrayList<>();
        data0.add(System.currentTimeMillis());
        data0.add("leave");
        data0.add(mChannel);
        if (statisticsItem != null) {
            statisticsItem.addEvtsAItem(data0);
        }

        mAgoraSigSDK.logout();

        List<Object> data1 = new ArrayList<>();
        data1.add(System.currentTimeMillis());
        data1.add("logout");
        if (statisticsItem != null) {
            statisticsItem.addEvtsAItem(data1);
        }

        RongIMClient.getInstance().disconnect();

        List<Object> data2 = new ArrayList<>();
        data2.add(System.currentTimeMillis());
        data2.add("logout");
        if (statisticsItem != null) {
            statisticsItem.addEvtsRItem(data2);
        }

        AgoraLinkToCloud.quitChatRoom(new RongIMClient.OperationCallback() {
            @Override
            public void onSuccess() {

            }

            @Override
            public void onError(RongIMClient.ErrorCode errorCode) {

            }
        }, mChannel);

        List<Object> data3 = new ArrayList<>();
        data3.add(System.currentTimeMillis());
        data3.add("leave");
        data3.add(mChannel);
        if (statisticsItem != null) {
            statisticsItem.addEvtsRItem(data3);
        }

        postStatistics();
        stopStatisticsTimer();
        // statisticsItem = null;

        mRcAppId = null;

        mAccount = null;

        mLastReceivedMsgIdList.clear();

        Log.d(LOG_TAG, "logout " + mChannel + " " + mAccount);
    }

    private void startStatisticsTimer() {
        if (mStatisticsTimer != null) {
            return;
        }

        mStatisticsTimer = new Timer();
        mStatisticsTimer.schedule(new TimerTask() {
            @Override
            public void run() {
                if (statisticsItem == null) {
                    return;
                }

                String json = statisticsItem.convertToJson();
                if(statisticsItem != null) {
                    statisticsItem.clear();
                }
                new HttpUrlUtils().execHttpSyncTask(Constants.URL_POST_STATISTICS_, true, null, json);

            }
        }, 0, 30000);
    }

    private void postStatistics() {
        if (statisticsItem == null) {
            return;
        }

        new HttpUrlUtils().execHttpAsyncTask(Constants.URL_POST_STATISTICS_, true, null, statisticsItem.convertToJson());
    }

    private void stopStatisticsTimer() {
        if (mStatisticsTimer != null) {
            mStatisticsTimer.cancel();
            mStatisticsTimer = null;
        }
    }

    private void onLoginSuccess(AgoraHQSigLoginStatus status) {
        if (mLoginStatus.value <= AgoraHQSigLoginStatus.Logining.value) {
            mLoginStatus = status;

            if (mEventHandler != null) {
                mEventHandler.onLoginSuccess();
            }
        } else {
            mLoginStatus = AgoraHQSigLoginStatus.AllLoggedIn;
        }
    }

    private void onLoginFailed(int error, String errMsg) {
        if (mLoginStatus == AgoraHQSigLoginStatus.Logining) {
            mLoginStatus = AgoraHQSigLoginStatus.LoginFailed;
        } else if (mLoginStatus == AgoraHQSigLoginStatus.LoginFailed) {
            if (mEventHandler != null) {
                mEventHandler.onError(error);
            }
            mLoginStatus = AgoraHQSigLoginStatus.Logout;
        }
    }

    private void onConnectionFatalError(int error, String errMsg) {
        // can not recover
        // invalid token
        // rejected
        if (mEventHandler != null) {
            mEventHandler.onError(error);
        }
    }

    private final Object[] mMsgLock = new Object[0];

    private final int MAX_MSG_COUNT_THRESHOLD = 30;

    private final TreeSet<Long> mLastReceivedMsgIdList = new TreeSet<>();

    private void checkAndReturnChannelMessage(String channel, long msgId, String msg) {
        Log.d("zhangtao  ss"," msggg = "+msg);
        synchronized (mMsgLock) {
            if (mLastReceivedMsgIdList.contains(msgId)) {
                return;
            }

            if (mLastReceivedMsgIdList.size() >= MAX_MSG_COUNT_THRESHOLD) {
                Long lastObj = mLastReceivedMsgIdList.pollLast();
                mLastReceivedMsgIdList.remove(lastObj);
            }

            mLastReceivedMsgIdList.add(msgId);
        }


        // check duplicated message
        if (mEventHandler != null) {
            mEventHandler.onChannelMessageReceived(channel, msgId, msg);
        }
    }

    private void checkAndReturnInstantMessage(String account, long msgId, String msg) {
        synchronized (mMsgLock) {
            if (mLastReceivedMsgIdList.contains(msgId)) {
                return;
            }

            if (mLastReceivedMsgIdList.size() >= MAX_MSG_COUNT_THRESHOLD) {
                Long lastObj = mLastReceivedMsgIdList.pollLast();
                mLastReceivedMsgIdList.remove(lastObj);
            }

            mLastReceivedMsgIdList.add(msgId);
        }

        // check duplicated message
        if (mEventHandler != null) {
            mEventHandler.onMessageReceivedFrom(account, msgId, msg);
        }
    }

    public interface Internal3rdDataHandler {
        void onRCReady(String appId);
    }

    public interface InternalMessageEventHandler {
        void onChannelMessageReceived(String channel, long msgId, String msg);

        void onMessageReceivedFrom(String account, long msgId, String msg);
    }

    public interface EventHandler {
        void onLoginSuccess();

        void onError(int error);

        void onChannelMessageReceived(String channel, long msgId, String msg);

        void onMessageReceivedFrom(String account, long msgId, String msg);
    }
}
