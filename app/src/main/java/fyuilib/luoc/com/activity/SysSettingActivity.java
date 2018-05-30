package fyuilib.luoc.com.activity;

import android.app.Activity;
import android.os.Bundle;
import android.view.View;
import android.view.Window;

import fyuilib.luoc.com.control.CButtonUI;
import fyuilib.luoc.com.control.CLayoutUI;
import fyuilib.luoc.com.control.CListElementUI;
import fyuilib.luoc.com.control.CListUI;
import fyuilib.luoc.com.control.COptionUI;
import fyuilib.luoc.com.control.CTabLayoutUI;
import fyuilib.luoc.com.control.UIAttribute;
import fyuilib.luoc.com.control.UIManagerDelegate;
import fyuilib.luoc.com.core.CXmlParse;
import fyuilib.luoc.com.core.UIDlgBuild;
import fyuilib.luoc.com.core.UIManager;
import fyuilib.luoc.com.util.FyUIActivity;

/**
 * Created by luoc on 2018/5/23.
 */

public class SysSettingActivity extends FyUIActivity {
    private CButtonUI   m_pBackBt;
    private CListUI     m_pListLan;
    private CListUI     m_pList;
    private CLayoutUI   m_pLayoutAlert;

    @Override
    public String getUIXml(){
        return "setting.xml";
    }

    @Override
    public void InitControl(){
        m_pBackBt = (CButtonUI)m_pManagerUI.GetViewOfName("back");
        m_pListLan = (CListUI)m_pManagerUI.GetViewOfName("listLan");
        m_pList = (CListUI)m_pManagerUI.GetViewOfName("list");
        m_pLayoutAlert = (CLayoutUI) m_pManagerUI.GetViewOfName("alert");
    }

    @Override
    public boolean OnNotify(UIAttribute pSender, int event, Object param) {
        if (event == UIManagerDelegate.FYEVENT_TOUCHUPINSIDE){
            if (pSender == m_pBackBt){
                finish();
            }
        }else if (event == UIManagerDelegate.FYEVENT_TB_SELECTED){
            if (pSender == m_pList){
                CListElementUI pElement = (CListElementUI)param;
                if (pElement.mIndex == 0){
                    m_pLayoutAlert.setVisibility(View.VISIBLE);
                    m_pListLan.layout(0, UIManager.sScreenH - m_pListLan.GetCommFun().getH(), UIManager.sScreenW, UIManager.sScreenH);
                }
            }else if (pSender == m_pListLan){
                CListElementUI pElement = (CListElementUI)param;
                if (pElement.mIndex == 0){
                    UIManager.ReloadLoadLanguage("language/cn.xml");
                }else if (pElement.mIndex == 1){
                    UIManager.ReloadLoadLanguage("language/en.xml");
                }
                m_pManagerUI.ReloadTextInfo();
                dismissAlert();
            }
        }else if (event == UIManagerDelegate.FYEVENT_TAP){
            dismissAlert();
        }

        return false;
    }

    void dismissAlert(){
        m_pLayoutAlert.setVisibility(View.INVISIBLE);
    }
}
