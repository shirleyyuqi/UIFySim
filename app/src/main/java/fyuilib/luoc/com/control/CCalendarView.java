package fyuilib.luoc.com.control;

import android.content.Context;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.Paint;
import android.util.DisplayMetrics;
import android.view.MotionEvent;
import android.view.View;

import java.util.ArrayList;
import java.util.Calendar;
import java.util.List;

import fyuilib.luoc.com.core.UIManager;
import fyuilib.luoc.com.util.TimeUtils;

/**
 * Created by luoc on 2018/5/25.
 */

public class CCalendarView extends View {
    private CCalendarUI     m_pCalendarUI = null;
    // 画笔
    private Paint paint;

    // 列数
    private static final int NUMS_COLUMN = 7;
    // 行数（星期一行加日期六行）
    private static final int NUMS_ROW = 7;

    // 周日到周六的颜色
    private int mWeekColor = Color.parseColor("#8B8B8B");
    // 本月日期的颜色
    private int mMonthDateColor = Color.parseColor("#000000");
    // 非本月日期的颜色
    private int mOtherDateColor = Color.parseColor("#AEAEAE");
    // 可选日期的背景颜色
    private int mSelectableDayColor = Color.parseColor("#9EB7B5");
    // 选中日期的背景颜色
    private int mSelectedDayColor = Color.parseColor("#E40F57");

    // 星期字体大小
    private int mWeekSize = 18;
    // 日期字体大小
    private int mDateSize = 15;
    // 可选、选择日期的圆圈半径
    private float mCircleR;

    // 当前年
    private int mCurrentYear;
    // 当前月
    private int mCurrentMonth;
    // 可选的日期（排班日期）
    private List<Integer> mSelectableDates = new ArrayList<Integer>();
    // 已选日期（已预约日期）
    private List<Integer> mSelectedDates = new ArrayList<Integer>();
    // 本月日期-在绘图时将数据储存在此，点击日历时做出判断
    // 7行7列（第一行没有数据，为了计算位置方便，将星期那一行考虑进去）
    private int[][] days = new int[NUMS_ROW][NUMS_COLUMN];

    // 列宽
    private int mColumnWidth;
    // 行高
    private int mRowHeight;

    // DisplayMetrics对象
    private DisplayMetrics displayMetrics;

    /**
     * 构造函数
     *
     * @param context
     * @description 初始化
     */
    public CCalendarView(Context context, CCalendarUI pCalendarUI) {
        super(context);
        m_pCalendarUI = pCalendarUI;

        // 获取DisplayMetrics实例
        displayMetrics = getResources().getDisplayMetrics();
        // 获取日历实例
        Calendar calendar = Calendar.getInstance();
        // new一个Paint实例(抗锯齿)
        paint = new Paint(Paint.ANTI_ALIAS_FLAG);
        // 获取当前年份
        mCurrentYear = calendar.get(Calendar.YEAR);
        // 获取当前月份
        mCurrentMonth = calendar.get(Calendar.MONTH);

        // 假数据，测试用，TODO:记得删除
        mSelectableDates.add(7);
        mSelectableDates.add(8);
        mSelectedDates.add(8);
    }

    @Override
    protected void onMeasure(int widthMeasureSpec, int heightMeasureSpec) {
        int widthSize = MeasureSpec.getSize(widthMeasureSpec);
        int widthMode = MeasureSpec.getMode(widthMeasureSpec);
        if (widthMode == MeasureSpec.AT_MOST) {
            widthSize = displayMetrics.densityDpi * 100;
        }

        int heightSize = MeasureSpec.getSize(widthMeasureSpec);
        int heightMode = MeasureSpec.getMode(widthMeasureSpec);
        if (heightMode == MeasureSpec.AT_MOST) {
            heightSize = displayMetrics.densityDpi * 120;
        }

        setMeasuredDimension(widthSize, heightSize);
    }

