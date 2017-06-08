/* mrandom.c 3.1 5/28/93 */
/*
 *		mrandom.c
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
#include "mrandom.h"
#endif

/*****************/
/* Include files */
/*****************/
#include <malloc.h> /* To allocate space for RNGs*/
#include <math.h>   /* we need floor() */
#include <stdio.h>  /* We need FILE */
#include <string.h>
#include <values.h> /* We need MAXLONG */

/*******************************************************/
/* Header files for RNGs currently included in package */
/*******************************************************/
#include "rngs.h"    /* "Built-in" rngs:
			Trivial RNG:               Algorithm #0
			4.3bsd random:             Algorithm #1
			4.3bsd nrand48:            Algorithm #4
			4.3bsd rand:               Algorithm #5 */
#include "bentley.h" /* Knuth/Bentley prand:       Algorithm #2 */
#include "pcrand.h"  /* The portable combined RNG: Algorithm #3 */
#include "ran0.h"    /* Press & Teukolsky's ran0:  Algorithm #6 */
#include "ran1.h"    /* Press & Teukolsky's ran1:  Algorithm #7 */
#include "ran2.h"    /* Press & Teukolsky's ran2:  Algorithm #8 */
#include "ultra.h"   /* Marsaglia's Ultra RNG:     Algorithm #9 */

/*****************************************/
/* Arrays to hold information about RNGs */
/*****************************************/
char RNGname_a[NUM_RNGS][RNGIDSTRLEN]={RNGname_0, RNGname_1,
					 RNGname_2, RNGname_3,
					 RNGname_4, RNGname_5,
					 RNGname_6, RNGname_7,
					 RNGname_8, RNGname_9};
long statesize_a[NUM_RNGS]={RNGstatesize_0, RNGstatesize_1,
			      RNGstatesize_2, RNGstatesize_3,
			      RNGstatesize_4, RNGstatesize_5,
			      RNGstatesize_6, RNGstatesize_7,
			      RNGstatesize_8, RNGstatesize_9},
  seedsize_a[NUM_RNGS]={RNGseedsize_0, RNGseedsize_1, RNGseedsize_2,
			  RNGseedsize_3, RNGseedsize_4, RNGseedsize_5,
			  RNGseedsize_6, RNGseedsize_7, RNGseedsize_8,
			  RNGseedsize_9};
double range_a[NUM_RNGS]={RNGrange_0, RNGrange_1, RNGrange_2,
			    RNGrange_3, RNGrange_4, RNGrange_5,
			    RNGrange_6, RNGrange_7, RNGrange_8,
			    RNGrange_9};
signed int returns_a[NUM_RNGS]={RNGreturns_0, RNGreturns_1,
				  RNGreturns_2, RNGreturns_3,
				  RNGreturns_4, RNGreturns_5,
				  RNGreturns_6, RNGreturns_7,
				  RNGreturns_8, RNGreturns_9};
  int statetype_a[NUM_RNGS]={RNGstatetype_0, RNGstatetype_1,
			     RNGstatetype_2, RNGstatetype_3,
			     RNGstatetype_4, RNGstatetype_5,
			     RNGstatetype_6, RNGstatetype_7,
			     RNGstatetype_8, RNGstatetype_9};


typedef void (*SP)(RNGdata *, long *);
typedef double (*DPP)(RNGdata *);
typedef long (*LPP)(RNGdata *);
typedef int (*CP)(RNGdata *);
SP seed_a[NUM_RNGS] = {RNGseed_0, RNGseed_1, RNGseed_2, RNGseed_3,
			 RNGseed_4, RNGseed_5, RNGseed_6, RNGseed_7,
			 RNGseed_8, RNGseed_9};
DPP dgen_a[NUM_RNGS] = {RNGdgen_0, RNGdgen_1, RNGdgen_2,
			   RNGdgen_3, RNGdgen_4, RNGdgen_5,
			   RNGdgen_6, RNGdgen_7,  RNGdgen_8,
			   RNGdgen_9};
LPP lgen_a[NUM_RNGS] = {RNGlgen_0, RNGlgen_1, RNGlgen_2, RNGlgen_3,
			   RNGlgen_4, RNGlgen_5, RNGlgen_6,
			   RNGlgen_7, RNGlgen_8, RNGlgen_9};
CP check_a[NUM_RNGS] = {RNGcheck_0, RNGcheck_1, RNGcheck_2,
			  RNGcheck_3, RNGcheck_4, RNGcheck_5,
			  RNGcheck_6, RNGcheck_7, RNGcheck_8, RNGcheck_9};
 
/* The most-recently initialized or restarted RNG */
RNGdata *mru_rng = 0;

/* Format of the RNG statefile */
#define RNGfileLINE0   "RNG statefile for algorithm %ld, "
#define RNGfileLINE1   "Buffer size = %ld bytes\n"
#define RNGfileLINE2   "Initial seed table =\n"
#define RNGfileLINE3   \
 "Number of calls to underlying RNG after seeding = %ld billion + %ld\n"
