package fyuilib.luoc.com.control;
import android.content.Context;
import android.view.ViewGroup;

import fyuilib.luoc.com.core.UIManager;

/**
 * Created by luoc on 2018/5/5.
 */

public class CCalendarUI extends ViewGroup implements UIAttribute {
    public CCommonFunUI     m_pCommonUI = null;
    private CCalendarView   m_pCalendarView = null;
    private CControlUI      m_pControl = null;

    public CCalendarUI(Context context, UIManager manager) {
        super(context);

        m_pControl = new CControlUI(context, manager);
        m_pCalendarView = new CCalendarView(context, this);
        m_pCommonUI = new CCommonFunUI();
        m_pCommonUI.initWithDelegateView(this, manager);
        this.addView(m_pCalendarView);

        m_pControl.m_pCommonUI.m_corner = UIManager.dip2px(context, 10);
        m_pControl.setBackgroundColor(0x88ff0000);
        this.addView(m_pControl);
    }

    public void Clicked(int x, int y, int w, int h, int year, int month, int day)
    {
        //int px = UIManager.dip2px(getContext(), 20);
        m_pControl.layout(x, y, x + w, y + h);
    }

    @Override
    public void SetAttribute(String name, String value) {
        m_pCommonUI.setAttribute(name, value);
    }

    @Override
    public CCommonFunUI GetCommFun() {
        return m_pCommonUI;
    }

    @Override
    protected void onLayout(boolean changed, int l, int t, int r, int b) {
        m_pCalendarView.layout(0, 0, r-l, b-t);
    }
}