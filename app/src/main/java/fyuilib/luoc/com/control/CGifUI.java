package fyuilib.luoc.com.control;

import android.annotation.SuppressLint;
import android.app.Activity;
import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.Canvas;
import android.graphics.Movie;
import android.os.SystemClock;
import android.view.View;
import android.widget.ImageView;

import java.io.InputStream;

import fyuilib.luoc.com.core.UIManager;

/**
 * Created by luoc on 2018/5/5.
 */

public class CGifUI extends View implements UIAttribute{
    private static final int DEFAULT_MOVIE_DURATION = 1000;
    public CCommonFunUI m_pCommonUI = null;

    private Movie mMovie;
    private long mStartTime;
    private long mPauseTime;
    private boolean mIsLoop = true;
    private boolean mIsStart;

    private long    mMovieStart;

    private int mLeft;
    private int mTop;
    private float mScaleX;
    private float mScaleY;
    private ImageView.ScaleType mScaleType = ImageView.ScaleType.CENTER_INSIDE;
    private Runnable mCalcRunnable = new Runnable() {
        @Override
        public void run() {
            calcScale();
            calcLocation();
        }
    };

    public CGifUI(Context context, UIManager manager) {
        super(context);

        m_pCommonUI = new CCommonFunUI();
        m_pCommonUI.initWithDelegateView(this, manager);
    }

    @Override
    protected void onMeasure(int widthMeasureSpec, int heightMeasureSpec) {
        int width = 0;
        int height = 0;
        if (mMovie != null) {
            int wMode = MeasureSpec.getMode(widthMeasureSpec);
            int hMode = MeasureSpec.getMode(heightMeasureSpec);
            int wSize = MeasureSpec.getSize(widthMeasureSpec);
            int hSize = MeasureSpec.getSize(heightMeasureSpec);

            if (wMode == MeasureSpec.EXACTLY) {
                width = wSize;
            } else {
                width = mMovie.width();
            }

            if (hMode == MeasureSpec.EXACTLY) {
                height = hSize;
            } else {
                height = mMovie.height();
            }

        }
        setMeasuredDimension(width, height);
    }

    @SuppressLint("WrongConstant")
    @Override
    protected void onDraw(Canvas canvas) {
        super.onDraw(canvas);
        if (mMovie == null) {
            return;
        }

        long now = SystemClock.uptimeMillis();
        int currentTime = (int) (now - mStartTime);

        if (currentTime >= mMovie.duration()) {
            if (mIsLoop) {
                mStartTime = SystemClock.uptimeMillis();
                currentTime = updateAnimationTime();
                mIsStart = true;
            } else if (mIsStart) {
                currentTime = mMovie.duration();
                mIsStart = false;
            }
        }

        mMovie.setTime(currentTime);
        canvas.save(Canvas.MATRIX_SAVE_FLAG);
        canvas.scale(mScaleX, mScaleY);
        mMovie.draw(canvas, mLeft / mScaleX, mTop / mScaleY);
        canvas.restore();
        if (mIsStart) {
            postInvalidate();
        }
    }

    private int updateAnimationTime() {
        long now = android.os.SystemClock.uptimeMillis();
        // 如果第一帧，记录起始时间
        if (mMovieStart == 0) {
            mMovieStart = now;
        }
        // 取出动画的时长
        int dur = mMovie.duration();
        if (dur == 0) {
            dur = DEFAULT_MOVIE_DURATION;
        }
        // 算出需要显示第几帧
        return (int) ((now - mMovieStart) % dur);
    }

    public void pause() {
        if (!mIsStart) {
            return;
        }
        mIsStart = false;
        // 记录播放的位置
        mPauseTime = SystemClock.uptimeMillis() - mStartTime;
        postInvalidate();
    }


    public void resume() {
        if (mIsStart) {
            return;
        }
        mIsStart = true;
        // 恢复到播放的相对位置
        mStartTime = SystemClock.uptimeMillis() - mPauseTime;
        postInvalidate();
    }


    /**
     * 获取当前播放的帧
     *
     * @return
     */
    public Bitmap getCurrentFrame() {
        if (mMovie == null) {
            return null;
        }
        Bitmap bitmap = Bitmap.createBitmap(mMovie.width(), mMovie.height(), Bitmap.Config.RGB_565);

        Canvas canvas = new Canvas(bitmap);

        canvas.scale(mScaleX, mScaleY);

        mMovie.draw(canvas, mLeft, mTop);

        return bitmap;

    }


