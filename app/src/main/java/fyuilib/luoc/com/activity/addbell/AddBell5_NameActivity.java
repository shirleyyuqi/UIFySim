package fyuilib.luoc.com.activity.addbell;

import android.content.Intent;

import fyuilib.luoc.com.activity.MainActivity;
import fyuilib.luoc.com.activity.util.LocalManager;
import fyuilib.luoc.com.control.CButtonUI;
import fyuilib.luoc.com.control.CEditUI;
import fyuilib.luoc.com.control.UIAttribute;
import fyuilib.luoc.com.control.UIManagerDelegate;
import fyuilib.luoc.com.util.FyUIActivity;

/**
 * Created by luoc on 2018/5/26.
 */

public class AddBell5_NameActivity extends FyUIActivity {
    private CButtonUI m_pBackBt;
    private CButtonUI m_pNextBt;
    private CButtonUI m_pHeadDoor;
    private CButtonUI m_pTailDoor;
    private CEditUI   m_pUsrDef;

    @Override
    public String getUIXml(){
        return "AddBell5Name.xml";
    }

    @Override
    public void InitControl(){
        m_pBackBt = (CButtonUI)m_pManagerUI.GetViewOfName("back");
        m_pNextBt = (CButtonUI)m_pManagerUI.GetViewOfName("nextBt");
        m_pHeadDoor = (CButtonUI)m_pManagerUI.GetViewOfName("headDoor");
        m_pTailDoor = (CButtonUI)m_pManagerUI.GetViewOfName("tailDoor");
        m_pUsrDef = (CEditUI)m_pManagerUI.GetViewOfName("usrDef");
    }

    @Override
    public boolean OnNotify(UIAttribute pSender, int event, Object param) {
        if (event == UIManagerDelegate.FYEVENT_TOUCHUPINSIDE){
            if (pSender == m_pBackBt){
                finish();
            }else if (pSender == m_pNextBt){
                AddBell1.sAddItemInfo.isSysLan = "true";
                AddBell1.sAddItemInfo.strDescription = m_pUsrDef.getText();
                LocalManager.addCameraInfo(AddBell1.sAddItemInfo);
                AddBell1.sAddItemInfo.clear();

                //pushMsgToQue

                Intent intent = new Intent(getApplicationContext(), MainActivity.class);
                intent.setFlags(Intent.FLAG_ACTIVITY_CLEAR_TOP);
                startActivity(intent);
            }else if(pSender == m_pHeadDoor){
                AddBell1.sAddItemInfo.isSysLan = "false";
                AddBell1.sAddItemInfo.strDescription = "IDS_HEAD_DOOR";
                LocalManager.addCameraInfo(AddBell1.sAddItemInfo);
                AddBell1.sAddItemInfo.clear();

                //pushMsgToQue

                Intent intent = new Intent(getApplicationContext(), MainActivity.class);
                intent.setFlags(Intent.FLAG_ACTIVITY_CLEAR_TOP);
                startActivity(intent);
            }else if (pSender == m_pTailDoor){
                AddBell1.sAddItemInfo.isSysLan = "false";
                AddBell1.sAddItemInfo.strDescription = "IDS_TAIL_DOOR";
                LocalManager.addCameraInfo(AddBell1.sAddItemInfo);
                AddBell1.sAddItemInfo.clear();

                //pushMsgToQue

                Intent intent = new Intent(getApplicationContext(), MainActivity.class);
                intent.setFlags(Intent.FLAG_ACTIVITY_CLEAR_TOP);
                startActivity(intent);
            }
        }

        return false;
    }
}
