import P.B;

public class A
{
  public int[] Test()
  {
    int ii[]=new int[4];
    
    P.B pb=new P.B(); 
    B    b=new B(); 

    ii[0]=pb.i;
    ii[1]=b.i;
    ii[2]=b.a();
    ii[3]=pb.a();

    return ii;
  }
}
