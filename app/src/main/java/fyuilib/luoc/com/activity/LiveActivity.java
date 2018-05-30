package fyuilib.luoc.com.activity;

import android.graphics.Bitmap;
import android.hardware.ConsumerIrManager;
import android.view.View;

import fyuilib.luoc.com.activity.util.CameraInfo;
import fyuilib.luoc.com.control.CButtonUI;
import fyuilib.luoc.com.control.CGifUI;
import fyuilib.luoc.com.control.CImageUI;
import fyuilib.luoc.com.control.COptionUI;
import fyuilib.luoc.com.control.CSufaceView;
import fyuilib.luoc.com.control.UIAttribute;
import fyuilib.luoc.com.util.FyUIActivity;
import hiro.com.hirocamsdk.HRSDKLogin;
import hiro.com.hirocamsdk.HiroCamJNI;

/**
 * Created by luoc on 2018/5/26.
 */

public class LiveActivity extends FyUIActivity {
    public static CameraInfo.CameraItem  sPlayItem;
    private CButtonUI m_pBackBt;
    private CImageUI  m_pImageView;
    private COptionUI m_pTalk;
    private COptionUI m_pAudio;
    private COptionUI m_pRecord;
    private CButtonUI m_pSnap;
    private CGifUI    m_pWait;
    //private Bitmap    m_bitMap;


    enum  CMD{
        CMD_NONE,
        CMD_STARTTALK,
        CMD_STOPTALK,
    };

    private int         m_iHandle;
    private long        m_iLoginTmPtr;
    private boolean     m_bThreadRun;
    private int         m_iThreadSt;

    private long        m_iTalkCmdTmPtr;
    private int         m_iThreadTalkSt;
    private boolean     m_bThreadRunTalk;
    private CMD         m_iCmdSt;

    @Override
    public String getUIXml(){
        return "live.xml";
    }

    @Override
    public void InitControl(){
        m_pBackBt = (CButtonUI)m_pManagerUI.GetViewOfName("back");
        m_pImageView = (CImageUI)m_pManagerUI.GetViewOfName("video");
        m_pAudio = (COptionUI)m_pManagerUI.GetViewOfName("audio");
        m_pTalk = (COptionUI)m_pManagerUI.GetViewOfName("talk");
        m_pRecord = (COptionUI)m_pManagerUI.GetViewOfName("record");
        m_pSnap = (CButtonUI)m_pManagerUI.GetViewOfName("snap");
        m_pWait = (CGifUI)m_pManagerUI.GetViewOfName("wait");

        m_pWait.setVisibility(View.INVISIBLE);
        m_iCmdSt = CMD.CMD_NONE;

        m_iHandle = HiroCamJNI.CreateHandle(sPlayItem.strUid);
        //m_bitMap = Bitmap.createBitmap(1280, 720, Bitmap.Config.RGB_565);
        RunSDK();
    }

    void RunSDK()
    {
        m_iLoginTmPtr = HiroCamJNI.GetTimeOutPrt(30000);
        m_bThreadRun = true;
        m_iThreadSt = 1;

        new Thread(new Runnable() {
            @Override
            public void run() {
                ThreadRun();
            }
        }).start();

        m_iTalkCmdTmPtr = HiroCamJNI.GetTimeOutPrt(3000);
        m_iThreadTalkSt = 1;
        m_bThreadRunTalk = true;
        new Thread(new Runnable() {
            @Override
            public void run() {
                ThreadRunTalk();
            }
        }).start();
    }

    @Override
    public boolean OnNotify(UIAttribute pSender, int event, Object param) {
        if (pSender == m_pBackBt){
            finish();
        }
        return false;
    }

    void ThreadRun()
    {
        m_iThreadSt = 2;

        HRSDKLogin loginRst = new HRSDKLogin();
        int rst = HiroCamJNI.HRSDKCMDRST_TIMEOUT;
        int outLenV = 0;
        int outLenA = 0;
        boolean bSleep = false;
        while (m_bThreadRun) {
            if (HiroCamJNI.HRSDKCMDRST_SUCCESS != rst) {
                rst = HiroCamJNI.Login(m_iHandle, sPlayItem.strUsr, sPlayItem.strPwd, 1, loginRst, m_iLoginTmPtr);
                if (rst != HiroCamJNI.HRSDKCMDRST_SUCCESS) {
                    if (HiroCamJNI.HRSDKCMDRST_USRORPWD_ERR == rst) {
                        //[self performSelectorOnMainThread:@selector(UsrOrPwdErr) withObject:nil waitUntilDone:NO];
                        break;
                    }

                    try{Thread.sleep(40*1000);}catch (Exception e){}
                    continue;
                }

                //[m_pWait performSelectorOnMainThread:@selector(stopGif) withObject:nil waitUntilDone:NO];
            }/*else if (m_isGifRun) {
                m_isGifRun = NO;
                [m_pWait performSelectorOnMainThread:@selector(stopGif) withObject:nil waitUntilDone:NO];
            }*/

            bSleep = true;
            if (0 == HiroCamJNI.UpdateBitmap(m_iHandle, m_pImageView.getBitmap(1, 1280, 720))){
                runOnUiThread(new Runnable() {
                    @Override
                    public void run() {
                        m_pImageView.invalidate();
                    }
                });
            }

            /*if (0 == HiroCamJNI.UpdateBitmap(m_iHandle, m_bitMap)){
                m_pImageView.DrawBitmap(m_bitMap);
            }*/
/*
            if ( HiroCamJNI.HRSDKCMDRST_SUCCESS == HiroCamSDK_GetAudioData(m_iHandle, &pAudioFrame, &outLenA) ){
                if (pAudioFrame->type == HRSDKMT_AUDIO) {
                    if (m_bAudio) {
                    [_out_audio WriteData:pAudioFrame->data : pAudioFrame->len];
                        bSleep = NO;
                    }
                }
            }
*/
            if (bSleep)
            {
                try{Thread.sleep(20);}catch (Exception e){}
            }
        }

        m_iThreadSt = 3;
    }

    void ThreadRunTalk()
    {
        m_iThreadTalkSt = 2;

        while (m_bThreadRunTalk) {
            if (m_iCmdSt == CMD.CMD_STARTTALK) {
                if ( HiroCamJNI.HRSDKCMDRST_SUCCESS == HiroCamJNI.StartTalk(m_iHandle, m_iTalkCmdTmPtr) ){
                    /*if (_in_audio) {
                    [_in_audio ReleaseAudio];
                        _in_audio = nil;
                    }
                    _in_audio=[[FCInputAudio alloc] init];
                    [_in_audio InitAudio:&_audio_fmt_inif :640 :input_audio_callback :(__bridge void *)(self)];*/
                }
                m_iCmdSt = CMD.CMD_NONE;
            }else if(m_iCmdSt == CMD.CMD_STOPTALK){
                if ( HiroCamJNI.HRSDKCMDRST_SUCCESS == HiroCamJNI.StopTalk(m_iHandle, m_iTalkCmdTmPtr) ){
                    /*if (_in_audio) {
                        [_in_audio ReleaseAudio];
                        _in_audio = nil;
                    }*/
                }
                m_iCmdSt = CMD.CMD_NONE;
            }else{
                try{
                    Thread.sleep(50*1000);
                }catch (Exception e){

                }
            }
        }

        /*if (_in_audio) {
            [_in_audio ReleaseAudio];
            _in_audio = nil;
        }*/
        m_iThreadTalkSt = 3;
    }
}
