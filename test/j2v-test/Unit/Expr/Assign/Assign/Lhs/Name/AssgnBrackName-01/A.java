class  A{

  public int [] Test()
  {
    int ii[]=new int[5];
    int i;

    i=10;
    ii[0]=i;

    i=10;
    ii[1]=i;

//    (i)=i++;
    i=i++;
    ii[2]=i;

    return ii;
  }
}
