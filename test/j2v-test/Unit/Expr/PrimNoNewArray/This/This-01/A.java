class B {
  int i=888;
}

class  A {
  int i=999;

//  A a() {
//    return this /*A.this*/;
//  }

  int ii() {
    return this.i;
  }

  public int[] Test()
  {
    int kk[]={0, /*a().i, ii()*/};
    return kk;
  }
}
