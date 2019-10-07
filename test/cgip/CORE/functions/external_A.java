import jp.co.csk.vdm.toolbox.VDM.*;
import java.util.*;

public class external_A {

  A parent = null; 

  external_A(A parentA){
    parent = parentA;
  }

  public Integer impl_g (final Integer var_1_1, final Integer var_2_2, 
                         final Set var_3_3) throws CGException{
    return Integer.valueOf(var_1_1.intValue() + var_2_2.intValue());
  } 
  public Tuple impl_h (final List var_1_1, final Tuple var_2_2) 
      throws CGException{
    Tuple t = new Tuple(var_1_1.size());
    int i = 1;
    for (Iterator enum = var_1_1.iterator(); enum.hasNext();){
      Object obj = enum.next();
      t.SetField(i,obj);
      i++;
    }
    return t;
  }
  
 public Integer impl_f1 () throws CGException{
   UTIL.RunTime("Implicit Function f1 has been called");
   return Integer.valueOf(0);
 } 
  
  public Vector impl_h2 (final Set var_1_1) throws CGException{
   Vector s = new Vector();
   for (Iterator enum = var_1_1.iterator(); enum.hasNext();){
     Object obj = enum.next();
        s.add(obj);       
   }
   return  s; 
  } 
}



