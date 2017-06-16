class A {
  public int[] Test() {
    int k[]=new int[15];
    
    int i;

    i=0;

    while(i<15) {
      k[i]=i;
      if(i>10) break;
      i++;
    }

    return k;
  }
}
