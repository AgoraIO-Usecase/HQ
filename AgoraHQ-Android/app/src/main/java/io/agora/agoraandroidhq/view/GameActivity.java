package io.agora.agoraandroidhq.view;

import android.Manifest;
import android.app.Activity;
import android.app.AlertDialog;
import android.content.Intent;
import android.content.pm.ActivityInfo;
import android.content.pm.PackageManager;
import android.graphics.Color;
import android.os.Build;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.support.annotation.NonNull;
import android.support.annotation.Nullable;
import android.support.annotation.RequiresApi;
import android.support.v4.app.ActivityCompat;
import android.support.v4.content.ContextCompat;
import android.support.v7.widget.LinearLayoutManager;
import android.support.v7.widget.RecyclerView;
import android.text.Editable;
import android.text.TextUtils;
import android.text.TextWatcher;
import android.util.Log;
import android.view.SurfaceView;
import android.view.View;
import android.view.ViewGroup;
import android.view.Window;
import android.view.WindowManager;
import android.view.animation.GridLayoutAnimationController;
import android.widget.Button;
import android.widget.CheckBox;
import android.widget.CompoundButton;
import android.widget.EditText;
import android.widget.FrameLayout;
import android.widget.ImageView;
import android.widget.LinearLayout;
import android.widget.TextView;
import android.widget.Toast;

import org.json.JSONException;
import org.json.JSONObject;

import java.lang.ref.WeakReference;
import java.util.ArrayList;
import java.util.Random;
import java.util.concurrent.Executor;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;

import io.agora.agoraandroidhq.R;
import io.agora.agoraandroidhq.control.AgoraSignal;
import io.agora.agoraandroidhq.control.JsonToString;
import io.agora.agoraandroidhq.module.Question;
import io.agora.agoraandroidhq.module.Result;
import io.agora.agoraandroidhq.module.User;
import io.agora.agoraandroidhq.tools.Constants;
import io.agora.agoraandroidhq.tools.GameControl;
import io.agora.agoraandroidhq.tools.GangUpRecycleViewAdapter;
import io.agora.agoraandroidhq.tools.HttpUrlUtils;
import io.agora.agoraandroidhq.tools.MessageListDecoration;
import io.agora.agoraandroidhq.tools.MessageRecyclerViewAdapter;
import io.agora.rtc.IRtcEngineEventHandler;
import io.agora.rtc.RtcEngine;
import io.agora.rtc.video.VideoCanvas;


/**
 * Created by zhangtao on 2018/1/12.
 */

public class GameActivity extends Activity {


    private boolean isFirst = true;

    @Override
    protected void onCreate(@Nullable Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        this.requestWindowFeature(Window.FEATURE_NO_TITLE);

        setContentView(R.layout.paly_game);
        this.getWindow().setFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN, WindowManager.LayoutParams.FLAG_FULLSCREEN);
        setRequestedOrientation(ActivityInfo.SCREEN_ORIENTATION_PORTRAIT);

