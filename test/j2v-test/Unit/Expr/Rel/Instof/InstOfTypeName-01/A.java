class C {
}

class B extends C {
}

class A {
  public boolean[] Test() {
    boolean bb[]=new boolean[10];

    C c=new C();
    B b=new B();
    A a=new A();
    A aa[]={new A()};

    bb[0]=a instanceof A;

/*
inconvertible types
found   : A
required: B
    bb[1]=a instanceof B;
          ^
found   : A
required: C
    bb[2]=a instanceof C;
          ^
found   : B
required: A
    bb[3]=b instanceof A;
          ^
found   : C
required: A
    bb[6]=c instanceof A;
          ^
*/
    bb[4]=b instanceof B;
    bb[5]=b instanceof C;

    bb[7]=c instanceof B;
    bb[8]=c instanceof C;
    bb[9]=aa instanceof A[];

    return bb;
  }
}
