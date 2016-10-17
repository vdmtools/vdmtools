class C {
  public int i=0;
}

class B extends C {
  public int i=0;
}

class  A extends B {
  public int i=1;

  public int [] Test()
  {
    int ii[]=new int[6];

    ((C)this).i=10;
    ((B)this).i=11;
    ((A)this).i=12;

    ii[0]=((A)this).i;
    ii[1]=((B)this).i;
    ii[2]=((C)this).i;
    ii[3]=this.i;
    ii[4]=super.i;
    
    return ii;
  }
}
