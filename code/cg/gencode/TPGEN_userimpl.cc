/***
*  * WHAT
*  *   Hand coded version of mod_tpgen version 1.5 -> 1.7
*  * FILE
*  *    $Source: /home/vdmtools/cvsroot/toolbox/code/cg/TPGEN_userimpl.cc,v $
*  * VERSION
*  *    $Revision: 1.2 $
*  * DATE
*  *    $Date: 2001/06/12 15:04:01 $
*  * STATUS
*  *    Under development
*  * REFERENCES
*  *    
*  * PROJECT
*  *    IDERES/AFRODITE
*  * AUTHOR
*  *    Lars T. Olsen +  $Author: paulm $
*  *    
*  * COPYRIGHT
*  *    (C) Kyushu University
***/

#include "vdmcg.h"
#include "AS.h"
#include "codegen_tools.h"

////////////////////////////////////////////
// OPTIMIZED INTERNAL FUNCTIONS
////////////////////////////////////////////

///////////////////////////////////
// This function overwrites the generated function: vdm_TPGEN_LOT
// This function should only be called from the generated code
// LOT : REP`TypeName ==> [REP`TypeRep]
///////////////////////////////////// 

extern vdmcg * CGi_ObjPtr;

Generic vdm_TPGEN_LOT(const TYPE_REP_TypeNameRep & tnmr) {

  Record tpr;
  tpr = h2gAS(CGi_ObjPtr->LOT(g2hAS(tnmr)));
  return tpr;
}

///////////////////////////////////
// This function overwrites the generated function: vdm_TPGEN_FindQualifiedName
// This function should only be called from the generated code
// The tpnm argument must be prefixed by current class
// FindQualifiedName : AS`Name ==> [AS`Name]
///////////////////////////////////// 
#ifdef VDMPP
extern Generic DefiningClass; // from the static semantics

Generic vdm_TPGEN_GenQualifiedTypeName(const TYPE_AS_Name & tpnm)
{
  TYPE_REP_TypeNameRep rtp;
  rtp.Init(tpnm);
  Generic g = vdm_TPGEN_LOT(rtp);
  if (DefiningClass.IsNil()) return tpnm;   // some kind of error
  else {
    TYPE_AS_Name cnm = (Generic) h2gAS(DefiningClass);
    TYPE_AS_Name qnm;
    SEQ< SEQ<Char> >  ids;
    ids.ImpAppend(cnm.get_ids().Hd());
    Sequence tpnm_ids (tpnm.get_ids());
    ids.ImpAppend(tpnm_ids[tpnm_ids.Length()]);
    qnm.Init((Generic) ids,tpnm.get_cid());

    return qnm;
  }
}
#endif // VDMPP

