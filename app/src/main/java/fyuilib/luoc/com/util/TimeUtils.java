package fyuilib.luoc.com.util;

import java.util.Calendar;

/**
 * Created by luoc on 2018/5/25.
 */

public class TimeUtils {
    static Calendar calendar = Calendar.getInstance();

    /**
     * 获取指定年份、月份的第一天是一周中的第几天（周日为第一天）
     * @param year 年
     * @param month 月
     * @return year年-month月的第一天是一周中的第几天（周日为第一天）
     */
    public static int getFirstDayOfWeekInMonth(int year, int month) {
        // 设置当前日期为：year年-month月-1号
        calendar.set(year, month, 1);
        return calendar.get(Calendar.DAY_OF_WEEK);
    }

    /**
     * 获取指定年份、月份的上一月份的最后一天
     * @param year 年
     * @param month 月
     * @return  year年-month月的上一月的最后一天
     */
    public static int getLastDayOfLastMonth(int year, int month) {
        calendar.set(year, month, 1);
        // 向前滚动一个月
        calendar.add(Calendar.MONTH, -1);
        // 返回最后一天
        return calendar.getActualMaximum(Calendar.DATE);
    }

    /**
     *  获取指定年份、月份的上个月的最后一天是一周中的第几天
     *
     * @param year
     * @param month
     * @return  year年-month月的上一月的最后一天是一周中的第几天（周日为第一天）
     */
    public static int getLastDayOfWeekInLastMonth(int year, int month) {
        calendar.set(year, month, 1);
        // 向前滚动一天，即到前一月最后一天
        calendar.add(Calendar.DATE, -1);
        // 返回最后一天是一周的第几天
        return calendar.get(Calendar.DAY_OF_WEEK);
    }

    /**
     *  获取指定年份、月份的下个月的第一天是一周中的第几天
     *
     * @param year
     * @param month
     * @return  year年-month月的上一月的第一天是一周中的第几天（周日为第一天）
     */
    public static int getFirstDayOfWeekInNextMonth(int year, int month) {
        calendar.set(year, month, 1);
        // 向后滚动一天，即到后一月
        calendar.add(Calendar.MONTH, 1);
        // 返回第一天是一周的第几天
        return calendar.get(Calendar.DAY_OF_WEEK);
    }
}