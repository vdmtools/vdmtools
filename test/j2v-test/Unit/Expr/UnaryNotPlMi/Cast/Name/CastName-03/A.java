class B {
  public int f(int i) { return 0;}
  public int g(int i) { return -1;}
}

class  A extends B {
  public int f(int i) { return 1;}

  public int [] Test()
  {
    int ii[]=new int[8];
    int i=0;

    A a=new A(); 
    B b=a;

    ii[0]=((A)a).f(0);
    ii[1]=((B)a).f(0);
    ii[2]=((A)b).f(0);
    ii[3]=((B)b).f(0);
    ii[4]=((A)a).g(0);
    ii[5]=((B)a).g(0);
    ii[6]=((A)b).g(0);
    ii[7]=((B)b).g(0);

    return ii;
  }
}
