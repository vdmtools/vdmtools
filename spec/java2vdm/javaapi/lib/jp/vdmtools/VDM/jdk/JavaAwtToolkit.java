package jp.vdmtools.VDM.jdk;

import java.util.HashMap;

public class JavaAwtToolkit extends JavaLangObject {

    private static final JavaLangClass CLASS =
            new JavaLangClass(new String("JavaAwtToolkit"),
                    new quotes.CLASS(),
                    Boolean.FALSE);

    public JavaAwtToolkit() {
    }


    public JavaLangClass getJClass() {
        return CLASS;
    }

    public JavaAwtToolkit(final quotes.NIL n) {
        isNil = Boolean.TRUE;
    }


    public JavaAwtImage createImage(final HashMap b) {
        return new JavaAwtImage(new quotes.NIL());
    }


    static public JavaAwtToolkit getDefaultToolkit() {
        return new JavaAwtToolkit(new quotes.NIL());
    }

}
