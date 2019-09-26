package jp.vdmtools.VDM;

import java.util.Collection;
import java.util.Collections;
import java.util.List;
import java.util.ArrayList;
import java.util.Vector;
import java.util.Set;
import java.util.HashSet;
import java.util.TreeSet;
import java.util.Map;
import java.util.HashMap;
import java.util.Iterator;
import java.util.Comparator;

/**
 * The VDM Java library provides some functions which are applicable to
 * instances of all types. There does not exist a common superclass to
 * all VDM types (because we choose to use the Java wrapper classes for
 * primitive data types). Therefore, we have chosen to let the UTIL class
 * provide the functions applicable to all VDM datatypes.
 * This class may not be instantiated and does only provide class methods.
 * The UTIL class contains methods used by the generated code and methods,
 * which can be used by the code written to interface the generated code.
*/

public abstract class UTIL {

  private static long rseed = -1;
  private static RANDOM rnd = new RANDOM();

  public static class VDMCompare implements Comparator {
    public int compare(final Object x, final Object y) {
      if ((x instanceof Number) && (y instanceof Number)) {
        double xval = ((Number) x).doubleValue();
        double yval = ((Number) y).doubleValue();
        if (xval < yval) {
          return -1;
        }
        else if (xval > yval) {
          return 1;
        }
        else {
          return 0;
        }
      }
      else if ((x == null) && (y == null)) {
        return 0;
      }
      else if (x == null) {
        return -1;
      }
      else if (y == null) {
        return 1;
      }
      else if (UTIL.equals(x,y)) {
        return 0;
      }
      else if (UTIL.toString(x).compareTo(UTIL.toString(y)) < 0) {
        return -1;
      }
      else {
        return 1;
      }
    }
  }

