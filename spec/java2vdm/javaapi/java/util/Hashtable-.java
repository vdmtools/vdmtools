package java.util;

import java.io.*;

public class Hashtable extends Dictionary implements Map, Cloneable, java.io.Serializable {
//public Hashtable( int initialCapacity,  float loadFactor){};
public Hashtable( int initialCapacity){};
public Hashtable(){};
//public Hashtable( Map t){};
public native int size();
public native boolean isEmpty();
public synchronized native Enumeration keys();
public synchronized native Enumeration elements();
//public synchronized native boolean contains( Object value);
//public native boolean containsValue( Object value);
public synchronized native boolean containsKey( Object key);
public synchronized native Object get( Object key);
//protected native void rehash();
public synchronized native Object put( Object key,  Object value);
public synchronized native Object remove( Object key);
//public synchronized native void putAll( Map t);
public synchronized native void clear();
//public synchronized native Object clone();
//public synchronized native String toString();
//public native Set keySet();
//public native Set entrySet();
//public native Collection values();
//public synchronized native boolean equals( Object o);
//public synchronized native int hashCode();

}
