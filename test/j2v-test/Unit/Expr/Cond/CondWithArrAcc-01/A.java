class  A{
  public int [] Test()
  {
    int ii[]={1,2};
    int jj[]={3,4};
    int ll[][]={{5,6,7},{7,8,10}};
    int mm[][]={{11,12,13},{14,15,16}};

    int kk[]=new int[15];

    int i=0;

    kk[0]=(true ? ii:jj)[i++];
    kk[1]=(false ? ii:jj)[i++];
    kk[2]=i++;

    i=0;
    kk[3]=(true ? ll[i++]:mm[i++])[i++];
    i=0;
    kk[4]=(false ? ll[i++]:mm[i++])[i++];
    kk[5]=i++;

    kk[6]=i;

    B b1=new B(), b2=new B();
    kk[7]=(true? b1:b2).i;      // VDM++ hides .i and assumes local i!
    
    return kk;
  }
}

class B {
  public int i=0;
}