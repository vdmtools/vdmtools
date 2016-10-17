/* mrtest.c 3.1 5/28/93 */
/*
 *		mrtest.c
 *
 *	Test routine for mrandom.c 
 *
 *	Original Implementation:
 *		Clark Thomborson, September 1991
 *
 *	Modifications:
 *
 *	Clark Thomborson, May 1992 -- used new mrandom.h features,
 *		-added Chi-square test (based on Sedgewick's {it Algorithms}),
 *		-added -dnnn option, to demonstrate a problem with nrand48,
 *		-added Marsaglia's k-tuple test (from {\it Computer Science and
 *		 Statistics: The Interface}, L. Billard (ed.), Elsevier 1985,
 *		 pp. 3--10,
 *		-added -v, -f, and -p options; added -DVECTOR cc option
 *		-added median and mod 3 tests
 *		-added -t,-e options; added lots of argerr() calls 
 *		-used xsq instead of Chi-square.  Statistic is not chi-squared,
 *		 except in large-n limit.
 *		-improved accuracy of xsq calcs for very large n
 *		-adjusted code, so "mrtest 0" prints current rngstate
 *		-added large-m equidistribution test
 *		-debugged default RNGfile creation code (Aug 1992)
 *
 *      Robert Plotkin, May 1993
 *              -update for use with mrandom 3.0
 *
 *	Possible future additions:
 *		-more RNGs
 *		-faster (hash-based) large-m equidistribution test 
 *		-add Marsaglia's "Birthday-Spacings" test, to exhibit a
 *		 shortcoming of Bentley's prand() (as well as 4.3bsd random()):
 *		 sort n results from mrandom(m), compute the list of "spacings"
 *		 (differences) between adjacent output values in the sorted
 *		 list, then let Y be the number of values that appear
 *		 more than once among the spacings.  Y is asymptotically
 *		 Poisson with parameter $\lambda = n^3/(4m)$ [Marsaglia,
 *		 ``A current view of random number generators'', Computer
 *		 Science and Statistics: The Interface, Elsevier, 1985;
 *		 Marsaglia says the ``proof, due to Janos Komlos, and
 *		 detailed discussion of the test will appear elsewhere.''].
 *		 According to Marsaglia, lagged-Fibbonacci generators
 *		 based on binary addition, e.g. Bentley's RNG, fail this
 *		 test.  Quite probably random() would fail it as well.
 *		-add a Poisson interpretation routine, to support the
 *		 Birthday Spacings test.  This could also be useful in
 *		 a possible improvement to the interpret_xsq routine,
 *		 since Poisson statistics could be used to analyze frequency
 *		 counts in (our usual) case of very small n/k.
 *		 
 *
 *	This material is based upon work supported by the National
 *	Science Foundation under grant number MIP-9023238.  The
 *	Government has certain rights in this material.
 *
 *	Any opinions, findings, and conclusions or recommendations
 *	expressed in this material are those of the author and do
 *	not necessarily reflect the view of the National Science
 *	Foundation.
 *
 *	This code is neither copyrighted nor patented.
 */

# include <sys/file.h> /* we use access() */
# include "mrandom.h"
# include "xsq.h" /* for interpretation of X-squared values */
# include <math.h>
# include <values.h> /* we need MAXLONG */

# ifndef RNGFILENAME
# define RNGFILENAME "RNGstatefile" /* where the RNG state is stored */
# endif

/* max(m,n) for which equidistribution test will be performed */
# define EQUIDISTMAX (1<<18)
/* max m for which 2-tuple correlation will be tested */
# define PAIRMAXM 256

