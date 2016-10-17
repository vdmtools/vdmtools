class  A{
  public int [] Test()
  {
    int ii[]=new int[5];

    int i;

    for(i=0; i<5; i++) ii[i]=i;

    for(i=0; i<5; i++)
    {
      ii[i]=i;
    }

    i--;

    for(; i>=0; i--)
    {
      ii[i]=i;
    }

    for(int j=0; j<5; j++)
    {
      ii[j]=j;
    }

    return ii;
  }
}
