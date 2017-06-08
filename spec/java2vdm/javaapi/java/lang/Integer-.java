package java.lang;


public final class Integer extends Number implements Comparable {
//public static final  int MIN_VALUE;
public static final  int MAX_VALUE;
//public static final  Class TYPE;
//final static  char[] digits;
//public static native String toString( int i,  int radix);
//public static native String toHexString( int i);
//public static native String toOctalString( int i);
//public static native String toBinaryString( int i);
public static native String toString( int i);
public static native int parseInt( String s,  int radix)throws NumberFormatException;
public static native int parseInt( String s)throws NumberFormatException;
//public static native Integer valueOf( String s,  int radix)throws NumberFormatException;
//public static native Integer valueOf( String s)throws NumberFormatException;
public Integer( int value){};
public Integer( String s)throws NumberFormatException{};
//public native byte byteValue();
//public native short shortValue();
public native int intValue();
//public native long longValue();
//public native float floatValue();
//public native double doubleValue();
public native String toString();
//public native int hashCode();
//public native boolean equals( Object obj);
//public static native Integer getInteger( String nm);
//public static native Integer getInteger( String nm,  int val);
//public static native Integer getInteger( String nm,  Integer val);
//public static native Integer decode( String nm)throws NumberFormatException;
////public native int compareTo( Integer anotherInteger);
//public native int compareTo( Object o);

}
