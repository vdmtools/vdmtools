class A
{
  int i=0;
  int j=i;
  
  public int[] Test() {
    int k[]=new int[6];

    k[0]=i;
    k[1]=j;

    return k;
  }
}
