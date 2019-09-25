package jp.vdmtools.VDM.jdk;

public class JavaLangIllegalArgumentException extends JavaLangRuntimeException {

    public JavaLangIllegalArgumentException() {
    }


    public JavaLangIllegalArgumentException(final quotes.NIL var_1_1) {
        this();
        isNil = Boolean.TRUE;
    }


    public JavaLangIllegalArgumentException(final JavaLangString s) {
        super(s);
    }

}