#define RNGfileLINE4   "Next value in this pseudorandom sequence = \
%08lx\n"
#define RNGfileLINE5   "This RNG returns every %ld generates\n"
#define RNGfileLINE6   "This RNG uses mrandom algorithm %ld\n"
#define RNGfileLINE7   "RNG state table =\n"
char *RNGfileLINEn[]={"",
			"   %02lx %02lx %02lx %02lx\n",
			"   %04lx %04lx %04lx %04lx\n",
			"",
			"   %08lx %08lx %08lx %08lx\n"},
  *RNGfileLINEnn[]={"", "   %02lx", "   %04lx", "", "   %08lx"};

/* Error message for invalid RNGs */
#define BADRNG_ERR \
 fprintf(stderr, "RNG must be initialized or restarted before use!\n"); \
 fflush(stderr);\
 exit(1)

/**************************************************/
/* Auxiliary macros for RNG generating procedures */
/**************************************************/
/*************************************/
/* set_rng                           */
/* Sets rng to point to a valid      */
/*  RNGdata structure, if any        */
/*  can be found.                    */
/* Otherwise it calls                */
/*  the error routine no_rng().      */
/*************************************/
#define set_rng(rng)   if (rng == 0) { \
				if (mru_rng) { \
					rng = mru_rng; \
				} else { \
					BADRNG_ERR; \
				} \
			}

/*************************************/
/* inc_counts                        */
/* Increment an RNG counter n times  */
/*************************************/
/* Using two while loops is necessary.  If RNGcount1 is added to n
 *  before n is reduced to a number less than BILLION, it is
 *  possible that RNGcount1+n will overflow.
 * Possible overflow of count2 is not handled, because on today's
 *  computers it will not occur.
 */
#define inc_counts(rng,n) \
  while (n >= BILLION) { \
    n -= BILLION; \
    RNGcount2 += 1; \
  } \
  RNGcount1 += n; \
  while (RNGcount1 >= BILLION) { \
    RNGcount1 -= BILLION; \
    RNGcount2 += 1; \
  }

/*************************************/
/* fill_buffer                       */
/* Fill an RNG buffer                */
/*************************************/
void fill_buffer(rng)
     RNGdata *rng;
{
  long i,j; /* Loop variables */
  
  inc_counts(rng,RNGbuffer.size);
  
  if (RNGreturns == RET_LONG) { /* Underlying RNG returns longs */
    LPP proc;
    long *temp_l; /* Temporary pointer for efficiency */
    proc=lgen_a[RNGalg];
    RNGbuffer.lbufp=temp_l=RNGbuffer.lbuf;
    
    for (i=0; i<RNGbuffer.size; i++) { /* Fill the buffer */
      *temp_l++  = proc(rng);
      for (j=0;j<RNGsplit;j++) /* Handle split */
	proc(rng);
    }
  }
  else if (RNGreturns == RET_DOUBLE) { /* Underlying RNG returns */
                                       /* doubles */
    DPP proc;
    double *temp_d; /* Temporary pointer for efficiency */
    proc=dgen_a[RNGalg];
    RNGbuffer.dbufp=temp_d=RNGbuffer.dbuf;
    
    for (i=0; i<RNGbuffer.size; i++) { /* Fill the buffer */
      *temp_d++ = proc(rng);
      for (j=0;j<RNGsplit;j++) /* Handle split */
	proc(rng);
    }
  }
  RNGbuffer.nleft=RNGbuffer.size-1; /* Buffer is full */
}

/*************************************/
/* bfill_vector                      */
/* Fill a vector, with buffering,    */
/*  with a function of               *
/*  the underlying RNG               */
/*************************************/
#define bfill_vector(function) \
  while (n--) { \
		 if (RNGbuffer.nleft-- == 0) \
		   fill_buffer(rng); \
		 *v++ = function; \
		 }

/*************************************/
/* random_setup                      */
/* Handle special case of n == 0     */
/*************************************/
#define random_setup()\
  set_rng(rng); \
  if (n == 0) { \
    n=1; \
    v=vdef; \
  } \
  vreturn=v

/*************************************/
/* randomrv                          */
/* Kernel of buffered routines       */
/*************************************/
/* The order of the cases is important for maximum efficiency.
 * The order is optimized for the case in which a request is made for
 *  the same type as the value returned by the underlying RNG
 */
#define randomrv(case1, function1, case2, function2)\
  random_setup(); \
  if (RNGreturns == case1) \
    bfill_vector(function1) \
  else if (RNGreturns == case2) \
    bfill_vector(function2) \
  else \
    while (n--) \
      *v++ = 0; \
  return(*vreturn)

/*************************************/
/* fill_vector                       */
/* Fill a vector, without buffering, */
/*  with a function of               */
/*  the underlying RNG               */
/*************************************/
#define fill_vector(function) \
  while (n--) { \
		 *v++ = function; \
		 for(j=0;j<RNGsplit;j++) \
		   proc(rng); \
		 }

/*************************************/
/* xrandom_setup                     */
/* Setup common to all xrandoms      */
/*************************************/
#define xrandom_setup() \
  random_setup(); \
  inc_counts(rng,n)

/* Auxiliary routines for 32-bit generators */
double longtodouble(l)
     long l;
{
  if (l > 0)
    return((double)l);
  else if (l < 0)
    return((double)(l & 0x7fffffff) + 2147483648.0); /* 2^31 */
  else
    return(4294967296.0); /* 2^32 */
}

