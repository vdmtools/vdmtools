class  A{
  public int [] Test()
  {
    int ii[]=new int[5];

    int i=10; double r=10.5;

    ii[0]=(int)r;
    ii[1]=(int)10.5;
    ii[2]=(int)i;
    ii[3]=(int)r;
    ii[4]=(int)(i+10.5);
    
    return ii;
  }
}
