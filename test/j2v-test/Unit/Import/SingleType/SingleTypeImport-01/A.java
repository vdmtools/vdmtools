import P.B;
import P.C;

public class A
{
  public int[] Test()
  {
    int ii[]=new int[4];
    
    P.B pb=new P.B(); 
    B    b=new B(); 

    ii[0]=pb.i;
    ii[1]=b.i;

    P.C pc=new P.C(); 
    C    c=new C(); 

    ii[2]=pc.i;
    ii[3]=c.i;

    return ii;
  }
}
