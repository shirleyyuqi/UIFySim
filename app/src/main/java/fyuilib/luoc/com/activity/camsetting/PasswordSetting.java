package fyuilib.luoc.com.activity.camsetting;

import fyuilib.luoc.com.control.CButtonUI;
import fyuilib.luoc.com.control.UIAttribute;
import fyuilib.luoc.com.util.FyUIActivity;

/**
 * Created by luoc on 2018/5/26.
 */

public class PasswordSetting extends FyUIActivity {
    private CButtonUI m_pBackBt;

    @Override
    public String getUIXml(){
        return "setting_changepwd.xml";
    }

    @Override
    public void InitControl(){
        m_pBackBt = (CButtonUI)m_pManagerUI.GetViewOfName("back");
    }

    @Override
    public boolean OnNotify(UIAttribute pSender, int event, Object param) {
        if (pSender == m_pBackBt){
            finish();
        }
        return false;
    }
}
