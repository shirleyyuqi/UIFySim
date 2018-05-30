package fyuilib.luoc.com.control;

import android.content.Context;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.Paint;
import android.os.Build;
import android.view.MotionEvent;
import android.view.View;
import android.widget.ImageButton;
import android.widget.TextView;

import org.w3c.dom.Text;

import fyuilib.luoc.com.core.UIManager;

/**
 * Created by luoc on 2018/5/5.
 */

public class CLabelUI extends View implements UIAttribute{
    //	public static final int TEXT_ALIGN_CENTER            = 0x00000000;
    public static final int TEXT_ALIGN_LEFT              = 0x00000001;
    public static final int TEXT_ALIGN_RIGHT             = 0x00000010;
    public static final int TEXT_ALIGN_CENTER_VERTICAL   = 0x00000100;
    public static final int TEXT_ALIGN_CENTER_HORIZONTAL = 0x00001000;
    public static final int TEXT_ALIGN_TOP               = 0x00010000;
    public static final int TEXT_ALIGN_BOTTOM            = 0x00100000;

    private CCommonFunUI m_pCommonUI = null;
    private int          m_iTextIndent = 0;
    private String       m_text = null;

    private int          m_textAlign = TEXT_ALIGN_CENTER_HORIZONTAL | TEXT_ALIGN_CENTER_VERTICAL;;
    private Paint        m_paint = null;

    /** 文本中轴线X坐标 */
    private float textCenterX;
    /** 文本baseline线Y坐标 */
    private float textBaselineY;


    public CLabelUI(Context context, UIManager manager) {
        super(context);

        m_pCommonUI = new CCommonFunUI();
        m_pCommonUI.initWithDelegateView(this, manager);
        m_paint = new Paint();
        m_paint.setAntiAlias(true);
        m_paint.setTextAlign(Paint.Align.CENTER);
    }

    //@android.support.annotation.RequiresApi(api = Build.VERSION_CODES.JELLY_BEAN_MR1)
    @Override
    public void SetAttribute(String name, String value) {
        if (name.compareTo("text") == 0){
            setText(value);
        }else if (name.compareTo("align") == 0){
            if (value.compareTo("center") == 0){
                m_textAlign = TEXT_ALIGN_CENTER_VERTICAL | TEXT_ALIGN_CENTER_HORIZONTAL;
            }else if (value.compareTo("right") == 0){
                m_textAlign = TEXT_ALIGN_CENTER_VERTICAL | TEXT_ALIGN_RIGHT;
            }else {
                m_textAlign = TEXT_ALIGN_CENTER_VERTICAL | TEXT_ALIGN_LEFT;
            }
        }else if (name.compareTo("textcolor") == 0){
            setTextColor(Color.parseColor(value));
        }else if (name.compareTo("font") == 0){
            setTextSize(Integer.parseInt(value));
        }else if (name.compareTo("textIndent") == 0){
            m_iTextIndent = UIManager.dip2px(this.getContext(), Integer.parseInt(value));
        }else{
            this.GetCommFun().setAttribute(name, value);
        }
    }

    @Override
    public CCommonFunUI GetCommFun() {
        return m_pCommonUI;
    }

    @Override
    protected void onDraw(Canvas canvas) {
        setTextPosition();
        if (m_text != null)
            canvas.drawText(m_pCommonUI.getLanString(m_text), textCenterX + m_iTextIndent, textBaselineY, m_paint);
        super.onDraw(canvas);
    }

    /**
     * 定位文本绘制的位置
     */
    private void setTextPosition() {
        Paint.FontMetrics fm = m_paint.getFontMetrics();
        //文本的宽度
        float textWidth = m_paint.measureText(m_text);
        float textCenterVerticalBaselineY = getHeight() / 2 - fm.descent + (fm.descent - fm.ascent) / 2;
        switch (m_textAlign) {
            case TEXT_ALIGN_CENTER_HORIZONTAL | TEXT_ALIGN_CENTER_VERTICAL:
                textCenterX = (float)getWidth() / 2;
                textBaselineY = textCenterVerticalBaselineY;
                break;
            case TEXT_ALIGN_LEFT | TEXT_ALIGN_CENTER_VERTICAL:
                textCenterX = textWidth / 2;
                textBaselineY = textCenterVerticalBaselineY;
                break;
            case TEXT_ALIGN_RIGHT | TEXT_ALIGN_CENTER_VERTICAL:
                textCenterX = getWidth() - textWidth / 2;
                textBaselineY = textCenterVerticalBaselineY;
                break;
            case TEXT_ALIGN_BOTTOM | TEXT_ALIGN_CENTER_HORIZONTAL:
                textCenterX = getWidth() / 2;
                textBaselineY = getHeight() - fm.bottom;
                break;
            case TEXT_ALIGN_TOP | TEXT_ALIGN_CENTER_HORIZONTAL:
                textCenterX = getWidth() / 2;
                textBaselineY = -fm.ascent;
                break;
            case TEXT_ALIGN_TOP | TEXT_ALIGN_LEFT:
                textCenterX = textWidth / 2;
                textBaselineY = -fm.ascent;
                break;
            case TEXT_ALIGN_BOTTOM | TEXT_ALIGN_LEFT:
                textCenterX = textWidth / 2;
                textBaselineY = getHeight() - fm.bottom;
                break;
            case TEXT_ALIGN_TOP | TEXT_ALIGN_RIGHT:
                textCenterX = getWidth() - textWidth / 2;
                textBaselineY = -fm.ascent;
                break;
            case TEXT_ALIGN_BOTTOM | TEXT_ALIGN_RIGHT:
                textCenterX = getWidth() - textWidth / 2;
                textBaselineY = getHeight() - fm.bottom;
                break;
        }
    }

    public void setText(String text) {
        m_text = text;
        invalidate();
    }

    public void setTextSize(int textSizeSp) {
        m_paint.setTextSize(textSizeSp);
        invalidate();
    }

    public void setTextAlign(int textAlign) {
        m_textAlign = textAlign;
        invalidate();
    }

    public void setTextColor(int textColor) {
        m_paint.setColor(textColor);
        invalidate();
    }
}