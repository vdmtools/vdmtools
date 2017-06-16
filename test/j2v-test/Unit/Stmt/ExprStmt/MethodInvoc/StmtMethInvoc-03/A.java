class BB
{
  public int i = 0;
  public void b() {i=1;}
}

class CC
{
  public static BB bb = new BB();
}


class A extends BB
{

  public int[] Test() {
    int k[]=new int[6];
    
    k[0]=CC.bb.i;
    CC.bb.b();
    k[1]=CC.bb.i;

    return k;
  }
}