package java.util;

import java.lang.*;

public class StringTokenizer implements Enumeration {
public StringTokenizer( String str,  String delim,  boolean returnDelims){};
//public StringTokenizer( String str,  String delim){};
//public StringTokenizer( String str){};
public native boolean hasMoreTokens();
public native String nextToken();
public native String nextToken( String delim);
//public native boolean hasMoreElements();
//public native Object nextElement();
//public native int countTokens();

}
