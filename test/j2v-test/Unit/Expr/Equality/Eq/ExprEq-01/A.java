class DD {}

class EE
{
  public DD getD() { return null; }
}

class BB extends EE
{
  static DD d = new DD(); 
  public DD getD() { return d; }
}

class CC extends EE
{
  static DD d = new DD(); 
  public DD getD() { return d; }
}

class  A{
  public boolean [] Test()
  {
    BB b = new BB();
    BB b1 = new BB();
    CC c = new CC();

    boolean ii[]={
      b.getD().equals(c.getD()),
      b.getD().equals(b1.getD())
    };

    return ii;
  }
}