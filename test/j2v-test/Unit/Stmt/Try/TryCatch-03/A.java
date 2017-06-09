class E extends Exception
{
}

class E1 extends E
{
}

class E2 extends E
{
}


class  A
{
  int [] m(int i) {
    int [] res=new int[5];

    try
    {
      if( i==0 ) {
//        throw null;   unreported exception <null>; must be caught or declared to be thrown
      } else if ( i==1 ) {
        throw (E1)null;
      } else if ( i==2 ) {
        throw (E2)null;
      } else {
        res[0] = 0;
      }
    }
    catch(E1 e1)
    {
      res[1]=1;
    }
    catch(E2 e2)    
    {
      res[2]=2;
    }
//    catch(E e)  //       !!! null is not caught
    catch(Exception e)
    {
      res[3]=3;
    }
    finally
    {
      res[4]=4;
    }

    return res;
  }

  public int[][] Test() {
    int ii[][]=new int[5][5];

    try {
      ii[0]=m(0);
      ii[1]=m(1);
      ii[2]=m(2);
      ii[3]=m(3);
    }
    catch(Exception e) {
      ii[4]=new int [] {999,999,999,999,999};
    }

    return ii;
  }
}