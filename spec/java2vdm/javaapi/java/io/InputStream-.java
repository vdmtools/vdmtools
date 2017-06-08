package java.io;


public abstract class InputStream {
//public abstract native int read()throws IOException;
//public native int read( byte[] b)throws IOException;
//public native int read( byte[] b,  int off,  int len)throws IOException;
//public native long skip( long n)throws IOException;
//public native int available()throws IOException;
public native void close()throws IOException;
//public synchronized native void mark( int readlimit);
//public synchronized native void reset()throws IOException;
//public native boolean markSupported();

}
