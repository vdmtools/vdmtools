abstract class B
{
  public abstract int compare(int i, int j);
}

class C extends B
{
  public int compare(int i, int j)
  {
    if(i==j) return 0;
    else return 1;
  }
}

class  A{
  public int [] Test()
  {
    C c = new C();
    int i = c.compare(1,1);
    int j = c.compare(1,2);

    int ii[]={i, j};

    return ii;
  }
}