/* Coded as a macro for efficiency */
#define doubletolong(d) ((long)((unsigned long)(d)))

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
double drandomrv(rng,n,v)
     RNGdata *rng;
     long n;
     double v[];
{
  long i;
  double vdef[1],*vreturn;
  
  if (RNGrange > MAXLONG + 1.0) {
    randomrv(RET_DOUBLE, *RNGbuffer.dbufp++,
	     RET_LONG, longtodouble(*RNGbuffer.lbufp++)/RNGrange);
  }
  else {
    randomrv(RET_DOUBLE, *RNGbuffer.dbufp++,
	     RET_LONG, *RNGbuffer.lbufp++/RNGrange);
  }
}

/******************/
/* frandomrv      */
/* Returns floats */
/******************/
float frandomrv(rng,n,v)
     RNGdata *rng;
     long n;
     float v[];
{
  long i;
  float vdef[1],*vreturn;
  
  if (RNGrange > MAXLONG + 1.0) {
    randomrv(RET_DOUBLE, *RNGbuffer.dbufp++,
	     RET_LONG, longtodouble(*RNGbuffer.lbufp++)/RNGrange);
  }
  else {
    randomrv(RET_DOUBLE, *RNGbuffer.dbufp++,
	     RET_LONG, *RNGbuffer.lbufp++/RNGrange);
  }
}

/*****************/
/* lrandomrv     */
/* Returns longs */
/*****************/
long lrandomrv(rng,n,v)
     RNGdata *rng;
     long n;
     long v[];
{
  long vdef[1],*vreturn;

  if (RNGrange > MAXLONG + 1.0) {
    randomrv(RET_LONG, *RNGbuffer.lbufp++,
	     RET_DOUBLE, doubletolong(*RNGbuffer.dbufp++ * RNGrange));
  }
  else {
    randomrv(RET_LONG, *RNGbuffer.lbufp++,
	     RET_DOUBLE, *RNGbuffer.dbufp++ * RNGrange);
  }
}

/******************************************/
/* brandomrv                              */
/* Returns "high quality" bits,           */
/*  i.e. each generate from the underlying*/
/*  RNG is used to generate one bit.      */
/******************************************/
int brandomrv(rng,n,v)
     RNGdata *rng;
     long n;
     int v[];
{
  long i,j;
  int vdef[1],*vreturn;
  
  random_setup();

  for (i=0;i<n;i++) {
    if (RNGbuffer.nbleft-- == 0) { /* Bit buffer is empty, */
      /* so fill it */
      for (j=0;j<BBUF_SIZE;j++)
	RNGbuffer.bbuf[j]=drandomr(rng)*2.0;
      RNGbuffer.bbufp=RNGbuffer.bbuf;
      RNGbuffer.nbleft=BBUF_SIZE-1; /* Bit buffer is full */
    }
    *v++ = *RNGbuffer.bbufp++;
  }
  return(*vreturn);
}

/******************************************/
/* brandomrv_f                            */
/* Returns "low quality" fast bits,       */
/*  i.e. each generate from the underlying*/
/*  RNG is used to generate 32 bits.      */
/* Return -1 if the range of the RNG      */
/*  is not 2^32.                          */
/******************************************/
/* Splitting of this fast bit stream is not currently supported. */
int brandomrv_f(rng,n,v)
     RNGdata *rng;
     long n;
     int v[];
{
  long i,j,r;
  int vdef[1],*vreturn;
  
  random_setup();
  
  for (i=0;i<n;i++) {
    if (RNGbuffer.nbleft-- == 0) { /* Bit buffer is empty, */
      /* so fill it */
      r=lrandomr(rng);
      for (j=0;j<BBUF_SIZE;j++) {
	RNGbuffer.bbuf[j] = r & 1;
	r = r >> 1;
      }
      RNGbuffer.bbufp=RNGbuffer.bbuf;
      RNGbuffer.nbleft=BBUF_SIZE-1; /* Bit buffer is full */
    }
    *v++ = *RNGbuffer.bbufp++;
  }
  return(RNGrange == 4294967296.0 ? *vreturn: -1);
}

/***********************************************/
/* Unbuffered calling procedures               */
/***********************************************/
/*******************/
/* dxrandomrv      */
/* Returns doubles */
/*******************/
double dxrandomrv(rng,n,v)
     RNGdata *rng;
     long n;
     double v[];
{
  long i,j;
  double vdef[1],*vreturn;
  double d;

  xrandom_setup();
  if (RNGreturns == RET_DOUBLE) {
    DPP proc;
    proc=dgen_a[RNGalg];
    fill_vector(proc(rng));
  }
  else if (RNGreturns == RET_LONG) {
    LPP proc;
    proc=lgen_a[RNGalg];
    if (RNGrange > MAXLONG + 1.0) /* 32-bit generator */
      { fill_vector(longtodouble(lgen_a[RNGalg](rng))/RNGrange); }
    else
      { fill_vector((double)lgen_a[RNGalg](rng)/RNGrange); }
  }
  else
    while (n--)
      *v++ = 0;
  return(*vreturn);
}

