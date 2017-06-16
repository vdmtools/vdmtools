interface BI {
  int bi=0;

  int ba();
}

interface II extends BI {
  int i=1;

  int a();
}

interface JJ {
  int j=2;

  int b();
}

class BB
{}

class CC extends BB
{}

class DD extends CC implements II
{
  public int ba() { return 0;}
  public int a() { return 1;}
}

class  A implements II, JJ {
  II i1 = null;
  JJ i2 = null;
  BI i3 = null;

  public int a() { return II.i;}
  public int b() { return JJ.j;}
  public int ba() { return 0; /* JSS: not found bi;*/ }

  public int [] Test()
  {
    int ii[]=new int[11];

    ii[0]=II.i;
    ii[1]=a();
    ii[2]=b();
    ii[3]=ba();
    ii[4]=i1 == null ? 1 : 0;
    ii[5]=i2 == null ? 1 : 0;
    ii[6]=i3 == null ? 1 : 0;

    i1 = new A();
    i2 = new A();
    i3 = new A();

    ii[7]=i1 == null ? 1 : 0;
    ii[8]=i2 == null ? 1 : 0;
    ii[9]=i3 == null ? 1 : 0;

    A aa = new A();

    ii[9]=aa == null ? 1 : 0;

    return ii;
  }
}
