// ***** VDMTOOLS START Name=HeaderComment KEEP=NO
// Implementation of the VDM standard VDMUtil library
// Use this file instead of the automatically generated VDMUtil.java file
// $Revision: 1.2 $
// $Date: 2005/05/11 06:45:49 $

// Note that there is no concurrency code, since all of the methods
// correspond to VDM++ functions, which do not have associated history
// counters. 

// ***** VDMTOOLS END Name=HeaderComment

// ***** VDMTOOLS START Name=package KEEP=NO
// ***** VDMTOOLS END Name=package

// ***** VDMTOOLS START Name=imports KEEP=YES
import jp.vdmtools.VDM.UTIL;
import jp.vdmtools.VDM.Tuple;
import jp.vdmtools.VDM.CGException;
import java.util.List;
import java.util.ArrayList;
import java.util.Set;
import java.util.Iterator;
import java.util.Date;
// ***** VDMTOOLS END Name=imports

public class VDMUtil {

// ***** VDMTOOLS START Name=set2seq#1|Set KEEP=YES
  @SuppressWarnings("unchecked")
  static public List set2seq (final Set x) throws CGException {
    List res = new ArrayList();
    for (Iterator itr = x.iterator(); itr.hasNext();) {
      res.add(itr.next());
    }
    return res;
  }
// ***** VDMTOOLS END Name=set2seq#1|Set

// ***** VDMTOOLS START Name=get_ufile_upos KEEP=YES
  static public Tuple get_ufile_upos () throws CGException {
    // TODO
    //return null;
    StackTraceElement[] ste = (new Throwable()).getStackTrace();
    return Tuple.mk_(ste[1].getFileName(),
                     new Integer(ste[1].getLineNumber()),
                     new Integer(0), // not supported
                     ste[1].getClassName(),
                     ste[1].getMethodName());
  }
// ***** VDMTOOLS END Name=get_ufile_upos

// ***** VDMTOOLS START Name=get_file_pos KEEP=YES
  static public Tuple get_file_pos () throws CGException {
    // TODO
    //return null;
    StackTraceElement[] ste = (new Throwable()).getStackTrace();
    return Tuple.mk_(ste[1].getFileName(),
                     new Integer(ste[1].getLineNumber()),
                     new Integer(0), // not supported
                     ste[1].getClassName(),
                     ste[1].getMethodName());
  }
// ***** VDMTOOLS END Name=get_file_pos

// ***** VDMTOOLS START Name=val2seq_uof_uchar#1|Object KEEP=YES
  static public String val2seq_uof_uchar (final Object x) throws CGException {
    return UTIL.toString(x);
  }
// ***** VDMTOOLS END Name=val2seq_uof_uchar#1|Object

// ***** VDMTOOLS START Name=val2seq_of_char#1|Object KEEP=YES
  static public String val2seq_of_char (final Object x) throws CGException {
    return UTIL.toString(x);
  }
// ***** VDMTOOLS END Name=val2seq_of_char#1|Object

// ***** VDMTOOLS START Name=seq_uof_uchar2val#1|String KEEP=YES
  static public Tuple seq_uof_uchar2val (final String s) throws CGException {
    try {
      return jp.vdmtools.VDM.ValueParser.ValueParser.ParseVDMString(s);
    }
    catch (Exception e) {
      try {
        Tuple result = new Tuple(2);
        result.SetField(1, new Boolean(false));
        result.SetField(2, e.getMessage());
        return result;
      }
      catch (Exception ex) {
          // Example of who comes first the egg or the hen ;-)
          // Exceptions thrown from Tuple.SetField are not returned.
      }
    }
    return new Tuple(2);
  }
// ***** VDMTOOLS END Name=seq_uof_uchar2val#1|String

// ***** VDMTOOLS START Name=seq_of_char2val#1|String KEEP=YES
  static public Tuple seq_of_char2val (final String s) throws CGException {
    return seq_uof_uchar2val(s);
  }
// ***** VDMTOOLS END Name=seq_of_char2val#1|String

// ***** VDMTOOLS START Name=vdm_cast#1|Object KEEP=YES
  static public Object vdm_cast (final Object s) throws CGException {
    return s;
  }
// ***** VDMTOOLS END Name=vdm_cast#1|Object

// ***** VDMTOOLS START Name=vdm_clone#1|Object KEEP=YES
  static public Object vdm_clone (final Object s) throws CGException {
    // TODO: not yet implemented
    UTIL.RunTime("Clone isn't implemented");
    return s;
  }
// ***** VDMTOOLS END Name=vdm_clone#1|Object

// ***** VDMTOOLS START Name=classname#1|Object KEEP=YES
  static public String classname (final Object s) throws CGException {
    return s.getClass().getName();
  }
// ***** VDMTOOLS END Name=classname#1|Object

// ***** VDMTOOLS START Name=char2code#1|Character KEEP=YES
  static public Number char2code (final Character c) throws CGException {
    return Integer.valueOf((int)(c.charValue()));
  }
// ***** VDMTOOLS END Name=char2code#1|Character

// ***** VDMTOOLS START Name=code2char#1|Integer KEEP=YES
  static public Character code2char (final Integer n) throws CGException {
    return Character.valueOf((char)(n.intValue()));
  }
// ***** VDMTOOLS END Name=code2char#1|Integer

// ***** VDMTOOLS START Name=current_time KEEP=YES
  static public Number current_time() throws CGException {
    return Long.valueOf(new Date().getTime());
  }
// ***** VDMTOOLS END Name=current_time

}
;
