class B {
  public void ba() {}
  public int bb() {return 0;}
}

class A extends B {
  void a() {}
  int b() {return 0;}

  public int[] Test() {
    int k[]=new int[1];

    a();

    b();

    B b1=new B();

//    this.a();
//    super.ba();
    b1.ba();

//    this.b();
//    super.bb();
    b1.bb();

    return k;
  }
}