void argerr(progname)
char *progname;
{
  printf(
    "Usage: %s [ nn -dnnnn -S[n[,n[,n[,n[,n]]]]] -mnn -Mnn -q -t -e]",
	 progname);
#ifndef VECTOR
  printf(" -f -v -p");
#endif
  printf(" ]\n");
  printf("  nn sets number of random generates to be tested.  Default 10.\n");
  printf(
   "  -dnnnn discards nnnn generates between tested values.  Default 0.\n");
  printf("  -S[n[,n[,n[,n[,n[,n]]]]]] initializes an RNG, as follows:\n");
  printf("    parameter #1 sets the RNG algorithm:\n");
  printf("      0 is an additive linear scheme (for testing only);\n");
  printf("      1 is 4.3bsd random,\n");
  printf("      2 is the Knuth/Bentley prand,\n");
  printf("      3 is L'Ecuyer's portable combined multiplicative RNG,\n");
  printf("      4 is 4.3bsd nrandom48, and\n");
  printf("      5 is 4.3bsd rand;\n");
  printf("      6 is Press & Teukolsky's ran0\n");
  printf("      7 is Press & Teukolsky's ran1\n");
  printf("      8 is Press & Teukolsky's ran2\n");
  printf("      9 is Marsaglia's Ultra\n");
  printf("    parameter #2 sets the first RNG seed\n");
  printf("    parameter #3 sets the second RNG seed (if any)\n");
  printf("    parameter #4 sets the number of times to cycle the RNG\n");
  printf("      before starting the tests, mod 1 billion.\n");
  printf("    parameter #5 sets the number of times to cycle the RNG\n");
  printf("      before starting the tests, div 1 billion.\n");
  printf("    defaults are 1,1,1,0,0 respectively.\n");
  printf("  -mnnnn sets the range of the RNG to be nnnn.  Default 100.\n");
  printf("  -Mnn sets the range of the RNG to be 2^nn, 0<=nn<=31.\n");
  printf("  -q or -quiet doesn't print any random generates.\n");
  printf("  -t eliminates most RNG tests, for timing measurements.\n");
  printf("  -e echos the command line (useful in scripts).\n");
# ifdef VECTOR
  printf("  This version of the code is optimized for speed.\n");
  printf("  Recompile without -DVECTOR if you want any of the following:\n");
# endif
  printf("  -f uses (int(dxrandom()*m): faster, but slightly biased.\n");
  printf("  -p uses random()%%m, a poor method.\n");
  exit(1);
} /* end argerr */

#define NORMAL      0
#define VECTORIZED  1
#define FAST        2
#define POOR        3

/* declare storage for a vector of random generates */
# define VECLENGTH 64
static long rpt=VECLENGTH;
static long rvals[VECLENGTH];

# ifndef VECTOR
/* slower code with runtime options */
long random_value(rng,m,method)
RNGdata *rng;
long m,method;
{

  switch (method) {
    case NORMAL:
	 return( mrandom(m) );
    case VECTORIZED:
	 if (rpt==VECLENGTH) {
	   rpt = 1;
	   return( mrandomrv(rng,m,VECLENGTH,rvals) );
	 } else {
	   return( rvals[rpt++] );
	 }
    case FAST:
	 return( (int) (dxrandom()*m) );
    case POOR:
	 return( lxrandom()%m );
    default:
	 return( 0 );
  }
}
# else
/* fast code, an optimized version of the -v option */
# define random_value(rng,m,method) \
	(rpt==VECLENGTH ? \
	   ( rpt = 1, mrandomrv(rng,m,VECLENGTH,rvals) ) : \
	   ( rvals[rpt++] ) \
	)
# endif /* VECTOR */

/* Comparison routine, for qsort() */
int comparelongs(a,b)
long *a,*b;
{
  return( ((*a)<(*b))? -1 : (((*a)==(*b))? 0 : 1) );
}

