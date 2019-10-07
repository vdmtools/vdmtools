import jp.vdmtools.VDM.UTIL;
import jp.vdmtools.VDM.VDMSpecException;

public class CGEX {
  public static void main(String[] args) {
    try {
      A obj = new A();
      System.out.println(UTIL.toString(obj.Test()));
    }
    catch (Exception e) {
      if (e instanceof VDMSpecException) {
        System.out.println("exit " + UTIL.toString(((VDMSpecException)e).getValue()));
      }
      else {
        System.out.println(e.getMessage());
        e.printStackTrace(System.err);
      }
    }
  }
}

