package jp.vdmtools.VDM.jdk;



import jp.vdmtools.VDM.CGException;
import jp.vdmtools.VDM.UTIL;

import java.util.HashMap;




public class JavaUtilLocale extends JavaLangObject implements JavaLangCloneable, JavaIoSerializable {

    public static JavaUtilLocale ENGLISH = null;
    public static JavaUtilLocale FRENCH = null;
    public static JavaUtilLocale GERMAN = null;
    public static JavaUtilLocale ITALIAN = null;
    public static JavaUtilLocale JAPANESE = null;
    public static JavaUtilLocale KOREAN = null;
    public static JavaUtilLocale CHINESE = null;
    public static JavaUtilLocale SIMPLIFIED_uCHINESE = null;
    public static JavaUtilLocale TRADITIONAL_uCHINESE = null;
    public static JavaUtilLocale FRANCE = null;
    public static JavaUtilLocale GERMANY = null;
    public static JavaUtilLocale ITALY = null;
    public static JavaUtilLocale JAPAN = null;
    public static JavaUtilLocale KOREA = null;
    public static JavaUtilLocale CHINA = null;
    public static JavaUtilLocale PRC = null;
    public static JavaUtilLocale TAIWAN = null;
    public static JavaUtilLocale UK = null;
    public static JavaUtilLocale US = null;
    public static JavaUtilLocale CANADA = null;
    public static JavaUtilLocale CANADA_uFRENCH = null;
    private static Integer serialVersionUID = new Integer(0);

    public JavaUtilLocale() throws CGException {

            ENGLISH = null;
            FRENCH = null;
            GERMAN = null;
            ITALIAN = null;
            JAPANESE = null;
            KOREAN = null;
            CHINESE = null;
            SIMPLIFIED_uCHINESE = null;
            TRADITIONAL_uCHINESE = null;
            FRANCE = null;
            GERMANY = null;
            ITALY = null;
            JAPAN = null;
            KOREA = null;
            CHINA = null;
            PRC = null;
            TAIWAN = null;
            UK = null;
            US = null;
            CANADA = null;
            CANADA_uFRENCH = null;
            serialVersionUID = new Integer(0);

    }

    public static JavaUtilLocale getDefault() throws CGException {
	UTIL.RunTime("JavaUtilLocale.getDefault not defined");
	return null;
    }

    public JavaUtilLocale(final quotes.NIL var_1_1) throws CGException {
        this();
        isNil = (Boolean) UTIL.clone(new Boolean(true));
    }


    public JavaUtilLocale(final JavaLangString language, final JavaLangString country) throws CGException {
        this();
    }

    public JavaUtilLocale(final JavaLangString language, final JavaLangString country, final JavaLangString variant) throws CGException {
        this();
    }
}

