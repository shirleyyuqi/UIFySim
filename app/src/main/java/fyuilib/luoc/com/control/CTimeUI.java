package fyuilib.luoc.com.control;

import android.content.Context;
import android.view.View;
import android.widget.TimePicker;

import fyuilib.luoc.com.core.UIManager;

/**
 * Created by luoc on 2018/5/5.
 */

public class CTimeUI extends TimePicker implements UIAttribute{
    public CCommonFunUI m_pCommonUI = null;

    public CTimeUI(Context context, UIManager manager) {
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
