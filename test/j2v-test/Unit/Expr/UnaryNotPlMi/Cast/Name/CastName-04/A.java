class B {
  public int i=123;
}

class  A {
  public Object bb() { return new B();}
  public int aa(B b) { return 1;}

  public int [] Test()
  {
    int ii[]=new int[8];

    B b=(B)bb();

    ii[0]=b.i;

    ii[1]=aa((B)bb());    

    return ii;
  }
}
