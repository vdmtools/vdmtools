package java.lang;


public final class StringBuffer implements java.io.Serializable {
//static final  long serialVersionUID;
public StringBuffer(){};
public StringBuffer( int length){};
public StringBuffer( String str){};
//public native int length();
//public native int capacity();
//public synchronized native void ensureCapacity( int minimumCapacity);
//public synchronized native void setLength( int newLength);
//public synchronized native char charAt( int index);
//public synchronized native void getChars( int srcBegin,  int srcEnd,  char[] dst,  int dstBegin);
//public synchronized native void setCharAt( int index,  char ch);
public synchronized native StringBuffer append( Object obj);
////public synchronized native StringBuffer append( String str);
public synchronized native StringBuffer append( char[] str);
public synchronized native StringBuffer append( char[] str,  int offset,  int len);
public native StringBuffer append( boolean b);
public synchronized native StringBuffer append( char c);
////public native StringBuffer append( int i);
////public native StringBuffer append( long l);
////public native StringBuffer append( float f);
public native StringBuffer append( double d);
//public synchronized native StringBuffer delete( int start,  int end);
//public synchronized native StringBuffer deleteCharAt( int index);
//public synchronized native StringBuffer replace( int start,  int end,  String str);
//public native String substring( int start);
//public synchronized native String substring( int start,  int end);
//public synchronized native StringBuffer insert( int index,  char[] str,  int offset,  int len);
//public synchronized native StringBuffer insert( int offset,  Object obj);
//public synchronized native StringBuffer insert( int offset,  String str);
//public synchronized native StringBuffer insert( int offset,  char[] str);
//public native StringBuffer insert( int offset,  boolean b);
//public synchronized native StringBuffer insert( int offset,  char c);
//public native StringBuffer insert( int offset,  int i);
//public native StringBuffer insert( int offset,  long l);
//public native StringBuffer insert( int offset,  float f);
//public native StringBuffer insert( int offset,  double d);
//public synchronized native StringBuffer reverse();
public native String toString();
//final native void setShared();
//final native char[] getValue();

}
