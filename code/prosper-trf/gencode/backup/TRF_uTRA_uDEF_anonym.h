//
// THIS FILE IS AUTOMATICALLY GENERATED!!
//
// Generated at Mon 11-Oct-99 by the VDM-SLtoC++ Code Generator
// (v3.3 - Mon 22-Mar-99)
//
// Supported compilers:
// 	egcs version 1.1 on SunOS4, SunOS5, Linux, HP-UX10
// 	VC++ version 6.0 on Windows95, Windows NT
//

#ifndef _TRF_uTRA_uDEF_anonym_h
#define _TRF_uTRA_uDEF_anonym_h

#include "TRF_uTRA_uDEF_userdef.h"
#include "metaiv.h"
class type_FL;
class type_11AS_ValueDefCL;
class type_14AS_PatTypePairCL;
class type_25TRF_uTAR_uDEF_PATTYPEPAIRCL;
class type_25TRF_uTAR_uDEF_PATTYPEPAIRCLL;
class type_FLL;
class type_10AS_TypeVarCL;
#ifndef TAG_type_FL
#define TAG_type_FL (TAG_TRF_uTRA_uDEF + 1)
#endif

#ifndef TAG_type_11AS_ValueDefCL
#define TAG_type_11AS_ValueDefCL (TAG_TRF_uTRA_uDEF + 2)
#endif

#ifndef TAG_type_14AS_PatTypePairCL
#define TAG_type_14AS_PatTypePairCL (TAG_TRF_uTRA_uDEF + 3)
#endif

#ifndef TAG_type_25TRF_uTAR_uDEF_PATTYPEPAIRCL
#define TAG_type_25TRF_uTAR_uDEF_PATTYPEPAIRCL (TAG_TRF_uTRA_uDEF + 4)
#endif

#ifndef TAG_type_25TRF_uTAR_uDEF_PATTYPEPAIRCLL
#define TAG_type_25TRF_uTAR_uDEF_PATTYPEPAIRCLL (TAG_TRF_uTRA_uDEF + 5)
#endif

#ifndef TAG_type_FLL
#define TAG_type_FLL (TAG_TRF_uTRA_uDEF + 6)
#endif

#ifndef TAG_type_10AS_TypeVarCL
#define TAG_type_10AS_TypeVarCL (TAG_TRF_uTRA_uDEF + 7)
#endif


#include "AS_anonym.h"
#include "CI_anonym.h"
#include "REP_anonym.h"
#include "TRF_uTAR_uDEF_anonym.h"
#include "TRF_uTAR_uEXPR_anonym.h"
#include "TRF_uTAR_uPAT_anonym.h"
#include "TRF_uTAR_uTYPE_anonym.h"
#include "TRF_uTRA_uEXPR_anonym.h"
#include "TRF_uTRA_uPAT_anonym.h"
#include "TRF_uTRA_uTYPE_anonym.h"
#ifndef DECL_type_FL
#define DECL_type_FL 1

class type_FL : public SEQ<Generic> {
public:

  type_FL() : SEQ<Generic>() {}

  type_FL(const SEQ<Generic> &c) : SEQ<Generic>(c) {}

  type_FL(const Generic &c) : SEQ<Generic>(c) {}

  const char * GetTypeName() const { return "type_FL"; }
} ;
#endif

#ifndef DECL_type_11AS_ValueDefCL
#define DECL_type_11AS_ValueDefCL 1

class type_11AS_ValueDefCL : public SEQ<TYPE_AS_ValueDef> {
public:

  type_11AS_ValueDefCL() : SEQ<TYPE_AS_ValueDef>() {}

  type_11AS_ValueDefCL(const SEQ<TYPE_AS_ValueDef> &c) : SEQ<TYPE_AS_ValueDef>(c) {}

  type_11AS_ValueDefCL(const Generic &c) : SEQ<TYPE_AS_ValueDef>(c) {}

  const char * GetTypeName() const { return "type_11AS_ValueDefCL"; }
} ;
#endif

#ifndef DECL_type_14AS_PatTypePairCL
#define DECL_type_14AS_PatTypePairCL 1

