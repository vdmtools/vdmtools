class C {
  public int i=0;
}

class A {
  public int[] Test() {
    int k[]=new int[2];

    C c=new C();

    k[0]=c.i;

//    c.i=1;    Not possible in VDM++

    k[1]=c.i;

    return k;
  }
}
