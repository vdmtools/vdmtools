package jp.vdmtools.VDM.jdk;


import jp.vdmtools.VDM.CGException;
import jp.vdmtools.VDM.Tuple;
import jp.vdmtools.VDM.UTIL;
import jp.vdmtools.VDM.VDMSpecException;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.Reader;
import java.util.HashMap;


public class JavaIoBufferedReader extends JavaIoReader {

    private BufferedReader wrappedObject;

    public Reader getWrappedObject() {
        return (Reader) wrappedObject;
    }

    public JavaIoBufferedReader() throws CGException {
    }


    public Integer read() throws CGException {
        try {
            return Integer.valueOf(wrappedObject.read());
        } catch (IOException e) {
            throw new VDMSpecException(new JavaIoIOException());
        }
    }


    public void close() throws CGException {
        try {
            wrappedObject.close();
        } catch (IOException e) {
            throw new VDMSpecException(new JavaIoIOException());
        }
    }


    public void mark(final Integer readAheadLimit_1) throws CGException {
        try {
            wrappedObject.mark(readAheadLimit_1.intValue());
        } catch (IOException e) {
            throw new VDMSpecException(new JavaIoIOException());
        }
    }


    public Boolean ready() throws CGException {
        try {
            return Boolean.valueOf(wrappedObject.ready());
        } catch (IOException e) {
            throw new VDMSpecException(new JavaIoIOException());
        }
    }


    public void reset() throws CGException {
        try {
            wrappedObject.reset();
        } catch (IOException e) {
            throw new VDMSpecException(new JavaIoIOException());
        }
    }


    public Integer skip_u_u(final Integer n) throws CGException {
        try {
            return Integer.valueOf((int) wrappedObject.skip(n.intValue()));
        } catch (IOException e) {
            throw new VDMSpecException(new JavaIoIOException());
        }
    }


    public JavaLangString readLine() throws CGException {
        try {
            return new JavaLangString(wrappedObject.readLine());
        } catch (IOException e) {
            throw new VDMSpecException(new JavaIoIOException());
        }
    }


    public Boolean markSupported() throws CGException {
        return Boolean.valueOf(wrappedObject.markSupported());
    }


    public Integer read(final HashMap cbuf, final Integer off, final Integer len_u_u) throws CGException {
        Object rexpr_4 = null;
        UTIL.RunTime("Run-Time Error:Can not evaluate an undefined expression");
        return UTIL.NumberToInt(rexpr_4);
    }

    public Integer readMICIntInt(final HashMap cbuf, final Integer off, final Integer len_u_u) throws CGException {
        Object rexpr_4 = null;
        UTIL.RunTime("Run-Time Error:Can not evaluate an undefined expression");
        return UTIL.NumberToInt(rexpr_4);
    }


    public Tuple read_q(final HashMap cbuf_u,
                        final Integer off,
                        final Integer len_u_u) throws CGException {
        return readMICIntInt_q(cbuf_u, off, len_u_u);
    }

    public Tuple readMICIntInt_q(final HashMap cbuf,
                                 final Integer off,
                                 final Integer len)
            throws CGException {
        try {
            HashMap result = new HashMap(cbuf);
            int offVal = off.intValue();
            int lenVal = len.intValue();
            int value = 0;
            int i;
            for (i = 0; i < lenVal && value != -1; i++) {
                value = wrappedObject.read();
                if (value != -1)
                    result.put(Integer.valueOf(offVal + i), Integer.valueOf(value));
            }
            Tuple restuple = new Tuple(2);
            restuple.SetField(1, Integer.valueOf(i));
            restuple.SetField(2, result);
            return restuple;
        } catch (IOException e) {
            throw new VDMSpecException(new JavaIoIOException());
        }
    }


    public JavaIoBufferedReader(final quotes.NIL var_1_1) throws CGException {
        this();
        isNil = Boolean.TRUE;
    }


    public JavaIoBufferedReader(final JavaIoFileReader in_u_u_1)
            throws CGException {
        wrappedObject = new BufferedReader(in_u_u_1.getWrappedObject());
    }


    public JavaIoBufferedReader(final JavaIoReader in_u_u_1,
                                final Integer sz) throws CGException {
        wrappedObject = new BufferedReader(in_u_u_1.getWrappedObject(),
                sz.intValue());
    }

}

;
