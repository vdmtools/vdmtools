#include "STMT.h"
#include "statsem.h"
#include "intconvquotes.h"

#ifdef VDMPP
// CheckOperationCall
// i : TYPE`Ind
// classes : set of AS`Name
// nm : AS`Name
// objnm : [AS`Expr]
// etp : seq of REP`TypeRep
// ==> bool * [REP`TypeRep] * REP`TypeRep
Tuple vdm_STMT_CheckOperationCall (const Quote & i,
                                   const SET<TYPE_AS_Name> & classes,
                                   const TYPE_AS_Name & nm,
                                   const Generic & objnm,
                                   const SEQ<TYPE_REP_TypeRep> & etp)
{
  return INT2Q::h2gAS(GetStatSem().CheckOperationCall (INT2Q::g2hAS(i),
                                                       INT2Q::g2hAS(classes),
                                                       nm,
                                                       objnm,
                                                       INT2Q::g2hAS(etp)));
}
#endif // VDMPP
