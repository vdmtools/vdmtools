class B {
  public boolean op1 (String s) { return s == null; }
}

class A extends B {
  
  public boolean op2 (String s) { return s == null; }

    public int put(String key, int value) {
      if (this.op1(key.toString())) {
        value++;
      }
      if (this.op2(key.toString())) {
        value++;
      }
    return value;
  }
  
  public int[] Test() {
    int k[] = new int[1];
    k[0] = put("",-1); 
    return k;
  }
}