  @SuppressWarnings("unchecked")
  public static class VDMCompare2 implements Comparator {
    public int compare(Object x, Object y) {

      if ((x == null) && (y == null)) {
        return 0;
      }
      else if (x == null) {
        return -1;
      }
      else if (y == null) {
        return 1;
      }
      else if (x.equals(y)) {
        return 0;
      }
      else if (x instanceof Character) {
        if (y instanceof Character) {
          return ((Character)x).compareTo((Character)y);
        }
        else {
          return -1;
        }
      }
      else if (x instanceof Number) {
        if (y instanceof Number) {
          double xval = ((Number) x).doubleValue();
          double yval = ((Number) y).doubleValue();
          if (xval < yval) {
            return -1;
          }
          else if (xval > yval) {
            return 1;
          }
          else {
           return 0;
          }
        }
        return ((y instanceof Character) ? 1 : -1);
      }
      else if (x instanceof Tuple) {
        if (y instanceof Tuple) {
          Tuple t1 = (Tuple)x;
          Tuple t2 = (Tuple)y;
          int len1 = t1.Length();
          int len2 = t2.Length();
          if (len1 == len2) {
            for (int i = 1; i <= len1; i++) {
              try {
                int c = compare(t1.GetField(i), t2.GetField(i));
                if (c != 0) {
                  return c;
                }
              } catch (INDEX_OUT_OF_RANGE e) {}
            }
            return 0;
          }
          return ((len1 < len2) ? -1 : 1);
        }
        return (((y instanceof Character) || (y instanceof Number)) ? 1 : -1);
      }
      else if (x instanceof Record) {
        if (y instanceof Record) {
          // TODO:
          if (((Record)x).equals(y)) {
            return 0;
          }
          String nm1 = x.getClass().getName();
          String nm2 = y.getClass().getName();
          return nm1.compareTo(nm2);
        }
        return (((y instanceof Character) || (y instanceof Number) ||
                 (y instanceof Tuple)) ? 1 : -1);
      }
      else if (x instanceof Set) {
        if (y instanceof Set) {
          Set s1 = (Set)x;
          Set s2 = (Set)y;
          int size1 = s1.size();
          int size2 = s2.size();
          if (size1 == size2) {
            try {
              Set ts1 = new TreeSet(s1);
              Set ts2 = new TreeSet(s2);
              Iterator iter1 = ts1.iterator();
              Iterator iter2 = ts2.iterator();
              for (; iter1.hasNext();) {
                int c = compare(iter1.next(), iter2.next());
                if (c != 0) {
                  return c;
                }
              }
              return 0;
            } catch (ClassCastException e) {
            }
            // TODO:
            List v1 = new ArrayList();
            v1.addAll(new TreeSet(getCharacterSet(s1)));
            v1.addAll(new TreeSet(getNumberSet(s1)));
            v1.addAll(getTupleSet(s1));
            v1.addAll(getRecordSet(s1));
            v1.addAll(getSetSet(s1));
            v1.addAll(getMapSet(s1));
            v1.addAll(new TreeSet(getBooleanSet(s1)));
            v1.addAll(new TreeSet(getStringSet(s1)));
            v1.addAll(getListSet(s1));
            List v2 = new ArrayList();
            v2.addAll(new TreeSet(getCharacterSet(s2)));
            v2.addAll(new TreeSet(getNumberSet(s2)));
            v2.addAll(getTupleSet(s2));
            v2.addAll(getRecordSet(s2));
            v2.addAll(getSetSet(s2));
            v2.addAll(getMapSet(s2));
            v2.addAll(new TreeSet(getBooleanSet(s2)));
            v2.addAll(new TreeSet(getStringSet(s2)));
            v2.addAll(getListSet(s2));
            return compare(v1, v2);
          }
          return ((size1 < size2) ? -1 : 1);
        }
        return (((y instanceof Character) || (y instanceof Number) ||
                 (y instanceof Tuple) || (y instanceof Record)) ? 1 : -1);
      }
      else if (x instanceof Map) {
        if (y instanceof Map) {
          Map m1 = (Map)x;
          Map m2 = (Map)y;
          int size1 = m1.size();
          int size2 = m2.size();
          if (size1 == size2) {
            try {
              TreeSet ts1 = new TreeSet (m1.keySet());
              TreeSet ts2 = new TreeSet (m2.keySet());
              int kc = compare(ts1, ts2);
              if (kc != 0) {
                return kc;
              }
              Iterator iter1 = ts1.iterator();
              Iterator iter2 = ts2.iterator();
              for (; iter1.hasNext();) {
                int c = compare(m1.get(iter1.next()), m2.get(iter2.next()));
                if (c != 0) {
                  return c;
                }
              }
              return 0;
            } catch (ClassCastException e) {
            }
             
            // TODO:
            Set s1 = m1.keySet();
            Set s2 = m2.keySet();
            List v1 = new ArrayList();
            v1.addAll(new TreeSet(getCharacterSet(s1)));
            v1.addAll(new TreeSet(getNumberSet(s1)));
            v1.addAll(getTupleSet(s1));
            v1.addAll(getRecordSet(s1));
            v1.addAll(getSetSet(s1));
            v1.addAll(getMapSet(s1));
            v1.addAll(new TreeSet(getBooleanSet(s1)));
            v1.addAll(new TreeSet(getStringSet(s1)));
            v1.addAll(getListSet(s1));
            List v2 = new ArrayList();
            v2.addAll(new TreeSet(getCharacterSet(s2)));
            v2.addAll(new TreeSet(getNumberSet(s2)));
            v2.addAll(getTupleSet(s2));
            v2.addAll(getRecordSet(s2));
            v2.addAll(getSetSet(s2));
            v2.addAll(getMapSet(s2));
            v2.addAll(new TreeSet(getBooleanSet(s2)));
            v2.addAll(new TreeSet(getStringSet(s2)));
            v2.addAll(getListSet(s2));
            return compare(v1, v2);
          }
          return ((size1 < size2) ? -1 : 1);
        }
        return (((y instanceof Boolean) ||
                 (y instanceof String) || (y instanceof List))
                 ? -1 : 1);
      }
      else if (x instanceof Boolean) {
        if (y instanceof Boolean) {
          return ((Boolean)x).compareTo((Boolean)y);
        }
        return (((y instanceof String) || (y instanceof List)) ? -1 : 1);
      }
      else if (x instanceof String) {
        if (y instanceof String) {
          return ((String)x).compareTo((String)y);
        }
        else if (y instanceof List) {
          List l1 = UTIL.ConvertToList((String)x);
          List l2 = (List)y;
          int len1 = l1.size();
          int len2 = l2.size();
          if (len1 == len2) {
            for (int i = 0; i < len1; i++) {
              int c = compare(l1.get(i), l2.get(i));
              if (c != 0) {
                return c;
              }
            }
            return 0;
          }
          return ((len1 < len2) ? -1 : 1);
        }
        return 1;
      }
      else if (x instanceof List) {
        if (y instanceof List) {
          List l1 = (List)x;
          List l2 = (List)y;
          int len1 = l1.size();
          int len2 = l2.size();
          if (len1 == len2) {
            for (int i = 0; i < len1; i++) {
              int c = compare(l1.get(i), l2.get(i));
              if (c != 0) {
                return c;
              }
            }
            return 0;
          }
          return ((len1 < len2) ? -1 : 1);
        }
        else if (y instanceof String) {
          List l1 = (List)x;
          List l2 = UTIL.ConvertToList((String)y);
          int len1 = l1.size();
          int len2 = l2.size();
          if (len1 == len2) {
            for (int i = 0; i < len1; i++) {
              int c = compare(l1.get(i), l2.get(i));
              if (c != 0) {
                return c;
              }
            }
            return 0;
          }
          return ((len1 < len2) ? -1 : 1);
        }
        return 1;
      }
      else {
        return 1;
      }
    }

