//class D {
//}

class BB //extends D
{
}

class CC //extends D
{
}

class A {

  int a(BB b) {return 1;}
  int a(CC c) {return 0;}

  public int[] Test() {
    int k[]={a(new CC()), a(new BB())};
/*
  Run-Time Error 285: No overloaded operation or function with this parameter list is in scope
*/
    
    return k;
  }
}