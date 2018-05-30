package fyuilib.luoc.com.control;

import android.content.Context;
import android.graphics.Color;
import android.view.View;
import android.view.ViewGroup;
import android.widget.AdapterView;
import android.widget.ArrayAdapter;
import android.widget.ListView;
import android.widget.TableLayout;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;

import fyuilib.luoc.com.core.UIManager;

/**
 * Created by luoc on 2018/5/5.
 */

public class CListUI extends ListView implements UIAttribute, UIListAttribute, AdapterView.OnItemClickListener {
    public CCommonFunUI     m_pCommonUI = null;
    private CListUIAdapter  m_pAdapter;
    private int             m_iItemHeight = 30;
    public CListUI(Context context, UIManager manager) {
        super(context);

        m_pCommonUI = new CCommonFunUI();
        m_pCommonUI.initWithDelegateView(this, manager);

        m_pAdapter = new CListUIAdapter(context, this);
        setAdapter(m_pAdapter);
        setOnItemClickListener(this);
    }

    @Override
    public void onItemClick(AdapterView<?> parent, View view, int position, long id){
        CListElementUI p = (CListElementUI)view;
        p.mIndex = position;
        m_pCommonUI.getUIManager().OnNotify(this, UIManagerDelegate.FYEVENT_TB_SELECTED, view);
    }

    @Override
    public void SetAttribute(String name, String value) {
        if (name.compareTo("itemheight") == 0) {
            m_iItemHeight = UIManager.dip2px(getContext(), Integer.parseInt(value));
        }else if(name.compareTo("scroll") == 0){
            if (value.compareTo("true") == 0) {
                setVerticalScrollBarEnabled(true);
            }else{
                setVerticalScrollBarEnabled(false);
            }
        }else if(name.compareTo("itemlinestyle") == 0){
            //self.separatorStyle = atoi(value) % 3;
        }else if(name.compareTo("itemselectstyle") == 0){
            //iCellSelectionStyle = atoi(value);
        }else if(name.compareTo("deselect") == 0){
            //deSelect = atoi(value);
        }else if(name.compareTo("itemxml") == 0){
            m_pAdapter.SetItemXml(value+".xml");
            //cellString = [NSString stringWithUTF8String:value];
        }else if(name.compareTo("itemaccessorytype") == 0){
            m_pAdapter.SetAccessoryType(Integer.parseInt(value));
            //iCellAccessoryType = atoi(value)%4;
        }else if(name.compareTo("headxml") == 0){
            /*pHeadManagerUI = [[UIManager alloc] init];
            UIDlgBuild* build = [[UIDlgBuild alloc] init];
            NSString *file = [[NSBundle mainBundle] pathForResource:[NSString stringWithUTF8String:value] ofType:@"xml"];
            self.tableHeaderView = [build Create:[file UTF8String] :^UIView *(const char *pstrClass) {
                return nil;
            } :pHeadManagerUI :self];*/
        }else if(name.compareTo("footxml") == 0){
            /*pFootManagerUI = [[UIManager alloc] init];
            UIDlgBuild* build = [[UIDlgBuild alloc] init];
            NSString *file = [[NSBundle mainBundle] pathForResource:[NSString stringWithUTF8String:value] ofType:@"xml"];
            self.tableFooterView = [build Create:[file UTF8String] :^UIView *(const char *pstrClass) {
                return nil;
            } :pFootManagerUI :self];*/
        }else if(name.compareTo("footheight") == 0){
            //fFootHeight = atof(value);
        }else if(name.compareTo("headheight") == 0){
            //fHeadHeight = atof(value);
        }else if(name.compareTo("secheight") == 0){
            /*CGSize size = [CCommonFunUI getSizeValue:value];
            secSize[[listSection count]-1]= size;*/
        }else{
            m_pCommonUI.setAttribute(name, value);
        }
    }

    public int getItemHeight(){
        return m_iItemHeight;
    }

    public int getItemWidth(){
        return getWidth();
    }

    public boolean addValue(String name, String attr, String value, int index){
        return m_pAdapter.addValue(name, attr, value, index);
    }

    public String GetValue(String name, String attr, int index){
        return m_pAdapter.GetValue(name, attr, index);
    }

    public void removeValue(int index){
        m_pAdapter.removeValue(index);
    }

    public void removeAll(){
        m_pAdapter.removeAll();
    }

    public void reloadData(){
        m_pAdapter.notifyDataSetChanged();
    }

    @Override
    public CCommonFunUI GetCommFun() {
        return m_pCommonUI;
    }

    @Override
    public void AddListElement(CListElementUI listElement) {
        m_pAdapter.addData(listElement);
    }
}
