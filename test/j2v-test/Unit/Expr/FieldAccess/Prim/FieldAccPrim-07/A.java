class A {
  int bb[]={1,2,3,4};

  int length() { return 0; }

  public int[] Test() {
    int k[]=new int[7];
    int a1[]={1};
    int a2[]={1,2};
    int aa[][]={{1},{1,2,3}};
    A a=new A();

    int l = 999;

    k[0]=a1.length;
    k[1]=a2.length;
    k[2]=aa[1].length;  // Incorrect typerep of aa[1]
    k[3]=length();
    k[4]=a.length();
    k[5]=a.bb.length;
    k[6]=l;
    return k;
  }
}
