package java.io;


public class BufferedReader extends Reader {
//public BufferedReader( Reader in,  int sz){};
public BufferedReader( Reader in){};
//public native int read()throws IOException;
//public native int read( char[] cbuf,  int off,  int len)throws IOException;
//native String readLine( boolean ignoreLF)throws IOException;
public native String readLine()throws IOException;
//public native long skip( long n)throws IOException;
//public native boolean ready()throws IOException;
//public native boolean markSupported();
//public native void mark( int readAheadLimit)throws IOException;
//public native void reset()throws IOException;
public native void close()throws IOException;

}
