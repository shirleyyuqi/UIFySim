package fyuilib.luoc.com.activity.addbell;

import android.content.Context;
import android.content.Intent;
import android.net.wifi.WifiInfo;
import android.net.wifi.WifiManager;
import android.util.Log;

import fyuilib.luoc.com.control.CButtonUI;
import fyuilib.luoc.com.control.UIAttribute;
import fyuilib.luoc.com.control.UIManagerDelegate;
import fyuilib.luoc.com.util.FyUIActivity;
import hiro.com.hirocamsdk.HRSDKLogin;
import hiro.com.hirocamsdk.HiroCamJNI;
import hiro.com.hirocamsdk.SinVoiceHelper;

/**
 * Created by luoc on 2018/5/26.
 */

public class AddBell4_ConnectActivity extends FyUIActivity {
    private CButtonUI m_pBackBt;
    private CButtonUI m_pNextBt;

    private long        m_iLoginTmPtr;
    private int         m_iHandle;
    private HRSDKLogin  m_loginRst = new HRSDKLogin();

    private int         m_iThreadSt = 0;
    private boolean     m_bThreadRun = false;

    @Override
    public String getUIXml(){
        return "AddBell4Connect.xml";
    }

    @Override
    public void InitControl(){
        m_pBackBt = (CButtonUI)m_pManagerUI.GetViewOfName("back");
        m_pNextBt = (CButtonUI)m_pManagerUI.GetViewOfName("nextBt");

        m_iLoginTmPtr = HiroCamJNI.GetTimeOutPrt(180000);
        m_iHandle =  HiroCamJNI.CreateHandle(AddBell1.sAddItemInfo.strUid);
        if (!AddBell1.sWifiSSID.isEmpty()){
            WifiManager wifiManager = (WifiManager) getApplicationContext().getSystemService(Context.WIFI_SERVICE);
            WifiInfo info = wifiManager.getConnectionInfo();
            Log.d("LinkBell", "Start smart config, localIP:"+info.getIpAddress());
            SinVoiceHelper.sContext = getApplicationContext();
            HiroCamJNI.StartWifiConfig(AddBell1.sWifiSSID, AddBell1.sWifiPWD, AddBell1.sAddItemInfo.strPwd, info.getIpAddress());
        }

        m_iThreadSt = 1;
        m_bThreadRun = true;
        new Thread(new Runnable() {
            @Override
            public void run() {
                m_iThreadSt = 2;
                while (m_bThreadRun){
                    final int ret = HiroCamJNI.Login(m_iHandle, AddBell1.sAddItemInfo.strUsr, AddBell1.sAddItemInfo.strPwd
                            , 0, m_loginRst, m_iLoginTmPtr);
                    if (HiroCamJNI.GetTimeOutValue(m_iLoginTmPtr) != 0) {
                        if (ret == HiroCamJNI.HRSDKCMDRST_USRORPWD_ERR || ret == HiroCamJNI.HRSDKCMDRST_SUCCESS) {
                            runOnUiThread(new Runnable() {
                                private int mRet;
                                public Runnable setParam(int param) {
                                    mRet = param;
                                    return  this;
                                }

                                @Override
                                public void run() {
                                    AddRst(mRet);
                                }
                            }.setParam(ret));
                        }
                    }

                    if (ret == HiroCamJNI.HRSDKCMDRST_TIMEOUT){
                        runOnUiThread(new Runnable() {
                            @Override
                            public void run() {
                                //dismiss
                            }
                        });
                    }
                }
                m_iThreadSt = 3;
            }
        }).start();
    }

    private void QuitThread() {
        HiroCamJNI.StopWifiConfig();

        m_bThreadRun = false;
        HiroCamJNI.SetTimeOutValue(m_iLoginTmPtr, 0);
        try {
            while (!(m_iThreadSt == 0 || m_iThreadSt == 3)) {
                Thread.sleep(10);
            }
        }catch (Exception e){

        }

        HiroCamJNI.ReleaseTimeOutPrt(m_iLoginTmPtr);
        HiroCamJNI.ReleaseHandle(m_iHandle);
    }

    private void AddRst(int rst){
        QuitThread();

        if (rst == HiroCamJNI.HRSDKCMDRST_SUCCESS){
            AddBell1.sAddItemInfo.iEventChn = m_loginRst.eventChannel;

            Intent intent = new Intent(AddBell4_ConnectActivity.this, AddBell5_NameActivity.class);
            startActivity(intent);
        }else if (rst == HiroCamJNI.HRSDKCMDRST_USRORPWD_ERR){

        }
    }

    @Override
    public boolean OnNotify(UIAttribute pSender, int event, Object param) {
        if (event == UIManagerDelegate.FYEVENT_TOUCHUPINSIDE) {
            if (pSender == m_pBackBt) {
                QuitThread();
                finish();
            }else if (pSender == m_pNextBt){

            }
        }
        return false;
    }
}
