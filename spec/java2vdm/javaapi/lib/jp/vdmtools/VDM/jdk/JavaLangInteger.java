package jp.vdmtools.VDM.jdk;

import jp.vdmtools.VDM.CGException;
import jp.vdmtools.VDM.VDMSpecException;

public class JavaLangInteger extends JavaLangObject 
    implements JavaLangNumber, JavaLangComparable 
{

    public static Integer MAX_uVALUE = new Integer(Integer.MAX_VALUE);
    public static final Integer MIN_uVALUE = new Integer(Integer.MIN_VALUE);

    public static final JavaLangClass CLASS =
            new JavaLangClass(new String("JavaLangInteger"),
                    new quotes.CLASS(),
                    new Boolean(false));
    public static final JavaLangClass TYPE =
            new JavaLangClass(new String("int"),
                    new quotes.CLASS(),
                    new Boolean(true));
    private Integer wrappedObject;

    public JavaLangInteger() {
        wrappedObject = new Integer(0);
    }

    public Integer getWrappedObject() {
        return wrappedObject;
    }

    public JavaLangInteger(final Integer value_u_u) {
        wrappedObject = value_u_u;
    }


    public JavaLangInteger(final Character value_u_u) {
        wrappedObject = new Integer((int) value_u_u.charValue());
    }

    public JavaLangInteger(final JavaLangString s) throws CGException {
        wrappedObject = new Integer(s.toSeqOfChar());
    }

    public JavaLangClass getJClass() {
        return CLASS;
    }


    public JavaLangClass vdm_getClass() {
	return new JavaLangClass(new String("JavaLangInteger"),
				 new quotes.CLASS(),
				 new Boolean(false));
    }


    public Integer jHashCode() {
        return new Integer(wrappedObject.hashCode());
    }

    public Integer vdm_HashCode() {
        return new Integer(wrappedObject.hashCode());
    }

    public Integer intValue() {
        return new Integer(wrappedObject.intValue());
    }


    public JavaLangString toJString() throws CGException {
        return new JavaLangString(wrappedObject.toString());
    }

    public JavaLangString vdm_toString() throws CGException {
        return new JavaLangString(wrappedObject.toString());
    }


    public Integer byteValue() {
        return new Integer(wrappedObject.byteValue());
    }


    public Integer longValue() {
        return new Integer((int) wrappedObject.longValue());
    }


    public Character charValue() {
        return new Character((char) wrappedObject.intValue());
    }


    static public JavaLangString toString(final Integer ii)
            throws CGException {
        return new JavaLangString(Integer.toString(ii.intValue()));
    }


    public Double floatValue() {
        return new Double((double) wrappedObject.floatValue());
    }


    public Integer shortValue() {
        return new Integer(wrappedObject.shortValue());
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
        isNil = new Boolean(true);
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
            return new Boolean(wrappedObject.equals(intObj.getWrappedObject()));
        }
        return new Boolean(false);
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
        return parseInt(s, new Integer(10));
    }


    public Integer compareTo(final JavaLangObject o) throws CGException {
        if (o instanceof JavaLangInteger)
            return compareToInt((JavaLangInteger) o);
        else
            throw new VDMSpecException(new JavaLangClassCastException());
    }


    public Integer compareToInt(final JavaLangInteger anotherInteger) {
        return new Integer(wrappedObject.compareTo(anotherInteger.
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
            return new Integer(Integer.parseInt(s.toSeqOfChar(),
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

