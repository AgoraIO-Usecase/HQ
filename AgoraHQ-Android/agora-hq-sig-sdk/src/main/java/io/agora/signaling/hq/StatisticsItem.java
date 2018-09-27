package io.agora.signaling.hq;

import java.util.ArrayList;
import java.util.List;

public class StatisticsItem {

    private String appid;
    private String account;
    private String channel;
    private String sid;
    private String v;
    private String va;
    private String vr;
    private String p;
    private String d;
    private String vos;
    private List<List<Object>> msgs = null;
    private List<List<Object>> evts_a = null;
    private List<List<Object>> evts_r = null;

    public StatisticsItem() {
        msgs = new ArrayList<>();
        evts_a = new ArrayList<>();
        evts_r = new ArrayList<>();
    }

    public String getAppid() {
        return appid;
    }

    public void setAppid(String appid) {
        this.appid = appid;
    }

    public String getAccount() {
        return account;
    }

    public void setAccount(String account) {
        this.account = account;
    }

    public String getChannel() {
        return channel;
    }

    public void setChannel(String channel) {
        this.channel = channel;
    }

    public String getSid() {
        return sid;
    }

    public void setSid(String sid) {
        this.sid = sid;
    }

    public String getV() {
        return v;
    }

    public void setV(String v) {
        this.v = v;
    }

    public String getVa() {
        return va;
    }

    public void setVa(String va) {
        this.va = va;
    }

    public String getVr() {
        return vr;
    }

    public void setVr(String vr) {
        this.vr = vr;
    }

    public String getP() {
        return p;
    }

    public void setP(String p) {
        this.p = p;
    }

    public String getD() {
        return d;
    }

    public void setD(String d) {
        this.d = d;
    }

    public String getVos() {
        return vos;
    }

    public void setVos(String vos) {
        this.vos = vos;
    }

    public void addMsgsItem(List<Object> item) {
        msgs.add(item);
    }

    public void addEvtsAItem(List<Object> item) {
        evts_a.add(item);
    }

    public void addEvtsRItem(List<Object> item) {
        evts_r.add(item);
    }

    public String convertToJson() {
        StringBuilder rawJson = new StringBuilder("{");
        rawJson.append("\"appid\":").append("\"").append(appid).append("\", ");
        rawJson.append("\"account\":").append("\"").append(account).append("\", ");
        rawJson.append("\"channel\":").append("\"").append(channel).append("\", ");
        rawJson.append("\"sid\":").append("\"").append(sid).append("\", ");
        rawJson.append("\"v\":").append("\"").append(v).append("\", ");
        rawJson.append("\"va\":").append("\"").append(va).append("\", ");
        rawJson.append("\"vr\":").append("\"").append(vr).append("\", ");
        rawJson.append("\"p\":").append("\"").append(p).append("\", ");
        rawJson.append("\"d\":").append("\"").append(d).append("\", ");
        rawJson.append("\"vos\":").append("\"").append(vos).append("\", ");
        rawJson.append("\"msgs\":").append("[");
        for (int i = 0; i < msgs.size(); i++) {
            rawJson.append("[");
            for (int j = 0; j < msgs.get(i).size(); j++) {
                if (msgs.get(i).get(j) instanceof String) {
                    rawJson.append("\"").append(msgs.get(i).get(j)).append("\"");
                } else {
                    rawJson.append(msgs.get(i).get(j));
                }
                if (j < msgs.get(i).size() - 1) {
                    rawJson.append(", ");
                }
            }
            if (i == msgs.size() - 1) {
                rawJson.append("]");
            } else {
                rawJson.append("], ");
            }
        }
        rawJson.append("], ");
        rawJson.append("\"evts_a\":").append("[");
        for (int i = 0; i < evts_a.size(); i++) {
            rawJson.append("[");
            for (int j = 0; j < evts_a.get(i).size(); j++) {
                if (evts_a.get(i).get(j) instanceof String) {
                    rawJson.append("\"").append(evts_a.get(i).get(j)).append("\"");
                } else {
                    rawJson.append(evts_a.get(i).get(j));
                }
                if (j < evts_a.get(i).size() - 1) {
                    rawJson.append(", ");
                }
            }
            if (i == evts_a.size() - 1) {
                rawJson.append("]");
            } else {
                rawJson.append("], ");
            }
        }
        rawJson.append("], ");
        rawJson.append("\"evts_r\":").append("[");
        for (int i = 0; i < evts_r.size(); i++) {
            rawJson.append("[");
            for (int j = 0; j < evts_r.get(i).size(); j++) {
                if (evts_r.get(i).get(j) instanceof String) {
                    rawJson.append("\"").append(evts_r.get(i).get(j)).append("\"");
                } else {
                    rawJson.append(evts_r.get(i).get(j));
                }
                if (j < evts_r.get(i).size() - 1) {
                    rawJson.append(", ");
                }
            }
            if (i == evts_r.size() - 1) {
                rawJson.append("]");
            } else {
                rawJson.append("], ");
            }
        }
        rawJson.append("]");
        rawJson.append("}");
        return rawJson.toString();
    }

    public void clear() {
        msgs.clear();
        evts_a.clear();
        evts_r.clear();
    }
}
