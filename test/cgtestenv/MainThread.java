import jp.vdmtools.VDM.*;

class MainThread extends Thread {
  private Finished f;

  MainThread(Finished nf){
      f = nf;
  }

  public void run() {
  try{
      Main m = new Main();
      System.out.println(UTIL.toString2(m.Test()));
      synchronized(f){
        f.Set(true);
        f.notify();
      }
      System.out.println("\n");          
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
