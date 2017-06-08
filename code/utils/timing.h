///////////////////////////////////////////////////////////////////////////// 
// WHAT
//    Definition of a simple Timing class. 
//    
// ID
//    $Id: timing.h,v 1.5 2001/10/19 15:53:04 paulm Exp $
// PROJECT
//   Toolbox
// COPYRIGHT
//   (C) Kyushu University
///////////////////////////////////////////////////////////////////////////// 

#ifndef __timing_h__
#define __timing_h__

#include <time.h>
#include <string>

struct Timing {
  clock_t t1, t2;
  Timing() : t1(0), t2(0) { }

  std::string ctm() const 
  {
    time_t t;
    std::string s;
    t = time(0);
    s = ctime(&t); 
    s.erase(s.length()-1);
    return s;
  }

  void start(const char *s = "") 
  {
    t1 = clock(); 
    cerr << s 
         << " ctime=" << ctm() << '\n' 
         << flush; 
  }
  void stop(const char *s) 
  { 
    t2 = clock(); 
    cerr << s << ' ' << (double)(t2-t1)/CLOCKS_PER_SEC 
         << " ctime=" << ctm() << "\n" 
         << flush; 
  }
};

#endif // __timing_h__
