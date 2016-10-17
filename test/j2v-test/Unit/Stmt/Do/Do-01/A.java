class A {
  public int[] Test() {
    int k[]=new int[15];
    
    int i;

    i=0;

    do {
      k[i]=i;
      if(i>10) break;
      i++;
    } while(i<15);

    return k;
  }
}