/******************/
/* fxrandomrv     */
/* Returns floats */
/******************/
float fxrandomrv(rng,n,v)
     RNGdata *rng;
     long n;
     float v[];
{
  long i,j;
  float vdef[1],*vreturn;
  
  xrandom_setup();
  if (RNGreturns == RET_DOUBLE) {
    DPP proc;
    proc=dgen_a[RNGalg];
    fill_vector(proc(rng));
  }
  else if (RNGreturns == RET_LONG) {
    LPP proc;
    proc=lgen_a[RNGalg];
    if (RNGrange > MAXLONG + 1.0) /* 32-bit generator */
      { fill_vector(longtodouble(lgen_a[RNGalg](rng))/RNGrange); }
    else
      { fill_vector((double)lgen_a[RNGalg](rng)/RNGrange); }
  }
  else
    while (n--)
      *v++ = 0;
  return(*vreturn);
}

/*****************/
/* lxrandomrv    */
/* Returns longs */
/*****************/
long lxrandomrv(rng,n,v)
     RNGdata *rng;
     long n;
     long v[];
{
  long i,j;
  long vdef[1],*vreturn;
  
  xrandom_setup();
  if (RNGreturns == RET_LONG) {
    LPP proc;
    proc=lgen_a[RNGalg];
    fill_vector(proc(rng));
  }
  else if (RNGreturns == RET_DOUBLE) {
    DPP proc;
    proc=dgen_a[RNGalg];
    if (RNGrange > MAXLONG + 1.0) /* 32-bit generator */
      { fill_vector(doubletolong(dgen_a[RNGalg](rng)*RNGrange)); }
    else
      { fill_vector(dgen_a[RNGalg](rng)*RNGrange); }
  }
  else
    while (n--)
      *v++ = 0;
  return(*vreturn);
}

/*******************************/
/* bxrandomrv                  */
/* Return "high quality" bits. */
/*******************************/
int bxrandomrv(rng,n,v)
     RNGdata *rng;
     long n;
     int v[];
{
  long i,j;
  int vdef[1],*vreturn;
  
  xrandom_setup();
  if (RNGreturns == RET_LONG) {
    LPP proc;
    proc=lgen_a[RNGalg];
    fill_vector(((double)proc(rng)/RNGrange)*2.0);
  }
  else if (RNGreturns == RET_DOUBLE) {
    DPP proc;
    proc=dgen_a[RNGalg];
    fill_vector(proc(rng)*2.0);
    }
  else
    while (n--)
      *v++ = 0;
  return(*vreturn);
}

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
int bxrandomrv_f(rng,n,v)
     RNGdata *rng;
     long n;
     int v[];
{
  long i,j;
  int vdef[1],*vreturn;
  long value;
  long index;
  
  inc_counts(rng,n);
  set_rng(rng);
  random_setup();
  
  index=0;
  
  for (i=0;i<n;i++) {
    if (index-- == 0) {
      value=lxrandomr(rng);;
      index=31;
    }
    v[i] = value & 1;
    value = value >> 1;
  }
  return(RNGrange == 4294967296.0 ? *vreturn: -1);
}

/*************************************/
/* seed_rng                          */
/* Seeds the underlying RNG          */
/*************************************/
void seed_rng(rng, seed)
     RNGdata *rng;
     long *seed;
{
  long i;
  
  /* Keep a record of the seeds */
  for (i=0;i<RNGseedsize;i++)
    RNGseed[i]=seed[i];
  RNGcount1 = 0;             /* Reset counter */
  RNGcount2 = 0;
  flush_rng(rng);            /* Flush the RNG's buffers */
  seed_a[RNGalg](rng, seed); /* Seed the underlying RNG */
  mru_rng = rng;             /* This RNG is now the most recently */
                             /* initialized or restarted */
}

/*************************************/
/* setRNGparams                      */
/* Set RNGalg to alg, and set all    */
/*  information which can be derived */
/*  from RNGalg.                     */
/* Return 0 and print message to     */
/*  stderr if alg is out of range    */
/* Return 1 otherwise                */
/*************************************/
int setRNGparams(rng,alg)
     RNGdata *rng;
     long alg;
{
  if (alg<0 || alg>NUM_RNGS-1) /* alg is out of range */
    return(0);
  else { /* Set RNG parameters */
    RNGalg = alg;
    RNGstatesize = statesize_a[alg];
    RNGseedsize = seedsize_a[alg];
    RNGrange = range_a[alg];
    strcpy(RNGname,RNGname_a[alg]);
    RNGreturns = returns_a[alg];
    RNGstatetype = statetype_a[alg];
    return(1);
  }
}

