package jp.vdmtools.VDM.jdk;

import jp.vdmtools.VDM.CGException;
import jp.vdmtools.VDM.VDMSpecException;

import java.io.FileOutputStream;
import java.io.IOException;

public class JavaIoFileOutputStream extends JavaIoOutputStream {

    FileOutputStream wrappedObject;

    public JavaIoFileOutputStream() {
        //wrappedObject = new FileOutputStream();
    }


    public void close() throws CGException {
        try {
          wrappedObject.close();
        }
        catch (IOException e){
            throw new VDMSpecException(new JavaIoIOException());
        }
    }

    public JavaIoFileDescriptor getFD() throws CGException {
        try {
            return new JavaIoFileDescriptor(wrappedObject.getFD());
        }
        catch (IOException e){
            throw new VDMSpecException(new JavaIoIOException());
        }
    }


    public void write(final Integer b) throws CGException {
        try {
            wrappedObject.write(b.intValue());
        } catch (java.io.IOException e) {
            throw new VDMSpecException(new JavaIoIOException());
        }
    }


//    protected void finalize() throws CGException {
//	try {
//	    wrappedObject.finalize();
//	}
//	catch (java.io.IOException e){
//	    throw new VDMSpecException(new JavaIoIOException());
//	}
//    }


    public JavaIoFileOutputStream(final JavaIoFile file) throws CGException {
        try {
            wrappedObject = new FileOutputStream(file.getWrappedObject());
        } catch (java.io.FileNotFoundException e) {
            throw new VDMSpecException(new JavaIoFileNotFoundException());
        }
    }


    public JavaIoFileOutputStream(final JavaLangString filename)
            throws CGException {
        try {
            wrappedObject = new FileOutputStream(filename.toSeqOfChar());
        } catch (java.io.FileNotFoundException e) {
            throw new VDMSpecException(new JavaIoFileNotFoundException());
        }
    }


    public JavaIoFileOutputStream(final JavaLangString name,
                                  final Boolean append) throws CGException {
        try {
            wrappedObject = new FileOutputStream(name.toSeqOfChar(),
                    append.booleanValue());
        } catch (java.io.FileNotFoundException e) {
            throw new VDMSpecException(new JavaIoFileNotFoundException());
        }
    }


    public JavaIoFileOutputStream(final String name,
                                  final Boolean append) throws CGException {
        try {
            wrappedObject = new FileOutputStream(name,
                    append.booleanValue());
        } catch (java.io.FileNotFoundException e) {
            throw new VDMSpecException(new JavaIoFileNotFoundException());
        }
    }


    public JavaIoFileOutputStream(final JavaIoFileDescriptor fdObj) {
        wrappedObject = new FileOutputStream(fdObj.getWrappedObject());
    }
}