//////////////////////////////
// Optimization of codegenerated IsTpRecursive
//////////////////////////////
// IsTpRecursive
// nm : AS`Name
// tp : REP`TypeRep
// names : set of AS`Name
// +> bool
Bool vdm_TPGEN_IsTpRecursive(const TYPE_AS_Name & nm,
                             const TYPE_REP_TypeRep & tp,
                             const type_7AS_NameCS & names) {

  switch (tp.GetTag()) {
    case TAG_TYPE_REP_SetTypeRep:
    case TAG_TYPE_REP_SeqTypeRep: {
      return vdm_TPGEN_IsTpRecursive(nm,tp.GetField(1),names);
    }
    case TAG_TYPE_REP_EmptySetTypeRep:
    case TAG_TYPE_REP_EmptySeqTypeRep:
    case TAG_TYPE_REP_EmptyMapTypeRep: {
      return false;
    }
    case TAG_TYPE_REP_CompositeTypeRep: {
      return false;
    }
    case TAG_TYPE_REP_UnionTypeRep: {
      SET<TYPE_REP_TypeRep> ts (tp.GetField(pos_REP_UnionTypeRep_tps));
      if (ts.IsEmpty()) return false;
      bool exists = false;
      Generic g;
      for (bool bb = ts.First(g); bb && !exists; bb = ts.Next(g)) {
//        if (vdm_TPGEN_IsTpRecursive(nm,g,names).GetValue()) return true;
        exists = vdm_TPGEN_IsTpRecursive(nm,g,names).GetValue();
      };
//      return false;
      return exists;
    }
    case TAG_TYPE_REP_ProductTypeRep: {
      SEQ<TYPE_REP_TypeRep> ts (tp.GetField(1));
      if (ts.IsEmpty()) return false;
      bool exists = false;
      Generic g;
      for (bool bb = ts.First(g); bb && !exists; bb = ts.Next(g)) {
//        if (vdm_TPGEN_IsTpRecursive(nm,g,names).GetValue()) return true;
        exists = vdm_TPGEN_IsTpRecursive(nm,g,names).GetValue();
      };
//      return false;
      return exists;
    }
    case TAG_TYPE_REP_GeneralMapTypeRep:
    case TAG_TYPE_REP_InjectiveMapTypeRep: { 
      return ( vdm_TPGEN_IsTpRecursive(nm,tp.GetField(1),names).GetValue() 
            || vdm_TPGEN_IsTpRecursive(nm,tp.GetField(2),names).GetValue() );
    }
    case TAG_TYPE_REP_TypeNameRep: {
      TYPE_AS_Name n (tp.GetField(pos_REP_TypeNameRep_nm));
      if ( n == nm || names.InSet(n) ) return true;
      Generic lot_tp = vdm_TPGEN_LOT((Generic)tp);
      if (lot_tp.IsNil()) return false;
      type_7AS_NameCS res_names = names;
      return vdm_TPGEN_IsTpRecursive(nm, lot_tp, res_names.Insert(n));
    }

#ifdef VDMPP
    case TAG_TYPE_REP_ObjRefTypeRep:
#endif
#ifdef VDMSL
    case TAG_TYPE_REP_TypeVarRep:
    case TAG_TYPE_REP_OpTypeRep:
#endif
    case TAG_TYPE_REP_BasicTypeRep:
    case TAG_TYPE_REP_QuoteTypeRep:
    case TAG_TYPE_REP_PartialFnTypeRep:
    case TAG_TYPE_REP_NilTypeRep:
    case TAG_TYPE_REP_UnitTypeRep:
    case TAG_TYPE_REP_TotalFnTypeRep: {
      return false;
    }
    default:
      GetVDMCG().ReportUndefined(L"vdm_TPGEN_IsTpRecursive");
  }
  return false; // Should newer be reached
}

//////////////////////////////
// Optimization of internal function GenCppTpDecl
//////////////////////////////

