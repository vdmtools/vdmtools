package jp.vdmtools.VDM.jdk;


import jp.vdmtools.VDM.UTIL;

import java.util.HashMap;

public class JavaLangString extends JavaLangObject 
    implements JavaIoSerializable,
	       JavaLangComparable 
{

    public static JavaLangString NILStr = new JavaLangString(new quotes.NIL());

    public static final JavaLangClass CLASS =
            new JavaLangClass(new String("JavaLangString"),
                    new quotes.CLASS(),
                    Boolean.FALSE);

    String wrappedObject;

    public String getWrappedObject() {
        return wrappedObject;
    }

    private void setWrappedObject(final String value)
    {
	if (value == null)
	    wrappedObject = "";
	else
	    wrappedObject = value;
    }


    public JavaLangString(final String value_u) {
        setWrappedObject(value_u);
    }


    public JavaLangString(final JavaLangString value_u) {
        setWrappedObject(value_u.getWrappedObject());
    }


    public JavaLangClass getJClass() {
        return CLASS;
    }


    public String toSeqOfChar() {
        if (wrappedObject == null)
	    return "";
	else
	    return wrappedObject;
    }


    public JavaLangString(final quotes.NIL p) {
        this();
        isNil = Boolean.TRUE;
    }


    public JavaLangString trim() {
        return new JavaLangString(wrappedObject.trim());
    }


    public JavaLangString() {
        setWrappedObject(new String());
    }


    public JavaLangString intern() {
        return new JavaLangString(wrappedObject.intern());
    }


    public Integer length() {
        return Integer.valueOf(wrappedObject.length());
    }


    public Character charAt(final Integer index) {
        return Character.valueOf(wrappedObject.charAt(index.intValue()));
    }


    public HashMap getBytes() {
        HashMap result = new HashMap();
        byte dst[] = wrappedObject.getBytes();
        for (int i = 0; i < dst.length; i++)
            result.put(Integer.valueOf(i), Integer.valueOf(dst[i]));
        return result;
    }


    public Integer jHashCode() {
        return Integer.valueOf(wrappedObject.hashCode());
    }

    public Integer vdm_hashCode() {
        return Integer.valueOf(wrappedObject.hashCode());
    }


    public Integer indexOf(final Object ch) {
        if (ch instanceof Integer)
            return Integer.valueOf(wrappedObject.indexOf(((Integer) ch).intValue()));
        else
            return Integer.valueOf(wrappedObject.indexOf(((Character) ch).charValue()));
    }


    public JavaLangString toJString() {
        return this;
    }


    public Integer indexOf(final Integer ch, final Integer fromIndex) {
        return Integer.valueOf(wrappedObject.indexOf(ch.intValue(),
                fromIndex.intValue()));
    }


    public JavaLangString replace(final Character oldChar,
                                  final Character newChar) {
        return new JavaLangString(wrappedObject.replace(oldChar.charValue(),
                newChar.charValue()));
    }


    public JavaLangString substring(final Integer beginIndex,
                                    final Integer endIndex) {
        return new JavaLangString(wrappedObject.substring(beginIndex.intValue(),
                endIndex.intValue()));
    }


    public JavaLangString substring(final Integer beginIndex) {
        return new JavaLangString(wrappedObject.substring(beginIndex.intValue()));
    }


    public JavaLangString(final HashMap value_u) {
        if (value_u.size() == 0)
            setWrappedObject(new String(""));

        Object obj = value_u.get(Integer.valueOf(0));
        if (obj instanceof Character) {
            char chars[] = new char[value_u.size()];
            for (int i = 0; i < value_u.size(); i++)
                chars[i] = ((Character) value_u.get(Integer.valueOf(i))).charValue();
            setWrappedObject(new String(chars));
        } else {
            byte bytes[] = new byte[value_u.size()];
            for (int i = 0; i < value_u.size(); i++)
                bytes[i] = (byte) ((Integer) value_u.get(Integer.valueOf(i))).intValue();
            setWrappedObject(new String(bytes));
        }
    }


    public HashMap toCharArray() {
        char chars[] = wrappedObject.toCharArray();
        HashMap result = new HashMap();
        for (int i = 0; i < chars.length; i++)
            result.put(Integer.valueOf(i), Character.valueOf(chars[i]));
        return result;
    }


    public JavaLangString toLowerCase() {
        return new JavaLangString(wrappedObject.toLowerCase());
    }


    public JavaLangString toUpperCase() {
        return new JavaLangString(wrappedObject.toUpperCase());
    }


    public Integer lastIndexOf(final Integer ch) {
        return Integer.valueOf(wrappedObject.lastIndexOf(ch.intValue()));
    }

/*
    public JavaLangString(final HashMap ascii, final Integer hibyte) {
        byte bytes[] = new byte[ascii.size()];
        for (int i = 0; i < ascii.size(); i++)
            bytes[i] = (byte) ((Integer) ascii.get(Integer.valueOf(i))).intValue();
        wrappedObject = new String(bytes,
                hibyte.intValue());
    }
*/

    public Integer lastIndexOf(final Integer ch, final Integer fromIndex) {
        return Integer.valueOf(wrappedObject.lastIndexOf(ch.intValue(),
                fromIndex.intValue()));
    }


    public JavaLangString(final HashMap value_u,
                          final Integer offset,
                          final Integer count) {
        if (value_u == null) {
            setWrappedObject(new String(""));
        } else {

            Object o = value_u.get(Integer.valueOf(0));
            if (o instanceof Character) {
                char chars[] = new char[value_u.size()];
                for (int i = 0; i < value_u.size(); i++)
                    chars[i] = ((Character) value_u.get(Integer.valueOf(i))).charValue();
                wrappedObject = new String(chars,
                        offset.intValue(),
                        count.intValue());
            } else {
                byte bytes[] = new byte[value_u.size()];
                for (int i = 0; i < value_u.size(); i++)
                    bytes[i] = (byte) ((Integer) value_u.get(Integer.valueOf(i))).intValue();
                setWrappedObject(new String(bytes));
            }
        }
    }


    public JavaLangString concat(final JavaLangString str) {
        return new JavaLangString(wrappedObject.concat(str.toSeqOfChar()));
    }


    static public JavaLangString copyValueOf(final HashMap data) {
        int size = data.size();
        char chars[] = new char[size];
        for (int i = 0; i < size; i++) {
            Character ch = (Character) data.get(Integer.valueOf(i));
            chars[i] = ch.charValue();
        }
        return new JavaLangString(String.copyValueOf(chars));
    }


    public Boolean equals(final JavaLangObject anObject) {
        if (anObject instanceof JavaLangString)
            return Boolean.valueOf(wrappedObject.equals(((JavaLangString) anObject).getWrappedObject()));
        else
            return Boolean.FALSE;
    }


    public Boolean vdm_equals(final JavaLangObject anObject) {
        if (anObject instanceof JavaLangString)
            return Boolean.valueOf(wrappedObject.equals(((JavaLangString) anObject).getWrappedObject()));
        else
            return Boolean.FALSE;
    }

    public JavaLangString vdm_toString()
    {
	return this;
    }


    public JavaLangString(final HashMap ascii,
                          final Integer hibyte,
                          final Integer offset,
                          final Integer count) {
        byte bytes[] = new byte[ascii.size()];
        for (int i = 0; i < ascii.size(); i++)
            bytes[i] = (byte) ((Integer) ascii.get(Integer.valueOf(i))).intValue();
        setWrappedObject(new String(bytes));
    }


    public Boolean endsWith(final JavaLangString suffix) {
        return Boolean.valueOf(wrappedObject.endsWith(suffix.toSeqOfChar()));
    }


    public HashMap getBytes(final JavaLangString enc) {
        HashMap result = new HashMap();
        try {
            byte bytes[] = wrappedObject.getBytes(enc.toSeqOfChar());
            for (int i = 0; i < bytes.length; i++)
                result.put(Integer.valueOf(i), Integer.valueOf(bytes[i]));
        } catch (java.io.UnsupportedEncodingException e) {
        }
        return result;
    }


    public Integer compareTo(final JavaLangObject o) {
        return compareTo_q(((JavaLangString) o).getWrappedObject());
    }

    public Integer compareTo_q(final String o) {
        return Integer.valueOf(wrappedObject.compareTo(o));
    }


    // Method not supported since dst should be a ref param
    public void getBytes(final Integer srcBegin,
                         final Integer srcEnd,
                         final HashMap dst,
                         final Integer dstBegin) {
    }


    // Method not supported since dst should be a ref param
    public void getChars(final Integer srcBegin,
                         final Integer srcEnd,
                         final HashMap dst,
                         final Integer dstBegin) {
    }


    public Integer indexOf(final JavaLangString str) {
        return Integer.valueOf(wrappedObject.indexOf(str.toSeqOfChar()));
    }


    public Integer indexOf(final JavaLangString str, final Integer fromIndex) {
        return Integer.valueOf(wrappedObject.indexOf(str.toSeqOfChar(),
                fromIndex.intValue()));
    }


    static public JavaLangString copyValueOf(final HashMap data,
                                             final Integer offset,
                                             final Integer count) {
        int size = data.size();
        char chars[] = new char[size];
        for (int i = 0; i < size; i++) {
            Character ch = (Character) data.get(Integer.valueOf(i));
            chars[i] = ch.charValue();
        }
        return new JavaLangString(String.copyValueOf(chars, offset.intValue(),
                count.intValue()));
    }


    public Boolean startsWith(final JavaLangString prefix,
                              final Integer toffset) {
        return Boolean.valueOf(wrappedObject.startsWith(prefix.toSeqOfChar(),
                toffset.intValue()));

    }


    public Boolean startsWith(final JavaLangString prefix) {
        return Boolean.valueOf(wrappedObject.startsWith(prefix.toSeqOfChar()));
    }


    public Integer lastIndexOf(final JavaLangString str) {
        return Integer.valueOf(wrappedObject.lastIndexOf(str.toSeqOfChar()));
    }


    public Integer lastIndexOf(final JavaLangString str,
                               final Integer fromIndex) {
        return Integer.valueOf(wrappedObject.lastIndexOf(str.toSeqOfChar(),
                fromIndex.intValue()));
    }


    public JavaLangString(final HashMap bytes,
                          final JavaLangString enc) {

        this(bytes, Integer.valueOf(0), Integer.valueOf(bytes.size()), enc);
    }


    public JavaLangString(final HashMap data,
                          final Integer offset,
                          final Integer len_ugth,
                          final JavaLangString enc) {
        int size = data.size();
        byte bytes[] = new byte[size];
        for (int i = 0; i < size; i++) {
            Integer ch = (Integer) data.get(Integer.valueOf(i));
            bytes[i] = (byte) ch.intValue();
        }
        try {
            wrappedObject = new String(bytes,
                    offset.intValue(),
                    len_ugth.intValue(),
                    enc.toSeqOfChar());
        } catch (java.io.UnsupportedEncodingException e) {
            setWrappedObject(new String(""));
        }
    }


    public Boolean equalsIgnoreCase(final JavaLangString anotherString) {
        return Boolean.valueOf(compareToIgnoreCase((JavaLangString) anotherString).intValue() == 0);
    }


    public Integer compareToIgnoreCase(final JavaLangString anotherString) {
        return Integer.valueOf(wrappedObject.compareToIgnoreCase(anotherString.toSeqOfChar()));
    }


    public Boolean regionMatches(final Integer toffset,
                                 final JavaLangString other,
                                 final Integer ooffset,
                                 final Integer len_u) {
        return Boolean.valueOf(wrappedObject.regionMatches(toffset.intValue(),
                other.toSeqOfChar(),
                ooffset.intValue(),
                len_u.intValue()));
    }


    public Boolean regionMatches(final Boolean ignoreCase,
                                 final Integer toffset,
                                 final JavaLangString other,
                                 final Integer ooffset,
                                 final Integer len_u) {
        return Boolean.valueOf(wrappedObject.regionMatches(ignoreCase.booleanValue(),
                toffset.intValue(),
                other.toSeqOfChar(),
                ooffset.intValue(),
                len_u.intValue()));
    }


    static public JavaLangString valueOf(final HashMap data,
                                         final Integer offset,
                                         final Integer count) {
        int size = data.size();
        char chars[] = new char[size];
        for (int i = 0; i < size; i++) {
            Character ch = (Character) data.get(Integer.valueOf(i));
            chars[i] = ch.charValue();
        }
        return new JavaLangString(String.valueOf(chars,
                offset.intValue(),
                count.intValue()));
    }


    static public JavaLangString valueOf(final HashMap data) {
        int size = data.size();
        char chars[] = new char[size];
        for (int i = 0; i < size; i++) {
            Character ch = (Character) data.get(Integer.valueOf(i));
            chars[i] = ch.charValue();
        }
        return new JavaLangString(String.valueOf(chars));
    }


    static public JavaLangString valueOf(final Object any) {
        return new JavaLangString(String.valueOf(any));
    }


}