    private static Set getCharacterSet(Set s) {
      Set rs = UTIL.createSet();
      for (Iterator iter = s.iterator(); iter.hasNext();) {
        Object o = iter.next();
        if (o instanceof Character) {
          rs.add(o);
        }
      }
      return rs;
    }

    private static Set getNumberSet(Set s) {
      Set rs = UTIL.createSet();
      for (Iterator iter = s.iterator(); iter.hasNext();) {
        Object o = iter.next();
        if (o instanceof Number) {
          rs.add(o);
        }
      }
      return rs;
    }

    private static Set getTupleSet(Set s) {
      Set rs = UTIL.createSet();
      for (Iterator iter = s.iterator(); iter.hasNext();) {
        Object o = iter.next();
        if (o instanceof Tuple) {
          rs.add(o);
        }
      }
      return rs;
    }

    private static Set getRecordSet(Set s) {
      Set rs = UTIL.createSet();
      for (Iterator iter = s.iterator(); iter.hasNext();) {
        Object o = iter.next();
        if (o instanceof Record) {
          rs.add(o);
        }
      }
      return rs;
    }

    private static Set getSetSet(Set s) {
      Set rs = UTIL.createSet();
      for (Iterator iter = s.iterator(); iter.hasNext();) {
        Object o = iter.next();
        if (o instanceof Set) {
          rs.add(o);
        }
      }
      return rs;
    }

    private static Set getMapSet(Set s) {
      Set rs = UTIL.createSet();
      for (Iterator iter = s.iterator(); iter.hasNext();) {
        Object o = iter.next();
        if (o instanceof Map) {
          rs.add(o);
        }
      }
      return rs;
    }

    private static Set getBooleanSet(Set s) {
      Set rs = UTIL.createSet();
      for (Iterator iter = s.iterator(); iter.hasNext();) {
        Object o = iter.next();
        if (o instanceof Boolean) {
          rs.add(o);
        }
      }
      return rs;
    }

    private static Set getStringSet(Set s) {
      Set rs = UTIL.createSet();
      for (Iterator iter = s.iterator(); iter.hasNext();) {
        Object o = iter.next();
        if (o instanceof String) {
          rs.add(o);
        }
      }
      return rs;
    }

    private static Set getListSet(Set s) {
      Set rs = UTIL.createSet();
      for (Iterator iter = s.iterator(); iter.hasNext();) {
        Object o = iter.next();
        if (o instanceof List) {
          rs.add(o);
        }
      }
      return rs;
    }
  }
//  /**
//   * prints the class name of obj to standard layout.
//   */

//  public static void printClassName(Object obj) {
//    System.out.println("The class of " + obj +
//                       " is " + obj.getClass().getName());
//  }

 /**
  * returns a string containing an ASCII representation of the VDM value
  *
  * @param obj a VDM value to string representation.
  * @return a string representation of the object.
  */
  public static String toString(Object obj) {
    StringBuffer buffer = new StringBuffer();
    UTIL.toStringBuffer(obj, buffer);
    return buffer.toString();
  }

