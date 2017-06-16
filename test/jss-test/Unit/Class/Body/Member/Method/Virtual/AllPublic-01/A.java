class B {
  int i;
  float f;
  public int m (float p_f, int p_i) { return i; }
}

class A extends B {

  public int m (float p_f, int p_i) { return i+1; }

  public int [] Test () {
    int k[] = new int[6];

    A a = new A ();
    B b = new B ();

    k[0] = b.m (0.0f, 1);
    k[1] = a.m (0.0f, 1);
    k[2] = super.m (0.0f, 1);

    return k;
  }
}