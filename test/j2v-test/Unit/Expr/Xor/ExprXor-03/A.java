class  A{
  boolean bb(boolean b) { return b; }

  public boolean [] Test()
  {
    boolean b1;

    boolean ii[]=new boolean[4];

    b1=false;

    ii[0]=b1^bb(false);
    ii[1]=b1^bb(true);

    b1=true;

    ii[2]=b1^bb(true);
    ii[3]=b1^bb(true);

    return ii;
  }
}