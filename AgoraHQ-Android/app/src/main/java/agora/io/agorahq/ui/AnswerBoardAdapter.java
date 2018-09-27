package agora.io.agorahq.ui;

import android.content.Context;
import android.support.v7.widget.RecyclerView;
import android.text.TextUtils;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.TextView;

import java.lang.ref.WeakReference;
import java.util.ArrayList;
import java.util.List;

import agora.io.agorahq.R;

public class AnswerBoardAdapter extends RecyclerView.Adapter<RecyclerView.ViewHolder> {
    private Context mContext;
    private List<String> mDataSet;
    private int mCorrect;
    private int mUserChoose;
    private boolean isUserRight;

    public AnswerBoardAdapter(WeakReference<Context> ctx, List<String> data, int corr, int userChoose) {
        this.mContext = ctx.get();
        this.mDataSet = data;
        this.mCorrect = corr;
        this.mUserChoose = userChoose;

        this.isUserRight = mUserChoose == mCorrect;
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

            if (isUserRight) {
                if (mUserChoose == position)
                    qHolder.tvOptions.setBackgroundResource(R.drawable.answer_text_bg);
            } else {
                if (mUserChoose == position)
                    qHolder.tvOptions.setBackgroundResource(R.drawable.answer_text_bg_error);

                if (mCorrect == position)
                    qHolder.tvOptions.setBackgroundResource(R.drawable.answer_text_bg);

            }
        }
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
}
