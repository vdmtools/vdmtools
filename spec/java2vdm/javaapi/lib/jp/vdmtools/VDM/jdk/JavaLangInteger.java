package jp.vdmtools.VDM.jdk;

import jp.vdmtools.VDM.CGException;
import jp.vdmtools.VDM.VDMSpecException;

public class JavaLangInteger extends JavaLangObject 
    implements JavaLangNumber, JavaLangComparable 
{

    public static Integer MAX_uVALUE = Integer.valueOf(Integer.MAX_VALUE);
    public static final Integer MIN_uVALUE = Integer.valueOf(Integer.MIN_VALUE);

    public static final JavaLangClass CLASS =
            new JavaLangClass(new String("JavaLangInteger"),
                    new quotes.CLASS(),
                    Boolean.FALSE);
    public static final JavaLangClass TYPE =
            new JavaLangClass(new String("int"),
                    new quotes.CLASS(),
                    Boolean.TRUE);
    private Integer wrappedObject;

    public JavaLangInteger() {
        wrappedObject = Integer.valueOf(0);
    }

    public Integer getWrappedObject() {
        return wrappedObject;
    }

    public JavaLangInteger(final Integer value_u_u) {
        wrappedObject = value_u_u;
    }


    public JavaLangInteger(final Character value_u_u) {
        wrappedObject = Integer.valueOf((int) value_u_u.charValue());
    }

    public JavaLangInteger(final JavaLangString s) throws CGException {
        wrappedObject = Integer.valueOf(s.toSeqOfChar());
    }

    public JavaLangClass getJClass() {
        return CLASS;
    }


    public JavaLangClass vdm_getClass() {
	return new JavaLangClass(new String("JavaLangInteger"),
				 new quotes.CLASS(),
				 Boolean.FALSE);
    }


    public Integer jHashCode() {
        return Integer.valueOf(wrappedObject.hashCode());
    }

    public Integer vdm_HashCode() {
        return Integer.valueOf(wrappedObject.hashCode());
    }

    public Integer intValue() {
        return Integer.valueOf(wrappedObject.intValue());
    }


    public JavaLangString toJString() throws CGException {
        return new JavaLangString(wrappedObject.toString());
    }

    public JavaLangString vdm_toString() throws CGException {
        return new JavaLangString(wrappedObject.toString());
    }


    public Integer byteValue() {
        return Integer.valueOf(wrappedObject.byteValue());
    }


    public Integer longValue() {
        return Integer.valueOf((int) wrappedObject.longValue());
    }


    public Character charValue() {
        return new Character((char) wrappedObject.intValue());
    }


    static public JavaLangString toString(final Integer ii)
            throws CGException {
        return new JavaLangString(Integer.toString(ii.intValue()));
    }


    public Double floatValue() {
        return Double.valueOf((double) wrappedObject.floatValue());
    }


    public Integer shortValue() {
        return Integer.valueOf(wrappedObject.shortValue());
    }


    public Double doubleValue() {
        return floatValue();
    }


    static public JavaLangString toString_u(final Integer i,
                                            final Integer radix) {
        return new JavaLangString(Integer.toString(i.intValue(),
                radix.intValue()));
    }


    public JavaLangInteger(final quotes.NIL n) {
        isNil = Boolean.TRUE;
    }


    static public JavaLangString toHexString(final Integer i)
            throws CGException {
        return new JavaLangString(Integer.toHexString(i.intValue()));
    }


    static public JavaLangInteger decode(final JavaLangString nm)
            throws CGException {
        return new JavaLangInteger(Integer.decode(nm.toSeqOfChar()));
    }


    public Boolean equals(final JavaLangObject obj) {
        if (obj instanceof JavaLangInteger) {
            JavaLangInteger intObj = (JavaLangInteger) obj;
            return Boolean.valueOf(wrappedObject.equals(intObj.getWrappedObject()));
        }
        return Boolean.FALSE;
    }


    static public JavaLangString toOctalString(final Integer i)
            throws CGException {
        return new JavaLangString(Integer.toOctalString(i.intValue()));
    }


    static public JavaLangInteger valueOf(final JavaLangString s)
            throws CGException {
        return new JavaLangInteger(Integer.valueOf(s.toSeqOfChar()));
    }


    static public Integer parseInt(final JavaLangString s)
            throws CGException {
        return parseInt(s, Integer.valueOf(10));
    }


    public Integer compareTo(final JavaLangObject o) throws CGException {
        if (o instanceof JavaLangInteger)
            return compareToInt((JavaLangInteger) o);
        else
            throw new VDMSpecException(new JavaLangClassCastException());
    }


    public Integer compareToInt(final JavaLangInteger anotherInteger) {
        return Integer.valueOf(wrappedObject.compareTo(anotherInteger.
                getWrappedObject()));
    }


    static public JavaLangInteger getInteger(final JavaLangString nm)
            throws CGException {
        return new JavaLangInteger(Integer.getInteger(nm.toSeqOfChar()));
    }


    static public Integer parseInt(final JavaLangString s,
                                   final Integer radix)
            throws CGException {
        try {
            return Integer.valueOf(Integer.parseInt(s.toSeqOfChar(),
                    radix.intValue()));
        } catch (NumberFormatException e) {
            throw new VDMSpecException(new JavaLangNumberFormatException());
        }
    }


    static public JavaLangInteger getInteger(final JavaLangString nm,
                                             final Integer val)
            throws CGException {
        return new JavaLangInteger(Integer.getInteger(nm.toSeqOfChar(),
                val.intValue()));
    }


    static public JavaLangInteger getInteger(final JavaLangString nm,
                                             final JavaLangInteger val)
            throws CGException {
        return getInteger(nm, val.getWrappedObject());
    }

}

