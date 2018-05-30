package fyuilib.luoc.com.control;

import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.Canvas;
import android.view.View;
import android.view.ViewGroup;
import android.widget.ImageView;

import fyuilib.luoc.com.core.UIManager;

/**
 * Created by luoc on 2018/5/5.
 */

public class CImageUI extends ImageView implements UIAttribute{
    public CCommonFunUI m_pCommonUI = null;
    public boolean      m_bUsrBitmap = false;
    public Bitmap       m_bitMap = null;

    public CImageUI(Context context, UIManager manager) {
        super(context);

        m_pCommonUI = new CCommonFunUI();
        m_pCommonUI.initWithDelegateView(this, manager);
    }

    @Override
    public void SetAttribute(String name, String value) {
        if (name.compareTo("src") == 0) {
            this.setImageResource(UIManager.getImage(value));
        }else if(name.compareTo("mode") == 0){
            switch (Integer.parseInt(value)) {
                case 0:
                    this.setScaleType(ScaleType.CENTER);
                    break;
                case 1:
                    this.setScaleType(ScaleType.FIT_CENTER);
                    break;
            }
        }else{
            m_pCommonUI.setAttribute(name, value);
        }
    }

    @Override
    protected void onDraw(Canvas canvas) {
        if (!m_bUsrBitmap){
            super.onDraw(canvas);
        }else if (m_bitMap != null){
            canvas.drawBitmap(m_bitMap, 0, 0, null);
        }
    }

    public void CreateBitmap(int type, int w, int h){
        if (type == 0){
            m_bitMap = Bitmap.createBitmap(w, h, Bitmap.Config.ARGB_8888);
        }else{
            m_bitMap = Bitmap.createBitmap(w, h, Bitmap.Config.RGB_565);
        }
        m_bUsrBitmap = true;
    }

    public Bitmap getBitmap(int type, int w, int h){
        if (m_bitMap == null){
            CreateBitmap(type, w, h);
        }
        return m_bitMap;
    }

    @Override
    public CCommonFunUI GetCommFun() {
        return m_pCommonUI;
    }
}
