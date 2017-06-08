package jp.vdmtools.VDM.jdk;

import jp.vdmtools.VDM.CGException;
import jp.vdmtools.VDM.UTIL;


public class JavaLangConversionBufferFullException extends JavaLangRuntimeException {

    public JavaLangConversionBufferFullException() throws CGException {
	super();
    }


    public JavaLangConversionBufferFullException(final quotes.NIL var_1_1) throws CGException {
        this();
        isNil = (Boolean) UTIL.clone(new Boolean(true));
    }


    public JavaLangConversionBufferFullException(final JavaLangString s) throws CGException {
        super(s);
    }

}

