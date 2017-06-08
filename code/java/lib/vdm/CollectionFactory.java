package jp.vdmtools.VDM;

import java.util.List;
import java.util.Set;
import java.util.Map;

/** 
 * Collection Factory for VDM.
 */

public interface CollectionFactory {

  /** create List Object.
   *
   * @param conc concurrent option.
   * @return new List instance.
   */

  public List createList(boolean conc);

  /** create Set Object.
   *
   * @param conc concurrent option.
   * @return new Set instance.
   */

  public Set createSet(boolean conc);

  /** create Map Object.
   *
   * @param conc concurrent option.
   * @return new Map instance.
   */

  public Map createMap(boolean conc);

  /** clones List, Set, Map Object.
   *
   * @param o instance for clone.
   * @return a clone of input instance.
   * @throws CGException if can`t clone then throws CGException.
   */

  public Object cloneCollection(Object o) throws CGException;
}


