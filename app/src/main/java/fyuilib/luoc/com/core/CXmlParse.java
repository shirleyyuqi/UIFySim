package fyuilib.luoc.com.core;

import android.content.res.AssetManager;
import android.util.Xml;

import org.xmlpull.v1.XmlPullParser;

import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.util.HashMap;

/**
 * Created by luoc on 2018/5/20.
 */

public class CXmlParse {
    public static AssetManager  s_ass = null;
    private static CXmlParse s_pParse = null;
    private HashMap m_pArray = new HashMap();
    private CXmlParse(){}

    public static CXmlParse sharedXmlParse(String file){
        if (s_pParse == null){
            s_pParse = new CXmlParse();
            try {
                s_pParse.parseXml(file);
            }catch (Exception e){

            }
        }
        return s_pParse;
    }

    public static void releasXmlParse(){
        s_pParse = null;
    }

    public String getValue(String key){
        String v = (String)m_pArray.get(key);
        if (v != null) return v;
        return key;
    }

    public void parseXml(String file) throws Exception {
        if (file == null) return;

        XmlPullParser parser = Xml.newPullParser();
        InputStream is = s_ass.open(file);
        parser.setInput(is, "utf-8");

        int type = parser.getEventType();
        //无限判断文件类型进行读取
        while (type != XmlPullParser.END_DOCUMENT) {
            switch (type) {
                //开始标签
                case XmlPullParser.START_TAG:
                    m_pArray.put(parser.getName(), parser.getAttributeValue(null, "text"));
                    break;
                case XmlPullParser.END_TAG:
                    break;
            }

            type = parser.next();
        }

        is.close();
    }
}
