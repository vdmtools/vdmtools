class BB
{
  public int a1(CC c, int i) {return 0;}
  public int a1(BB b, CC c ) {return 1;}
}

class CC
{
}

class A extends BB
{
  public int a1(CC c, int i) {return 0;}
  public int a1(BB b, CC c ) {return 1;}

  public int[] Test() {
    int k[]=new int[6];
    
    BB b=new BB();

    k[0]=b.a1(null, 0);
    k[1]=b.a1(null, new CC());

    k[2]=a1(null, 0);
    k[3]=a1(null, new CC());

    k[4]=a1((CC)null, 0);
    k[5]=a1((BB)null, new CC());

    return k;
  }
}