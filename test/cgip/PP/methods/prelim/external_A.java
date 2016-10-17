import jp.co.csk.vdm.toolbox.VDM.*;
import java.util.*;

public class external_A {

  A parent = null; 

  external_A(A parentA){
    parent = parentA;
  }

  Tuple impl_m1 (final Integer i_1, final Integer j_1) throws CGException  {
    Tuple t = new Tuple(2);
    parent.i = i_1;
    parent.j = j_1;
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
    parent.s.add(parent.i);
    parent.s.add(parent.j);
    parent.s.add(parent.c);
    parent.s.add(parent.d);
  }
};
