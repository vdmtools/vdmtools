class B
{
  static public int MIN_RADIX=0;
  static public int MAX_RADIX=10;
}

class  A{
  public int parseInt(int radix)
  {
    int res = 0;
	  if (radix < B.MIN_RADIX) {
      res=-1;
	  }
	  if (radix > B.MAX_RADIX) {
      res=1;
	  }
    return res;
  }

  public int [] Test()
  {
    int ii[]={
      parseInt(-1),
      parseInt(5),
      parseInt(10),
      parseInt(11)
    };

    return ii;
  }
}