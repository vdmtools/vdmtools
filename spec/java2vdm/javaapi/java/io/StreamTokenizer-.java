package java.io;


public class StreamTokenizer {
public  int ttype;
public static final  int TT_EOF;
public static final  int TT_EOL;
public static final  int TT_NUMBER;
public static final  int TT_WORD;
public  String sval;
public  double nval;
//public StreamTokenizer( InputStream is){};
public StreamTokenizer( Reader r){};
//public native void resetSyntax();
public native void wordChars( int low,  int hi);
//public native void whitespaceChars( int low,  int hi);
//public native void ordinaryChars( int low,  int hi);
//public native void ordinaryChar( int ch);
public native void commentChar( int ch);
public native void quoteChar( int ch);
//public native void parseNumbers();
//public native void eolIsSignificant( boolean flag);
//public native void slashStarComments( boolean flag);
//public native void slashSlashComments( boolean flag);
//public native void lowerCaseMode( boolean fl);
public native int nextToken()throws IOException;
//public native void pushBack();
//public native int lineno();
//public native String toString();

}
