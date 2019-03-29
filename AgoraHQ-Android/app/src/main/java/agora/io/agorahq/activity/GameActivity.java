package agora.io.agorahq.activity;

import android.app.AlertDialog;
import android.content.Context;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.support.v7.widget.LinearLayoutManager;
import android.support.v7.widget.RecyclerView;
import android.text.TextUtils;
import android.view.SurfaceView;
import android.view.View;
import android.view.ViewGroup;
import android.view.animation.Animation;
import android.view.animation.AnimationUtils;
import android.widget.EditText;
import android.widget.FrameLayout;
import android.widget.ImageView;
import android.widget.TextView;

import org.json.JSONException;

import java.lang.ref.WeakReference;
import java.util.ArrayList;
import java.util.List;
import java.util.Timer;
import java.util.TimerTask;

import agora.io.agorahq.R;
import agora.io.agorahq.bean.Answer;
import agora.io.agorahq.bean.QuestionAnswerResult;
import agora.io.agorahq.bean.ChatMessage;
import agora.io.agorahq.bean.PlayState;
import agora.io.agorahq.bean.Question;
import agora.io.agorahq.bean.Reset;
import agora.io.agorahq.logicworker.IMediaEventHandler;
import agora.io.agorahq.logicworker.IRtmEventHandler;
import agora.io.agorahq.logicworker.ServerLogic;
import agora.io.agorahq.ui.AnswerBoardDialog;
import agora.io.agorahq.ui.ChatMessageAdapter;
import agora.io.agorahq.ui.DefaultDecoration;
import agora.io.agorahq.ui.QuestionBoardDialog;
import agora.io.agorahq.utils.Constants;
import agora.io.agorahq.utils.HQJson2StrUtil;
import agora.io.agorahq.utils.HttpUtil;
import agora.io.agorahq.utils.LogUtil;
import io.agora.rtc.IRtcEngineEventHandler;
import io.agora.rtc.RtcEngine;
import io.agora.rtc.video.VideoCanvas;
import io.agora.rtm.ErrorInfo;

public class GameActivity extends BaseActivity implements IMediaEventHandler, IRtmEventHandler, QuestionBoardDialog.DialogListener, View.OnClickListener, AnswerBoardDialog.AnswerBoardFinishedListener {
    private QuestionBoardDialog mQuestionBoardDialog;
    private AnswerBoardDialog mAnswerBoardDialog;

    private LogUtil mLogUtils;

    private FrameLayout mFlBigVideoView;
    private RecyclerView mRcChatMessage;

    private EditText mEtChatMessage;
    private ImageView mIvChatMessageSend;

    private ChatMessageAdapter mChatMessageAdapter;

    private List<ChatMessage> mChatMessageDataset;

    private SurfaceView mSvRemoteView;

    private FrameLayout mFlCongratulationView;
    private TextView mTvCongratulationName;
    private ImageView mIvCongratulationIcon;
    private ImageView mIvCongratulationCancel;

    private LogicHandler mHandler;

