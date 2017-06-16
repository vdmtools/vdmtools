class  A {
  public int i=0;

  public A(int i, int j, int k) {}
  public A() {}

  public int [] Test()
  {
    int ii[]=new int[6];

    A a=new A();
    A a1=new A(1,2,3);
    
    A a2=new A(i++,i++,i++);

    ii[0]=a2.i;
    
    return ii;
  }
}
