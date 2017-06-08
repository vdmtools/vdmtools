package jp.vdmtools.VDM;

import java.io.Serializable;

/** 
 * All record types in a VDM specification implements this interface.
 */

public interface Record extends Cloneable, Serializable {

  /** returns true, if the object is equal to this Record.
   * The equals method has to be implemented in subclasses (does
   * the codegenerator)
   *
   * @param obj the reference object with which to compare.
   * @return true if this object is the same as the obj argument; false otherwise.
   */

  public boolean equals(Object obj);

  /** returns a string containing an ASCII representation of the VDM
   * value. The toString method has to be implemented in subclasses (does
   * the codegenerator)
   *
   * @return a string representation of the object.
   */

  public String toString();

  /** clones the Record.
   * The clone method has to be implemented in subclasses (does
   * the codegenerator)
   *
   * @return a clone of this instance.
   */

  public Object clone(); 
}


