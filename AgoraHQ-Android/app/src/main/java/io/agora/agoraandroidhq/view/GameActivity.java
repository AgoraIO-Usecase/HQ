package io.agora.agoraandroidhq.view;

import android.Manifest;
import android.app.Activity;
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

import java.util.ArrayList;
import java.util.Random;

import io.agora.agoraandroidhq.R;
import io.agora.agoraandroidhq.control.AgoraSignal;
import io.agora.agoraandroidhq.control.JsonToString;
import io.agora.agoraandroidhq.module.Question;
import io.agora.agoraandroidhq.module.Result;
import io.agora.agoraandroidhq.module.User;
import io.agora.agoraandroidhq.tools.Constants;
import io.agora.agoraandroidhq.tools.GameControl;
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


    @Override
    protected void onCreate(@Nullable Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        this.requestWindowFeature(Window.FEATURE_NO_TITLE);

        setContentView(R.layout.paly_game);
        this.getWindow().setFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN, WindowManager.LayoutParams.FLAG_FULLSCREEN);
        setRequestedOrientation(ActivityInfo.SCREEN_ORIENTATION_PORTRAIT);


        //joinChatRoom(Constants.chatRoom);
        //joinChatRoom(Constants.questionRoom);
        try {
            init();
        } catch (Exception e) {
            e.printStackTrace();
        }


    }


    private User getUser() {

        User currentUser = new User();
        String channelName = getChannelName();
        currentUser.setChannelName(channelName);
        GameControl.currentUser = currentUser;

        return currentUser;
    }

    private void init() throws Exception {
        loginAgoraSignal();
        GameControl.logD("init");
        findView();
        //AgoraLinkToCloud.addEventHandler(handler);
        //joinChannel();

        initQuestionLayout();

        //startCheckWheatherCanPlay();

        GameControl.controlCheckThread = true;

        getUser();


        checkSelfPermissions();
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
                    GameControl.logD("AGORA_SIGNAL_RECEIVE   =  " + mess);
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
                                if (correct == 0) {
                                    time_reduce.setText(getString(R.string.answer_error_message) + res);

                                    time_reduce.setTextColor(Color.RED);
                                    time_reduce.setVisibility(View.VISIBLE);
                                    //game_title.setVisibility(View.INVISIBLE);
                                    GameControl.clientWheatherCanPlay = false;
                                } else {
                                    time_reduce.setText(R.string.answer_correct_message);
                                    time_reduce.setTextColor(Color.RED);
                                    time_reduce.setVisibility(View.VISIBLE);
                                    //game_title.setVisibility(View.INVISIBLE);
                                    GameControl.clientWheatherCanPlay = true;
                                }
                                questionTimeHandler.sendEmptyMessageDelayed(1, 5000);
                                //time_reduce.setVisibility(View.INVISIBLE);
                                game_layout.setVisibility(View.VISIBLE);
                                submit_btn.setVisibility(View.GONE);
                            }

                            logD(" Serverwheather " + GameControl.serverWheatherCanPlay + "  " + "ClientServer  " + GameControl.clientWheatherCanPlay);

                            break;


                        case "quiz":
