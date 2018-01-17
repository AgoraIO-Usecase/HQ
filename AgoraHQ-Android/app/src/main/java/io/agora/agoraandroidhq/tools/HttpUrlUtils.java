package io.agora.agoraandroidhq.tools;

import android.os.AsyncTask;
import android.support.v4.util.ArrayMap;
import android.util.Log;
import android.widget.Toast;

import org.json.JSONException;

import java.io.ByteArrayOutputStream;
import java.io.DataOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.UnsupportedEncodingException;
import java.net.HttpURLConnection;
import java.net.MalformedURLException;
import java.net.ProtocolException;
import java.net.URL;
import java.net.URLEncoder;
import java.security.MessageDigest;
import java.util.Iterator;
import java.util.Map;
import java.util.Set;
import java.util.Map.Entry;

import io.agora.agoraandroidhq.view.GameActivity;

/**
 * Created by zhangtao on 2018/1/12.
 */

public class HttpUrlUtils {

    public interface OnResponse {
        void onResponse(String data) throws JSONException;
    }


    public void execHttpAsyncTask(String url, boolean isPost, HttpUrlUtils.OnResponse callback, Map data) {

        new HttpAsyncTask(url, isPost, callback, data).execute();
    }

    private class HttpAsyncTask extends AsyncTask {
        private String url;

        private boolean isPost;

        private HttpURLConnection conn;
        private Map data;
        private HttpUrlUtils.OnResponse callback;
        private String serverData;


        public HttpAsyncTask(String url, boolean isPost, HttpUrlUtils.OnResponse callback, Map data) {
            this.url = url;
            this.isPost = isPost;
            this.callback = callback;
            this.data = data;
        }

        @Override
        protected void onPreExecute() {
            super.onPreExecute();
            URL reqUrl = null;
            try {
                reqUrl = new URL(url);
            } catch (MalformedURLException e) {
                e.printStackTrace();
            }
            try {
                conn = (HttpURLConnection) reqUrl.openConnection();
            } catch (IOException e) {
                e.printStackTrace();
            }

            conn.setRequestProperty("Connection", "Keep-Alive");
            conn.setRequestProperty("Charset", "UTF-8");

            try {
                if (isPost) {
                    serverData = chageDataToserver(data);
                }
            } catch (UnsupportedEncodingException e) {
                e.printStackTrace();
            }

            if (isPost) {
                try {
                    conn.setRequestMethod("POST");
                    conn.setDoOutput(true);
                    conn.setDoInput(true);
                    conn.setUseCaches(false);
                    conn.setInstanceFollowRedirects(true);
                    conn.setRequestProperty("accept", "application/json");
                    conn.setConnectTimeout(10000);
                    conn.setReadTimeout(10000);
                    //conn.setRequestProperty("Content-Length", String.valueOf(data.getBytes().length));
                    // conn.setDoInput(true);
                } catch (ProtocolException e) {
                    e.printStackTrace();
                }
            } else {
                try {
                    conn.setRequestMethod("GET");
                    //conn.setDoOutput(true);
                    conn.setDoInput(true);
                    conn.setConnectTimeout(10000);
                    conn.setReadTimeout(10000);

                } catch (ProtocolException e) {
                    e.printStackTrace();
                }
            }
            //   conn.setInstanceFollowRedirects(true);

        }


        @Override
        protected Object doInBackground(Object[] objects) {
           /*if(!isPost) {
               try {
                 //  conn.connect();
               } catch (IOException e) {
                   e.printStackTrace();
               }
           }*/

            logD("Post =  " + isPost + " post to server data  = " + data);
            logD("Post url = " + url);
            DataOutputStream out = null;
            if (isPost) {

                try {
                    out = new DataOutputStream(conn.getOutputStream());
                } catch (IOException e) {

                    e.printStackTrace();

                    return Constants.MESSAGE_TOAST+"";
                }
                try {
                    out.writeBytes(serverData);
                } catch (IOException e) {
                    e.printStackTrace();
                    return Constants.MESSAGE_TOAST+"";
                }
                try {
                    out.flush();
                } catch (IOException e) {
                    e.printStackTrace();
                    return Constants.MESSAGE_TOAST+"";
                }
            } else {
                try {
                    conn.connect();
                } catch (IOException e) {
                    e.printStackTrace();
                }

            }

            int code = 0;
            try {
                code = conn.getResponseCode();
            } catch (IOException e) {
                e.printStackTrace();
            }

            logD("ResponseCode = " + code);


            if (code == 200) {
                InputStream in = null;

                try {
                    in = conn.getInputStream();
                } catch (IOException e) {
                    e.printStackTrace();
                }


                // Send http request, convert response to String
                ByteArrayOutputStream outStream = new ByteArrayOutputStream();
                byte[] buffer = new byte[1024];
                int len;
                try {
                    while ((len = in.read(buffer)) != -1) {
                        outStream.write(buffer, 0, len);
                    }
                } catch (IOException e) {
                    e.printStackTrace();
                }
                byte[] data = outStream.toByteArray();
                try {
                    if (out != null) {
                        out.close();
                    }
                } catch (IOException e) {
                    e.printStackTrace();
                }
                try {
                    outStream.close();
                } catch (IOException e) {
                    e.printStackTrace();
                }

                String json = new String(data);
                logD("data from server" + json);
                return json;
            } else {
                logD("ResponseCode  =    " +"error");
                return "";
            }
        }

        @Override
        protected void onPostExecute(Object o) {
            super.onPostExecute(o);


            try {
                callback.onResponse((String) o);
            } catch (JSONException e) {
                e.printStackTrace();
            }
        }
    }


    private void logD(String message) {
        Log.d("zhang  HttpUrlUtils", message);
    }




    public String chageDataToserver(Map parameters) throws UnsupportedEncodingException {

        //String responseContent = null;

        StringBuilder params = new StringBuilder();
        if (parameters.size() > 0) {
            for (Iterator iter = parameters.entrySet().iterator(); iter
                    .hasNext(); ) {
                Entry element = (Entry) iter.next();
                params.append(element.getKey().toString());
                params.append("=");
                params.append(URLEncoder.encode(element.getValue().toString(),
                        "UTF-8"));
                params.append("&");
            }
            params = params.deleteCharAt(params.length() - 1);
            logD(" sendDataToServer   =  " + params.toString());
            return params.toString();

        }
        return null;
    }

}
