package fyuilib.luoc.com.control;

import android.content.Context;
import android.view.View;

import fyuilib.luoc.com.core.UIManager;

/**
 * Created by luoc on 2018/5/5.
 */

public class CScrollViewUI extends View implements UIAttribute{
    public CCommonFunUI m_pCommonUI = null;

    public CScrollViewUI(Context context, UIManager manager) {
        super(context);

        m_pCommonUI = new CCommonFunUI();
        m_pCommonUI.initWithDelegateView(this, manager);
    }

    @Override
    public void SetAttribute(String strName, String value) {

    }

    @Override
    public CCommonFunUI GetCommFun() {
        return m_pCommonUI;
    }
}