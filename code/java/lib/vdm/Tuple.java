package jp.vdmtools.VDM;

import java.util.Enumeration;
import java.io.Serializable;

/**
 * This class models the VDM Product Type. It  can
 * be found in the file Tuple.java in the VDM package.
 * A tuple is essentially an array of fixed size. Therefore, the Tuple class 
 * has the following protected attribute: protected Object[] t;
 */
 

public class Tuple extends Object implements Cloneable, Serializable{
  
  protected Object[] value;

  /**
   * initializes a tuple with 0 fields.
   */

  public Tuple() {
    this.value = new Object[0];
  }
  
  /**
   * initializes a tuple with i fields.
   *
   * @param i field size of new instance.
   */

  public Tuple(int i) {
    this.value = new Object[i];
  }
 
  /** clones the tuple.
   *
   * @return a clone of this instance.
   */

  public Object clone() {
    Tuple newtuple = new Tuple(this.value.length);
    for (int i = 0; i < this.value.length; i++) {
      newtuple.value[i] = UTIL.clone(this.value[i]);
    }
    return newtuple;
  }

  /** returns true if the value of obj is equal to this sequence.
   *
   * @return true if this object is the same as the obj argument; false otherwise.
   */
  
  public boolean equals (Object obj) {
    if (obj == null) return false;
    if (!(obj instanceof Tuple)) return false;
    else {
      Tuple tu = (Tuple) obj;
      if ((tu.value.length) != (this.value.length)) return false;
      else {
        for (int i = 0; i < this.value.length; i++) {
          if (!UTIL.equals(this.value[i], tu.value[i])) 
            { 
              return false;
            }
        }
      }
    }
    return true;
  }

  public int hashCode() {
    int hash = 0;
    for (int i = 0; i < this.value.length; i++) {
	if (this.value[i] != null)
	    hash += this.value[i].hashCode();
    }
    return hash;
  }

  /** Modifies the i'th field of the tuple to obj. 
   * If i is not within the defined number of fields of this tuple, 
   * an "INDEX_OUT_OF_ RANGE" is thrown. This tuple is returned. 
   *
   * @param i index of field.
   * @param obj a instance to set i'th field.
   * @return a clone of this instance.
   * @throws INDEX_OUT_OF_RANGE If i is not within the defined number of fields of this tuple, 
   * an "INDEX_OUT_OF_ RANGE" is thrown. This tuple is returned. 
   */
  
  public Tuple SetField(int i, Object obj) throws INDEX_OUT_OF_RANGE {
    try{ this.value[i-1] = obj; }
    catch (ArrayIndexOutOfBoundsException e) {
      throw new INDEX_OUT_OF_RANGE("<Tuple.SetField>");
    }
    return this;
  }
  
  /**
   * returns the contents of the i'th field of this tuple. 
   * If i is not within the defined number of fields for the tuple, 
   * an exception is thrown.
   *
   * @param i index of field.
   * @return a instance of i'th field.
   * @throws INDEX_OUT_OF_RANGE If i is not within the defined number of fields for the tuple, 
   * an exception is thrown.
   */

  public Object GetField(int i) throws INDEX_OUT_OF_RANGE {
    Object obj; 
    try{ obj = this.value[i-1]; }
    catch (ArrayIndexOutOfBoundsException e) {
      throw new INDEX_OUT_OF_RANGE("<Tuple.GetField>");
    }
    return obj;
  }

  /**
   * returns the number of fields declared for the tuple.
   *
   * @return field size of this instance.
   */

  public int Length() {
    return this.value.length;
  }

  /**
   * returns a string containing an ASCII representation of the VDM value
   *
   * @return a string representation of the object.
   */

  public String toString() {
    String s = new String();
    s = s + "mk_(";
    for (int i = 0; i < this.value.length; i++) {
      s = s + UTIL.toString(this.value[i]);
      if (i < (this.value.length-1)) {s = s + ", "; }
    }
    s = s + ")";
    return s;
  }

  public static Tuple mk_(Object p1, Object p2, Object... args) {
    Tuple t = new Tuple( 2 + args.length );
    try {
      t.SetField(1, p1);
      t.SetField(2, p2);
      for (int i = 0; i < args.length; i++ )
        t.SetField(3 + i, args[i]);
    } catch (Exception e) {}
    return t;
  }

/*
  public static Tuple mk_(Object p1, Object p2) {
    Tuple t = new Tuple(2);
    try {
      t.SetField(1, p1);
      t.SetField(2, p2);
    } catch (Exception e) {}
    return t;
  }

  public static Tuple mk_(Object p1, Object p2, Object p3) {
    Tuple t = new Tuple(3);
    try {
      t.SetField(1, p1);
      t.SetField(2, p2);
      t.SetField(3, p3);
    } catch (Exception e) {}
    return t;
  }

  public static Tuple mk_(Object p1, Object p2, Object p3, Object p4) {
    Tuple t = new Tuple(4);
    try {
      t.SetField(1, p1);
      t.SetField(2, p2);
      t.SetField(3, p3);
      t.SetField(4, p4);
    } catch (Exception e) {}
    return t;
  }

  public static Tuple mk_(Object p1, Object p2, Object p3, Object p4,
                          Object p5) {
    Tuple t = new Tuple(5);
    try {
      t.SetField(1, p1);
      t.SetField(2, p2);
      t.SetField(3, p3);
      t.SetField(4, p4);
      t.SetField(5, p5);
    } catch (Exception e) {}
    return t;
  }

  public static Tuple mk_(Object p1, Object p2, Object p3, Object p4,
                          Object p5, Object p6) {
    Tuple t = new Tuple(6);
    try {
      t.SetField(1, p1);
      t.SetField(2, p2);
      t.SetField(3, p3);
      t.SetField(4, p4);
      t.SetField(5, p5);
      t.SetField(6, p6);
    } catch (Exception e) {}
    return t;
  }

  public static Tuple mk_(Object p1, Object p2, Object p3, Object p4,
                          Object p5, Object p6, Object p7) {
    Tuple t = new Tuple(7);
    try {
      t.SetField(1, p1);
      t.SetField(2, p2);
      t.SetField(3, p3);
      t.SetField(4, p4);
      t.SetField(5, p5);
      t.SetField(6, p6);
      t.SetField(7, p7);
    } catch (Exception e) {}
    return t;
  }

  public static Tuple mk_(Object p1, Object p2, Object p3, Object p4,
                          Object p5, Object p6, Object p7, Object p8) {
    Tuple t = new Tuple(8);
    try {
      t.SetField(1, p1);
      t.SetField(2, p2);
      t.SetField(3, p3);
      t.SetField(4, p4);
      t.SetField(5, p5);
      t.SetField(6, p6);
      t.SetField(7, p7);
      t.SetField(8, p8);
    } catch (Exception e) {}
    return t;
  }
*/


//    /**
//     * returns a string containing an ASCII representation of the VDM value. 
//     * A newline is written after each pair of the map.
//     */

//    package String toString2(){
//      String s = new String();
//      for (int i=0; i<t.length;i++){
//        s = s + UTIL.toString(t[i]);
//        if (i<(t.length-1)) {s=s+", \n";}
//      }
//      return s;
//    }
}

