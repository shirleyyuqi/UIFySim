package fyuilib.luoc.com.control;

import android.view.View;

/**
 * Created by luoc on 2018/5/19.
 */

public interface UIManagerDelegate {
    public static final int FYEVENT_INITVIEW                = 0x00000001;
    public static final int FYEVENT_TOUCHUPINSIDE           = 0x00000002;
    public static final int FYEVENT_TOUCHDOWN               = 0x00000003;
    public static final int FYEVENT_SW_SELECTED             = 0x00000004;
    public static final int FYEVENT_OP_SELECTED             = 0x00000005;
    public static final int FYEVENT_TB_SELECTED             = 0x00000006;
    public static final int FYEVENT_TX_RETURN               = 0x00000007;
    public static final int FYEVENT_TX_BEGIN                = 0x00000008;
    public static final int FYEVENT_LIST_SCROLLBEGIN        = 0x00000009;
    public static final int FYEVENT_SCROLLDRAGBEGIN         = 0x0000000a;
    public static final int FYEVENT_SCROLLDRAGEND           = 0x0000000b;
    public static final int FYEVENT_SCROLLWILLBEGINDECELERAT    = 0x0000000c;
    public static final int FYEVENT_SCROLLENDDECELERAT          = 0x0000000d;
    public static final int FYEVENT_TAP                         = 0x0000000e;
    public static final int FYEVENT_MBSLIDER_SLIDER             = 0x0000000f;
    public static final int FYEVENT_CALENDAR_DAY_SELECT         = 0x00000010;


    boolean OnNotify(UIAttribute pSender, int event, Object param);
}
