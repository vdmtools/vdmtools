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


public abstract class JavaUtilAbstractCollection extends JavaLangObject implements JavaUtilCollection {

// ***** VDMTOOLS START Name=vdmComp KEEP=NO
    static UTIL.VDMCompare vdmComp = new UTIL.VDMCompare();
// ***** VDMTOOLS END Name=vdmComp

// ***** VDMTOOLS START Name=CLASS KEEP=NO
    private static final JavaLangClass CLASS = (JavaLangClass) new JavaLangClass(new String("JavaUtilAbstractList"), new quotes.CLASS(), Boolean.FALSE);
// ***** VDMTOOLS END Name=CLASS


// ***** VDMTOOLS START Name=JavaUtilAbstractCollection KEEP=NO
    public JavaUtilAbstractCollection() throws CGException {
    }
// ***** VDMTOOLS END Name=JavaUtilAbstractCollection


// ***** VDMTOOLS START Name=getClass KEEP=NO
    public JavaLangClass getJClass() throws CGException {
        return (JavaLangClass) CLASS;
    }
// ***** VDMTOOLS END Name=getClass


// ***** VDMTOOLS START Name=clear KEEP=NO
    abstract public void clear() throws CGException;
// ***** VDMTOOLS END Name=clear


// ***** VDMTOOLS START Name=isEmpty KEEP=NO
    public Boolean isEmpty() throws CGException {
        return Boolean.valueOf(size().intValue() == Integer.valueOf(0).intValue());
    }
// ***** VDMTOOLS END Name=isEmpty


// ***** VDMTOOLS START Name=toArray KEEP=NO
    abstract public HashMap toArray() throws CGException;
// ***** VDMTOOLS END Name=toArray


// ***** VDMTOOLS START Name=toString KEEP=NO
    abstract public JavaLangString toJString() throws CGException;
// ***** VDMTOOLS END Name=toString


// ***** VDMTOOLS START Name=contains KEEP=NO
    abstract public Boolean contains(final JavaLangObject o) throws CGException;
// ***** VDMTOOLS END Name=contains


// ***** VDMTOOLS START Name=addAll KEEP=NO
    abstract public Boolean addAll(final JavaUtilCollection c) throws CGException;
// ***** VDMTOOLS END Name=addAll


// ***** VDMTOOLS START Name=removeAll KEEP=NO
    abstract public Boolean removeAll(final JavaUtilCollection c) throws CGException;
// ***** VDMTOOLS END Name=removeAll


// ***** VDMTOOLS START Name=retainAll KEEP=NO
    abstract public Boolean retainAll(final JavaUtilCollection c) throws CGException;
// ***** VDMTOOLS END Name=retainAll


// ***** VDMTOOLS START Name=JavaUtilAbstractCollection KEEP=NO
    public JavaUtilAbstractCollection(final quotes.NIL var_1_1) throws CGException {
        this();
        isNil = Boolean.TRUE;
    }
// ***** VDMTOOLS END Name=JavaUtilAbstractCollection

}

;
