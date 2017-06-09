class B {
  public static int m1(int i){ return i;}
  public int m2(int i){return i;}
}

class A {
  static int i=0;

  static int m1(int i){ return i;}
  int m2(int i){return i;}

  public int[] Test() {
    int k[]=new int[6];
    
    A a=new A();
    B b=new B();

    k[0]=A.m1(100);
    k[1]=a.m2(200);

    k[2]=B.m1(300);
    k[3]=b.m2(400);

    return k;
  }
}