type_cL vdm_TPGEN_GenCppTpDecl(const Record & tp) {

  Generic t;
  TYPE_TPGEN_Identifier id;

  switch  (tp.GetTag()) {
  case EmptySetTypeRep     : 
  case EmptySeqTypeRep     : 
  case EmptyMapTypeRep     : break;
  
  case NilTypeRep            : id = (SEQ<Char>) Sequence(L"Generic"); break;
  case UnitTypeRep           : id = (SEQ<Char>) Sequence(L"void"); break;
  case AllTypeRep            : id = (SEQ<Char>) Sequence(L"Generic"); break;

  case SetTypeRep            : id = vdm_TPGEN_GenSetType((Generic)tp); break; 
  case SeqTypeRep            : id = vdm_TPGEN_GenSeqType((Generic)tp); break;  
   
  case UnionTypeRep          : {
    Record ctp = vdm_CGAUX_CleanFlatType(tp); // Optimize Clean ???
    if (ctp.Is(UnionTypeRep))      
        id = vdm_TPGEN_GenUnionType(ctp);
    else                            
        id = vdm_TPGEN_GenCppTpDecl(ctp); 
    break;
  }
  
  case ProductTypeRep      : id = vdm_TPGEN_GenProductType((Generic) tp); break;
  case CompositeTypeRep    : id = vdm_TPGEN_GenCompositeType((Generic)tp); break;
    
  case GeneralMapTypeRep :
  case InjectiveMapTypeRep : id = vdm_TPGEN_GenMapType(tp.GetField(1),tp.GetField(2)); break;

  case TypeNameRep      : {
    t = vdm_TPGEN_LOT((Generic) tp);
#ifdef VDMPP
    if (t.IsRecord() && Record (t).Is (ObjRefTypeRep))
      return vdm_TPGEN_GenCppTpDecl (t);
#endif // VDMPP
    TYPE_AS_Name nm = tp.GetField(1);
    if (! t.IsNil() ) {
      if (vdm_TPGEN_IsTpRecursive(nm,t,(SET<TYPE_AS_Name>) Set()).GetValue()) id = (SEQ<Char>) Sequence(L"F");
      else                         id = vdm_TPGEN_GenCppTpDecl(t);
    }    
    else
      GetVDMCG().ReportUndefined(L"vdm_TPGEN_GenCppTpDecl : type name unknown");
    break;
  }  
  case BasicTypeRep      : {
    Quote t = tp.GetField(1);
    if      (t == (Quote) L"BOOLEAN")            id = (SEQ<Char>) Sequence("b");
    else if ( t == (Quote) L"NATONE" ||           
              t == (Quote) L"NAT" ||
              t == (Quote) L"INTEGER" )          id = (SEQ<Char>) Sequence("i");
    else if ( t == (Quote) L"RAT"  || 
              t == (Quote) L"REAL")              id = (SEQ<Char>) Sequence("r");
    else if ( t == (Quote) L"TOKEN")             id = (SEQ<Char>) Sequence("t");
    else if ( t == (Quote) L"CHAR")              id = (SEQ<Char>) Sequence("c");
    else 
      GetVDMCG().ReportUndefined(L"vdm_TPGEN_GenCppTpDecl : undefined base type");
    break;
  }
  
  case QuoteTypeRep   : {
    id = (SEQ<Char>) Sequence(L"Q");
    TYPE_AS_Name nm = tp.GetField(1);
    TYPE_AS_Id q_name = nm.get_ids().Hd();
    
    if (! vdm_TPGEN_cppenv.DomExists(q_name) ) {
      vdm_TPGEN_quotes.ImpAppend(vdm_TPGEN_GenQuoteId(q_name));
      vdm_TPGEN_AddNewCppTp(q_name,(Generic) Sequence());
    }
    break;
  }
  
  case PartialFnTypeRep: 
  case TotalFnTypeRep : { 
    Sequence tpl = tp.GetField(1);
    Generic t;
    
    for (int i = tpl.First(t); i ; i = tpl.Next(t)) id = vdm_TPGEN_GenCppTpDecl(t);
    id = vdm_TPGEN_GenCppTpDecl(tp.GetField(2)); 
    id = (SEQ<Char>) Sequence(L"function");
    break;
  }
  
#ifdef VDMPP
  case ObjRefTypeRep : {
    vdm_TPGEN_obj_urefs.Insert(tp.GetField(1)); 
    id = vdm_TPGEN_GenObjRefType((Generic) tp);
    break;
  }
#endif

  case OpTypeRep : {
    Sequence dtpl = tp.GetField(1);
    Generic t;
    
    for (int i = dtpl.First(t); i ; i = dtpl.Next(t)) id = vdm_TPGEN_GenCppTpDecl(t);
    id = vdm_TPGEN_GenCppTpDecl(tp.GetField(2)); 
    id = (SEQ<Char>) Sequence(L"operation"); 
    break;
  }
  case TypeVarRep :   id = (SEQ<Char>) Sequence(L"typevariable"); break;
    
  default : 
    GetVDMCG().ReportUndefined(L"vdm_TPGEN_GenCppTpDecl");    
  }
  return id;
}

