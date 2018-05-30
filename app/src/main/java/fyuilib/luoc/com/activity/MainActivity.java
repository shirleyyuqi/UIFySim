package fyuilib.luoc.com.activity;

import android.Manifest;
import android.app.Activity;
import android.content.Intent;
import android.os.Bundle;
import android.view.View;
import android.view.ViewGroup;
import android.view.Window;

import java.util.ArrayList;
import java.util.List;

import fyuilib.luoc.com.activity.addbell.AddBell1;
import fyuilib.luoc.com.activity.camsetting.BellSetting;
import fyuilib.luoc.com.activity.camsetting.DevInfoActivity;
import fyuilib.luoc.com.activity.camsetting.MotionActivity;
import fyuilib.luoc.com.activity.util.CameraInfo;
import fyuilib.luoc.com.activity.util.LocalManager;
import fyuilib.luoc.com.control.CButtonUI;
import fyuilib.luoc.com.control.CListUI;
import fyuilib.luoc.com.control.COptionUI;
import fyuilib.luoc.com.control.CTabLayoutUI;
import fyuilib.luoc.com.control.UIAttribute;
import fyuilib.luoc.com.control.UIManagerDelegate;
import fyuilib.luoc.com.core.CXmlParse;
import fyuilib.luoc.com.core.UIDlgBuild;
import fyuilib.luoc.com.core.UIManager;
import fyuilib.luoc.com.util.FyUIActivity;
import hiro.com.hirocamsdk.HiroCamJNI;
import pub.devrel.easypermissions.AfterPermissionGranted;
import pub.devrel.easypermissions.EasyPermissions;

/**
 * Created by luoc on 2018/5/23.
 */

public class MainActivity extends FyUIActivity implements EasyPermissions.PermissionCallbacks{
    private static final int REQUEST_CODE_QRCODE_PERMISSIONS = 1;

    private CTabLayoutUI m_pTab;
    private CListUI      m_pListUI;
    private CListUI      m_pCamListUI;
    private CButtonUI    m_pAddBt;

    @Override
    public void Init() {
        HiroCamJNI.Init();
        LocalManager.sActivity = this;
        LocalManager.sCameraInfo = LocalManager.getLocalCamareInfo();
        CXmlParse.s_ass = getAssets();
        UIDlgBuild.s_ass = getAssets();
        UIManager.sScreenH = getResources().getDisplayMetrics().heightPixels;
        UIManager.sScreenW = getResources().getDisplayMetrics().widthPixels;

        UIManager.ReloadLoadLanguage(LocalManager.getLocalLanguageInfo());

        //test数据
       /* for (int i=0; i<5; i++){
            CameraInfo.CameraItem item = new CameraInfo.CameraItem("测试"+i, "AACS-00100-MCSA"+i
                    , "a", "a", 0, "true");
            LocalManager.sCameraInfo.Add(item);
        }
        LocalManager.saveLocalCameraInfo();*/
    }

    @Override
    protected  void onDestroy() {
        HiroCamJNI.UnInit();
        super.onDestroy();
    }

    @Override
    public String getUIXml(){
        return "main.xml";
    }

    @Override
    public void InitControl(){
        m_pTab = (CTabLayoutUI) m_pManagerUI.GetViewOfName("tab");
        m_pListUI = (CListUI)m_pManagerUI.GetViewOfName("list");
        m_pAddBt = (CButtonUI)m_pManagerUI.GetViewOfName("add");
        m_pCamListUI =(CListUI)m_pManagerUI.GetViewOfName("camList");

        ArrayList<CameraInfo.CameraItem> list = LocalManager.sCameraInfo.mList;
        int count = list.size();
        for (int i=0; i<count; i++){
            CameraInfo.CameraItem item = list.get(i);
            m_pCamListUI.addValue("tname", "lgOff", item.isSysLan, i);
            m_pCamListUI.addValue("tname", "text", item.strDescription, i);
            m_pCamListUI.addValue("tsetting", "tag", Integer.toString(i), i);
            m_pCamListUI.addValue("tuid", "text", item.strUid, i);
        }

        //Zm_pCamListUI.addValue("tname", "text", "aaa", 0);
        m_pCamListUI.reloadData();
    }

    @Override
    public boolean OnNotify(UIAttribute pSender, int event, Object param) {
        if (event == UIManagerDelegate.FYEVENT_TOUCHUPINSIDE){
            if (pSender == m_pAddBt){
                Intent intent =new Intent(MainActivity.this,AddBell1.class);
                startActivity(intent);
            }else if (pSender.GetCommFun().mName.compareTo("tsetting") == 0){
                Intent intent =new Intent(MainActivity.this,BellSetting.class);
                startActivity(intent);
            }else if (pSender.GetCommFun().mName.compareTo("tplayback") == 0){
                Intent intent =new Intent(MainActivity.this,SDCardActivity.class);
                startActivity(intent);
            }else if (pSender.GetCommFun().mName.compareTo("timg") == 0){
                LiveActivity.sPlayItem = LocalManager.sCameraInfo.get(0);

                Intent intent =new Intent(MainActivity.this,LiveActivity.class);
                startActivity(intent);
            }
        }else if (event == UIManagerDelegate.FYEVENT_OP_SELECTED){
            m_pTab.SelectIndex(((COptionUI)pSender).getUsrTag());

        }else if (event == UIManagerDelegate.FYEVENT_TB_SELECTED){
            if (pSender == m_pListUI){
                Intent intent =new Intent(MainActivity.this,SysSettingActivity.class);
                startActivity(intent);
            }else{

            }
        }
        return false;
    }

    @Override
    protected void onStart() {
        super.onStart();
        requestCodeQRCodePermissions();
    }

    @Override
    public void onRequestPermissionsResult(int requestCode, String[] permissions, int[] grantResults) {
        EasyPermissions.onRequestPermissionsResult(requestCode, permissions, grantResults, this);
    }

    @AfterPermissionGranted(REQUEST_CODE_QRCODE_PERMISSIONS)
    private void requestCodeQRCodePermissions() {
        String[] perms = {Manifest.permission.CAMERA, Manifest.permission.READ_EXTERNAL_STORAGE};
        if (!EasyPermissions.hasPermissions(this, perms)) {
            EasyPermissions.requestPermissions(this, "扫描二维码需要打开相机和散光灯的权限", REQUEST_CODE_QRCODE_PERMISSIONS, perms);
        }
    }

    @Override
    public void onPermissionsGranted(int requestCode, List<String> perms) {

    }

    @Override
    public void onPermissionsDenied(int requestCode, List<String> perms) {

    }
}
