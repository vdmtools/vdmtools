/* mrandom.h 3.1 5/28/93 */
/*
 *		mrandom.h
 *
 *      An interface for random number generators (RNGs)
 *
 *	Original Implementation:
 *		Clark Thomborson, September 1991
 *	Modifications:
 *	   - give uniform interface to other rngs
 *	   - remove bias from integer generator
 *	   - avoid possible infinite loop in mrandom(m)
 *	   - allow access to multiple simultaneous rngs
 *	   - add interface to nrand48()
 *	   - add interface to rand()
 *	   - tuned for speed
 *		Clark Thomborson, May 1992
 *	
 *      Version 3.0:
 *              Robert Plotkin, May 1993
 *      Modifications include:
 *         - Standardized interface to underlying RNGs
 *         - Added buffered generating routines
 *         - Added interfaces to ran0, ran1, ran2, Ultra RNGs
 *         - Added routines for generating bits
 *         - Added split feature
 *         - Allow choice of algorithms in mrandomrv
 *         - Dynamic allocation of RNGs
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
 *	This code is neither copyrighted nor patented, although we are
 *	not sure about the status of some of the routines it calls.
 */

#ifndef MRANDOM
#define MRANDOM
#endif

/*****************************/
/* Miscellaneous definitions */
/*****************************/
#define RNGIDSTRLEN 80 /* Maximum length of string written by */
		       /* describe_rng() */
#define BILLION	1000000000 /* The modulus for 32-bit RNGcount1 in our */
			   /* statefile, i.e. */
                           /* (Number of calls to RNG since seeding) = */
			   /* RNGcount1 + BILLION * RNGcount2 */
#define BBUF_SIZE    32 /* Size of the bit buffer */
#define NUM_RNGS     10 /* Number of RNGs currently installed */
#define SPLIT_DEF    0  /* Default split value for all RNGs */

#define RET_LONG     4  /* Types of values returned by */
#define RET_DOUBLE   -8 /* RNG generating routines */

#define STATE_CHAR   1  /* Types of values stored in */
#define STATE_INT    2  /* RNG state and seed vectors */
#define STATE_LONG   4
#define STATE_FLOAT  -4 /* STATE_FLOAT and STATE_DOUBLE are */
#define STATE_DOUBLE -8 /* currently unsupported */

/**************************/
/* RNGdata                */
/* Data describing an RNG */
/**************************/
struct rngdata {
  /* The following data is taken directly from the statefile */
  long	rngalg;	    /* algorithm used to generate pseudorandoms:
		     * 0 = trivial rng (long state=seed1; state += seed2)
		     * 1 = 4.3bsd random.c,
		     * 2 = bentley.c,
		     * 3 = pcrand.c,
		     * 4 = 4.3bsd nrand48.c,
		     * 5 = 4.3bsd rand.c,
		     * 6 = Press & Teukolsky's ran0 (ran0.c)
		     * 7 = Press & Teukolsky's ran1 (ran1.c)
		     * 8 = Press & Teukolsky's ran2 (ran2.c)
		     * 9 = Marsaglia's Ultra (ultra.c)
		     */
  long mrandom_alg; /* Algorithm used by mrandomrv */
  long *rngstate;   /* The RNG's state vector */
  long *rngseed;    /* The seed originally used to initialize rngstate */
  long rngcount1;   /* mod-BILLION counter of calls to the RNG */
  long rngcount2;   /* div-BILLION counter */
  struct {
    long size;              /* Number of entries in buffer */
    long nleft;             /* Number of values left in buffer */
    int nbleft;             /* Number of values in bit buffer */
    double *dbuf,*dbufp;    /* RNG double buffer [0,1) */
    long *lbuf,*lbufp;      /* RNG long buffer 0...RNGrange-1 */
    int *bbuf,*bbufp;       /* RNG bit buffer 0,1 */
  } buffer;
  long rngnextval;          /* the RNG's next output */
  long rngsplit;            /* Return every rngsplit-th generate */
  
