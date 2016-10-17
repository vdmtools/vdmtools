/*
 $Id: random.cc,v 1.4 2004/03/22 11:34:30 mikhail Exp $

 This program is public domain and was written by William S. England
 (Oct 1988).  It is based on an article by:

 Stephen K. Park and Keith W. Miller. RANDOM NUMBER GENERATORS:
 GOOD ONES ARE HARD TO FIND. Communications of the ACM,
 New York, NY.,October 1988 p.1192

 Modifications;

 $Log: random.cc,v $
 Revision 1.4  2004/03/22 11:34:30  mikhail
 An expression can be used as the argument of the duration statement

 Revision 1.3  2001/06/12 15:04:50  paulm
 Updates for wchar_t/wstring/string literals (i18n)

 Revision 1.2  1995/07/05 14:34:30  erik
 Changed defines to const variables.
 Changed name of seed function from srand to set_seed.
 Added get_random function to compute random [0..m).

// Revision 1.1.1.1  1995/07/05  11:35:08  erik
// Simple RNG.
//

######

 The following is a portable c program for generating random numbers.
 The modulus and multiplier have been extensively tested and should
 not be changed except by someone who is a professional Lehmer generator
 writer.  THIS GENERATOR REPRESENTS THE MINIMUM STANDARD AGAINST WHICH
 OTHER GENERATORS SHOULD BE JUDGED. ("Quote from the referenced article's
 authors. WSE" )
*/

/*
**  These are pre-calculated  below to compensate for c
**  compilers that may overflow when building the code.
**
**  q = (m / a)
**  r = (m % a)
*/

/*
** To build the generator with the original ACM
** article's numbers use -DORIGINAL_NUMBERS when compiling.
**
** Original_numbers are the original published m and q in the
** ACM article above.  John Burton has furnished numbers for
** a reportedly better generator.  The new numbers are now
** used in this program by default.
*/

/*
** F(z) = (az)%m
**      = az-m(az/m)
**
** F(z)  = G(z)+mT(z)
** G(z)  = a(z%q)- r(z/q)
** T(z)  = (z/q) - (az/m)
**
** F(z)  = a(z%q)- rz/q+ m((z/q) - a(z/m))
**       = a(z%q)- rz/q+ m(z/q) - az
*/

#include "random.h"
#include <iostream>
using namespace std;

#ifndef ORIGINAL_NUMBERS
unsigned long Random::m = (unsigned long)2147483647;
unsigned long Random::q = (unsigned long)44488;
unsigned long Random::a = (unsigned int)48271;
unsigned long Random::r = (unsigned int)3399;

#define successfulltest 399268537
#else
unsigned long Random::m = (unsigned long)2147483647;
unsigned long Random::q = (unsigned long)127773;
unsigned long Random::a = (unsigned int)16807;
unsigned long Random::r = (unsigned int)2836;

#define successfulltest 1043618065
#endif // ORIGINAL_NUMBERS

void Random::set_seed( unsigned long initial_seed)
{
  if (initial_seed == 0)
    this->seed = 89482311;
  else
    this->seed = initial_seed;
}

unsigned long Random::vdm_rand()
{
  int hi = this->seed/q;
  int lo = this->seed%q;
  int test = a * lo - r * hi;

  this->seed = ((test > 0) ? test : test + m);

  return this->seed;
}

int Random::get_random(int limit)
{
  return (int) (((double)this->vdm_rand()/(double)m)*limit);
}

#ifdef TESTRAND
#include <stdio.h>
#include <wchar.h>
#include <stdlib.h>
#include <iostream>
//using namespace std;
/*
**   The result of running this program should be
**   L"successfulltest".  If this program does not yield this
**   value then your compiler has not implemented this
**   program correctly.
**
**   To compile with test option under unix use; 'cc -DTESTRAND random.c'
**
**   Be sure to compile without test option for use in applications.
**   ( Now why did I have to say that ??? )
*/

int main(/*void*/)
{
  bool success = false;

  Random rnd;

  //set_seed(1);
  rnd.set_seed(1);

  for(int i = 1; i <= 10001; i++)
  {
    unsigned long n_rand = rnd.vdm_rand();

    if( i> 9998)
      wcout << L"Sequence " << i << L", Seed= " << rnd.seed << endl;

    if( i == 10000)
      if( rnd.seed == successfulltest ) success = true;
  }

  if (success){
    wcout << L"The random number generator works correctly." << endl;
  }else{
    wcout << L"The random number generator DOES NOT WORK!" << endl;
  }

  const int Mn = 100; 
  int N[1000] = {0,};
  int M[Mn+1] = {0,}; 
  for ( int j = 0; j < 10000000; j++ ) {
//    unsigned int n = vdm_rand();
    unsigned int n = rnd.vdm_rand();
    if ( n < 1000 ) {
      N[n]++;
    }
    double d = (double)n/Random::m;
    d = d*Mn;
    M[(int)d] ++;    
  }
//  set_seed(87654321); 
  rnd.set_seed(87654321); 

  for ( int k = 0; k < 1000; k++ ) {
    if (N[k] != 0) {
      wcout << L"n[" << k << L"] = " << N[k] << endl;
    }
  }

  for ( int l = 0; l <= Mn; l++ ) {
    wcout << L"[" << l << L"] = " << M[l] << L"\t";
    if (((l+1) % 5) == 0)
      wcout << endl;
  }
  return 0;
}
#endif // TESTRAND

