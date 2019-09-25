package jp.vdmtools.VDM.jdk;



public class JavaLangClassCastException extends JavaLangRuntimeException {

    public JavaLangClassCastException() {
    }


    public JavaLangClassCastException(final quotes.NIL var_1_1) {
        this();
        isNil = Boolean.TRUE;
    }


    public JavaLangClassCastException(final JavaLangString s) {
        super(s);
    }

}

