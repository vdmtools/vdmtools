class A {
  int jj[][]={{},{}};
  int jjj[][][]={{{}},{{}},{{}}};

  void initJJ(int[] j) { this.jj=new int[][] {j}; }
  void initJJJ(int[][] jj) { this.jjj=new int[][][] {jj}; }

  public int[][] Test() {
    int ii[][]={{},{},{},{}};

    int i=0;

    ii[0]=new int[] {1,2,3,4,5};
    ii[1]=new int[] {6,7,8,9,++i};

    initJJ(new int[] {11,12,13});  
    ii[2]=jj[0];

    initJJJ(new int[][] {{11},{12},{13}});  
    ii[3]=jjj[0][0];

    return ii;
  }
}