package fyuilib.luoc.com.control;

import android.content.Context;
import android.util.ArraySet;
import android.view.View;
import android.view.ViewGroup;
import android.widget.AbsListView;
import android.widget.BaseAdapter;
import android.widget.PopupMenu;

import java.lang.reflect.Array;
import java.util.ArrayList;
import java.util.IllegalFormatCodePointException;
import java.util.List;

import fyuilib.luoc.com.core.UIDlgBuild;
import fyuilib.luoc.com.core.UIManager;

/**
 * Created by luoc on 2018/5/19.
 */

public class CListUIAdapter extends BaseAdapter {
    class AttrArray{
        public String mName = null;
        public String mAttr = null;
        public String mValue = null;
        public AttrArray(String name, String attr, String value){
            mName = name;
            mAttr = attr;
            mValue = value;
        }
    };
    private Context context;
    private List<CListElementUI> m_pDatas = new ArrayList<CListElementUI>();
    private String               m_strItemXml = null;
    private CListUI              m_pList = null;
    private int                  m_iItemAccessoryType = 0;
    private List<UIManager>      m_plistUIManager = new ArrayList<UIManager>();
    private List<Object>         m_plistData = new ArrayList<Object>();

    public CListUIAdapter(Context context, CListUI pList) {
        this.context = context;
        m_pList = pList;
    }

    public void SetItemXml(String xml){
        m_strItemXml = new String(xml);
    }

    public boolean addValue(String name, String attr, String value, int index) {
        ArrayList<AttrArray> array = null;
        if (m_plistData.size() > index){
            array = (ArrayList<AttrArray>)m_plistData.get(index);
        }else if(m_plistData.size() == index){
            array = new ArrayList<AttrArray>();
            m_plistData.add(array);
        }else{
            return false;
        }

        for (AttrArray a:
                array ) {
            if (a.mName.compareTo(name) == 0 && a.mAttr.compareTo(attr) == 0){
                array.remove(a);
                break;
            }
        }

        AttrArray arr = new AttrArray(name, attr, value);
        array.add(arr);
        return true;
    }

    public String GetValue(String name, String attr, int index){
        if (m_plistData.size() < index) return null;
        ArrayList<AttrArray> array = (ArrayList<AttrArray>)m_plistData.get(index);
        for (AttrArray a:
             array) {
            if (a.mName.compareTo(name) == 0 && a.mAttr.compareTo(attr) == 0){
                return a.mValue;
            }
        }
        return null;
    }

    public void removeValue(int index){
        m_plistData.remove(index);
    }

    public void removeAll(){
        m_plistData.clear();
    }

    public void SetAccessoryType(int type){
        m_iItemAccessoryType = type;
    }

    public void addData(CListElementUI element) {
        m_pDatas.add(element);
        notifyDataSetChanged();
    }

    public void delData(int position) {
        if (m_pDatas.size() > position)
            m_pDatas.remove(position);
    }

    @Override
    public int getCount() {
        if (m_strItemXml == null)
            return m_pDatas.size();
        return m_plistData.size();
    }

    @Override
    public Object getItem(int position) {
        if (m_strItemXml == null)
            return m_pDatas.get(position);
        return m_plistData.get(position);
    }

    @Override
    public long getItemId(int position) {
        return position;
    }

    @Override
    public View getView(int position, View convertView, ViewGroup parent) {
        if (m_strItemXml == null){
            if (m_pDatas.size() > position) {

                CListElementUI listElement = m_pDatas.get(position);
                ViewGroup.LayoutParams lp  = listElement.getLayoutParams();
                if (lp == null){
                    lp = new ViewGroup.LayoutParams(ViewGroup.LayoutParams.MATCH_PARENT, 1);
                    listElement.setLayoutParams(lp);
                }
                if (lp.height != m_pList.getItemHeight()) {
                    lp.height = m_pList.getItemHeight();
                    lp.width = m_pList.getItemWidth();
                    listElement.setLayoutParams(lp);
                }
                listElement.SetAccessoryType(m_iItemAccessoryType);
                return (convertView = listElement);
            }else
                return convertView;
        }else{
            if (convertView == null){
                UIManager pListManager = m_pList.m_pCommonUI.getUIManager();
                UIManager pManagerUI = new UIManager((UIManagerDelegate) pListManager.getActivity());
                UIDlgBuild build = new UIDlgBuild();

                convertView = build.Create(m_strItemXml, pManagerUI, m_pList, pListManager.getActivity());
                if (convertView == null) {
                    return null;
                }

                ((CListElementUI)convertView).iFlag = m_plistUIManager.size();
                m_plistUIManager.add(pManagerUI);
            }

            int iFlag = ((CListElementUI)convertView).iFlag;
            if (m_plistUIManager.size() <= iFlag || m_plistData.size() <= iFlag || m_plistData.size() < position){
                return null;
            }

            ViewGroup.LayoutParams lp  = convertView.getLayoutParams();
            if (lp == null){
                lp = new ViewGroup.LayoutParams(ViewGroup.LayoutParams.MATCH_PARENT, 1);
                convertView.setLayoutParams(lp);
            }
            if (lp.height != m_pList.getItemHeight()) {
                lp.height = m_pList.getItemHeight();
                lp.width = m_pList.getItemWidth();
                convertView.setLayoutParams(lp);
            }

            UIManager pManagerUI = m_plistUIManager.get(iFlag);
            ArrayList<AttrArray> array = (ArrayList<AttrArray>)m_plistData.get(position);
            for (int i=0; i<array.size(); i++){
                AttrArray a = array.get(i);
                UIAttribute obj = (UIAttribute) pManagerUI.GetViewOfName(a.mName);
                if (obj != null)
                    obj.SetAttribute(a.mAttr, a.mValue);
            }
        }
        return convertView;
    }
}
