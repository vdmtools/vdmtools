class A {
  public int[] Test() {
    int k[]=new int[15];
    
    int j=0;

    while(j<10) {
      j=j+1;
    }

    int i;

    i=0;

    try {

      while(i<15) {
        k[i]=i;
        if(i>9) throw new Exception();
        if(i>10) break;
        i++;
      }

    }
    catch (Exception e) {

    }

    return k;
  }
}
