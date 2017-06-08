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

// ***** VDMTOOLS END Name=imports


public class HMKeySet extends JavaUtilAbstractSet {

// ***** VDMTOOLS START Name=vdmComp KEEP=NO
    static UTIL.VDMCompare vdmComp = new UTIL.VDMCompare();
// ***** VDMTOOLS END Name=vdmComp

// ***** VDMTOOLS START Name=hm KEEP=NO
    private JavaUtilHashMap hm = null;
// ***** VDMTOOLS END Name=hm


// ***** VDMTOOLS START Name=HMKeySet KEEP=NO
    public HMKeySet() throws CGException {
    }
// ***** VDMTOOLS END Name=HMKeySet

    public Integer jHashCode(){
	return new Integer(hashCode());
    }

    public Boolean add (JavaLangObject o) {
	System.err.println("WARNING: HMKeySet.add not defined");
	return new Boolean(false);
    }

    public java.util.HashMap toArray(){
	System.err.println("WARNING: HMKeySet.toArray not defined");
	return new java.util.HashMap();
    }

    public java.util.HashMap toArray(java.util.HashMap m){
	System.err.println("WARNING: HMKeySet.toArray not defined");
	return new java.util.HashMap();
    }

    public Boolean containsAll(final JavaUtilCollection c){
	System.err.println("WARNING: HMKeySet.containsAll not defined");
	return new Boolean(false);
    }

    public Boolean retainAll(final JavaUtilCollection c){
	System.err.println("WARNING: HMKeySet.retainAll not defined");
	return new Boolean(false);
    }

    public Boolean removeAll(final JavaUtilCollection c){
	System.err.println("WARNING: HMKeySet.removeAll not defined");
	return new Boolean(false);
    }

    public Boolean addAll(final JavaUtilCollection c){
	System.err.println("WARNING: HMKeySet.addAll not defined");
	return new Boolean(false);
    }

    public JavaLangString toJString(){
	System.err.println("WARNING: HMKeySet.toJString not defined");
	return new JavaLangString("");
    }

// ***** VDMTOOLS START Name=HMKeySet KEEP=NO
    public HMKeySet(final JavaUtilHashMap hm_1) throws CGException {
        this();
        ((HMKeySet) this).hm = (JavaUtilHashMap) UTIL.clone(hm_1);
    }
// ***** VDMTOOLS END Name=HMKeySet


// ***** VDMTOOLS START Name=iterator KEEP=NO
    public JavaUtilIterator iterator() throws CGException {
        JavaUtilIterator rexpr_1 = null;
        rexpr_1 = (JavaUtilIterator) hm.getHashIterator(JavaUtilHashMap.KEYS);
        return (JavaUtilIterator) rexpr_1;
    }
// ***** VDMTOOLS END Name=iterator


// ***** VDMTOOLS START Name=size KEEP=NO
    public Integer size() throws CGException {
        Integer rexpr_1 = null;
        rexpr_1 = hm.count;
        return rexpr_1;
    }
// ***** VDMTOOLS END Name=size


// ***** VDMTOOLS START Name=contains KEEP=NO
    public Boolean contains(final JavaLangObject value_u_u) throws CGException {
        Boolean rexpr_2 = null;
        rexpr_2 = hm.containsKey((JavaLangObject) value_u_u);
        return rexpr_2;
    }
// ***** VDMTOOLS END Name=contains


// ***** VDMTOOLS START Name=remove KEEP=NO
    public Boolean remove(final JavaLangObject key) throws CGException {
        Boolean rexpr_2 = null;
        Boolean unArg_3 = null;
        JavaLangObject obj_4 = null;
        obj_4 = (JavaLangObject) hm.remove((JavaLangObject) key);
        unArg_3 = obj_4.IsNil();
        rexpr_2 = new Boolean(!unArg_3.booleanValue());
        return rexpr_2;
    }
// ***** VDMTOOLS END Name=remove


// ***** VDMTOOLS START Name=clear KEEP=NO
    public void clear() throws CGException {
        hm.clear();
    }
// ***** VDMTOOLS END Name=clear

}

;