    private final static int ANSWER_BOARD_DISMISSED = 0x9001;
    private final static int RECEIVED_CHANNEL_MESSAGE = 0x9002;
    private final static int RECEIVED_SEI = 0x9003;
    private final static int QUESTION_BOARD_DISMISSED = 0x9004;
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_game);
    }

    @Override
    protected void initUIandEvent() {
        initEngine();
        initWidget();
        initQuestionBoard();
    }

    @Override
    protected void deInitUIandEvent() {
        releaseRes();
    }

    // <------------------------------------- UI BLOCK ---------------------------------->

    public void initWidget() {
        mFlBigVideoView = findViewById(R.id.fl_video_container);

        mRcChatMessage = findViewById(R.id.rv_chat_message_view);
        mChatMessageDataset = new ArrayList<>();
        mChatMessageAdapter = new ChatMessageAdapter(new WeakReference<Context>(this), mChatMessageDataset);
        mChatMessageAdapter.setHasStableIds(true);
        mRcChatMessage.setAdapter(mChatMessageAdapter);
        mRcChatMessage.setLayoutManager(new LinearLayoutManager(this, RecyclerView.VERTICAL, false));
        mRcChatMessage.addItemDecoration(new DefaultDecoration());

        mEtChatMessage = findViewById(R.id.et_game_chat_message_content);

        mIvChatMessageSend = findViewById(R.id.iv_game_chat_send);
        mIvChatMessageSend.setTag(false);
        mIvChatMessageSend.setOnClickListener(this);

        mFlCongratulationView = findViewById(R.id.fl_game_congratulation_view);
        mFlCongratulationView.setVisibility(View.INVISIBLE);

        mTvCongratulationName = findViewById(R.id.tv_game_congratulation_name);

        mIvCongratulationCancel = findViewById(R.id.iv_game_congratulation_cancel);
        mIvCongratulationCancel.setOnClickListener(this);

        mIvCongratulationIcon = findViewById(R.id.iv_game_congratulation_headImage);

        mLogUtils.log("init widgets");
    }

    public void onBackClicked(View v) {
        finish();
    }

    @Override
    public void onClick(View v) {
        switch (v.getId()) {
            case R.id.iv_game_chat_send:
                if (!(boolean) mIvChatMessageSend.getTag()) {
                    mIvChatMessageSend.setTag(true);
                    mIvChatMessageSend.setImageResource(R.drawable.icon_send_message);
                    mEtChatMessage.setVisibility(View.VISIBLE);
                } else {
                    if (TextUtils.isEmpty(mEtChatMessage.getText().toString().trim())) {
                        showShortToast("send message should not be null!");
                        return;
                    }

                    sendMessage(mEtChatMessage.getText().toString(), true, gameInfo().getUser().getUserName());
                    String message = HQJson2StrUtil.sendMessageString(mEtChatMessage.getText().toString(), gameInfo().getUser().getUserName());

                    worker().sendSignalChannelMessage(message);

                    mEtChatMessage.setText("");
                    mEtChatMessage.requestFocus();
                }
                break;
            case R.id.iv_game_congratulation_cancel:
                cancelCongraAnim();
                break;
            default:
                break;
        }
    }

    public void sendMessage(final String message, final boolean isMe, final String account) {
        runOnUiThread(new Runnable() {
            @Override
            public void run() {
                ChatMessage cm = new ChatMessage(account, message, isMe);
                mChatMessageDataset.add(cm);

                int MAX_MESSAGE_COUNT = 24;
                if (mChatMessageDataset.size() > MAX_MESSAGE_COUNT) {
                    int toRemove = mChatMessageDataset.size() - MAX_MESSAGE_COUNT;
                    for (int i = 0; i < toRemove; i++) {
                        mChatMessageDataset.remove(i);
                    }
                }

                mChatMessageAdapter.notifyDataSetChanged();

                mRcChatMessage.scrollToPosition(mChatMessageDataset.size() - 1);
            }
        });

    }

    public void showCanPlayBoard() {
        AlertDialog.Builder builder = new AlertDialog.Builder(GameActivity.this, AlertDialog.THEME_DEVICE_DEFAULT_LIGHT);
        View view = View.inflate(GameActivity.this, R.layout.remote_play_status_layout, null);
        builder.setView(view);
        builder.setCancelable(true);
        final AlertDialog dialog = builder.create();
        dialog.setCancelable(false);
        dialog.show();

        new Timer().schedule(new TimerTask() {
            @Override
            public void run() {
                dialog.dismiss();
            }
        }, 2000);
    }

    // <------------------------------------- FUNCTION BLOCK ---------------------------------->
    public void initEngine() {
        mediaHanlder().addMediaEventHandler(this);
        rtmHandler().addSignalingEventHandler(this);

        worker().joinChannel(gameInfo().getUser().getChannelName());
        worker().loginSignaling(gameInfo().getUser().getSignalAccount(), gameInfo().getUser().getChannelName());

        mLogUtils = LogUtil.newInstance();
        mLogUtils.log("--Init GameEngine--");
    }

    public void initQuestionBoard() {
        mQuestionBoardDialog = new QuestionBoardDialog();
        mAnswerBoardDialog = new AnswerBoardDialog();
        mLogUtils.log("--Init QuestionBoard--");
    }


    public void checkRemotePlayStatus() {
        try {
            ServerLogic.sendServerLocalJoin(new HttpUtil.OnResponse() {
                @Override
                public void onResponse(String data) {
                    Object obj = HQJson2StrUtil.resolvePlayStatus(data);
                    if ((obj instanceof Boolean) && (gameInfo().getPlayState() == PlayState.NONE)) {
                        gameInfo().setRemotePlayStatus(true);
                        gameInfo().updateLocalPlayStatus(true);
                        gameInfo().updatePlayState(PlayState.WAIT_FOR_PLAY);
                        mLogUtils.log("--wait for play start--");
                    } else {
                        gameInfo().setRemotePlayStatus(false);
                        gameInfo().updateLocalPlayStatus(false);
                        gameInfo().updatePlayState(PlayState.WAIT_FOR_PLAY);
                        showShortToast(obj.toString());

                        runOnUiThread(new Runnable() {
                            @Override
                            public void run() {
                                showCanPlayBoard();
                            }
                        });

                        mLogUtils.log("--you can not play because:" + obj.toString());
                    }
                }
            }, gameInfo().getUser().getSignalAccount(), gameInfo().getUser().getChannelName());
        } catch (JSONException e) {
            e.printStackTrace();
        }
    }

    private void startCongraAnim() {
        if (gameInfo().getAnswer() != null && gameInfo().getQuestion() != null) {
            if (gameInfo().getLocalPlayStatus() && (gameInfo().getQuestion().getTotalQuestion() == gameInfo().getAnswer().getSid() + 1)) {
                mTvCongratulationName.setText(gameInfo().getUser().getUserName());
                mIvCongratulationIcon.setImageDrawable(gameInfo().getUser().getDrawable());
                Animation animationUtils = AnimationUtils.loadAnimation(GameActivity.this, R.anim.congra_anim);
                animationUtils.setFillAfter(true);
                mFlCongratulationView.startAnimation(animationUtils);
                mFlCongratulationView.setVisibility(View.VISIBLE);
            }
        }
    }

    private void cancelCongraAnim() {
        if (mFlCongratulationView != null) {
            mFlCongratulationView.clearAnimation();
            mFlCongratulationView.setVisibility(View.INVISIBLE);
        }
    }

    public void releaseRes() {
        gameInfo().updatePlayState(PlayState.NONE);
        gameInfo().updateQuestion(null);
        gameInfo().updateLocalPlayStatus(true);
        gameInfo().updateAnswer(null);

        worker().leaveChannel();
        worker().signalLogout();

        mChatMessageDataset.clear();
        cancelCongraAnim();

        mLogUtils.log("release Res");
    }

    // <------------------------------------- ENGINE CALLBACK ---------------------------------->
    @Override
    public void onFirstRemoteVideoDecoded(int uid, int width, int height, int elapsed) {

    }

    @Override
    public void onUserJoined(final int uid, int elapsed) {
        if (Constants.DEFAULT_BROADCASTER_UID == uid) {
            mLogUtils.log("broadcaster joined channel:" + uid);
            runOnUiThread(new Runnable() {
                @Override
                public void run() {
                    mSvRemoteView = RtcEngine.CreateRendererView(GameActivity.this);
                    mSvRemoteView.setZOrderMediaOverlay(false);
                    mSvRemoteView.setZOrderOnTop(false);
                    mSvRemoteView.setLayoutParams(new FrameLayout.LayoutParams(ViewGroup.LayoutParams.MATCH_PARENT, ViewGroup.LayoutParams.MATCH_PARENT));

                    rtcEngine().setupRemoteVideo(new VideoCanvas(mSvRemoteView, VideoCanvas.RENDER_MODE_HIDDEN, uid));

                    if (gameInfo().getPlayState() != PlayState.SHOWING_QUESTION && gameInfo().getPlayState() != PlayState.SHOWING_ANSWER) {
                        mFlBigVideoView.removeAllViews();

                        rtcEngine().setRemoteVideoStreamType(gameInfo().getUser().getMediaUid(), io.agora.rtc.Constants.VIDEO_STREAM_HIGH);
                        mFlBigVideoView.addView(mSvRemoteView);
                    }
                }
            });
        }
    }

    @Override
    public void onUserOffline(final int uid, int reason) {
        runOnUiThread(new Runnable() {
            @Override
            public void run() {
                if (uid == Constants.DEFAULT_BROADCASTER_UID && gameInfo().getPlayState() != PlayState.SHOWING_QUESTION && gameInfo().getPlayState() != PlayState.SHOWING_ANSWER) {
                    if (mSvRemoteView.getParent() != null)
                        ((ViewGroup) mSvRemoteView.getParent()).removeAllViews();
                }

                if (gameInfo().getPlayState() == PlayState.SHOWING_QUESTION || gameInfo().getPlayState() == PlayState.SHOWING_ANSWER) {
                    if (mQuestionBoardDialog != null) {
                        mQuestionBoardDialog.dissMiss();

                        mLogUtils.log("User offline to dismiss question dialog");
                    }

                    if (mAnswerBoardDialog != null)
                        mAnswerBoardDialog.dissMiss();
                }


                mLogUtils.log("on user offline-->");
            }
        });
    }


    @Override
    public void onLeaveChannel(IRtcEngineEventHandler.RtcStats stats) {
        mLogUtils.log("leave channel success");
    }

    @Override
    public void onJoinChannelSuccess(String channel, int uid, int elapsed) {
        gameInfo().getUser().setMediaUid(uid);
        mLogUtils.log("join channel success:" + uid + "  channel:" + channel);
    }

    @Override
    public void onAudioVolumeIndication(IRtcEngineEventHandler.AudioVolumeInfo[] speakers, int totalVolume) {

    }

    @Override
    public void onUserMuteVideo(int uid, boolean muted) {

    }

    /**
     * if app should display Answer-Board, onReceiveSEI will be called
     *
     * @param info json-info
     */
    @Override
    public void onReceiveSEI(String info) {
        mLogUtils.log("got sei form sdk..");
        if (gameInfo().getPlayState() == PlayState.WAIT_FOR_SHOW) {
            gameInfo().updatePlayState(PlayState.SHOWING_QUESTION);

            int sid = HQJson2StrUtil.resolveSEI(info);
            if (gameInfo().getQuestion().getSid() == sid && mSvRemoteView != null) {
                rtcEngine().setRemoteVideoStreamType(gameInfo().getUser().getMediaUid(), io.agora.rtc.Constants.VIDEO_STREAM_LOW);
                mLogUtils.log((gameInfo().getRemotePlayStatus() && gameInfo().getLocalPlayStatus()) + "----SEI---1");
                mQuestionBoardDialog.show(getFragmentManager(), "QUESTION_DIALOG", mSvRemoteView, gameInfo().getQuestion(), this, gameInfo().getRemotePlayStatus() && gameInfo().getLocalPlayStatus());
            }
        }
    }

    @Override
    public void onLoginSuccess() {
        checkRemotePlayStatus();

        mLogUtils.log("on rtm login success-->");
    }

    @Override
    public void onError(ErrorInfo error) {
        mLogUtils.log("on rtm error-->" + error);

    }

    @Override
    public void onError(int error) {
        mLogUtils.log("on media error-->" + error);

    }

    @Override
    public void onChannelMessageReceived(String channel, String msg) {
        try {
            Object o = HQJson2StrUtil.json2Str(this, msg);
            if (o instanceof ChatMessage) {
                mLogUtils.log("receive chat message form signaling channel message:" + msg);
                sendMessage(((ChatMessage) o).getContent(), ((ChatMessage) o).getIsMe(), ((ChatMessage) o).getSendAccount());
            } else if (o instanceof QuestionAnswerResult) {

                mLogUtils.log("receive answer result message form signaling channel message:" + msg);
                mLogUtils.log("current play state :" + gameInfo().getPlayState().name());
                if (gameInfo().getPlayState() == PlayState.SHOWING_QUESTION) {
                    mLogUtils.log("get a answer result, and dismiss question dialog");
                    mQuestionBoardDialog.dissMiss();
                    gameInfo().updatePlayState(PlayState.WAIT_FOR_ANSWER);
                }

                mLogUtils.log("current play state :" + gameInfo().getPlayState().name());
                if (gameInfo().getPlayState() == PlayState.WAIT_FOR_ANSWER) {
                    mLogUtils.log("update play state to: showing answer");
                    if (gameInfo().getQuestion().getSid() == Integer.parseInt(((QuestionAnswerResult) o).sid)) {
                        gameInfo().updatePlayState(PlayState.SHOWING_ANSWER);
                        if (gameInfo().getAnswer() != null) {
                            mAnswerBoardDialog.show(getFragmentManager(), "ANSWER_RESULT", mSvRemoteView, (QuestionAnswerResult) o, gameInfo().getQuestion().getQuestion(), gameInfo().getQuestion().getAnswerString(), gameInfo().getAnswer().getResult(), this);

                            if (gameInfo().getLocalPlayStatus() && (gameInfo().getAnswer().getResult() != ((QuestionAnswerResult) o).getResult()))
                                gameInfo().updateLocalPlayStatus(false);
                        } else {
                            mAnswerBoardDialog.show(getFragmentManager(), "ANSWER_RESULT", mSvRemoteView, (QuestionAnswerResult) o, gameInfo().getQuestion().getQuestion(), gameInfo().getQuestion().getAnswerString(), -1, this);
                            gameInfo().updateLocalPlayStatus(false);
                        }
                    }
                }
            } else if (o instanceof Question) {
                mLogUtils.log("receive question message form signaling channel message:" + msg);
                if (gameInfo().getPlayState() == PlayState.SHOWING_ANSWER) {
                    mAnswerBoardDialog.dissMiss();
                    gameInfo().updatePlayState(PlayState.WAIT_FOR_SHOW);
                }

                mLogUtils.log("current play state :" + gameInfo().getPlayState().name());
                if (gameInfo().getPlayState() == PlayState.WAIT_FOR_PLAY)
                    gameInfo().updatePlayState(PlayState.WAIT_FOR_SHOW);

                gameInfo().updateQuestion((Question) o);
            } else if (o instanceof Reset) {
                finish();
                showShortToast("Game has been reset,please rejoin!");
            }
        } catch (JSONException e) {
            e.printStackTrace();
        }
    }

    @Override
    public void onMessageReceivedFrom(String account, String msg) {

    }

    // TODO this method called when question_board_option had been selected
    @Override
    public void onItemClicked(int position, int sid) {
        mLogUtils.log("on item clicked, user choose result:" + position + " ,sid:" + sid);
        gameInfo().updateAnswer(new Answer(sid, position));
        mLogUtils.log("current play state :" + gameInfo().getPlayState().name());

        if (gameInfo().getAnswer() != null) {
            ServerLogic.sendAnswer(new HttpUtil.OnResponse() {
                @Override
                public void onResponse(String data) throws JSONException {
                    mLogUtils.log("send answer response:" + data);
                }
            }, gameInfo().getUser().getSignalAccount(), gameInfo().getUser().getChannelName(), gameInfo().getAnswer().getSid(), gameInfo().getAnswer().getResult(), gameInfo().getRemotePlayStatus());
        }
    }

    @Override
    public void onCountDownFinish() {
        mLogUtils.log("time count down finish...");
        if (gameInfo().getPlayState() == PlayState.SHOWING_QUESTION)
            gameInfo().updatePlayState(PlayState.WAIT_FOR_ANSWER);

        runOnUiThread(new Runnable() {
            @Override
            public void run() {
                if (mSvRemoteView != null) {
                    mFlBigVideoView.removeAllViews();
                    if (mSvRemoteView.getParent() != null)
                        ((ViewGroup) mSvRemoteView.getParent()).removeAllViews();
                    mFlBigVideoView.addView(mSvRemoteView);
                }
            }
        });
    }

    @Override
    public void onAnswerBoardDismiss() {
        mLogUtils.log("answer board dismissed:");
        mLogUtils.log("current play state :" + gameInfo().getPlayState().name());
        gameInfo().clearAnswer();
        gameInfo().clearQuestion();
        gameInfo().updatePlayState(PlayState.WAIT_FOR_PLAY);

        runOnUiThread(new Runnable() {
            @Override
            public void run() {
                if (mSvRemoteView != null) {
                    mFlBigVideoView.removeAllViews();
                    if (mSvRemoteView.getParent() != null)
                        ((ViewGroup) mSvRemoteView.getParent()).removeAllViews();
                    rtcEngine().setRemoteVideoStreamType(gameInfo().getUser().getMediaUid(), io.agora.rtc.Constants.VIDEO_STREAM_HIGH);
                    mFlBigVideoView.addView(mSvRemoteView);
                }
                mLogUtils.log("clear game info: answer and question!!");

                startCongraAnim();
            }
        });
    }

    private static class LogicHandler extends Handler{
        private GameActivity gameActivity;

        public LogicHandler(WeakReference<GameActivity> g) {
            this.gameActivity = g.get();
        }

        @Override
        public void handleMessage(Message msg) {
            super.handleMessage(msg);

        }
    }
}
