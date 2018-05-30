package fyuilib.luoc.com.activity.addbell;


import android.Manifest;
import android.app.Activity;
import android.content.Intent;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.os.AsyncTask;
import android.os.Bundle;

import android.os.Vibrator;
import android.text.TextUtils;
import android.util.Log;
import android.view.View;
import android.view.ViewGroup;
import android.view.Window;
import android.widget.Toast;

import java.nio.ByteBuffer;

import cn.bingoogolapple.photopicker.activity.BGAPhotoPickerActivity;
import cn.bingoogolapple.qrcode.core.QRCodeView;
import fyuilib.luoc.com.activity.R;
import fyuilib.luoc.com.activity.util.Person;
import fyuilib.luoc.com.control.CButtonUI;
import fyuilib.luoc.com.control.UIAttribute;
import fyuilib.luoc.com.control.UIManagerDelegate;
import fyuilib.luoc.com.core.UIDlgBuild;
import fyuilib.luoc.com.core.UIManager;
import fyuilib.luoc.com.util.FyUIActivity;

import cn.bingoogolapple.qrcode.zbar.ZBarView;
//import cn.bingoogolapple.qrcode.zxing.QRCodeDecoder;
//import cn.bingoogolapple.qrcode.zxing.ZXingView;

/**
 * Created by luoc on 2018/5/26.
 */


public class QRScanActivity extends FyUIActivity implements UIManagerDelegate, QRCodeView.Delegate {
    private static final int REQUEST_CODE_CHOOSE_QRCODE_FROM_GALLERY = 666;

    private CButtonUI       m_pBackBt;
    private CButtonUI       m_pAddBt;
    private CButtonUI       m_pPhotoBt;
    public static Person    s_mandAdd = new Person();
    public static Person    s_audoAdd = new Person();

    private QRCodeView      mQRCodeView;

    @Override
    public String getUIXml(){
        return "scanConn.xml";
    }

    @Override
    public void ContentViewInit(){
        setContentView(R.layout.activity_scan);
        //setContentView(R.layout.activity_scan_zxing);
    }

    @Override
    public void InitControl(){
        m_pBackBt = (CButtonUI)m_pManagerUI.GetViewOfName("back");
        m_pAddBt = (CButtonUI)m_pManagerUI.GetViewOfName("add");
        m_pPhotoBt = (CButtonUI)m_pManagerUI.GetViewOfName("photo");

        mQRCodeView = (ZBarView) findViewById(R.id.zbarview);
        //mQRCodeView = (ZXingView) findViewById(R.id.zxingview);

        mQRCodeView.setDelegate(this);
        //mQRCodeView.changeToScanQRCodeStyle();
        //mQRCodeView.setOnlyDecodeScanBoxArea(false);
        //mQRCodeView.setOnlyDecodeScanBoxArea()
    }

    @Override
    public boolean OnNotify(UIAttribute pSender, int event, Object param) {
        if (event == UIManagerDelegate.FYEVENT_TOUCHUPINSIDE) {
            if (pSender == m_pBackBt) {
                finish();
            } else if (pSender == m_pAddBt) {
                s_mandAdd.clickCall.onClick(QRScanActivity.this, null);
            } else if (pSender == m_pPhotoBt){

            }
        }
        return false;
    }

    @Override
    protected void onStart() {
        super.onStart();
        mQRCodeView.startCamera();
//        mQRCodeView.startCamera(Camera.CameraInfo.CAMERA_FACING_FRONT);

        mQRCodeView.startSpot();

        Log.e("aaaa", "" + mQRCodeView.getIsScanBarcodeStyle());
        //mQRCodeView.hiddenScanRect();

        //mQRCodeView.showScanRect();
    }

    @Override
    protected void onStop() {
        mQRCodeView.stopCamera();
        super.onStop();
    }

    @Override
    protected void onDestroy() {
        mQRCodeView.onDestroy();
        super.onDestroy();
    }

    @Override
    public void onScanQRCodeSuccess(String result) {
        Log.i("aaa", "result:" + result);
        Toast.makeText(this, result, Toast.LENGTH_SHORT).show();
        vibrate();
        mQRCodeView.startSpot();
    }

    private void vibrate() {
        Vibrator vibrator = (Vibrator) getSystemService(VIBRATOR_SERVICE);
        vibrator.vibrate(200);
    }

