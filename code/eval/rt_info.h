/***
*  * WHAT
*  *    Gnerates information for run-time analysis
*  * FILE
*  *    $Source: /home/vdmtools/cvsroot/toolbox/code/eval/rt_info.h,v $
*  * VERSION
*  *    $Revision: 1.10 $
*  * DATE
*  *    $Date: 2005/12/02 06:28:32 $
*  * STATUS
*  *    $State: Exp $
*  * REFERENCES
*  *    
*  * PROJECT
*  *    Afrodite - ESPRIT programme, project no. 6500.
*  * AUTHOR
*  *    Poul Boegh Lassen (PBL) + $Author: vdmtools $
*  * COPYRIGHT
*  *    (C) Kyushu University
***/

#include "metaiv.h"
#include <fstream>
#include "contextinfo.h"

//extern int RT_INFO;

//void Set_RT_INFO (Record rc);
class RTINFO
{
public:
  static int Is_Covered (const Record & rc, ContextInfo & ci);
  static Tuple Calc_Coverage(const Generic & ast, ContextInfo & ci);

private:
  static void IncComposite(const Generic & expr, ContextInfo & ci);
  static void Calc_Coverage_Aux(const Generic & ast, ContextInfo & ci);

  static int exprs;
  static int covered;

#ifdef PROTO
public:
  static void Set_RT_Val_Cov (Record mod, Record fn, Sequence vals);
  static int  Save_Val_Cov (wstring);
  static int  Set_Val_Cov (wstring);
  static Sequence CalculateValCov (Record mod, Record fn, ofstream&);

#define int_m    1
#define int_0    2
#define int_p    3
#define real_m   4
#define real_0   5
#define real_p   6
#define bool_f   7
#define bool_t   8
#define char_    9
#define nil_    10
#define token_  11
#define set_0   12
#define set_1   13
#define set_2   14
#define seq_0   15
#define seq_1   16
#define seq_2   17
#define map_0   18
#define map_1   19
#define map_2   20
#define quote_  21
#define record_ 22
#define tuple_  23
#define cont_   24
#define exit_   25
#define fct_    26
#define undef_  27
#endif //PROTO
};
