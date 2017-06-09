class  A{
  public int [] Test()
  {
    int ii[]=new int[5];

    int i=0;

    for(;;) {
    
      ii[i]=i;
      if(i > 3) break;

      i=i+1;
    }

    return ii;
  }
}
