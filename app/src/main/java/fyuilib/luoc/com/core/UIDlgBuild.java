package fyuilib.luoc.com.core;

import android.app.Activity;
import android.content.res.AssetManager;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.*;

import java.util.IllegalFormatCodePointException;

import fyuilib.luoc.com.control.*;

/**
 * Created by luoc on 2018/5/5.
 */


public class UIDlgBuild {
    public long m_xmlHandle = 0;
    public static AssetManager  s_ass = null;

    public View Create(String xmlFile, UIManager manager, View parent, Activity activity){
        m_xmlHandle = FyUILibJNI.LaodXml(s_ass, xmlFile);
        if (m_xmlHandle == 0) return new View(activity);
        long root = FyUILibJNI.GetRoot(m_xmlHandle);
        View v = null;
        do {
            if( !FyUILibJNI.NodeIsValid(root) ) {
                break;
            }
            v = Parse(root, manager, parent, activity);
        }while( false );
        FyUILibJNI.NodeRelease(root);
        FyUILibJNI.ReleaseXml(m_xmlHandle);
        return v;
    }

    public View Parse( long root, UIManager manager, View parent, Activity activity)
    {
        View pReturn = null;
        long nodeT = 0;
        for( long node = FyUILibJNI.NodeGetChild(root); FyUILibJNI.NodeIsValid(node);
             nodeT = FyUILibJNI.NodeGetSibling(node), FyUILibJNI.NodeRelease(node), node = nodeT )
        {
            String name = FyUILibJNI.NodeGetName(node);
            if (name.compareTo("Image") == 0 || name.compareTo("Font") == 0
                    || name.compareTo("Default") == 0 || name.compareTo("javascript") == 0) {
                continue;
            }

            View pControl = null;
            if (name.compareTo("Include") == 0) {
                if ( FyUILibJNI.NodeHasAttributes(node) ) continue;
                //TODO
            } else {
                switch (name.length()) {
                    case 3:
                        if (name.compareTo("Gif") == 0)
                            pControl = new CGifUI(activity, manager);
                        break;
                    case 4:
                        if (name.compareTo("Edit") == 0)
                            pControl = new CEditUI(activity, manager);
                        else if (name.compareTo("List") == 0)
                            pControl = new CListUI(activity, manager);
                        else if (name.compareTo("Text") == 0)
                            pControl = new CTextUI(activity, manager);
                        else if (name.compareTo("Time") == 0)
                            pControl = new CTimeUI(activity, manager);
                        break;
                    case 5:
                        if (name.compareTo("Combo") == 0)
                            pControl = new CComboUI(activity, manager);
                        else if (name.compareTo("Label") == 0)
                            pControl = new CTextUI(activity, manager);//new CLabelUI(activity, manager);
                        else if (name.compareTo("List2") == 0)
                            pControl = new CListUI2(activity, manager);
                        break;
                    case 6:
                        if (name.compareTo("Button") == 0)
                            pControl = new CButtonUI(activity, manager);
                        else if (name.compareTo("Option") == 0)
                            pControl = new COptionUI(activity, manager);
                        else if (name.compareTo("Switch") == 0)
                            pControl = new CSwitchUI(activity, manager);
                        else if (name.compareTo("Slider") == 0)
                            pControl = new CSliderUI(activity, manager);
                        else if (name.compareTo("CNumUI") == 0)
                            pControl = new CNumberUI(activity, manager);
                        break;
                    case 7:
                        if (name.compareTo("Control") == 0)
                            pControl = new CControlUI(activity, manager);
                        break;
                    case 8:
                        if (name.compareTo("Progress") == 0)
                            pControl = new CProgressUI(activity, manager);
                        else if (name.compareTo("RichEdit") == 0)
                            pControl = new CRichEditUI(activity, manager);
                        else if (name.compareTo("DateTime") == 0)
                            pControl = new CDateUI(activity, manager);
                        else if (name.compareTo("MBSlider") == 0)
                            pControl = new CMBSliderUI(activity, manager);
                        else if (name.compareTo("calendar") == 0)
                            pControl = new CCalendarUI(activity, manager);
                        break;
                    case 9:
                        if (name.compareTo("Container") == 0)
                            pControl = new CContainerUI(activity, manager);
                        else if (name.compareTo("TabLayout") == 0)
                            pControl = new CTabLayoutUI(activity, manager);
                        else if (name.compareTo("ImageView") == 0)
                            pControl = new CImageUI(activity, manager);
                        break;
                    case 10:
                        if (name.compareTo("LayoutView") == 0)
                            pControl = new CLayoutUI(activity, manager);
                        else if (name.compareTo("ScrollView") == 0)
                            pControl = new CScrollViewUI(activity, manager);
                        break;
                    case 11:
                        if (name.compareTo("ListElement") == 0)
                            pControl = new CListElementUI(activity, manager);
                        else if(name.compareTo("SurfaceView") == 0)
                            pControl = new CSufaceView(activity, manager);
                        break;
                    case 12:
                        if (name.compareTo("ListElementf") == 0)
                            pControl = new CListElementUI(activity, manager);
                        break;
                    case 20:
                        break;
                }

                /*if( pControl == NULL && _pCreateBlcok != NULL ) {
                    pControl = _pCreateBlcok(pstrClass);
                }*/
            }

            if (pControl == null) {
                if (name.compareTo("JSScript") == 0) {

                } else if (name.compareTo("ListSection") == 0) {
                    Parse(node, manager, parent, activity);
                }
                continue;
            }

            if (FyUILibJNI.NodeHasChildren(node)) {
                Parse(node, manager, pControl, activity);
            }

            if (parent != null) {
                if (name.compareTo("ListElement") == 0 || name.compareTo("ListElementf") == 0) {
                    if (parent.getClass().toString().endsWith(".CListUI") ||
                            parent.getClass().toString().endsWith(".CListUI2")){
                        UIListAttribute o = (UIListAttribute)parent;
                        o.AddListElement((CListElementUI)pControl);
                    }
                } else {
                    String str = parent.getClass().toString();
                    if ( str.endsWith(".CTabLayoutUI") ) {
                        UITabAttribute o = (UITabAttribute)parent;
                        o.AddTabElement(pControl);
                    }
                    ViewGroup pView = (ViewGroup)parent;
                    pView.addView(pControl);
                }
            }

            if ( FyUILibJNI.NodeHasAttributes(node) ) {
                int nAttributes = FyUILibJNI.NodeGetAttributeCount(node);
                UIAttribute pTempUI = (UIAttribute)pControl;
                for( int i = 0; i < nAttributes; i++ ) {
                    Log.d("AAA", "Parse: " + name);
                    pTempUI.SetAttribute(FyUILibJNI.NodeGetAttributeName(node, i), FyUILibJNI.NodeGetAttributeValueWithIndex(node, i));
                }
            }

            if (pReturn == null)
                pReturn = pControl;
        }
        return pReturn;
    }
}
