package jp.vdmtools.VDM.jdk;

import java.util.HashMap;
import jp.vdmtools.VDM.CGException;

public interface JavaUtilSet extends JavaUtilCollection {

    abstract public void clear() throws CGException;

    abstract public Boolean isEmpty() throws CGException;

    abstract public HashMap toArray() throws CGException;

    abstract public Integer jHashCode() throws CGException;

    abstract public JavaUtilIterator iterator() throws CGException;

    abstract public Boolean add(final JavaLangObject o) throws CGException;

    abstract public Boolean equals(final JavaLangObject o) throws CGException;

    abstract public Boolean contains(final JavaLangObject o) throws CGException;

    abstract public Boolean addAll(final JavaUtilCollection c) throws CGException;

    abstract public HashMap toArray(final HashMap a) throws CGException;

    abstract public Boolean removeAll(final JavaUtilCollection c) throws CGException;

    abstract public Boolean retainAll(final JavaUtilCollection c) throws CGException;

    abstract public Boolean containsAll(final JavaUtilCollection c) throws CGException;
}

