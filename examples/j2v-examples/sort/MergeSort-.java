//
//  The MergeSort class only provides the operation Sort and
//  the functions MergeSorter and Merge to the outside world.
//  It is a subclass of Sorter which is natural in the sense that
//  it is one way of providing the sorting functionality.
//
class MergeSort extends Sorter
{
  public MergeSort() {}

  public int [] sort(int [] l)
  {
    return mergeSorter(l);
  }
//
//  The function mergeSorter is using the traditional algorith for
//  splitting a sequence into two almost equally long sequences and
//  then merging the results of sorting each of them.
//  The split here is made using a let-be expression where
//  l1 and l2 are introduced in a sequence concatenation pattern.
//  A matching solution must satisfy the predicate which states that
//  the difference of the lengths is less than 2.
//
  int [] mergeSorter(int [] l)
  {    
    if(l.length<=1) {
      return l;
    } else {
      int mid = (int)(l.length/2);

      int [] l1 = new int[mid];
      int [] l2 = new int[l.length-mid];

      for(int j=0; j<l.length; j++) {
        if(j<mid) l1[j]=l[j];
        else l2[j-mid]=l[j];
      }

      return merge(mergeSorter(l1),mergeSorter(l2));
    }
  }
//
// The Merge function is an auxiliary function which simply merges two
// sorted sequences into a new sorted sequence.
//
  int [] merge(int [] l1, int [] l2)
  {
    if(l1.length==0) return l2;
    if(l2.length==0) return l1;

    if(l1[0]<=l2[0]) {
      int [] tl_l1=new int[l1.length-1];
      for (int j=0; j < tl_l1.length; j++) tl_l1[j]=l1[j+1];

      int [] merged = merge(tl_l1, l2);

      int [] res=new int[merged.length+1];
      res[0]=l1[0];
      for (int j=1; j < res.length; j++) res[j]=merged[j-1];

      return res;
    } else {
      int [] tl_l2=new int[l2.length-1];
      for (int j=0; j < tl_l2.length; j++) tl_l2[j]=l2[j+1];

      int [] merged = merge(l1, tl_l2);

      int [] res=new int[merged.length+1];
      res[0]=l2[0];
      for (int j=1; j < res.length; j++) res[j]=merged[j-1];

      return res;
    }
  }
}