  @SuppressWarnings("unchecked")
  private static void toStringBuffer(Object obj, StringBuffer buffer) {
    if (obj == null) {
      buffer.append("nil");
    }
    else if (obj instanceof String) {
      buffer.append("\"");
      buffer.append(obj.toString());
      buffer.append("\"");
    }
    else if (obj instanceof Character) {
      buffer.append("'");
      buffer.append(obj.toString());
      buffer.append("'");
    }
    else if (obj instanceof Tuple) {
      buffer.append(((Tuple)obj).toString());
    }
    else if  (obj instanceof Map) {
      Map m = (Map) obj;
      buffer.append("{ ");
      if (m.isEmpty()) {
        buffer.append("|->");
      }
      else {
        UTIL.collectionToString((Collection) m.entrySet(), buffer);
      }
      buffer.append(" }");
    }
    else if (obj instanceof Map.Entry) {
      Map.Entry entry = (Map.Entry) obj;
      UTIL.toStringBuffer(entry.getKey(), buffer);
      buffer.append(" |-> ");
      UTIL.toStringBuffer(entry.getValue(), buffer);
    }
    else if (obj instanceof Set) {
      buffer.append("{ ");
      //collectionToString((Collection)obj, buffer);
      TreeSet ts = new TreeSet(new VDMCompare2());
      ts.addAll((Collection)obj);
      UTIL.collectionToString(ts, buffer);
      buffer.append(" }");
    }
    else if (obj instanceof List) {
      // 20091113
      List l = (List)obj;
      int len_l = l.size();
      boolean forall = (len_l > 0);
      for (int i = 0; (i < len_l) && forall; i++) {
        forall = (l.get(i) instanceof Character);
      }
      if (forall) {
        UTIL.toStringBuffer(UTIL.ConvertToString(l), buffer);
      }
      else {
        buffer.append("[ ");
        UTIL.vectorToString((List)obj, buffer);
        buffer.append(" ]");
      }
    }
    else if (obj instanceof Double) {
      Double dv = (Double)obj;
      if( dv.doubleValue() == dv.intValue() ) {
        buffer.append(Integer.valueOf(dv.intValue()).toString());
      }
      else {
        buffer.append(obj.toString());
      }
    }
    else
      buffer.append(obj.toString());
  }

//  private static void toString2(Object obj, StringBuffer buffer){
//    if (obj == null)
//      s = s + "nil";
//    else if (obj instanceof Character)
//      s = s + "'" + obj.toString() +"'";
//    else if (obj instanceof String)
//      s = s + "\"" + obj.toString() +"\"";
//    else if (obj instanceof Tuple)
//      s = s + "mk_(" + ((Tuple)obj).toString2() + ")";
//    else if  (obj instanceof Map) {
//      s = s + "{";
//      if (((Map)obj).isEmpty())
//        s = s + " |-> ";
//      else
//        s = s + collectionToString((Collection)
//                                   ((Map)obj).entrySet());
//        s = s + "}";
//    }
//    else if (obj instanceof Map.Entry)
//      s = s + toString2(((Map.Entry) obj).getKey()) +
//          " |-> " + toString2(((Map.Entry) obj).getValue());
//    else if (obj instanceof Set)
//      s = s + "{" + collectionToString((Collection)obj) + "}";
//    else if (obj instanceof List)
//      s = s + "[" + collectionToString((Collection)obj) + "]";
//    else
//      s = s + obj.toString();
//    return s;
//  }

  private static void collectionToString(Collection c, StringBuffer buffer) {
    for(Iterator i = c.iterator(); i.hasNext();) {
      Object obj = i.next();
      buffer.append(toString(obj));
      if (i.hasNext()) {
        buffer.append(", ");
      }
    }
  }

  private static void vectorToString(List v, StringBuffer buffer) {
    for(Iterator i = v.iterator(); i.hasNext();) {
      Object obj = i.next();
      buffer.append(toString(obj));
      if (i.hasNext()) {
        buffer.append(", ");
      }
    }
  }

  /**
   * is called when a run-time error occurs.
   *
   * @param obj error message or VDM value.
   * @throws CGException run-time error exception.
   */
  public static void RunTime(Object obj) throws CGException {
    if (obj instanceof String) {
      String s = (String)obj;
      if (!s.startsWith("Run-Time Error:")) {
        throw new VDMRunTimeException("Run-Time Error:" + s);
      }
    }
    throw new VDMRunTimeException(obj);
    // for 
    // throw new VDMSpecException(new quotes.RuntimeError());
  }

  /**
   * is called when a not supported error occurs.
   *
   * @param obj error message or VDM value.
   * @throws NotSupportedConstructException not supported error exception.
   */
  public static void NotSupported(Object obj) throws NotSupportedConstructException {
    if (obj instanceof String) {
      String s = (String)obj;
      if (!s.startsWith("The construct is not supported: ")) {
        throw new NotSupportedConstructException("The construct is not supported: " + s);
      }
    }
    throw new NotSupportedConstructException(obj);
  }

