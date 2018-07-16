package io.agora.livequiz.tools;

import android.app.Activity;
import android.content.Context;
import android.support.v7.widget.RecyclerView;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.ImageView;
import android.widget.TextView;

import java.util.ArrayList;

import io.agora.livequiz.R;

/**
 * Created by zhangtao on 2018/2/7.
 */

public class GangUpRecycleViewAdapter extends RecyclerView.Adapter<RecyclerView.ViewHolder> {
    public ArrayList<String> mUidList;
    private final Context mContext;
    protected LayoutInflater mInflater;
    private String tag = "[GangUpRecycleViewAdapter]  ";

    public void updateData() {
        notifyDataSetChanged();
    }

    public void addGangUpUidList(String uid) {
        mUidList.add(uid);
        notifyDataSetChanged();
        GameControl.logD(tag + "sub addGangUpUidList uid  =  " + uid + "  mUidList Size = " + mUidList.size());
    }

    public void removeUidFromGangUpList(String uid) {
        mUidList.remove(uid);
        notifyDataSetChanged();
        GameControl.logD(tag + "sub removeUidFromGangUpList uid  =  " + uid + "  mUidList Size = " + mUidList.size());
    }

    public void clearGangUpUidList() {
        mUidList.clear();
        notifyDataSetChanged();
        GameControl.logD(tag + "sub clearGangUpUidList uid  =  ");
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

        GameControl.logD(tag + "sub onBindViewHolder uidListSize  =  " + mUidList.size());
        if (mUidList == null | mUidList.size() == 0) {
            return;
        }
        String uid = mUidList.get(position);
        GangUpRecycleViewAdapter.MessageHolder myHolder = (GangUpRecycleViewAdapter.MessageHolder) holder;
        if (uid.equals(GameControl.currentUser.getMediaUid().toString())) {
            myHolder.uidContent.setBackgroundResource(R.drawable.rounded_bg_blue);
            myHolder.uidContent.setText(uid + "");
            myHolder.uidHeadImage.setImageDrawable(GameControl.currentUser.getDrawable());
        } else {
            myHolder.uidContent.setText(uid + "");
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
            uidContent = v.findViewById(R.id.gang_up_recycleview_item_uid);
            uidHeadImage = v.findViewById(R.id.gang_up_recycleview_item_headImage);
        }
    }

    public void releaseRecycleViewAdapter() {
        mUidList.clear();
        mUidList = null;
        notifyDataSetChanged();
        mInflater = null;
    }
}
