class A
{
  static int i=0;
  static {
    i=i+1;
  }
  
  public int[] Test() {
    int k[]=new int[6];

    k[0]=i;

    return k;
  }
}
