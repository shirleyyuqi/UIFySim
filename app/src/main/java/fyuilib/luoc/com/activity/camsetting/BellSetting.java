package fyuilib.luoc.com.activity.camsetting;

import android.app.Activity;
import android.content.Intent;

import fyuilib.luoc.com.activity.MainActivity;
import fyuilib.luoc.com.control.CButtonUI;
import fyuilib.luoc.com.control.CListElementUI;
import fyuilib.luoc.com.control.CListUI;
import fyuilib.luoc.com.control.UIAttribute;
import fyuilib.luoc.com.control.UIManagerDelegate;
import fyuilib.luoc.com.util.FyUIActivity;

/**
 * Created by luoc on 2018/5/26.
 */

public class BellSetting extends FyUIActivity {
    private CButtonUI m_pBackBt;
    private CListUI   m_pList;

    @Override
    public String getUIXml(){
        return "bellsetting.xml";
    }

    @Override
    public void InitControl(){
        m_pBackBt = (CButtonUI)m_pManagerUI.GetViewOfName("back");
        m_pList = (CListUI)m_pManagerUI.GetViewOfName("list");
    }

    @Override
    public boolean OnNotify(UIAttribute pSender, int event, Object param) {
        if (pSender == m_pBackBt){
            finish();
        }else if (event == UIManagerDelegate.FYEVENT_TB_SELECTED){
            if (pSender == m_pList){
                CListElementUI v = (CListElementUI)param;
                switch (v.GetCommFun().m_iTag){
                    case 0:
                    {
                        Intent intent =new Intent(BellSetting.this, DevInfoSetting.class);
                        startActivity(intent);
                    }
                        break;
                    case 1:
                    {
                        Intent intent =new Intent(BellSetting.this, PasswordSetting.class);
                        startActivity(intent);
                    }
                        break;
                    case 2:
                    {
                        Intent intent =new Intent(BellSetting.this, MotionActivity.class);
                        startActivity(intent);
                    }
                        break;
                    case 3:
                    {
                        Intent intent =new Intent(BellSetting.this, ImageSetting.class);
                        startActivity(intent);
                    }
                        break;
                    case 4:
                    {
                        Intent intent =new Intent(BellSetting.this, PowerFreqSetting.class);
                        startActivity(intent);
                    }
                        break;
                    case 5:
                    {
                        Intent intent =new Intent(BellSetting.this, VolumeSetting.class);
                        startActivity(intent);
                    }
                        break;
                    case 6:
                    {
                        Intent intent =new Intent(BellSetting.this, SharedQRCode.class);
                        startActivity(intent);
                    }
                        break;
                }
            }
        }
        return false;
    }
}