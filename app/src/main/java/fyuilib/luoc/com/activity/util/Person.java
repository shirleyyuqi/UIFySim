package fyuilib.luoc.com.activity.util;

import android.app.Activity;
import android.content.Context;
import android.os.Parcel;
import android.os.Parcelable;

import java.io.Serializable;

/**
 * Created by luoc on 2018/5/26.
 */

public class Person {
    public interface OnClickCall{
        boolean onClick(Context packageContext, Object obj);
    }
    public OnClickCall clickCall;

    public void SetClickCall(OnClickCall click){
        clickCall = click;
    }
}
