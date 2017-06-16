class  A {
  int dr    = 24;
  int dc = 80;

  public A(String t, int r, int c) {}

  public A() { this(null, 0, 1); }

  public A(String t, int r, int c, String f) {}

  public void ff(String t, int r, int c, String f) {}

  String Test1() { return null; }

  public boolean [] Test()
  {
    boolean bb[]=new boolean[6];

    A a0[]=new A[0];
    A a[]={null, null, (A)null};
    A aa[]=null;
    A aaa[][]={null, null};
//    A aaa[][]={null, {new A(), null}};
    String s=null;
    Object s1=(String)null;

    boolean b = s==null &&  s!=null;

    int ii[]=null;
    ii=null;
    s=null;
    boolean b1 = aa==null;
    boolean b2 = ii==null;

    bb[0]=b;
    bb[1]=b1;
    bb[2]=b2;

    A a1 = new A(null, 0, 1, null);
    a1.ff(null, 0, 1, null);

    return bb;
  }
}
