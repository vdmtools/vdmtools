package jp.vdmtools.VDM.jdk;


import jp.vdmtools.VDM.CGException;
import jp.vdmtools.VDM.UTIL;

import java.util.Date;
import java.util.Calendar;
import java.util.GregorianCalendar;
import java.util.TimeZone;
import java.text.DateFormat;
import java.text.SimpleDateFormat;


public class JavaUtilDate extends JavaLangObject
        implements JavaIoSerializable, JavaLangCloneable, JavaLangComparable {

    private static final DateFormat fm = new SimpleDateFormat("d MMM yyyy HH:mm:ss z");

    private Date wrappedObject;
    private Calendar cl = new GregorianCalendar();

    public Date getWrappedObject() {
        return wrappedObject;
    }


    public JavaUtilDate() throws CGException {
        wrappedObject = new Date();
        cl.setTime(wrappedObject);
    }

    public JavaUtilDate(Date d) {
        wrappedObject = d;
        cl.setTime(wrappedObject);
    }

    public JavaUtilDate(final Integer date) throws CGException {
        this();
    }

    public Object clone()  {
        return new JavaUtilDate((Date) wrappedObject.clone());
    }

    public Integer getDay() throws CGException {
//        return Integer.valueOf(wrappedObject.getDay());
        return Integer.valueOf(cl.get(Calendar.DAY_OF_WEEK));
    }

    public Integer getDate() throws CGException {
//        return Integer.valueOf(wrappedObject.getDate());
        return Integer.valueOf(cl.get(Calendar.DAY_OF_MONTH));
    }

    public Integer getTime() throws CGException {
        return Integer.valueOf((int) wrappedObject.getTime());
    }

    public Integer getYear() throws CGException {
//        return Integer.valueOf(wrappedObject.getYear());
        return Integer.valueOf(cl.get(Calendar.YEAR));
    }

    public Integer getHours() throws CGException {
//        return Integer.valueOf(wrappedObject.getHours());
        return Integer.valueOf(cl.get(Calendar.HOUR_OF_DAY));
    }

    public Integer getMonth() throws CGException {
//        return Integer.valueOf(wrappedObject.getMonth());
        return Integer.valueOf(cl.get(Calendar.MONTH));
    }

    public Integer jHashCode() throws CGException {
        return  Integer.valueOf(wrappedObject.hashCode());
    }

    public void setDate(final Integer date) throws CGException {
//        wrappedObject.setDate(date.intValue());
        cl.set(Calendar.DAY_OF_MONTH, date.intValue());
        wrappedObject.setTime(cl.getTimeInMillis());
    }

    public void setTime(final Integer time) throws CGException {
        wrappedObject.setTime(time.intValue());
    }

    public void setYear(final Integer year) throws CGException {
//        wrappedObject.setYear(year.intValue());
        cl.set(Calendar.YEAR, year.intValue());
        wrappedObject.setTime(cl.getTimeInMillis());
    }

    public JavaLangString toJString() throws CGException {
        return new JavaLangString(wrappedObject.toString());
    }

    public JavaUtilDate(final quotes.NIL var_1_1) throws CGException {
        this();
        isNil = Boolean.TRUE;
    }

    public JavaUtilDate(final Integer year, final Integer month, final Integer date) throws CGException {
        this();
    }

    public void setHours(final Integer hours) throws CGException {
//        wrappedObject.setHours(hours.intValue());
        cl.set(Calendar.HOUR_OF_DAY, hours.intValue());
        wrappedObject.setTime(cl.getTimeInMillis());
    }

    public void setMonth(final Integer month) throws CGException {
//        wrappedObject.setMonth(month.intValue());
        cl.set(Calendar.MONTH, month.intValue());
        wrappedObject.setTime(cl.getTimeInMillis());
    }

    public Integer getMinutes() throws CGException {
//        return Integer.valueOf(wrappedObject.getMinutes());
        return Integer.valueOf(cl.get(Calendar.MINUTE));
    }

    public Integer getSeconds() throws CGException {
//        return Integer.valueOf(wrappedObject.getSeconds());
        return Integer.valueOf(cl.get(Calendar.SECOND));
    }

    public Boolean after(final JavaUtilDate when) throws CGException {
        return Boolean.valueOf(wrappedObject.after(when.getWrappedObject()));
    }

    public void setMinutes(final Integer minutes) throws CGException {
//        wrappedObject.setMinutes(minutes.intValue());
        cl.set(Calendar.MINUTE, minutes.intValue());
        wrappedObject.setTime(cl.getTimeInMillis());
    }

    public void setSeconds(final Integer seconds) throws CGException {
//        wrappedObject.setSeconds(seconds.intValue());
        cl.set(Calendar.SECOND, seconds.intValue());
        wrappedObject.setTime(cl.getTimeInMillis());
    }

    public JavaLangString toGMTString() throws CGException {
//        return new JavaLangString(wrappedObject.toGMTString());
        return new JavaLangString(fm.format(cl.getInstance(TimeZone.getTimeZone("GMT")).getTime()));
    }

    public JavaUtilDate(final JavaLangString s) throws CGException {
        this();
    }

    public Boolean before(final JavaUtilDate when) throws CGException {
        return Boolean.valueOf(wrappedObject.before(when.getWrappedObject()));
    }

    public JavaUtilDate(final Integer year, final Integer month, final Integer date, final Integer hrs, final Integer min) throws CGException {
        this();
    }

    static public Integer parse(final JavaLangString s) throws CGException {
//        return  Integer.valueOf((int) Date.parse(s.toSeqOfChar()));
        try
        {
          return  Integer.valueOf((int)(fm.parse(s.toSeqOfChar()).getTime()));
        }
        catch (Exception e)
        {
          throw new CGException(e.getMessage());
        }
    }

    static public Integer UTC(final Integer year, final Integer month, final Integer date, final Integer hrs,
                              final Integer min, final Integer sec) throws CGException {
//        return Integer.valueOf((int) Date.UTC(year.intValue(), month.intValue(), date.intValue(),
//                                          hrs.intValue(), min.intValue(), sec.intValue()));
        Calendar utccl = new GregorianCalendar();
        utccl.setTimeZone(TimeZone.getTimeZone("UTC"));
        utccl.set(year.intValue() + 1900, month.intValue(), date.intValue(),
                                          hrs.intValue(), min.intValue(), sec.intValue());
        return Integer.valueOf((int) utccl.getTimeInMillis());
    }

    public Boolean equals(final JavaLangObject obj) throws CGException {
        if (obj instanceof JavaUtilDate)
            return Boolean.valueOf(wrappedObject.equals(((JavaUtilDate) obj).getWrappedObject()));
        else
            return Boolean.FALSE;
    }

    public JavaLangString toLocaleString() throws CGException {
//        return new JavaLangString(wrappedObject.toLocaleString());
        return new JavaLangString(new SimpleDateFormat().format(wrappedObject));
    }

    public JavaUtilDate(final Integer year, final Integer month, final Integer date, final Integer hrs,
                        final Integer min, final Integer sec) throws CGException {
        this();
    }

    public Integer compareTo(final JavaLangObject o) throws CGException {
//        return Integer.valueOf(wrappedObject.compareTo(o));
        return Integer.valueOf(wrappedObject.compareTo(((JavaUtilDate) o).getWrappedObject()));
    }


    public Integer getTimezoneOffset() throws CGException {
//        return Integer.valueOf(wrappedObject.getTimezoneOffset());
        return Integer.valueOf(cl.get(Calendar.ZONE_OFFSET) + cl.get(Calendar.DST_OFFSET)/( 60 * 1000 ));
    }
}

