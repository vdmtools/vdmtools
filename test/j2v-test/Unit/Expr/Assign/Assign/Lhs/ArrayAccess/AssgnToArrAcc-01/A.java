class A {
  
  int[] GetArr() {
    int jj[]={1,2,3};
    return jj;
  }

  public int[] Test() {
    int k[]=new int[11];
    int k1[]=new int[11];
    int i=1;

    k[0]=1;
    k[1]=i++;
    k[i]=i;
    i=4;
    k[i]=--i;

//    ((new A()).GetArr())[3]=0;   Not possible in VDM++

    (true ? k:k1)[5]=555; 

    k[6]=((true ? k:k1)[7]=777)+1;   

    k[8]=(true ? true ? k1 : k : k1)[7];

    i=9;
    (true ? k:k1)[i++]=999; 

/*
    int ll[][]={{1,2,3},{2,4,8},{5,6,7}};
    i=0; int j=0;
    (true ? ll[i++]:ll[j++])[0]=999; 
*/
    return k;
  }
}
