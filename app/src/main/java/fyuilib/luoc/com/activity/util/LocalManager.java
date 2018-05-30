package fyuilib.luoc.com.activity.util;

import android.app.Activity;
import android.content.SharedPreferences;
import android.widget.CalendarView;

/**
 * Created by luoc on 2018/5/27.
 */

/*
* 取数据时直接使用静态变量sCameraInfo
* */
public class LocalManager {
    public static Activity  sActivity;
    public static CameraInfo sCameraInfo = null;

    public static CameraInfo getLocalCamareInfo(){
        SharedPreferences sp = sActivity.getSharedPreferences("HiroCam", Activity.MODE_PRIVATE);
        String info = sp.getString("CameraInfo","");
        if (info == ""){
            return new CameraInfo();
        }

        CameraInfo Camera = new CameraInfo();
        Camera.fromJsonString(info);
        return Camera;
    }

    public void replaceCameraInfo(CameraInfo info){
        sCameraInfo = info;
        SharedPreferences sp = sActivity.getSharedPreferences("HiroCam",Activity.MODE_PRIVATE);//创建sp对象
        SharedPreferences.Editor editor = sp.edit() ;
        editor.putString("CameraInfo", sCameraInfo.toJsonString());
        editor.commit();
    }

    public static void saveLocalLanguageInfo(String lan){
        SharedPreferences sp = sActivity.getSharedPreferences("HiroCam",Activity.MODE_PRIVATE);//创建sp对象
        SharedPreferences.Editor editor = sp.edit() ;
        editor.putString("LANINFO", lan);
        editor.commit();
    }

    public static String getLocalLanguageInfo(){
        SharedPreferences sp = sActivity.getSharedPreferences("HiroCam", Activity.MODE_PRIVATE);
        String lan = sp.getString("LANINFO","");
        if (lan == ""){
            return "language/cn.xml";
        }
        return lan;
    }

    public static void saveLocalCameraInfo(){
        SharedPreferences sp = sActivity.getSharedPreferences("HiroCam",Activity.MODE_PRIVATE);//创建sp对象
        SharedPreferences.Editor editor = sp.edit() ;
        editor.putString("CameraInfo", sCameraInfo.toJsonString());
        editor.commit();
    }

    public static boolean IsExist(String uid){
        return sCameraInfo.IsExist(uid);
    }

    public static boolean addCameraInfo(CameraInfo.CameraItem item){
        if ( sCameraInfo.Add(item) ){
            saveLocalCameraInfo();
            return true;
        }
        return false;
    }

    public static void removeCameraInfo(CameraInfo.CameraItem item){
        if ( sCameraInfo.Remove(item) ){
            saveLocalCameraInfo();
        }
    }

    public static void removeCamera(){
        SharedPreferences sp = sActivity.getSharedPreferences("HiroCam",Activity.MODE_PRIVATE);//创建sp对象
        SharedPreferences.Editor editor = sp.edit() ;
        editor.putString("CameraInfo", "");
        editor.commit();
    }
}
