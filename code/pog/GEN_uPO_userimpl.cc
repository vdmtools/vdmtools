
#include "AS.h"
// ExtractParamsCtxt
// ctxt : seq of POGTP`ExprContext
// -> seq of AS`MultTypeBind
type_15AS_MultTypeBindCL vdm_GEN_uPO_ExtractParamsCtxt (const type_UL & ctxt)
{
  if (ctxt.IsEmpty())
    return type_15AS_MultTypeBindCL();

  Generic g (ctxt.Hd());
  if (g.IsSequence())
  {
    Generic h (Sequence(g).Hd());
    if (h.IsRecord() && Record(h).Is(TAG_TYPE_AS_MultTypeBind))
      return g;
  }
  return vdm_GEN_uPO_ExtractParamsCtxt(ctxt.Tl());
}
