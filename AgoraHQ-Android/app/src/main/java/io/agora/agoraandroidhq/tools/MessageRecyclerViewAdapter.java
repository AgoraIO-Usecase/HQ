package io.agora.agoraandroidhq.tools;

import android.app.Activity;
import android.content.Context;
import android.os.Build;
import android.support.annotation.RequiresApi;
import android.support.v7.widget.RecyclerView;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.ImageView;
import android.widget.TextView;

import java.util.ArrayList;

import io.agora.agoraandroidhq.R;
import io.agora.agoraandroidhq.module.Message;

/**
 * Created by zhangtao on 2018/1/13.
 */

public class MessageRecyclerViewAdapter extends RecyclerView.Adapter<RecyclerView.ViewHolder> {
    public ArrayList<Message> mMsglist;

    private final Context mContext;
    protected final LayoutInflater mInflater;

    public void updateData(Message message) {
        mMsglist.add(message);
        notifyDataSetChanged();
    }

    public MessageRecyclerViewAdapter(Context context, ArrayList<Message> list) {
        mContext = context;
        mInflater = ((Activity) context).getLayoutInflater();
        mMsglist = list;
    }

    @Override
    public RecyclerView.ViewHolder onCreateViewHolder(ViewGroup parent, int viewType) {
        View v = mInflater.inflate(R.layout.in_channel_message, parent, false);
        return new MessageHolder(v);
    }

    @Override
    public void onBindViewHolder(RecyclerView.ViewHolder holder, int position) {
        if (mMsglist == null | mMsglist.size() == 0) {
            return;
        }
        Message msg = mMsglist.get(position);

        MessageHolder myHolder = (MessageHolder) holder;
        String sender = msg.getSender();
        if (sender.equals(GameControl.currentUser.getUserName().toString()) && msg.getIsMe()) {
            // myHolder.itemView.setBackgroundResource(R.drawable.rounded_bg_blue);
            myHolder.msgTitle.setText("[" + sender + "] ");
            myHolder.msgContent.setBackgroundResource(R.drawable.rounded_bg_blue);
            myHolder.msgContent.setText(msg.getContent());
            myHolder.msgContent.setAlpha(0.5f);
            myHolder.headImage.setImageDrawable(GameControl.currentUser.getDrawable());
        } else {
            // myHolder.itemView.setBackgroundResource(R.drawable.rounded_bg);
            myHolder.msgTitle.setText("[" + sender + "] ");
            myHolder.msgContent.setBackgroundResource(R.drawable.rounded_bg);
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
            msgContent = (TextView) v.findViewById(R.id.msg_content);
            msgTitle = v.findViewById(R.id.msg_title);
            headImage = v.findViewById(R.id.msg_headImage);
        }
    }
}

