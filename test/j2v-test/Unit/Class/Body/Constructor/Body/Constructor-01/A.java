class CC
{
}

class BB
{
  static BB b = new BB(new CC(),0);
  public  BB(CC s,int i)
  {
    ;
  } 
}

class A
{
  public int Test() {
//    BB b=new BB();
    return 0;
  }
}

/*
Initializing specification ...../powertest/../j2v-test/Object.vpp, l. 53, c. 27:
  Run-Time Error 259: Initialisation is not possible
*/