package fyuilib.luoc.com.control;

import android.content.Context;
import android.graphics.Canvas;
import android.graphics.Paint;
import android.graphics.Path;
import android.graphics.Rect;
import android.graphics.RectF;
import android.view.View;
import android.view.ViewGroup;

import fyuilib.luoc.com.core.UIManager;

/**
 * Created by luoc on 2018/5/5.
 */

public class CLayoutUI extends ViewGroup implements UIAttribute{
    private CCommonFunUI m_pCommonUI = null;
    private int          m_iLayoutStyle = 0;
    private Rect         m_rtInset = new Rect(0, 0, 0, 0);
    private Paint        m_Paint;
    private Path         m_Path;
    private RectF         m_rtItem = new RectF(0, 0, 0, 0);
    public CLayoutUI(Context context, UIManager manager) {
        super(context);

        m_pCommonUI = new CCommonFunUI();
        m_pCommonUI.initWithDelegateView(this, manager);
        m_Paint = new Paint();
        m_Paint.setColor(0x00000000);
        m_Paint.setAntiAlias(true);

        m_Path = new Path();
    }

    @Override
    public void SetAttribute(String name, String value) {
        if (name.compareTo("style") == 0) {
            m_iLayoutStyle = Integer.parseInt(value);
        }else if(name.compareTo("clip") == 0){
            //self.clipsToBounds = atoi(value)%2;
        }else if(name.compareTo("inset") == 0){
            m_rtInset = CCommonFunUI.getRectValue(getContext(), value);
        }else{
            m_pCommonUI.setAttribute(name, value);
        }

    }

    @Override
    protected void onLayout(boolean changed, int l, int t, int r, int b) {
        Rect frame = new Rect(m_rtInset.left, m_rtInset.top, r-l-m_rtInset.right, b-t-m_rtInset.bottom);
        if (m_iLayoutStyle == 0) {
            m_pCommonUI.SetFrameOfVertical(frame, this);
        }else if(m_iLayoutStyle == 1){
            m_pCommonUI.SetFrameOfHorizontal(frame, this);
        }else if (m_iLayoutStyle == 3){
            m_pCommonUI.SetFrameOfOverlap(frame, this);
        }
    }

    @Override
    protected void onSizeChanged(int w, int h, int oldw, int oldh) {
        super.onSizeChanged(w, h, oldw, oldh);
        if (m_pCommonUI.m_corner > 0) {
            m_Path.reset();
            m_rtItem.set(0, 0, w, h);
            m_Path.addRoundRect(m_rtItem, m_pCommonUI.m_corner, m_pCommonUI.m_corner, Path.Direction.CW);
        }
    }

    @Override
    protected void dispatchDraw(Canvas canvas) {
        if (m_pCommonUI.m_corner > 0) {
            canvas.save();

            canvas.clipPath(m_Path);
            setLayerType(View.LAYER_TYPE_SOFTWARE, null);
            if (m_pCommonUI.m_borderSize > 0){
                m_Paint.setStrokeWidth(m_pCommonUI.m_borderSize);
                m_Paint.setColor(m_pCommonUI.m_borderColor);
                m_Paint.setStyle(Paint.Style.STROKE);
            }

            RectF rectF = new RectF(0, 0, getWidth(), getHeight());
            canvas.drawRoundRect(rectF, m_pCommonUI.m_corner, m_pCommonUI.m_corner, m_Paint);

            super.dispatchDraw(canvas);
            canvas.restore();
        }else{
            super.dispatchDraw(canvas);
        }
    }


    @Override
    public CCommonFunUI GetCommFun() {
        return m_pCommonUI;
    }
}
