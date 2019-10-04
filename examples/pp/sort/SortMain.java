import jp.vdmtools.VDM.*;
import java.io.*;
import java.util.*;

public class SortMain
{
  @SuppressWarnings("unchecked")
  public static void main(String[] args)
  {
    try
    {
      java.util.List arr1 = new java.util.ArrayList();
      java.util.List arr2 = new java.util.ArrayList();
      arr1.add(Integer.valueOf(3));
      arr1.add(Integer.valueOf(5));
      arr1.add(Integer.valueOf(2));
      arr1.add(Integer.valueOf(23));
      arr1.add(Integer.valueOf(1));
      arr1.add(Integer.valueOf(42));
      arr1.add(Integer.valueOf(98));
      arr1.add(Integer.valueOf(31));
      arr2.add(Integer.valueOf(3));
      arr2.add(Integer.valueOf(1));
      arr2.add(Integer.valueOf(2));

      java.util.List res = null;

      SortMachine smach = new SortMachine();

      System.out.println("Evaluating DoSort(" + arr1.toString() + "):");
      Sorter dos = new DoSort();
      res = smach.SetAndSort(dos, arr1);
      System.out.println(res.toString());
      System.out.println();

      System.out.println("Evaluating ExplSort(" + arr2.toString() + "):");
      Sorter expls = new ExplSort();
      res = smach.SetAndSort(expls, arr2);
      System.out.println(res.toString());
      System.out.println();

      System.out.println("Evaluating ImplSort(" + arr2.toString() + "):");
      Sorter imps = new ImplSort();
      res = smach.SetAndSort(imps, arr2);
      System.out.println(res.toString());
      System.out.println();

      System.out.println("Evaluating post condition for ImplSort:");
      Boolean p = ((ImplSort)imps).post_ImplSorter(arr2, res);
      System.out.println("post_ImplSort(" + arr2.toString() + "," + res.toString() + "):");
      System.out.println(p.toString());
      System.out.println();

      System.out.println("Evaluating MergeSort(" + arr2.toString() + "):");
      Sorter mergs = new ExplSort();
      res = smach.SetAndSort(mergs, arr2);
      System.out.println(res.toString());
    }
    catch (Exception e) {
      System.out.println(e.getMessage());
    }
  }
}

