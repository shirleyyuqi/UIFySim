package hiro.com.hirocamsdk;
import android.content.Context;
import android.graphics.Bitmap;
import android.util.Base64;
import android.util.Log;

import com.hisilicon.hisilinkapi.HisiLibApi;
import com.libra.sinvoice.Common;
import com.libra.sinvoice.SinVoicePlayer;

import org.json.JSONException;
import org.json.JSONObject;

import java.util.ArrayList;

/**
 * Created by luoc on 2018/5/1.
 */

public class HiroCamJNI {
    public static final int HRSDKCMDRST_SUCCESS = 0x0;
    public static final int HRSDKCMDRST_HNDLE_ERR = 0x1;
    public static final int HRSDKCMDRST_TIMEOUT = 0x2;
    public static final int HRSDKCMDRST_TIMESYNC_ERR = 0x3;
    public static final int HRSDKCMDRST_USRORPWD_ERR = 0x4;
    public static final int HRSDKCMDRST_ARG_ERR = 0x5;
    public static final int HRSDKCMDRST_ACCESSDENY = 0x6;
    public static final int HRSDKCMDRST_SYSBUSY = 0x7;
    public static final int HRSDKCMDRST_UNSUPPORT = 0x8;
    public static final int HRSDKCMDRST_ACHIEVE_FILE_MAXSIZE = 0x9;
    public static final int HRSDKCMDRST_ACHIEVE_FILE_CHANGE_RES = 0xa;
    public static final int HRSDKCMDRST_ERR_UNKNOW = 0xb;

    public static SinVoiceHelper sSinVoiceHelper = null;


    static {
        try
        {
            System.loadLibrary("HiroCamSDK");
        }catch(UnsatisfiedLinkError e)
        {
            e.printStackTrace();
        }
        try
        {
            System.loadLibrary("DPS_API_PPCS");
        }catch(UnsatisfiedLinkError e)
        {
            e.printStackTrace();
        }
        try
        {
            System.loadLibrary("NDT_API_PPCS");
        }catch(UnsatisfiedLinkError e)
        {
            e.printStackTrace();
        }
        try
        {
            System.loadLibrary("mp4v2");
        }catch(UnsatisfiedLinkError e)
        {
            e.printStackTrace();
        }

        try {
            System.loadLibrary("sinvoice");
        } catch (UnsatisfiedLinkError e){
            e.printStackTrace();
        }
    }

    public static void StartWifiConfig(String ssid, String wifiPwd, String devPwd, int localIp){

        JSONObject jsonObj = new JSONObject();
        try {
            jsonObj.put("wifi_pwd", wifiPwd);
            jsonObj.put("dev_pwd", devPwd);
        } catch (JSONException e) {
        }
        HisiLibApi.setNetworkInfo(-1, 1131, 0, localIp, ssid, jsonObj.toString(), "");
        HisiLibApi.startMulticast();

        if (sSinVoiceHelper != null){
            sSinVoiceHelper.StopWifiConfig();
            sSinVoiceHelper = null;
        }
        sSinVoiceHelper = new SinVoiceHelper();
        sSinVoiceHelper.StartWifiConfig(ssid, wifiPwd, devPwd);
    }

    public static void StopWifiConfig(){
        HisiLibApi.stopMulticast();
        if (sSinVoiceHelper != null) {
            sSinVoiceHelper.StopWifiConfig();
            sSinVoiceHelper = null;
        }
    }

    public native static String stringFromJNI();
    public native static long GetTimeOutPrt(int value);
    public native static long ReleaseTimeOutPrt(long ptr);
    public native static long SetTimeOutValue(long ptr, int value);
    public native static long GetTimeOutValue(long ptr);

    public native static void Init();
    public native static void UnInit();

    public native static int CreateHandle(String uid);
    public native static int ReleaseHandle(int handle);
    public native static int Login(int handle, String usr, String pwd, int bLive, HRSDKLogin loginRst, long timeOutPrt);
    public native static int Logout(int handle);

    public native static int StartTalk(int handle, long timeOutPrt);
    public native static int StopTalk(int handle, long timeOutPrt);

    public native static int ChangePwd(int handle, String oldPwd, String newPwd, long timeOutPrt);
    public native static int VideoFlip(int handle, long timeOutPrt);
    public native static int VideoMirror(int handle, long timeOutPrt);

    public native static int GetDevInfo(int handle, GetDevInfo getDevInfo, long timeOutPrt);
    public native static int SetDevInfo(int handle, SetDevInfo getDevInfo, long timeOutPrt);

    public native static int GetPirInfo(int handle, GetPirInfo gePirInfo, long timeOutPrt);
    public native static int SetPirInfo(int handle, SetPirInfo setPirInfo, long timeOutPrt);

    public native static int FormatDCardInfo(int handle, SDCardInfo sdCardInfo, long timeOutPrt);
    public native static int GetRecordMap(int handle, int year, int month, RecordMap recordMap, long timeOutPrt);
    public native static int GetRecordList(int handle, int year, int month, int day, RecordList list, long timeOutPrt);

    public native static int SendTalkData(int handle, byte[] data, int datalen);
    public native static int UpdateBitmap(int handle, Bitmap bitmap);

    public native static int StartPlayback(int handle, String filename, RecordInfo recordInfo, long timeOutPtr);
    public native static int StopPlayback(int handle, long timeOutPtr);

    public native static int StartRecord(int handle, int type, String filename);
    public native static int StopRecord(int handle);

    public native static int SubScribe(String uid, String agName, String appName, String devToken, int eventChn);
    public native static int UnSubScribe(String uid, String agName, String appName, String devToken, int eventChn);
    public native static int ResetBadge(String uid, String agName, String appName, String devToken, int eventChn);

    public native static int GetKlResult(String uid);
    public native static int QueryKlSt(String uid);

    //public native static int StartWifiConfig(String ssid, String wifiPwd, String devPwd);
    //public native static int StopWifiConfig();
}
