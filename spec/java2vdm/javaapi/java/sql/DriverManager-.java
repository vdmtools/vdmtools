package java.sql;


public class DriverManager {
//final static  SQLPermission SET_LOG_PERMISSION;
//public static native java.io.PrintWriter getLogWriter();
//public static synchronized native void setLogWriter( java.io.PrintWriter out);
public static synchronized native Connection getConnection( String url,  java.util.Properties info)throws SQLException;
public static synchronized native Connection getConnection( String url,  String user,  String password)throws SQLException;
public static synchronized native Connection getConnection( String url)throws SQLException;
//public static synchronized native Driver getDriver( String url)throws SQLException;
//public static synchronized native void registerDriver( java.sql.Driver driver)throws SQLException;
//public static synchronized native void deregisterDriver( Driver driver)throws SQLException;
//public static synchronized native java.util.Enumeration getDrivers();
//public static native void setLoginTimeout( int seconds);
//public static native int getLoginTimeout();
//public static synchronized native void setLogStream( java.io.PrintStream out);
//public static native java.io.PrintStream getLogStream();
//public static synchronized native void println( String message);
//static native void initialize();

}
//class DriverInfo {
// Driver driver;
// Class driverClass;
// String driverClassName;
//public native String toString();
//
//}
