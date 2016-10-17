package java.lang;

import java.lang.reflect.Member;
import java.lang.reflect.Field;
import java.lang.reflect.Method;
import java.lang.reflect.Constructor;
import java.lang.reflect.Modifier;
import java.io.InputStream;
import java.io.ObjectStreamClass;
import java.io.ObjectStreamField;

public final class Class implements java.io.Serializable {
//public native String toString();
public static native Class forName( String className)throws ClassNotFoundException;
//public static native Class forName( String name,  boolean initialize,  ClassLoader loader)throws ClassNotFoundException;
public native Object newInstance()throws InstantiationException, IllegalAccessException;
//public native boolean isInstance( Object obj);
//public native boolean isAssignableFrom( Class cls);
//public native boolean isInterface();
//public native boolean isArray();
//public native boolean isPrimitive();
public native String getName();
//public native ClassLoader getClassLoader();
//public native Class getSuperclass();
//public native Package getPackage();
//public native Class[] getInterfaces();
//public native Class getComponentType();
//public native int getModifiers();
//public native Object[] getSigners();
//native void setSigners( Object[] signers);
//public native Class getDeclaringClass();
//public native Class[] getClasses();
//public native Field[] getFields()throws SecurityException;
//public native Method[] getMethods()throws SecurityException;
//public native Constructor[] getConstructors()throws SecurityException;
//public native Field getField( String name)throws NoSuchFieldException, SecurityException;
//public native Method getMethod( String name,  Class[] parameterTypes)throws NoSuchMethodException, SecurityException;
//public native Constructor getConstructor( Class[] parameterTypes)throws NoSuchMethodException, SecurityException;
//public native Class[] getDeclaredClasses()throws SecurityException;
//public native Field[] getDeclaredFields()throws SecurityException;
//public native Method[] getDeclaredMethods()throws SecurityException;
//public native Constructor[] getDeclaredConstructors()throws SecurityException;
//public native Field getDeclaredField( String name)throws NoSuchFieldException, SecurityException;
//public native Method getDeclaredMethod( String name,  Class[] parameterTypes)throws NoSuchMethodException, SecurityException;
//public native Constructor getDeclaredConstructor( Class[] parameterTypes)throws NoSuchMethodException, SecurityException;
//public native InputStream getResourceAsStream( String name);
//public native java.net.URL getResource( String name);
//public native java.security.ProtectionDomain getProtectionDomain();
//native void setProtectionDomain0( java.security.ProtectionDomain pd);
//static native Class getPrimitiveClass( String name);

}
