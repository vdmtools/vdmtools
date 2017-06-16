class  A{
  public int [] Test()
  {
    int i, i1, i2, i3, j=0, k=0;

    i= j==0? 2-1:2-2;

    i1= j==1? 2-1:2-2;

    i2= j==1? 1 - k++ : 1 - ++k;

    i3= j==1? 1 - k++ : 1 - ++k;

    int ii[]={j, i, i1, i2, i3, k};

    return ii;
  }
}