  /**
   * returns true, if obj1 and obj2 have the same VDM base class
   *
   * @param obj1 VDM value.
   * @param obj2 VDM value.
   * @return returns true, if obj1 and obj2 have the same VDM base class.
   */
  public static boolean samebaseclass(Object obj1, Object obj2) {
    if ((obj1 == null) || (obj2 == null) ) {
      return false;
    }

    Class c1 = obj1.getClass();
    Class c2 = obj2.getClass();
    if (c1.getName().equals("java.lang.Integer") ||
        c1.getName().equals("java.lang.Long") ||
        c1.getName().equals("java.lang.Double") ||
        c1.getName().equals("java.lang.Character") ||
        c1.getName().equals("java.lang.Boolean") ||
        c1.getName().equals("jp.vdmtools.VDM.Tuple") ||
        c1.getName().endsWith("Token") ||
        c1.getName().startsWith("quotes") ||
        c2.getName().equals("java.lang.Integer") ||
        c2.getName().equals("java.lang.Long") ||
        c2.getName().equals("java.lang.Double") ||
        c2.getName().equals("java.lang.Character") ||
        c2.getName().equals("java.lang.Boolean") ||
        c2.getName().equals("jp.vdmtools.VDM.Tuple") ||
        c2.getName().endsWith("Token") ||
        c2.getName().startsWith("quotes")) {
      return false;
    }

    Class interfs[] = c1.getInterfaces();
    for (int i = 0; i < interfs.length; i++) {
      if (interfs[i].getName().equals("jp.vdmtools.VDM.Record")) {
        return false;
      }
    }

    Class interfs2[] = c2.getInterfaces();
    for (int i = 0; i < interfs2.length; i++) {
      if (interfs2[i].getName().equals("jp.vdmtools.VDM.Record")) {
        return false;
      }
    }

    Class base1 = c1;
    Class base2 = c2;

    while (!(isbaseclass(base1))) {
      base1 = base1.getSuperclass();
    }

    while (!(isbaseclass(base2))) {
      base2 = base2.getSuperclass();
    }
    return base1 == base2;
  }

  /**
   * returns true, if obj is a base class (VDM++),
   * does not work together with MI in VDM++ that is translated to
   * a class and interface structure in Java
   * does not work together with concurrency
   *
   * @param c VDM class.
   * @return returns true, if obj is a base class (VDM++).
   */
  public static boolean isbaseclass(Class c) {

    Class superc = c.getSuperclass();
    if (superc == null) {
      return false;
    }
    return superc.getName().equals("java.lang.Object");
  }

  /**
   * returns true, if obj1 and obj2 are instances of the same VDM class
   *
   * @param obj1 VDM value.
   * @param obj2 VDM value.
   * @return returns true, if obj1 and obj2 are instances of the same VDM class
   */
  public static boolean sameclass(Object obj1, Object obj2) {
    if ((obj1 == null) || (obj2 == null) ) {
      return false;
    }

    Class c1 = obj1.getClass();
    Class c2 = obj2.getClass();
    if (c1.getName().equals("java.lang.Integer") ||
        c1.getName().equals("java.lang.Long") ||
        c1.getName().equals("java.lang.Double") ||
        c1.getName().equals("java.lang.Character") ||
        c1.getName().equals("java.lang.Boolean") ||
        c1.getName().equals("jp.vdmtools.VDM.Tuple") ||
        c1.getName().endsWith("Token") ||
        c1.getName().startsWith("quotes") ||
        c2.getName().equals("java.lang.Integer") ||
        c2.getName().equals("java.lang.Long") ||
        c2.getName().equals("java.lang.Double") ||
        c2.getName().equals("java.lang.Character") ||
        c2.getName().equals("java.lang.Boolean") ||
        c2.getName().equals("jp.vdmtools.VDM.Tuple") ||
        c2.getName().endsWith("Token") ||
        c2.getName().startsWith("quotes")) {
      return false;
    }

    Class interfs[] = c1.getInterfaces();
    for (int i = 0; i < interfs.length; i++) {
      if (interfs[i].getName().equals("jp.vdmtools.VDM.Record")) {
        return false;
      }
    }

    Class interfs2[] = c2.getInterfaces();
    for (int i = 0; i < interfs2.length; i++) {
      if (interfs2[i].getName().equals("jp.vdmtools.VDM.Record")) {
        return false;
      }
    }

    return (c1 == c2);
  }

  /**
   * clones the obj.
   *
   * @param obj VDM value for clone.
   * @return a clone of input VDMvalue.
   */
  public static Object clone(Object obj) {
    if (obj == null) {
       return null;
    }

    if (!(obj instanceof Cloneable)) {
      return obj;
    }

    // clone compound datatypes
    if (obj instanceof Tuple) {
      return ((Tuple)obj).clone();
    }

    if (obj instanceof Record) {
      return ((Record)obj).clone();
    }

    if (obj instanceof List) {
      if (obj instanceof Vector) {
        return ((Vector)obj).clone();
      }
      if (obj instanceof ArrayList) {
        return ((ArrayList)obj).clone();
      }
      else {
        return ((Vector)obj).clone();
      }
    }

    if (obj instanceof Set) {
      if (obj instanceof HashSet) {
        return ((HashSet)obj).clone();
      }
      else {
        return ((HashSet)obj).clone();
      }
    }

    if (obj instanceof Map) {
      if (obj instanceof HashMap) {
        return ((HashMap)obj).clone();
      }
      else {
        return ((HashMap)obj).clone();
      }
    }

    return obj;
    // clone of primitive datatypes (Integer, Double, Boolean,
    // String, Characeter, Token) and classes not necessary
  }

