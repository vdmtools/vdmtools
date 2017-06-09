class C {
  public int i=3;
  public int j=4;
}

class B {
  public int i=1;
  public int j=2;
  public C c=new C();
}

class A {
  B b;

  public int[] Test() {
    int k[]=new int[4];

    b=new B();

    k[0]=b.i;
    k[1]=b.j;
    k[2]=b.c.i;
    k[3]=b.c.j;

    return k;
  }
}