    @Override
    protected void onDraw(Canvas canvas) {
        // 计算每一列宽度
        mColumnWidth = getWidth() / NUMS_COLUMN;
        // 计算每一行高度
        mRowHeight = getHeight() / NUMS_ROW;
        // 绘制星期
        drawDayOfWeekText(canvas);
        // 绘制本月日期
        drawDateText(canvas);
        // 绘制前一个月的日期
        drawDateTextOfLastMonth(canvas);
        // 绘制下一个月的日期
        drawDateTextOfNextMonth(canvas);
    }

    private int downX = 0, downY = 0, upX = 0, upY = 0;
    @Override
    public boolean onTouchEvent(MotionEvent event) {
        switch (event.getAction()) {
            // 若是按下，则获取坐标
            case MotionEvent.ACTION_DOWN:
                downX = (int) event.getX();
                downY = (int) event.getY();
                break;

            // 若是点击后放开
            case MotionEvent.ACTION_UP:
                upX = (int) event.getX();
                upY = (int) event.getY();
                if(Math.abs(downX - upX) < 10 && Math.abs(downY - upY) < 10) {
                    // （点击事件）因为这里返回true，导致事件不会往上传，因此“手动”上传
                    performClick();
                    // 处理点击事件
                    handleClick((upX + downX) / 2, (upY + downY) / 2);
                }
                break;

            default:
                break;

        }
        // 返回true表示已经消费此事件，不上传了（这样才能监听所有动作，而不是只有ACTION_DOWN）
        return true;
    }

    @Override
    public boolean performClick() {
        return super.performClick();
    }

    /**
     * 处理点击事件
     * @param x
     * @param y
     */
    private void handleClick(int x, int y) {
        // 获取行
        int row = y / mRowHeight;
        // 获取列
        int column = x / mColumnWidth;
        // 获取点击的日期
        int clickedDay = days[row][column];
        if (row > 1)
            m_pCalendarUI.Clicked(column*mColumnWidth, row*mRowHeight, mColumnWidth, mRowHeight, mCurrentYear, mCurrentMonth, clickedDay);
        //onDateClick.onClick(mCurrentYear, mCurrentMonth, clickedDay);
    }

    /**
     * 绘制星期
     *
     * @param canvas
     */
    private void drawDayOfWeekText(Canvas canvas) {
        for (int dayOfWeek = 0; dayOfWeek < 7; dayOfWeek++) {
            // 星期在第0行
            int row = 0;
            // 列数
            int column = dayOfWeek;
            // 填写星期
            String day[] = {"日", "一", "二", "三", "四", "五", "六"};
            // 设置字体大小
            paint.setTextSize(mWeekSize * displayMetrics.scaledDensity);
            // 设置画笔颜色
            paint.setColor(mWeekColor);
            // 左边坐标（居中显示）
            int left = (int) (mColumnWidth * column + (mColumnWidth - paint.getTextSize()) / 2);
            // 顶部坐标 (注意，竖直方向上是以baseline为基准写字的，因此要 - (paint.ascent() + paint.descent()) / 2)
            int top = (int) (mRowHeight * row + mRowHeight / 2 - (paint.ascent() + paint.descent()) / 2);
            // 绘制文字
            canvas.drawText(day[dayOfWeek] + "", left, top, paint);
        }
    }


