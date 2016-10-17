package java.io;


public class BufferedWriter extends Writer {
public BufferedWriter( Writer out){};
//public BufferedWriter( Writer out,  int sz){};
//native void flushBuffer()throws IOException;
//public native void write( int c)throws IOException;
//public native void write( char[] cbuf,  int off,  int len)throws IOException;
public native void write( String s,  int off,  int len)throws IOException;
public native void newLine()throws IOException;
//public native void flush()throws IOException;
public native void close()throws IOException;

}