/* A simple test-driver */
int main(argc,argv)
int argc; char *argv[];
{
  /* command-line arguments, with default values */
  int reseeding=0; /* nonzero if RNG state will be re-initialized */
  int quiet=0; /* nonzero if we don't want to print random generates */
  int echo=0; /* nonzero if we want to echo the mrtest invocation line */
  int timing=0; /* nonzero if we don't want to make a lot of RNG tests */
# ifndef VECTOR
  int method=NORMAL; /* how we should call the mrandom package;
	* other defined values are VECTORIZED (the -v option),
	* FAST (-f), and POOR (-p).
	*/
# else
  int method = VECTORIZED; /* default (and only) option in this version */ 
# endif
  long alg=1;
  long mrandom_alg=0;
  long bufsize=1;
  long seed1=1,seed2=1, seed[2]; /* seed for RNG */
  long count1=0,count2=0; /* init call count for RNG */
  long n=10; /* number of randoms to generate */
  long m=100; /* desired range of random outputs: 0..m-1 */
  long discards=0; /* how many generates to discard between outputs */

  long i,j,k; /* temp counters */
  char rngdesc[RNGIDSTRLEN]; /* for a describe_rng() call */

  RNGdata *myrng; /* area to store this RNG's state & other data */

  double dm,dn; /* = (double) m, (double) n */

  /* temps used to calculate x-squared values */
  double expf,f,sumfsq; /* we use doubles, not longs, to avoid overflow */
  long sumf;  /* sum of all freqs, for error check */
  double zerofreqs; /* number of freqs = 0 */
  /* flags to indicate which tests will be run */
  int testequidist; /* 0: max(m,n) too big, don't test;
		     * 1: count freqs online,
		     * 3: m is huge, count freqs offline.
		     */
  int test2tuples, test3tuples;
  /* various xsq stats */
  double xsq,xsq2,xsq3, xsq2l, xsq3l;
  /* random generates: current, previous, previous^2, first, second */
  long x, prevx, pprevx, firstx, secondx;

  /* var for max test */
  long maxx;
  /* var for median test */
  long lowcount,median;
  /* vars for mod 3 test */
  long mod0, mod1;
  /* data for equi-distribution test:
   * freq[i] = sum_{1 \leq j \leq n} (x[j]==i), if testequidist == 1
   * freq[i] = x[i], if testequidist == 3
   * freq[i] = undefined, otherwise
   */
  long freq[EQUIDISTMAX];
  /* data for 2-tuple test */
  long pairs[PAIRMAXM][PAIRMAXM];
  /* vars for 3-tuple test on low-order 3 bits */
  long lowpairs[8][8];
  long lowtrips[8][8][8];

  if(argc > 1) {
    for(i=1;i<argc;i++) {
      if(argv[i][0] >= '0' && argv[i][0] <= '9') {
	n = atol(&(argv[i][0]));
	if (n < 0) {
	  printf("Illegal value, %ld, for number of random generates.\n",n);
	  argerr((char*) argv[0]);
        }
      } else if(argv[i][0] == '-') {
	switch(argv[i][1]) {
	case 'S': /* new seed(s) for rng */
	  seed1 = 1; /* defaults */
	  seed2 = 1;
	  count1 = 0;
	  count2 = 0;
	  sscanf (&(argv[i][2]), "%ld,%ld,%ld,%ld,%ld,&ld,&ld",
		 &alg, &seed1, &seed2, &count1, &count2);
	  seed[0]=seed1; seed[1]=seed2;
	  reseeding = 1;
	  if (seed1<0) {
	    printf("Illegal value, %ld, for RNG seed.\n",seed1);
	    argerr((char*) argv[0]);
          }
	  if (seed2<0) {
	    printf("Illegal value, %ld, for second RNG seed.\n",seed2);
	    argerr((char*) argv[0]);
          }
	  if (count1<0) {
	    printf("Illegal value, %ld, for number of times ");
	    printf(" to cycle rng before starting tests.\n",count1);
	    argerr((char*) argv[0]);
          }
	  if (count2<0) {
	    printf("Illegal value, %ld, for number of billions of times ");
	    printf(" to cycle rng before starting tests.\n",count1);
	    argerr((char*) argv[0]);
          }
	  break;
	case 'd': /* adjust number of discards */
	  discards = atol(&(argv[i][2]));
	  if (discards < 0) {
	    printf("Illegal value, %ld, for number of discards.\n",discards);
	    argerr((char*) argv[0]);
          }
	  break;
	case 'm': /* adjust range of rng */
	  m = atol(&(argv[i][2]));
	  break;
	case 'M': /* adjust log range of rng */
	  j = atol(&(argv[i][2]));
	  if (j<0 || j>31) {
	    printf("Illegal value, %ld, for log range of rng.\n",j);
	    argerr((char*) argv[0]);
          }
	  if (j == 31) {
	    m = ~MAXLONG; /* note: m is an unsigned long */
	  }
	  else if (j==0) {
	    m = 0;
	  }
	  else {
	    m = (long)pow( 2.0, (double) j );
	  }
	  break;
	case 'q': /* quiet! */
	  quiet = 1;
	  break;
	case 't': /* strip testing code from inner loop, for timing */
	  timing = 1;
	  quiet = 1; /* -t implies -q */
	  break;
	case 'e':
	  echo = 1; /* echo mrtest invocation */
	  break;
# ifndef VECTOR
	case 'v':
	  method = VECTORIZED;
	  break;
	case 'f':
	  method = FAST;
	  break;
	case 'p':
	  method = POOR;
	  break;
# endif VECTOR
	default:
	  argerr((char*) argv[0]);
	}
      } else {
	argerr((char*) argv[0]);
      }
    }
  }

  if (echo) for (i=0; i<argc; i++) {
    printf("%s ",argv[i]);
  }
  printf("\n");

  dn = (double) n;

  if (m > 0) {
    dm = (double) m;
  }
  else if (m < 0) {
    dm = (double)(m & 0x7fffffff) + 2147483648.0; /* 2^31 */
  }
  else {
    dm = 4294967296.0; /* 2^32 */
  }

  if (!reseeding ) {
    if (access(RNGFILENAME, R_OK)) {
      printf("There is no RNG statefile in this directory, so ");
      printf("I'll make one for you.\n");
      reseeding = 1;
    }
  }

  if (reseeding) { /* create a new statefile from scratch */
    printf("Initializing RNG.\n");
    myrng=init_rng(alg,mrandom_alg,seed,count1,count2,bufsize);
  } else { /* use an existing statefile */
    if (n != 0) printf("Restarting RNG.\n");
    if (!(myrng=restart_rng(RNGFILENAME))) {
      exit(1);
    }
  }
  printf(describe_rng(myrng,rngdesc));

  if (n == 0) {
    if (reseeding) {
      exit( save_rng(myrng,RNGFILENAME) ); /* save new rng, exit */
    } else {
      exit(0); /* immediate exit if n == 0 */
    }
  }

  /* set flags: will we run the various tests? */
  if (timing) { /* avoid time-consuming tests */
    testequidist = 0;
    test2tuples = 0;
    test3tuples = 0;
  } else {
    if (m == ~MAXLONG) { /* special case for M==31 (m==2^31) */
      testequidist = (n <= EQUIDISTMAX ? 3 : 0);
      test2tuples = 0;
      test3tuples=1;
    } else {
      testequidist = ( m <= EQUIDISTMAX ? 1 : (n <= EQUIDISTMAX ? 3 : 0));
      test2tuples = ( m <= PAIRMAXM );
      test3tuples = ( (m%8) == 0 );
    }
  }

  /* initialize the various frequency-counting arrays, if needed */
  if (testequidist==1) for (i=0; i<m; i++) freq[i] = 0;
  if (test2tuples) for (i=0; i<m; i++) for (j=0; j<m; j++) pairs[i][j] = 0;
  if (test3tuples) {
    for (i=0; i<8; i++) {
      for (j=0; j<8; j++) {
	lowpairs[i][j] = 0;
        for (k=0; k<8; k++) {
	  lowtrips[i][j][k] = 0;
	}
      }
    }
  }

  /* initialize counter, threshhold for median test */
  lowcount = 0;
  median = ( (m == ~MAXLONG) ? 1<<30 : m/2 );

  /* initialize counters for mod 3 test */
  mod0 = 0;
  mod1 = 0;

  /* initialize storage for max test */
  maxx = -1;

  if (timing) discards=0; /* -d is meaningless with -t */

  if (!quiet) {
    printf("Here %s %ld random value", ((n == 1)? "is" : "are"), n);
  } else {
    printf("Generating %ld random value", n);
  }
  printf("%s", ((n == 1)? " " : "s "));
  printf("in the range 0 to %ld\n", ((m == ~MAXLONG)? MAXLONG : m-1));

  if (discards > 0) {
    printf("Note: discards = %ld, so mrandom() will be called a",discards);
    printf(" total of %.0f times.\n", (1.+(float)discards)*(float)n);
  }


  if (timing) {

    /* vectorized inner loop */
    for (i=0; i+VECLENGTH<n; i+=VECLENGTH) {
      mrandomrv(myrng,m,VECLENGTH,rvals);
      for (j=0; j<VECLENGTH; j++) {
	/* Note: there are no subroutine calls in this loop! */
	x = rvals[j];
	if (x > maxx) maxx = x;
      }
    }
    /* the last (partial) block, if any */
    for ( ; i<n; i++) {
      x = mrandomr(myrng,m);
      if (x > maxx) maxx = x;
    }

  } else for (i=0; i<n; i++) { /* do lots of RNG tests */

    x = random_value(myrng,m,method);
    if ( (x >= m && m != ~MAXLONG) || x < 0) {
      printf("Illegal output, %ld, from mrandom(%ld).\n",x,m);
      printf("Please contact cthombor@mars.d.umn.edu.\n");
      printf(describe_rng(myrng,rngdesc));
      exit(1);
    }

    /* max test */
    if (x > maxx) maxx = x;

    /* count number of occurrences of each distinct output, if range is small,
     * otherwise just keep track of outputs (we'll count freqs later)
     */
    if (testequidist==1) {
      freq[x]++; 
    } else if (testequidist==3) {
      freq[i] = x;
    }

    /* count pairs */
    if (i == 0) {
      firstx = x; /* keep track of first generate for "circular" pairs count */
    } else {
      if (test2tuples) {
	pairs[x][prevx]++;
	if (i == n-1) pairs[firstx][x]++; /* handle the "wraparound pair" */
      }
      if (test3tuples) {
	lowpairs[x&7][prevx&7]++;
	if (i == n-1) pairs[firstx&7][x&7]++; /* handle the "wraparound pair" */
      }
    }

    /* count triples */
    if (i == 1) {
      secondx = x; /* needed for the last "wraparound triplet" */
    }
    if (i > 1) {
      if (test3tuples) {
	lowtrips[x&7][prevx&7][pprevx&7]++;
	if (i == n-1) { /* time to do the wraparound triplets? */
	  lowtrips[firstx&7][x&7][prevx&7]++;
	  lowtrips[secondx&7][firstx&7][x&7]++;
	}
      }
    }

    /* count number below median */
    if (x < median) lowcount++;

    /* count number ==0 and ==1, mod 3 */
    j = x%3;
    if (j==0) {
      mod0++;
    } else if (j==1) {
      mod1++;
    }

    /* keep track of previous two generates, for pair and triple counts */
    pprevx = prevx;
    prevx = x;

    /* use fixed-width format, to make it easy to sort the output */
    if (!quiet) {
      if (m < 0 && x < 0) { /* print an unsigned long */
        printf("  %.0f\n", (double)(x&MAXLONG)+(double)MAXLONG+1.0);
      } else if (m <= 10) {
        printf("  %d\n", x);
      } else if (m <= 100) {
        printf("  %2d\n", x);
      } else if (m <= 1000) {
        printf("  %3d\n", x);
      } else {
        printf("  %4d\n", x);
      }
    }
    /* now discard some generates: d=2^k, k>9, gives nrand48() trouble */ 
    for (j=0; j<discards; j++) {
      /* note: we don't even check whether x is in range */
      x = random_value(myrng,m,method);
    }
  }

  printf("Max value from %ld call%s", n, (n==1? "" : "s"));
  if (discards) printf(" (discards = %ld)",discards);
  printf(" to mrandom(%ld) = %ld\n",m,maxx);
  if ( (maxx >= m && m != ~MAXLONG) || maxx < 0) {
    printf("Illegal value for max!\n");
    printf("Please contact cthombor@mars.d.umn.edu.\n");
    printf(describe_rng(myrng,rngdesc));
    exit(1);
  }

  if (!timing && n>2) {
    printf("Equi-distribution test results:\n");
    if (testequidist == 0) {
      printf("  Range m and/or number of random generates n is too large.\n");
      printf("  Max(m,n) for this test is %d.\n", EQUIDISTMAX);
    } else if (m == 1) {
      printf("  You need m > 1 for this test.\n");
    } else {
      sumfsq = 0.0;
      expf = dn/dm; /* expected freq[] */
      if (testequidist == 1) { /* freq[] array has frequencies */
        sumf = 0; /* error check: is ((sumf = \sum_i{freq[i]}) == n)? */
        for (i=0; i<m; i++) {
	  sumf += freq[i];
          f = ((double)freq[i]) - expf;
          sumfsq += f*f;
        }
        if (sumf != n) {
	  printf("Warning: a frequency counter has overflowed.");
	  printf("  Test invalid!\n");
	  goto testinvalid;
        }
      } else if (testequidist == 3) { 
	/* freq[] array has the random generates: sort, count dups */
        qsort(freq,n,sizeof(long),comparelongs);
	f = 1.0;
	zerofreqs = dm; /* number of integers NOT seen in RNG output */ 
	for (i=0; i<n; i++) {
	  if (i == (n-1) || freq[i] != freq[i+1]) {
	    sumfsq += (f-expf)*(f-expf); /* end of one run of duplicates... */
	    zerofreqs -= 1.0;
	    f = 1.0; /* ...and the beginning of another run */
	  } else {
	    f += 1.0; /* a "match" extends a run */
	  }
	}
	/* add contributions from zero-frequency outputs */
	sumfsq += zerofreqs*(-expf)*(-expf);
      }
      /* Note: many texts suggest summing t += freq[i]^2, then calculating
       * xsq by (dm*t/dn)-dn.  This is slightly faster, but MUCH less
       * accurate when dn is much larger (e.g. 2^20 times larger) than dm.
       */
      xsq = sumfsq/expf;
      interpret_xsq(xsq, dm, dn);
      /* End of equi-distribution test code */
  
      printf("Pairwise correlation test results:\n");
      if (!test2tuples) {
        printf("  Range is too large.  Max m for this test is %d.\n", PAIRMAXM);
      } else {
        sumfsq = 0.0;
        expf = dn/(dm*dm); /* expected freq[] */
        for (i=0; i<m; i++) for (j=0; j<m; j++) {
	  f = (double)pairs[i][j] - expf;
          sumfsq += f*f;
        }
        xsq2 = sumfsq/expf;
	/* note that we use the xsq from the equi-distribution test */
        interpret_xsq(xsq2-xsq, dm*dm-dm+1.0, dn);
      } /* end of pairwise correlation test code */
    }
  
    printf("3-tuple, low-order 3 bits, correlation test results:\n");
    if (!test3tuples) {
      printf("  You need (m mod 8) == 0 for this test.\n");
    } else {
      sumfsq = 0.0;
      expf = dn/64.0; /* expected freq in each bin */
      for (i=0; i<8; i++) {
        for (j=0; j<8; j++) {
          f = (double)lowpairs[i][j] - expf;
          sumfsq += f*f;
        }
      }
      xsq2l = sumfsq/expf;
      sumfsq = 0.0;
      expf = dn/512.0; /* expected freq in each bin */
      for (i=0; i<8; i++) {
        for (j=0; j<8; j++) {
	  for (k=0; k<8; k++) {
            f = (double)lowtrips[i][j][k] - expf;
            sumfsq += f*f;
	  }
        }
      }
      xsq3l = sumfsq/expf;
      interpret_xsq(xsq3l-xsq2l, 8.0*8.0*8.0-8.0*8.0+1.0, dn);
    } /* end of 3-tuple test code */
  
    printf("Most-significant bit test results:\n");
    f = (double)lowcount; /* freq below m/2 */
    expf = dn*(floor(dm/2.))/dm; /* expected val of f */
    sumfsq = ((f-expf)*(f-expf))/expf; /* normalized dev^2 from expectation */
    f = dn-f; /* freq at or above m/2 */
    expf = dn-expf;
    sumfsq += (f-expf)*(f-expf)/expf;
    interpret_xsq(sumfsq,2.0,dn);
    /* end of MSB test code */

    printf("Mod-3 test results:\n");
    /* We need the freq[] to be reasonably equal for xsq analysis */
    if (!(m == 3 || m > 5 || m == ~MAXLONG)) {
      printf("  You need m==3 or m>5 to run this test.\n");
    } else {
      f = (double)mod0; /* freq of x==0 mod 3 */
      expf = dn*(ceil(dm/3.))/dm;
      sumfsq = ((f-expf)*(f-expf))/expf;
      f = (double)mod1; /* freq of x==1 mod 3 */
      expf = dn*(ceil(2.*dm/3.))/dm - expf;
      sumfsq += (f-expf)*(f-expf)/expf;
      f = dn-f-(double)mod0; /* freq of x==2 mod 3 */
      expf = dn-dn*(ceil(2.*dm/3.))/dm;
      sumfsq += (f-expf)*(f-expf)/expf;
      interpret_xsq(sumfsq,3.0,dn);
    } /* end of mod-3 test code */
  } else if (n < 3) {
    printf("At least 3 random generates are required for the testing module.\n"
	  );
  }
testinvalid:
  /* terminate job */
  printf("Final ");
  printf(describe_rng(myrng,rngdesc));

  return( save_rng(myrng,RNGFILENAME) );

} /* end main */
