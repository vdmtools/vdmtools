/***
*  * WHAT
*  * FILE
*  *    $Source:$
*  * VERSION
*  *    $Revision:$
*  *    $Date:$
*  * AUTHOR
*  *    $Author:$
*  * COPYRIGHT
*  *    (C) Kyushu University
***/

#ifndef _random_h_
#define _random_h_

class Random
{
public:
  Random() {this->seed = 0;};
  Random(unsigned long initial_seed) {this->seed = initial_seed;};
  void set_seed(unsigned long initial_seed);
  int get_random(int limit);

#ifndef TESTRAND
private:
#endif // TESTRAND
  unsigned long vdm_rand();
  unsigned long seed;

  static unsigned long m; 
  static unsigned long q; 
  static unsigned long a; 
  static unsigned long r; 
};

#endif // _random_h_

