//
// THIS FILE IS AUTOMATICALLY GENERATED!!
//
// Generated at Tue 30-Oct-2001 by the VDM++ JAVA Code Generator
// (v6.7.19 - Mon 29-Oct-2001)
//
// Supported compilers:
// jdk1.3
//

// ***** VDMTOOLS START Name=HeaderComment KEEP=NO

// ***** VDMTOOLS END Name=HeaderComment

// ***** VDMTOOLS START Name=package KEEP=NO
package jp.vdmtools.VDM.jdk;

// ***** VDMTOOLS END Name=package


// ***** VDMTOOLS START Name=imports KEEP=NO

import jp.vdmtools.VDM.CGException;
import jp.vdmtools.VDM.UTIL;

import java.util.HashMap;

// ***** VDMTOOLS END Name=imports


public class JavaUtilResourceBundle extends JavaLangObject {

// ***** VDMTOOLS START Name=vdmComp KEEP=NO
    static UTIL.VDMCompare vdmComp = new UTIL.VDMCompare();
// ***** VDMTOOLS END Name=vdmComp

// ***** VDMTOOLS START Name=parent KEEP=NO
    protected JavaUtilResourceBundle parent = null;
// ***** VDMTOOLS END Name=parent



// ***** VDMTOOLS START Name=JavaUtilResourceBundle KEEP=NO
    public JavaUtilResourceBundle() throws CGException {
        ;
    }
// ***** VDMTOOLS END Name=JavaUtilResourceBundle


// ***** VDMTOOLS START Name=getBundle KEEP=NO
    static public JavaUtilResourceBundle getBundle(final JavaLangString baseName) throws CGException {
        return (JavaUtilResourceBundle) new JavaUtilResourceBundle(new quotes.NIL());
    }
// ***** VDMTOOLS END Name=getBundle



// ***** VDMTOOLS START Name=getString KEEP=NO
    public JavaLangString getString(final JavaLangString key) throws CGException {
        return (JavaLangString) new JavaLangString(new quotes.NIL());
    }
// ***** VDMTOOLS END Name=getString


// ***** VDMTOOLS START Name=JavaUtilResourceBundle KEEP=NO
    public JavaUtilResourceBundle(final quotes.NIL var_1_1) throws CGException {
        this();
        isNil = (Boolean) UTIL.clone(new Boolean(true));
    }
// ***** VDMTOOLS END Name=JavaUtilResourceBundle



}

