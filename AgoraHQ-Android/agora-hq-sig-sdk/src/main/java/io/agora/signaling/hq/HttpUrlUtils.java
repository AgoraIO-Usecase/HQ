package io.agora.signaling.hq;

import android.os.Looper;

import java.io.ByteArrayOutputStream;
import java.io.DataOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.net.HttpURLConnection;
import java.net.MalformedURLException;
import java.net.ProtocolException;
import java.net.URL;

public class HttpUrlUtils {

    private static final boolean DBG_ENABLED = BuildConfig.DEBUG;

    public interface OnResponse {
        void onResponse(String data);
    }

    public void execHttpAsyncTask(final String url, final boolean isPost, final HttpUrlUtils.OnResponse callback, final String data) {
        new Thread(new Runnable() {
            @Override
            public void run() {
                new HttpAsyncTask(url, isPost, callback, data).doInBackground();
            }
        }).start();
    }

    public void execHttpSyncTask(final String url, final boolean isPost, final HttpUrlUtils.OnResponse callback, final String data) {
        if (Looper.myLooper() == Looper.getMainLooper()) {
            throw new RuntimeException("the current thread is main thread");
        }
        new HttpAsyncTask(url, isPost, callback, data).doInBackground();
    }

    private class HttpAsyncTask {
        private String mUrl;

        private boolean isPost;

        private HttpURLConnection mConn;
        private String mData;
        private HttpUrlUtils.OnResponse callback;

        public HttpAsyncTask(String url, boolean isPost, HttpUrlUtils.OnResponse callback, String data) {
            this.mUrl = url;
            this.isPost = isPost;
            this.callback = callback;
            this.mData = data;
        }

        void doInBackground() {
            URL reqUrl = null;
            try {
                reqUrl = new URL(mUrl);
            } catch (MalformedURLException e) {
                if (DBG_ENABLED) {
                    e.printStackTrace();
                }
            }
            try {
                mConn = (HttpURLConnection) reqUrl.openConnection();
            } catch (IOException e) {
                if (DBG_ENABLED) {
                    e.printStackTrace();
                }
            }
            mConn.setRequestProperty("Connection", "Keep-Alive");
            mConn.setRequestProperty("Charset", "UTF-8");

            if (isPost) {
                try {
                    mConn.setRequestMethod("POST");
                    mConn.setRequestProperty("Content-Type", "application/json");
                    mConn.setDoOutput(true);
                    mConn.setUseCaches(false);
                    mConn.setRequestProperty("accept", "application/json");
                    mConn.setRequestProperty("Content-Length", String.valueOf(mData.getBytes().length));
                    // mConn.setDoInput(true);
                } catch (ProtocolException e) {
                    if (DBG_ENABLED) {
                        e.printStackTrace();
                    }
                }
            } else {
                try {
                    mConn.setRequestMethod("GET");
                } catch (ProtocolException e) {
                    if (DBG_ENABLED) {
                        e.printStackTrace();
                    }
                }
            }
            // mConn.setInstanceFollowRedirects(true);
            mConn.setConnectTimeout(10000);
            mConn.setReadTimeout(10000);

            DataOutputStream out = null;

            if (isPost) {
                try {
                    out = new DataOutputStream(mConn.getOutputStream());
                    out.writeBytes(mData);
                    out.flush();
                } catch (IOException e) {
                    if (DBG_ENABLED) {
                        e.printStackTrace();
                    }
                }
            }

            int code = 0;
            try {
                code = mConn.getResponseCode();
            } catch (IOException e) {
                e.printStackTrace();
            }

            if (code == HttpURLConnection.HTTP_OK) {
                InputStream in = null;

                try {
                    in = mConn.getInputStream();
                } catch (IOException e) {
                    if (DBG_ENABLED) {
                        e.printStackTrace();
                    }
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
                    if (DBG_ENABLED) {
                        e.printStackTrace();
                    }
                }
                byte[] data = outStream.toByteArray();

                try {
                    if (out != null) {
                        out.close();
                    }
                } catch (IOException e) {
                    if (DBG_ENABLED) {
                        e.printStackTrace();
                    }
                }

                try {
                    outStream.close();
                } catch (IOException e) {
                    if (DBG_ENABLED) {
                        e.printStackTrace();
                    }
                }

                String json = new String(data);
                if (callback != null) {
                    callback.onResponse(json);
                }
            } else {
                if (callback != null) {
                    callback.onResponse(null);
                }
            }
        }
    }
}
