package fyuilib.luoc.com.activity.addbell;

import fyuilib.luoc.com.activity.util.Person;
import fyuilib.luoc.com.control.CButtonUI;
import fyuilib.luoc.com.control.CEditUI;
import fyuilib.luoc.com.control.UIAttribute;
import fyuilib.luoc.com.control.UIManagerDelegate;
import fyuilib.luoc.com.util.FyUIActivity;

/**
 * Created by luoc on 2018/5/26.
 */

public class AddBell1_2_UIDActivity extends FyUIActivity {
    public static Person  s_next = new Person();
    private CButtonUI m_pBackBt;
    private CButtonUI m_pNextBt;
    private CEditUI   m_pUID;

    @Override
    public String getUIXml(){
        return "AddBell1+2UID.xml";
    }

    @Override
    public void InitControl(){
        m_pBackBt = (CButtonUI)m_pManagerUI.GetViewOfName("back");
        m_pNextBt = (CButtonUI)m_pManagerUI.GetViewOfName("nextBt");
        m_pUID = (CEditUI) m_pManagerUI.GetViewOfName("uid");
    }

    @Override
    public boolean OnNotify(UIAttribute pSender, int event, Object param) {
        if (event == UIManagerDelegate.FYEVENT_TOUCHUPINSIDE) {
            if (pSender == m_pBackBt) {
                finish();
            }else if (pSender == m_pNextBt){
                AddBell1.sAddItemInfo.strUid = m_pUID.getText();
                s_next.clickCall.onClick(AddBell1_2_UIDActivity.this, null);
            }
        }
        return false;
    }
}
