class DoSort extends Sorter
{
  public DoSort() {}

  public int [] sort(int [] l)
  {
    return doSorting(l);
  }

  int [] doSorting(int [] l)
  {
    int [] res=new int[]{};

    if(l.length == 0) {
      return res;
    }
    int [] tl_l=new int[l.length-1];
    for (int i=0; i < tl_l.length; i++) tl_l[i]=l[i+1];
    int [] sorted = doSorting (tl_l);
    return insertSorted (l[0], sorted);
  }

  int [] insertSorted(int i, int [] l)
  {
    if(l.length == 0) {
      int [] res=new int[1];
      res[0]=i;
      return res;
    } else if(i<=l[0]) {
      int [] res=new int[l.length+1];
      res[0]=i;
      for (int j=1; j < res.length; j++) res[i]=l[i-1];
      return res;
    } else {
      int [] tl_l=new int[l.length-1];
      for (int j=0; j < tl_l.length; j++) tl_l[j]=l[j+1];

      int [] res=insertSorted(i,tl_l);
      return res;
    }
  }
}
