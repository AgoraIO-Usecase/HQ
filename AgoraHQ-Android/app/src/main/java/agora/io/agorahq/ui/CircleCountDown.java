package agora.io.agorahq.ui;

import android.content.Context;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.Paint;
import android.graphics.RectF;
import android.support.annotation.Nullable;
import android.util.AttributeSet;
import android.view.View;

import agora.io.agorahq.R;
import agora.io.agorahq.utils.MetricsTransUtil;

public class CircleCountDown extends View {
    private Paint p;
    private Context ctx;

    private float endAn;
    private RectF rect;

    private int diffWH;

    public CircleCountDown(Context context) {
        super(context);
        init(context);
    }

    public CircleCountDown(Context context, @Nullable AttributeSet attrs) {
        super(context, attrs);
        init(context);
    }

    public CircleCountDown(Context context, @Nullable AttributeSet attrs, int defStyleAttr) {
        super(context, attrs, defStyleAttr);
        init(context);
    }

    void updateProcess(int p, int total){
        endAn = (float)(p * Math.toDegrees(Math.PI) * 2 / total);
        postInvalidate();
    }

    void reset(){
        endAn = 0;
        postInvalidate();
    }

    public void init(Context c){
        ctx = c;
        rect = new RectF();
        diffWH = MetricsTransUtil.dip2px(ctx, 3);// we set 3dip offset to adapt video view

        p = new Paint();
        p.setStyle(Paint.Style.STROKE);
        p.setColor(c.getResources().getColor(R.color.agora_option_error));
        p.setStrokeWidth(15);
        p.setAntiAlias(true);

    }

    @Override
    protected void onDraw(Canvas canvas) {
        super.onDraw(canvas);

        rect.set( diffWH, diffWH, canvas.getHeight() - diffWH, canvas.getHeight() - diffWH);
        canvas.drawArc(rect , -90 , endAn, false, p) ;
    }
}
