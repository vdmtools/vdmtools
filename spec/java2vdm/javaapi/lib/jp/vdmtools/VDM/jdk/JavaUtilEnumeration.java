package jp.vdmtools.VDM.jdk;

import jp.vdmtools.VDM.CGException;

public interface JavaUtilEnumeration extends Nullable {

    abstract public JavaLangObject nextElement() throws CGException;

    abstract public Boolean hasMoreElements() throws CGException;
}

