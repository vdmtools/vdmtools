class B {
  protected int i=0;

  public B(int i, int j) { this.i=i+j; }
  public B(int i) { this.i=i; }
  public B() {}
}

class A extends B
{
  int i=1;

  public A(int i, int i1) {
//    super(i);
    super(i,i);
    this.i=i1;
  }

  public A(int i) {
    this(i, i+5);
  }

  public A() {
  }

  public int[] Test() {
    int k[]=new int[6];

    A a =new A();
    A a1=new A(10);
    A a2=new A(10,100);

    k[0]=a.i;
    k[1]=((B)a).i;
    k[2]=a1.i;
    k[3]=((B)a1).i;
    k[4]=a2.i;
    k[5]=((B)a2).i;

    return k;
  }
}
