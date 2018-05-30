package fyuilib.luoc.com.activity.addbell;

import android.content.Intent;

import fyuilib.luoc.com.control.CButtonUI;
import fyuilib.luoc.com.control.CEditUI;
import fyuilib.luoc.com.control.UIAttribute;
import fyuilib.luoc.com.control.UIManagerDelegate;
import fyuilib.luoc.com.util.FyUIActivity;

/**
 * Created by luoc on 2018/5/26.
 */

public class AddBell3_PWDActivity extends FyUIActivity {
    private CButtonUI m_pBackBt;
    private CButtonUI m_pNextBt;
    private CEditUI   m_pPwd;
    private CEditUI   m_pSurPwd;

    @Override
    public String getUIXml(){
        return "AddBell3Pwd.xml";
    }

    @Override
    public void InitControl(){
        m_pBackBt = (CButtonUI)m_pManagerUI.GetViewOfName("back");
        m_pNextBt = (CButtonUI)m_pManagerUI.GetViewOfName("nextBt");
        m_pPwd = (CEditUI)m_pManagerUI.GetViewOfName("pwd");
        m_pSurPwd = (CEditUI)m_pManagerUI.GetViewOfName("newpwd");
    }

    @Override
    public boolean OnNotify(UIAttribute pSender, int event, Object param) {
        if (event == UIManagerDelegate.FYEVENT_TOUCHUPINSIDE) {
            if (pSender == m_pBackBt) {
                finish();
            }else if (pSender == m_pNextBt){
                AddBell1.sAddItemInfo.strUsr = m_pPwd.getText();
                AddBell1.sAddItemInfo.strPwd = m_pPwd.getText();
                Intent intent = new Intent(AddBell3_PWDActivity.this, AddBell4_ConnectActivity.class);
                startActivity(intent);
            }
        }
        return false;
    }
}
