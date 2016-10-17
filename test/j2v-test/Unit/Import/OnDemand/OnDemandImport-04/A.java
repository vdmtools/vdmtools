import P.P1.*;
import P.P2.*;

public class A
{
  public int[] Test()
  {
    int ii[]=new int[5];
    
    P.P1.B pb=new P.P1.B();
//    B    b=new B(); 

    ii[0]=pb.i;
//    ii[1]=b.i;


    P.P2.C pc=new P.P2.C();   // P.P2.C should be just C in VDM++
//    C    c=new C(); 

    ii[2]=pc.i;
//    ii[3]=c.i;

    return ii;
  }
}
