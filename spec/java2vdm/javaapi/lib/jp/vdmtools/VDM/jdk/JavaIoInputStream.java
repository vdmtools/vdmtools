package jp.vdmtools.VDM.jdk;

import jp.vdmtools.VDM.CGException;
import jp.vdmtools.VDM.Tuple;
import jp.vdmtools.VDM.UTIL;
import jp.vdmtools.VDM.VDMSpecException;

import java.util.HashMap;
import java.util.TreeSet;

public abstract class JavaIoInputStream extends JavaLangObject {

    private static final JavaLangClass CLASS =
            new JavaLangClass(new String("JavaIoInputStream"),
                    new quotes.CLASS(),
                    new Boolean(false));

    static UTIL.VDMCompare vdmComp = new UTIL.VDMCompare();

    public abstract java.io.InputStream getWrappedObject();

    public JavaIoInputStream() {
    }


    // This should really be abstract; subclass provide implementations
    public Integer read() throws CGException {
        return new Integer(0);
    }


    public void close() throws CGException {
    }


    public void mark(final Integer readlimit) throws CGException {
    }


    public void reset() throws CGException {
        String msgStr = new String("mark/reset not supported");
        JavaLangString msg = new JavaLangString(msgStr);
        throw new VDMSpecException(new JavaIoIOException(msg));
    }


    public Integer skip_u_u(final Integer n) throws CGException {
        Integer remaining = n;
        Integer c = new Integer(0);
        while (true) {
            Boolean whCrtl_2 = null;
            {
                if ((whCrtl_2 = new Boolean((remaining.intValue()) > (new Integer(0).intValue()))).booleanValue())
                    whCrtl_2 = new Boolean((c.intValue()) >= (new Integer(0).intValue()));
            }
            if (whCrtl_2.booleanValue()) {
                Integer c_1 = null;
                c_1 = UTIL.NumberToInt(UTIL.clone(read()));
                if (new Boolean((c_1.intValue()) >= (new Integer(0).intValue())).booleanValue())
                    remaining = UTIL.NumberToInt(UTIL.clone(new Integer(remaining.intValue() - new Integer(1).intValue())));
            } else
                break;
        }
        return new Integer(n.intValue() - remaining.intValue());
    }


//    public JavaLangClass getClass () {
//	return CLASS;
//    }


    public Integer available() throws CGException {
        return new Integer(0);
    }


    public Integer read(final HashMap b) throws CGException {
        return readMIIIntInt(b, new Integer(0), new Integer(b.size()));
    }


    public Boolean markSupported() throws CGException {
        return new Boolean(false);
    }


    public Integer readMIIIntInt(final HashMap b_u,
                                 final Integer off,
                                 final Integer len_u_u) throws CGException {
        UTIL.RunTime("Run-Time Error:Can not evaluate an undefined expression");
        return new Integer(0);
    }


    public Tuple readMIIIntInt_q(final HashMap b_u, final Integer off, final Integer len_u_u) throws CGException {
        HashMap b = b_u;
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
            else if (new Boolean(len_u_u.intValue() == new Integer(0).intValue()).booleanValue()) {
                Tuple rexpr_37 = new Tuple(2);
                rexpr_37 = new Tuple(2);
                rexpr_37.SetField(1, new Integer(0));
                rexpr_37.SetField(2, b);
                return rexpr_37;
            }
        }
        {
            Integer c = read();
            if (new Boolean(c.intValue() == new Integer(-new Integer(1).intValue()).intValue()).booleanValue()) {
                Tuple rexpr_46 = new Tuple(2);
                rexpr_46 = new Tuple(2);
                rexpr_46.SetField(1, new Integer(-new Integer(1).intValue()));
                rexpr_46.SetField(2, b);
                return rexpr_46;
            }
            b.put(off, c);
            {
                int i = 1;
                while (i < len_u_u.intValue()) {
                    c = read();
                    if (c.intValue() == -1)
                        break;
                    if (!b_u.isEmpty())
                        b.put(new Integer(off.intValue() + i), c);
                    i++;
                }

                Tuple rexpr_53 = new Tuple(2);
                rexpr_53 = new Tuple(2);
                rexpr_53.SetField(1, new Integer(i));
                rexpr_53.SetField(2, b);
                return rexpr_53;
            }
        }
    }


    public JavaIoInputStream(final quotes.NIL n) {
        isNil = new Boolean(true);
    }

}
