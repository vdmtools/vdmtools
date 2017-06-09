class B {
  static public int i=10;
  static public int j=11,kk=12;
}

class A {
  static int i=0;
  static int j=1,kk=2;

  public int[] Test() {
    int k[]=new int[6];
    
    k[0]=A.i;
    k[1]=A.j;
    k[2]=A.kk;

    k[3]=B.i;
    k[4]=B.j;
    k[5]=B.kk;

    return k;
  }
}