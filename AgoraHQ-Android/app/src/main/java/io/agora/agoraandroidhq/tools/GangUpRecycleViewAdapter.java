package io.agora.agoraandroidhq.tools;

import android.app.Activity;
import android.content.Context;
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
 * Created by zhangtao on 2018/2/7.
 */

public class GangUpRecycleViewAdapter extends RecyclerView.Adapter<RecyclerView.ViewHolder>{
    public ArrayList<String> mUidList;

    private final Context mContext;
    protected final LayoutInflater mInflater;


    public void updateData() {
        notifyDataSetChanged();
    }


    public void addGangUpUidList(String uid){

        mUidList.add(uid);
        notifyDataSetChanged();
        GameControl.logD("sub addGangUpUidList uid  =  "+uid +"  mUidList Size = "+mUidList.size());
    }

    public void removeUidFromGangUpList(String uid){
        mUidList.remove(uid);
        notifyDataSetChanged();
        GameControl.logD("sub removeUidFromGangUpList uid  =  "+uid +"  mUidList Size = "+mUidList.size());

    }

    public void clearGangUpUidList(){

        mUidList.clear();

        notifyDataSetChanged();
        GameControl.logD("sub clearGangUpUidList uid  =  " );
    }


    public GangUpRecycleViewAdapter(Context context) {
        mContext = context;
        mInflater = ((Activity) context).getLayoutInflater();
        mUidList = new ArrayList<String>();
    }

    @Override
    public RecyclerView.ViewHolder onCreateViewHolder(ViewGroup parent, int viewType) {
        View v = mInflater.inflate(R.layout.gang_up_recycleview_item, parent, false);
        return new GangUpRecycleViewAdapter.MessageHolder(v);
    }

    @Override
    public void onBindViewHolder(RecyclerView.ViewHolder holder, int position) {

        GameControl.logD("sub onBindViewHolder uidListSize  =  "+mUidList.size() );
        if (mUidList == null | mUidList.size() == 0) {
            return;
        }
        String uid = mUidList.get(position);

        GangUpRecycleViewAdapter.MessageHolder myHolder = (GangUpRecycleViewAdapter.MessageHolder) holder;
        //String sender = msg.getSender();
       // Log.d("zhang ", "sender " + sender + "   currentUser  = " + GameControl.currentUser.getName());
        if (uid.equals(GameControl.currentUser.getAccount().toString())) {
            /*myHolder.itemView.setBackgroundResource(R.drawable.rounded_bg_blue);
            myHolder.msgTitle.setText("[" + sender + "] ");
            myHolder.msgContent.setBackgroundResource(R.drawable.rounded_bg_blue);
            myHolder.msgContent.setText(msg.getContent());*/
            myHolder.uidContent.setBackgroundResource(R.drawable.rounded_bg_blue);

            myHolder.uidContent.setText(uid+"");
           // myHolder.msgContent.setAlpha(0.5f);
            myHolder.uidHeadImage.setImageDrawable(GameControl.currentUser.drawable);
        } else {
            // myHolder.itemView.setBackgroundResource(R.drawable.rounded_bg);
           /* myHolder.msgTitle.setText("[" + sender + "] ");
            myHolder.msgContent.setBackgroundResource(R.drawable.rounded_bg);
            myHolder.msgContent.setText(msg.getContent());
            myHolder.msgContent.setAlpha(0.5f);*/
           myHolder.uidContent.setText(uid+"");

        }
    }

    @Override
    public int getItemCount() {
        return mUidList.size();
    }

    @Override
    public long getItemId(int position) {
        return mUidList.get(position).hashCode();
    }

    public class MessageHolder extends RecyclerView.ViewHolder {
        public TextView uidContent;


        public ImageView uidHeadImage;

        public MessageHolder(View v) {
            super(v);
           /* msgContent = (TextView) v.findViewById(R.id.msg_content);
            msgTitle = v.findViewById(R.id.msg_title);

            headImage = v.findViewById(R.id.msg_headImage);*/

           uidContent = v.findViewById(R.id.gang_up_recycleview_item_uid);

           uidHeadImage = v.findViewById(R.id.gang_up_recycleview_item_headImage);


        }
    }

}
