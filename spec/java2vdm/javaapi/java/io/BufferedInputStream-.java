package java.io;


public class BufferedInputStream extends FilterInputStream {
//protected  byte buf[];
//protected  int count;
//protected  int pos;
//protected  int markpos;
//protected  int marklimit;
public BufferedInputStream( InputStream in){};
//public BufferedInputStream( InputStream in,  int size){};
//public synchronized native int read()throws IOException;
public synchronized native int read( byte[] b,  int off,  int len)throws IOException;
//public synchronized native long skip( long n)throws IOException;
//public synchronized native int available()throws IOException;
//public synchronized native void mark( int readlimit);
//public synchronized native void reset()throws IOException;
//public native boolean markSupported();
//public native void close()throws IOException;

}
