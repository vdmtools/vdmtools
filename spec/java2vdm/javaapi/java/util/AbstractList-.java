package java.util;


public abstract class AbstractList extends AbstractCollection implements List {

//protected AbstractList(){};
//public native boolean add( Object o);
//abstract public native Object get( int index);
//public native Object set( int index,  Object element);
//public native void add( int index,  Object element);
//public native Object remove( int index);
//public native int indexOf( Object o);
//public native int lastIndexOf( Object o);
//public native void clear();
//public native boolean addAll( int index,  Collection c);
public native Iterator iterator();
//public native ListIterator listIterator();
//public native ListIterator listIterator(final  int index);
//public native List subList( int fromIndex,  int toIndex);
//public native boolean equals( Object o);
//public native int hashCode();
//protected native void removeRange( int fromIndex,  int toIndex);
//protected transient  int modCount;
}
/*
class SubList extends AbstractList {
SubList( AbstractList list,  int fromIndex,  int toIndex){};
public native Object set( int index,  Object element);
public native Object get( int index);
public native int size();
public native void add( int index,  Object element);
public native Object remove( int index);
protected native void removeRange( int fromIndex,  int toIndex);
public native boolean addAll( Collection c);
public native boolean addAll( int index,  Collection c);
public native Iterator iterator();
public native ListIterator listIterator(final  int index);
public native List subList( int fromIndex,  int toIndex);
}
*/
