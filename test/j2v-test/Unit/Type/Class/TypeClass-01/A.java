class B {
}

class  A {
  public Object[] a(Object[] objs) { return objs;}

  public int[] Test()
  {
    int kk[]=new int[2];

    B bb[]={new B(), new B()};

    Object objs[] = a(bb);

    return kk;
  }
}
