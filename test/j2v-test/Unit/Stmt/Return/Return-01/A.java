class  A{
  public void a(){
    return;
  }

  public int a1(){
    return 1;
  }

  public int a2(){
    return 1+1;
  }

  public int a3(){
    int i=1;
    return i++;
  }

  public int a4(){
    int i=1;
    return ++i;
  }

  public int[] Test(){
    a();
    int ii[]={a1(), a2(), a3(), a4()};
    return ii;
  }
}