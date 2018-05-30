package fyuilib.luoc.com.control;

import android.content.Context;
import android.graphics.Color;
import android.graphics.Rect;
import android.os.Build;
import android.util.Size;
import android.view.MotionEvent;
import android.view.View;
import android.view.ViewGroup;

import fyuilib.luoc.com.core.UIManager;

/**
 * Created by luoc on 2018/5/5.
 */

public class CCommonFunUI {
    private UIManager m_pManager = null;
    private View     m_pView = null;
    private boolean  m_IsWidthSet = false;
    private boolean  m_IsHeightSet = false;
    private boolean  m_IsFloat = false;
    private int      m_iWidth = 0;
    private int      m_iHeight =0;
    private Rect     m_szMargin = new Rect(0, 0, 0, 0);
    public int       m_iTag = 0;
    private String   m_textInfo = null;
    private String   m_textInfo2 = null;
    private boolean  m_replaceTextOff = true;
    public  int      m_corner = 0;
    public  int      m_borderSize = 0;
    public  int      m_borderColor = 0x0;
    public  String   mName = "";

    public static Size getSizeValue(String value){
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.LOLLIPOP) {
            return new Size(0 ,0);
        }
        return null;
    }

    public static Rect getRectValue(Context context, String value){
        String[] s = value.replace(" ", "").split(",");
        Rect rt = new Rect(0, 0, 0, 0);
        if (s.length == 4){
            rt.left = UIManager.dip2px(context,Integer.parseInt(s[0]));
            rt.top = UIManager.dip2px(context,Integer.parseInt(s[1]));
            rt.right = UIManager.dip2px(context,Integer.parseInt(s[2]));
            rt.bottom = UIManager.dip2px(context,Integer.parseInt(s[3]));
        }
        return rt;
    }

    public UIManager getUIManager(){
        return m_pManager;
    }

    public static void SetFrameOfVertical(Rect frame, ViewGroup parentView){
        int count = parentView.getChildCount();
        if (count == 0) {
            return;
        }

        int w = 0; int i = 0;
        for (int j=0; j<count; j++) {
            View view = parentView.getChildAt(j);
            UIAttribute v = (UIAttribute)view;

            if ( v.GetCommFun().m_IsFloat || view.getVisibility() == View.INVISIBLE){
                i++;
                continue;
            }

            if (v.GetCommFun().m_IsWidthSet){
                i++;
                w += v.GetCommFun().m_iWidth;
            }
        }

        if (frame.width() - w <= 0){
            int originX = 0;
            for (int j=0; j<count; j++){
                View view = (View)parentView.getChildAt(j);
                UIAttribute v = (UIAttribute)view;
                if (v.GetCommFun().m_IsFloat) continue;
                if (view.getVisibility() == View.INVISIBLE) continue;
                int x = v.GetCommFun().m_szMargin.left;
                int y = v.GetCommFun().m_szMargin.top;
                int x2 = v.GetCommFun().m_szMargin.right;
                int y2 = v.GetCommFun().m_szMargin.bottom;
                if (v.GetCommFun().m_IsWidthSet) {
                    if (v.GetCommFun().m_IsHeightSet) {
                        view.layout(frame.left + originX + x, frame.top+y, frame.left+originX + x + v.GetCommFun().m_iWidth - x2, frame.top+y + v.GetCommFun().m_iHeight - y2);
                    }else{
                        view.layout(frame.left + originX + x, frame.top+y, frame.left+originX + x + v.GetCommFun().m_iWidth - x2, frame.top+y + frame.height() - y2);
                    }
                    originX += v.GetCommFun().m_iWidth;
                }else{
                    view.layout(frame.left + originX + x, frame.top+y, frame.left+originX + x, frame.top+y + v.GetCommFun().m_iHeight);
                }
            }
        }else{
            if (count == i)  w = (frame.width() - w);
            else w = (frame.width() - w) / (count-i);

            int originX = 0;
            for (int j=0; j<count; j++) {
                View view = (View) parentView.getChildAt(j);
                UIAttribute v = (UIAttribute)view;
                if (v.GetCommFun().m_IsFloat) continue;
                if (view.getVisibility() == View.INVISIBLE) continue;
                int x = v.GetCommFun().m_szMargin.left;
                int y = v.GetCommFun().m_szMargin.top;
                int x2 = v.GetCommFun().m_szMargin.right;
                int y2 = v.GetCommFun().m_szMargin.bottom;
                if (v.GetCommFun().m_IsWidthSet) {
                    if (v.GetCommFun().m_IsHeightSet) {
                        view.layout(frame.left + originX + x, frame.top+y, frame.left+originX + x + v.GetCommFun().m_iWidth - x2, frame.top+y + v.GetCommFun().m_iHeight - y2);
                    }else{
                        view.layout(frame.left + originX + x, frame.top+y, frame.left+originX + x + v.GetCommFun().m_iWidth - x2, frame.top+y + frame.height() - y2);
                    }

                    originX += v.GetCommFun().m_iWidth;
                }else{
                    if (v.GetCommFun().m_IsHeightSet) {
                        view.layout(frame.left + originX + x, frame.top+y, frame.left+originX + x + w - x2, frame.top+y + v.GetCommFun().m_iHeight - y2);
                    }else{
                        view.layout(frame.left + originX + x, frame.top+y, frame.left+originX + x + w - x2, frame.top+y + frame.height() -y2);
                    }

                    originX += w;
                }
            }
        }
    }

    public static void SetFrameOfHorizontal(Rect frame, ViewGroup parentView){
        int count = parentView.getChildCount();
        if (count == 0) {
            return;
        }

        int h = 0; int i = 0;
        for (int j=0; j<count; j++) {
            View view = parentView.getChildAt(j);
            UIAttribute v = (UIAttribute)view;
            if ( v.GetCommFun().m_IsFloat || view.getVisibility() == View.INVISIBLE){
                i++;
                continue;
            }

            if (v.GetCommFun().m_IsHeightSet){
                i++;
                h += v.GetCommFun().m_iHeight;
            }
        }

        if (frame.height() - h <= 0){
            int originY = 0;
            for (int j=0; j<count; j++){
                View view = (View)parentView.getChildAt(j);
                UIAttribute v = (UIAttribute)view;
                if (v.GetCommFun().m_IsFloat) continue;
                if (view.getVisibility() == View.INVISIBLE) continue;
                int x = v.GetCommFun().m_szMargin.left;
                int y = v.GetCommFun().m_szMargin.top;
                int x2 = v.GetCommFun().m_szMargin.right;
                int y2 = v.GetCommFun().m_szMargin.bottom;
                if (v.GetCommFun().m_IsHeightSet) {
                    if (v.GetCommFun().m_IsWidthSet){
                        view.layout(frame.left + x, frame.top+originY+y, frame.left+x + v.GetCommFun().m_iWidth - x2, frame.top+originY+y + v.GetCommFun().m_iHeight - y2);
                    }else{
                        view.layout(frame.left + x, frame.top+originY+y, frame.left+x + frame.width() - x2, frame.top+originY+y + frame.height() - y2);
                    }
                    originY += v.GetCommFun().m_iHeight;
                }else{
                    view.layout(frame.left + x, frame.top+originY+y, frame.left+x + v.GetCommFun().m_iWidth, frame.top+originY+y);
                }
            }
        }else{
            if (count == i) h = frame.height() - h;
            else h = (frame.height() - h) / (count-i);
            int originY = 0;
            for (int j=0; j<count; j++) {
                View view = (View) parentView.getChildAt(j);
                UIAttribute v = (UIAttribute)view;
                if (v.GetCommFun().m_IsFloat) continue;
                if (view.getVisibility() == View.INVISIBLE) continue;
                int x = v.GetCommFun().m_szMargin.left;
                int y = v.GetCommFun().m_szMargin.top;
                int x2 = v.GetCommFun().m_szMargin.right;
                int y2 = v.GetCommFun().m_szMargin.bottom;
                if (v.GetCommFun().m_IsHeightSet) {
                    if (v.GetCommFun().m_IsWidthSet) {
                        view.layout(frame.left + x, frame.top+originY+y, frame.left+x + v.GetCommFun().m_iWidth - x2, frame.top+originY+y + v.GetCommFun().m_iHeight - y2);
                    }else{
                        view.layout(frame.left + x, frame.top+originY+y, frame.left+x + frame.width() - x2, frame.top+originY+y + v.GetCommFun().m_iHeight - y2);
                    }

                    originY += v.GetCommFun().m_iHeight;
                }else{
                    if (v.GetCommFun().m_IsWidthSet) {
                        view.layout(frame.left + x, frame.top+originY+y, frame.left+x + v.GetCommFun().m_iWidth - x2, frame.top+originY+y + h - y2);
                    }else{
                        view.layout(frame.left + x, frame.top+originY+y, frame.left+x + frame.width() - x2, frame.top+originY+y + h -y2);
                    }

                    originY += h;
                }
            }
        }
    }

    public static void SetFrameOfOverlap(Rect frame, ViewGroup parentView){
        int count = parentView.getChildCount();
        if (count == 0) {
            return;
        }

        for (int i=0; i<count; i++) {
            View v = parentView.getChildAt(i);
            v.layout(frame.left, frame.top, frame.left+frame.width(), frame.top+frame.height());
        }
    }

    public void initWithDelegateView(View view, UIManager manager){
        m_pManager = manager;
        m_pView = view;
    }

    public void setAttribute(String name, String value){
        if (name.compareTo("bkcolor") == 0) {
            m_pView.setBackgroundColor(Color.parseColor(value));
        }else if (name.compareTo("width") == 0){
            m_IsWidthSet = true;
            m_iWidth = UIManager.dip2px(m_pView.getContext(), Integer.parseInt(value));
        }else if (name.compareTo("height") == 0){
            m_IsHeightSet = true;
            m_iHeight = UIManager.dip2px(m_pView.getContext(), Integer.parseInt(value));
        }else if(name.compareTo("removeheight") == 0){
            m_IsHeightSet = false;
        }else if(name.compareTo("removewidth") == 0){
            m_IsWidthSet = false;
        }else if (name.compareTo("name") == 0){
            mName = value;
            m_pManager.AddObject(value, m_pView);
        }else if (name.compareTo("border") == 0){
            String s[] = value.split(" ");
            if (s.length == 3){
                m_borderSize = UIManager.dip2px(m_pView.getContext(),Integer.parseInt(s[0].split("px")[0]));
                m_borderColor = Color.parseColor(s[2]);
            }
            //[self setBorder:value];
        }else if (name.compareTo("bordersize") == 0){
            //_delegateView.layer.borderWidth = atof(value);
        }else if (name.compareTo("bordercolor") == 0){
            //_delegateView.layer.borderColor = [CCommonFunUI getColorValue:value].CGColor;
        }else if (name.compareTo("corner") == 0){
            m_corner = UIManager.dip2px(m_pView.getContext(), Integer.parseInt(value));
            //_delegateView.layer.cornerRadius = atof(value);
            //_delegateView.layer.masksToBounds = YES;
        }else if (name.compareTo("hidden") == 0){
            if (value.compareTo("true") == 0) {
                m_pView.setVisibility(View.INVISIBLE);
            }else{
                m_pView.setVisibility(View.VISIBLE);
            }
        }else if (name.compareTo("float") == 0){
            if (value.compareTo("true") == 0) {
                m_IsFloat = true;
                m_pManager.AddFloatObject(m_pView);
            }else{
                m_IsFloat = false;
                m_pManager.RemoveFloatObject(m_pView);
            }
        }else if (name.compareTo("left") == 0){
            //self.fLeft = atof(value);
        }else if (name.compareTo("right") == 0){
            //self.fRight = atof(value);
        }else if (name.compareTo("top") == 0){
            //self.fTop = atof(value);
        }else if (name.compareTo("bottom") == 0){
            //self.fBottom = atof(value);
        }else if (name.compareTo("margin") == 0){
            m_szMargin = CCommonFunUI.getRectValue(m_pView.getContext(), value);
        }else if(name.compareTo("tapEvent") == 0) {
            m_pView.setOnTouchListener(new View.OnTouchListener() {
                @Override
                public boolean onTouch(View v, MotionEvent event) {
                    m_pManager.OnNotify((UIAttribute) m_pView, UIManagerDelegate.FYEVENT_TAP, event);
                    return false;
                }
            });
            /*UITapGestureRecognizer *recognizer = [[UITapGestureRecognizer alloc] initWithTarget:self action:@selector(handleTap:)];
            recognizer.numberOfTapsRequired = atoi(value);
            recognizer.numberOfTouchesRequired = 1;
            [_delegateView addGestureRecognizer:recognizer];*/
        }else if(name.compareTo("tag") == 0){
            m_iTag = Integer.parseInt(value);
        }else if(name.compareTo("lgOff") == 0){
            if (value.compareTo("true") == 0) {
                m_replaceTextOff = false;
            }else{
                m_replaceTextOff = true;
            }
        }else if(name.compareTo("background") == 0){
            /*UIColor *bgColor = [UIColor colorWithPatternImage:[UIImage imageNamed:[NSString stringWithUTF8String:value]]];
            [_delegateView setBackgroundColor:bgColor];*/

        }
    }

    public int getW(){
        return m_iWidth;
    }

    public int getH(){
        return m_iHeight;
    }

    public void ChangeTextInfo(){
        if (m_pView != null && m_textInfo != null && m_replaceTextOff){
            UIAttribute pPoint = (UIAttribute)m_pView;
            pPoint.SetAttribute("text", m_textInfo);
        }
    }

    public String getLanString(String value){
        if (m_textInfo == null && value == null){
            return "";
        }
        if (!m_replaceTextOff){
            return value;
        }

        m_textInfo = value;
        return UIManager.getTextOfLanguageTag(m_textInfo);
    }

    public String getLanString2(String value){
        if (m_textInfo2 == null && value == null){
            return "";
        }

        if (!m_replaceTextOff){
            return value;
        }

        m_textInfo2 = value;
        return UIManager.getTextOfLanguageTag(m_textInfo2);
    }

    public String getTextTag(int tag){
        if (tag == 1 && m_textInfo != null){
            return m_textInfo;
        }else if (tag == 2 && m_textInfo2 != null) {
            return m_textInfo2;
        }
        return null;
    }
}
