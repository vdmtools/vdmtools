class R extends Exception {}
class S extends Exception {}
class V extends Exception {}
class nonV extends Exception {}
class abstrE extends Exception {}

class  A
{
  int [] m(int iTry, int iCatch, int iFinally) {
    int [] res=new int[4];
    try {
      try {
        if( iTry==0 ) {
          res[0] = 1;
        } else if ( iTry==1 ) {
          throw new V();
        } else if ( iTry==2 ) {
          throw new nonV();
        } else if ( iTry==3 ) {
          throw new abstrE();
        } else if ( iTry==4 ) {
          throw new Exception();
        }
      }
      catch(V v) {
        if(iCatch==0) {
          res[1] = 1;
        } else if(iCatch==1) {
          throw new R();
        }
      }
      finally {
        if(iFinally==0) {
          res[2] = 1;
        } else if(iFinally==1) {
          throw new S();
        }
      }
    }
    catch(R r) {
      res[3] = 1;
    }
    catch(S r) {
      res[3] = 2;
    }
    catch(nonV nonv) {
      res[3] = 3;
    }
    catch(Exception e) {
      res[3] = 4;
    }
    return res;
  }

  public int[][] Test() {
    int ii[][]=new int[20][4];

    ii[0]=m(0,0,0);
    ii[1]=m(0,0,1);
    ii[2]=m(0,1,0);
    ii[3]=m(0,1,1);

    ii[4]=m(1,0,0);
    ii[5]=m(1,0,1);
    ii[6]=m(1,1,0);
    ii[7]=m(1,1,1);

    ii[8] =m(2,0,0);
    ii[9] =m(2,0,1);
    ii[10]=m(2,1,0);
    ii[11]=m(2,1,1);
 
    ii[12]=m(3,0,0);
    ii[13]=m(3,0,1);
    ii[14]=m(3,1,0);
    ii[15]=m(3,1,1);

    ii[16]=m(4,0,0);
    ii[17]=m(4,0,1);
    ii[18]=m(4,1,0);
    ii[19]=m(4,1,1);

    return ii;
  }
}