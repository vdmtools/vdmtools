package jp.vdmtools.VDM.jdk;

import jp.vdmtools.VDM.CGException;

public interface JavaUtilMap extends Nullable {

    abstract public Integer size() throws CGException;

    abstract public void clear() throws CGException;

    abstract public JavaUtilSet keySet() throws CGException;

    abstract public Boolean isEmpty() throws CGException;

    abstract public JavaLangObject get(final JavaLangObject key) throws CGException;

    abstract public Boolean containsKey(final JavaLangObject key) throws CGException;

    abstract public JavaLangObject put(final JavaLangObject key,
                                       final JavaLangObject value_u_u) throws CGException;

    abstract public Boolean containsValue(final JavaLangObject value_u_u) throws CGException;
}

