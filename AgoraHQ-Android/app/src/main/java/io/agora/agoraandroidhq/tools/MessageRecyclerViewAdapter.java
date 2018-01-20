package io.agora.agoraandroidhq.tools;

import android.app.Activity;
import android.content.Context;

import android.support.v7.widget.RecyclerView;
import android.text.TextUtils;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.TextView;

import java.util.ArrayList;

import io.agora.agoraandroidhq.R;
import io.agora.agoraandroidhq.control.AgoraLinkToCloud;
import io.agora.agoraandroidhq.module.Message;

/**
 * Created by zhangtao on 2018/1/13.
 */

public class MessageRecyclerViewAdapter extends RecyclerView.Adapter<RecyclerView.ViewHolder> {
    public  ArrayList<Message> mMsglist;

    private final Context mContext;
    protected final LayoutInflater mInflater;


    public void updateData(Message message){
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
        if(mMsglist==null | mMsglist.size()==0){
            return;
        }
        Message msg = mMsglist.get(position);

        MessageHolder myHolder = (MessageHolder) holder;
        String sender = msg.getSender();
        Log.d("zhang ","sender "+sender +"   currentUser  = "+AgoraLinkToCloud.currentUser.getName());
        if (sender.equals(AgoraLinkToCloud.currentUser.getName().toString())) {
            myHolder.itemView.setBackgroundResource(R.drawable.rounded_bg_blue);
            myHolder.msgContent.setText("[" + sender + "] " + msg.getContent());
            myHolder.msgContent.setAlpha(0.5f);
        } else {
            myHolder.itemView.setBackgroundResource(R.drawable.rounded_bg);
            myHolder.msgContent.setText("[" + sender + "] " + msg.getContent());
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

        public MessageHolder(View v) {
            super(v);
            msgContent = (TextView) v.findViewById(R.id.msg_content);
        }
    }
}

