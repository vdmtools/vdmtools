package java.io;


public class StringWriter extends Writer {
public StringWriter(){};
//public StringWriter( int initialSize){};
//public native void write( int c);
//public native void write( char[] cbuf,  int off,  int len);
//public native void write( String str);
//public native void write( String str,  int off,  int len);
public native String toString();
//public native StringBuffer getBuffer();
//public native void flush();
//public native void close()throws IOException;

}
