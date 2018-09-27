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
import java.util.Timer;
import java.util.TimerTask;

import agora.io.agorahq.R;
import agora.io.agorahq.bean.Question;
import agora.io.agorahq.utils.LogUtil;

public class QuestionBoardDialog extends DialogFragment implements QuestionBoardAdapter.QuestionItemListener {
    private final static int DIALOG_FINISH = 0x50001;

    private SurfaceView mSmallVideoView;
    private Question mQuestion;

    private DialogListener mListener;
    private RecyclerView rvQuestionOptions;
    private boolean mPlayStatus;

    private Timer mCountDownTimer;
    private TimerTask mCountDownTimerTask;

    private InnerHandler mHandler;

    private LogUtil mLogUtils;

    public void show(FragmentManager fm, String tag, SurfaceView smallView, Question question, DialogListener l, boolean playStatus) {
        super.show(fm, tag);
        this.mSmallVideoView = smallView;
        this.mQuestion = question;
        this.mListener = l;
        this.mPlayStatus = playStatus;
    }

    @Override
    public Dialog onCreateDialog(Bundle savedInstanceState) {
        AlertDialog.Builder builder = new AlertDialog.Builder(getActivity());

        if (mCountDownTimer != null) {
            mCountDownTimer.cancel();
            mCountDownTimer = null;
        }

        mCountDownTimer = new Timer();

        mLogUtils = LogUtil.newInstance();

        LayoutInflater inflater = getActivity().getLayoutInflater();
        this.mHandler = new InnerHandler(this);
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
        ViewGroup v = (ViewGroup) mSmallVideoView.getParent();
        v.removeAllViews();
        flSmallVideo.addView(mSmallVideoView);

        final CircleCountDown ccdTime = dialog.findViewById(R.id.ccd_timeout_view);

        final int totlaTimeOut = mQuestion.getTimeOut() * 50;
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
                        mLogUtils.log("question handler to send count down finish");
                        mHandler.sendEmptyMessage(DIALOG_FINISH);
                        mHandler = null;
                    }
                } else {
                    count[0]++;
                    ccdTime.updateProcess(count[0], totlaTimeOut);
                }

            }
        };
        mCountDownTimer.schedule(mCountDownTimerTask, 0, 20);

        TextView questionTitle = dialog.findViewById(R.id.tv_question_title);
        questionTitle.setText(mQuestion.getQuestion());

        TextView qustionStatus = dialog.findViewById(R.id.tv_question_status);
        if (!mPlayStatus)
            qustionStatus.setText("Can't play!");

        rvQuestionOptions = dialog.findViewById(R.id.rv_question_options);
        rvQuestionOptions.setClickable(mPlayStatus);
        rvQuestionOptions.setFocusable(mPlayStatus);
        QuestionBoardAdapter qbaQuestionOptionAdapter = new QuestionBoardAdapter(new WeakReference<Context>(getActivity()), mQuestion.getAnswerString());
        qbaQuestionOptionAdapter.setHasStableIds(true);
        qbaQuestionOptionAdapter.setItemClickListener(this);
        rvQuestionOptions.setAdapter(qbaQuestionOptionAdapter);
        rvQuestionOptions.setLayoutManager(new LinearLayoutManager(getActivity(), RecyclerView.VERTICAL, false));
        rvQuestionOptions.addItemDecoration(new DefaultDecoration());
    }

    @Override
    public void onCancel(DialogInterface dialog) {
        this.dismissAllowingStateLoss();
        if (getActivity() != null && getActivity() instanceof DialogListener) {
            mSmallVideoView = null;
            mQuestion = null;
            mListener = null;
        }
    }

    public void dissMiss() {
        if (mListener != null)
            mListener.onCountDownFinish();
        this.resetTimer();
        this.dismiss();
        mListener = null;
        mHandler = null;
    }

    @Override
    public void onQuestionOptionItemClicked(int position) {
        if (mListener != null)
            mListener.onItemClicked(position, mQuestion.getSid());
    }

    public interface DialogListener {
        void onItemClicked(int position, int sid);

        void onCountDownFinish();
    }

    private class InnerHandler extends Handler {
        private QuestionBoardDialog qbd;

        public InnerHandler(QuestionBoardDialog dialog) {
            this.qbd = dialog;
        }

        @Override
        public void handleMessage(Message msg) {
            super.handleMessage(msg);
            switch (msg.what) {
                case DIALOG_FINISH:
                    qbd.mLogUtils.log("disMissQuestionDialog");
                    qbd.dissMiss();
                    break;
                default:
                    break;
            }
        }
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
