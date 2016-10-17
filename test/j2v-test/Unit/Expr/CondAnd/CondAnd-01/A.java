class A { 
  int i;

  public boolean [] Test()
  {
    boolean b[]= {
      true && true,
      true && false,
      false && true,
      false && false,
      false && ((1./0)==0),
      ( true ? 3 == 3.0 : 1 == 1 ) &&
      ( false ? 3== 4   : 3 == 9 ),
      false && ( true ? true : false )
    };
   
    return b;
  }
}
