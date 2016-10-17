class  A{
  int a(int i, int j) {
    return j;
  }

  int b(int i, int j) {
    return i;
  }

  public int [] Test()
  {
    int kk[]=new int[12];

    int i=0, j=1;

//    kk[0]=(true ? a:b)(i,j);
//../j2v-test/Unit/Expr/MethodInvoc/Prim/MethInvPrim-01/A.java:16: ';' expected
//    kk[0]=(true ? a:b)(i,j);
//                    ^

    kk[0]=a(i,j);
    kk[1]=a(i,b(i,j));
    kk[2]=a(b(i,j),i);
    kk[3]=b(a(i,j),i);
    kk[4]=b(i,a(i,j));
    kk[5]=a(i++,++j);
    kk[6]=a(++i,b(i++,++j));
    kk[7]=a(b(i++,j++),++i);
    kk[8]=b(a(i,j++),i);
    kk[9]=b(--i,a(i,j));

    kk[10]=this.b(i,this.a(i,j));
    kk[11]=this.b(--i,this.a(--i,--j));
    
    return kk;
  }
}