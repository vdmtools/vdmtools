package jp.vdmtools.VDM.jdk;

import jp.vdmtools.VDM.CGException;
import jp.vdmtools.VDM.UTIL;

public class JavaTextSimpleDateFormat extends JavaTextDateFormat {

    static UTIL.VDMCompare vdmComp = new UTIL.VDMCompare();

    private static final JavaLangClass CLASS = 
	new JavaLangClass(new String("JavaTextSimpleDateFormat"), 
			  new quotes.CLASS(), 
			  new Boolean(false));


    public JavaTextSimpleDateFormat() throws CGException {
    }


    public JavaLangClass getJClass() throws CGException {
        return (JavaLangClass) CLASS;
    }


    public JavaTextSimpleDateFormat(final quotes.NIL var_1_1) 
	throws CGException 
    {
        this();
        isNil = (Boolean) UTIL.clone(new Boolean(true));
    }


    public JavaTextSimpleDateFormat(final JavaLangString pattern) 
	throws CGException 
    {
        this();
    }

    public JavaLangObject parseObject(JavaLangString s, 
				      JavaTextParsePosition p)
    {
	return null;
    }

}

;
