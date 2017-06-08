package jp.vdmtools.VDM.jdk;

import jp.vdmtools.VDM.CGException;

public interface JavaUtilListIterator extends JavaUtilIterator {

    abstract public JavaLangObject next() throws CGException;

    abstract public void remove() throws CGException;

    abstract public Boolean hasNext() throws CGException;

    abstract public JavaLangObject previous() throws CGException;

    abstract public Integer nextIndex() throws CGException;

    abstract public void add(final JavaLangObject o) throws CGException;

    abstract public Boolean hasPrevious() throws CGException;

    abstract public Integer previousIndex() throws CGException;

    abstract public void set_u_u(final JavaLangObject o) throws CGException;
}

