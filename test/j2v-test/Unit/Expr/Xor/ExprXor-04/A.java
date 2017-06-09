class  A{
  boolean bb(boolean b) { return b; }

  public boolean [] Test()
  {
    boolean b1, b=false;

    if (b ^ (1 > 0)) ;

    A a =new A();

    boolean ii[]=new boolean[6];

    b1=false;

    b=b1^bb(false); ii[0]=b;
    b=b1^bb(true); ii[1]=b;

    b1=true;

    b=b1^bb(true); ii[2]=b;
    b=b1^bb(false); ii[3]=b;

    b=b1^a.bb(true); ii[4]=b;
    b=b1^a.bb(false); ii[5]=b;

    if (b ^ a.bb(false)) ;

    return ii;
  }
}