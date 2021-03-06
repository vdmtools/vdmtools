//
// THIS FILE IS AUTOMATICALLY GENERATED!!
//
// Generated at Tue 30-Oct-2001 by the VDM++ JAVA Code Generator
// (v6.7.19 - Mon 29-Oct-2001)
//
// Supported compilers:
// jdk1.3
//

// ***** VDMTOOLS START Name=HeaderComment KEEP=NO

// ***** VDMTOOLS END Name=HeaderComment

// ***** VDMTOOLS START Name=package KEEP=NO
package jp.vdmtools.VDM.jdk;

// ***** VDMTOOLS END Name=package


// ***** VDMTOOLS START Name=imports KEEP=NO

import jp.vdmtools.VDM.CGException;
import jp.vdmtools.VDM.VDMSpecException;

import java.io.FileWriter;
import java.io.IOException;

// ***** VDMTOOLS END Name=imports


public class JavaIoFileWriter extends JavaIoOutputStreamWriter {

    private FileWriter wrappedObject;

    public FileWriter getWrappedObject() {
        return wrappedObject;
    }

    public JavaIoFileWriter() throws CGException {
    }

    public JavaIoFileWriter(final quotes.NIL var_1_1) throws CGException {
        this();
        isNil = Boolean.TRUE;
    }

    public JavaIoFileWriter(final JavaLangString fileName) throws CGException {
        try {
            wrappedObject = new FileWriter(fileName.getWrappedObject());
        } catch (IOException e) {
            throw new VDMSpecException(new JavaIoIOException());
        }
    }

    public JavaIoFileWriter(final JavaLangString fileName, final Boolean append) throws CGException {
        try {
            wrappedObject = new FileWriter(fileName.getWrappedObject(), append.booleanValue());
        } catch (IOException e) {
            throw new VDMSpecException(new JavaIoIOException());
        }
    }

    public JavaIoFileWriter(final JavaIoFileDescriptor fd) throws CGException {
        wrappedObject = new FileWriter(fd.getWrappedObject());
    }

    public JavaIoFileWriter(final JavaIoFile f) throws CGException {
        try {
            wrappedObject = new FileWriter(f.getWrappedObject());
        } catch (IOException e) {
            throw new VDMSpecException(new JavaIoIOException());
        }
    }

}

;
