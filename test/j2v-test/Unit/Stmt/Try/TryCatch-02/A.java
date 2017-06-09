class E extends Exception
{}

class  A
{
  int [] m(int i) {
    int [] res=new int[5];

    try {
      if( i==0 ) {
        throw (Exception)null;
      } else if ( i==1 ) {
        throw new Exception();
      } else if ( i==2 ) {
        throw (E)null;
      } else if ( i==3 ) {
        throw new E();
      } else {
        res[0] = -1;
      }
    }
    catch(E e) {
      res[1]=1;
    }
    catch(Exception e) {
      res[2]=2;
    }
    finally {
      res[3]=3;
    }

    return res;
  }

  public int[][] Test() {
    int ii[][]=new int[5][5];

    ii[0]=m(0);
    ii[1]=m(1);
    ii[2]=m(2);
    ii[3]=m(3);
    ii[4]=m(4);

    return ii;
  }
}