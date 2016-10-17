class A0 {
  int i = 1;
  float f = 1.0f;
  public int m (float p_f, int p_i  ) { return i; }
}

class A extends A0 {

  public float m (int p_i, float p_f) { return f+1.0f; }

  public int [] Test () {
    int k [] = new int[6];
    A a = new A();

    k[0] = a.m (0.0f, 0);
    k[1] = (int) a.m (0, 0.0f);

    return k;
  }
}