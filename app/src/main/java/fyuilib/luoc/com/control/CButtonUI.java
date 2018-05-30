package fyuilib.luoc.com.control;

import android.content.Context;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.Paint;
import android.graphics.Path;
import android.graphics.RectF;
import android.view.Gravity;
import android.view.MotionEvent;
import android.view.View;
import android.view.ViewGroup;
import android.widget.Button;
import android.widget.ImageButton;
import android.widget.TextView;

import java.lang.reflect.Field;

import fyuilib.luoc.com.core.UIManager;

/**
 * Created by luoc on 2018/5/5.
 */

public class CButtonUI extends ViewGroup implements UIAttribute {
    private CCommonFunUI m_pCommonUI = null;
    private ImageButton  m_pImageBtUI = null;
    private TextView     m_pTextUI = null;
    private int  m_iNormalImgId = 0;
    private int  m_iSelectImgId = 0;
    private int  m_iDisableImgId = 0;
    private int  m_iNormalColorId = 0;
    private int  m_iHotColorId = 0;
    private int m_iLabelAlign = 0;
    private int m_iPos = 0;

    private Paint   m_Paint = new Paint();
    private RectF   m_rtItem = new RectF(0, 0, 0, 0);
    private Path    m_Path = new Path();



    public CButtonUI(Context context, UIManager manager) {
        super(context);

        m_pCommonUI = new CCommonFunUI();
        m_pCommonUI.initWithDelegateView(this, manager);
        m_pImageBtUI = new ImageButton(context);
        m_pImageBtUI.setBackgroundColor(0);
        m_pImageBtUI.setTag(this);
        this.addView(m_pImageBtUI);

        m_pImageBtUI.setOnTouchListener(new View.OnTouchListener() {
            @Override
            public boolean onTouch(View v, MotionEvent event) {
                CButtonUI btn = (CButtonUI)v.getTag();

                if (event.getAction() == MotionEvent.ACTION_DOWN) {
                    if(btn.m_iSelectImgId != 0){
                        btn.m_pImageBtUI.setImageResource(btn.m_iSelectImgId);
                    }else if (m_iHotColorId != 0){
                        btn.m_pImageBtUI.setBackgroundColor(m_iHotColorId);
                    }
                    btn.m_pCommonUI.getUIManager().OnNotify(btn, UIManagerDelegate.FYEVENT_TOUCHDOWN, null);
                    //btn.setTextColor(getResources().getColor(R.color.text_pressed));
                } else if (event.getAction() == MotionEvent.ACTION_UP) {
                    if(btn.m_iNormalImgId != 0){
                        btn.m_pImageBtUI.setImageResource(btn.m_iNormalImgId);
                    }else if (m_iNormalColorId != 0){
                        btn.m_pImageBtUI.setBackgroundColor(m_iNormalColorId);
                    }

                    btn.m_pCommonUI.getUIManager().OnNotify(btn, UIManagerDelegate.FYEVENT_TOUCHUPINSIDE, null);
                    //btn.setBackgroundResource(R.mipmap.btn_status_normal);
                    //btn.setTextColor(getResources().getColor(R.color.text_normal));
                }
                return false;
            }
        });
    }

