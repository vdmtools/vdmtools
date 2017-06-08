package java.util.zip;

import java.util.Date;

public class ZipEntry implements ZipConstants, Cloneable {
// String name;
// long time;
// long crc;
// long size;
// long csize;
// int method;
// byte[] extra;
// String comment;
// int flag;
// int version;
// long offset;
//public static final  int STORED;
//public static final  int DEFLATED;
//public ZipEntry( String name){};
//public ZipEntry( ZipEntry e){};
//ZipEntry( String name,  long jzentry){};
//ZipEntry( long jzentry){};
public native String getName();
//public native void setTime( long time);
//public native long getTime();
//public native void setSize( long size);
public native long getSize();
//public native long getCompressedSize();
//public native void setCompressedSize( long csize);
//public native void setCrc( long crc);
//public native long getCrc();
//public native void setMethod( int method);
//public native int getMethod();
//public native void setExtra( byte[] extra);
//public native byte[] getExtra();
//public native void setComment( String comment);
//public native String getComment();
//public native boolean isDirectory();
//public native String toString();
//public native int hashCode();
//public native Object clone();

}
