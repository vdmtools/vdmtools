class C {
  public int ii[]={3,4};
}

class A {
  C c1;
  C c2;

  public int[] Test() {
    int k[]=new int[6];

    c1=new C();
    c2=new C();

    k[0]=(true? c1:c2).ii[0];
    k[1]=(false? c1:c2).ii[1];

    int i=0, j=0;

    C cc[]={new C(),new C()};

    k[2]=(true? cc[j++]:cc[j++]).ii[i++];
    k[3]=(false? cc[j++]:cc[j++]).ii[i++];

    i=0;

    k[4]=cc[i++].ii[i++];
    k[5]=i;

    return k;
  }
}