  /**
   * Returns true, if the two objects are equal. Note: An Integer
   * and a Double are equal if they have the same numeric value.
   *
   * @param obj1 VDM value.
   * @param obj2 VDM value.
   * @return returns true, if the two objects are equal.
   */
  public static boolean equals(final Object obj1, final Object obj2) {
    if ((obj1 == null) && (obj2 == null)) {
      return true;
    }
    if ((obj1 == null) || (obj2 == null)) {
      return false;
    }
    if ((obj1 instanceof Number) && (obj2 instanceof Number)) {
      return (((Number) obj1).doubleValue() == ((Number) obj2).doubleValue());
    }
    if ((obj1 instanceof String) && (obj2 instanceof List)) {
      return UTIL.equals(UTIL.ConvertToList(obj1), obj2);
    }
    if ((obj2 instanceof String) && (obj1 instanceof List)) {
      return UTIL.equals(UTIL.ConvertToList(obj2), obj1);
    }
    if ((obj1 instanceof List) && (obj2 instanceof List)) {
      return UTIL.listEquals((List) obj1, (List) obj2);
    }
    if ((obj1 instanceof Set) && (obj2 instanceof Set)) {
      return UTIL.setEquals((Set) obj1, (Set) obj2);
    }
    if ((obj1 instanceof Map) && (obj2 instanceof Map)) {
      return UTIL.mapEquals((Map) obj1, (Map) obj2);
    }
    if (!(obj1.getClass() == obj2.getClass())) {
      return false;
    }
    else {
      return obj1.equals(obj2);
    }
  }

  private static boolean listEquals (List l1, List l2) {
    if (l1.size() != l2.size()) {
      return false;
    }
    else {
      boolean equal = true;
      int s = l1.size();
      for (int i = 0; i < s && equal; i++) {
        equal = (UTIL.equals(l1.get(i), l2.get(i)));
      }
      return equal;
    }
  }

  @SuppressWarnings("unchecked")
  private static boolean setEquals (Set s1, Set s2) {
    if (s1.size() != s2.size()) {
      return false;
    }
    else {
      boolean forall = true;
      Set s3 = new HashSet(s2);
      for (Iterator iter = s1.iterator(); iter.hasNext() && forall;) {
        Object o1 = iter.next();
        boolean exists = false;
        Set s4 = s3;
        for (Iterator iter2 = s4.iterator(); iter2.hasNext() && !exists;) {
          Object o2 = iter2.next();
          exists = UTIL.equals(o1, o2);
          if(exists) {
            s3.remove(o2);
          }
        }
        forall = exists;
      }
      return forall;
    }
  }

  @SuppressWarnings("unchecked")
  private static boolean mapEquals (Map m1, Map m2) {
    if (m1.size() != m2.size()) {
      return false;
    }
    else {
      boolean forall = true;
      Set ks1 = new HashSet(m1.keySet());
      Set ks2 = new HashSet(m2.keySet());
      for (Iterator iter = ks1.iterator(); iter.hasNext() && forall;) {
        Object k1 = iter.next();
        boolean exists = false;
        Set ks3 = ks2;
        for (Iterator iter2 = ks3.iterator(); iter2.hasNext() && !exists;) {
          Object k2 = iter2.next();
          exists = UTIL.equals(k1, k2) && UTIL.equals(m1.get(k1), m2.get(k2));
          if(exists) {
            ks2.remove(k2);
          }
        }
        forall = exists;
      }
      return forall;
    }
  }

  /**
   * converts a instance to an Integer.
   * A VDMRunTimeException is thrown when the instance has no integer
   * value.
   *
   * @param n VDM value.
   * @return Integer value.
   * @throws VDMRunTimeException if the instance has no integer value.
   */
  public static Integer NumberToInt(Object n) throws VDMRunTimeException {
    if (n==null) {
      return null;
    }
    if (n instanceof Number) {
      if (((Number) n).intValue() == ((Number) n).doubleValue()) {
        return Integer.valueOf(((Number) n).intValue());
      }
      else {
        throw new VDMRunTimeException("<UTIL.NumberToInt>: number is not int " + n.toString());
      }
    }
    else {
      return (Integer) n;
    }
  }

