package java.lang;


public class Throwable implements java.io.Serializable {
public Throwable(){};
//public Throwable( String message){};
public native String getMessage();
//public native String getLocalizedMessage();
//public native String toString();
public native void printStackTrace();
public native void printStackTrace( java.io.PrintStream s);
public native void printStackTrace( java.io.PrintWriter s);
//public native Throwable fillInStackTrace();

}