/*************************************/
/* check_rng                         */
/* Check the integrity of the RNG.   */
/* Return 1 if ok, 0 if not.         */
/*************************************/
int check_rng(rng)
     RNGdata *rng;
{
  /* Temporary variables */
  long statesize, seedsize;
  double range;
  signed int returns;
  char name[RNGIDSTRLEN];
  int statetype;

  if (rng == 0)
    return(0);
  
  /* Have derived RNG values been overwritten? */
  statesize = RNGstatesize;
  seedsize = RNGseedsize;
  range = RNGrange;
  strcpy(name,RNGname);
  returns = RNGreturns;
  statetype = RNGstatetype;

  if (!setRNGparams(rng,RNGalg))
    return(0);

  if (statesize != RNGstatesize || seedsize != RNGseedsize ||
      range != RNGrange || strcmp(name,RNGname) || returns !=
      RNGreturns || statetype != RNGstatetype)
    return(0);

  /* Now look at RNGstate (algorithm-specific tests) */
  return(check_a[RNGalg](rng));
}

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
char *describe_rng (rng,rngid)
     RNGdata *rng;
     char rngid[RNGIDSTRLEN];
{
  if (!check_rng(rng)) {
    BADRNG_ERR;
  }
  sprintf(rngid, "RNG state identifier is (%ld, %ld: %ld, %ld; %ld,\
%ld; %ld, %ld)\n",
	  RNGalg, RNGmrandom_alg, RNGseed[0], RNGseed[1],
	  RNGcount1, RNGcount2, RNGbuffer.size, RNGsplit);
  return(rngid);
}

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
int split_rng(rng,new_value)
     RNGdata *rng;
     long new_value;
{
  if (check_rng(rng)) {
    if (new_value < 0)
      return(0);
    else {
      RNGsplit=new_value;
      return(1);
    }
  }
  else {
    BADRNG_ERR;
  }
}

/*************************************/
/* mralg_rng                         */
/* Modify rng to use a different     */
/*  algorithm for mrandom()          */
/* Returns 0 if mralg is out of range*/
/* Returns 1 otherwise               */
/* Exits on error if rng has not     */
/*  been initialized                 */
/*************************************/
int mralg_rng(rng,new_value)
     RNGdata *rng;
     long new_value;
{
  if (check_rng(rng)) {
    if (new_value == 0 || new_value == 1) {
      RNGmrandom_alg=new_value;
      return(1);
    }
    else
      return(0);
  }
  else {
    BADRNG_ERR;
  }
}

/*************************************/
/* range_rng                         */
/* Return the range of the RNG       */
/*  (i.e. rng is capable of producing*/
/*  generates in the range           */
/*  0...(range_rng(rng)-1)           */
/* Exits on error if rng has not     */
/*  been initialized                 */
/*************************************/
double range_rng(rng)
     RNGdata *rng;
{
  if (check_rng(rng))
    return(RNGrange);
  else {
    BADRNG_ERR;
  }
}

/* Auxiliary routine for allocating memory for an RNG */
RNGdata *allocate_rng(rng, alg, bufsize)
     RNGdata *rng;
     long alg, bufsize;
{
  if (bufsize <= 0)
    bufsize=1;
  if ((rng = (RNGdata *)malloc(sizeof(RNGdata))) == 0)
    return(0);
  if (!setRNGparams(rng,alg))
    return(0);
  if ((RNGstate=(long *)calloc(RNGstatesize,sizeof(long))) == 0)
    return(0);
  /* Allocate and clear at least two elements for describe_rng */
  if ((RNGseed=(long *)calloc(RNGseedsize > 2 ?
			    RNGseedsize:2,sizeof(long))) == 0)
    return(0);
  if (RNGreturns == RET_LONG) {
    if ((RNGbuffer.lbuf=RNGbuffer.lbufp =
	 (long *) calloc(bufsize,sizeof(long))) == 0)
      return(0);
    RNGbuffer.dbuf=RNGbuffer.dbufp=0;
  }
  else if (RNGreturns == RET_DOUBLE) {
    if ((RNGbuffer.dbuf=RNGbuffer.dbufp =
	 (double *) calloc(bufsize,sizeof(double))) == 0)
      return(0);
    RNGbuffer.lbuf=RNGbuffer.lbufp=0;
  }
  if ((RNGbuffer.bbuf=RNGbuffer.bbufp =
       (int *) calloc(BBUF_SIZE,sizeof(int))) == 0)
    return(0);
  RNGbuffer.size=bufsize;
  RNGbuffer.nleft=0;
  RNGbuffer.nbleft=0;
  return(rng);
}

/************/
/* init_rng */
/************/
/* Initialize the general-purpose rng data area so that it holds the
 * state and other data required for the selected algorithm "alg", where
 *	alg = 0 is a trivial generator that returns state += seed2,
 *	  where state is initially set to seed1.  Use for debugging only!
 *	alg = 1 is 4.3bsd random.c (non-linear additive feedback)
 *	alg = 2 is the Knuth/Bentley prand (lagged Fibonnacci)
 *	alg = 3 is the portable combined (multiplicative) RNG
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
 *		  /* generator #i gets seed1 = i
 *                seed[0]=i;
 *		  myrngs[i]=init_rng(2,0,seed,100000,0,1024);
 *		}
 *		/* our eighth RNG uses algorithm #3
 *              seed[7]=7;
 *		myrngs[7]=init_rng(3,0,seed,100000,0,1024);
 *		/* add 1-bit numbers, one from each generator
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
 *              i=lrandomr(rng); /* This RNG is in use
 *              kill_rng(rng);   /* Kill the RNG, and THEN
 *              rng=init_rng(3,1,seed,1000,0,256); /* init a new one
 *
 * We recommend that init_rng() be used very sparingly.  Except when
 * replicating experiments or debugging, it is better to restart an
 * existing generator (stored in a statefile) than to initialize a new one.
 */
