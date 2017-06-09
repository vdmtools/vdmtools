import jp.vdmtools.VDM.*;
//import java.io.*;

public class J2VUTIL
{
  static String toString(Object arg)
  {
    return UTIL.toString(arg);
  }

  static String toString(char arg)
  {
    return "'" + new Character(arg).toString() + "'";
  }

  static String toString(boolean arg)
  {
    return new Boolean(arg).toString();
  }

  static String toString(int arg)
  {
    return new Integer(arg).toString();
  }

  static String toString(float arg)
  {
    return new Float(arg).toString();
  }

  static String toString(double arg)
  {
    return new Double(arg).toString();
  }

  static String toString(boolean arg[])
  {
    String result="{ ";

    for(int i=0; i<arg.length; i++) {
      result += new Integer(i).toString() + " |-> " + new Boolean(arg[i]).toString();
      if( i<arg.length-1 ) result += ",";
    }

    return result+" }";
  }

  static String toString(int arg[])
  {
    String result="{ ";

    for(int i=0; i<arg.length; i++) {
      result += new Integer(i).toString() + " |-> " + new Integer(arg[i]).toString();
      if( i<arg.length-1 ) result += ",";
    }

    return result+" }";
  }

  static String toString(int arg[][])
  {
    String result="{ ";
    String ident="  ";

    for(int i=0; i<arg.length; i++) {
      result += (i>0 ? ident : "") + new Integer(i).toString() +
                " |->\n" + ident + toString(arg[i]);
      if( i<arg.length-1 ) result += ",\n";
    }

    return result+" }";
  }

  static String toString(char arg[])
  {
    String result="{ ";

    for(int i=0; i<arg.length; i++) {
      result += new Integer(i).toString() + " |-> " + "'" + new Character(arg[i]).toString() + "'";
      if( i<arg.length-1 ) result += ",";
    }

    return result+" }";
  }

  static String toString(char arg[][])
  {
    String result="{ ";
    String ident="  ";

    for(int i=0; i<arg.length; i++) {
      result += (i>0 ? ident : "") + new Integer(i).toString() +
                " |->\n" + ident + toString(arg[i]);
      if( i<arg.length-1 ) result += ",\n";
    }

    return result+" }";
  }

}
