/***
*  * WHAT
*  *    Exported functions from debug.cc
*  * FILE
*  *    $Id: evalstmt.h,v 1.5 2005/12/02 06:28:31 vdmtools Exp $
*  * COPYRIGHT
*  *    (C) Kyushu University
***/

#ifndef EVALSTMT_H
#define EVALSTMT_H

#include "semvalues.h"
#include "blkenv.h"

/*****************************************************************************/
/** stmt.vdm *****************************************************************/
/*****************************************************************************/

svt_any EvalStmt (Record);
#ifdef VDMPP
Record EvalNewStmt(Record newstmt);
Tuple MakeCopyOfInstVal(const Map &, const Map&);
bool CheckIfAbstractClass(const Record & name);
//Record EvalInvokeStmt( Record invokestmt);
#endif //VDMPP


int IsOpApply (Record);
#ifdef VDMSL
svt_any EvalOpApply (Record);
#endif //VDMSL

svt_any EvalDefStmt (Record);
svt_any EvalLetStmt (Record);
svt_any EvalLetBeSTStmt (Record);
svt_any EvalAssignStmt (Record);
svt_any EvalAssignInit (Record);

svt_any EvalSeqForLoopStmt (Record);
svt_any EvalSetForLoopStmt (Record);
svt_any EvalIndexForLoopStmt (Record); //fixed ret
svt_any EvalWhileLoopStmt (Record);


svt_any EvalNonDetStmt (Record);
#ifdef VDMSL
Tuple SDValToState (Record, Record);
Record SDtoExpr(Record sd);
int IsProperStateType (Record, Record);
Record StateToSDVal (Map, Record);
#endif //VDMSL

bool EvalPre (Generic);
bool EvalPost (Generic);

svt_any EvalReturnStmt (Record);
svt_any EvalIfStmt (Record);
svt_any EvalCasesStmt (Record); // Fixed ret
 
svt_any EvalAlwaysStmt (Record);
svt_any EvalTrapStmt (Record);
svt_any EvalRecTrapStmt (Record);
svt_any EvalExitStmt (Record);   // Fixed ret




BlkEnvCt ConstructFN (Record, Set);
Record   ConstructExplFN (Record);

Record SelPattern (Record);
Record GetExpr (Record);
Set Partition (Sequence env_ls);
Set Collapse (Sequence env_ls);
wstring NatToString (int);
wstring NewBase (wstring, int);
Record DoCarePattern (Record, wstring);


#endif

// Force emacs to use C++ mode for this file.
//// Local Variables:
//// mode: c++
//// End:

