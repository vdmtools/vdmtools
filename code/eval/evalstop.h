/***
*  * WHAT
*  *    
*  * FILE
*  *    $Source:$
*  * PROJECT
*  *    VDM-SL Toolbox
*  * COPYRIGHT
*  *    (C) Kyushu University
***/

#include <time.h>
#include "tools.h"
#include "Interface.h"
#include "stackeval.h"

#ifndef CLOCKS_PER_SEC
#define CLOCKS_PER_SEC 50
#define _LOCALDEF_CLOCKS_PER_SEC
#endif

extern int StopTestCounter;
extern int CounterThreshold;
extern double LastCountTime;
extern StackEval & theDebugger();

inline void TestForStop() {
  
  if (StopTestCounter > CounterThreshold) {
    StopTestCounter=0;
    double timeUsed = (clock() - LastCountTime)/(double) CLOCKS_PER_SEC;
    if (timeUsed == 0) {
      // This is to avoid that the toolbox will get through CounterThreshold evaluations
      // even before the clock has increased by one.
      timeUsed = 0.1;
    }
    CounterThreshold = (int) (CounterThreshold*(1/timeUsed));
    LastCountTime = clock();
    //cout << L"UPDATE\n";
    Interf()->vdm_RefreshInterface();
  }
  StopTestCounter++;
}

#ifdef _LOCALDEF_CLOCKS_PER_SEC
#undef CLOCKS_PER_SEC
#endif
