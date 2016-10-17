package jp.vdmtools.VDM.jdk;

public class JavaLangRuntimeException extends JavaLangException {

    public JavaLangRuntimeException() {
    }


    public JavaLangRuntimeException(final quotes.NIL var_1_1) {
        this();
        isNil = new Boolean(true);
    }


    public JavaLangRuntimeException(final JavaLangString s) {
        super(s);
    }

}