  /* The next six values are inferred from rngalg */
  char rngname[RNGIDSTRLEN]; /* Name of the RNG */
  long rngstatesize;
  long rngseedsize; /* It is permissible to have an RNG which can use */
		    /* different numbers of seeds.  In that case, use */
		    /* the maximum permissible number of seeds for */
		    /* this value. */
  double rngrange;  /* The RNG's range, expressed as a d.p. float */
  signed int rngreturns; /* The kind of value the rng returns
			    - Positive numbers indicate integers in the
			      range 0...RNGrange-1
			    - Negative numbers indicate floats in the
			      range [0,1)
			    - Absolute value indicates the number of
			      bytes per RNG output
			      e.g. 4 means a 32-bit integer,
			      -16 means a 128 bit float
			      Currently only 4 and -8 are supported.
			      These are defined as RET_LONG and
			      RET_DOUBLE, respectively. */
  int rngstatetype; /* The type of the value stored in the RNG's state */
		    /* and seed vectors.  Currently, three types are */
		    /* supported:
		       Type            Value
		       -----------     ----------
		       8-bit char      STATE_CHAR
		       16-bit int      STATE_INT
		       32-bit long     STATE_LONG
		       */
};
typedef struct rngdata RNGdata;

/* Convenient names for data in our RNGdata struct */
#define RNGalg		(rng->rngalg)
#define RNGname         (rng->rngname)
#define RNGseed	        (rng->rngseed)
#define RNGcount1	(rng->rngcount1)
#define RNGcount2	(rng->rngcount2)
#define RNGnextval	(rng->rngnextval)
#define RNGstatesize	(rng->rngstatesize)
#define RNGseedsize     (rng->rngseedsize)
#define RNGrangem1	(rng->rngrangem1)
#define RNGrange	(rng->rngrange)
#define RNGstate	(rng->rngstate)
#define RNGmrandom_alg  (rng->mrandom_alg)
#define RNGbuffer       (rng->buffer)
#define RNGreturns      (rng->rngreturns)
#define RNGstatetype    (rng->rngstatetype)
#define RNGsplit        (rng->rngsplit)

/*******************************************************************/
/* Procedures for generating pseudorandom numbers                  */
/* These procedures write a vector v of length n, by repeating the */
/* following two steps n times:                                    */
/*      - place the next generate in v                             */
/*      - discard the next k generates, where k is the split value */
/*         of the RNG                                              */
/*                                                                 */
/* Special cases:                                                  */
/*  Argument  Value  Meaning                                       */
/*  --------  -----  -------                                       */
/*  rng         0    Use most recently used or initialized rng.    */
/*  n           0    Return  a single generate.                    */
/*                   The value of v is ignored(it may be set to 0).*/
/*                                                                 */  
/* All procedures return the first entry in the vector v.          */
/*******************************************************************/

/***********************************************/
/* Buffered calling procedures.                */
/***********************************************/
/*******************/
/* drandomrv       */
/* Returns doubles */
/*******************/
double drandomrv(/*RNGdata *rng, long n, double v[]*/);
#define drandomr(rng)  drandomrv(rng,0,0)
#define drandomv(n,v)  drandomrv(0,n,v)
#define drandom()      drandomrv(0,0,0)

/******************/
/* frandomrv      */
/* Returns floats */
/******************/
float frandomrv(/*RNGdata *rng, long n, float v[]*/);
#define frandomr(rng)  frandomrv(rng,0,0)
#define frandomv(n,v)  frandomrv(0,n,v)
#define frandom()      frandomrv(0,0,0)

/*****************/
/* lrandomrv     */
/* Returns longs */
/*****************/
long lrandomrv(/*RNGdata *rng, long n, long v[]*/);
#define lrandomr(rng)  lrandomrv(rng,0,0)
#define lrandomv(n,v)  lrandomrv(0,n,v)
#define lrandom()      lrandomrv(0,0,0)

/******************************************/
/* brandomrv                              */
/* Returns "high quality" bits,           */
/*  i.e. each generate from the underlying*/
/*  RNG is used to generate one bit.      */
/******************************************/
int brandomrv(/*RNGdata *rng, long n, int v[]*/);
#define brandomr(rng)  brandomrv(rng,0,0)
#define brandomv(n,v)  brandomrv(0,n,v)
#define brandom()      brandomrv(0,0,0)

