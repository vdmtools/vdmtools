package jp.vdmtools.VDM;

public class RANDOM {

// ORIGINAL_NUMBERS
/*
  private static final long m = 2147483647;
  private static final long q = 127773;

  private static final long a = 16807;
  private static final long r = 2836;

  public static final long successfulltest = 1043618065;
*/
  private static final long m = 2147483647;
  private static final long q = 44488;

  private static final long a = 48271;
  private static final long r = 3399;

  public static final long successfulltest = 399268537;

  private long seed = 0;

  public RANDOM()
  {
     this.seed = 0;
  }

  public RANDOM(long initial_seed)
  {
    if (initial_seed == 0)
      this.seed = 89482311;
    else
      this.seed = initial_seed;
  }

  public void set_seed(long seed)
  {
    if (seed == 0)
      this.seed = 89482311;
    else
      this.seed = seed;
  }

  public long vdm_rand() 
  {
    int hi = (int)(this.seed/q);
    int lo = (int)(this.seed%q);
    int test = (int)(a * lo - r * hi);

    if (test > 0)
      this.seed = test;
    else
      this.seed = test + m;

    return this.seed;
  }

  public int get_random(int limit)
  {
    return (int) (((double)this.vdm_rand()/(double)m)*limit);
  }

  public long get_seed()
  {
    return this.seed;
  }
}
