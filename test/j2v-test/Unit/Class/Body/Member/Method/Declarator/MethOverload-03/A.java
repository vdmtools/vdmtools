class B {
  public int bb[]={0,1,2,3,4,5};

  public int write(int i) { return 1; }
  public int write(char c) { return 2; }
  public int write(String c) { return 3; }
//  public int write(int i[]) { return 4; }   Static semantics complains!!!

  public void println() 
  {
    bb[0]=5;
  }

  public void println(int i) 
  {
    bb[1]=4;
  }

  public void println(char i) 
  {
    bb[2]=3;
  }
/*
  public void println(char ii[]) 
  {
    bb[1]=2;
  }
*/
}

class A {

  public int[] Test() {
    int ii[]={1,2,3,4};
    char cc[]={'1','2','3','4'};
    int k[]=new int[8];
    
    B b=new B();
    k[0]=b.write(1);
    k[1]=b.write('1');
    k[2]=b.write("1234");
//    k[3]=b.write(ii);
    b.println(1);
    b.println('1');
//    b.println(cc);
    b.println();

    k[4]=b.bb[0];
    k[5]=b.bb[1];
    k[6]=b.bb[2];
    k[7]=b.bb[3];

    return k;
  }
}