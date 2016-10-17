import jp.vdmtools.VDM.RANDOM;

public class TestRandom
{
  public static void main(String args[])
  {
    jp.vdmtools.VDM.RANDOM rnd = new jp.vdmtools.VDM.RANDOM();
//    RANDOM rnd = new RANDOM();
    rnd.set_seed(1);

    boolean success = false;

    for( int i = 1; i <= 10001; i++ )
    {
      long n_rand = rnd.vdm_rand();

      if( i > 9998 )
      {
         System.out.print("Sequence ");
         System.out.print(i);
         System.out.print(", seed= ");
         System.out.println(rnd.get_seed());
      }

      if( i == 10000 )
      {
        if( rnd.get_seed() == rnd.successfulltest ) success = true;
      }
    }
    if (success){
      System.out.println("The random number generator works correctly.");
    }else{
      System.out.println("The random number generator DOES NOT WORK!");
    }
  }
}