RNGdata *init_rng(alg, mrandom_alg, seed, count1, count2, bufsize)
     long alg;         /* The RNG algorithm to use */
     long mrandom_alg; /* Algorithm to use for mrandom:
			  0 = Thomborson's slow but unbasied conversion
			  1 = (int) (m*drandom()) */
     long *seed;          /* Seed */
     long count1, count2; /* Initial counts */
     long bufsize;        /* Size of buffer */
{
  RNGdata *rng;
  double x[64]; /* Temporary vector */
  long i;       /* Loop variable    */
  
  /* Are count1 and count2 in range? */
  if (count1 >= BILLION || count1 < 0 || count2 < 0) {
    return(0);
  }
  /* Is count2 nonzero? */
  if (count2 != 0) {
    fprintf(stderr, "Warning: this initialization will take a LONG time!\n");
    fflush(stderr);
  }
  if ((rng=allocate_rng(rng,alg,bufsize))==0)
    return(0); /* Allocate space for the RNG */
  seed_rng(rng, seed);       /* Seed the RNG */
  if (mralg_rng(rng, mrandom_alg) == 0)
    return(0); /* Set default algorithm for mrandom */
  if (split_rng(rng, SPLIT_DEF) == 0)
    return(0); /* Set default split */
  /* Cycle the generator, in blocks of 64 (for speed) */
  for ( ; RNGcount2 < count2; ) { /* Get to the right BILLION count */
    dxrandomrv(rng,64,x);
  }
  for ( ; RNGcount1+64 < count1; ) { /* Get close to the right count1 */
    dxrandomrv(rng,64,x);
  }
  for ( ; RNGcount1 != count1; ) { /* and, finally, do the last few */
    dxrandomrv(rng,1,x);
  }
  return(rng); /* normal exit */
  
}

/*******************************/
/* kill_rng                    */
/* Frees memory used by an RNG */
/* Returns 0 if kill failed    */
/* Returns 1 otherwise         */
/*******************************/
int kill_rng(rng)
     RNGdata *rng;
{
  if (check_rng(rng)) {
    free(RNGstate);
    free(RNGseed);
    free(RNGbuffer.lbuf);
    free(RNGbuffer.dbuf);
    free(RNGbuffer.bbuf);
    free(rng);
    return(1);
  }
  else
    return(0);
}

/***************************************/
/* nextval                             */
/* Return the next lxrandomrv() output */
/*  without disturbing the RNG state.  */
/***************************************/
long nextval(rng)
     RNGdata *rng;
{
  static long *state=0;
  long i, r, tcount1, tcount2, retval;
  
  if (state != 0)
    free(state);
  state = (long *)calloc(RNGstatesize,sizeof(long));
  /* Check that this only allocates the first time */
  
  /* Preserve old RNG state */
  tcount1 = RNGcount1;
  tcount2 = RNGcount2;
  for (i=0; i<RNGstatesize; i++) {
    state[i] = RNGstate[i];
  }
  
  /* Find the next value in this pseudorandom sequence */
  retval=lxrandomr(rng);
  
  /* Restore the RNG state */
  RNGcount1 = tcount1;
  RNGcount2 = tcount2;
  /* Special fixup for 4.3bsd random()'s hidden state variables */
  if (RNGalg == 1) for (i=1; i<31; i++) {
    r = random();
  }
  for (i=0; i<RNGstatesize; i++) {
    RNGstate[i] = state[i];
  }
  
  return(retval);
}

/* printvector */
/* auxiliary routine for save_rng */
void printvector(fp,size,vector,type)
     FILE *fp;
     long size;
     long *vector;
     int type;
{
  long i,j,printval;
  long windex,bindex;
  long mask;

  mask=0;
  for(j=0;j < (type << 3); j++)
    mask = (mask << 1) | 1;
  
  windex=0;
  bindex=31;
  i=0;

  while (windex<size) {
    printval = 0;
    for (j=0; j < (type << 3); j++) {
      printval |= (vector[windex] & (1 << bindex));
      if (bindex-- == 0) {
	bindex = 31;
	windex++;
      }
    }
    fprintf(fp, RNGfileLINEnn[type],
	    (printval >> ((bindex+1)%32)) & mask);
    if (++i == 4) {
      fprintf(fp,"\n");
      i = 0;
    }
  }
  fprintf(fp,"\n");
}

/* scanvector */
/* auxiliary routine for restart_rng */
void scanvector(fp,size,vector,type)
     FILE *fp;
     long size;
     long *vector;
     int type;
{
  long i,j,scanval;
  long windex,bindex;

  windex=-1;
  bindex=0;
  i=0;

  while (windex<size) {
    fscanf(fp,RNGfileLINEnn[type],&scanval);
    for (j=0; j < (type << 3); j++) {
      if (bindex-- == 0) {
	bindex=31;
	windex++;
	vector[windex]=0;
      }
    }
    vector[windex] |= (scanval << bindex);
    if (++i == 4) {
      fscanf(fp,"\n");
      i = 0;
    }
  }
  fscanf(fp,"\n");
}

