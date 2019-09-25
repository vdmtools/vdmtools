package jp.vdmtools.VDM.jdk;

import jp.vdmtools.VDM.CGException;
import jp.vdmtools.VDM.UTIL;

public class JavaTextDecimalFormatSymbols extends JavaLangObject
        implements JavaLangCloneable, JavaIoSerializable {


    private static Integer serialVersionUID = Integer.valueOf(0);
    private static final JavaLangClass CLASS = (JavaLangClass) new JavaLangClass(new String("JavaTextDecimalFormatSymbols"), new quotes.CLASS(), Boolean.FALSE);

    public JavaLangClass getJClass() throws CGException {
        return (JavaLangClass) CLASS;
    }

    public JavaTextDecimalFormatSymbols() throws CGException {
        ;
    }

    public JavaTextDecimalFormatSymbols(final quotes.NIL var_1_1) throws CGException {
        this();
        isNil = Boolean.TRUE;
    }

    public JavaTextDecimalFormatSymbols(final JavaUtilLocale locale) throws CGException {
        this();
    }
}
