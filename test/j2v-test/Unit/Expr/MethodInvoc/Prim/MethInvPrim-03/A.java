class  BB
{
  public int op() { return 0; }

}

class  A
{
  public int Test()
  {
    Object b = new BB();

    int i = ((BB)b).op();
    
    return i;
  }
}