/*********************************************/
/* save_rng                                  */
/* Save the RNG state to a statefile.        */
/* Return 0 if RNG couldn't be saved.        */
/* Returns 1 otherwise.                      */
/*********************************************/
int save_rng(rng,filename)
     RNGdata *rng;
     char *filename;
{
  FILE *fp;
  long i;
  unsigned short *sarray;
  RNGdata *temp_rng;

  if (!check_rng(rng))
    return(0);

  /* Write the statefile */
  fp = fopen(filename, "w");
  if (!fp) /* Couldn't open file for writing */
    return(0);
  
  fprintf(fp, RNGfileLINE0, RNGalg);
  fprintf(fp,RNGname_a[RNGalg]);
  fprintf(fp, RNGfileLINE1,RNGbuffer.size);
  fprintf(fp, RNGfileLINE2);
  printvector(fp,RNGseedsize,RNGseed,RNGstatetype);
  fprintf(fp, RNGfileLINE3, RNGcount2, RNGcount1);
  fprintf(fp, RNGfileLINE4, nextval(rng));
  fprintf(fp, RNGfileLINE5, RNGsplit+1);
  fprintf(fp, RNGfileLINE6, RNGmrandom_alg);
  fprintf(fp, RNGfileLINE7);
  printvector(fp,RNGstatesize,RNGstate,RNGstatetype);
  fclose(fp);

  temp_rng=restart_rng(filename); /* Verify save */
  if (temp_rng == 0)
    return(0);
  else {
    kill_rng(temp_rng);
    return(1);
  }
}

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
RNGdata *restart_rng(filename)
     char *filename;
{
  FILE *fp;
  unsigned short *sarray; /* for reading alg #4's statefile */
  long i, m, r; /* temps */
  long alg, bufsize;
  int errflag; /* initially 0, becomes 1 if a problem is discovered */
  RNGdata *rng;
  
  /* If mru_rng == 0, we assume random() hasn't been called yet,
   * so its internal state is at its startup values.
   * If mru_rng != 0 and alg=1, then we've used random() already, so we
   * must reset its internal state to its startup values.
   */
  if (mru_rng != 0 && mru_rng->rngalg == 1) {
    m = (mru_rng->rngcount1%31) + mru_rng->rngcount2*(BILLION%31);
    /* note: the hidden state variables are counters mod 31 */
    for ( ; (m%31) != 0; m++) {
      r = random();
    }
  }
  
  /* Restore counter values, retrieve original RNG seeds and current state */
  fp = fopen(filename, "r");
  if (!fp)
    return(0);
  /* Read statefile */
  fscanf(fp, RNGfileLINE0, &alg);
  fscanf(fp,RNGname_a[alg]);
  fscanf(fp, RNGfileLINE1, &bufsize);
  if ((rng=allocate_rng(rng, alg, bufsize))==0)
    return(0);
  fscanf(fp, RNGfileLINE2);
  scanvector(fp,RNGseedsize,RNGseed,RNGstatetype);
  fscanf(fp, RNGfileLINE3, &RNGcount2, &RNGcount1);
  fscanf(fp, RNGfileLINE4, &RNGnextval);
  fscanf(fp, RNGfileLINE5, &RNGsplit);
  RNGsplit--;
  fscanf(fp, RNGfileLINE6, &RNGmrandom_alg);
  fscanf(fp, RNGfileLINE7);
  scanvector(fp,RNGstatesize,RNGstate,RNGstatetype);
  fclose(fp);

  errflag = 0; /* no errors detected yet */

  /* If reconstruction will be rapid, do it as an error check. */ 
  if (RNGcount1 < 1000 && RNGcount2 == 0) {
    RNGdata *test_rng;
    test_rng = init_rng(RNGalg, RNGmrandom_alg, RNGseed,
			RNGcount1, RNGcount2, RNGbuffer.size);
    /* See if we got to the same state */
    for (i=0; i<RNGstatesize; i++) {
      if ((test_rng->rngstate)[i] != RNGstate[i]) {
        errflag = 1;
      }
    }
    kill_rng(test_rng);
  } else { /* Just update mru_rng */
    /* first, some special hacks for alg 1 */
    if (RNGalg == 1) {
      /* tell random() we've got a 31-word statefile */
      RNGstate[0] = MRNGSTATE0;
      setstate(RNGstate);
      /* and modify random()'s hidden state to correspond to the RNGcount */
      m = RNGcount1%31 + RNGcount2*(BILLION%31);
      for (i=0 ; i<(m%31); i++) {
        r = random();
      }
    }
    mru_rng = rng; /* remember this rng, for use by mrandom() and frandom() */
  }
  
  /* see if RNG state looks ok */
  errflag = errflag || !check_rng(rng);

  /* Check nextval() operation */
  if (nextval(rng) != nextval(rng)) {
    errflag = 1;
  }

  if (errflag) {
    kill_rng(rng);
    return(0);
  }
  return(rng);
}