/*
                            try {
                                checkWheatherCanPlay();
                            } catch (JSONException e) {
                                e.printStackTrace();
                            }*/
                            Question question = (Question) jsonObject;
                            ;

                            /*if (GameControl.clientWheatherCanPlay && GameControl.serverWheatherCanPlay) {

                                showquestionView(question);

                            }*/
                            if (question != null) {

                                GameControl.setCurrentQuestion(question);
                            }

                            logD("quiz   showQuestion");
                            //showQuestion();
                            //showquestionView(GameControl.currentQuestion);
                            break;

                    }
                    break;

                case Constants.AGORA_SIGNAL_SEND:
                    // TextMessage sendContent = (TextMessage) msg.obj;

                    // String sendName = sendContent.getUserInfo().getUserId();
                    // String messge = sendContent.getContent();
                    //logD("handleMessage   = " + messge);

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
        new Thread() {
            @Override
            public void run() {
                super.run();
                while (GameControl.controlCheckThread) {
                    GameControl.logD("startCheckWheatherCanPalyThread");
                    try {
                        Thread.sleep(15000);
                    } catch (InterruptedException e) {
                        e.printStackTrace();
                    }
                    try {
                        checkWheatherCanPlay();
                    } catch (JSONException e) {
                        e.printStackTrace();
                    }
                }
            }
        }.start();
    }


    private RecyclerView recyclerView;
    private MessageRecyclerViewAdapter messageRecyclerViewAdapter;
    private LinearLayout messageLinearLayou;
    private EditText input_editor;
    private TextView sendButton;
    private ImageView imageViewBack;
    private ImageView sendMessageImage;
    private FrameLayout game_view_layout;
    private boolean wheatherHasFocus = false;

    private void findView() {
        imageViewBack = findViewById(R.id.back_image);
        imageViewBack.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                //TODO


                GameActivity.this.finish();
            }
        });

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


        GameControl.controlCheckThread = false;

        agoraSignal.removeEnventHandler();
        agoraSignal.onLogoutSDKClick();

        leaveChannel();

    }


    private RtcEngine rtcEngine;


    private final IRtcEngineEventHandler mRtcEventHandler = new IRtcEngineEventHandler() { // Tutorial Step 1
        @Override
        public void onFirstRemoteVideoDecoded(final int uid, int width, int height, int elapsed) { // Tutorial Step 5
            // logD("onFirstRemoteVideoDecode");
            GameControl.logD("onFirstRemoteVideoDecoded");
            runOnUiThread(new Runnable() {
                @Override
                public void run() {
                    setupRemoteVideo(uid);
                }
            });
        }


        @Override
        public void onUserOffline(int uid, int reason) { // Tutorial Step 7
            logD("onUserOffline");
            runOnUiThread(new Runnable() {
                @Override
                public void run() {
                    onRemoteUserLeft();
                }
            });
            GameControl.logD("onUserOffline");
        }

        @Override
        public void onUserMuteVideo(final int uid, final boolean muted) { // Tutorial Step 10

            GameControl.logD("onUserMuteVideo");
            runOnUiThread(new Runnable() {
                @Override
                public void run() {
                    onRemoteUserVideoMuted(uid, muted);
                }
            });
        }

        @Override
        public void onJoinChannelSuccess(String channel, int uid, int elapsed) {
            GameControl.logD("onJoinChannelSuccess");
            super.onJoinChannelSuccess(channel, uid, elapsed);
            logD("onJOinChannelSuccess");
        }

        @Override
        public void onReceiveSEI(String info) {
            super.onReceiveSEI(info);
            GameControl.logD("onReceiveSEI  = " + info);

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

                logD("sei   = " + sid + "   questionId  =  " + GameControl.currentQuestion.getId());

                if (sid == GameControl.currentQuestion.getId()) {
                    logD("sei  show");
                    showQuestion();
                }
            }
        }
    };


    private boolean questionFlag = true;
    private Handler questionTimeHandler = new Handler() {
        @Override
        public void handleMessage(Message msg) {
            super.handleMessage(msg);

            switch (msg.what) {
                case 0:
                    int questime = (int) msg.obj;
                    logD("questionTime  = " + questime);
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
                        questionTime = 10;
                    }
                    break;

                case 1:
                    if (game_layout.getVisibility() == View.VISIBLE) {
                        game_layout.setVisibility(View.GONE);
                        time_reduce.setText(10 + "");
                    }
                    break;
            }
        }

    };

    private int questionTime = 10;

    private void showQuestion() {

        new Thread() {
            @Override
            public void run() {
                questionTime = 10;

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
        }.start();


        logD("  runOnUiThread   ");
        questionFlag = true;
        GameActivity.this.runOnUiThread(new Runnable() {
            @Override
            public void run() {
                if (GameControl.serverWheatherCanPlay && GameControl.clientWheatherCanPlay) {
                    logD("gameActivituy  =  " + "1111");
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

        Log.d("zhang   ", "checkSelfPermissionssss");
        return checkSelfPermission(Manifest.permission.RECORD_AUDIO, 0) &&
                checkSelfPermission(Manifest.permission.CAMERA, 1) &&
                checkSelfPermission(Manifest.permission.WRITE_EXTERNAL_STORAGE, 2);
    }


    public boolean checkSelfPermission(String permission, int requestCode) throws Exception {
        // log.debug("checkSelfPermission " + permission + " " + requestCode);
        if (ContextCompat.checkSelfPermission(this,
                permission)
                != PackageManager.PERMISSION_GRANTED) {
            Log.d("zhang   ", "  permission != granted");

            ActivityCompat.requestPermissions(this,
                    new String[]{permission},
                    requestCode);
            return false;
        }

        Log.d("zhang  ", " permission  granted");

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


    private void joinAgoraLiveChannel() throws Exception {
        initAgoraEngine();
        setupVideoProfile();
        //joinChannel();
    }

    // Tutorial Step 2
    private void setupVideoProfile() {
        rtcEngine.enableVideo();
        rtcEngine.setVideoProfile(io.agora.rtc.Constants.VIDEO_PROFILE_360P, false);
    }

    /*// Tutorial Step 3
    private void setupLocalVideo() {
        FrameLayout container = (FrameLayout) findViewById(R.id.local_video_view_container);
        SurfaceView surfaceView = RtcEngine.CreateRendererView(getBaseContext());
        surfaceView.setZOrderMediaOverlay(true);
        container.addView(surfaceView);
        rtcEngine.setupLocalVideo(new VideoCanvas(surfaceView, VideoCanvas.RENDER_MODE_ADAPTIVE, 0));
    }*/


    private void initAgoraEngine() throws Exception {
        rtcEngine = RtcEngine.create(getBaseContext(), Constants.AGORA_APP_ID, mRtcEventHandler);
        rtcEngine.setChannelProfile(io.agora.rtc.Constants.CHANNEL_PROFILE_LIVE_BROADCASTING);
        rtcEngine.setClientRole(io.agora.rtc.Constants.CLIENT_ROLE_AUDIENCE, null);
        GameControl.logD("channelName   account  = " + GameControl.currentUser.channelName + "   " + GameControl.currentUser.account);
        rtcEngine.joinChannel(null, GameControl.currentUser.channelName, "Extra Optional Data", Integer.parseInt(Constants.UID)); // if you do not specify the uid, we will generate the uid for you
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

        surfaceView.setTag(uid); // for mark purpose
       /* View tipMsg = findViewById(R.id.quick_tips_when_use_agora_sdk); // optional UI
        tipMsg.setVisibility(View.GONE);*/
    }

    // Tutorial Step 6
    private void leaveChannel() {
        if (rtcEngine != null) {
            rtcEngine.leaveChannel();
            RtcEngine.destroy();
        }
    }

    // Tutorial Step 7
    private void onRemoteUserLeft() {
        FrameLayout container = (FrameLayout) findViewById(R.id.live_view);
        container.removeAllViews();
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


    // private Button button_show;
    private LinearLayout game_layout;
    private Button submit_btn;
    private LinearLayout question_layout;

    private ArrayList<String> questionData = new ArrayList<String>();
    private ArrayList<CheckBox> checkBox_item = new ArrayList<CheckBox>();

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


    /*@Override
    public void onClick(View v) {
        switch (v.getId()) {
            case R.id.submit_button:

                submitAnswer();


                break;

        }
    }*/

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

        //  Toast.makeText(GameActivity.this, builder.toString(), Toast.LENGTH_SHORT).show();

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
        // questionData.clear();
        game_layout.setVisibility(View.GONE);
        questionTime = 10;

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
        submit_btn.setVisibility(View.VISIBLE);

        answerList = GameControl.currentQuestion.getAnswerString();
        logD("showQuestionMid  : " + question.getAnswerString().toString());

        String title = question.getQuestion();
        game_title.setTextSize(20);

        String questionTitle = question.getId() + " " + title;
        game_title.setText(questionTitle);
        game_title.setVisibility(View.VISIBLE);

        for (int i = 0; i < answerList.size(); i++) {
            CheckBox checkBox = createCheckBox((String) (answerList.get(i)), i);
            checkBox.setTextSize(25);
            checkBox_item.add(checkBox);
            question_layout.setDividerPadding(10);
            question_layout.addView(checkBox);

        }
        game_layout.setVisibility(View.VISIBLE);
        wheath_canPlay_TextView.setVisibility(View.GONE);

        logD("showQuestionend   : " + question.getAnswerString().toString());
        logD("showQuestionend   :" + checkBox_item.size() + " ------  question  ---" + game_layout.getVisibility() + "   " + View.VISIBLE);
        time = 10;
    }


    private int time = 10;

    private CheckBox createCheckBox(String text, int position) {

        // View view = game_layout.findViewById(R.id.question_layout);
        CheckBox box = new CheckBox(GameActivity.this);
        ViewGroup.LayoutParams layoutParams = new ViewGroup.LayoutParams(ViewGroup.LayoutParams.MATCH_PARENT, ViewGroup.LayoutParams.WRAP_CONTENT);
        box.setText(position + " " + text);
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


    private void changeQuestionViewToRelive() throws JSONException {
        showquestionView(GameControl.currentQuestion);
        wheath_canPlay_TextView.setVisibility(View.VISIBLE);
        wheath_canPlay_TextView.setText("you can not play,because you are already die!");
        wheath_canPlay_TextView.setTextColor(Color.RED);
        submit_btn.setText(R.string.relive_message);
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

    /*private Handler handler = new Handler(){
        @Override
        public void handleMessage(Message msg) {
            super.handleMessage(msg);
            switch (msg.what){

                case Constants.MESSAGE_TOAST:

            }
        }
    };*/


}
