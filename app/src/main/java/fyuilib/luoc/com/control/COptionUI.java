package fyuilib.luoc.com.control;

import android.content.Context;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.CornerPathEffect;
import android.graphics.Paint;
import android.graphics.Rect;
import android.graphics.RectF;
import android.view.Gravity;
import android.view.MotionEvent;
import android.view.View;
import android.view.ViewGroup;
import android.widget.ImageButton;
import android.widget.TextView;

import fyuilib.luoc.com.core.UIManager;

/**
 * Created by luoc on 2018/5/5.
 */

public class COptionUI extends ViewGroup implements UIAttribute{
    private CCommonFunUI m_pCommonUI = null;
    private ImageButton  m_pImageBtUI = null;
    private TextView     m_pTextUI = null;
    private int m_iSelectImgId =0;
    private int m_iNormalImgId = 0;
    private int m_iSelectedBkColor = 0x00000000;
    private int m_iNormalBkColor = 0x00000000;
    private int m_iTextStyle = 0;
    private int m_iTextNormalColor = 0xFF000000;
    private int m_iTextSelectColor = 0xFF000000;
    private int m_iTextHeight = 0;
    private int m_iNormalColorId = 0x00000000;

    private boolean m_bIsSelected = false;
    private String  m_text = null;

    private String m_sGroup = null;
    private Paint   m_Paint;

    public COptionUI(Context context, UIManager manager) {
        super(context);
        m_pCommonUI = new CCommonFunUI();
        m_pCommonUI.initWithDelegateView(this, manager);
        m_pImageBtUI = new ImageButton(context);
        m_pImageBtUI.setBackgroundColor(0x00000000);
        m_pImageBtUI.setTag(this);
        this.addView(m_pImageBtUI);

        m_Paint = new Paint();
        m_Paint.setAntiAlias(true);
        m_Paint.setStyle(Paint.Style.STROKE);

        m_pImageBtUI.setOnTouchListener(new View.OnTouchListener() {
            @Override
            public boolean onTouch(View v, MotionEvent event) {
                if (event.getAction() != MotionEvent.ACTION_DOWN)
                    return false;

                COptionUI opt = (COptionUI)v.getTag();
                opt.setSelected(true);
                opt.m_pCommonUI.getUIManager().OnNotify(opt, UIManagerDelegate.FYEVENT_OP_SELECTED, null);
                return false;
            }
        });
    }

    public void setSelected(boolean bSelected)
    {
        //m_bIsSelected = bSelected;
        if (m_sGroup != null) {
            m_pCommonUI.getUIManager().ChangeOptionSelectObject(this, m_sGroup);
        }else{
            ChangeSelected(!m_bIsSelected);
        }
    }

    public void ChangeSelected(boolean bSelected)
    {
        m_bIsSelected = bSelected;
        if ( m_bIsSelected ){
            if(m_iSelectImgId != 0){
                m_pImageBtUI.setImageResource(m_iSelectImgId);
            }
            if (m_pTextUI != null){
                m_pTextUI.setTextColor(m_iTextSelectColor);
            }
            if (m_iSelectedBkColor != 0){
                setBackgroundColor(m_iSelectedBkColor);
            }
        }else{
            if(m_iNormalImgId != 0){
                m_pImageBtUI.setImageResource(m_iNormalImgId);
            }
            if (m_pTextUI != null){
                m_pTextUI.setTextColor(m_iTextNormalColor);
            }
            setBackgroundColor(m_iNormalColorId);
        }
    }

