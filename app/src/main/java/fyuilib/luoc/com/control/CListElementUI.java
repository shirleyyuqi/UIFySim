package fyuilib.luoc.com.control;

import android.content.Context;
import android.graphics.Canvas;
import android.graphics.Paint;
import android.graphics.Rect;
import android.view.View;
import android.view.ViewGroup;

import fyuilib.luoc.com.core.UIManager;

/**
 * Created by luoc on 2018/5/5.
 */

public class CListElementUI extends ViewGroup implements UIAttribute{
    private CCommonFunUI m_pCommonUI = null;
    private int          m_iLayoutStyle = 0;
    private int          m_iAccessoryType = 0;
    private boolean      m_bAccessoryTypeSet = false;
    public  int          iFlag = 0;
    public  int          mIndex = 0;

    public CListElementUI(Context context, UIManager manager) {
        super(context);

        m_pCommonUI = new CCommonFunUI();
        m_pCommonUI.initWithDelegateView(this, manager);
        this.setBackgroundColor(0xFFFFFFFF);
    }

    public void SetAccessoryType(int type){
        if (!m_bAccessoryTypeSet){
            m_iAccessoryType = type;
        }
    }

    @Override
    public void SetAttribute(String name, String value) {
        if (name.compareTo("style") == 0) {
            m_iLayoutStyle = Integer.parseInt(value);
        }else if(name.compareTo("accessorytype") == 0){
            m_iAccessoryType = Integer.parseInt(value);
            m_bAccessoryTypeSet = true;
        }else if(name.compareTo("selecttype") == 0){
            //_iSelectType = atoi(value);
        }else{
            m_pCommonUI.setAttribute(name, value);
        }
    }

    @Override
    public CCommonFunUI GetCommFun() {
        return m_pCommonUI;
    }

    @Override
    protected void onLayout(boolean changed, int l, int t, int r, int b) {
        Rect frame = new Rect(0, 0, r-l, b-t);
        if (m_iLayoutStyle == 0) {
            m_pCommonUI.SetFrameOfVertical(frame, this);
        }else if(m_iLayoutStyle == 1){
            m_pCommonUI.SetFrameOfHorizontal(frame, this);
        }else if (m_iLayoutStyle == 3){
            m_pCommonUI.SetFrameOfOverlap(frame, this);
        }
    }

    @Override
    protected void onDraw(Canvas canvas) {
        super.onDraw(canvas);

        if (m_iAccessoryType == 1) {
            Paint paint = new Paint();
            paint.setColor(0x88888888);
            paint.setStrokeWidth(5);
            paint.setAntiAlias(true);
            canvas.drawLine(getWidth() - UIManager.dip2px(getContext(), 25)
                    , getHeight() / 2 - UIManager.dip2px(getContext(), 8)
                    , getWidth() - UIManager.dip2px(getContext(), 15)
                    , getHeight() / 2 + 1, paint);

            canvas.drawLine(getWidth() - UIManager.dip2px(getContext(), 25)
                    , getHeight() / 2 + UIManager.dip2px(getContext(), 8)
                    , getWidth() - UIManager.dip2px(getContext(), 15)
                    , getHeight() / 2, paint);
        }
    }
}
