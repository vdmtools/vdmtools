package jp.vdmtools.VDM.jdk;

import jp.vdmtools.VDM.CGException;
import jp.vdmtools.VDM.UTIL;
import jp.vdmtools.VDM.VDMSpecException;

import java.util.HashMap;
import java.util.TreeSet;


public class JavaIoOutputStream extends JavaLangObject {

    static UTIL.VDMCompare vdmComp = new UTIL.VDMCompare();

    private static final JavaLangClass CLASS =
            new JavaLangClass(new String("JavaIoOutputStream"),
                    new quotes.CLASS(),
                    new Boolean(false));


    public JavaIoOutputStream() {
    }


    private JavaIoOutputStream(final quotes.NIL n) {
        isNil = new Boolean(true);
    }


    public void close() throws CGException {
    }


    public void flush() throws CGException {
    }


    // This should really be abstract; subclasses provide implementations
    public void write(final Integer b) throws CGException {
    }


    public void write1(final HashMap b) throws CGException {
        write2(b, new Integer(0), new Integer(b.size()));
    }


    public void write2(final HashMap b,
                       final Integer off,
                       final Integer len_u_u) throws CGException {
        if (new Boolean(UTIL.equals(b, new HashMap())).booleanValue())
            throw new VDMSpecException(new JavaLangNullPointerException());
        else {
            Boolean cond_7 = null;
            Boolean var1_8 = null;
            Boolean var1_9 = null;
            Boolean var1_10 = null;
            {
                if (!(var1_10 = new Boolean((off.intValue()) < (new Integer(0).intValue()))).booleanValue()) {
                    Boolean var2_14 = null;
                    Integer var2_16 = null;
                    TreeSet unArg_17 = new TreeSet(vdmComp);
                    unArg_17.clear();
                    unArg_17.addAll(b.keySet());
                    var2_16 = new Integer(unArg_17.size());
                    var2_14 = new Boolean((off.intValue()) > (var2_16.intValue()));
                    var1_10 = var2_14;
                }
            }
            {
                if (!(var1_9 = var1_10).booleanValue())
                    var1_9 = new Boolean((len_u_u.intValue()) < (new Integer(0).intValue()));
            }
            {
                if (!(var1_8 = var1_9).booleanValue()) {
                    Boolean var2_22 = null;
                    Integer var2_26 = null;
                    TreeSet unArg_27 = new TreeSet(vdmComp);
                    unArg_27.clear();
                    unArg_27.addAll(b.keySet());
                    var2_26 = new Integer(unArg_27.size());
                    var2_22 = new Boolean((new Integer(off.intValue() + len_u_u.intValue()).intValue()) > (var2_26.intValue()));
                    var1_8 = var2_22;
                }
            }
            {
                if (!(cond_7 = var1_8).booleanValue())
                    cond_7 = new Boolean((new Integer(off.intValue() + len_u_u.intValue()).intValue()) < (new Integer(0).intValue()));
            }
            if (cond_7.booleanValue())
                throw new VDMSpecException(new JavaLangIndexOutOfBoundsException());
            else if (new Boolean(len_u_u.intValue() == new Integer(0).intValue()).booleanValue())
                return;
        }
        {
            Integer i = new Integer(0);
            while (new Boolean((i.intValue()) < (len_u_u.intValue())).booleanValue()) {
                write(UTIL.NumberToInt(b.get(new Integer(off.intValue() + i.intValue()))));
                i = UTIL.NumberToInt(UTIL.clone(new Integer(i.intValue() + new Integer(1).intValue())));
            }
        }
        ;
    }

}
