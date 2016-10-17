class A {
  int a(int i, int j) {return i+j;}

  public int[] Test() {
    int k[]=new int[15];
    int l[]={0,1,2};
    
    int i=4, j=5;
/*
    i=4;

    while(i>=0) {
      k[i]=i;
      i--;
    }

    i=10;

    while((j=i)>=0) {
      k[i]=i;
      i--;
    }
*/
    i=13;

    while(i++>=10) {
      k[i]=--i;
      i--;
    }

    return k;
  }
}
