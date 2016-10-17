package jp.vdmtools.VDM.jdk;

public class JavaLangIllegalArgumentException extends JavaLangRuntimeException {

    public JavaLangIllegalArgumentException() {
    }


    public JavaLangIllegalArgumentException(final quotes.NIL var_1_1) {
        this();
        isNil = new Boolean(true);
    }


    public JavaLangIllegalArgumentException(final JavaLangString s) {
        super(s);
    }

}
