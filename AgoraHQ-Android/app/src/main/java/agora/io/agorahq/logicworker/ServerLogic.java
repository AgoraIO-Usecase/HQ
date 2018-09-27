package agora.io.agorahq.logicworker;

import org.json.JSONException;

import java.util.HashMap;
import java.util.Map;

import agora.io.agorahq.utils.Constants;
import agora.io.agorahq.utils.HttpUtil;

public class ServerLogic {
    public static void checkRelive(HttpUtil.OnResponse callback, String account, String channelName) {
        Map<String, String> data = new HashMap();
        data.put("uid", account);
        data.put("gid", channelName);
        HttpUtil utils = new HttpUtil();
        utils.execHttpAsyncTask(Constants.HTTP_URL_RELIVE, true, callback, data);
    }

    /**
     * send answer to server
     * @param callback
     * @param account
     * @param channelName
     * @param sid
     * @param result
     * @param localPlayStatus
     */
    public static void sendAnswer(HttpUtil.OnResponse callback, String account, String channelName, int sid, int result, boolean localPlayStatus) {
       if (localPlayStatus) {
           Map data = new HashMap<>();
           data.put("uid", account);
           data.put("gid", channelName);
           data.put("sid", sid);
           data.put("result", result);

           HttpUtil hu = new HttpUtil();
           hu.execHttpAsyncTask(Constants.HTTP_URL_ANSWER, true, callback, data);
       }
    }

    public static void sendServerLocalJoin(HttpUtil.OnResponse callback, String account, String channelName) throws JSONException {
        Map data = new HashMap<>();
        data.put("uid", account);
        data.put("gid", channelName);

        HttpUtil hu = new HttpUtil();
        hu.execHttpAsyncTask(Constants.HTTP_URL_LOCAL_JOIN, true, callback, data);
    }

    /**
     * whether accept invite from broadcaster
     * @param isAccept
     * @param account
     * @param mediaUid
     * @param channelName
     */
    public static void acceptInvitFromHQBroadcast(boolean isAccept, String account, String mediaUid, String channelName) {
        Map<String, String> mData = new HashMap<>();
        mData.put("accept", isAccept + "");
        mData.put("account", account);
        mData.put("mediaUid", mediaUid);
        mData.put("gid", channelName);

        HttpUtil httpUtil = new HttpUtil();
        httpUtil.execHttpAsyncTask(Constants.HTTP_URL_INVITE_RESPONSE, true, null, mData);
    }
}
