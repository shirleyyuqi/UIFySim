package fyuilib.luoc.com.control;

import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.Canvas;
import android.view.SurfaceHolder;
import android.view.SurfaceView;

import fyuilib.luoc.com.core.UIManager;

/**
 * Created by luoc on 2018/5/27.
 */

public class CSufaceView extends SurfaceView implements UIAttribute, SurfaceHolder.Callback{
    private CCommonFunUI m_pCommonUI = null;
    private SurfaceHolder mHolder;
    private Canvas        mCanvas;

    public CSufaceView(Context context, UIManager manager) {
        super(context);

        m_pCommonUI = new CCommonFunUI();
        m_pCommonUI.initWithDelegateView(this, manager);
        initView();
    }

    private void initView() {
        mHolder = getHolder();
        mHolder.addCallback(this);
        setFocusable(true);
        setFocusableInTouchMode(true);
        this.setKeepScreenOn(true);
        //mHolder.setFormat(PixelFormat.OPAQUE);
    }

    public void DrawBitmap(Bitmap bitmap){
        try {
            mCanvas = mHolder.lockCanvas();
            mCanvas.drawBitmap(bitmap, 0, 0, null);
        } catch (Exception e) {
        } finally {
            if (mCanvas != null)
                mHolder.unlockCanvasAndPost(mCanvas);
        }
    }

    @Override
    public void surfaceCreated(SurfaceHolder holder) {

    }

    @Override
    public void surfaceChanged(SurfaceHolder holder, int format, int width, int height) {

    }

    @Override
    public void surfaceDestroyed(SurfaceHolder holder) {

    }

    @Override
    public void SetAttribute(String name, String value) {
        m_pCommonUI.setAttribute(name, value);
    }

    @Override
    public CCommonFunUI GetCommFun() {
        return m_pCommonUI;
    }
}
