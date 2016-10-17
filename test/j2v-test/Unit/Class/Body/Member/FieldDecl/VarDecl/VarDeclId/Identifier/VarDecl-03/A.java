class CC {
  public int i=999;
}

class BB {
  public CC c;

  public BB() { c=new CC(); }

  public BB(CC c1)
  {
    c=c1;
  }
}

class A extends BB {

  public A(CC c1)
  { super(c1);
  }

  public A() {}

  public int[] Test() {
    int k[]=new int[6];
    
    A a=new A(new CC());

    k[0]=a.c.i;

    return k;
  }
}

/*
  ../powertest/../j2v-test/Object.vpp, l. 53, c. 27:
  Run-Time Error 281: Unable to resolve overloaded operation call

class Nullable
operations
  public IsNil: () ==> bool
  IsNil() == return isNil
instance variables
  protected isNil: bool :=false			<<< l. 53
end Nullable


*/