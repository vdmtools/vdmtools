class  A{
  public boolean [] Test()
  {
    boolean ii[]={
      false & false,
      false & true,
      true & false,
      true & true,
      false & ((1./0)==0)
    };

    return ii;
  }
}