    /**
     * 绘制本月日期(若有背景，需要先绘制背景，否则会覆盖文字)
     *
     * @param canvas
     */
    private void drawDateText(Canvas canvas) {
        // 获取日历实例
        Calendar calendar = Calendar.getInstance();
        // 获取当前月份天数
        int daysOfMonth = calendar.getActualMaximum(Calendar.DATE);
        // 获取当月第一天是一周中的第几天（周日为第一天）
        int firstDayOfWeekInMonth = TimeUtils.getFirstDayOfWeekInMonth(mCurrentYear, mCurrentMonth);
        // 写入文字
        for (int date = 1; date <= daysOfMonth; date++) {
            // 当前日期所在行数（第一行为1：由于第一行显示星期，故日期行数加一）
            int row = (date + firstDayOfWeekInMonth - 1 - 1) / 7 + 1;
            // 当前日期所在列数(第一列为0)
            int column = (date + firstDayOfWeekInMonth - 1 - 1) % 7;
            if (firstDayOfWeekInMonth == 1) {
                // 如果第一天是周日，那么，根据日历习惯，日期行数应该加一
                row = row + 1;
            }
            // 储存日期信息
            days[row][column] = date;

            // 若是可选日期，绘制背景
            for (Integer day : mSelectableDates) {
                if (day.intValue() == date) {
                    drawSelectableBackground(canvas, row, column);
                }
            }
            // 若是已选择日期，绘制背景
            for (Integer day : mSelectedDates) {
                if (day.intValue() == date) {
                    drawSelectedBackground(canvas, row, column);
                }
            }

            // 设置字体大小
            paint.setTextSize(mDateSize * displayMetrics.scaledDensity);
            // 设置画笔颜色
            paint.setColor(mMonthDateColor);
            // 日期左边坐标（居中显示）
            int left = (int) (mColumnWidth * column + (mColumnWidth - paint.measureText(date + "")) / 2);
            // 日期顶部坐标 (注意，竖直方向上是以baseline为基准写字的，因此要 - (paint.ascent() + paint.descent()) / 2)
            int top = (int) (mRowHeight * row + mRowHeight / 2 - (paint.ascent() + paint.descent()) / 2);
            // 绘制文字
            canvas.drawText(date + "", left, top, paint);
        }
    }

    /**
     * 绘制可选日期背景
     *
     * @param canvas
     * @param row
     * @param column
     */
    private void drawSelectableBackground(Canvas canvas, int row, int column) {
        // 画笔颜色
        paint.setColor(mSelectableDayColor);
        // 圆心位置
        float cX = (float) (mColumnWidth * column + mColumnWidth / 2);
        float cY = (float) (mRowHeight * row + mRowHeight / 2);
        // 圆形半径
        mCircleR = (float) (mColumnWidth / 2 * 0.8);
        // 绘制圆形背景
        canvas.drawCircle(cX, cY, mCircleR, paint);
    }

    /**
     * 绘制已选日期背景
     *
     * @param canvas
     * @param row
     * @param column
     */
    private void drawSelectedBackground(Canvas canvas, int row, int column) {
        // 画笔颜色
        paint.setColor(mSelectedDayColor);
        // 圆心位置
        float cX = (float) (mColumnWidth * column + mColumnWidth / 2);
        float cY = (float) (mRowHeight * row + mRowHeight / 2);
        // 圆形半径
        mCircleR = (float) (mColumnWidth / 2 * 0.8);
        // 绘制圆形背景
        canvas.drawCircle(cX, cY, mCircleR, paint);
    }


    /**
     * 绘制前一个月的日期
     *
     * @param canvas
     */
    private void drawDateTextOfLastMonth(Canvas canvas) {
        // 获取上一个月的最后一天是一周内的第几天
        int lastDayOfWeek = TimeUtils.getLastDayOfWeekInLastMonth(mCurrentYear, mCurrentMonth);
        // 获取上一个月的最后一天
        int lastDay = TimeUtils.getLastDayOfLastMonth(mCurrentYear, mCurrentMonth);
        // 写入文字
        for (int column = lastDayOfWeek - 1, i = 0; column >= 0; column--, i++) {
            // 设置字体大小
            paint.setTextSize(mDateSize * displayMetrics.scaledDensity);
            // 设置画笔颜色
            paint.setColor(mOtherDateColor);
            // 日期左边坐标
            int left = (int) (mColumnWidth * column + (mColumnWidth - paint.measureText((lastDay - i) + "")) / 2);
            // 日期顶部坐标 (所在行数为第二行)
            int top = (int) (mRowHeight * 1 + mRowHeight / 2 - (paint.ascent() + paint.descent()) / 2);
            // 绘制文字
            canvas.drawText((lastDay - i) + "", left, top, paint);
        }
    }

