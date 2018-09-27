package agora.io.agorahq.ui;

import android.app.Activity;
import android.content.Context;
import android.support.v7.widget.RecyclerView;
import android.text.TextUtils;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.TextView;

import java.lang.ref.WeakReference;
import java.util.ArrayList;

import agora.io.agorahq.GlobalApplication;
import agora.io.agorahq.R;

public class QuestionBoardAdapter extends RecyclerView.Adapter<RecyclerView.ViewHolder> {
    private Context mContext;
    private ArrayList<String> mDataSet;
    private QuestionItemListener mListener;

    private boolean mHasChooseItem = false;

    public QuestionBoardAdapter(WeakReference<Context> ctx, ArrayList<String> data) {
        this.mContext = ctx.get();
        this.mDataSet = data;
        this.mHasChooseItem = false;
    }

    public void setItemClickListener(QuestionItemListener l) {
        mListener = l;
    }

    @Override
    public QuestionViewHolder onCreateViewHolder(ViewGroup parent, int viewType) {
        View v = LayoutInflater.from(mContext).inflate(R.layout.item_question_option, parent, false);
        return new QuestionViewHolder(v);
    }

    @Override
    public void onBindViewHolder(RecyclerView.ViewHolder holder, final int position) {
        String option = mDataSet.get(position);

        final QuestionViewHolder qHolder = (QuestionViewHolder) holder;
        if (TextUtils.isEmpty(option)) {
           qHolder.tvOptions.setText("unknow error!");
        } else {
            qHolder.tvOptions.setText(option);
        }

        qHolder.tvOptions.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                if (((GlobalApplication)((Activity)mContext).getApplication()).getGlobalGameInfo().getLocalPlayStatus() && mListener != null && !mHasChooseItem) {
                    mListener.onQuestionOptionItemClicked(position);

                    mHasChooseItem = true;
                    qHolder.tvOptions.setBackgroundResource(R.drawable.answer_text_bg);
                }
            }
        });
    }

    @Override
    public int getItemCount() {
        return mDataSet.size();
    }

    public class QuestionViewHolder extends RecyclerView.ViewHolder {
        public TextView tvOptions;

        public QuestionViewHolder(View itemView) {
            super(itemView);
            tvOptions = itemView.findViewById(R.id.tv_question_option_content);
        }
    }

    public interface QuestionItemListener {
        void onQuestionOptionItemClicked(int position);
    }
}
