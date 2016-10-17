package java.lang;


public class Object {
public final native Class getClass();
//public native int hashCode();
public native boolean equals( Object obj);
//protected native Object clone()throws CloneNotSupportedException;
public native String toString();
//public final native void notify();
//public final native void notifyAll();
//public final native void wait( long timeout)throws InterruptedException;
//public final native void wait( long timeout,  int nanos)throws InterruptedException;
//public final native void wait()throws InterruptedException;
protected native void finalize()throws Throwable;

}