    /**
     * 绘制下一个月的日期
     *
     * @param canvas
     */
    private void drawDateTextOfNextMonth(Canvas canvas) {
        // 获取下一个月的第一天是一周内的第几天
        int firstDayOfWeekInNextMonth = TimeUtils.getFirstDayOfWeekInNextMonth(mCurrentYear, mCurrentMonth);
        // 获取本月的第一天是一周内的第几天
        int firstDayOfWeek = TimeUtils.getFirstDayOfWeekInMonth(mCurrentYear, mCurrentMonth);
        // 下个月的第一天
        int firstDay = 1;
        // 所在行数
        int row;
        // 所在列数
        int column = firstDayOfWeekInNextMonth - 1;

        // 如果本月第一天是周五、周六或者周日，下个月从最后一行开始绘制（第6行）,否则，从倒数第二行开始绘制（第5行）
        if (firstDayOfWeek == 6 || firstDayOfWeek == 7 || firstDayOfWeek == 1) {
            row = 6;
        } else {
            row = 5;
        }

        // 写入文字
        for (; row <= 6; row++) {
            for (; column <= 6; column++) {
                // 设置字体大小
                paint.setTextSize(mDateSize * displayMetrics.scaledDensity);
                // 设置画笔颜色
                paint.setColor(mOtherDateColor);
                // 日期左边坐标
                int left = (int) (mColumnWidth * column + (mColumnWidth - paint.measureText(firstDay + "")) / 2);
                // 日期顶部坐标 (所在行数为第七行即最后一行)
                int top = (int) (mRowHeight * row + mRowHeight / 2 - (paint.ascent() + paint.descent()) / 2);
                // 绘制文字
                canvas.drawText(firstDay + "", left, top, paint);
                // 日期加一
                firstDay += 1;
            }
            // 若还有一行要绘制，则column置为0
            column = 0;
        }
    }

    public int getmWeekColor() {
        return mWeekColor;
    }

    public void setmWeekColor(int mWeekColor) {
        this.mWeekColor = mWeekColor;
    }

    public int getmMonthDateColor() {
        return mMonthDateColor;
    }

    public void setmMonthDateColor(int mMonthDateColor) {
        this.mMonthDateColor = mMonthDateColor;
    }

    public int getmOtherDateColor() {
        return mOtherDateColor;
    }

    public void setmOtherDateColor(int mOtherDateColor) {
        this.mOtherDateColor = mOtherDateColor;
    }

    public int getmSelectableDayColor() {
        return mSelectableDayColor;
    }

    public void setmSelectableDayColor(int mSelectableDayColor) {
        this.mSelectableDayColor = mSelectableDayColor;
    }

    public int getmSelectedDayColor() {
        return mSelectedDayColor;
    }

    public void setmSelectedDayColor(int mSelectedDayColor) {
        this.mSelectedDayColor = mSelectedDayColor;
    }

    public int getmWeekSize() {
        return mWeekSize;
    }

    public void setmWeekSize(int mWeekSize) {
        this.mWeekSize = mWeekSize;
    }

    public int getmDateSize() {
        return mDateSize;
    }

    public void setmDateSize(int mDateSize) {
        this.mDateSize = mDateSize;
    }

    public float getmCircleR() {
        return mCircleR;
    }

    public void setmCircleR(float mCircleR) {
        this.mCircleR = mCircleR;
    }

    public List<Integer> getmSelectableDates() {
        return mSelectableDates;
    }

    public void setmSelectableDates(List<Integer> mSelectableDates) {
        this.mSelectableDates.clear();
        this.mSelectableDates = mSelectableDates;
    }

    public List<Integer> getmSelectedDates() {
        return mSelectedDates;
    }

    public void setmSelectedDates(List<Integer> mSelectedDates) {
        this.mSelectedDates = mSelectedDates;
    }
}
