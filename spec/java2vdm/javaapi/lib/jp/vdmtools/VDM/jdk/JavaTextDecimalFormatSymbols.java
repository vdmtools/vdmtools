package jp.vdmtools.VDM.jdk;

import jp.vdmtools.VDM.CGException;
import jp.vdmtools.VDM.UTIL;

public class JavaTextDecimalFormatSymbols extends JavaLangObject
        implements JavaLangCloneable, JavaIoSerializable {


    private static Integer serialVersionUID = new Integer(0);
    private static final JavaLangClass CLASS = (JavaLangClass) new JavaLangClass(new String("JavaTextDecimalFormatSymbols"), new quotes.CLASS(), new Boolean(false));

    public JavaLangClass getJClass() throws CGException {
        return (JavaLangClass) CLASS;
    }

    public JavaTextDecimalFormatSymbols() throws CGException {
        ;
    }

    public JavaTextDecimalFormatSymbols(final quotes.NIL var_1_1) throws CGException {
        this();
        isNil = (Boolean) UTIL.clone(new Boolean(true));
    }

    public JavaTextDecimalFormatSymbols(final JavaUtilLocale locale) throws CGException {
        this();
    }
}
