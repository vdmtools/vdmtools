#include "DEF.h"
#include "statsem.h"
#include "intconvquotes.h"

#ifdef VDMSL
// extractDomRng
// g : [REP`TypeRep ]
// -> (seq of REP`TypeRep) * REP`TypeRep
Tuple vdm_DEF_extractDomRng(const Generic & g)
{
  return INT2Q::h2gAS(GetStatSem().extractDomRng(INT2Q::g2hAS(g)));
}
#endif // VDMSL

#ifdef VDMPP
// extractDomRng
// g : [REP`TypeRep | ENV`AccessType |
//      set of (ENV`AccessOpTypeRep | ENV`AccessFnTypeRep | ENV`AccessPolyTypeRep) |
//      set of REP`TypeRep]
// -> (seq of REP`TypeRep) * REP`TypeRep
Tuple vdm_DEF_extractDomRng(const Generic & g)
{
  return INT2Q::h2gAS(GetStatSem().extractDomRng(INT2Q::g2hAS(g)));
}
#endif // VDMPP

