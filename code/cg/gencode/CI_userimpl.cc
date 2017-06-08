#include "CI.h"
#include "intconvquotes.h"
#include "contextinfo.h"
#ifdef VDMPP
#include "JSSREP.h"
#endif // VDMPP

// SetTypeInfo
// cid : ContextId
// rep : REP`TypeRep
// ==> ()
void vdm_CI_SetTypeInfo (const TYPE_CI_ContextId & cid, const TYPE_REP_TypeRep & rep)
{
  GetCI().SetTypeInfo(cid, INT2Q::g2hAS(rep)); 
}

// GetFileLineColPos
// cid : ContextId
// ==> seq of char * Line * Column
type_cLii3P vdm_CI_GetFileLineColPos (const TYPE_CI_ContextId & cid)
{
  return Generic(GetCI().GetFileLineColPos(cid));
}

// SplitCid
// cid : ContextId
// -> FileId * NodeId
type_ii2P vdm_CI_SplitCid (const TYPE_CI_ContextId & cid)
{
  return ContextInfo::SplitCid(cid);
}

// ConvCid2Fid
// cid : ContextId
// -> FileId
TYPE_CI_FileId vdm_CI_ConvCid2Fid (const TYPE_CI_ContextId & cid)
{
  Tuple t (ContextInfo::SplitCid(cid));
  return t.GetInt(1);
}

// ConvCid2Nid
// cid : ContextId
// -> NodeId
TYPE_CI_NodeId vdm_CI_ConvCid2Nid (const TYPE_CI_ContextId & cid)
{
  Tuple t (ContextInfo::SplitCid(cid));
  return t.GetInt(2);
}

// CreateCid
// fid : FileId
// nid : NodeId
// ==> ContextId
TYPE_CI_ContextId vdm_CI_CreateCid (const TYPE_CI_FileId & fid, const TYPE_CI_NodeId & nid)
{
  return ContextInfo::CreateCid(fid.GetValue(), nid.GetValue());
}

#ifdef VDMPP
// SetJSSTypeInfo
// cid : ContextId
// rep : [JSSREP`TypeRep]
// ==> ()
void vdm_CI_SetJSSTypeInfo (const TYPE_CI_ContextId & cid, const Generic & rep)
{
  GetCI().SetTypeInfo(cid, INT2Q::g2hAS(rep)); 
}
#endif // VDMPP

// GetLocation
// cid : ContextId
// ==> nat * nat
type_ii2P vdm_CI_GetLocation(const TYPE_CI_ContextId & cid)
{
  Tuple gfp (GetCI().GetFilePos(cid));
  const TYPE_CI_TokenPos & astpos (gfp.GetRecord(4));
  type_ii2P tple;
  tple.set_1(astpos.GetInt(pos_CI_TokenPos_abs_uline));
  tple.set_2(astpos.GetInt(pos_CI_TokenPos_column));
  return tple;
}

// GetTypeInfo
// cid : ContextId
// ==> [REP`TypeRep]
Generic vdm_CI_GetTypeInfo(const TYPE_CI_ContextId & cid)
{
  return INT2Q::h2gAS(GetCI().GetTypeInfo(cid));
}

#ifdef VDMPP
// GetJSSTypeInfo
// cid : ContextId
// ==> [JSSREP`TypeRep]
Generic vdm_CI_GetJSSTypeInfo(const TYPE_CI_ContextId & cid)
{
  return INT2Q::h2gAS(GetCI().GetTypeInfo(cid));
}
#endif // VDMPP

// HasTypeInfo
// cid : ContextId
// ==> bool
Bool vdm_CI_HasTypeInfo(const TYPE_CI_ContextId & cid)
{
  return GetCI().HasTypeInfo(cid);
}

// IsAValidContextId
// cid : ContextId
// ==> bool
Bool vdm_CI_IsAValidContextId(const TYPE_CI_ContextId & cid)
{
  return true;  // no pendant in implementation
}