    @Override
    public void SetAttribute(String name, String value) {
        if(name.compareTo("group") == 0){
            m_sGroup = value;
            if (m_bIsSelected) {
                m_pCommonUI.getUIManager().ChangeOptionSelectObject(this, m_sGroup);
                if (m_iSelectImgId != 0) {
                    m_pImageBtUI.setImageResource(m_iSelectImgId);
                    //m_pCommonUI.getUIManager().OnNotify(this, UIManagerDelegate.FYEVENT_OP_SELECTED, null);
                }
            }
        }else if (name.compareTo("normalImage") == 0) {
            m_iNormalImgId = UIManager.getImage(value);
            m_pImageBtUI.setImageResource(this.m_iNormalImgId);
        }else if (name.compareTo("selectedImage") == 0) {
            m_iSelectImgId = UIManager.getImage(value);
        }else if (name.compareTo("selectedBkColor") == 0){
            m_iSelectedBkColor = Color.parseColor(value);
            if (m_bIsSelected) {
                setBackgroundColor(m_iSelectedBkColor);
            }
        }else if(name.compareTo("normalBkColor") == 0){
            m_iNormalBkColor = Color.parseColor(value);
            if (!m_bIsSelected) {
                setBackgroundColor(m_iNormalBkColor);
            }
        }else if(name.compareTo("selected") == 0){
            m_bIsSelected = (Integer.parseInt(value) == 1);
            if (m_sGroup != null) {
                m_pCommonUI.getUIManager().ChangeOptionSelectObject(this, m_sGroup);
            }

            if (m_iSelectImgId != 0 && m_bIsSelected) {
                m_pImageBtUI.setImageResource(m_iSelectImgId);
                //m_pCommonUI.getUIManager().OnNotify(this, UIManagerDelegate.FYEVENT_OP_SELECTED, null);
            }


            if (m_pTextUI != null && m_bIsSelected) {
                m_pTextUI.setTextColor(m_iTextSelectColor);
            }
            if (m_bIsSelected){
                setBackgroundColor(m_iSelectedBkColor);
            }
        }else if(name.compareTo("mode") == 0){
            switch (Integer.parseInt(value)) {
                case 0:
                    m_pImageBtUI.setScaleType(ImageButton.ScaleType.CENTER);
                    break;
                case 1:
                    m_pImageBtUI.setScaleType(ImageButton.ScaleType.FIT_CENTER);
                    break;
                case 2:
                    m_pImageBtUI.setScaleType(ImageButton.ScaleType.FIT_START);
                    break;
                case 3:
                    m_pImageBtUI.setScaleType(ImageButton.ScaleType.FIT_XY);
                    break;
            }
        }else if(name.compareTo("label") == 0){
            String s[] = value.split(" ");
            if (s.length == 6){
                if (m_pTextUI == null){
                    m_pTextUI = new TextView(getContext());
                    this.addView(m_pTextUI);
                }

                m_pTextUI.setTextSize(Integer.parseInt(s[0]));
                if (s[3].compareTo("center") == 0){
                    m_pTextUI.setGravity(Gravity.CENTER);
                }else if (s[3].compareTo("right") == 0){
                    m_pTextUI.setGravity(Gravity.RIGHT);
                }else{
                    m_pTextUI.setGravity(Gravity.LEFT);
                }
                m_pTextUI.setText(m_pCommonUI.getLanString(s[1]));

                m_iTextHeight = UIManager.dip2px(getContext(), Integer.parseInt(s[2]));
                m_iTextNormalColor = Color.parseColor(s[4]);
                m_iTextSelectColor = Color.parseColor(s[5]);
                if (m_bIsSelected){
                    m_pTextUI.setTextColor(m_iTextSelectColor);
                }else{
                    m_pTextUI.setTextColor(m_iTextNormalColor);
                }
            }else{
                m_text = value;
            }
        }else if(name.compareTo("labelStyle") == 0){
            m_iTextStyle = Integer.parseInt(value);
        }else if(name.compareTo("text") == 0){
            if (m_pTextUI != null) {
                m_pTextUI.setText(m_pCommonUI.getLanString(value));
            }
        }else {
            m_pCommonUI.setAttribute(name, value);
            if (name.compareTo("border") == 0){
                m_Paint.setColor(m_pCommonUI.m_borderColor);
                m_Paint.setStrokeWidth(m_pCommonUI.m_borderSize);
            }
        }
    }

    public int getUsrTag()
    {
        return m_pCommonUI.m_iTag;
    }

    @Override
    public CCommonFunUI GetCommFun() {
        return m_pCommonUI;
    }


    @Override
    protected void onLayout(boolean changed, int l, int t, int r, int b) {
        if (m_pTextUI != null) {
            switch (m_iTextStyle) {
                case 0:
                {
                    m_pImageBtUI.layout(0, -UIManager.dip2px(getContext(), 6), r-l, b-t-UIManager.dip2px(getContext(), 6));
                    m_pTextUI.layout(0, b-t-m_iTextHeight, r-l, b-t);
                    break;
                }
                case 1:
                {
                    m_pImageBtUI.layout(0, -UIManager.dip2px(getContext(), 6), r-l, b-t-UIManager.dip2px(getContext(), 6));
                    m_pTextUI.layout(0, (b-t-m_iTextHeight)/2, r-l, (b-t-m_iTextHeight)/2+m_iTextHeight);
                    break;
                }
            }

        }else{
            m_pImageBtUI.layout(0, 0, r-l, b-t);
        }
    }

    @Override
    protected void onDraw(Canvas canvas) {
        super.onDraw(canvas);
        if (m_pCommonUI.m_borderSize > 0){
            canvas.drawRect((new Rect(0, 0, getWidth(), getHeight())), m_Paint);
        }
    }
}
