package io.agora.agoraandroidhq.view;

import android.Manifest;
import android.annotation.SuppressLint;
import android.app.Activity;
import android.app.AlertDialog;
import android.content.Intent;
import android.content.pm.ActivityInfo;
import android.content.pm.PackageManager;
import android.content.res.Resources;
import android.graphics.Color;
import android.net.Uri;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.support.annotation.NonNull;
import android.support.v4.app.ActivityCompat;
import android.support.v4.content.ContextCompat;
import android.util.Log;
import android.view.View;
import android.view.Window;
import android.view.WindowManager;
import android.widget.Button;
import android.widget.ImageButton;
import android.widget.ImageView;
import android.widget.TextView;
import android.widget.Toast;

import org.json.JSONException;
import org.json.JSONObject;


import io.agora.agoraandroidhq.R;
import io.agora.agoraandroidhq.control.AgoraLinkToCloud;
import io.agora.agoraandroidhq.tools.Constants;
import io.agora.agoraandroidhq.tools.HttpUrlUtils;
import io.rong.imlib.RongIMClient;
import io.rong.imlib.model.UserInfo;

public class MainActivity extends Activity {

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        requestWindowFeature(Window.FEATURE_NO_TITLE);
        setContentView(R.layout.main_view);
        this.getWindow().setFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN, WindowManager.LayoutParams.FLAG_FULLSCREEN);

        setRequestedOrientation(ActivityInfo.SCREEN_ORIENTATION_PORTRAIT);
        findButton();


    }

    @Override
    protected void onResume() {
        super.onResume();
        playGame.setClickable(true);
    }

    private Button playGame;
    private ImageButton questionImage;

    private void findButton() {
        questionImage = findViewById(R.id.image_question);
        questionImage.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                AlertDialog.Builder builder = new AlertDialog.Builder(MainActivity.this, AlertDialog.THEME_DEVICE_DEFAULT_LIGHT);
                builder.setTitle("Suggestion");

                View view = View.inflate(MainActivity.this, R.layout.dialog_view, null);
                // Toast.makeText(MainActivity.this,"hehe ",Toast.LENGTH_SHORT).show();

                final TextView tel = view.findViewById(R.id.question_tel_text);
                tel.setOnClickListener(new View.OnClickListener() {
                    @Override
                    public void onClick(View v) {

                        if (ActivityCompat.checkSelfPermission(MainActivity.this, Manifest.permission.CALL_PHONE) != PackageManager.PERMISSION_GRANTED) {
                            // TODO: Consider calling
                            //    ActivityCompat#requestPermissions
                            // here to request the missing permissions, and then overriding
                            //   public void onRequestPermissionsResult(int requestCode, String[] permissions,
                            //                                          int[] grantResults)
                            // to handle the case where the user grants the permission. See the documentation
                            // for ActivityCompat#requestPermissions for more details.
                            ActivityCompat.requestPermissions(MainActivity.this, new String[]{Manifest.permission.CALL_PHONE}, 200);
                        } else {
                            Intent intent = new Intent(Intent.ACTION_CALL, Uri.parse(tel.getText().toString()));
                            startActivity(intent);
                        }
                    }
                });

                builder.setView(view);
                builder.setCancelable(true);
                builder.create().show();


            }
        });


        playGame = findViewById(R.id.btnStartPlayGame);

        playGame.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {

                try {
                    playGame.setClickable(false);
                    loginAndConnect("yourid", "password", "http://yourImageUrl");
                } catch (JSONException e) {
                    e.printStackTrace();
                }
               // playGame.setClickable(false);
            }
        });
    }


    @SuppressLint("MissingPermission")
    @Override
    public void onRequestPermissionsResult(int requestCode, @NonNull String[] permissions, @NonNull int[] grantResults) {
        super.onRequestPermissionsResult(requestCode, permissions, grantResults);
        if (requestCode == 200) {
            Log.d("zhangpermission  " , grantResults[0]+"");
            if(grantResults[0] == PackageManager.PERMISSION_GRANTED) {
                Intent intent = new Intent(Intent.ACTION_CALL, Uri.parse("tel:400 632 6626"));
                startActivity(intent);
            }
        }
    }

    @Override
    protected void onStart() {
        super.onStart();


    }

    private void loginAndConnect(String id, String password, String url) throws JSONException {

        final UserInfo userInfo = AgoraLinkToCloud.getLoginUser(id, password);

        AgoraLinkToCloud.getToken(userInfo, new HttpUrlUtils.OnResponse() {


            @Override
            public void onResponse(String body) {

                String token;

                if (body == null) {
                    Toast.makeText(MainActivity.this, R.string.net_error, Toast.LENGTH_SHORT).show();

                    playGame.setClickable(true);
                    return;
                }
                try {


                    JSONObject jsonObject = new JSONObject(body);
                    logD(jsonObject.toString());
                    token = jsonObject.getString("token");
                    logD("token  = " + token);
                } catch (JSONException e) {
                    Toast.makeText(MainActivity.this, R.string.error_to_resolve_token, Toast.LENGTH_SHORT).show();
                    logD("token   catchException" + e);
                    playGame.setClickable(true);
                    return;
                }

                logD("connect");

                //String tokens = "gX1Mw1M2/GgcKjcEqYkSE+JEGPi2u0lwmZGRhExpByVM4V3Bog9dAwIT/trJYlHj8a20NtADe7y2nHvNwatRm7Kp6+douBexDuzXJli5HQY=";
                AgoraLinkToCloud.connect(token, new RongIMClient.ConnectCallback() {
                    @Override
                    public void onTokenIncorrect() {
                        logD("onTokenIncorrect");
                    }

                    @Override
                    public void onSuccess(String s) {

                        AgoraLinkToCloud.setCurrentUser(userInfo);
                        Intent intent = new Intent(MainActivity.this, GameActivity.class);
                        startActivity(intent);
                        logD("onConnectSuccess");
                        playGame.setClickable(true);

                    }

                    @Override
                    public void onError(RongIMClient.ErrorCode errorCode) {
                       // logD("11111111");
                        logD("onError  errorCode = " + errorCode);

                        playGame.setClickable(true);
                    }
                });
    /*}
        });*/
            }

            private void logD(String message) {

                Log.d("zhangHQ  ", message);
            }
        });
    }
}