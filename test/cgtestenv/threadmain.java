import jp.dmtools.VDM.*;
import java.io.*;

public class threadmain {
  public static void main(String[] args){ 
    try{
      Finished f = new Finished(false);
      WatchDog w = new WatchDog(f);
      MainThread obj = new MainThread(f); 
      obj.start();
      w.start();
    }
//    catch (VDM_ERROR e){
//      System.out.println(e.getMessage());
//    }
    catch (ClassCastException e){
      System.out.println("Run-Time Error: Another type was expected");
    }               
    catch (Exception e){
      System.out.println(e.getMessage());
    }               
  }
}
