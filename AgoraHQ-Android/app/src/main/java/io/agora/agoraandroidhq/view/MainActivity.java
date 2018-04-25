package io.agora.agoraandroidhq.view;

import android.Manifest;
import android.annotation.SuppressLint;
import android.app.Activity;
import android.app.AlertDialog;
import android.content.Intent;
import android.content.SharedPreferences;
import android.content.pm.ActivityInfo;
import android.content.pm.PackageManager;
import android.database.Cursor;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.graphics.Color;
import android.graphics.Matrix;
import android.graphics.drawable.BitmapDrawable;
import android.graphics.drawable.Drawable;
import android.net.Uri;
import android.os.Bundle;
import android.provider.MediaStore;
import android.support.annotation.NonNull;
import android.support.v4.app.ActivityCompat;
import android.support.v4.content.ContextCompat;
import android.support.v7.widget.AppCompatImageView;
import android.text.Editable;
import android.text.TextUtils;
import android.text.TextWatcher;
import android.util.Log;
import android.view.View;
import android.view.Window;
import android.view.WindowManager;
import android.widget.Button;
import android.widget.EditText;
import android.widget.ImageButton;
import android.widget.ImageView;
import android.widget.TextView;
import android.widget.Toast;
import io.agora.agoraandroidhq.HqApplication;
import io.agora.agoraandroidhq.R;
import io.agora.agoraandroidhq.tools.Constants;
import io.agora.agoraandroidhq.tools.DecryptData;
import io.agora.agoraandroidhq.tools.DecryptHelpter;
import io.agora.agoraandroidhq.tools.GameControl;
import io.agora.agoraandroidhq.tools.SharedPreferenceHelper;
import io.agora.rtc.RtcEngine;

public class MainActivity extends BaseActivity {

