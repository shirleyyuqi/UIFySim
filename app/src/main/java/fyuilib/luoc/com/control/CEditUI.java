package fyuilib.luoc.com.control;

import android.content.Context;
import android.graphics.Color;
import android.text.Editable;
import android.text.InputType;
import android.text.TextWatcher;
import android.view.MotionEvent;
import android.view.View;
import android.view.ViewGroup;
import android.widget.EditText;
import android.widget.ImageButton;
import android.widget.ImageView;
import android.widget.TextView;

import fyuilib.luoc.com.core.UIManager;

/**
 * Created by luoc on 2018/5/5.
 */

public class CEditUI extends ViewGroup implements UIAttribute{
    public CCommonFunUI m_pCommonUI = null;
    private EditText    m_pEdit = null;
    private int         m_iTextIndent = 0;
    private CButtonUI   m_pImageView = null;
    private boolean     m_bLeftView = true;
    private int         m_ImageViewWidth = 0;
    private int         m_ImageViewHeight = 0;
    private int         m_iTextColor = 0xFF000000;

    private String      m_placeText = null;
    private boolean     m_bShowPlace = false;
    private int         m_iLayoutStyle = -1;

    public CEditUI(Context context, UIManager manager) {
        super(context);

        m_pCommonUI = new CCommonFunUI();
        m_pCommonUI.initWithDelegateView(this, manager);

        m_pEdit = new EditText(context);
        m_pEdit.setBackgroundColor(0x00);
        m_pEdit.setSingleLine();
        //m_pEdit.setFocusable(false);
        this.addView(m_pEdit);
        /*m_pEdit.setOnTouchListener(new OnTouchListener() {
            @Override
            public boolean onTouch(View v, MotionEvent event) {
                m_pEdit.setFocusable(true);
                m_pEdit.setFocusableInTouchMode(true);
                return false;
            }
        });*/

        m_pEdit.setOnFocusChangeListener(new OnFocusChangeListener() {
            @Override
            public void onFocusChange(View v, boolean hasFocus) {
                if (m_placeText == null) return;
                if (hasFocus){
                    if (m_bShowPlace){
                        m_pEdit.setTextColor(m_iTextColor);
                        m_pEdit.setText("");
                        m_bShowPlace = false;
                    }
                }else{
                    if (m_pEdit.getText().toString().isEmpty()){
                        m_pEdit.setTextColor(0xFF888888);
                        m_pEdit.setText(m_placeText);
                        m_bShowPlace = true;
                    }
                }
            }
        });


    }

    public String getText(){
        return m_pEdit.getText().toString();
    }

    public void setText(String str){
        m_pEdit.setText(str);
        m_bShowPlace = false;
    }

