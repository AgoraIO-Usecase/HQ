package agora.io.agorahq.ui;

import android.graphics.Rect;
import android.support.v7.widget.RecyclerView;
import android.view.View;

public class DefaultDecoration extends RecyclerView.ItemDecoration{
    int divider = 6;
    int header = 4;
    int footer = 4;
    @Override
    public void getItemOffsets(Rect outRect, View view, RecyclerView parent, RecyclerView.State state) {
        super.getItemOffsets(outRect, view, parent, state);

        int itemCount = parent.getAdapter().getItemCount();
        int viewPostion = parent.getChildLayoutPosition(view);

        outRect.left = divider;
        outRect.right = divider;

        if (viewPostion == 0) {
            outRect.top = header;
            outRect.bottom = divider/2;
        } else if (viewPostion == itemCount -1){
            outRect.top = divider/2;
            outRect.bottom = footer;
        } else {
            outRect.top = divider /2 ;
            outRect.bottom = divider /2;
        }
    }
}