    private Button playGame;
    private ImageButton questionImage;
    private EditText channelNameEditText;
    private EditText labelName;
    private ImageView labelImage;
    private SharedPreferenceHelper sharedPreferenceHelper;
    private static final int IMAGE = 1;
    private Bitmap currentBitmap;
    private String tag = "[MainActivity]  ";

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.main_view);
        initUIandEvent();
        setUiListener();
        initConstantsHttp();
    }

    @Override
    protected void onResume() {
        super.onResume();
        playGame.setClickable(true);
        channelNameEditText.requestFocus();
        labelImage.setImageDrawable(null);
        setUserImageFormSharedPreference(labelImage);
    }

    @Override
    protected void initUIandEvent() {
        GameControl.context = getApplicationContext();
        GameControl.deleteLogFile();
        channelNameEditText = findViewById(R.id.channel_name_editText);
        playGame = findViewById(R.id.btnStartPlayGame);
        labelName = findViewById(R.id.user_label);
        labelImage = findViewById(R.id.user_image);
        String userNameFromSharedPreference = getNameFromSharedPreference();
        labelName.setText(userNameFromSharedPreference);
    }

    private String getNameFromSharedPreference() {
        GameControl.logD(tag + "getNameFromSharedPreference");
        String userName = SharedPreferenceHelper.newInstance(getApplicationContext()).getName();
        return userName;
    }

    private void setUserImageFormSharedPreference(ImageView imageView) {
        Drawable drawable = getDrawableFromSharedPreference();
        if (drawable == null) {
            return;
        } else {
            imageView.setImageDrawable(drawable);
        }
    }

    private Drawable getDrawableFromSharedPreference() {
        Drawable drawable = SharedPreferenceHelper.newInstance(getApplicationContext()).getDrawable();
        return drawable;
    }

    private void setOnEditTextChangeListener(final EditText editText) {
        editText.addTextChangedListener(new TextWatcher() {
            @Override
            public void beforeTextChanged(CharSequence s, int start, int count, int after) {
            }

            @Override
            public void onTextChanged(CharSequence s, int start, int before, int count) {
            }

            @Override
            public void afterTextChanged(Editable s) {
                String userName = editText.getText().toString();
                if (TextUtils.isEmpty(userName)) {
                    editText.setHint("Label");
                    editText.setHintTextColor(Color.GRAY);
                    GameControl.logD(tag + "text is null");
                } else {

                }
            }
        });
    }

    public void startClick(View view) {
        String ChannelName = channelNameEditText.getText().toString().trim();
        String userName = labelName.getText().toString().trim();
        if (TextUtils.isEmpty(ChannelName)) {
            Toast.makeText(MainActivity.this, R.string.text_channel_name_can_not_be_null, Toast.LENGTH_SHORT).show();
        } else {
            playGame.setClickable(false);
            Intent intent = new Intent(MainActivity.this, GameActivity.class);
            intent.putExtra("ChannelName", ChannelName);
            startActivity(intent);
            // logD("onConnectSuccess");
            sharedPreferenceHelper.saveName(userName);
            GameControl.currentUserHeadImage = labelImage.getDrawable();
            GameControl.currentUserName = labelName.getText().toString().trim();
            GameControl.signalAccount = labelName.getText().toString().trim();
            // GameControl.logD("saveName = " + userName);
        }
    }

    public void questionImageClick(View views) {
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


    @SuppressLint("MissingPermission")
    @Override
    public void onRequestPermissionsResult(int requestCode, @NonNull String[] permissions, @NonNull int[] grantResults) {
        super.onRequestPermissionsResult(requestCode, permissions, grantResults);
        if (requestCode == 200) {
            Log.d("zhangpermission  ", grantResults[0] + "");
            if (grantResults[0] == PackageManager.PERMISSION_GRANTED) {
                Intent intent = new Intent(Intent.ACTION_CALL, Uri.parse("tel:400 632 6626"));
                startActivity(intent);
            }
        } else if (requestCode == 0) {
            if (grantResults[0] == PackageManager.PERMISSION_GRANTED) {
                Intent intent = new Intent(Intent.ACTION_PICK,
                        android.provider.MediaStore.Images.Media.EXTERNAL_CONTENT_URI);
                startActivityForResult(intent, IMAGE);
            } else {
                Toast.makeText(MainActivity.this, R.string.text_do_not_have_permission, Toast.LENGTH_SHORT).show();
            }
        }
    }

    @Override
    protected void onStart() {
        super.onStart();
        sharedPreferenceHelper = getSharedPreference();
        sharedPreferenceHelper.saveDrawable(((BitmapDrawable) labelImage.getDrawable()).getBitmap());
    }

    @Override
    protected void deInitUIandEvent() {
        io.agora.agoraandroidhq.control.WorkerThread workerThread = ((HqApplication) getApplication()).getWorkerThread();
        if (workerThread != null) {
            workerThread.destoryEngine();
            ((HqApplication) getApplication()).deInitWorkerThread();
        }
        GameControl.context = null;
    }

    @Override
    protected void setUiListener() {
        setOnEditTextChangeListener(labelName);
    }

    private SharedPreferenceHelper getSharedPreference() {
        return SharedPreferenceHelper.newInstance(getApplicationContext());
    }

    public void userImage(View v) {
        if (ContextCompat.checkSelfPermission(this,
                Manifest.permission.READ_EXTERNAL_STORAGE)
                != PackageManager.PERMISSION_GRANTED) {
            ActivityCompat.requestPermissions(this,
                    new String[]{Manifest.permission.READ_EXTERNAL_STORAGE},
                    0);
        } else {
            Intent intent = new Intent(Intent.ACTION_PICK,
                    android.provider.MediaStore.Images.Media.EXTERNAL_CONTENT_URI);
            startActivityForResult(intent, IMAGE);
        }
    }

    @Override
    protected void onActivityResult(int requestCode, int resultCode, Intent data) {
        super.onActivityResult(requestCode, resultCode, data);
        if (requestCode == IMAGE && resultCode == Activity.RESULT_OK && data != null) {
            Uri selectedImage = data.getData();
            String[] filePathColumns = {MediaStore.Images.Media.DATA};
            Cursor c = getContentResolver().query(selectedImage, filePathColumns, null, null, null);
            c.moveToFirst();
            int columnIndex = c.getColumnIndex(filePathColumns[0]);
            String imagePath = c.getString(columnIndex);
            GameControl.logD(tag + "bitmapSIze  filePath  =  " + imagePath);
            if (imagePath != null) {
                //GameControl.logD("bitmapSIze =  " + bitmap.getByteCount());
                //Bitmap bitmaps = resizeBitmap(imagePath, 80, 80);
                Bitmap bitmaps = getFitSampleBitmap(imagePath, 150, 150);
                GameControl.logD(tag + "bitmap Size = " + bitmaps.getByteCount());
                if (bitmaps != null) {
                   /* Bitmap bitmap = ((BitmapDrawable) ((ImageView) labelImage).getDrawable()).getBitmap();
                    if(bitmap!=null){
                        bitmap.recycle();
                        bitmap = null;
                    }*/
                    //labelImage.setImageBitmap(0);
                    labelImage.setImageBitmap(bitmaps);
                    sharedPreferenceHelper.saveDrawable(bitmaps);

                    if (currentBitmap != null) {
                        currentBitmap.recycle();
                        currentBitmap = null;
                        currentBitmap = bitmaps;
                    }
                }
                System.gc();
                c.close();
            }
        }
    }

    public static Bitmap getFitSampleBitmap(String file_path, int width, int height) {
        BitmapFactory.Options options = new BitmapFactory.Options();
        options.inJustDecodeBounds = true;
        BitmapFactory.decodeFile(file_path, options);
        options.inSampleSize = getFitInSampleSize(width, height, options);
        options.inJustDecodeBounds = false;
        return BitmapFactory.decodeFile(file_path, options);
    }

    public static int getFitInSampleSize(int reqWidth, int reqHeight, BitmapFactory.Options options) {
        int inSampleSize = 1;
        if (options.outWidth > reqWidth || options.outHeight > reqHeight) {
            int widthRatio = Math.round((float) options.outWidth / (float) reqWidth);
            int heightRatio = Math.round((float) options.outHeight / (float) reqHeight);
            inSampleSize = Math.min(widthRatio, heightRatio);
        }
        return inSampleSize;
    }
}