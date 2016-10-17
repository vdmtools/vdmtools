class  A{
  public int a(int i)
  {
    return i & 1;
  }


  public int [] Test()
  {
    int i=0;
    int j=1;

    int ii[]={
      0 & 0,
      0 & 1,
      0 & 2,
      0 & 3,
      1 & 0,
      1 & 1,
      1 & 2,
      1 & 3,
      2 & 0,
      2 & 1,
      2 & 2,
      2 & 3,
      3 & 0,
      3 & 1,
      3 & 2,
      3 & 3,
      a(3 & 3),
      a(i & j),
    };

    return ii;
  }
}