package fyuilib.luoc.com.control;

import android.content.Context;
import android.transition.Slide;
import android.view.View;
import android.widget.SeekBar;

import fyuilib.luoc.com.core.UIManager;

/**
 * Created by luoc on 2018/5/5.
 */

public class CSliderUI extends SeekBar implements UIAttribute{
    public CCommonFunUI m_pCommonUI = null;

    public CSliderUI(Context context, UIManager manager) {
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
