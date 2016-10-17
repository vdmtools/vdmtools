package java.util;


public interface Map {
//int size();
//boolean isEmpty();
boolean containsKey( Object key);
//boolean containsValue( Object value);
Object get( Object key);
Object put( Object key,  Object value);
//Object remove( Object key);
//void putAll( Map t);
void clear();
public Set keySet();
//public Collection values();
//public Set entrySet();
 }
