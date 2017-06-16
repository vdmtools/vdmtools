class A {
  int s(int i) {
    int res=-10;

    switch(i) {
      case 1:
        res=1;
      break;
      case 2:
        res=2;
      break;
      case 3:
        res=3;
      break;
      case 4:
        if(false) res=4; else break;
      break;
      default:
        res=5;
      break;
    }

    return res;
  }

  public int[] Test() {
    int k[]={
      s(1),
      s(2),
      s(3),
      s(4),
      s(5),
      s(6),
      s(7),
      s(8),
      s(9),
      s(10),
    };
    return k;
  }
}
