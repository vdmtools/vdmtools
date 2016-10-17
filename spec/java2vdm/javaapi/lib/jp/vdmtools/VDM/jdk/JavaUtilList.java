package jp.vdmtools.VDM.jdk;

import java.util.HashMap;
import jp.vdmtools.VDM.CGException;

public interface JavaUtilList extends JavaUtilCollection {

    abstract public JavaLangObject get(final Integer index) throws CGException;

    abstract public Integer size() ;

    abstract public void clear() throws CGException;

    abstract public Boolean isEmpty();

    abstract public HashMap toArray();

    abstract public Integer vdm_hashCode() throws CGException;

    abstract public JavaUtilIterator iterator() throws CGException;

    abstract public JavaUtilList subList(final Integer fromIndex, final Integer toIndex) throws CGException;

    abstract public Boolean vdm_equals(final JavaLangObject o) throws CGException;

    abstract public Integer indexOf(final JavaLangObject o) throws CGException;

    abstract public JavaLangObject set_u_u(final Integer index, final JavaLangObject element) throws CGException;

    abstract public Boolean contains(final JavaLangObject o);

    abstract public Boolean addAll(final JavaUtilCollection c) throws CGException;

    abstract public Integer lastIndexOf(final JavaLangObject o) throws CGException;

    abstract public HashMap toArray(final HashMap a);

    abstract public Boolean addAll(final Integer index, final JavaUtilCollection c) throws CGException;

    abstract public Boolean removeAll(final JavaUtilCollection c) throws CGException;

    abstract public Boolean retainAll(final JavaUtilCollection c) throws CGException;

    abstract public Boolean containsAll(final JavaUtilCollection c) throws CGException;
}

