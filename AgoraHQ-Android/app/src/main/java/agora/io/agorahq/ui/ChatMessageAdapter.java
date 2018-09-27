package agora.io.agorahq.ui;

import android.app.Activity;
import android.content.Context;
import android.support.v7.widget.RecyclerView;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.ImageView;
import android.widget.TextView;

import java.lang.ref.WeakReference;
import java.util.List;

import agora.io.agorahq.GlobalApplication;
import agora.io.agorahq.R;
import agora.io.agorahq.bean.ChatMessage;

public class ChatMessageAdapter extends RecyclerView.Adapter<RecyclerView.ViewHolder> {
    public List<ChatMessage> mMsglist;

    private final Context mContext;
    protected LayoutInflater mInflater;

    public ChatMessageAdapter(WeakReference<Context> context, List<ChatMessage> list) {
        mContext = context.get();
        mInflater = ((Activity) context.get()).getLayoutInflater();
        mMsglist = list;
    }

    @Override
    public RecyclerView.ViewHolder onCreateViewHolder(ViewGroup parent, int viewType) {
        View v = mInflater.inflate(R.layout.item_chat_message, parent, false);
        return new MessageHolder(v);
    }

    @Override
    public void onBindViewHolder(RecyclerView.ViewHolder holder, int position) {

        if (mMsglist == null || mMsglist.size() == 0) {
            return;
        }

        ChatMessage msg = mMsglist.get(position);

        MessageHolder myHolder = (MessageHolder) holder;
        String sender = msg.getSendAccount();

        if (sender.equals(((GlobalApplication) ((Activity)mContext).getApplication()).getGlobalGameInfo().getUser().getUserName()) && msg.getIsMe()) {
            myHolder.msgTitle.setText("[" + sender + "] ");
            myHolder.msgContent.setBackgroundResource(R.drawable.chat_bubble_bg);

            myHolder.msgContent.setText(msg.getContent());
            myHolder.msgContent.setAlpha(0.5f);

            myHolder.headImage.setImageDrawable(((GlobalApplication)((Activity)mContext).getApplication()).getGlobalGameInfo().getUser().getDrawable());
        } else {
            myHolder.msgTitle.setText("[" + sender + "] ");
            myHolder.msgContent.setBackgroundResource(R.drawable.chat_bubble_other_bg);
            myHolder.msgContent.setText(msg.getContent());
            myHolder.msgContent.setAlpha(0.5f);
        }
    }

    @Override
    public int getItemCount() {
        return mMsglist.size();
    }

    @Override
    public long getItemId(int position) {
        return mMsglist.get(position).hashCode();
    }

    public class MessageHolder extends RecyclerView.ViewHolder {
        public TextView msgContent;
        public TextView msgTitle;
        public ImageView headImage;

        public MessageHolder(View v) {
            super(v);
            msgContent = v.findViewById(R.id.msg_content);
            msgTitle = v.findViewById(R.id.msg_title);
            headImage = v.findViewById(R.id.msg_headImage);
        }
    }

    public void releaseAdapter() {
        mMsglist.clear();
        mMsglist = null;
        notifyDataSetChanged();
        mInflater = null;
    }
}

