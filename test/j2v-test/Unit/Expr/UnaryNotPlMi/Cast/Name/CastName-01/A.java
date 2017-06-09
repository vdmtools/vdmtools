class B {
  public int i=0;
}

class  A extends B {
  public int i=1;

  public int [] Test()
  {
    int ii[]=new int[6];

    A a=new A();
    B b=(B)new A();

    ii[0]=((B)a).i;
    ii[1]=((A)a).i;
    ii[2]=((B)b).i;
    ii[3]=((A)b).i;
    
    return ii;
  }
}
