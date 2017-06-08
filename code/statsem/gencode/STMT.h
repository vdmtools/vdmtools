#ifndef __stmt_h__
#define __stmt_h__

#include "REP.h"
#include "statsem.h"

#ifdef VDMPP
Tuple vdm_STMT_CheckOperationCall (const Quote &,
                                   const SET<TYPE_AS_Name> &,
                                   const TYPE_AS_Name &,
                                   const Generic &,
                                   const SEQ<TYPE_REP_TypeRep> &);
#endif // VDMPP

#endif // __stmt_h__
