package fyuilib.luoc.com.core;

import android.app.Activity;
import android.content.Context;
import android.content.res.Resources;
import android.graphics.Paint;
import android.util.Log;
import android.view.View;
import android.view.ViewGroup;

import java.lang.reflect.Array;
import java.lang.reflect.Field;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

import fyuilib.luoc.com.activity.R;
import fyuilib.luoc.com.control.COptionUI;
import fyuilib.luoc.com.control.UIAttribute;
import fyuilib.luoc.com.control.UIManagerDelegate;

/**
 * Created by luoc on 2018/5/5.
 */

public class UIManager {
    public static int   sScreenW;
    public static int   sScreenH;
    private HashMap m_pArray = new HashMap();
    private HashMap  m_groupOption = new HashMap();
    private ArrayList m_pFloatArray = new ArrayList();
    private UIManagerDelegate m_uiDelegate = null;
    private static String     g_lgFile = "";
    private String            m_lgFile = null;
    private View              m_mainView = null;

    public UIManager(UIManagerDelegate delegate) {
        m_uiDelegate = delegate;
        m_lgFile = g_lgFile;
    }

    public void SetMainView(View view){
        m_mainView = view;
    }

    public Activity getActivity(){
        return (Activity)m_uiDelegate;
    }

    public boolean OnNotify(UIAttribute pSender, int event, Object param) {
        return m_uiDelegate.OnNotify(pSender, event, param);
    }

    public void AddObject(String name, View view){
        m_pArray.put(name, view);
    }

    public void AddFloatObject(View view){
        m_pFloatArray.add(view);
    }

    public void RemoveFloatObject(View view){
        m_pFloatArray.remove(view);
    }

    public View GetViewOfName(String name){
        return (View)m_pArray.get(name);
    }


    public void ChangeOptionSelectObject(COptionUI option, String groupName)
    {
        COptionUI opt = (COptionUI) m_groupOption.get(groupName);
        if (opt != null){
            opt.ChangeSelected(false);
            option.ChangeSelected(true);
        }

        m_groupOption.put(groupName, option);
    }

    public static int getImage(String imageName){
        Class mipmap = R.mipmap.class;
        try {
            String name = imageName.split(".jpg")[0].split(".png")[0];
            Field field = mipmap.getField(name);
            int resId = field.getInt(imageName);
            return resId;
        } catch (NoSuchFieldException e) {
            return 0;
        } catch (IllegalAccessException e) {
            return 0;
        }
    }

    public static int getXml(String xmlName){
        Class layout = R.layout.class;
        try {
            Field field = layout.getField(xmlName);
            int resId = field.getInt(xmlName);
            return resId;
        } catch (NoSuchFieldException e) {
            return 0;
        } catch (IllegalAccessException e) {
            return 0;
        }
    }

    public static int dip2px(Context context, float dpValue) {
        final float scale = context.getResources().getDisplayMetrics().density;
        return (int) (dpValue * scale + 0.5f);
    }

    public static int px2dip(Context context, float pxValue) {
        final float scale = context.getResources().getDisplayMetrics().density;
        return (int) (pxValue / scale + 0.5f);
    }

    public static int px2dip(float pxValue, float scale) {
        return (int) (pxValue / scale + 0.5f);
    }

    public static int dip2px(float dipValue, float scale) {
        return (int) (dipValue * scale + 0.5f);
    }

    public static int getNavigationBarHeight(Activity activity) {
        Resources resources = activity.getResources();
        int resourceId = resources.getIdentifier("navigation_bar_height","dimen", "android");
        int height = resources.getDimensionPixelSize(resourceId);
        Log.v("dbw", "Navi height:" + height);
        return height;
    }

    public static int getStatusBarHeight(Activity activity) {
        Resources resources = activity.getResources();
        int resourceId = resources.getIdentifier("status_bar_height", "dimen","android");
        int height = resources.getDimensionPixelSize(resourceId);
        Log.v("dbw", "Status height:" + height);
        return height;
    }

    public void ReloadTextInfoWithView(View view){
        if (view.getClass().toString().endsWith("CLayoutUI")
                || view.getClass().toString().endsWith("CListUI")){
            ViewGroup v = (ViewGroup)view;
            int count = v.getChildCount();
            for (int i=0; i<count; i++){
                View t = v.getChildAt(i);
                ReloadTextInfoWithView(t);
                Log.e("a", t.getClass().toString());
                if (t.getClass().toString().startsWith("class fyuilib.luoc.com.control.")){
                    UIAttribute pElem = (UIAttribute)t;
                    ((UIAttribute) t).GetCommFun().ChangeTextInfo();
                }
            }
        }
    }

    public void ReloadTextInfo(){
        if (m_lgFile == null || g_lgFile == null || g_lgFile.compareTo(m_lgFile) == 0)
            return;
        m_lgFile = g_lgFile;
        ReloadTextInfoWithView(m_mainView);
    }

    public static String getTextOfLanguageTag(String lgTag){
        return CXmlParse.sharedXmlParse(g_lgFile).getValue(lgTag);
    }

    public static void ReloadLoadLanguage(String file){
        if (file.compareTo(g_lgFile) != 0){
            CXmlParse.releasXmlParse();

            g_lgFile = file;
            CXmlParse.sharedXmlParse(file);
        }
    }

    public static int getFontHeight(float fontSize)
    {
        Paint paint = new Paint();
        paint.setTextSize(fontSize);
        Paint.FontMetrics fm = paint.getFontMetrics();
        return (int) Math.ceil(fm.descent - fm.top) + 2;
    }
}
