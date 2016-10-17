class C {
  public int ii[]={1,2};
}

class A {
  public int[] Test() {
    int k[]=new int[2];

    k[0]=new C().ii[0];
    k[1]=new C().ii[1];

    return k;
  }
}
