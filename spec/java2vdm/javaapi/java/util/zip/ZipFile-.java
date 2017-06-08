package java.util.zip;

import java.io.InputStream;
import java.io.IOException;
import java.io.EOFException;
import java.io.File;
import java.util.Vector;
import java.util.Enumeration;
import java.util.NoSuchElementException;
import java.security.AccessController;

public class ZipFile implements ZipConstants {
//public static final  int OPEN_READ;
//public static final  int OPEN_DELETE;
public ZipFile( String name)throws IOException{};
//public ZipFile( File file,  int mode)throws IOException{};
//public ZipFile( File file)throws ZipException, IOException{};
public native ZipEntry getEntry( String name);
public native InputStream getInputStream( ZipEntry entry)throws IOException;
//public native String getName();
public native Enumeration entries();
//public native int size();
public native void close()throws IOException;
//protected native void finalize()throws IOException;

}
