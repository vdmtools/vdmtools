package jp.vdmtools.VDM.jdk;

import java.util.HashMap;

public class JavaAwtToolkit extends JavaLangObject {

    private static final JavaLangClass CLASS =
            new JavaLangClass(new String("JavaAwtToolkit"),
                    new quotes.CLASS(),
                    new Boolean(false));

    public JavaAwtToolkit() {
    }


    public JavaLangClass getJClass() {
        return CLASS;
    }

    public JavaAwtToolkit(final quotes.NIL n) {
        isNil = new Boolean(true);
    }


    public JavaAwtImage createImage(final HashMap b) {
        return new JavaAwtImage(new quotes.NIL());
    }


    static public JavaAwtToolkit getDefaultToolkit() {
        return new JavaAwtToolkit(new quotes.NIL());
    }

}
