package jp.vdmtools.VDM.jdk;

import jp.vdmtools.VDM.CGException;
import jp.vdmtools.VDM.UTIL;
import jp.vdmtools.VDM.VDMSpecException;

import java.util.HashMap;
import java.util.HashSet;
import java.util.Iterator;


public class JavaIoOutputStreamWriter extends JavaIoWriter {

    private CharToByteConverter ctb = null;

    private JavaIoOutputStream out = null;

    private static Integer defaultByteBufferSize = new Integer(80);

    private HashMap bb = new HashMap();

    private Integer nextByte = null;

    private Integer nBytes = null;


    public JavaIoOutputStreamWriter() throws CGException {
        defaultByteBufferSize = new Integer(80);
        nextByte = new Integer(0);
        nBytes = new Integer(0);
    }


    public void close() throws CGException {
        {
            flush();
            out.close();
            bb = (HashMap) UTIL.clone(new HashMap());
            ctb = (CharToByteConverter) UTIL.clone(new CharToByteConverter(new quotes.NIL()));
        }
        ;
    }


    public void flush() throws CGException {
        {
            flushBuffer();
            out.flush();
        }
        ;
    }


    public void write(final Integer c) throws CGException {
        HashMap cbuf = new HashMap();
        HashMap res_m_2 = new HashMap();
        {
            boolean succ_5 = true;
            HashSet e_set_6 = new HashSet();
            e_set_6 = new HashSet();
            int ubi_11 = new Integer(new Integer(1).intValue() - new Integer(1).intValue()).intValue();
            for (int count_12 = new Integer(0).intValue(); count_12 <= ubi_11; count_12++)
                e_set_6.add(new Integer(count_12));
            Integer l_u3 = null;
            {
                for (Iterator enum_14 = e_set_6.iterator(); enum_14.hasNext();) {
                    Integer elem_13 = UTIL.NumberToInt(enum_14.next());
                    succ_5 = true;
                    l_u3 = elem_13;
                    if (succ_5)
                        res_m_2.put(l_u3, new Character(' '));
                }
            }
        }
        cbuf = res_m_2;
        Character mr_17 = null;
        JavaLangInteger obj_18 = null;
        obj_18 = (JavaLangInteger) new JavaLangInteger(c);
        mr_17 = obj_18.charValue();
        cbuf.put(new Integer(0), mr_17);
        writeICMIntInt(cbuf, new Integer(0), new Integer(1));
    }


    private void ensureOpen() throws CGException {
        ;
    }


    public void flushBuffer() throws CGException {
        ensureOpen();
        out.write2(bb, new Integer(0), nextByte);
        nextByte = UTIL.NumberToInt(UTIL.clone(new Integer(0)));
    }


    public JavaLangString getEncoding() throws CGException {
        return (JavaLangString) new JavaLangString(new quotes.NIL());
    }


    public void writeICMIntInt(final HashMap cbuf, final Integer off, final Integer len_u_u) throws CGException {
        ensureOpen();
        Boolean cond_5 = null;
        Boolean var1_6 = null;
        Boolean var1_7 = null;
        Boolean var1_8 = null;
        {
            if (!(var1_8 = new Boolean((off.intValue()) < (new Integer(0).intValue()))).booleanValue()) {
                Boolean var2_12 = null;
                Integer var2_14 = null;
                HashSet unArg_15 = new HashSet();
                unArg_15.clear();
                unArg_15.addAll(cbuf.keySet());
                var2_14 = new Integer(unArg_15.size());
                var2_12 = new Boolean((off.intValue()) > (var2_14.intValue()));
                var1_8 = var2_12;
            }
        }
        {
            if (!(var1_7 = var1_8).booleanValue())
                var1_7 = new Boolean((len_u_u.intValue()) < (new Integer(0).intValue()));
        }
        {
            if (!(var1_6 = var1_7).booleanValue()) {
                Boolean var2_20 = null;
                Integer var2_24 = null;
                HashSet unArg_25 = new HashSet();
                unArg_25.clear();
                unArg_25.addAll(cbuf.keySet());
                var2_24 = new Integer(unArg_25.size());
                var2_20 = new Boolean((new Integer(off.intValue() + len_u_u.intValue()).intValue()) > (var2_24.intValue()));
                var1_6 = var2_20;
            }
        }
        {
            if (!(cond_5 = var1_6).booleanValue())
                cond_5 = new Boolean((new Integer(off.intValue() + len_u_u.intValue()).intValue()) < (new Integer(0).intValue()));
        }
        if (cond_5.booleanValue())
            throw new VDMSpecException(new JavaLangIndexOutOfBoundsException());
        else if (new Boolean(len_u_u.intValue() == new Integer(0).intValue()).booleanValue())
            return;
        {
            Integer ci = off;
            Integer end_u_u = new Integer(off.intValue() + len_u_u.intValue());
            Boolean bufferFlushed = new Boolean(false);
            flushBuffer();
            if (new Boolean((new Integer(nBytes.intValue() - nextByte.intValue()).intValue()) < (len_u_u.intValue())).booleanValue())
                throw new VDMSpecException(new JavaLangConversionBufferFullException());
            else {
                HashMap rhs_44 = new HashMap();
                HashMap modmap_45 = new HashMap();
                HashMap res_m_48 = new HashMap();
                {
                    boolean succ_58 = true;
                    HashSet e_set_59 = new HashSet();
                    e_set_59 = new HashSet();
                    int ubi_66 = new Integer(new Integer(nextByte.intValue() + len_u_u.intValue()).intValue() - new Integer(1).intValue()).intValue();
                    for (int count_67 = nextByte.intValue(); count_67 <= ubi_66; count_67++)
                        e_set_59.add(new Integer(count_67));
                    Integer i = null;
                    {
                        for (Iterator enum_69 = e_set_59.iterator(); enum_69.hasNext();) {
                            Integer elem_68 = UTIL.NumberToInt(enum_69.next());
                            succ_58 = true;
                            i = elem_68;
                            if (succ_58) {
                                Integer mr_50 = null;
                                mr_50 = J2VUTIL.toInt((Character) cbuf.get(new Integer(new Integer(off.intValue() + i.intValue()).intValue() - nextByte.intValue())));
                                res_m_48.put(i, mr_50);
                            }
                        }
                    }
                }
                modmap_45 = res_m_48;
                rhs_44 = new HashMap(bb);
                rhs_44.putAll(modmap_45);
                bb = (HashMap) UTIL.clone(rhs_44);
                nextByte = UTIL.NumberToInt(UTIL.clone(new Integer(nextByte.intValue() + len_u_u.intValue())));
            }
        }
    }


