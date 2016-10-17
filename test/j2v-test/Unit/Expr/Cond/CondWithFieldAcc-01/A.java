class  A{
  public int Test()
  {
    int j;

    B b1=new B(), b2=new B();
    
    j=(true? b1:b2).i;

//  It seems that VDM does not notice FieldSelectExpr.
//  It tries to get i's value instead. 
//
//  int i; i=(true? b1:b2).i;
    
    return j;
  }
}

class B {
  public int i=0;
}