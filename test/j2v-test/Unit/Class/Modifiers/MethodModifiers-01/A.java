class A {
  int count=0;
  /* synchronized */ void bump() { count++; }
  static int classCount=0;
  static void classBump() {
         classCount++;
  }
  public int [] Test()
  {
    int ii[] = new int[4];
    ii[0]=classCount;
    ii[1]=count;
    classBump();
    bump();
    ii[2]=classCount;
    ii[3]=count;
    return ii;
  }
}
