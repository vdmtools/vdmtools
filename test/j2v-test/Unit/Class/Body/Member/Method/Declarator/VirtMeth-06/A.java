class DD 
{
  public int f() { return 0; }
}

class CC extends DD
{
  public int f() { return 1; }
  public int g() { return super.f(); }
}

class BB extends CC
{
//  public int f() { return 2; }
  public int f() { return super.f(); }
}

class A  {
  public int[] Test()
  {
    BB b=new BB();
    CC c=new CC();
    
    int ii[]=new int[6];

    ii[0]=b.f();
    ii[1]=b.g();
    ii[2]=c.g();

    return ii;
  }
}

