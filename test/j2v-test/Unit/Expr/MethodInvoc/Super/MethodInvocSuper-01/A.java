class B {
//  int a() { return 1;}     Scope problem. Explicit modifier needed

  protected int a() { return 1;}
}

class A extends B {
//  int a() { return 2;}

  public int[] Test() {
    int i=0;
    int k[]=new int[2];

    k[0]=a();
    k[1]=super.a();
/*

Javamain output:

FctCall(
  mk_CPP`QualifiedPackageName(
    mk_CPP`SimplePackageName(
      mk_CPP`Identifier( "super" ) ),
    mk_CPP`Identifier( "a",419430459 )),
    [  ]
)
*/
    return k;
  }
}
