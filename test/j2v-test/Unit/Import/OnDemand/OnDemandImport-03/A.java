import P.*;
import P.*;

public class A
{
  public int[] Test()
  {
    int ii[]=new int[5];
    
    P.B pb=new P.B();
//    B    b=new B(); 

    ii[0]=pb.i;
//    ii[1]=b.i;


    P.C pc=new P.C();   // P.C should be just C in VDM++
//    C    c=new C(); 

    ii[2]=pc.i;
//    ii[3]=c.i;

    D P=new D();
    
//    P.E=111;      No such possibility in VDM++

    ii[4]=P.E;      // P.E should remain P.E in VDM++

    return ii;
  }
}

class D {
  public int E=0;
}