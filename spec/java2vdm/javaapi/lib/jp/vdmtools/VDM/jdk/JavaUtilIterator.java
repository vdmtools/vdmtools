package jp.vdmtools.VDM.jdk;

import jp.vdmtools.VDM.CGException;

public interface JavaUtilIterator extends Nullable {

    abstract public JavaLangObject next() throws CGException;

    abstract public void remove() throws CGException;

    abstract public Boolean hasNext() throws CGException;
}