  /**
   * converts a instance to an Short.
   * A VDMRunTimeException is thrown when the instance has no integer value.
   *
   * @param n VDM value.
   * @return Short value.
   * @throws VDMRunTimeException if the instance has no integer value.
   */
  public static Short NumberToShort(Object n) throws VDMRunTimeException {
    if (n == null) {
      return null;
    }
    if (n instanceof Number) {
      if (((Number) n).intValue() == ((Number) n).doubleValue()) {
        return Short.valueOf(((Number) n).shortValue());
      }
      else {
        throw new VDMRunTimeException("<UTIL.NumberToShort>: number is not int " + n.toString());
      }
    }
    else {
      return (Short) n;
    }
  }

  /**
   * converts a instance to a Long.
   * The VDMRunTimeException exception is thrown when the instance has no integer value.
   *
   * @param n VDM value.
   * @return Long value.
   * @throws VDMRunTimeException if the instance has no integer value.
   */
  public static Long NumberToLong(Object n) throws VDMRunTimeException {
    if (n == null) {
      return null;
    }
    if (n instanceof Number) {
      if (((Number) n).intValue() == ((Number) n).doubleValue()) {
        return Long.valueOf(((Number) n).longValue());
      }
      else {
        throw new VDMRunTimeException("<UTIL.NumberToLong>: number is not int " + n.toString());
      }
    }
    else {
      return (Long) n;
    }
  }

  /**
   * converts a instance to a Double.
   *
   * @param n VDM value.
   * @return Double value.
   */
  public static Double NumberToReal(Object n) {
    if (n == null) {
      return null;
    }
    return Double.valueOf(((Number) n).doubleValue());
  }

  /**
   * converts a String to a List.
   *
   * @param s VDM value.
   * @return a List.
   */
  @SuppressWarnings("unchecked")
  public static List ConvertToList(Object s) {
    if (s instanceof String) {
      List v = UTIL.createList();
      char cs[] = ((String) s).toCharArray();
      for (int i = 0; i < cs.length; i++) {
        v.add(Character.valueOf(cs[i]));
      }
      return v;
    }
    else {
      return (List) s;
    }
  }

  /**
   * converts a List/String to a String.
   *
   * @param s VDM value.
   * @return a String.
   */
  public static String ConvertToString(Object s) {
    if (s instanceof String || s == null) {
      return (String) s;
    }
    else if (s instanceof List) {
      StringBuffer str_s = new StringBuffer();
      List l = (List) s;
      boolean succ = true;
      for (Iterator iter = l.iterator(); iter.hasNext() && succ;) {
        Object c = iter.next();
        if (c instanceof Character) {
          str_s.append(c);
        }
        else {
          succ = false;
        }
      }
      if (!succ) {
        return "";
      }
      else {
        return str_s.toString();
      }
    }
    else {
      return s.toString();
    }
  }

  /**
   * reverse String.
   *
   * @param s VDM value.
   * @return a String.
   */
  public static String ReverseString(Object s) {
    StringBuffer str_s = new StringBuffer();
    if (s instanceof String) {
      char cs[] = ((String) s).toCharArray();
      for (int i = cs.length - 1; i >= 0; i--) {
        str_s.append(Character.valueOf(cs[i]));
      }
    }
    return str_s.toString();
  }

  /**
   * reverse List.
   *
   * @param s VDM value.
   * @return a List.
   */
  @SuppressWarnings("unchecked")
  public static List Reverse(Object s) {
    List v = UTIL.createList();
    if (s instanceof List) {
      List l = (List) s;
      for (int i = l.size() - 1; i >= 0; i--) {
        v.add(l.get(i));
      }
    }
    return v;
  }

  /**
   * returns true if obj is a Number and has an integer value.
   *
   * @param obj VDM value.
   * @return returns true if obj is a Number and has an integer value.
   */
  public static boolean IsInteger(Object obj) {
    return ((obj instanceof Number) &&
            (((Number) obj).intValue() == ((Number) obj).doubleValue()));
  }

  /**
   * returns true if obj is a Number and has an nat value.
   *
   * @param obj VDM value.
   * @return returns true if obj is a Number and has an nat value.
   */
  public static boolean IsNat(Object obj) {
    return IsInteger(obj) && (((Number) obj).longValue() >= 0);
  }

  /**
   * returns true if obj is a Number and has an nat1 value.
   *
   * @param obj VDM value.
   * @return returns true if obj is a Number and has an nat1 value.
   */
  public static boolean IsNatOne(Object obj) {
    return IsInteger(obj) && (((Number) obj).longValue() > 0);
  }