/******************************************/
/* brandomrv_f                            */
/* Returns "low quality" fast bits,       */
/*  i.e. each generate from the underlying*/
/*  RNG is used to generate 32 bits.      */
/* Return -1 if the range of the RNG      */
/*  is not 2^32.                          */
/******************************************/
/* Splitting of this fast bit stream is not currently supported. */
int brandomrv_f(/*RNGdata *rng, long n, int v[]*/);
#define brandomr_f(rng)  brandomrv_f(rng,0,0)
#define brandomv_f(n,v)  brandomrv_f(0,n,v)
#define brandom_f()      brandomrv_f(0,0,0)

/*********************************/
/* Unbuffered calling procedures */
/*********************************/
/*******************/
/* dxrandomrv      */
/* Returns doubles */
/*******************/
double dxrandomrv(/*RNGdata *rng, long n, double v[]*/);
#define dxrandomr(rng) dxrandomrv(rng,0,0)
#define dxrandomv(n,v) dxrandomrv(0,n,v)
#define dxrandom()     dxrandomrv(0,0,0)

/******************/
/* fxrandomrv     */
/* Returns floats */
/******************/
float fxrandomrv(/*RNGdata *rng, long n, float v[]*/);
#define fxrandomr(rng) fxrandomrv(rng,0,0)
#define fxrandomv(n,v) fxrandomrv(0,n,v)
#define fxrandom()     fxrandomrv(0,0,0)

/*****************/
/* lxrandomrv    */
/* Returns longs */
/*****************/
long lxrandomrv(/*RNGdata *rng, long n, long v[]*/);
#define lxrandomr(rng) lxrandomrv(rng,0,0)
#define lxrandomv(n,v) lxrandomrv(0,n,v)
#define lxrandom()     lxrandomrv(0,0,0)

/*******************************/
/* bxrandomrv                  */
/* Return "high quality" bits. */
/*******************************/
int bxrandomrv(/*RNGdata *rng, long n, int v[]*/);
#define bxrandomr(rng) bxrandomrv(rng,0,0)
#define bxrandomv(n,v) bxrandomrv(0,n,v)
#define bxrandom()     bxrandomrv(0,0,0)

/*******************************/
/* bxrandomrv_f                */
/* Return "low quality" bits.  */
/* Return -1 if range of RNG   */
/*  is not 2^32                */
/*******************************/
/* This routine will "lose" random bits if they are not requested in
 *  multiples of (RNGsplit+1)*BBUF_SIZE, since bits are returned by
 *  pulling them off the random stream sequentially, using up exactly
 *  as many as are needed in order to generate the requested number
 *  of bits.
 * Splitting of this fast bit stream is not currently supported.
 */
int bxrandomrv_f(/*RNGdata *rng, long n, int v[]*/);
#define bxrandomr_f(rng) bxrandomrv_f(rng,0,0)
#define bxrandomv_f(n,v) bxrandomrv_f(0,n,v)
#define bxrandom_f()     bxrandomrv_f(0,0,0)

/****************************/
/* seed_rng                 */
/* Seeds the underlying RNG */
/****************************/
void seed_rng(/*RNGdata *rng, long *seed*/);

/*************************************/
/* check_rng                         */
/* Check the integrity of the RNG.   */
/* Return 1 if ok, 0 if not.         */
/*************************************/
int check_rng(/*RNGdata *rng*/);

/*************************************/
/* describe_rng                      */
/* Write a short ASCII description   */
/*  of the RNG to the user-supplied  */
/*  string rngid, which must be of   */
/*  length at least RNGIDSTRLEN.     */
/* If the user has not initialized   */
/*  the rng with init_rng() or       */
/*  restart_rng(), abort with an     */
/*  error message to stderr.         */
/* Otherwise return rngid.           */
/* Currently only supports RNGs with */
/*  at most two seeds.               */
/*************************************/
char *describe_rng (/*RNGdata *rng, char rngid[RNGIDSTRLEN]*/);

