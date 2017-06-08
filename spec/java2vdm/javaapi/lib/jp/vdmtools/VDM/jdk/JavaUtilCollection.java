package jp.vdmtools.VDM.jdk;

import jp.vdmtools.VDM.CGException;

public interface JavaUtilCollection extends Nullable {


    abstract public Integer size() throws CGException;

    abstract public void clear() throws CGException;

    abstract public Boolean isEmpty() throws CGException;

//    abstract public Integer hashCode() throws CGException;

    abstract public JavaUtilIterator iterator() throws CGException;

    abstract public Boolean vdm_equals(final JavaLangObject o) throws CGException;

    abstract public Boolean contains(final JavaLangObject o) throws CGException;

    abstract public Boolean addAll(final JavaUtilCollection c) throws CGException;

    abstract public Boolean removeAll(final JavaUtilCollection c) throws CGException;

    abstract public Boolean retainAll(final JavaUtilCollection c) throws CGException;
}

