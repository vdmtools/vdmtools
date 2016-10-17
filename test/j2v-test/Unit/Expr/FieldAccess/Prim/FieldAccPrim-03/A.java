class C {
  public int i=3;
  public int j=4;
}

class A {

  public int[] Test() {
    int k[]=new int[2];

    C cc[]=new C[1];

    cc[0]=new C();

    k[0]=cc[0].i;
    k[1]=cc[0].j;

    return k;
  }
}
