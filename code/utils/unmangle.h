// unmangle.h
// exported interface for unmangle.y
//
// $Id: unmangle.h,v 1.2 2001/06/12 15:05:01 paulm Exp $
//

#ifndef _unmangle_h_
#define _unmangle_h_

#include "metaiv.h"
#include "AS.h"

SEQ<TYPE_AS_Type> unmangle_main(const wstring & nm);

#endif //_unmangle_h_
