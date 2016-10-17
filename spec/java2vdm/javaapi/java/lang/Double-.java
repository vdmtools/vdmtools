package java.lang;


public final class Double extends Number implements Comparable {
//public static final  double POSITIVE_INFINITY;
//public static final  double NEGATIVE_INFINITY;
//public static final  double NaN;
public static final  double MAX_VALUE;
public static final  double MIN_VALUE;
public static final  Class TYPE;
public static native String toString( double d);
public static native Double valueOf( String s)throws NumberFormatException;
public static native double parseDouble( String s)throws NumberFormatException;
//static public native boolean isNaN( double v);
//static public native boolean isInfinite( double v);
public Double( double value){};
//public Double( String s)throws NumberFormatException{};
//public native boolean isNaN();
//public native boolean isInfinite();
public native String toString();
public native byte byteValue();
public native short shortValue();
public native int intValue();
public native long longValue();
public native float floatValue();
public native double doubleValue();
//public native int hashCode();
//public native boolean equals( Object obj);
//public static native long doubleToLongBits( double value);
//public static native long doubleToRawLongBits( double value);
//public static native double longBitsToDouble( long bits);
////public native int compareTo( Double anotherDouble);
//public native int compareTo( Object o);

}