  /**
   * returns true if obj is an Number.
   *
   * @param obj VDM value.
   * @return returns true if obj is an Number.
   */
  public static boolean IsReal(Object obj) {
    return (obj instanceof Number);
  }

  /**
   * permutes a list
   *
   * @param s list of VDM value.
   * @return a set of permuted list.
   */
  @SuppressWarnings("unchecked")
  public static Set Permute (List s) {
    //VDMCompare vdmComp = new VDMCompare();
    Set perm = UTIL.createSet();
    if (s.size() == 0) {
      return perm;
    }
    if (s.size() == 1) {
      List elem = UTIL.createList();
      try {
        elem.add(s.get(0));
      } catch (IndexOutOfBoundsException e) {}
      perm.add(elem);
      return perm;
    }

    Object obj = null;
    try {
      obj = s.get(0);
    } catch (IndexOutOfBoundsException e) {}

    List seq = (List)clone(s);
    try {
      seq.remove(0);
    } catch (IndexOutOfBoundsException e) {}

    Set ss = Permute(seq);
    for (int i = 0; i < s.size(); i++) {
      for (Iterator itr = ss.iterator(); itr.hasNext();) {
        List seq1 = (List)(clone(itr.next()));
        seq1.add(i,obj);
        perm.add(seq1);
      }
    }
    return perm;
  }

  @SuppressWarnings("unchecked")
  public static List Sort(Set s) {
    List v = UTIL.createList();
    VDMCompare compare = new VDMCompare();
    Set ts = new TreeSet(compare);
    ts.addAll(s);
    v.addAll(ts);
    return v;
  }

  @SuppressWarnings("unchecked")
  public static List Sort2(List s) {
    List ss = UTIL.createList();
    List help = (List)clone(s);

    for (int j = 0; j < s.size(); j++) {
      Object maxi = null;
      try {maxi = help.get(0);} catch (IndexOutOfBoundsException e) {}
      for (Iterator e = help.iterator(); e.hasNext();) {
        Object obj = e.next();
        if (IsInteger(obj)) {
          Long I = Long.valueOf(obj.toString());
          if (maxi instanceof Integer) {
            if (I.intValue() < ((Integer) maxi).intValue()) {
               maxi = I;
            }
          }
          if (maxi instanceof Long) {
            if (I.longValue() < ((Long) maxi).longValue()) {
              maxi = I;
            }
          }
          if (maxi instanceof Character) {
            maxi = I;
          }
        }
        else {
          if (obj instanceof Character) {
            Character C;
            C = (Character) obj;
            if (maxi instanceof Character) {
              if (Character.getNumericValue(C.charValue()) <
                  Character.getNumericValue(((Character) maxi).charValue())) {
                maxi = C;
              }
            }
          }
        }
      }
      ss.add(maxi);
      help.remove(maxi);
    }
    return ss;
  }

  public static boolean Contains(Set s, Object o) {
    if (!s.contains(o)) {
      if (o instanceof Number) {
        double val = ((Number)o).doubleValue();
        for (Iterator it = s.iterator(); it.hasNext();) {
          Object e = it.next();
          if (e instanceof Number) {
            if (val == ((Number)e).doubleValue()) {
              return true;
            }
          }
        }
      }
      return false;
    }
    return true;
  }

  public static List createList() {
    return UTIL.createList(Collections.EMPTY_LIST);
  }

  @SuppressWarnings("unchecked")
  public static List createList(Collection c) {
    return new ArrayList(c);
  }

  public static Set createSet() {
    return UTIL.createSet(Collections.EMPTY_SET);
  }

  @SuppressWarnings("unchecked")
  public static Set createSet(Collection c) {
    return new HashSet(c);
  }

  public static Map createMap() {
    return createMap(Collections.EMPTY_MAP);
  }

  @SuppressWarnings("unchecked")
  public static Map createMap(Map m) {
    return new HashMap(m);
  }

  public static void SetRandomSeed(long s) {
    rseed = s;
    if (s >= 0) {
      rnd.set_seed(rseed);
    }
  }

  @SuppressWarnings("unchecked")
  public static List GetRandomSequence(Number n) {
    ArrayList l = new ArrayList();
    for (int i = 1; i <= n.intValue(); i++) {
      l.add(Integer.valueOf(i));
    }
    if ((rseed < 0) || (l.size() <= 1)) {
      return l;
    }
    List res = new ArrayList();
    while(l.size() > 1) {
      int idx = rnd.get_random(l.size());
      res.add(l.remove(idx));
    }
    res.add(l.get(0));
    return res;
  }
}

