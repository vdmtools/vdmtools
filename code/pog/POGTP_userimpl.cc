#include "AS.h"
#include "POGTP.h"

Bool vdm_POGTP_IsBindList (const TYPE_POGTP_ExprContext &vdm_POGTP_ec)
{
  if (vdm_POGTP_ec.IsSequence()) {
    Sequence s (vdm_POGTP_ec);
    if (s.IsEmpty()) {
      return Bool(false);
    }
    bool forall = true;
    Generic g;
    for (bool bb = s.First(g); bb && forall; bb = s.Next(g)) {
      forall = (g.IsRecord() && 
                (Record(g).Is(TAG_TYPE_AS_MultSetBind) ||
                 Record(g).Is(TAG_TYPE_AS_MultSeqBind) ||
                 Record(g).Is(TAG_TYPE_AS_MultTypeBind)));
    }
    return Bool(forall);
  }
  else {
    return Bool(false);
  }
}