    @Override
    public void SetAttribute(String name, String value) {
        if (name.compareTo("text") == 0) {
            m_pEdit.setText(value);
        }else if (name.compareTo("place") == 0){
            m_placeText = m_pCommonUI.getLanString(value);
            m_pEdit.setText(m_placeText);
            m_pEdit.setTextColor(0xFF888888);
            m_bShowPlace = true;
        }else if (name.compareTo("password") == 0){
            if (value.compareTo("true") == 0) {
                m_pEdit.setInputType(InputType.TYPE_TEXT_VARIATION_PASSWORD);
            }else{
                m_pEdit.setInputType(InputType.TYPE_CLASS_TEXT);
            }
        }else if (name.compareTo("align") == 0){
            if (value.compareTo("center") == 0) {
                //self.textAlignment = NSTextAlignmentCenter;
            }else if (value.compareTo("right") == 0){
                //self.textAlignment = NSTextAlignmentRight;
            }else{
                //self.textAlignment = NSTextAlignmentLeft;
            }
        }else if (name.compareTo("textcolor") == 0){
            m_iTextColor = Color.parseColor(value);
            if (m_placeText == null)
                m_pEdit.setTextColor(m_iTextColor);
        }else if (name.compareTo("font") == 0){
            m_pEdit.setTextSize(Integer.parseInt(value));
        }else if (name.compareTo("style") == 0) {
            m_iLayoutStyle = Integer.parseInt(value);
        }else if(name.compareTo("textIndent") == 0){
            m_iTextIndent = UIManager.dip2px(getContext(), Integer.parseInt(value));
        }else if( (name.compareTo("leftview") == 0) || (name.compareTo("rightview") == 0) ){
            if (m_pImageView == null) {
                String s[] = value.split(" ");
                if (s.length == 5) {
                    String st[] = s[2].split(",");
                    if (st.length == 3){
                        st[0] = st[0].substring(1, st[0].length());
                        st[2] = st[2].substring(0, st[2].length() - 1);
                    }
                    m_pImageView = new CButtonUI(getContext(), m_pCommonUI.getUIManager());
                    m_ImageViewWidth = UIManager.dip2px(getContext(), Integer.parseInt(st[1]));
                    m_ImageViewHeight = UIManager.dip2px(getContext(), Integer.parseInt(st[2]));
                    m_pImageView.SetAttribute("normalImage", s[3]);
                    m_pImageView.SetAttribute("selectImage", s[4]);
                    m_pImageView.SetAttribute("name", s[1]);
                    m_pImageView.SetAttribute("downEvent", "true");
                } else if (s.length == 3) {
                    String st[] = s[1].split(",");
                    if (st.length == 3){
                        st[0] = st[0].substring(1, st[0].length());
                        st[2] = st[2].substring(0, st[2].length() - 1);
                    }
                    m_pImageView = new CButtonUI(getContext(), m_pCommonUI.getUIManager());
                    m_ImageViewWidth = UIManager.dip2px(getContext(), Integer.parseInt(st[1]));
                    m_ImageViewHeight = UIManager.dip2px(getContext(), Integer.parseInt(st[2]));
                    m_pImageView.SetAttribute("normalImage", s[2]);
                    m_pImageView.SetAttribute("mode", "1");
                }

                if (name.compareTo("leftview") == 0){
                    m_bLeftView = true;
                }else{
                    m_bLeftView = false;
                }
                if (m_pImageView != null)
                    this.addView(m_pImageView);
            }
        }else if(name.compareTo("rightview") == 0){
            /*int w = 0,h = 0,x=0;
            char style[20] = {0}, imageNormal[40] ={0}, imagePress[40], name[40];
            if(7 == sscanf(value, "%20s %40s {%d,%d,%d} %40s %40s", style, name, &x, &w, &h, imageNormal, imagePress)){
                self.rightView = nil;
                if (strcasecmp(style, "button") == 0) {
                    CButtonUI* pButton = [[CButtonUI alloc] initWithUIManager:[commonFun pManagerUI]];
                [pButton setFrame:CGRectMake(0, 0, w, h)];
                [pButton SetAttribute:"normalImage" :imageNormal];
                [pButton SetAttribute:"selectImage" :imagePress];
                [pButton SetAttribute:"name" :name];
                [pButton SetAttribute:"downEvent" :"true"];
                    iRightViewRight = x;
                    self.rightView = pButton;
                    self.rightViewMode = UITextFieldViewModeAlways;
                }
            }else if(5 == sscanf(value, "%20s {%d,%d,%d} %40s", style, &x, &w, &h, imageNormal)){
                self.leftView = nil;
                if (strcasecmp(style, "image") == 0) {
                    CImageUI* pImg = [[CImageUI alloc] initWithUIManager:[commonFun pManagerUI]];
                [pImg setFrame:CGRectMake(0, 0, w, h)];
                [pImg SetAttribute:"src" :imageNormal];
                [pImg SetAttribute:"name" :name];
                    iRightViewRight = x;
                    self.rightView = pImg;
                    self.leftViewMode = UITextFieldViewModeAlways;
                }
            }*/
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
        if (m_pImageView != null){
            if (m_bLeftView){
                m_pImageView.layout(0, 0, m_ImageViewWidth, b-t);
                m_pEdit.layout(m_iTextIndent+m_ImageViewWidth+2, 0, r-l-m_iTextIndent-m_ImageViewWidth-2, b-t);
            }else{
                m_pImageView.layout(m_ImageViewWidth, 0, m_ImageViewWidth+m_ImageViewWidth, b-t);
                m_pEdit.layout(m_iTextIndent+2, 0, r-l-m_iTextIndent-m_ImageViewWidth-2, b-t);
            }
        }else{
            m_pEdit.layout(m_iTextIndent, 0, r-l-m_iTextIndent, b-t);
        }
    }
}
