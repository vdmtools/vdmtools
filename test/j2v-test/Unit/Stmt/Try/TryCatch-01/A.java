class B extends Exception
{
}

class  A
{
  int [] m(int i) {
    int [] res=new int[4];

    try
    {
      if( i==0 ) {
        throw new B();
      } else if ( i==1 ) {
        throw new Exception();
      } else {
        res[0] = 0;
      }
    }
    catch(B b1)
    {
      res[1]=1;
    }
    catch(Exception b2)
    {
      res[2]=2;
    }
    finally
    {
      res[3]=3;
    }

    return res;
  }

  public int[][] Test() {
    int ii[][]=new int[3][4];

    ii[0]=m(0);
    ii[1]=m(1);
    ii[2]=m(2);

    return ii;
  }
}