/*************************************/
/* split_rng                         */
/* Modify rng to "leapfrog" a number */
/*  of generates determined by the   */
/*  value of new_value               */
/*  (new_value == 0 returns          */
/*     every generate)               */
/* Returns 0 if new_value < 0        */
/* Returns 1 otherwise               */
/* Exits on error if rng has not     */
/*  been initialized                 */
/*************************************/
int split_rng(/* RNGdata *rng, long new_value*/);

/*************************************/
/* mralg_rng                         */
/* Modify rng to use a different     */
/*  algorithm for mrandom()          */
/* Returns 0 if mralg is out of range*/
/* Returns 1 otherwise               */
/* Exits on error if rng has not     */
/*  been initialized                 */
/*************************************/
int mralg_rng(/* RNGdata *rng, long new_value*/);

/*************************************/
/* range_rng                         */
/* Return the range of the RNG       */
/*  (i.e. rng is capable of producing*/
/*  generates in the range           */
/*  0...(range_rng(rng)-1)           */
/* Exits on error if rng has not     */
/*  been initialized                 */
/*************************************/
double range_rng(/*RNGdata *rng*/);

/************/
/* init_rng */
/************/
/* Initialize the general-purpose rng data area so that it holds the
 * state and other data required for the selected algorithm "alg", where
 *	alg = 0 is a trivial generator that returns state += seed2,
 *	  where state is initially set to seed1.  Use for debugging only!
 *	alg = 1 is 4.3bsd random.c (non-linear additive feedback)
 *	alg = 2 is the Knuth/Bentley prand (lagged Fibonnacci)
 *	alg = 3 is L'Ecuyer's portable combined (multiplicative) RNG
 *	alg = 4 is 4.3bsd nrand48.c,
 *      alg = 5 is 4.3bsd rand
 *      alg = 6 is Press and Teukolsky's ran0
 *      alg = 7 is Press and Teukolsky's ran1
 *      alg = 8 is Press and Teukolsky's ran2
 *      alg = 9 is Marsaglia's Ultra RNG
 *
 * Note: Memory for rng is allocated by this routine.  Before calling
 *  this routine the user need only declare a pointer to the generator,
 *  for example
 *		RNGdata *myrng;
 *
 * The mrandom_alg parameter determines which algorithm will be used
 *  by mrandom when call with this RNG:
 *      0 = Thomborson's unbiased conversion
 *      1 = (int) (m * drandomr(rng))
 *
 * The bufsize parameter determines the number of entries in the
 *  buffer.  Buffer space is allocated dynamically during
 *  initialization.  Thirty-two entries are allocated for the bit
 *  buffer.
 *
 * The count1 parameter indicates how many times the selected RNG algorithm
 *  should be called prior to returning control to the calling routine.
 *  We recommend a high value, at least 10000, for this parameter, to minimize
 *  the ill effects of seeding.  The count2 parameter can be given a non-zero
 *  value if you wish to "cycle" the generator a huge number of times: it 
 *  will be called (count2*1e9 + count1) times.  Probably count2 should always
 *  be 0 until computers become much, much faster than today.
 *
 * init_rng() returns a pointer to the initialized RNG unless an
 *  out-of-range argument is detected (rngalg >9 or < 0; count1 >1e9 or <0;
 *  or count2 <0), or if memory cannot be allocated for the RNG,
 *  in which case it returns a null pointer.
 *
 * Note: a single program may call init_rng() any number of times, to set up
 * multiple generators (possibly using more than one RNG algorithm), e.g with 
 *		RNGdata *myrngs[8];
 *		long i,sum=0,seed[2];
 *              seed[1]=0;
 *		for (i=0; i<7; i++) {
 *		  / * generator #i gets seed1 = i
 *                seed[0]=i;
 *		  myrngs[i]=init_rng(2,0,seed,100000,0,1024);
 *		}
 *		/ * our eighth RNG uses algorithm #3
 *              seed[7]=7;
 *		myrngs[7]=init_rng(3,0,seed,100000,0,1024);
 *		/ * add 1-bit numbers, one from each generator
 *		for (i=0; i<7; i++) {
 *		  sum += mrandom(&myrngs[i],2);
 *		}
 *
 * Warning: do not attempt to use multiple RNGdata areas for algorithm #1.
 *  The 4.3bsd random.c code has internal state that will not be modified
 *  correctly when you switch generators (this was a bug in the original
 *  implementation and it would be very difficult to fix here).
 * 
 * Warning: Do NOT override previously-initialized RNGs with the results
 *  of this procedure.  If you have a pointer to a valid RNG and wish to
 *  initialize a new RNG using the same pointer, you should call
 *  kill_rng() before calling init_rng().  For example:
 *              ...
 *              i=lrandomr(rng); / * This RNG is in use
 *              kill_rng(rng);   / * Kill the RNG, and THEN
 *              rng=init_rng(3,1,seed,1000,0,256); / * init a new one
 *
 * We recommend that init_rng() be used very sparingly.  Except when
 * replicating experiments or debugging, it is better to restart an
 * existing generator (stored in a statefile) than to initialize a new one.
 */
