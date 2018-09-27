package agora.io.agorahq.activity;

import android.Manifest;
import android.app.Activity;
import android.app.AlertDialog;
import android.content.Intent;
import android.content.pm.PackageManager;
import android.database.Cursor;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.graphics.drawable.BitmapDrawable;
import android.graphics.drawable.Drawable;
import android.net.Uri;
import android.provider.MediaStore;
import android.support.annotation.Nullable;
import android.os.Bundle;
import android.support.v4.app.ActivityCompat;
import android.support.v4.content.ContextCompat;
import android.text.TextUtils;
import android.view.View;
import android.widget.Button;
import android.widget.EditText;
import android.widget.ImageView;

import agora.io.agorahq.GlobalApplication;
import agora.io.agorahq.R;
import agora.io.agorahq.bean.User;
import agora.io.agorahq.utils.Bitmap2DrawableUtil;
import agora.io.agorahq.utils.Constants;
import agora.io.agorahq.utils.LogUtil;
import agora.io.agorahq.utils.PreferenceUtil;
import agora.io.agorahq.utils.StringUtil;

public class MainActivity extends BaseActivity {
    private final static String PREFERENCE_NAME = MainActivity.class.getSimpleName();

    private Button mBtnPlayGame;
    private EditText mEtChannelNameEditText;
    private EditText mEtLabelName;
    private ImageView mIvLabelImage;

    private PreferenceUtil mPreferenceUtil;
    private LogUtil mLogUtil;

    private Drawable mPersonDrawable;
    private String mPersonName;
    private String mChannelName;
    private int RESULT_ACTIVITY_CODE = 1;

    @Override
    protected void onCreate(@Nullable Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
    }

    @Override
    protected void initUIandEvent() {
        mPreferenceUtil = PreferenceUtil.getInstance(this, PREFERENCE_NAME);
        mLogUtil = LogUtil.newInstance();

        mEtChannelNameEditText = findViewById(R.id.channel_name_editText);

        mBtnPlayGame = findViewById(R.id.btnStartPlayGame);
        mBtnPlayGame.setClickable(true);

        mEtLabelName = findViewById(R.id.user_label);
        mIvLabelImage = findViewById(R.id.user_image);

        mPersonName = mPreferenceUtil.getString(Constants.PREFERENCE_PERSON_NAME_KEY, StringUtil.genRandomName(8));
        mLogUtil.log("set user name:" + mPersonName);
        mEtLabelName.setText(mPersonName);

        mPersonDrawable = mPreferenceUtil.getDrawable(Constants.PREFERENCE_PERSON_IMAGE_KEY, BitmapFactory.decodeResource(getResources(), R.drawable.icon_default_user));
        mLogUtil.log("set person drawable!");
        mIvLabelImage.setImageBitmap(Bitmap2DrawableUtil.conver2Bitmap(mPersonDrawable));

        mChannelName = mPreferenceUtil.getString(Constants.PREFERENCE_CHANNEL_NAME_KEY, StringUtil.genRandomName(16));
        mLogUtil.log("set channel name:" + mChannelName);
        mEtChannelNameEditText.setText(mChannelName);

        ((GlobalApplication) getApplication()).initWorkThread();
        mLogUtil.log("init work thread in mainActivity!");
    }

    @Override
    protected void deInitUIandEvent() {
//        ((GlobalApplication) getApplication()).deinitWorkThread();
        mLogUtil.log("deinit work thread in mainActivity!");
    }

    public void startPlayClick(View v) {
        mChannelName = mEtChannelNameEditText.getText().toString().trim();
        if (TextUtils.isEmpty(mChannelName)) {
            showShortToast("Please input a channel name!");
            return;
        }

        mPersonName = mEtLabelName.getText().toString().trim();
        if (TextUtils.isEmpty(mPersonName)) {
            showShortToast("Please input a user name!");
            return;
        }

        mPreferenceUtil.putStringByCommit(Constants.PREFERENCE_PERSON_NAME_KEY, mPersonName);
        mPreferenceUtil.putStringByCommit(Constants.PREFERENCE_CHANNEL_NAME_KEY, mChannelName);

        // set 4 life
        mPreferenceUtil.putIntByCommit(Constants.PREFERENCE_RELIVE_COUNT_KEY, 4);

        User user = new User(mPersonName, mPersonName, mChannelName, mPersonDrawable);

        ((GlobalApplication) getApplication()).getGlobalGameInfo().updateUser(user);
        mLogUtil.log("update user: " + user.toString());

        Intent intent = new Intent(MainActivity.this, GameActivity.class);
        startActivity(intent);
    }

    public void onUserImageClicked(View v) {
        if (ContextCompat.checkSelfPermission(this, Manifest.permission.READ_EXTERNAL_STORAGE) != PackageManager.PERMISSION_GRANTED) {
            ActivityCompat.requestPermissions(this,
                    new String[]{Manifest.permission.READ_EXTERNAL_STORAGE},
                    0);
        } else {
            Intent intent = new Intent(Intent.ACTION_PICK,
                    android.provider.MediaStore.Images.Media.EXTERNAL_CONTENT_URI);
            startActivityForResult(intent, RESULT_ACTIVITY_CODE);
        }
    }

    public void onQuestionImageClick(View v) {
        AlertDialog.Builder builder = new AlertDialog.Builder(MainActivity.this, AlertDialog.THEME_DEVICE_DEFAULT_LIGHT);
        View view = View.inflate(MainActivity.this, R.layout.main_activity_question_layout, null);
        builder.setView(view);
        builder.setCancelable(true);
        builder.create().show();
    }

    @Override
    protected void onActivityResult(int requestCode, int resultCode, Intent data) {
        super.onActivityResult(requestCode, resultCode, data);

        if (requestCode == RESULT_ACTIVITY_CODE && resultCode == Activity.RESULT_OK && data != null) {
            Uri selectedImage = data.getData();
            String[] filePathColumns = {MediaStore.Images.Media.DATA};
            String imagePath;
            if (selectedImage != null) {
                Cursor c = getContentResolver().query(selectedImage, filePathColumns, null, null, null);
                assert c != null;
                c.moveToFirst();
                int columnIndex = c.getColumnIndex(filePathColumns[0]);
                imagePath = c.getString(columnIndex);
                mLogUtil.log("user image filePath:" + imagePath);
                if (imagePath != null) {
                    mPersonDrawable = Bitmap2DrawableUtil.conver2Drawable(getResources(), getFitSampleBitmap(imagePath, 150, 150));
                    mPreferenceUtil.putDrawableByCommit(Constants.PREFERENCE_PERSON_IMAGE_KEY, ((BitmapDrawable) mPersonDrawable).getBitmap());
                    mIvLabelImage.setImageDrawable(mPersonDrawable);
                    System.gc();
                    c.close();
                }
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
