class C {
  public int ii[]={1,2};
}

class A {
  C c;

  public int[] Test() {
    int k[]=new int[2];

    c=new C();

    k[0]=c.ii[0];
    k[1]=c.ii[1];

    return k;
  }
}
