class SortMachine {
//
//  The instance variable "srt" is an object reference to the sorting algorithm currently in use. The initial sorting algorithm is MergeSort.
//
  public Sorter srt=new MergeSort();

  public SortMachine() {}
//
// Setting/changing which sorting algorithm to use. 
//
  public void setSort(Sorter s)
  {
    srt = s;
  }
//
// Sorting with the sorting algorithm currently in use. 
//
  public int [] goSorting(int[] arr)
  { 
    return srt.sort(arr);
  }
//
// Set/change first the sorting algorithm and sort afterwards.
//
  public int [] setAndSort(Sorter s, int [] arr)
  { 
    srt = s;
    return srt.sort(arr);
  }
}