    @Override
    public void SetAttribute(String name, String value) {
        if (name.compareTo("normalImage") == 0) {
            m_iNormalImgId = UIManager.getImage(value);
            this.m_pImageBtUI.setImageResource(this.m_iNormalImgId);
        }else if (name.compareTo("selectImage") == 0) {
            m_iSelectImgId = UIManager.getImage(value);
        }else if (name.compareTo("disableImage") == 0) {
            m_iDisableImgId = UIManager.getImage(value);
        }else if (name.compareTo("bkcolor") == 0) {
            m_iNormalColorId = Color.parseColor(value);
            if (m_iNormalImgId == 0)
                this.setBackgroundColor(m_iNormalColorId);
        }else if (name.compareTo("hotcolor") == 0) {
            m_iHotColorId = Color.parseColor(value);
        }else if(name.compareTo("mode") == 0){
            switch (Integer.parseInt(value)) {
                case 0:
                    this.m_pImageBtUI.setScaleType(ImageButton.ScaleType.CENTER);
                    break;
                case 1:
                    this.m_pImageBtUI.setScaleType(ImageButton.ScaleType.FIT_START);
                    break;
                case 2:
                    this.m_pImageBtUI.setScaleType(ImageButton.ScaleType.FIT_CENTER);
                    break;
                case 3:
                    this.m_pImageBtUI.setScaleType(ImageButton.ScaleType.CENTER_INSIDE);
                    break;
            }
        }else if(name.compareTo("text") == 0){
            if (m_pTextUI == null) {
                m_pTextUI = new CTextUI(getContext(), m_pCommonUI.getUIManager());
                this.addView(m_pTextUI);
            }
            m_pTextUI.setText(m_pCommonUI.getLanString(value));
        }else if(name.compareTo("textcolor") == 0){
            if (m_pTextUI == null) {
                m_pTextUI = new CTextUI(getContext(), m_pCommonUI.getUIManager());
                this.addView(m_pTextUI);
            }

            m_pTextUI.setTextColor(Color.parseColor(value));
        }else if(name.compareTo("align") == 0){
            if (m_pTextUI == null) {
                m_pTextUI = new CTextUI(getContext(), m_pCommonUI.getUIManager());
                this.addView(m_pTextUI);
            }

            m_iLabelAlign = 0;
            if (value.compareTo("center") == 0) {
                m_pTextUI.setGravity(Gravity.CENTER);
            }else if (value.compareTo("right") == 0){
                m_pTextUI.setGravity(Gravity.RIGHT);
            }else if (value.compareTo("left") == 0){
                m_pTextUI.setGravity(Gravity.LEFT);
            }else{
                m_iLabelAlign = 1;
                String s[] = value.split(" ");
                if (s.length == 4){
                    int w = 0, h = 0;
                    if (s[0].compareTo("top") == 0){
                        m_iLabelAlign = 1;
                        m_iPos = Integer.parseInt(s[1]);
                    }else if (s[0].compareTo("bottom") == 0){
                        m_iLabelAlign = 2;
                        m_iPos = Integer.parseInt(s[1]);
                    }else if (s[0].compareTo("left") == 0){
                        m_iLabelAlign = 3;
                        m_iPos = Integer.parseInt(s[1]);
                    }else if (s[0].compareTo("right") == 0){
                        m_iLabelAlign = 4;
                        m_iPos = Integer.parseInt(s[1]);
                    }

                    m_iPos = UIManager.dip2px(getContext(), m_iPos);
                    if (s[2].compareTo("center") == 0) {
                        m_pTextUI.setGravity(Gravity.CENTER);
                    }else if (s[2].compareTo("right") == 0){
                        m_pTextUI.setGravity(Gravity.RIGHT);
                    }else if (s[2].compareTo("left") == 0){
                        m_pTextUI.setGravity(Gravity.LEFT);
                    }

                    m_pTextUI.setTextSize(Integer.parseInt(s[3]));
                }
            }
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
        if (m_pTextUI != null){
            switch (m_iLabelAlign) {
                case 0:
                {
                    m_pImageBtUI.layout(0, 0, r-l, b-t);
                    m_pTextUI.layout(0, 0, r-l, b-t);
                    break;
                }
                case 1:
                {
                    m_pImageBtUI.layout(0, 0 + m_iPos, r-l, b-t);
                    m_pTextUI.layout(0, 0, r-l, m_iPos);
                    break;
                }
                case 2:
                {
                    m_pImageBtUI.layout(0, 0, r-l, b-t-m_iPos);
                    m_pTextUI.layout(0, t + b - m_iPos, r-l, b-t);
                    break;
                }
                case 3:
                {
                    m_pImageBtUI.layout(m_iPos, 0, r-l-m_iPos, b-t);
                    m_pTextUI.layout(0, 0, m_iPos, b-t);
                    break;
                }
                case 4:
                {
                    m_pImageBtUI.layout(0, 0, r-l-m_iPos, b-t);
                    m_pTextUI.layout(r-l-m_iPos, 0, r-l, b-t);
                    break;
                }
            }
        }else{
            m_pImageBtUI.layout(0, 0, r-l, b-t);
        }
    }

  /*  @Override
    protected void onSizeChanged(int w, int h, int oldw, int oldh) {
        super.onSizeChanged(w, h, oldw, oldh);
        m_Path.reset();
        m_rtItem.set(0, 0, w, h);
        m_Path.addRoundRect(m_rtItem ,m_pCommonUI.m_corner, m_pCommonUI.m_corner , Path.Direction.CW);
    }

    @Override
    protected void dispatchDraw(Canvas canvas) {
        canvas.save();
        canvas.clipPath(m_Path);
        super.dispatchDraw(canvas);
        canvas.restore();
    }
    */
}
