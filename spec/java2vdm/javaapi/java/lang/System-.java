package java.lang;

import java.io.*;
import java.util.Properties;
import java.util.PropertyPermission;
import java.util.StringTokenizer;
import java.security.AccessController;
import java.security.PrivilegedAction;
import java.security.AllPermission;
import sun.net.InetAddressCachePolicy;

public final class System {
//public final static  InputStream in;
public final static  PrintStream out;
public final static  PrintStream err;
//public static native void setIn( InputStream in);
//public static native void setOut( PrintStream out);
//public static native void setErr( PrintStream err);
//public static native void setSecurityManager(final  SecurityManager s);
//public static native SecurityManager getSecurityManager();
public static native long currentTimeMillis();
public static native void arraycopy( Object src,  int src_position,  Object dst,  int dst_position,  int length);
//public static native int identityHashCode( Object x);
//public static native Properties getProperties();
//public static native void setProperties( Properties props);
public static native String getProperty( String key);
public static native String getProperty( String key,  String def);
public static native String setProperty( String key,  String value);
//public static native String getenv( String name);
public static native void exit( int status);
//public static native void gc();
//public static native void runFinalization();
//public static native void runFinalizersOnExit( boolean value);
//public static native void load( String filename);
//public static native void loadLibrary( String libname);
//public static native String mapLibraryName( String libname);
//static native Class getCallerClass();

}
