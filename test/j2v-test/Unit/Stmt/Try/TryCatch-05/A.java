abstract class B {
  abstract public int m(int i) throws Exception;

  public int m1(int j) {
    int i=j;
    try {
      while(true) { i=m(j); break; }
    }
    catch(Exception e) {
      i=-1111;
    }
    return i;
  }
}

class  A extends B
{
  public int m(int i) throws Exception
  {
    if(i==0) throw new Exception();
    else return i;
  }

  public int[] Test() {
    int ii[]=new int[7];
    try {
      ii[3]=m1(0);
      ii[4]=m1(1);
      ii[5]=m1(2);
      ii[0]=m(2);
      ii[1]=m(1);
      ii[2]=m(0);
    }
    catch(Exception e) {
      ii[6]=-111;
    }
    return ii;
  }
}