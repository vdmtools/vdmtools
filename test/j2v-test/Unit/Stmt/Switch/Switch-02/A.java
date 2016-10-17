class A {
  int ii[]=new int[3];
  void howMany(int k) {
    switch (k) {
      case 1: ii[0] = 1; break;
      case 2: ii[1] = 1; break;
      case 3: ii[2] = 3; break;
      default:  ;  break;
    }
  }
  public int [] Test() {
     howMany(3);
     howMany(2);
     howMany(1);
     return ii;
  }
}
