package fyuilib.luoc.com.control;

import android.content.Context;
import android.view.View;
import android.view.ViewGroup;

import java.util.ArrayList;

import fyuilib.luoc.com.core.UIManager;

/**
 * Created by luoc on 2018/5/5.
 */

public class CTabLayoutUI extends ViewGroup implements UIAttribute, UITabAttribute{
    private CCommonFunUI m_pCommonUI = null;
    private int          m_iLayoutStyle = 0;
    private int          m_iSelectIndex = 0;
    private ArrayList    m_tabArray = new ArrayList();

    public CTabLayoutUI(Context context, UIManager manager) {
        super(context);

        m_pCommonUI = new CCommonFunUI();
        m_pCommonUI.initWithDelegateView(this, manager);
    }

    @Override
    public void AddTabElement(View element)
    {
        m_tabArray.add(element);
        if (m_tabArray.size() >1){
            element.setVisibility(View.INVISIBLE);
        }
    }

    public void SelectIndex(int index)
    {
        if (index < m_tabArray.size()){
            View o = (View)m_tabArray.get(m_iSelectIndex);
            o.setVisibility(View.INVISIBLE);
            View n = (View) m_tabArray.get(index);
            n.setVisibility(View.VISIBLE);
            n.layout(0, 0, getWidth() , getHeight());
            m_iSelectIndex = index;
        }
    }

    public View GetSelectView()
    {
        if (m_iSelectIndex < m_tabArray.size()){
            return (View)m_tabArray.get(m_iSelectIndex);
        }
        return null;
    }

    public View GetViewOfIndex(int index)
    {
        if (index < m_tabArray.size()){
            return (View)m_tabArray.get(index);
        }
        return null;
    }

    @Override
    public void SetAttribute(String name, String value) {
        m_pCommonUI.setAttribute(name, value);
    }

    @Override
    protected void onLayout(boolean changed, int l, int t, int r, int b) {
        int count = m_tabArray.size();
        for (int i=0; i < count; i++) {
            View v = (View)m_tabArray.get(i);
            if (v.getVisibility() == View.VISIBLE){
                v.layout(l, t, r, b);
            }
        }
    }

    @Override
    public CCommonFunUI GetCommFun() {
        return m_pCommonUI;
    }
}
