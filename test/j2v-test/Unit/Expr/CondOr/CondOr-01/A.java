class A { 
  int i;

  public boolean [] Test()
  {
    boolean b[]= {
      true || true,
      true || false,
      false || true,
      false || false,
      true || true ? true : false,
      ( true ? 3 == 4 : true ) || 3 == 3
    };

   
    return b;
  }
}