class type_14AS_PatTypePairCL : public SEQ<TYPE_AS_PatTypePair> {
public:

  type_14AS_PatTypePairCL() : SEQ<TYPE_AS_PatTypePair>() {}

  type_14AS_PatTypePairCL(const SEQ<TYPE_AS_PatTypePair> &c) : SEQ<TYPE_AS_PatTypePair>(c) {}

  type_14AS_PatTypePairCL(const Generic &c) : SEQ<TYPE_AS_PatTypePair>(c) {}

  const char * GetTypeName() const { return "type_14AS_PatTypePairCL"; }
} ;
#endif

#ifndef DECL_type_25TRF_uTAR_uDEF_PATTYPEPAIRCL
#define DECL_type_25TRF_uTAR_uDEF_PATTYPEPAIRCL 1

class type_25TRF_uTAR_uDEF_PATTYPEPAIRCL : public SEQ<TYPE_TRF_uTAR_uDEF_PATTYPEPAIR> {
public:

  type_25TRF_uTAR_uDEF_PATTYPEPAIRCL() : SEQ<TYPE_TRF_uTAR_uDEF_PATTYPEPAIR>() {}

  type_25TRF_uTAR_uDEF_PATTYPEPAIRCL(const SEQ<TYPE_TRF_uTAR_uDEF_PATTYPEPAIR> &c) : SEQ<TYPE_TRF_uTAR_uDEF_PATTYPEPAIR>(c) {}

  type_25TRF_uTAR_uDEF_PATTYPEPAIRCL(const Generic &c) : SEQ<TYPE_TRF_uTAR_uDEF_PATTYPEPAIR>(c) {}

  const char * GetTypeName() const { return "type_25TRF_uTAR_uDEF_PATTYPEPAIRCL"; }
} ;
#endif

#ifndef DECL_type_25TRF_uTAR_uDEF_PATTYPEPAIRCLL
#define DECL_type_25TRF_uTAR_uDEF_PATTYPEPAIRCLL 1

class type_25TRF_uTAR_uDEF_PATTYPEPAIRCLL : public SEQ<type_25TRF_uTAR_uDEF_PATTYPEPAIRCL> {
public:

  type_25TRF_uTAR_uDEF_PATTYPEPAIRCLL() : SEQ<type_25TRF_uTAR_uDEF_PATTYPEPAIRCL>() {}

  type_25TRF_uTAR_uDEF_PATTYPEPAIRCLL(const SEQ<type_25TRF_uTAR_uDEF_PATTYPEPAIRCL> &c) : SEQ<type_25TRF_uTAR_uDEF_PATTYPEPAIRCL>(c) {}

  type_25TRF_uTAR_uDEF_PATTYPEPAIRCLL(const Generic &c) : SEQ<type_25TRF_uTAR_uDEF_PATTYPEPAIRCL>(c) {}

  const char * GetTypeName() const { return "type_25TRF_uTAR_uDEF_PATTYPEPAIRCLL"; }
} ;
#endif

#ifndef DECL_type_FLL
#define DECL_type_FLL 1

class type_FLL : public SEQ<type_FL> {
public:

  type_FLL() : SEQ<type_FL>() {}

  type_FLL(const SEQ<type_FL> &c) : SEQ<type_FL>(c) {}

  type_FLL(const Generic &c) : SEQ<type_FL>(c) {}

  const char * GetTypeName() const { return "type_FLL"; }
} ;
#endif

#ifndef DECL_type_10AS_TypeVarCL
#define DECL_type_10AS_TypeVarCL 1

class type_10AS_TypeVarCL : public SEQ<TYPE_AS_TypeVar> {
public:

  type_10AS_TypeVarCL() : SEQ<TYPE_AS_TypeVar>() {}

  type_10AS_TypeVarCL(const SEQ<TYPE_AS_TypeVar> &c) : SEQ<TYPE_AS_TypeVar>(c) {}

  type_10AS_TypeVarCL(const Generic &c) : SEQ<TYPE_AS_TypeVar>(c) {}

  const char * GetTypeName() const { return "type_10AS_TypeVarCL"; }
} ;
#endif


#endif

