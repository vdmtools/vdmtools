package jp.vdmtools.VDM.jdk;

import jp.vdmtools.VDM.CGException;
import jp.vdmtools.VDM.UTIL;
import jp.vdmtools.VDM.VDMSpecException;

import java.util.HashMap;
import java.util.HashSet;
import java.util.Iterator;


public class JavaIoBufferedOutputStream extends JavaIoFilterOutputStream {

    protected HashMap buf = new HashMap();

    protected Integer count = null;

    private static final JavaLangClass CLASS = (JavaLangClass) new JavaLangClass(new String("JavaIoBufferedOutputStream"), new quotes.CLASS(), new Boolean(false));


    public JavaIoBufferedOutputStream() throws CGException {
    }


    public void flush() throws CGException {
        flushBuffer();
        out.flush();
    }


    private void flushBuffer() throws CGException {
        if (new Boolean((count.intValue()) > (new Integer(0).intValue())).booleanValue())
            out.write2(buf, new Integer(0), count);
        count = UTIL.NumberToInt(UTIL.clone(new Integer(0)));
    }


    public void write(final Integer b) throws CGException {
        Boolean cond_2 = null;
        Integer var2_4 = null;
        HashSet unArg_5 = new HashSet();
        unArg_5.clear();
        unArg_5.addAll(buf.keySet());
        var2_4 = new Integer(unArg_5.size());
        cond_2 = new Boolean((count.intValue()) >= (var2_4.intValue()));
        if (cond_2.booleanValue())
            flushBuffer();
        buf.put(count, b);
        count = UTIL.NumberToInt(UTIL.clone(new Integer(count.intValue() + new Integer(1).intValue())));
    }


    public JavaLangClass getJClass() throws CGException {
        return CLASS;
    }


    public void write2(final HashMap b, final Integer off, final Integer len_u_u) throws CGException {
        Boolean cond_4 = null;
        Integer var2_6 = null;
        HashSet unArg_7 = new HashSet();
        unArg_7.clear();
        unArg_7.addAll(buf.keySet());
        var2_6 = new Integer(unArg_7.size());
        cond_4 = new Boolean((len_u_u.intValue()) >= (var2_6.intValue()));
        if (cond_4.booleanValue()) {
            flushBuffer();
            out.write2(b, off, len_u_u);
            return;
        }
        Boolean cond_14 = null;
        Integer var2_16 = null;
        Integer var1_17 = null;
        HashSet unArg_18 = new HashSet();
        unArg_18.clear();
        unArg_18.addAll(buf.keySet());
        var1_17 = new Integer(unArg_18.size());
        var2_16 = new Integer(var1_17.intValue() - count.intValue());
        cond_14 = new Boolean((len_u_u.intValue()) > (var2_16.intValue()));
        if (cond_14.booleanValue())
            flushBuffer();
        for (int ilb_26 = 0; ilb_26 <= new Integer(len_u_u.intValue() - new Integer(1).intValue()).intValue(); ilb_26++) {
            Integer i = new Integer(ilb_26);
            buf.put(new Integer(count.intValue() + i.intValue()), UTIL.NumberToInt(b.get(new Integer(off.intValue() + i.intValue()))));
        }
        count = UTIL.NumberToInt(UTIL.clone(new Integer(count.intValue() + len_u_u.intValue())));
    }


    public JavaIoBufferedOutputStream(final quotes.NIL var_1_1) throws CGException {
        this();
        isNil = (Boolean) UTIL.clone(new Boolean(true));
    }


    public JavaIoBufferedOutputStream(final JavaIoOutputStream out) throws CGException {
	this (out, new Integer(512));
    }


    public JavaIoBufferedOutputStream(final JavaIoOutputStream out, final Integer size) throws CGException {
        super((JavaIoOutputStream) out);
        if (new Boolean((size.intValue()) <= (new Integer(0).intValue())).booleanValue())
            throw new VDMSpecException(new JavaLangIllegalArgumentException(new JavaLangString(new String("Buffer size <= 0"))));
        HashMap rhs_12 = new HashMap();
        HashMap res_m_13 = new HashMap();
        {
            boolean succ_16 = true;
            HashSet e_set_17 = new HashSet();
            e_set_17 = new HashSet();
            int ubi_22 = new Integer(size.intValue() - new Integer(1).intValue()).intValue();
            for (int count_23 = new Integer(0).intValue(); count_23 <= ubi_22; count_23++)
                e_set_17.add(new Integer(count_23));
            Integer i = null;
            {
                for (Iterator enum_25 = e_set_17.iterator(); enum_25.hasNext();) {
                    Integer elem_24 = UTIL.NumberToInt(enum_25.next());
                    succ_16 = true;
                    i = elem_24;
                    if (succ_16)
                        res_m_13.put(i, new Integer(0));
                }
            }
        }
        rhs_12 = res_m_13;
        buf = (HashMap) UTIL.clone(rhs_12);
        count = UTIL.NumberToInt(UTIL.clone(new Integer(0)));
    }

}

;
