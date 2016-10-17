class A {
  public int[][] Test() {
    int i=2, j=3;

    int k[][]=new int[++i][++j];

    i=0; j=1;

    k[++i][++j]=5;

    k[0][0]=k[0].length;

    return k;
  }
}