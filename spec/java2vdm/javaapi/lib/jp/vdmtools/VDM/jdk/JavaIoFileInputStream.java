package jp.vdmtools.VDM.jdk;

import jp.vdmtools.VDM.CGException;
import jp.vdmtools.VDM.Tuple;
import jp.vdmtools.VDM.VDMSpecException;

import java.util.HashMap;

public class JavaIoFileInputStream extends JavaIoInputStream {

    private java.io.FileInputStream wrappedObject;

    public java.io.InputStream getWrappedObject() {
        return wrappedObject;
    }

    public JavaIoFileInputStream() {
    }

    public Integer read() throws CGException {
        try {
            int res = wrappedObject.read();
            return new Integer(res);
        } catch (java.io.IOException e) {
            throw new VDMSpecException(new JavaIoIOException());
        }
    }


    public void close() throws CGException {
        try {
            wrappedObject.close();
        } catch (java.io.IOException e) {
            throw new VDMSpecException(new JavaIoIOException());
        }
    }


    public JavaIoFileDescriptor getFD() throws CGException {
        try {
            return new JavaIoFileDescriptor(wrappedObject.getFD());
        } catch (java.io.IOException e) {
            throw new VDMSpecException(new JavaIoIOException());
        }
    }


    public Integer skip_u_u(final Integer n) throws CGException {
        try {
            return new Integer((int) wrappedObject.skip(n.intValue()));
        } catch (java.io.IOException e) {
            throw new VDMSpecException(new JavaIoIOException());
        }
    }


    protected void finalize() {
        //wrappedObject.finalize();
    }


    public Integer available() throws CGException {
        try {
            return new Integer(wrappedObject.available());
        } catch (java.io.IOException e) {
            throw new VDMSpecException(new JavaIoIOException());
        }
    }


    public Tuple readMIIIntInt_q(final HashMap b_u,
                                 final Integer off,
                                 final Integer len_u_u) throws CGException {
        return readBytes(b_u, off, len_u_u);
    }


    private Tuple readBytes(final HashMap b_u,
                            final Integer off,
                            final Integer len_u_u) throws CGException {
        Tuple tupVal = readBytes_q(len_u_u);
        Integer l_n = (Integer) tupVal.GetField(1);
        HashMap l_b = (HashMap) tupVal.GetField(2);
        int l_len;
        if (l_n.intValue() < len_u_u.intValue())
            l_len = l_n.intValue();
        else
            l_len = len_u_u.intValue();
        HashMap b = b_u;
        for (int i = off.intValue(); i <= off.intValue() + l_len - 1;
             i++)
            b.put(new Integer(i), l_b.get(new Integer(i - off.intValue())));
        Tuple result = new Tuple(2);
        result.SetField(1, l_n);
        result.SetField(2, b);
        return result;
    }


    private Tuple readBytes_q(final Integer l) throws CGException {
        try {
            HashMap bytes = new HashMap();
            int lval = l.intValue();
            int bytev = 0;
            for (int i = 0; (i < lval) && (bytev != -1); i++) {
                bytev = wrappedObject.read();
                if (bytev != -1)
                    bytes.put(new Integer(i), new Integer(bytev));
            }
            Tuple result = new Tuple(2);
            result.SetField(1, new Integer(bytes.size()));
            result.SetField(2, bytes);
            return result;
        } catch (java.io.IOException e) {
            throw new VDMSpecException(new JavaIoIOException());
        }
    }


    public JavaIoFileInputStream(final JavaIoFile file) throws CGException {
        try {
            wrappedObject = new java.io.FileInputStream(file.getWrappedObject());
        } catch (java.io.FileNotFoundException e) {
            throw new VDMSpecException(new JavaIoFileNotFoundException());
        }
    }

    public JavaIoFileInputStream(final JavaLangString name)
            throws CGException {
        try {
            wrappedObject = new java.io.FileInputStream(name.toSeqOfChar());
        } catch (java.io.FileNotFoundException e) {
            throw new VDMSpecException(new JavaIoFileNotFoundException());
        }
    }

    public JavaIoFileInputStream(final String name) throws CGException {
        try {
            wrappedObject = new java.io.FileInputStream(name);
        } catch (java.io.FileNotFoundException e) {
            throw new VDMSpecException(new JavaIoFileNotFoundException());
        }
    }

    public JavaIoFileInputStream(final JavaIoFileDescriptor fdObj)
            throws CGException {
        wrappedObject = new java.io.FileInputStream(fdObj.getWrappedObject());
    }
}

