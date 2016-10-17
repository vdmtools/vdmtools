class A {
  public int[] Test() {
    int i=0;
    int k[]=new int[8];

    if(true)  k[0]=1; else k[0]=2;
    if(false) k[1]=1; else k[1]=2;

    if(true) {
      k[2]=1;
    } else k[2]=2;

    if(false) k[3]=1;
    else {
      k[3]=2;
    }

    if(true) {
      k[4]=1;
    } else {
      k[4]=2;
    }

    if(false) {
      k[5]=1;
    } else {
      k[5]=2;
    }

    if(i++>0) {
      k[6]=1;
    } else {
      k[6]=2;
    }

    return k;
  }
}