RNGdata *init_rng(/* long alg, long mrandom_alg, long *seed,
		     long count1, long count2, long bufsize*/);

/*******************************/
/* kill_rng                    */
/* Frees memory used by an RNG */
/* Returns 0 if kill failed    */
/* Returns 1 otherwise         */
/*******************************/
int kill_rng(/*RNGdata *rng*/);

/*********************************************/
/* save_rng                                  */
/* Save the RNG state to a statefile.        */
/* Return 0 if RNG couldn't be saved.        */
/* Returns 1 otherwise.                      */
/*********************************************/
int save_rng(/* RNGdata *rng, char *filename*/);

/****************************************************************/
/* restart_rng                                                  */
/* Restart a generator from a statefile.                        */
/* Return a null pointer if the restart failed due to a garbled */
/*  or nonexistent statefile.                                   */
/* Otherwise return a pointer to the restarted RNG.             */
/* WARNING: An RNG which has been previously initialized using  */
/*  init_rng() should NOT be overwritten with the return value  */
/*  of this procedure.  In order to provide a "fresh" RNG for   */
/*  this procedure, do one of the following:                    */
/*     - Declare a new RNG                                      */
/*     - Kill a previously initialized RNG using kill_rng()     */
/****************************************************************/
RNGdata *restart_rng(/* char *filename*/);

/*********************************************/
/* flush_rng                                 */
/* Flush the contents of the RNG's buffers   */
/* Returns 1 upon success, 0 upon failure    */
/*********************************************/
int flush_rng(/*RNGdata *rng*/);

/*************/
/* mrandomrv */
/*************/
/* Generate a length-n vector v of random longs, uniformly distributed
 * in the range 0..m-1, using the indicated rng.  Return a copy of the
 * first random variate, v[0].
 *
 * Special-case parameter values: if rng==0, use the RNG that was
 * the most-recently initialized or restarted; if n==0, return one
 * random variate and don't write into v[]. 
 *
 * Our code does not have a deterministic bias for any m, unlike the
 * typical "good" code
 *		(int) floor( drandom() * (double) m )
 * or the commonly-used, but hazardous (because it exposes the flaws
 * in many RNGs) code 
 *		random()%m
 * We remove the bias by making multiple calls (on rare occasions)
 * to the underlying generator.  The expected number of RNG calls
 * is upper-bounded by n/(1 - (RNGrange%m)/RNGrange) < 2n.
 *
 * The program is aborted, with an error message, if
 *  m exceeds the range of the RNG.
 *
 * The program will also abort, again with an error message to stderr,
 * if the generator is behaving so non-randomly that our multiple-call
 * bias-removal algorithm makes an excessive number of calls to the
 * underlying generator.
 */
long mrandomrv (/* RNGdata *rng, long m, long n, long v*/);
#define mrandomr(rng,m)  mrandomrv(rng,m,0,0)
#define mrandomv(m,n,v)  mrandomrv(0,m,n,v)
#define mrandom(m)       mrandomrv(0,m,0,0)
