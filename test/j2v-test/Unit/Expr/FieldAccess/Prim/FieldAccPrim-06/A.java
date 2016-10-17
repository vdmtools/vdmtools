class B {
  protected int i=-999;
  protected int l[]={100,200,300};
}

class A extends B{
  int i=0;
  int l[]={1,2,3};

  public int[] Test() {
    int k[]=new int[14];
    int l[]=new int[3];

    int i=1;

    k[0]=this.i;
    k[1]=i;

    k[2]=l[0];
    k[3]=this.l[0];

    k[4]=this.i++;
    k[5]=this.l[0]++;

    k[8]=(k[6]=this.l[++i]++);

    k[9]=(k[7]=--this.l[i--]);

    k[10]=super.i++;
    k[11]=super.l[2]++;

    return k;
  }
}
