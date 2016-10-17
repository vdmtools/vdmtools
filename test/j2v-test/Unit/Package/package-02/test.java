import a.b.*;
import java.io.*;

public class Test {
	
  static void test (boolean b) {
    cc c = new cc ();
    a.b.cc g = c;
    cc.cd h = (new cc ()).new cd();
    a.b.cc.cd i = h;
    System.out.println (c);
    System.out.println (c.d);
    System.out.println (c.d.e);
    System.out.println (c.d.e.f);
    System.out.println (g.d.e.f);
    System.out.println ((b ? h : i).e.f);
    System.out.println ((b ? h.e : i.e).f);
    System.out.println (h.e.f);
  }

  public static void main(String[] args) 
  {
    test (true);
  }
}
