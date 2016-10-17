class A {
  public int[] Test() {
    int i=3;
    int k[]=new int[6];
    int l[]={0,1};

    k[0]=i;

    --i;

    k[1]=i;

    --i;

    k[2]=i;

    k[3]=l[0];

    --l[0];

    k[4]=l[0];

    i=1;

    --l[--i];

    k[5]=l[0];

    return k;
  }
}