    public void writeStrIntInt(final JavaLangString str, final Integer off, final Integer len_u_u) throws CGException {
        if (new Boolean((len_u_u.intValue()) < (new Integer(0).intValue())).booleanValue())
            throw new VDMSpecException(new JavaLangIndexOutOfBoundsException());
        {
            HashMap cbuf = new HashMap();
            HashMap res_m_8 = new HashMap();
            {
                boolean succ_11 = true;
                HashSet e_set_12 = new HashSet();
                e_set_12 = new HashSet();
                int ubi_17 = new Integer(len_u_u.intValue() - new Integer(1).intValue()).intValue();
                for (int count_18 = new Integer(0).intValue(); count_18 <= ubi_17; count_18++)
                    e_set_12.add(new Integer(count_18));
                Integer l_u4 = null;
                {
                    for (Iterator enum_20 = e_set_12.iterator(); enum_20.hasNext();) {
                        Integer elem_19 = UTIL.NumberToInt(enum_20.next());
                        succ_11 = true;
                        l_u4 = elem_19;
                        if (succ_11)
                            res_m_8.put(l_u4, new Character(' '));
                    }
                }
            }
            cbuf = res_m_8;
            str.getChars(off, new Integer(off.intValue() + len_u_u.intValue()), cbuf, new Integer(0));
            writeICMIntInt(cbuf, new Integer(0), len_u_u);
        }
        ;
    }


    public JavaIoOutputStreamWriter(final quotes.NIL var_1_1) throws CGException {
        this();
        isNil = (Boolean) UTIL.clone(new Boolean(true));
    }


    public JavaIoOutputStreamWriter(final JavaIoOutputStream out_1) throws CGException {
        this(out_1, new CharToByteConverter(new quotes.NIL()));
    }


    public JavaIoOutputStreamWriter(final JavaIoOutputStream out_1, final JavaLangString enc) throws CGException {
        this(out_1, new CharToByteConverter(new quotes.NIL()));
    }


    private JavaIoOutputStreamWriter(final JavaIoOutputStream out_1, final CharToByteConverter ctb_1) throws CGException {
        super(out_1);
        ((JavaIoOutputStreamWriter) this).out = (JavaIoOutputStream) UTIL.clone(out_1);
        ((JavaIoOutputStreamWriter) this).ctb = (CharToByteConverter) UTIL.clone(ctb_1);
        HashMap rhs_11 = new HashMap();
        HashMap res_m_12 = new HashMap();
        {
            boolean succ_15 = true;
            HashSet e_set_16 = new HashSet();
            e_set_16 = new HashSet();
            int ubi_21 = new Integer(defaultByteBufferSize.intValue() - new Integer(1).intValue()).intValue();
            for (int count_22 = new Integer(0).intValue(); count_22 <= ubi_21; count_22++)
                e_set_16.add(new Integer(count_22));
            Integer l_u3 = null;
            {
                for (Iterator enum_24 = e_set_16.iterator(); enum_24.hasNext();) {
                    Integer elem_23 = UTIL.NumberToInt(enum_24.next());
                    succ_15 = true;
                    l_u3 = elem_23;
                    if (succ_15)
                        res_m_12.put(l_u3, new Integer(0));
                }
            }
        }
        rhs_11 = res_m_12;
        bb = (HashMap) UTIL.clone(rhs_11);
        nBytes = UTIL.NumberToInt(UTIL.clone(defaultByteBufferSize));
    }

}

