package java.text;

import java.util.Date;
import java.util.Locale;
import java.text.DecimalFormat;
import java.text.Utility;
import java.io.ObjectInputStream;
import java.io.IOException;
import java.io.InvalidObjectException;

public class MessageFormat extends Format {
//public MessageFormat( String pattern){};
//public native void setLocale( Locale theLocale);
//public native Locale getLocale();
//public native void applyPattern( String newPattern);
//public native String toPattern();
//public native void setFormats( Format[] newFormats);
//public native void setFormat( int variable,  Format newFormat);
//public native Format[] getFormats();
//public final native StringBuffer format( Object[] source,  StringBuffer result,  FieldPosition ignore);
public static native String format( String pattern,  Object[] arguments);
//public final native StringBuffer format( Object source,  StringBuffer result,  FieldPosition ignore);
//public native Object[] parse( String source,  ParsePosition status);
//public native Object[] parse( String source)throws ParseException;
//public native Object parseObject( String text,  ParsePosition status);
//public native Object clone();
//public native boolean equals( Object obj);
//public native int hashCode();
//native NumberFormat getIntegerFormat( Locale locale);

}
