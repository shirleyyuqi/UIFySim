package fyuilib.luoc.com.activity.addbell;

import android.content.Context;
import android.content.Intent;
import android.net.wifi.WifiInfo;
import android.net.wifi.WifiManager;

import fyuilib.luoc.com.activity.util.Person;
import fyuilib.luoc.com.control.CButtonUI;
import fyuilib.luoc.com.control.CEditUI;
import fyuilib.luoc.com.control.UIAttribute;
import fyuilib.luoc.com.control.UIManagerDelegate;
import fyuilib.luoc.com.core.UIDlgBuild;
import fyuilib.luoc.com.util.FyUIActivity;

/**
 * Created by luoc on 2018/5/26.
 */

public class AddBell2_WifiActivity extends FyUIActivity {
    private CButtonUI m_pBackBt;
    private CButtonUI m_pNextBt;
    private CEditUI   m_pWifSSid;
    private CEditUI   m_pWifiPwd;

    @Override
    public String getUIXml(){
        return "AddBell2Wifi.xml";
    }

    @Override
    public void InitControl(){
        m_pBackBt = (CButtonUI)m_pManagerUI.GetViewOfName("back");
        m_pNextBt = (CButtonUI)m_pManagerUI.GetViewOfName("nextBt");
        m_pWifiPwd = (CEditUI)m_pManagerUI.GetViewOfName("pwd");
        m_pWifSSid = (CEditUI)m_pManagerUI.GetViewOfName("ssid");

        WifiManager wifiManager = (WifiManager) getApplicationContext().getSystemService(Context.WIFI_SERVICE);
        WifiInfo info = wifiManager.getConnectionInfo();

        String ssid = info.getSSID().replace("\"", "");
        m_pWifSSid.setText(ssid);
    }

    @Override
    public boolean OnNotify(UIAttribute pSender, int event, Object param) {
        if (event == UIManagerDelegate.FYEVENT_TOUCHUPINSIDE) {
            if (pSender == m_pBackBt) {
                finish();
            }else if (pSender == m_pNextBt){
                AddBell1.sWifiSSID = m_pWifSSid.getText();
                AddBell1.sWifiPWD = m_pWifiPwd.getText();

                Intent intent = new Intent(AddBell2_WifiActivity.this, AddBell3_PWDActivity.class);
                startActivity(intent);
            }
        }
        return false;
    }
}
