import jp.vdmtools.VDM.*;
import java.util.List;
import java.util.ArrayList;

public class MainSort {
  
  @SuppressWarnings("unchecked")
  public static void main(String[] args){ 
    try{
      List arr = new ArrayList();
      arr.add(new Integer(23));
      arr.add(new Integer(1));
      arr.add(new Integer(42));
      arr.add(new Integer(31));
      DoSort dos = new DoSort();      
      System.out.println("Evaluating Sort("+UTIL.toString(arr)+"):");
      List res = dos.Sort(arr);
      System.out.println(UTIL.toString(res));
    }
    catch (CGException e){
      System.out.println(e.getMessage());
    }
  }
}
