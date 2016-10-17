package java.io;


public class PrintStream extends FilterOutputStream {
//public PrintStream( OutputStream out){};
//public PrintStream( OutputStream out,  boolean autoFlush){};
//public native void flush();
//public native void close();
//public native boolean checkError();
//protected native void setError();
//public native void write( int b);
//public native void write( byte[] buf,  int off,  int len);
public native void print( boolean b);
public native void print( char c);
////public native void print( int i);
////public native void print( long l);
////public native void print( float f);
public native void print( double d);
public native void print( char[] s);
////public native void print( String s);
public native void print( Object obj);
public native void println();
public native void println( boolean x);
public native void println( char x);
////public native void println( int x);
////public native void println( long x);
////public native void println( float x);
public native void println( double x);
public native void println( char[] x);
////public native void println( String x);
public native void println( Object x);

}
