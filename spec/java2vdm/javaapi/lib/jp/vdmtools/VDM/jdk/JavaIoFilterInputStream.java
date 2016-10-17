package jp.vdmtools.VDM.jdk;

import jp.vdmtools.VDM.CGException;
import jp.vdmtools.VDM.UTIL;

import java.io.InputStream;
import java.util.HashMap;


public class JavaIoFilterInputStream extends JavaIoInputStream {

    public InputStream getWrappedObject() {
        return in_u_u.getWrappedObject();
    }

    protected JavaIoInputStream in_u_u = null;

    private static final JavaLangClass CLASS = (JavaLangClass) new JavaLangClass(new String("JavaIoFilterInputStream"), new quotes.CLASS(), new Boolean(false));


    public JavaIoFilterInputStream() throws CGException {
    }


    public Integer read() throws CGException {
        return in_u_u.read();
    }


    public void close() throws CGException {
        in_u_u.close();
    }


    public void mark(final Integer readlimit) throws CGException {
        in_u_u.mark(readlimit);
    }


    public void reset() throws CGException {
        in_u_u.reset();
    }


    public Integer skip_u_u(final Integer n) throws CGException {
        Integer rexpr_2 = null;
        rexpr_2 = in_u_u.skip_u_u(n);
        return rexpr_2;
    }


//  public JavaLangClass getClass () throws CGException {
//    return (JavaLangClass) CLASS;
//  }


    public Integer available() throws CGException {
        Integer rexpr_1 = null;
        rexpr_1 = in_u_u.available();
        return rexpr_1;
    }


    public Integer read(final HashMap b) throws CGException {
        return read(b, new Integer(0), new Integer(b.size()));
    }


    public Boolean markSupported() throws CGException {
        Boolean rexpr_1 = null;
        rexpr_1 = in_u_u.markSupported();
        return rexpr_1;
    }


    public Integer read(final HashMap b, final Integer off, final Integer len_u_u) throws CGException {
        Object rexpr_4 = null;
        UTIL.RunTime("Run-Time Error:Can not evaluate an undefined expression");
        return UTIL.NumberToInt(rexpr_4);
    }


    public JavaIoFilterInputStream(final quotes.NIL var_1_1) throws CGException {
        this();
        isNil = (Boolean) UTIL.clone(new Boolean(true));
    }


    protected JavaIoFilterInputStream(final JavaIoInputStream in_u_u_1) throws CGException {
        this();
        ((JavaIoFilterInputStream) this).in_u_u = (JavaIoInputStream) UTIL.clone(in_u_u_1);
    }

}

;
