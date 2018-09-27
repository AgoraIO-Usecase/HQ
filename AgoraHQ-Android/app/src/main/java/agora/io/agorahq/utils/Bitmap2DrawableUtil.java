package agora.io.agorahq.utils;

import android.content.res.Resources;
import android.graphics.Bitmap;
import android.graphics.drawable.BitmapDrawable;
import android.graphics.drawable.Drawable;

public class Bitmap2DrawableUtil {
    public static Bitmap conver2Bitmap(Drawable drawable) {
        return ((BitmapDrawable)(drawable)).getBitmap();
    }

    public static Drawable conver2Drawable(Resources res, Bitmap bitmap) {
        return new BitmapDrawable(res, bitmap);
    }
}
