package jp.vdmtools.VDM.jdk;

import jp.vdmtools.VDM.VDMSpecException;

public class JavaIoFileDescriptor extends JavaLangObject {

    private java.io.FileDescriptor wrappedObject;

    public java.io.FileDescriptor getWrappedObject(){
        return wrappedObject;
    }

    private static final JavaLangClass CLASS = new JavaLangClass(new String("JavaIoFileDescriptor"),
            new quotes.CLASS(),
            Boolean.FALSE);


    public Boolean valid() {
        return Boolean.valueOf(wrappedObject.valid());
    }


    public void sync_u_u() throws VDMSpecException {
        try {
            wrappedObject.sync();
        } catch (java.io.SyncFailedException e) {
            throw new VDMSpecException(e);
        }
    }


    public JavaIoFileDescriptor() {
        wrappedObject = new java.io.FileDescriptor();
    }


    public JavaIoFileDescriptor(final quotes.NIL n) {
        this();
        isNil = Boolean.TRUE;
    }

    // Needed by JavaIoFileInputStream
    public JavaIoFileDescriptor(final java.io.FileDescriptor fd) {
        wrappedObject = fd;
    }

//    public JavaLangClass getClass () {
//	return CLASS;
//    }
}
