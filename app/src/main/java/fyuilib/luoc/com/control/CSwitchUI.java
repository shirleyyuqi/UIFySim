package fyuilib.luoc.com.control;


import java.util.Timer;
import java.util.TimerTask;

import android.content.Context;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.Paint;
import android.graphics.Paint.Style;
import android.graphics.Rect;
import android.graphics.RectF;
import android.graphics.drawable.GradientDrawable;
import android.util.AttributeSet;
import android.view.MotionEvent;
import android.view.ViewConfiguration;
import android.widget.ImageButton;

import android.content.Context;
import android.graphics.drawable.Drawable;
import android.graphics.drawable.GradientDrawable;
import android.view.View;
import android.widget.CompoundButton;
import android.widget.Switch;

import fyuilib.luoc.com.core.UIManager;
/**
 * 自定义SwitchButton(仿Android 5.0自带)
 *
 * @author HeyGays
 *
 */


/**
 * Created by luoc on 2018/5/5.
 */
/*
public class CSwitchUI extends Switch implements UIAttribute, CompoundButton.OnCheckedChangeListener {
    public CCommonFunUI m_pCommonUI = null;

    public CSwitchUI(Context context, UIManager manager) {
        super(context);

        m_pCommonUI = new CCommonFunUI();
        m_pCommonUI.initWithDelegateView(this, manager);

        setOnCheckedChangeListener(this);
    }

    public GradientDrawable getDrawable(int bgCircleAngle, int bgColor, int width, int strokeColor) {
        GradientDrawable gradientDrawable = new GradientDrawable();
        gradientDrawable.setCornerRadius(bgCircleAngle);
        gradientDrawable.setColor(bgColor);
        gradientDrawable.setStroke(width, strokeColor);
        return gradientDrawable;
    }

    @Override
    public void onCheckedChanged(CompoundButton buttonView, boolean isChecked){
        if (buttonView.isChecked()){

        }
    }

    @Override
    public void SetAttribute(String name, String value) {
        if (name.compareTo("oncolor") == 0) {

        }else if (name.compareTo("offcolor") == 0) {
        }else if (name.compareTo("ontext") == 0) {
            this.setTextOn(value);
        }else if (name.compareTo("offtext") == 0) {
            this.setTextOff(value);
        }else if (name.compareTo("text") == 0) {
            if (m_pCommonUI.getTextTag(1) != null){
                this.setTextOn(m_pCommonUI.getLanString(m_pCommonUI.getTextTag(1)));
            }

            if (m_pCommonUI.getTextTag(2) != null){
                this.setTextOff(m_pCommonUI.getLanString(m_pCommonUI.getTextTag(2)));
            }
        }else{
            m_pCommonUI.setAttribute(name, value);
        }

    }

    @Override
    public CCommonFunUI GetCommFun() {
        return m_pCommonUI;
    }
}
*/


public class CSwitchUI extends View implements UIAttribute, View.OnClickListener{
    public CCommonFunUI m_pCommonUI = null;

    private boolean     m_bOpen = true;
    private int         m_colorClose = 0xff888888;
    private int         m_colorOpen = 0xff32CD32;

    private Paint       m_paintThum;
    private Paint       m_paintBack;

    private int         m_xThum = 0;  //圆的圆心x坐标
    private int         m_yThum = 0;  //圆的圆心y坐标
    private int         m_radiusPrev; //圆的半径

    public CSwitchUI(Context context, UIManager manager) {
        super(context);

        m_pCommonUI = new CCommonFunUI();
        m_pCommonUI.initWithDelegateView(this, manager);

        m_paintThum = new Paint();
        m_paintBack = new Paint();
        //关闭锯齿
        m_paintThum.setAntiAlias(true);
        m_paintBack.setAntiAlias(true);

        //设置圆点颜色
        m_paintThum.setColor(Color.parseColor("#ffffffff"));
        m_paintBack.setColor(m_colorClose);
        this.setOnClickListener(this);
    }

    @Override
    protected void onDraw(Canvas canvas) {
        super.onDraw(canvas);

        if (m_bOpen) {
            m_xThum = getHeight() / 2;  //关闭时候 圆的圆心x坐标
            m_paintBack.setColor(m_colorOpen);
        }else{
            m_xThum = getWidth() - (getHeight() / 2);  //开启时候 圆的圆心x坐标
            m_paintBack.setColor(m_colorClose);
        }
        m_yThum = getHeight() /2;
        m_radiusPrev = getHeight() /2;

        //画圆角矩形背景
        RectF rectF = new RectF(0, 0, getWidth(), getHeight());
        canvas.drawRoundRect(rectF, m_radiusPrev, m_radiusPrev, m_paintBack);

        //画圆点
        canvas.drawCircle(m_xThum, m_yThum, m_radiusPrev-2, m_paintThum);
    }

    /**
     * 程序运行时候代码设置开关
     */
    public void setSwitchState(boolean flag){
        if(!((flag && m_bOpen) || (!flag && !m_bOpen) )){
            onClick(this);
        }
    }

    @Override
    public void onClick(View view) {

        m_bOpen = !m_bOpen;

        //重新绘制
        invalidate();
    }

    @Override
    public void SetAttribute(String name, String value) {
        if (name.compareTo("oncolor") == 0) {

        }else if (name.compareTo("offcolor") == 0) {
        }else if (name.compareTo("ontext") == 0) {
            //this.setTextOn(value);
        }else if (name.compareTo("offtext") == 0) {
           // this.setTextOff(value);
        }else if (name.compareTo("text") == 0) {
            if (m_pCommonUI.getTextTag(1) != null){
                //this.setTextOn(m_pCommonUI.getLanString(m_pCommonUI.getTextTag(1)));
            }

            if (m_pCommonUI.getTextTag(2) != null){
                //this.setTextOff(m_pCommonUI.getLanString(m_pCommonUI.getTextTag(2)));
            }
        }else{
            m_pCommonUI.setAttribute(name, value);
        }

    }

    @Override
    public CCommonFunUI GetCommFun() {
        return m_pCommonUI;
    }
}