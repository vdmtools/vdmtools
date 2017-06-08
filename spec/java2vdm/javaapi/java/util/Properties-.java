package java.util;

import java.io.IOException;
import java.io.PrintStream;
import java.io.PrintWriter;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.io.BufferedReader;
import java.io.OutputStream;
import java.io.OutputStreamWriter;
import java.io.BufferedWriter;
import java.util.Hashtable;

public class Properties extends Hashtable {
//protected  Properties defaults;
public Properties(){};
//public Properties( Properties defaults){};
//public synchronized native Object setProperty( String key,  String value);
public synchronized native void load( InputStream inStream)throws IOException;
//public synchronized native void save( OutputStream out,  String header);
//public synchronized native void store( OutputStream out,  String header)throws IOException;
public native String getProperty( String key);
public native String getProperty( String key,  String defaultValue);
//public native Enumeration propertyNames();
//public native void list( PrintStream out);
//public native void list( PrintWriter out);

}
