

package jp.vdmtools.VDM.jdk;


import jp.vdmtools.VDM.CGException;


public class JavaLangThrowable extends JavaLangObject implements JavaIoSerializable {

    private Throwable wrappedObject;

    public Throwable getWrappedObject(){
        return wrappedObject;
    }

    public JavaLangString toJString() throws CGException {
        return new JavaLangString(wrappedObject.getMessage());
    }


    public JavaLangThrowable() {
        wrappedObject = new Throwable();
    }

    public JavaLangThrowable(Throwable t) {
        wrappedObject = t;
    }


    public JavaLangString getMessage() throws CGException {
        return new JavaLangString(wrappedObject.getMessage());
    }


    public JavaLangThrowable(final quotes.NIL var_1_1) {
        this();
        isNil = new Boolean(true);
    }


    public void printStackTrace() throws CGException {
        wrappedObject.printStackTrace();
    }


    public JavaLangThrowable fillInStackTrace() throws CGException {
        return new JavaLangThrowable(wrappedObject.fillInStackTrace());
    }


    public JavaLangThrowable(final JavaLangString message) {
        wrappedObject = new Throwable(message.toSeqOfChar());
    }


    public JavaLangString getLocalizedMessage() throws CGException {
        return new JavaLangString(wrappedObject.getLocalizedMessage());
    }


    public void printStackTrace(final JavaIoPrintStream s) throws CGException {
        s.println(new JavaLangString(new String("printStackTrace: is not implemented at VDM++")));
    }


    public void printStackTrace(final JavaIoPrintWriter s) throws CGException {
        s.println(new JavaLangString(new String("printStackTrace: is not implemented at VDM++")));
    }

}

