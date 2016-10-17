#ifndef _TYPE_h
#define _TYPE_h

#include "REP.h"
#include "statsem.h"

#ifdef VDMPP
Bool vdm_TYPE_IsSubType(const TYPE_REP_TypeRep &,
                        const TYPE_REP_TypeRep &,
                        const Set &); 
#endif // VDMPP

#endif // _TYPE_h
