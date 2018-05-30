package hiro.com.hirocamsdk;

import android.content.Context;
import android.util.Base64;
import android.util.Log;

import com.libra.sinvoice.Common;
import com.libra.sinvoice.SinVoicePlayer;

import java.util.ArrayList;

/**
 * Created by luoc on 2018/5/27.
 */

public class SinVoiceHelper {
    public static Context   sContext;
    private ArrayList<StringBuilder> wifiSyncMsgList = new ArrayList<>();
    private boolean         mSinVoiceSendThreadRun;
    private int             mSinVoiceStep = 0;
    private int             mLastSinVoiceStep = 0;
    private Thread          mSinvoiceSendThread;
    private SinVoicePlayer  mSinVoicePlayer = new SinVoicePlayer(32000, 3);

    public void StartWifiConfig(String wifiSSID, String wifiPwd, String devPwd){
        mSinVoicePlayer.init(sContext);
        mSinVoicePlayer.setListener(new SinVoicePlayer.Listener() {
            @Override
            public void onSinVoicePlayStart() {

            }

            @Override
            public void onSinVoicePlayEnd() {
                if( (++mLastSinVoiceStep) >= wifiSyncMsgList.size() ){
                    mLastSinVoiceStep = 0;
                }
                mSinVoiceStep = mLastSinVoiceStep;
            }

            @Override
            public void onSinToken(int[] tokens) {

            }
        });

        // 格式：
        // 分包发送，每个包36个字节
        // 第一个包: 序号+总包数+ssid长度+pwd长度+dev_pwd长度+数据
        // 之后的包: 序号+数据
        char[] codeBook = Common.DEFAULT_CODE_BOOK.toCharArray();
        String ssidBase64 = Base64.encodeToString(wifiSSID.getBytes(), Base64.NO_WRAP);
        StringBuilder sinVoiceData = new StringBuilder();
        sinVoiceData.append(codeBook[0]); // 总包数，占位，后面修改
        sinVoiceData.append(codeBook[ssidBase64.length()]);
        sinVoiceData.append(codeBook[wifiPwd.length()]);
        sinVoiceData.append(codeBook[devPwd.length()]);
        sinVoiceData.append(ssidBase64);
        sinVoiceData.append(wifiPwd);
        sinVoiceData.append(devPwd);

        wifiSyncMsgList.clear();

        int remainDataLen = sinVoiceData.length();
        int dataOffset = 0;
        int packSeq = 0;
        do{
            if( remainDataLen > 35 ){
                wifiSyncMsgList.add(new StringBuilder(""+codeBook[packSeq]+sinVoiceData.substring(dataOffset, dataOffset+35)));
                dataOffset += 35;
                remainDataLen -= 35;
            }else{
                wifiSyncMsgList.add(new StringBuilder(""+codeBook[packSeq]+sinVoiceData.substring(dataOffset)));
                remainDataLen -= (sinVoiceData.length()-dataOffset);
            }
            packSeq++;

        }while( remainDataLen > 0 );
        // 设置总包数
        wifiSyncMsgList.get(0).setCharAt(1, codeBook[packSeq]);


        mSinVoiceStep = 0;
        mLastSinVoiceStep = 0;
        mSinVoiceSendThreadRun = true;
        mSinvoiceSendThread = new Thread(new Runnable() {
            @Override
            public void run() {
                while( mSinVoiceSendThreadRun ) {
                    if( mSinVoiceStep != -1 ){
                        mSinVoicePlayer.stop();
                        mSinVoicePlayer.play(wifiSyncMsgList.get(mLastSinVoiceStep).toString(), false, 1000);
                        mSinVoiceStep = -1;
                    }else{
                        try {
                            Thread.sleep(10);
                        } catch (InterruptedException e) {
                            e.printStackTrace();
                        }
                    }
                }
            }
        });
        mSinvoiceSendThread.start();
    }

    public void StopWifiConfig(){
        if( mSinvoiceSendThread != null ){
            try {
                mSinVoiceSendThreadRun = false;
                mSinVoicePlayer.stop();
                mSinvoiceSendThread.join();
                mSinvoiceSendThread = null;
            } catch (InterruptedException e) {
                e.printStackTrace();
            }
        }
    }
}
