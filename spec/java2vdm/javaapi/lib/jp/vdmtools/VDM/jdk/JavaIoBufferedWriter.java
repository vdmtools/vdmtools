package jp.vdmtools.VDM.jdk;

import jp.vdmtools.VDM.CGException;
import jp.vdmtools.VDM.UTIL;
import jp.vdmtools.VDM.VDMSpecException;

import java.util.HashMap;
import java.util.HashSet;
import java.util.Iterator;


public class JavaIoBufferedWriter extends JavaIoWriter {

    private JavaIoWriter out = null;

    private HashMap cb = new HashMap();

    private Integer nChars = null;

    private Integer nextChar = null;

    private static Integer defaultCharBufferSize = new Integer(80);

    private JavaLangString lineSeparator = null;


    public JavaIoBufferedWriter() throws CGException {
        defaultCharBufferSize = new Integer(80);
    }


    public void close() throws CGException {
        {
            flushBuffer();
            out.close();
            cb = (HashMap) UTIL.clone(new HashMap());
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


    private Integer min(final Integer a, final Integer b) throws CGException {
        if (new Boolean((a.intValue()) < (b.intValue())).booleanValue())
            return a;
        return b;
    }


    public void write(final Integer c) throws CGException {
        {
            ensureOpen();
            if (new Boolean((nextChar.intValue()) >= (nChars.intValue())).booleanValue())
                flushBuffer();
            {
                Integer l_u3 = nextChar;
                Integer l_u2 = l_u3;
                {
                    nextChar = UTIL.NumberToInt(UTIL.clone(new Integer(l_u3.intValue() + new Integer(1).intValue())));
                    Character mr_15 = null;
                    JavaLangInteger obj_16 = null;
                    obj_16 = (JavaLangInteger) new JavaLangInteger(c);
                    mr_15 = obj_16.charValue();
                    cb.put(l_u2, mr_15);
                }
            }
        }
        ;
    }


    public void newLine() throws CGException {
        writeStrIntInt((JavaLangString) lineSeparator, new Integer(0), new Integer(1));
        ;
    }


    private void ensureOpen() throws CGException {
        ;
    }


    public void flushBuffer() throws CGException {
        ensureOpen();
        if (new Boolean(nextChar.intValue() == new Integer(0).intValue()).booleanValue())
            return;
        out.writeICMIntInt(cb, new Integer(0), nextChar);
        nextChar = UTIL.NumberToInt(UTIL.clone(new Integer(0)));
    }


    public void write(final Object buf, final Integer off, final Integer len_u_u) throws CGException {
        Boolean cond_4 = null;
        cond_4 = new Boolean(buf instanceof JavaLangString);
        if (cond_4.booleanValue())
            writeStrIntInt((JavaLangString) buf, off, len_u_u);
        else
            writeICMIntInt((HashMap) buf, off, len_u_u);
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
        else {
            if (new Boolean(len_u_u.intValue() == new Integer(0).intValue()).booleanValue())
                return;
            flushBuffer();
            if (new Boolean((new Integer(nChars.intValue() - nextChar.intValue()).intValue()) < (len_u_u.intValue())).booleanValue())
                throw new VDMSpecException(new JavaLangConversionBufferFullException());
            else {
                HashMap rhs_41 = new HashMap();
                HashMap modmap_42 = new HashMap();
                HashMap res_m_45 = new HashMap();
                {
                    boolean succ_54 = true;
                    HashSet e_set_55 = new HashSet();
                    e_set_55 = new HashSet();
                    int ubi_62 = new Integer(new Integer(nextChar.intValue() + len_u_u.intValue()).intValue() - new Integer(1).intValue()).intValue();
                    for (int count_63 = nextChar.intValue(); count_63 <= ubi_62; count_63++)
                        e_set_55.add(new Integer(count_63));
                    Integer i = null;
                    {
                        for (Iterator enum_65 = e_set_55.iterator(); enum_65.hasNext();) {
                            Integer elem_64 = UTIL.NumberToInt(enum_65.next());
                            succ_54 = true;
                            i = elem_64;
                            if (succ_54)
                                res_m_45.put(i, (Character) cbuf.get(new Integer(new Integer(off.intValue() + i.intValue()).intValue() - nextChar.intValue())));
                        }
                    }
                }
                modmap_42 = res_m_45;
                rhs_41 = new HashMap(cb);
                rhs_41.putAll(modmap_42);
                cb = (HashMap) UTIL.clone(rhs_41);
                nextChar = UTIL.NumberToInt(UTIL.clone(new Integer(nextChar.intValue() + len_u_u.intValue())));
            }
        }
    }


    public void writeStrIntInt(final JavaLangString s, final Integer off, final Integer len_u_u) throws CGException {
        {
            ensureOpen();
            {
                Integer b = off;
                Integer t = new Integer(off.intValue() + len_u_u.intValue());
                while (new Boolean((b.intValue()) < (t.intValue())).booleanValue()) {
                    Integer d = min(new Integer(nChars.intValue() - nextChar.intValue()), new Integer(t.intValue() - b.intValue()));
                    HashMap m = new HashMap();
                    JavaLangString obj_16 = null;
                    Object tmpGen_17 = null;
		    obj_16 = s.substring(b, new Integer(b.intValue() + 
							d.intValue()));
                    m = obj_16.toCharArray();
                    for (int ilb_26 = b.intValue(); ilb_26 <= new Integer(new Integer(b.intValue() + d.intValue()).intValue() - new Integer(1).intValue()).intValue(); ilb_26++) {
                        Integer i = new Integer(ilb_26);
                        cb.put(new Integer(new Integer(nextChar.intValue() + i.intValue()).intValue() - b.intValue()), (Character) m.get(new Integer(i.intValue() - b.intValue())));
                    }
                    {
                        Integer l_u3 = b;
                        b = UTIL.NumberToInt(UTIL.clone(new Integer(l_u3.intValue() + d.intValue())));
                    }
                    {
                        Integer l_u3 = nextChar;
                        nextChar = UTIL.NumberToInt(UTIL.clone(new Integer(l_u3.intValue() + d.intValue())));
                    }
                    if (new Boolean((nextChar.intValue()) >= (nChars.intValue())).booleanValue())
                        flushBuffer();
                }
            }
        }
        ;
    }


    public JavaIoBufferedWriter(final quotes.NIL var_1_1) throws CGException {
        this();
        isNil = (Boolean) UTIL.clone(new Boolean(true));
    }


    public JavaIoBufferedWriter(final JavaIoWriter out_1) throws CGException {
        this(out_1, defaultCharBufferSize);
    }


    public JavaIoBufferedWriter(final JavaIoWriter out_1, final Integer sz) throws CGException {
        super(out_1);
        if (new Boolean((sz.intValue()) <= (new Integer(0).intValue())).booleanValue())
            throw new VDMSpecException(new JavaLangIllegalArgumentException(new JavaLangString(new String("Buffer size <= 0"))));
        ((JavaIoBufferedWriter) this).out = (JavaIoWriter) UTIL.clone(out_1);
        HashMap rhs_16 = new HashMap();
        HashMap res_m_17 = new HashMap();
        {
            boolean succ_20 = true;
            HashSet e_set_21 = new HashSet();
            e_set_21 = new HashSet();
            int ubi_26 = new Integer(sz.intValue() - new Integer(1).intValue()).intValue();
            for (int count_27 = new Integer(0).intValue(); count_27 <= ubi_26; count_27++)
                e_set_21.add(new Integer(count_27));
            Integer l_u3 = null;
            {
                for (Iterator enum_29 = e_set_21.iterator(); enum_29.hasNext();) {
                    Integer elem_28 = UTIL.NumberToInt(enum_29.next());
                    succ_20 = true;
                    l_u3 = elem_28;
                    if (succ_20)
                        res_m_17.put(l_u3, new Character(' '));
                }
            }
        }
        rhs_16 = res_m_17;
        cb = (HashMap) UTIL.clone(rhs_16);
        nChars = UTIL.NumberToInt(UTIL.clone(sz));
        nextChar = UTIL.NumberToInt(UTIL.clone(new Integer(0)));
        lineSeparator = (JavaLangString) UTIL.clone(new JavaLangString(new String("\n")));
    }

}

