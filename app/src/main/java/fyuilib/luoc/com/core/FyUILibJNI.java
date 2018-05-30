package fyuilib.luoc.com.core;

import android.content.res.AssetManager;

/**
 * Created by luoc on 2018/5/5.
 */

public class FyUILibJNI {
    static {
        System.loadLibrary("native-lib");
    }

    public native static String stringFromJNI();
    public native static long  LaodXml(AssetManager aas, String file);
    public native static void  ReleaseXml(long xmlHandle);
    public native static long  GetRoot(long xmlHandle);

    public native static void  NodeRelease(long node);
    public native static long  NodeGetParent(long node);
    public native static long  NodeGetSibling(long node);
    public native static long  NodeGetChild(long node);
    public native static long  NodeGetChild2(long node, String strName);

    public native static boolean  NodeIsValid(long node);
    public native static String  NodeGetName(long node);

    public native static boolean  NodeHasAttributes(long node);
    public native static String  NodeGetAttributeValueWithName(long node, String name);
    public native static boolean  NodeHasChildren(long node);
    public native static int  NodeGetAttributeCount(long node);
    public native static String  NodeGetAttributeName(long node, int index);
    public native static String  NodeGetAttributeValueWithIndex(long node, int index);
}
