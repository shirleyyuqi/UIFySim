package fyuilib.luoc.com.activity.addbell;

import android.app.Activity;
import android.content.Context;
import android.content.Intent;
import android.widget.Toast;

import fyuilib.luoc.com.activity.util.CameraInfo;
import fyuilib.luoc.com.activity.util.LocalManager;
import fyuilib.luoc.com.activity.util.Person;
import fyuilib.luoc.com.control.CButtonUI;
import fyuilib.luoc.com.control.CCommonFunUI;
import fyuilib.luoc.com.control.UIAttribute;
import fyuilib.luoc.com.control.UIManagerDelegate;
import fyuilib.luoc.com.core.UIManager;
import fyuilib.luoc.com.util.FyUIActivity;

/**
 * Created by luoc on 2018/5/25.
 */

public class AddBell1 extends FyUIActivity {
    public static CameraInfo.CameraItem sAddItemInfo = new CameraInfo.CameraItem();
    public static String sWifiSSID = "";
    public static String sWifiPWD = "";

    private CButtonUI m_pBackBt;
    private CButtonUI m_pRedBt;
    private CButtonUI m_pOnLineBt;

    @Override
    public String getUIXml(){
        return "AddBell1.xml";
    }

    @Override
    public void InitControl(){
        m_pBackBt = (CButtonUI)m_pManagerUI.GetViewOfName("back");
        m_pRedBt = (CButtonUI)m_pManagerUI.GetViewOfName("bredflash");
        m_pOnLineBt = (CButtonUI)m_pManagerUI.GetViewOfName("bonline");
    }

    @Override
    public boolean OnNotify(UIAttribute pSender, int event, Object param) {
        if (event == UIManagerDelegate.FYEVENT_TOUCHUPINSIDE) {
            if (pSender == m_pBackBt) {
                finish();
            } else if (pSender == m_pRedBt) {
                AddBell1_2_UIDActivity.s_next.SetClickCall(new Person.OnClickCall() {
                    @Override
                    public boolean onClick(Context packageContext, Object obj) {
                        if (LocalManager.IsExist(AddBell1.sAddItemInfo.strUid)){
                            Toast.makeText(getApplicationContext(), UIManager.getTextOfLanguageTag("IDS_DEVISEXIST"), Toast.LENGTH_SHORT).show();
                            return false;
                        }
                        Intent intent = new Intent(packageContext, AddBell2_WifiActivity.class);
                        startActivity(intent);
                        return false;
                    }
                });
                QRScanActivity.s_mandAdd.SetClickCall(new Person.OnClickCall() {
                    @Override
                    public boolean onClick(Context packageContext, Object obj) {
                        Intent intent = new Intent(packageContext, AddBell1_2_UIDActivity.class);
                        startActivity(intent);
                        return false;
                    }
                });
                QRScanActivity.s_audoAdd.SetClickCall(new Person.OnClickCall() {
                    @Override
                    public boolean onClick(Context packageContext, Object obj) {
                        Intent intent = new Intent(AddBell1.this, AddBell2_WifiActivity.class);
                        startActivity(intent);
                        return false;
                    }
                });

                Intent intent = new Intent(AddBell1.this, QRScanActivity.class);
                startActivity(intent);
            }else if (pSender == m_pOnLineBt){
                AddBell1_2_UIDActivity.s_next.SetClickCall(new Person.OnClickCall() {
                    @Override
                    public boolean onClick(Context packageContext, Object obj) {
                        if (LocalManager.IsExist(AddBell1.sAddItemInfo.strUid)){
                            Toast.makeText(getApplicationContext(), UIManager.getTextOfLanguageTag("IDS_DEVISEXIST"), Toast.LENGTH_SHORT).show();
                            return false;
                        }
                        Intent intent = new Intent(packageContext, AddBell2_3_PWDActivity.class);
                        startActivity(intent);
                        return false;
                    }
                });
                QRScanActivity.s_mandAdd.SetClickCall(new Person.OnClickCall() {
                    @Override
                    public boolean onClick(Context packageContext, Object obj) {
                        Intent intent = new Intent(packageContext, AddBell1_2_UIDActivity.class);
                        startActivity(intent);
                        return false;
                    }
                });
                QRScanActivity.s_audoAdd.SetClickCall(new Person.OnClickCall() {
                    @Override
                    public boolean onClick(Context packageContext, Object obj) {
                        //Intent intent = new Intent(AddBell1.this, AddBell1_2_UIDActivity.class);
                        //startActivity(intent);
                        return false;
                    }
                });

                Intent intent = new Intent(AddBell1.this, QRScanActivity.class);
                startActivity(intent);
            }
        }
        return false;
    }
}
