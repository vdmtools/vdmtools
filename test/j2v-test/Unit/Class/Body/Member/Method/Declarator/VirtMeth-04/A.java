abstract class BB
{
  abstract public boolean eq(BB b);
}

class CC extends BB
{
  char v;

  public CC(char c) { v=c; }

  public boolean eq(BB b)
  {
    if (b instanceof CC) {
      return ((CC)b).v==v;
    } else {
      return false;
    }
  }

}

class A extends BB
{
  int v;

  public A() { v=0; }

  public A(int i) { v=i; }

  public boolean eq(BB b)
  {
    if (b instanceof A) {
      return ((A)b).v==v;
    } else {
      return false;
    }
  }

  public boolean[] Test() {
    A a=new A(123);
    A a1=new A(123);
    
    boolean bb[]={
      a.eq(a),
      a.eq(a1),
      a1.eq(a),
      a1.eq(this),
      a1.eq(new CC('c'))
    };

    return bb;
  }
}