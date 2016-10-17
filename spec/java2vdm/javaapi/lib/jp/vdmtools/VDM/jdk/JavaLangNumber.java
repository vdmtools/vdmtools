package jp.vdmtools.VDM.jdk;

import jp.vdmtools.VDM.CGException;

public interface JavaLangNumber extends JavaIoSerializable {


    public abstract Integer intValue() throws CGException;

    public abstract Integer byteValue() throws CGException;

    public abstract Integer longValue() throws CGException;

    public abstract Double floatValue() throws CGException;

    public abstract Integer shortValue() throws CGException;

    public abstract Double doubleValue() throws CGException;
}