    public Movie getMovie() {
        return mMovie;
    }


    public void setMovie(Movie movie) {
        mMovie = movie;
        mStartTime = SystemClock.uptimeMillis();
        mIsStart = true;
        if (getMeasuredHeight() == 0 || getMeasuredWidth() == 0) {
            post(mCalcRunnable);
        } else {
            mCalcRunnable.run();
        }
        requestLayout();
        postInvalidate();
    }

    public void setSource(int id) {
        setSource(getResources().openRawResource(id));
    }

    public void setSource(byte[] bytes, int start, int len) {
        setMovie(Movie.decodeByteArray(bytes, start, len));
    }

    public void setSource(InputStream inputStream) {
        setMovie(Movie.decodeStream(inputStream));
    }

    public void setSource(String pathName) {
        setMovie(Movie.decodeFile(pathName));
    }

    public void setLoop(boolean loop) {
        mIsLoop = loop;
        postInvalidate();
    }

    public void setScaleType(ImageView.ScaleType scaleType) {
        if (scaleType == ImageView.ScaleType.MATRIX) {
            throw new UnsupportedOperationException("不支持MATRIX类型缩放");
        }
        this.mScaleType = scaleType;
        if (getMeasuredHeight() == 0 || getMeasuredWidth() == 0) {
            post(mCalcRunnable);
        } else {
            mCalcRunnable.run();
        }
    }

    private void calcScale() {
        if (mMovie == null) {
            return;
        }
        float imageW = mMovie.width();
        float imageH = mMovie.height();
        float viewW = getWidth();
        float viewH = getHeight();

        if (mScaleType == ImageView.ScaleType.FIT_XY) {
            mScaleX = viewW / imageW;
            mScaleY = viewH / imageH;
        } else if (mScaleType == ImageView.ScaleType.FIT_START
                || mScaleType == ImageView.ScaleType.FIT_CENTER
                || mScaleType == ImageView.ScaleType.FIT_END) {
            mScaleY = mScaleX = viewH / imageH;
        } else if (mScaleType == ImageView.ScaleType.CENTER) {
            mScaleX = mScaleY = 1;
        } else if (mScaleType == ImageView.ScaleType.CENTER_CROP) {
            mScaleX = viewW / imageW;
            mScaleY = viewH / imageH;
            mScaleX = mScaleY = Math.max(mScaleX, mScaleY);
        } else if (mScaleType == ImageView.ScaleType.CENTER_INSIDE) {
            mScaleX = viewW / imageW;
            mScaleY = viewH / imageH;
            mScaleX = mScaleY = Math.min(mScaleX, mScaleY);
        }
    }

    private void calcLocation() {
        if (mMovie == null) {
            return;
        }
        int imageW = mMovie.width();
        int imageH = mMovie.height();
        int viewW = getWidth();
        int viewH = getHeight();
        int left = getPaddingLeft();
        int top = getPaddingTop();

        if (mScaleType == ImageView.ScaleType.FIT_XY) {
            mLeft = left;
            mTop = top;
        } else if (mScaleType == ImageView.ScaleType.FIT_START) {
            mLeft = left;
            mTop = top;
        } else if (mScaleType == ImageView.ScaleType.FIT_CENTER) {
            mLeft = (int) (left + (viewW - imageW * mScaleX) / 2);
            mTop = top;
        } else if (mScaleType == ImageView.ScaleType.FIT_END) {
            mLeft = (int) (left + viewW - imageW * mScaleX);
            mTop = top;
        } else if (mScaleType == ImageView.ScaleType.CENTER) {
            mLeft = -(imageW - viewW) / 2;
            mTop = -(imageH - viewH) / 2;
        } else if (mScaleType == ImageView.ScaleType.CENTER_CROP) {
            mLeft = (int) -(Math.abs(viewW - imageW * mScaleX) / 2);
            mTop = (int) -(Math.abs(viewH - imageH * mScaleY) / 2);
        } else if (mScaleType == ImageView.ScaleType.CENTER_INSIDE) {
            mLeft = (int) (left + (viewW - imageW * mScaleX) / 2);
            mTop = (int) (top + (viewH - imageH * mScaleY) / 2);
        }
    }



    @Override
    public void SetAttribute(String name, String value) {
        if (name.compareTo("src") == 0) {
           setSource(UIManager.getImage(value));
        }else{
            m_pCommonUI.setAttribute(name, value);
        }
    }

    @Override
    public CCommonFunUI GetCommFun() {
        return m_pCommonUI;
    }
}