    @Override
    public void onScanQRCodeOpenCameraError() {
        Log.e("errr", "打开相机出错");
    }

/*
    @Override
    protected void onActivityResult(int requestCode, int resultCode, Intent data) {
        super.onActivityResult(requestCode, resultCode, data);

        // 识别图片中的二维码还有问题，占时不要用
        if (resultCode == Activity.RESULT_OK && requestCode == REQUEST_CODE_CHOOSE_QRCODE_FROM_GALLERY) {
            final String picturePath = BGAPhotoPickerActivity.getSelectedPhotos(data).get(0);


            //这里为了偷懒，就没有处理匿名 AsyncTask 内部类导致 Activity 泄漏的问题
            //请开发在使用时自行处理匿名内部类导致Activity内存泄漏的问题，处理方式可参考 https://github.com/GeniusVJR/LearningNotes/blob/master/Part1/Android/Android%E5%86%85%E5%AD%98%E6%B3%84%E6%BC%8F%E6%80%BB%E7%BB%93.md

            new AsyncTask<Void, Void, String>() {
                @Override
                protected String doInBackground(Void... params) {
                    Bitmap bitmap = getDecodeAbleBitmap(picturePath);
                    int picw = bitmap.getWidth();
                    int pich = bitmap.getHeight();
                    int[] pix = new int[picw * pich];
                    byte[] pixytes = new byte[picw * pich];
                    bitmap.getPixels(pix, 0, picw, 0, 0, picw, pich);
                    int R, G, B, Y;

                    for (int y = 0; y < pich; y++) {
                        for (int x = 0; x < picw; x++) {
                            int index = y * picw + x;
                            R = (pix[index] >> 16) & 0xff;     //bitwise shifting
                            G = (pix[index] >> 8) & 0xff;
                            B = pix[index] & 0xff;

                            //R,G.B - Red, Green, Blue
                            //to restore the values after RGB modification, use
                            //next statement
                            pixytes[index] = (byte) (0xff000000 | (R << 16) | (G << 8) | B);
                        }
                    }
                    ByteBuffer buffer = ByteBuffer.allocate(bitmap.getByteCount());
                    byte[] data = new byte[(int) (bitmap.getHeight() * bitmap.getWidth() * 1.5)];
                    rgba2Yuv420(pixytes, data, bitmap.getWidth(), bitmap.getHeight());
                    return mQRCodeView.processData(data, bitmap.getWidth(), bitmap.getHeight(), true);
                }

                @Override
                protected void onPostExecute(String result) {
                    if (TextUtils.isEmpty(result)) {
                        Toast.makeText(QRScanActivity.this, "未发现二维码", Toast.LENGTH_SHORT).show();
                    } else {
                        Toast.makeText(QRScanActivity.this, result, Toast.LENGTH_SHORT).show();
                    }
                }
            }.execute();
        }
    }*/


    /**
     * 将本地图片文件转换成可解码二维码的 Bitmap。为了避免图片太大，这里对图片进行了压缩。感谢 https://github.com/devilsen 提的 PR
     *
     * @param picturePath 本地图片文件路径
     * @return
     */
    private static Bitmap getDecodeAbleBitmap(String picturePath) {
        try {
            BitmapFactory.Options options = new BitmapFactory.Options();
            options.inJustDecodeBounds = true;
            BitmapFactory.decodeFile(picturePath, options);
            int sampleSize = options.outHeight / 400;
            if (sampleSize <= 0) {
                sampleSize = 1;
            }
            options.inSampleSize = sampleSize;
            options.inJustDecodeBounds = false;

            return BitmapFactory.decodeFile(picturePath, options);
        } catch (Exception e) {
            return null;
        }
    }

    public static void rgba2Yuv420(byte[] src, byte[] dst, int width, int height) {
        // Y
        for (int y = 0; y < height; y++) {
            int dstOffset = y * width;
            int srcOffset = y * width * 4;
            for (int x = 0; x < width && dstOffset < dst.length && srcOffset < src.length; x++) {
                dst[dstOffset] = src[srcOffset];
                dstOffset += 1;
                srcOffset += 4;
            }
        }
        /* Cb and Cr */
        for (int y = 0; y < height / 2; y++) {
            int dstUOffset = y * width + width * height;
            int srcUOffset = y * width * 8 + 1;

            int dstVOffset = y * width + width * height + 1;
            int srcVOffset = y * width * 8 + 2;
            for (int x = 0; x < width / 2 && dstUOffset < dst.length && srcUOffset < src.length && dstVOffset < dst.length && srcVOffset < src.length; x++) {
                dst[dstUOffset] = src[srcUOffset];
                dst[dstVOffset] = src[srcVOffset];

                dstUOffset += 2;
                dstVOffset += 2;

                srcUOffset += 8;
                srcVOffset += 8;
            }
        }
    }
}
