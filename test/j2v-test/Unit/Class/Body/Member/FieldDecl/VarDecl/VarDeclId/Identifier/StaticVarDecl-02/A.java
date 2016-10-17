class B {
  public int b = 999;

  public B(int i)
  {
    b=i;
  }
}

class A {

  static B b=new B(111);

  public int[] Test() {
    int k[]=new int[6];
    
    k[0]=A.b.b;

    return k;
  }
}

/*
Initializing specification ...../powertest/../j2v-test/Object.vpp, l. 53, c. 27:
  Run-Time Error 259: Initialisation is not possible

class Nullable
operations
  public IsNil: () ==> bool
  IsNil() == return isNil
instance variables
  protected isNil: bool :=false			<<< 53
end Nullable

*/