        try {
            init();
        } catch (Exception e) {
            e.printStackTrace();
        }
        GameControl.logD("sub GameThread name = " + Thread.currentThread());
    }


    private User getUser() {

        User currentUser = new User();
        currentUser.name = GameControl.currentUserName;
        currentUser.drawable = GameControl.currentUserHeadImage;
        String channelName = getChannelName();
        currentUser.setChannelName(channelName);
        GameControl.currentUser = currentUser;

        return currentUser;
    }

    private void init() throws Exception {
        getUser();

        loginAgoraSignal();
        GameControl.logD("init");
        findView();
        initQuestionLayout();

        //startCheckWheatherCanPlay();
        // checkWheatherCanPlay();
        GameControl.controlCheckThread = true;
        checkSelfPermissions();
        executorService = createExcetorService();
        isInGangUp = false;

    }

    private ExecutorService executorService;

    private ExecutorService createExcetorService() {

        ExecutorService executorService = Executors.newCachedThreadPool();
        return executorService;
    }

    private String getAccount() {
        String uid = Constants.UID.toString() + new Random().nextInt(100000);
        GameControl.logD("getAccount = " + uid);
        return uid;
    }

    private String getChannelName() {
        Intent intent = getIntent();
        String channelName = intent.getStringExtra("ChannelName");
        GameControl.logD("getChannelName  = " + channelName);
        return channelName;
    }

    private AgoraSignal agoraSignal;

    private void loginAgoraSignal() {
        agoraSignal = AgoraSignal.newInstance(GameActivity.this, Constants.AGORA_APP_ID, getAccount(), getChannelName());
        agoraSignal.addEventHandler(agoraHandler);
        agoraSignal.login();
    }

    private boolean wheatherChangeGameReuslt = true;

    private Handler agoraHandler = new Handler() {
        @Override
        public void handleMessage(Message msg) {
            super.handleMessage(msg);
            switch (msg.what) {
                case Constants.LOGIN_AGORA_SIGNAL_FAIL:
                    GameControl.logD("LOGIN_AGORA_SIGNAL_FAIL");
                    toastHelper(getString(R.string.login_agora_signal_fail));
                    //finish();
                    agoraSignal.login();
                    break;
                case Constants.LOGIN_AGORA_SIGNAL_SUCCESS:
                    GameControl.logD("LOGIN_AGORA_SIGNAL_SUCCESS");
                    toastHelper(getString(R.string.login_agora_signal_success));

                    break;

                case Constants.AGORA_SIGNAL_RECEIVE:

                    String mess = (String) msg.obj;
                    Object jsonObject = null;
                    try {
                        jsonObject = JsonToString.jsonToString(mess);
                    } catch (JSONException e) {
                        e.printStackTrace();
                    }
                    GameControl.logD("AGORA_SIGNAL_RECEIVE   =  " + mess + "  strinType = " + JsonToString.strinType);
                    switch (JsonToString.strinType) {

                        case "chat":
                            io.agora.agoraandroidhq.module.Message message = (io.agora.agoraandroidhq.module.Message) jsonObject;
                            messageRecyclerViewAdapter.updateData(message);
                            recyclerView.smoothScrollToPosition(messageRecyclerViewAdapter.getItemCount() - 1);

                            break;

                        case "result":
                            Result result = (Result) jsonObject;

                            int correct = result.correct;
                            int res = result.result;

                            if (GameControl.currentQuestion != null) {
                                GameControl.logD("GameControl.currentQues = " + GameControl.currentQuestion.toString());
                                GameControl.logD("result  showHighLightCheckBox  =  " + checkBox_item.size());
                                   /* setCheckBoxBackHighLight(res);*/
                                if (correct == 0) {
                                    int answer = res + 1;
                                    time_reduce.setText(getString(R.string.answer_error_message));

                                    time_reduce.setTextColor(Color.RED);
                                    time_reduce.setVisibility(View.VISIBLE);
                                    //game_title.setVisibility(View.INVISIBLE);
                                    GameControl.clientWheatherCanPlay = false;

                                    if (questionTime != 0 && (questionTime != GameControl.timeOut)) {
                                        questionTime = 0;
                                    }

                                    if (wheatherChangeGameReuslt) {

                                        GameControl.gameResult = false;
                                        wheatherChangeGameReuslt = false;
                                    }

                                } else {


                                    time_reduce.setText(R.string.answer_correct_message);
                                    time_reduce.setTextColor(Color.GREEN);
                                    time_reduce.setVisibility(View.VISIBLE);
                                    //game_title.setVisibility(View.INVISIBLE);
                                    GameControl.clientWheatherCanPlay = true;

                                    if (questionTime != 0 && (questionTime != GameControl.timeOut)) {
                                        questionTime = 0;
                                    }
                                }


                                questionTimeHandler.sendEmptyMessageDelayed(1, 5000);
                                //time_reduce.setVisibility(View.INVISIBLE);
                                game_layout.setVisibility(View.VISIBLE);
                                submit_btn.setVisibility(View.GONE);
                                getCorrectCheckBox(res);


                                if ((GameControl.currentQuestion.getId() + 1) == GameControl.total) {
                                    questionTimeHandler.sendEmptyMessageDelayed(2, 6000);
                                }
                            }
                            break;

                        case "quiz":

                            Question question = (Question) jsonObject;
                            if (question.getId() == 1) {
                                GameControl.gameResult = true;
                                wheatherChangeGameReuslt = true;
                            }
                            GameControl.logD("save Question :  id = " + question.getId() + "  " + question.getTimeOut());
                            if (question != null) {
                                GameControl.setCurrentQuestion(question);
                                int total = GameControl.currentQuestion.getTotalQuestion();
                                int timeOut = GameControl.currentQuestion.getTimeOut();

                                if (total != 0) {
                                    GameControl.total = total;
                                }

                                if (timeOut != 0) {
                                    GameControl.timeOut = timeOut;
                                    time_reduce.setText(timeOut + " s");
                                }
                            }
                            try {
                                if (!isFirst) {
                                    checkWheatherCanPlay();

                                    isFirst = false;
                                }
                            } catch (JSONException e) {
                                e.printStackTrace();
                            }
                            break;
                        case "inviteRequest":
                            GameControl.logD("inviteRequest");
                            showInvitationDialog();


                            break;

                    }
                    break;

                case Constants.AGORA_SIGNAL_SEND:

                    String sendMessage = (String) msg.obj;
                    GameControl.logD("sendMessage  = " + sendMessage);
                    io.agora.agoraandroidhq.module.Message jsonObjects = null;
                    try {
                        jsonObjects = (io.agora.agoraandroidhq.module.Message) JsonToString.jsonToString(sendMessage);
                    } catch (JSONException e) {
                        e.printStackTrace();
                    }
                    GameControl.logD("sendMessage  sendName  sendContent " + jsonObjects.getContent() + "   " + jsonObjects.getSender());
                    String sendName = jsonObjects.getSender();

                    String content = jsonObjects.getContent();
                    GameControl.logD("sendMessage = ");
                    io.agora.agoraandroidhq.module.Message message = new io.agora.agoraandroidhq.module.Message(sendName, content);
                    message.setIsMe(true);
                    messageRecyclerViewAdapter.updateData(message);


                    try {
                        Thread.sleep(600);
                    } catch (InterruptedException e) {
                        e.printStackTrace();
                    }

                    recyclerView.smoothScrollToPosition(messageRecyclerViewAdapter.getItemCount() - 1);
                    break;


                case Constants.MESSAGE_SEND_ERROR:
                    //TODO
                    break;
                default:
                    break;

            }
        }
    };


    private void toastHelper(String message) {
        Toast.makeText(GameActivity.this, message, Toast.LENGTH_SHORT).show();
    }

    private void logD(String message) {
        Log.d("zhangHQ", message + "");
    }

    private void startCheckWheatherCanPlay() {

        executorService.execute(new Runnable() {
            @Override
            public void run() {


                try {
                    Thread.sleep(1000);
                } catch (InterruptedException e) {
                    e.printStackTrace();
                }
                GameControl.logD("startCheckWheatherCanPalyThread");

                try {
                    if (isFirst) {
                        checkWheatherCanPlay();

                        isFirst = false;
                    }
                } catch (JSONException e) {
                    e.printStackTrace();
                }
            }
        });
    }


    private RecyclerView recyclerView;
    private MessageRecyclerViewAdapter messageRecyclerViewAdapter;
    private LinearLayout messageLinearLayou;
    private EditText input_editor;
    private TextView sendButton;
    private ImageView imageViewBack;
    private ImageView sendMessageImage;
    private FrameLayout game_view_layout;
    private Button gameGangUpButton;
    private boolean wheatherHasFocus = false;


    private RecyclerView gangUpUidRecycleView;
    private GangUpRecycleViewAdapter gangUpRecycleViewAdapter;
    private ArrayList<String> gangUpUidList = new ArrayList<String>();


    private FrameLayout localSmallVideo;
    private Button disConnectBtn;
    private boolean isInVideoWithBroadcast = false;


    private void findView() {
        imageViewBack = findViewById(R.id.back_image);
        imageViewBack.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                //TODO


                GameActivity.this.finish();
            }
        });

        gameGangUpButton = findViewById(R.id.game_gang_up_btn);
        disConnectBtn = findViewById(R.id.disConnectVideo);
        game_view_layout = findViewById(R.id.game_view_layout);
        messageLinearLayou = findViewById(R.id.sendMessage_layout);
        input_editor = findViewById(R.id.input_editor);
        sendButton = findViewById(R.id.sendMessage_text);
        sendMessageImage = findViewById(R.id.image_sendMessage);
        sendMessageImage.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                messageLinearLayou.setVisibility(View.VISIBLE);
                input_editor.requestFocus();
            }
        });

        input_editor.addTextChangedListener(new TextWatcher() {
            @Override
            public void beforeTextChanged(CharSequence s, int start, int count, int after) {

            }

            @RequiresApi(api = Build.VERSION_CODES.JELLY_BEAN)
            @Override
            public void onTextChanged(CharSequence s, int start, int before, int count) {
            }

            @Override
            public void afterTextChanged(Editable s) {
                sendButton.setEnabled(!s.toString().isEmpty());
                sendButton.setEnabled(true);
                sendButton.setClickable(true);
                input_editor.removeTextChangedListener(this);
            }
        });

        input_editor.setOnFocusChangeListener(new View.OnFocusChangeListener() {
            @Override
            public void onFocusChange(View v, boolean hasFocus) {
                if (hasFocus) {
                    wheatherHasFocus = true;
                } else {

                }

            }
        });

        sendButton.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                logD("sendButton Click");
                String message = input_editor.getText().toString();
                input_editor.clearFocus();
                input_editor.setText("");
                String messageString = null;

                try {
                    messageString = JsonToString.sendMessageString(message);

                } catch (JSONException e) {
                    e.printStackTrace();
                }

                String selfMessage = null;
                try {
                    selfMessage = JsonToString.sendMessageSelf(message);
                } catch (JSONException e) {

                }

                GameControl.logD("messageSend  =  " + messageString);

                if (TextUtils.isEmpty(message)) {
                    Toast.makeText(GameActivity.this, "content can not be null", Toast.LENGTH_SHORT).show();
                } else {
                    /*TextMessage content = TextMessage.obtain(messageString);
                    AgoraLinkToCloud.sendMessage(content, Constants.questionRoom);*/
                    if (agoraSignal != null) {
                        agoraSignal.sendChannelMessage(messageString, selfMessage);
                    }
                }
            }
        });


        game_view_layout.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                messageLinearLayou.setVisibility(View.GONE);
            }
        });

        ArrayList arrayList = new ArrayList();


        recyclerView = findViewById(R.id.messageRecycleView);
        messageRecyclerViewAdapter = new MessageRecyclerViewAdapter(GameActivity.this, arrayList);
        recyclerView.setAdapter(messageRecyclerViewAdapter);
        recyclerView.setLayoutManager(new LinearLayoutManager(this, RecyclerView.VERTICAL, false));
        recyclerView.addItemDecoration(new MessageListDecoration());

        if (messageRecyclerViewAdapter.getItemCount() > 0) {
            recyclerView.smoothScrollToPosition(messageRecyclerViewAdapter.getItemCount() - 1);
        }
        // AgoraLinkToCloud.addEventHandler(handler);


        gameResult = findViewById(R.id.game_result);
        gangUpUidRecycleView = findViewById(R.id.gangUpUidRecycleView);
        gangUpRecycleViewAdapter = new GangUpRecycleViewAdapter(GameActivity.this);
        gangUpUidRecycleView.setAdapter(gangUpRecycleViewAdapter);
        gangUpUidRecycleView.setLayoutManager(new LinearLayoutManager(this, RecyclerView.VERTICAL, false));

        localSmallVideo = findViewById(R.id.small_video);

    }


    @Override
    protected void onStart() {
        super.onStart();


    }


    private void checkWheatherCanPlay() throws JSONException {
        AgoraSignal.checkWheatherCanPlay(new HttpUrlUtils.OnResponse() {
            @Override
            public void onResponse(String data) throws JSONException {
                //logD(data);
                if (data.equals(Constants.MESSAGE_TOAST)) {
                    return;
                }
                // isFirst = false;
                if (data != null) {
                    JSONObject object = new JSONObject(data);
                    boolean wheatherCanPlay = object.getBoolean("result");
                    logD("wheatherCanPlay  =  " + wheatherCanPlay);
                    GameControl.serverWheatherCanPlay = wheatherCanPlay;
                    object = null;
                }

            }
        });
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();

        GameControl.logD("GameActivityDestory");
        GameControl.controlCheckThread = false;
        GameControl.currentQuestion = null;
        questionFlag = false;


        if (agoraSignal != null) {
            agoraSignal.removeEnventHandler();
            executorService.execute(new Runnable() {
                @Override
                public void run() {
                    agoraSignal.onLogoutSDKClick();
                }
            });

        }

        if (gangUpRtcEngineEventHandler != null) {
            gangUpRtcEngineEventHandler = null;
        }
        leaveChannel();
        // RtcEngine.destroy(rtcEngine);
        mRtcEventHandler = null;

        recyclerView = null;
        messageRecyclerViewAdapter = null;

        checkBox_item.clear();
        checkBox_item = null;

        board.clear();
        board = null;

        questionTimeHandler = null;
        executorService = null;

        System.gc();
    }


    private RtcEngine rtcEngine;
    private boolean isFirstInVideo = true;
    private int invisitUid = -1;
    private int broadcastUid = -1;

    private IRtcEngineEventHandler mRtcEventHandler = new IRtcEngineEventHandler() { // Tutorial Step 1

        @Override
        public void onFirstRemoteVideoDecoded(final int uid, int width, int height, int elapsed) { // Tutorial Step 5
            // logD("onFirstRemoteVideoDecode");
            GameControl.logD("sub onFirstRemoteVideoDecoded");
            runOnUiThread(new Runnable() {
                @Override
                public void run() {
                    if (uid == Constants.broadCastUid) {
                        GameControl.logD("sub onFirstRemoteVideoDecoded  big  =  " + uid);
                        setupRemoteVideo(uid);

                    } else {

                        GameControl.logD("sub onFirstRemoteVideoDecoded  small  uid = " + uid);
                        setUpSmallVideo(uid);

                        invisitUid = uid;
                    }

                }
            });
        }

        @Override
        public void onAudioVolumeIndication(AudioVolumeInfo[] speakers, int totalVolume) {
            super.onAudioVolumeIndication(speakers, totalVolume);
            //GameControl.logD("sub onAudioVolumeIndication  totalvolume =  " + speakers.length);
            if (speakers.length == 0) {
                return;
            }

            int count = 0;
            for (int i = 0; i < speakers.length; i++) {

                if ((speakers[i].uid == Constants.broadCastUid) || (speakers[i].uid == invisitUid) || (speakers[i].uid == 0)) {

                    count++;
                }
            }

            if ((speakers.length - count) > 0) {

                GameControl.logD("sub onAudioVolumeIndication  totalvolume =  >0");

                rtcEngine.setParameters("{\"che.audio.playout.uid.volume\":{\"uid\":1,\"volume\":20}}");

                if (invisitUid != -1) {
                    GameControl.logD("sub onAudioVolumeIndication  totalvolume != -1");
                    String params = "{\"che.audio.playout.uid.volume\":{\"uid\":" + invisitUid + ",\"volume\":20}}";
                    rtcEngine.setParameters(params);
                }
            } else {

                // GameControl.logD("sub onAudioVolumeIndication  totalvolume <=0  1");
                rtcEngine.setParameters("{\"che.audio.playout.uid.volume\":{\"uid\":1,\"volume\":100}}");

                String param3 = "{\"che.audio.playout.uid.volume\":{\"uid\":" + invisitUid + ",\"volume\":100}}";
                rtcEngine.setParameters(param3);


            }
        }


        @Override
        public void onUserOffline(final int uid, int reason) { // Tutorial Step 7
            logD("onUserOffline");
            runOnUiThread(new Runnable() {
                @Override
                public void run() {
                    if (uid == Constants.broadCastUid) {
                        onRemoteUserLeft();
                    } else if (uid == invisitUid) {

                        invisitUid = -1;

                        onRemoteSmallVideoLeft();

                    }
                }
            });
            GameControl.logD("onUserOffline");
        }

        @Override
        public void onUserMuteVideo(final int uid, final boolean muted) { // Tutorial Step 10

            GameControl.logD("onUserMuteVideo  " + muted + "  uid  =  " + uid + "  invisitUid = " + invisitUid);
            runOnUiThread(new Runnable() {
                @Override
                public void run() {

                    if (uid == Constants.broadCastUid) {
                        onRemoteUserVideoMuted(uid, muted);
                    } else if (muted) {
                        onRemoteSmallVideoLeft();

                    }

                    if (!muted) {

                        setUpSmallVideo(uid);
                    }
                }
            });
        }

        @Override
        public void onJoinChannelSuccess(String channel, int uid, int elapsed) {
            super.onJoinChannelSuccess(channel, uid, elapsed);
            GameControl.logD("onJoinChannelSuccess  channel  =  " + channel);
        }

        @Override
        public void onReceiveSEI(final String info) {
            super.onReceiveSEI(info);
            GameControl.logD("onReceiveSEI  = " + info);

            runOnUiThread(new Runnable() {
                @Override
                public void run() {
                    JSONObject jsonObject = null;
                    int sid = -1;
                    if (info != null) {
                        try {
                            jsonObject = new JSONObject(info);
                            // sid = jsonObject.g etInt("questionId");
                            JSONObject data = jsonObject.getJSONObject("data");

                            sid = data.getInt("questionId");
                        } catch (JSONException e) {
                            e.printStackTrace();
                        }

                        if (GameControl.currentQuestion != null) {
                            logD("sei   = " + sid + "   questionId  =  " + GameControl.currentQuestion.getId());

                            if (sid == GameControl.currentQuestion.getId() && isFirstTimeSEI) {
                                logD("sei  show");
                                showQuestion();
                                isFirstTimeSEI = false;
                            }
                        }
                    }
                }
            });
        }

        @Override
        public void onUserJoined(int uid, int elapsed) {
            super.onUserJoined(uid, elapsed);

            GameControl.logD("onUserJoined");
        }
    };

    private boolean isFirstTimeSEI = true;

    private boolean questionFlag = true;
    private Handler questionTimeHandler = new Handler() {
        @Override
        public void handleMessage(Message msg) {
            super.handleMessage(msg);

            switch (msg.what) {
                case 0:
                    int questime = (int) msg.obj;
                    //logD("questionTime  = " + questime);
                    GameControl.logD("questionReduceTime  =  " + questime);
                    if (questime < 0) {

                        questionTime = GameControl.timeOut;
                        return;
                    }
                    time_reduce.setTextColor(Color.RED);
                    time_reduce.setText(questime + " s");

                    if (questime == 0) {

                        questionFlag = false;
                        if (GameControl.serverWheatherCanPlay && GameControl.clientWheatherCanPlay) {
                            GameControl.clientWheatherCanPlay = false;
                            submitAnswer();
                        } else {
                            Toast.makeText(GameActivity.this, "you can not play", Toast.LENGTH_SHORT).show();
                        }
                        game_layout.setVisibility(View.GONE);
                        questionTime = GameControl.timeOut;
                    }
                    break;

                case 1:

                    if (game_layout.getVisibility() == View.VISIBLE) {
                        game_layout.setVisibility(View.GONE);
                        time_reduce.setTextColor(Color.RED);
                        time_reduce.setText(GameControl.timeOut + " s");
                        questionFlag = true;
                        isFirstTimeSEI = true;
                    }
                    break;

                case 2:
                    break;
            }
        }

    };

    private int questionTime;
    private ImageView gameResult;
    private void showQuestion() {
        executorService.execute(new Runnable() {
            @Override
            public void run() {
                questionTime = GameControl.timeOut;


                GameControl.logD("showQuestion questionFlag = " + questionFlag);
                while (questionFlag) {

                    try {
                        Thread.sleep(1000);
                    } catch (InterruptedException e) {
                        e.printStackTrace();
                    }

                    questionTime = questionTime - 1;
                    Message message = questionTimeHandler.obtainMessage();
                    message.what = 0;
                    message.obj = questionTime;
                    questionTimeHandler.sendMessage(message);

                    if (questionTime == -1) {
                        questionFlag = false;
                    }
                }
            }
        });

        GameActivity.this.runOnUiThread(new Runnable() {
            @Override
            public void run() {

                GameControl.logD("GameControl serverWheatherCanPlay = " + GameControl.serverWheatherCanPlay + "  GameControl.clientWheatherCanPlay  = " + GameControl.clientWheatherCanPlay);

                if (GameControl.serverWheatherCanPlay && GameControl.clientWheatherCanPlay) {
                    // logD("gameActivituy  =  " + "1111");

                    showquestionView(GameControl.currentQuestion);

                } else {
                    showquestionView(GameControl.currentQuestion);
                    try {
                        changeQuestionViewToRelive();
                    } catch (JSONException e) {
                        e.printStackTrace();
                    }
                }
            }
        });
    }

    private boolean checkSelfPermissions() throws Exception {
        return checkSelfPermission(Manifest.permission.RECORD_AUDIO, 0) &&
                checkSelfPermission(Manifest.permission.CAMERA, 1) &&
                checkSelfPermission(Manifest.permission.WRITE_EXTERNAL_STORAGE, 2);
    }


    public boolean checkSelfPermission(String permission, int requestCode) throws Exception {
        if (ContextCompat.checkSelfPermission(this,
                permission)
                != PackageManager.PERMISSION_GRANTED) {
            Log.d("zhang   ", "  permission != granted");

            ActivityCompat.requestPermissions(this,
                    new String[]{permission},
                    requestCode);
            return false;
        }

        if (Manifest.permission.CAMERA.equals(permission)) {
            // getApplication()).initWorkerThread();
            Log.d("zhang  ", "  camera permission");
            joinAgoraLiveChannel();
        }
        return true;
    }


    @Override
    public void onRequestPermissionsResult(int requestCode,
                                           @NonNull String permissions[], @NonNull int[] grantResults) {
        //  log.debug("onRequestPermissionsResult " + requestCode + " " + Arrays.toString(permissions) + " " + Arrays.toString(grantResults));
        switch (requestCode) {
            case 0: {
                if (grantResults.length > 0
                        && grantResults[0] == PackageManager.PERMISSION_GRANTED) {
                    try {
                        checkSelfPermission(Manifest.permission.CAMERA, 1);
                    } catch (Exception e) {
                        e.printStackTrace();
                    }
                } else {
                    // finish();

                }
                break;
            }
            case 1: {
                if (grantResults.length > 0
                        && grantResults[0] == PackageManager.PERMISSION_GRANTED) {
                    try {
                        checkSelfPermission(Manifest.permission.WRITE_EXTERNAL_STORAGE, 2);
                    } catch (Exception e) {
                        e.printStackTrace();
                    }
                    //  ((AGApplication) getApplication()).initWorkerThread();
                    try {
                        joinAgoraLiveChannel();
                    } catch (Exception e) {
                        e.printStackTrace();
                    }
                } else {
                    // finish();
                }
                break;
            }
            case 2: {
                if (grantResults.length > 0
                        && grantResults[0] == PackageManager.PERMISSION_GRANTED) {

                } else {
                    // finish();
                }
                break;
            }
        }
    }

    public void disConnectVideo(View view) {
        rtcEngine.muteLocalVideoStream(true);
        rtcEngine.setParameters("{\"rtc.hq_mode\": {\"hq\": true, \"broadcaster\":false, \"bitrate\":0}}");
        isInVideoWithBroadcast = false;
        FrameLayout frameLayout = findViewById(R.id.small_video);
        frameLayout.setVisibility(View.GONE);
        if (frameLayout.getChildCount() > 0) {
            frameLayout.removeAllViews();
        }

        disConnectBtn.setVisibility(View.GONE);
    }

    private void joinAgoraLiveChannel() throws Exception {

        initAgoraEngine();
        setupVideoProfile();
        //setUpLocalVideoCall();

        //joinChannel();
    }

    // Tutorial Step 2
    private void setupVideoProfile() {
        rtcEngine.enableVideo();
        rtcEngine.enableLocalVideo(false);
        rtcEngine.setVideoProfile(io.agora.rtc.Constants.VIDEO_PROFILE_360P, true);
    }


    private void initAgoraEngine() throws Exception {
        rtcEngine = RtcEngine.create(getApplicationContext(), Constants.AGORA_APP_ID, mRtcEventHandler);

        rtcEngine.setChannelProfile(io.agora.rtc.Constants.CHANNEL_PROFILE_LIVE_BROADCASTING);
        //rtcEngine.disableAudio();
        rtcEngine.setClientRole(io.agora.rtc.Constants.CLIENT_ROLE_BROADCASTER, null);
        //rtcEngine.setClientRole(io.agora.rtc.Constants.CLIENT_ROLE_BROADCASTER, null);
        rtcEngine.setParameters("{\"rtc.hq_mode\": {\"hq\": true, \"broadcaster\":false, \"bitrate\":0}}");
        rtcEngine.enableAudioVolumeIndication(1000, 3);
       // rtcEngine.setParameters("{\"rtc.log_filter\": 65535}");
        /*rtcEngine.enableVideo();
        rtcEngine.enableLocalVideo(false);
        rtcEngine.setVideoProfile(io.agora.rtc.Constants.VIDEO_PROFILE_360P, false);
*/
        GameControl.rtcEngine = new WeakReference<RtcEngine>(rtcEngine);
        GameControl.logD("channelName   account  = " + GameControl.currentUser.channelName + "   " + GameControl.currentUser.account);
        rtcEngine.joinChannel(null, GameControl.currentUser.channelName, "Extra Optional Data", Integer.parseInt(GameControl.currentUser.account)); // if you do not specify the uid, we will generate the uid for you
    }


    // Tutorial Step 5
    private void setupRemoteVideo(int uid) {
        FrameLayout container = (FrameLayout) findViewById(R.id.live_view);

        if (container.getChildCount() >= 1) {
            return;
        }
        SurfaceView surfaceView = RtcEngine.CreateRendererView(getBaseContext());
        container.addView(surfaceView);
        rtcEngine.setupRemoteVideo(new VideoCanvas(surfaceView, VideoCanvas.RENDER_MODE_HIDDEN, uid));
    }

    private void setUpSmallVideo(int uid) {

        FrameLayout container = findViewById(R.id.small_video);
        if (container.getChildCount() >= 1) {
            container.removeAllViews();
        }
        container.setVisibility(View.VISIBLE);
        SurfaceView surfaceView = RtcEngine.CreateRendererView(getBaseContext());
        surfaceView.setZOrderMediaOverlay(true);
        // isInVideoWithBroadcast = true;
        GameControl.logD("setUpSmallVideo isInVideoWithBroadcast  " + isInVideoWithBroadcast);
        container.addView(surfaceView);
        rtcEngine.setupRemoteVideo(new VideoCanvas(surfaceView, VideoCanvas.RENDER_MODE_HIDDEN, uid));
        //gameGangUpButton.setClickable(false);
        surfaceView.setTag(uid);
    }

    // Tutorial Step 6
    private void leaveChannel() {


        new Thread() {
            @Override
            public void run() {
                super.run();
                if (gangUpRtcEngine != null) {
                    gangUpRtcEngine.leaveChannel();
                }

                if (rtcEngine != null) {
                    rtcEngine.leaveChannel();
                }
            }
        }.start();
    }

    // Tutorial Step 7
    private void onRemoteUserLeft() {
        FrameLayout container = (FrameLayout) findViewById(R.id.live_view);
        container.removeAllViews();
    }

    private void onRemoteSmallVideoLeft() {
        FrameLayout container = (FrameLayout) findViewById(R.id.small_video);
        container.removeAllViews();
        container.setVisibility(View.GONE);
        GameControl.logD("onRemoteSmallVideoLeft isInVideoWithBroadcast = " + isInVideoWithBroadcast);
        gameGangUpButton.setClickable(true);
    }

    // Tutorial Step 10
    private void onRemoteUserVideoMuted(int uid, boolean muted) {
        FrameLayout container = (FrameLayout) findViewById(R.id.live_view);

        SurfaceView surfaceView = (SurfaceView) container.getChildAt(0);

        Object tag = surfaceView.getTag();
        if (tag != null && (Integer) tag == uid) {
            surfaceView.setVisibility(muted ? View.GONE : View.VISIBLE);
        }
    }

    // Step 11
    private void setUpLocalVideoCall() {
        FrameLayout frameLayout = findViewById(R.id.small_video);
        frameLayout.setVisibility(View.VISIBLE);
        if (frameLayout.getChildCount() > 0) {
            return;
        }

        isInVideoWithBroadcast = true;
        GameControl.logD("setUpLocalVideoCall  isInVideoWithBroadcast = " + isInVideoWithBroadcast);
        rtcEngine.setParameters("{\"rtc.hq_mode\": {\"hq\": true, \"broadcaster\":true, \"bitrate\":30}}");
        rtcEngine.enableLocalVideo(true);
        rtcEngine.muteLocalVideoStream(false);
        SurfaceView surfaceView = RtcEngine.CreateRendererView(GameActivity.this);
        rtcEngine.setupLocalVideo(new VideoCanvas(surfaceView, VideoCanvas.RENDER_MODE_HIDDEN, 0));

        surfaceView.setZOrderOnTop(true);
        surfaceView.setZOrderMediaOverlay(true);
        frameLayout.addView(surfaceView);
    }

    // private Button button_show;
    private LinearLayout game_layout;
    private Button submit_btn;
    private LinearLayout question_layout;
    // private ArrayList<String> questionData = new ArrayList<String>();
    private ArrayList<CheckBox> checkBox_item = new ArrayList<CheckBox>();
    private ArrayList<View> board = new ArrayList<View>();
    private TextView game_title;
    private TextView wheath_canPlay_TextView;
    private TextView time_reduce;

    private void initQuestionLayout() {
        // button_show = findViewById(R.id.show_question_btn);
        game_layout = findViewById(R.id.game_layout);
        submit_btn = findViewById(R.id.submit_button);
        game_layout.setVisibility(View.GONE);
        question_layout = findViewById(R.id.question_layout);
        game_title = findViewById(R.id.game_title);

        wheath_canPlay_TextView = findViewById(R.id.wheather_canplay_TextView);
        wheath_canPlay_TextView.setVisibility(View.GONE);
        time_reduce = findViewById(R.id.time_reduce);
        setSubmitbtnListener();
    }

    private void setSubmitbtnListener() {
        submit_btn.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                if (GameControl.serverWheatherCanPlay && GameControl.clientWheatherCanPlay) {
                    if (GameControl.currentQuestion != null) {
                        //    showquestionView(GameControl.currentQuestion);
                        submitAnswer();
                    }
                } else if ((!GameControl.serverWheatherCanPlay) | (!GameControl.clientWheatherCanPlay)) {
                    try {
                        buttonToRelive();
                    } catch (JSONException e) {
                        e.printStackTrace();
                    }
                }
            }
        });
    }

    private void submitAnswer() {
        questionFlag = false;
        // StringBuilder builder = new StringBuilder();
        int a = -1;
        for (int i = 0; i < checkBox_item.size(); i++) {

            CheckBox checkBox = checkBox_item.get(i);
            if (checkBox.isChecked()) {
                a = i;
            }
        }

        if (a == -1) {
            Toast.makeText(GameActivity.this, R.string.answer_is_null_message, Toast.LENGTH_SHORT).show();
        } else {
            Toast.makeText(GameActivity.this, R.string.choose_success_message, Toast.LENGTH_SHORT).show();
        }

        GameControl.logD("submit answer  url = " + Constants.HTTP_SEND_ANSWER_TO_SERVER);
        try {
            AgoraSignal.sendAnswerToserver(GameControl.currentQuestion.getId(), a, new HttpUrlUtils.OnResponse() {
                @Override
                public void onResponse(String data) {

                    // logD("sendAnswerToserver   = " + data);
                    GameControl.logD("sendAnswer OnResponse  =  " + data);
                }
            });
        } catch (JSONException e) {
            e.printStackTrace();
        }

        //question_layout.removeAllViews();
        checkBox_item.clear();
        board.clear();
        // questionData.clear();
        game_layout.setVisibility(View.GONE);
        questionTime = GameControl.timeOut;
    }

    private void setCheckBoxBackHighLight(int result) {
        checkBox_item.get(result).setBackgroundColor(Color.GREEN);
    }

    private ArrayList answerList;
    private void showquestionView(Question question) {
        //  int num = random.nextInt(10);
        //  logD("num  = "+num);
        answerList = new ArrayList();
        logD("c  :  " + question.getAnswerString().toString());
        logD("aanswer  = " + answerList.toString());
        question_layout.removeAllViews();
        checkBox_item.clear();
        board.clear();
        submit_btn.setVisibility(View.VISIBLE);

        answerList = GameControl.currentQuestion.getAnswerString();
        logD("showQuestionMid  : " + question.getAnswerString().toString());

        String title = question.getQuestion();
        game_title.setTextSize(20);
        int questionId = question.getId() + 1;
        String questionTitle = questionId + "   " + title;
        game_title.setText(questionTitle);
        game_title.setVisibility(View.VISIBLE);


        for (int i = 0; i < answerList.size(); i++) {
            if (i == 0) {
                View bo = createBoard();
                board.add(bo);
                question_layout.addView(bo);
            }
            CheckBox checkBox = createCheckBox((String) (answerList.get(i)), i);
            checkBox.setTextSize(20);
            checkBox_item.add(checkBox);
            question_layout.setDividerPadding(20);
            question_layout.addView(checkBox);
            if (i < (answerList.size())) {
                View bo = createBoard();
                board.add(bo);
                question_layout.addView(bo);
            }


        }
        game_layout.setVisibility(View.VISIBLE);
        wheath_canPlay_TextView.setVisibility(View.GONE);
   }

    private CheckBox createCheckBox(String text, int position) {

        // View view = game_layout.findViewById(R.id.question_layout);
        CheckBox box = new CheckBox(GameActivity.this);
        //GameControl.logD("text  = " + text);
        ViewGroup.MarginLayoutParams layoutParams = new ViewGroup.MarginLayoutParams(ViewGroup.LayoutParams.MATCH_PARENT, ViewGroup.LayoutParams.WRAP_CONTENT);
        layoutParams.setMargins(0, 15, 0, 15);
        box.setText(text);
        box.setTextColor(Color.BLACK);
        box.setTag(position);
        box.setLayoutParams(layoutParams);
        box.setOnCheckedChangeListener(new CompoundButton.OnCheckedChangeListener() {
            @Override
            public void onCheckedChanged(CompoundButton buttonView, boolean isChecked) {
                if (isChecked) {
                    for (int i = 0; i < checkBox_item.size(); i++) {
                        if (checkBox_item.get(i) != buttonView) {
                            checkBox_item.get(i).setChecked(false);
                        }
                    }
                }
            }
        });
        return box;
    }

    private void getCorrectCheckBox(int positions) {

        int question_count = question_layout.getChildCount();
        GameControl.logD("getCorrectCheckBoxitem =  " + question_count);
        for (int i = 0; i < question_count; i++) {
            View view = question_layout.getChildAt(i);
            String tag = view.getTag() + "";
            if (tag.equals(positions + "")) {
                // GameControl.logD("correctChild  =  setBackGround");
                view.setBackgroundColor(Color.GREEN);
            }
        }

    }

    private View createBoard() {
        View view = new View(GameActivity.this);
        LinearLayout.LayoutParams params = new LinearLayout.LayoutParams(
                LinearLayout.LayoutParams.MATCH_PARENT, 1);
        // params.setMargins(left, top, right, bottom);
        params.setMargins(0, 20, 0, 20);
        // view.setBackgroundColor(Color.BLACK);
        view.setLayoutParams(params);
        return view;
    }

    private void changeQuestionViewToRelive() throws JSONException {
        showquestionView(GameControl.currentQuestion);
        wheath_canPlay_TextView.setVisibility(View.VISIBLE);
        wheath_canPlay_TextView.setText(R.string.game_cannot_play);
        wheath_canPlay_TextView.setTextSize(16);
        wheath_canPlay_TextView.setTextColor(Color.RED);
        submit_btn.setText(R.string.relive_message);
        submit_btn.setTextColor(Color.RED);
        game_layout.setVisibility(View.VISIBLE);
        game_title.setVisibility(View.VISIBLE);
    }

    private void buttonToRelive() throws JSONException {
        AgoraSignal.checkRelive(new HttpUrlUtils.OnResponse() {
            @Override
            public void onResponse(String data) throws JSONException {
                // logD("checkWheatherCanPlay -------------");

                //logD(data + "");
                GameControl.logD("reliveButton  onResponse =  " + data);
                if (data.equals(Constants.MESSAGE_TOAST)) {
                    System.out.println(Toast.makeText(GameActivity.this, R.string.connect_net_error_or_server_error, Toast.LENGTH_SHORT));
                }

                if (data.equals("{}")) {
                    // logD("checkWheatherCanPlay");
                    GameControl.serverWheatherCanPlay = true;
                    GameControl.clientWheatherCanPlay = true;

                    GameActivity.this.runOnUiThread(new Runnable() {
                        @Override
                        public void run() {
                            wheath_canPlay_TextView.setVisibility(View.GONE);
                            submit_btn.setText(R.string.submit_message);
                            submit_btn.setTextColor(Color.BLACK);
                            Toast.makeText(GameActivity.this, R.string.relive_success_message, Toast.LENGTH_SHORT).show();
                            showquestionView(GameControl.currentQuestion);
                            logD("GameControl CurrentQuestion:  " + GameControl.currentQuestion.getAnswerString().toString());
                        }
                    });
                } else {
                    GameControl.serverWheatherCanPlay = false;
                    Toast.makeText(GameActivity.this, R.string.fail_to_relive_message, Toast.LENGTH_SHORT).show();
                }
            }
        });
    }

    private EditText gang_up_channel_name;
    private Button createGangUpChannel;
    private Button joinGangUpChannel;
    private AlertDialog gangUpAlertDialog;
    private RtcEngine gangUpRtcEngine;
    private boolean isInGangUp;
    private boolean wheatherCanPlayGame;

    IRtcEngineEventHandler gangUpRtcEngineEventHandler = new IRtcEngineEventHandler() {
        @Override
        public void onJoinChannelSuccess(String channel, final int uid, int elapsed) {
            super.onJoinChannelSuccess(channel, uid, elapsed);
            GameControl.logD("sub onJoinChannelSuccess gangUp  =  " + channel);
            runOnUiThread(new Runnable() {
                @Override
                public void run() {
                    gameGangUpButton.setClickable(true);
                    isInGangUp = true;

                    rtcEngine.setEnableSpeakerphone(true);
                    gameGangUpButton.setText(R.string.leave_gang_up_room);
                    gangUpUidRecycleView.setVisibility(View.VISIBLE);

                    gangUpRecycleViewAdapter.addGangUpUidList(uid + "");

                    GameControl.logD("sub subEngine Thread name = " + Thread.currentThread());
                }
            });
        }


        @Override
        public void onLeaveChannel(RtcStats stats) {
            super.onLeaveChannel(stats);
            GameControl.logD("sub onLeaveChannel gangUp  =  ");
            runOnUiThread(new Runnable() {
                @Override
                public void run() {

                    gameGangUpButton.setClickable(true);
                    toastHelper(getString(R.string.leave_gang_up_room_success));
                    isInGangUp = false;
                    gameGangUpButton.setText(R.string.gang_up_string);
                    gangUpUidRecycleView.setVisibility(View.GONE);
                    gangUpRecycleViewAdapter.clearGangUpUidList();

                }
            });
        }

        @Override
        public void onUserJoined(final int uid, int elapsed) {
            super.onUserJoined(uid, elapsed);
            GameControl.logD("sub onUserJoined uid  =  " + uid);
            runOnUiThread(new Runnable() {
                @Override
                public void run() {
                    gangUpRecycleViewAdapter.addGangUpUidList(uid + "");

                }
            });
        }

        @Override
        public void onUserOffline(final int uid, int reason) {
            super.onUserOffline(uid, reason);
            GameControl.logD("sub onUserOffline uid  =  " + uid);
            runOnUiThread(new Runnable() {
                @Override
                public void run() {
                    gangUpRecycleViewAdapter.removeUidFromGangUpList(uid + "");

                }
            });
        }


    };

    private void initGangUpRtcEngine() {

        try {
            gangUpRtcEngine = RtcEngine.create(getApplicationContext(), Constants.AGORA_APP_ID, gangUpRtcEngineEventHandler);
            GameControl.gangUpRtcEngine = new WeakReference<RtcEngine>(gangUpRtcEngine);
            GameControl.logD("sub initGangUpRtcEngine");
        } catch (Exception e) {
            e.printStackTrace();
        }
    }

    private void showInvitationDialog() {
        AlertDialog.Builder builder = new AlertDialog.Builder(GameActivity.this);
        View gang_up_view = View.inflate(GameActivity.this, R.layout.invisit_video_dialog_layout, null);
        Button acceptVideoInvationBtn = gang_up_view.findViewById(R.id.accept_video_invitation);
        //*createGangUpChannel = gang_up_view.findViewById(R.id.create_gang_up_channel);
        Button refuseVideoInvationBtn = gang_up_view.findViewById(R.id.refuse_video_invitation);
        //*
        builder.setView(gang_up_view);
        builder.setCancelable(false);
        AlertDialog dialog = builder.create();
        gangUpAlertDialog = dialog;
        acceptVideoInvationBtn.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                if (isInGangUp) {
                    toastHelper("Your are in Team Mode");
                    gangUpAlertDialog.dismiss();
                } else {
                    setUpLocalVideoCall();
                    gangUpAlertDialog.dismiss();
                    disConnectBtn.setVisibility(View.VISIBLE);
                    disConnectBtn.setClickable(true);
                }
            }
        });

        refuseVideoInvationBtn.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                gangUpAlertDialog.dismiss();
            }
        });
        dialog.show();
    }

    public void gangUpClick(View view) {
        GameControl.logD("gangUpClick  isInVideoWithBroadcast = " + isInVideoWithBroadcast);

        if (isInVideoWithBroadcast) {
            toastHelper(getString(R.string.can_not_team_mode));
            return;
        }

        if (isInGangUp) {
            gangUpRtcEngine.setParameters("{\"rtc.hq_mode\": {\"hq\": true, \"broadcaster\":false, \"bitrate\":0}}");
            gangUpRtcEngine.leaveChannel();
            gameGangUpButton.setClickable(false);
            // rtcEngine.disableAudio();
        } else {

            AlertDialog.Builder builder = new AlertDialog.Builder(GameActivity.this);

            View gang_up_view = View.inflate(GameActivity.this, R.layout.gang_up_layout, null);
            gang_up_channel_name = gang_up_view.findViewById(R.id.gang_up_channel_name_editText);
            /*createGangUpChannel = gang_up_view.findViewById(R.id.create_gang_up_channel);
            joinGangUpChannel = gang_up_view.findViewById(R.id.join_gang_up_channel);
            */
            builder.setView(gang_up_view);
            builder.setCancelable(true);
            AlertDialog dialog = builder.create();
            gangUpAlertDialog = dialog;
            dialog.show();
        }
    }

    public void createGangUpChannelClick(View view) {
        if (gangUpRtcEngine == null) {
            initGangUpRtcEngine();
        }
        String channelName = getGangUpChannelName();


        if (TextUtils.isEmpty(channelName)) {
            toastHelper("Channel name can not be null");
        } else {
            toastHelper("Create Channel =  " + channelName);
            if (gangUpAlertDialog != null) {
                gangUpAlertDialog.dismiss();
            }

            isJoinGangUpChannel = false;
            gangUpRtcEngine.enableAudio();
            //rtcEngine.enableAudio();

            String realChannelName = GameControl.currentUser.channelName + "_" + channelName;
            gangUpRtcEngine.setParameters("{\"rtc.hq_mode\": {\"hq\": true, \"broadcaster\":true, \"bitrate\":50}}");
            gangUpRtcEngine.joinChannel(null, realChannelName, "Extra Optional Data", Integer.parseInt(GameControl.currentUser.account)); // if you do not specify the uid, we will generate the uid for you

        }
    }

    private boolean isJoinGangUpChannel;

    public void joinGangUpChannel(View view) {

        if (gangUpRtcEngine == null) {
            initGangUpRtcEngine();
        }

        String channelName = getGangUpChannelName();
        if (TextUtils.isEmpty(channelName)) {
            toastHelper("Channel name can not be null");
        } else {
            toastHelper(" Joian Channel = " + channelName);
            if (gangUpAlertDialog != null) {
                gangUpAlertDialog.dismiss();
                isJoinGangUpChannel = true;
                gangUpRtcEngine.enableAudio();
                //rtcEngine.enableAudio();

                String realChannelName = GameControl.currentUser.channelName + "_" + channelName;
                gangUpRtcEngine.setParameters("{\"rtc.hq_mode\": {\"hq\": true, \"broadcaster\":true, \"bitrate\":50}}");

                gangUpRtcEngine.joinChannel(null, realChannelName, "Extra Optional Data", Integer.parseInt(GameControl.currentUser.account)); // if you do not specify the uid, we will generate the uid for you

            }
        }
    }

    private String getGangUpChannelName() {
        String gangUpChannelName = gang_up_channel_name.getText().toString();
        return gangUpChannelName;
    }
}
