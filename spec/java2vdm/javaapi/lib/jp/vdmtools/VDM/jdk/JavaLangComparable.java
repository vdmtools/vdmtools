package jp.vdmtools.VDM.jdk;

import jp.vdmtools.VDM.CGException;

public interface JavaLangComparable extends Nullable {
    abstract public Integer compareTo(final JavaLangObject o) throws CGException;
}
