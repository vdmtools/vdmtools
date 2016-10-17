class A {
  public int Test() {
    A t = new A();
//    synchronized(t) {
//     synchronized(t) {
      int i=1;
//     }
//    }
    return i;
  }
}
