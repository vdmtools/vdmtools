package java.util;


public class Vector extends AbstractList implements List, Cloneable, java.io.Serializable {
//protected  Object elementData[];
//protected  int elementCount;
//protected  int capacityIncrement;
public Vector( int initialCapacity,  int capacityIncrement){};
public Vector( int initialCapacity){};
public Vector(){};
//public Vector( Collection c){};
public synchronized native void copyInto( Object[] anArray);
//public synchronized native void trimToSize();
//public synchronized native void ensureCapacity( int minCapacity);
//public synchronized native void setSize( int newSize);
//public native int capacity();
public native int size();
public native boolean isEmpty();
public native Enumeration elements();
public native boolean contains( Object elem);
public native int indexOf( Object elem);
public synchronized native int indexOf( Object elem,  int index);
//public native int lastIndexOf( Object elem);
//public synchronized native int lastIndexOf( Object elem,  int index);
public synchronized native Object elementAt( int index);
//public synchronized native Object firstElement();
//public synchronized native Object lastElement();
public synchronized native void setElementAt( Object obj,  int index);
public synchronized native void removeElementAt( int index);
public synchronized native void insertElementAt( Object obj,  int index);
public synchronized native void addElement( Object obj);
public synchronized native boolean removeElement( Object obj);
public synchronized native void removeAllElements();
//public synchronized native Object clone();
public synchronized native Object[] toArray();
public synchronized native Object[] toArray( Object[] a);
public synchronized native Object get( int index);
//public synchronized native Object set( int index,  Object element);
public synchronized native boolean add( Object o);
//public native boolean remove( Object o);
public native void add( int index,  Object element);
//public synchronized native Object remove( int index);
//public native void clear();
//public synchronized native boolean containsAll( Collection c);
//public synchronized native boolean addAll( Collection c);
//public synchronized native boolean removeAll( Collection c);
//public synchronized native boolean retainAll( Collection c);
//public synchronized native boolean addAll( int index,  Collection c);
//public synchronized native boolean equals( Object o);
//public synchronized native int hashCode();
//public synchronized native String toString();
//public native List subList( int fromIndex,  int toIndex);
//protected native void removeRange( int fromIndex,  int toIndex);

}