/*********************************************/
/* flush_rng                                 */
/* Flush the contents of the RNG's buffers   */
/* Returns 1 upon success, 0 upon failure    */
/*********************************************/
int flush_rng(rng)
     RNGdata *rng;
{
  if (check_rng(rng)) {
    if (RNGreturns == RET_LONG) {
      RNGbuffer.lbufp=RNGbuffer.lbuf;
      RNGbuffer.dbuf=RNGbuffer.dbufp=0;
    }
    else if (RNGreturns == RET_DOUBLE) {
      RNGbuffer.dbufp=RNGbuffer.dbuf;
      RNGbuffer.lbuf=RNGbuffer.lbufp=0;
    }
    RNGbuffer.nleft=0;
    RNGbuffer.bbufp=RNGbuffer.bbuf;
    RNGbuffer.nbleft=0;
    return(1);
  }
  else
    return(0);
}

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
long mrandomrv(rng,m,n,v)
     RNGdata *rng;
     long m,n,v[];
{
  long i,yield; /* temps */
  long ncalls; /* counts number of RNG calls during this mrandomrv call */
  long vdef[1],*vreturn; /* default value for v */
  char rngdesc[RNGIDSTRLEN]; /* for a describe_rng() call */
  double d; /* temporary variable */

  /* we can avoid some calcs&tests if m and rng range are unchanged */ 
  static long lastm=0,lastrangem1=0; /* m on last call */
  static double lastrange=0;         /* range on last call */
  static double maxv;  /* largest "useful" RNG output for this m */
  static double ifreq; /* multiplicity of RNG -> mrandomr() mapping */
  static double dm;    /* floated value of m */
  static int is32bit;  /* is rng is 32-bit generator? */

  set_rng(rng);

  random_setup();

  if (m != lastm) {
    dm=longtodouble(m);
    lastm=m;
    if (dm > RNGrange) { /* Is m in range? */
      fprintf(stderr,
	      "Error: mrandom() argument value, %ld, is out of range.\n", m);
      fprintf(stderr,
	      "The range of this RNG is %.0f.\n", RNGrange);
      fflush(stderr);
      exit(1);
    }
  }
  switch (RNGmrandom_alg) {
  case 0: /* Thomborson's unbiased conversion */
    /* see if RNGrange has changed recently */
    if (RNGrange != lastrange) {
      /* yes, save current RNGrange ... */
      lastrange = RNGrange;

      /* ... and recalculate ifreq and maxv */      
      /* Compute ifreq = multiplicity of RNG -> mrandom(m) mapping,
       * and maxv = the largest "useful" RNG output for this m.
       *
       * You might want to rewrite this code if you don't have FP hardware.
       */
      ifreq = floor(RNGrange/dm);
      maxv = RNGrange - (int)(RNGrange - ifreq*dm);
      is32bit = (RNGrange > MAXLONG + 1.0);
    }
    
    /* the expected # of calls to underlying RNG is */
    /* n/(1-xdiscard/range) < 2n */
    
    ncalls = 0; /* number of RNG calls so far */
    for (yield=0; yield<n;  ) {
      
      /* fill (or re-fill) v[] */
      lxrandomrv(rng,n-yield,&v[yield]);
      /* make sure ncalls doesn't get ridiculously large */
      ncalls += n-yield;
      if (ncalls > 3*n+300) {
	/* For yield ==n, mean(ncalls) < 2*n; std dev < sqrt(2*n);
	 * If ncalls > 3*n + 300, we are dozens of stddevs away from mean!
	 */
	fprintf(stderr, "Trouble in mrandomrv, m = %ld\n",m);
	fprintf(stderr, "Aborting program: this RNG is apparently stuck!\n");
	fprintf(stderr, describe_rng(rng,rngdesc));
	exit(1);
      }
      
      /* the following steps are coded for 32-bit and non-32-bit RNGs */
      /* for reasons of efficiency (32-bit RNGs will be slower)       */
      /* first, for all generators except 32-bit generators */
      if (!is32bit) {
	for (  ; yield<n; yield++) {
	  if (v[yield] > maxv) {
	    break;
	  }
	}
	for (i=yield+1; i<n; i++) {
	  if (v[i] <= maxv) {
	    v[yield] = v[i];
	    yield++;
	  }
	}
	for (i=0; i<n; i++) {
	  v[i] = (long) ((double)v[i] / ifreq);
	}
      }
      else { /* for 32-bit generators */
	/* find first out-of-range v[], if any, */
	for (  ; yield<n; yield++) {
	  if (longtodouble(v[yield]) > maxv) {
	    break;
	  }
	}
	/* and move in-range values to front of v[] */
	for (i=yield+1; i<n; i++) {
	  if (longtodouble(v[i]) <= maxv) {
	    v[yield] = v[i];
	    yield++;
	  }
	}
	/* map v[] values, with ifreq:1 multiplicity, */
	/* into the range 0..m-1 */
	for (i=0; i<n; i++) {
	  v[i] = (long) (longtodouble(v[i]) / ifreq);
	}
      }
    }
    return (*vreturn);
    break;
  case 1: /* (long) m*dxrandomr() */
    for (i=0;i<n;i++)
      v[i]= (long) (dm*dxrandomr(rng));
    return(*vreturn);
    break;
  default:
    break;
  }
}
