class A {
  int m(int i){ return i; }

  int ovl(double r){ return (int)r; }
//  int ovl(int i){ return i; }
  int ovl(char i){ return 0; }

  public int[] Test() {
    int k[]=new int[3];
    
    k[0]=m(1);
    k[1]=ovl(' ');
    k[2]=ovl(10.5);

    return k;
  }
}

/*

[ mk_JAVAERR`ErrMsg(
     10,
     4,
     7,
     "Error[10] : The overloading for the method 'ovl' cannot be translated to V
DM++\r",
     <ERROR> ),
  mk_JAVAERR`ErrMsg(
     10,
     11,
     10,
     "Error[9] : The Method 'ovl' was not found\r",
     <ERROR> ),
  mk_JAVAERR`ErrMsg(
     10,
     12,
     10,
     "Error[9] : The Method 'ovl' was not found\r",
     <ERROR> ) ]

*/