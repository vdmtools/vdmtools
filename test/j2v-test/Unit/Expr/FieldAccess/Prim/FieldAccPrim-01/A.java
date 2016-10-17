class B {
  public int i=123;
  public int j=124;
}

class A {
  B b;

  public int[] Test() {
    int k[]=new int[2];

    b=new B();

    k[0]=b.i;
    k[1]=b.j;

    return k;
  }
}
