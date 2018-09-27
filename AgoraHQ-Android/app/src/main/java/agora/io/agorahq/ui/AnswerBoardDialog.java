package agora.io.agorahq.ui;

import android.app.AlertDialog;
import android.app.Dialog;
import android.app.DialogFragment;
import android.app.FragmentManager;
import android.content.Context;
import android.content.DialogInterface;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.support.v7.widget.LinearLayoutManager;
import android.support.v7.widget.RecyclerView;
import android.view.LayoutInflater;
import android.view.SurfaceView;
import android.view.ViewGroup;
import android.widget.FrameLayout;
import android.widget.TextView;

import java.lang.ref.WeakReference;
import java.util.ArrayList;
import java.util.List;
import java.util.Timer;
import java.util.TimerTask;

import agora.io.agorahq.R;
import agora.io.agorahq.bean.QuestionAnswerResult;

public class AnswerBoardDialog extends DialogFragment{
    private final static int DIALOG_FINISH = 0x50001;

    private SurfaceView mSmallVideoView;
    private QuestionAnswerResult mQuestionResult;
    private int mUserChoose;
    private String mTitle;
    private List<String> mOptions;

    private RecyclerView rvQuestionOptions;
    private InnerHandler mHandler;
    private AnswerBoardFinishedListener mLis;

    private Timer mCountDownTimer;
    private TimerTask mCountDownTimerTask;

    public void show(FragmentManager fm, String tag, SurfaceView smallView, QuestionAnswerResult result, String questionTitle, ArrayList<String> options, int userChoose, AnswerBoardFinishedListener l) {
        this.mUserChoose = userChoose;
        this.mSmallVideoView = smallView;
        this.mQuestionResult = result;
        this.mTitle = questionTitle;
        this.mOptions = options;
        this.mLis = l;
        super.show(fm, tag);
    }

    @Override
    public Dialog onCreateDialog(Bundle savedInstanceState) {
        AlertDialog.Builder builder = new AlertDialog.Builder(getActivity());
        this.mHandler = new InnerHandler(this);

        if (mCountDownTimer != null) {
            mCountDownTimer.cancel();
            mCountDownTimer = null;
        }

        mCountDownTimer = new Timer();
        LayoutInflater inflater = getActivity().getLayoutInflater();
        builder.setView(inflater.inflate(R.layout.board_question_dialog, null));
        return builder.create();
    }


    @Override
    public void onStart() {
        super.onStart();

        final AlertDialog dialog = (AlertDialog) getDialog();
        dialog.setCanceledOnTouchOutside(false);

        FrameLayout flSmallVideo = dialog.findViewById(R.id.sv_question_video_view);
        mSmallVideoView.setZOrderOnTop(false);
        mSmallVideoView.setZOrderMediaOverlay(false);
        mSmallVideoView.setLayoutParams(new FrameLayout.LayoutParams(ViewGroup.LayoutParams.MATCH_PARENT, ViewGroup.LayoutParams.MATCH_PARENT));
        ((ViewGroup)mSmallVideoView.getParent()).removeAllViews();
        flSmallVideo.addView(mSmallVideoView);

        final CircleCountDown ccdTime = dialog.findViewById(R.id.ccd_timeout_view);

        final int totlaTimeOut = mQuestionResult.getTimeout() * 50;
        final int[] count = new int[1];

        if (mCountDownTimerTask != null) {
            mCountDownTimerTask.cancel();
            mCountDownTimerTask = null;
        }
        mCountDownTimerTask = new TimerTask() {
            @Override
            public void run() {
                if (count[0] == totlaTimeOut) {
                    if (mHandler != null) {
                        mHandler.sendEmptyMessage(DIALOG_FINISH);
                    }
                } else {
                    count[0]++;
                    ccdTime.updateProcess(count[0], totlaTimeOut);
                }

            }
        };
        mCountDownTimer.schedule(mCountDownTimerTask, 0, 20);

//        startCountDown(ccdTime, );

        TextView questionTitle = dialog.findViewById(R.id.tv_question_title);
        questionTitle.setText(mTitle);

        rvQuestionOptions = dialog.findViewById(R.id.rv_question_options);
        rvQuestionOptions.setClickable(false);
        AnswerBoardAdapter abdOptionAdapter = new AnswerBoardAdapter(new WeakReference<Context>(getActivity()), mOptions, mQuestionResult.result ,mUserChoose);
        abdOptionAdapter.setHasStableIds(true);
        rvQuestionOptions.setAdapter(abdOptionAdapter);
        rvQuestionOptions.setLayoutManager(new LinearLayoutManager(getActivity(), RecyclerView.VERTICAL, false));
        rvQuestionOptions.addItemDecoration(new DefaultDecoration());
    }

    @Override
    public void onCancel(DialogInterface dialog) {
        this.dismissAllowingStateLoss();
        mSmallVideoView = null;
        mQuestionResult = null;
    }

    public void dissMiss(){
        if (mLis != null)
            mLis.onAnswerBoardDismiss();

        mLis = null;
        this.resetTimer();
        this.dismiss();
        mHandler = null;
    }

    private static class InnerHandler extends Handler {
        private AnswerBoardDialog qbd;

        public InnerHandler(AnswerBoardDialog dialog) {
            this.qbd = dialog;
        }

        @Override
        public void handleMessage(Message msg) {
            super.handleMessage(msg);
            switch (msg.what) {
                case DIALOG_FINISH:
                    qbd.dissMiss();
                    break;
                default:
                    break;
            }
        }
    }

    public interface AnswerBoardFinishedListener {
        void onAnswerBoardDismiss();
    }

    private void resetTimer(){
        if (mCountDownTimerTask != null)
            mCountDownTimerTask.cancel();

        if (mCountDownTimer != null)
            mCountDownTimer.cancel();

        mCountDownTimerTask = null;
        mCountDownTimer = null;
    }
}
