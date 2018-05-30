package fyuilib.luoc.com.activity.util;

import org.json.JSONArray;
import org.json.JSONException;
import org.json.JSONObject;

import java.util.ArrayList;
import java.util.List;

/**
 * Created by luoc on 2018/5/27.
 */

public class CameraInfo {
    public static class CameraItem{
        public CameraItem(){

        }

        public CameraItem(String descript, String uid, String usr, String pwd, int eventChn, String SysLan){
            strDescription = descript;
            strUid = uid;
            strUid = usr;
            strPwd = pwd;
            isSysLan = SysLan;
            iEventChn = eventChn;
        }

        public void clear(){
            strDescription = null;
            strUid = null;
            strUid = null;
            strPwd = null;
            isSysLan = null;
            iEventChn = 0;
        }

        public  String  strDescription;
        public  String  strUid;
        public  String  strUsr;
        public  String  strPwd;
        public  String  isSysLan;
        public  int     iEventChn;
    }
    public ArrayList<CameraItem> mList = new ArrayList<CameraItem>();

    public void Add(String descript, String uid, String usr, String pwd, int eventChn, String isSysLan){
        CameraItem item = new CameraItem();
        item.strDescription = descript;
        item.strUid = uid;
        item.strUid = usr;
        item.strPwd = pwd;
        item.isSysLan = isSysLan;
        item.iEventChn = eventChn;

        mList.add(item);
    }

    public boolean Add(CameraItem item){
        if (item != null){
            mList.add(item);
            return true;
        }
        return false;
    }

    public boolean Remove(int index){
        if (mList.size() > index){
            mList.remove(index);
            return true;
        }
        return false;
    }

    public boolean Remove(CameraItem item){
        int count = mList.size();
        for (int i=0; i<count; i++){
            CameraItem o = mList.get(i);
            if (o.strUid.compareTo(item.strUid) == 0){
                mList.remove(i);
                return true;
            }
        }
        return false;
    }

    public CameraItem get(int index){
        if (mList.size() > index){
            return mList.get(index);
        }
        return null;
    }

    public boolean IsExist(String strUid){
        int count = mList.size();
        for (int i=0; i<count; i++){
            CameraItem o = mList.get(i);
            if (o.strUid.compareTo(strUid) == 0){
                return true;
            }
        }
        return false;
    }

    public String toJsonString() {
        String jsonresult = "";
        JSONObject object = new JSONObject();
        try {
            JSONArray jsonarray = new JSONArray();
            int count = mList.size();
            for (int i=0; i<count; i++){
                CameraItem item = mList.get(i);
                JSONObject jsonObj = new JSONObject();
                jsonObj.put("Discription", item.strDescription);
                jsonObj.put("uid", item.strUid);
                jsonObj.put("usr", item.strUsr);
                jsonObj.put("pwd", item.strPwd);
                jsonObj.put("eventchn", item.iEventChn);
                jsonObj.put("syslan", item.isSysLan);
                jsonarray.put(jsonObj);
            }
            object.put("CameraInfo", jsonarray);
            jsonresult = object.toString();
        } catch (JSONException e) {
            // TODO Auto-generated catch block
            e.printStackTrace();
        }
        return jsonresult;
    }

    public boolean fromJsonString(String json){
        try {
            JSONObject jsonObject=new JSONObject(json);
            JSONArray jsonArray = jsonObject.getJSONArray("CameraInfo");
            int count = jsonArray.length();
            for (int i=0; i<count; i++){
                JSONObject jsonitem = jsonArray.getJSONObject(i);

                CameraItem item = new CameraItem();
                item.strDescription = jsonitem.getString("Discription");
                item.strUid = jsonitem.getString("uid");
                item.strUsr = jsonitem.getString("usr");
                item.strPwd = jsonitem.getString("pwd");
                item.iEventChn = jsonitem.getInt("eventchn");
                item.isSysLan = jsonitem.getString("syslan");

                mList.add(item);
            }
            return true;
        } catch (JSONException e) {
            e.printStackTrace();
            return false;
        }
    }
}
