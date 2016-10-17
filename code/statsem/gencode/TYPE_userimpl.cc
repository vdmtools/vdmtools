#include "TYPE.h"
#include "statsem.h"
#include "intconvquotes.h"

#ifdef VDMPP
// IsSubType
// TpR1 : REP`TypeRep
// TpR2 : REP`TypeRep
// assump : set of (REP`TypeNameRep * REP`TypeNameRep)
// ==> bool
Bool vdm_TYPE_IsSubType(const TYPE_REP_TypeRep & TpR1,
                        const TYPE_REP_TypeRep & TpR2,
                        const Set & assump)
{
  return Bool(GetStatSem().IsSubType(INT2Q::g2hAS(TpR1),
                                     INT2Q::g2hAS(TpR2),
                                     INT2Q::g2hAS(assump)));
}
#endif // VDMPP
