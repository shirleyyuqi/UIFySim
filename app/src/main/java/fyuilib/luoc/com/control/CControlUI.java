package fyuilib.luoc.com.control;

import android.content.Context;
import android.graphics.Canvas;
import android.graphics.Paint;
import android.graphics.Path;
import android.graphics.RectF;
import android.view.View;

import fyuilib.luoc.com.core.UIManager;

/**
 * Created by luoc on 2018/5/5.
 */

public class CControlUI extends View  implements UIAttribute{
    public CCommonFunUI m_pCommonUI = null;

    public CControlUI(Context context, UIManager manager) {
        super(context);

        m_pCommonUI = new CCommonFunUI();
        m_pCommonUI.initWithDelegateView(this, manager);
    }

    @Override
    public void SetAttribute(String name, String value) {
        m_pCommonUI.setAttribute(name, value);
    }

    @Override
    public CCommonFunUI GetCommFun() {
        return m_pCommonUI;
    }
}
