package jp.vdmtools.VDM.jdk;



public class JavaAwtImage extends JavaLangObject {

    private static final JavaLangClass CLASS =
            new JavaLangClass(new String("JavaAwtImage"),
                    new quotes.CLASS(),
                    new Boolean(false));

    public JavaAwtImage(final quotes.NIL n) {
        isNil = new Boolean(true);
    }

    public JavaAwtImage() {
    }

    public JavaLangClass getJClass() {
        return CLASS;
    }
}
