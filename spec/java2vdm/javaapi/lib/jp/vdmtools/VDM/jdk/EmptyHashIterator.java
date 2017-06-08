

package jp.vdmtools.VDM.jdk;


import jp.vdmtools.VDM.CGException;
import jp.vdmtools.VDM.UTIL;
import jp.vdmtools.VDM.VDMSpecException;


public class EmptyHashIterator extends JavaLangObject implements JavaUtilIterator {

    static UTIL.VDMCompare vdmComp = new UTIL.VDMCompare();


    public JavaLangObject next() throws CGException {
        throw new VDMSpecException(new JavaUtilNoSuchElementException(new JavaLangString(new String("JavaUtilHashMap JavaUtilIterator"))));
    }


    public void remove() throws CGException {
        throw new VDMSpecException(new JavaLangIllegalStateException(new JavaLangString(new String("JavaUtilHashMap JavaUtilIterator"))));
    }


    public Boolean hasNext() throws CGException {
        return new Boolean(false);
    }


    public EmptyHashIterator() throws CGException {
    }

}
