class A {
  public A() {}

  public int[] Test() {
    SortMachine sm=new SortMachine();
    int kk[]={-1,6,-2,5,-123,-7,9,1,-111};
    return sm.goSorting(kk);
  }

  public static void main()
  {
    int kk[]=(new A()).Test();

    for(int i=0; i<kk.length; i++) {
      System.out.println(kk[i]);
    }
  }

}