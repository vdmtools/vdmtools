package jp.vdmtools.VDM;

import java.util.Collections;
import java.util.List;
import java.util.ArrayList;
import java.util.Set;
import java.util.HashSet;
import java.util.Map;
import java.util.HashMap;

/** 
 * Default Collection for VDM java
 */

public class DefaultCollection implements CollectionFactory {

  /** create List Object.
   */

  @SuppressWarnings("unchecked")
  public List createList(boolean conc)
  {
    if (conc)
      return Collections.synchronizedList(new ArrayList());
    else
      return new ArrayList();
  }

  /** create Set Object.
   */

  @SuppressWarnings("unchecked")
  public Set createSet(boolean conc)
  {
    if (conc)
      return Collections.synchronizedSet(new HashSet());
    else
      return new HashSet();
  }

  /** create Map Object.
   */

  @SuppressWarnings("unchecked")
  public Map createMap(boolean conc)
  {
    if (conc)
      return Collections.synchronizedMap(new HashMap());
    else
      return new HashMap();
  }

  /** clones List Object.
   */

  public Object cloneCollection(Object o) throws CGException
  {
    if (o instanceof ArrayList) {
      return ((ArrayList)o).clone();
    }
    else if (o instanceof HashSet) {
      return ((HashSet)o).clone();
    }
    else if (o instanceof HashMap) {
      return ((HashMap)o).clone();
    }
    else
      throw new VDMRunTimeException("Not expected object found");
  }
}

