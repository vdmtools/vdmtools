package java.util;

import java.io.InputStream;
import java.io.FileInputStream;
import sun.misc.SoftCache;
import java.lang.ref.SoftReference;

abstract public class ResourceBundle {
//protected  ResourceBundle parent;
//public ResourceBundle(){};
public final native String getString( String key)throws MissingResourceException;
//public final native String[] getStringArray( String key)throws MissingResourceException;
//public final native Object getObject( String key)throws MissingResourceException;
//public native Locale getLocale();
//protected native void setParent( ResourceBundle parent);
public static final native ResourceBundle getBundle( String baseName)throws MissingResourceException;
//public static final native ResourceBundle getBundle( String baseName,  Locale locale);
//public static native ResourceBundle getBundle( String baseName,  Locale locale,  ClassLoader loader)throws MissingResourceException;
//protected abstract native Object handleGetObject( String key)throws MissingResourceException;
//public abstract native Enumeration getKeys();

}
