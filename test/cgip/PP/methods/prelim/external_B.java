import jp.co.csk.vdm.toolbox.VDM.*;
import java.util.*;

public class external_B {

  B parent = null;

  public external_B (B parentB) { parent = parentB; }
  
  Tuple impl_m1 (final Integer i, final Integer j) throws CGException  {
    Tuple t = new Tuple(2);
    parent.i = i;
    parent.j = j;
    t.SetField(1,parent.i).SetField(2,parent.j);
    return t;
  }

  Tuple impl_m2 (final Character ch1, final Character ch2) throws CGException  {
    Tuple t = new Tuple(2);
    parent.c = ch1;
    parent.d = ch2;
    t.SetField(1,parent.c).SetField(2,parent.d);
    return t;
  }

  void impl_m3 () throws CGException{ 
    if (parent.s == null) parent.s = new Vector();
    parent.s.add(parent.d);
    parent.s.add(parent.c);
    parent.s.add(parent.j);
    parent.s.add(parent.i);
  }
};
