package jp.vdmtools.VDM.jdk;



public class JavaAwtImage extends JavaLangObject {

    private static final JavaLangClass CLASS =
            new JavaLangClass(new String("JavaAwtImage"),
                    new quotes.CLASS(),
                    Boolean.FALSE);

    public JavaAwtImage(final quotes.NIL n) {
        isNil = Boolean.TRUE;
    }

    public JavaAwtImage() {
    }

    public JavaLangClass getJClass() {
        return CLASS;
    }
}
