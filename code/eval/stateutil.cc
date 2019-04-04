#include "metaiv.h"
#include "astaux.h"
#include "stackeval.h"
#include "libman.h"
#include "rt_errmsg.h"
#ifdef VDMSL
#include "evalmod.h"
#endif // VDMSL
#include "intconvquotes.h"
#include "val2x.h"
#include "tb_wstring.h"
#include "tbutils.h"
#include "tb_exceptions.h"
#include "tbdebug.h"
#include <iomanip>
#include <as2ascii.h>
#include <sstream>

/*
bool EvalState::ExistsExplFnOp(const TYPE_AS_Name & mod_name, const TYPE_AS_Name & fnop_name, wostream & wos)
{
#ifdef VDMSL
  wstring stripped;
  SigmaMO * sigmamo = AUX::RenameExtractModuleSigma(break_nm, stripped);

  if (!sigmamo) {
    wos << L"Could not find a break position for "
              << ASTAUX::ASName2String (Break_Name) << endl;
    return false;
  }

  if (sigmamo->IsSigmaIMO()) {
    wos << L"Cannot set break point in dl module" << endl;
    return false;
  }

  TYPE_CI_ContextId cid (GetCid(break_nm)); // We use this cid for the created n
ames

  Mod_Name = sigmamo->mod_name;
//  Mod_Name.SetField(2, cid);
  Mod_Name.set_cid(cid);

  Funop_Name = MkNameFromId (MkId(stripped), cid);
  Break_Name = MkNameFromRec (Mod_Name, Funop_Name);

  if (sigmamo->M_efn.DomExists (Funop_Name) ||
      sigmamo->M_eop.DomExists (Funop_Name) ||
      sigmamo->M_epf.DomExists (Funop_Name))
    return true;

#endif // VDMSL
}
*/

#ifdef VDMSL
bool EvalState::UserPushModule(const TYPE_AS_Name & mod_name, wostream & wos)
{
  if (this->mods.DomExists ( mod_name )) {
    theStackMachine().PushModule (mod_name);
    wos << L"Current module is now: " << ASTAUX::ASName2String (mod_name)<< endl;
    return true;
  }
  else if ( this->imods.DomExists ( mod_name ) ) {
    wos << L"Cannot push dl module"<< endl;
  }
  else {
    wos << L"No such module is defined: " << ASTAUX::ASName2String (mod_name)<< endl;
  }
  return false;
}

void EvalState::DumpSigmaModule( const Generic & modnm, wostream & wos )
{
  TYPE_AS_Name mod_nm ( modnm.IsNil() ? ASTAUX::GetDefaultModASName()
                                      : (const TYPE_AS_Name &)modnm );

  Record sigmamo (GetAnyModule(mod_nm));
  if (sigmamo.Length() > 0 ) {
    wos << INT2Q::h2gAS(sigmamo) << endl;
  }
  else {
    wos << L"No such module is defined: " << ASTAUX::ASName2String (mod_nm) << endl;
  }
}
#endif // VDMSL

#ifdef VDMPP
// ReCreateDObjs
bool EvalState::ReCreateDObjs(wostream & wos)
{
  if (this->d_objs.IsEmpty()) {
    return true;
  }
  wos << L"Initialising user defined objects " << endl;

  size_t len_d_objs_order = this->d_objs_order.Length();
  for (size_t idx = 1; idx <= len_d_objs_order; idx++)
  {
    const TYPE_AS_Name & nm (this->d_objs_order[idx]);
    Tuple t (this->d_objs[nm]);
    TYPE_AS_Expr stmt (t.GetRecord(2));
    SEQ<Char> init_wstring (t.GetField(3));
    wos << L"Object \"" << ASTAUX::ASName2String(nm) << L"\": "<< init_wstring.GetString() << endl;

    Tuple res (theStackMachine().EvalPrint(stmt, Sequence(L"UPS!")));

    const TYPE_STKM_EvaluationState & eval_state (res.GetRecord(1));

    if (eval_state.Is(TAG_TYPE_STKM_Success))
    {
      const TYPE_SEM_VAL & rhs_v (res.GetRecord(2));
      if ( !rhs_v.Is(TAG_TYPE_SEM_OBJ_uRef))
      {
        wos << L"The right hand side does not evaluate to an object. "
            << L"The object is not initialised " << endl;
        return false;
      }
      else {
        this->d_objs.ImpModify(nm, mk_(rhs_v,stmt,init_wstring));
      }
    }
    else {
      RTERR::Error(L"ReCreateDObjs", RTERR_INTERNAL_ERROR, Nil(), Nil(), Sequence());
      return false;
    }
  }
  return true;
}

// CreateUserDefinedObject
// stmt : AS`AssignStmt
// args : token
// wos  : ostream
void EvalState::CreateUserDefinedObject(const TYPE_AS_AssignStmt & stmt, const SEQ<Char> & args, wostream & wos)
{
  // stmt : mk_AS`AssignStmt(nm, invokestmt)
  const TYPE_AS_StateDesignator & obj_name (stmt.GetRecord(pos_AS_AssignStmt_lhs));
  const TYPE_AS_Expr & rhs (stmt.GetRecord(pos_AS_AssignStmt_rhs));

  if (this->d_objs.DomExists(obj_name)) {
    this->d_objs.RemElem(obj_name);
    bool found = false;
    int64_t index = 0;
    size_t len_d_objs_order = this->d_objs_order.Length();
    for (size_t idx = 1; (idx <= len_d_objs_order) && !found; idx++)
    {
      if (this->d_objs_order[idx] == obj_name)
      {
        found = true;
        index = idx;
      }
    }
    if(found)
    {
      this->d_objs_order.RemElem(index);
    }
  }


  AS2ASCII as2as;
  wostringstream wostr;
  as2as.Stmt2ASCII(stmt, wostr);
  SEQ<Char> id (L"Create User Object: " + wostr.str());
  Tuple res (theStackMachine().EvalPrint(rhs, id));

  const TYPE_STKM_EvaluationState & eval_state (res.GetRecord(1));
  if (eval_state.Is(TAG_TYPE_STKM_Success)) {
    const TYPE_SEM_VAL & rhs_v (res.GetRecord(2));
    if (!rhs_v.Is(TAG_TYPE_SEM_OBJ_uRef)) {
      wos << L"The right hand side does not evaluate to an object."
          << L" No object is created" << endl << flush;
    }
    else {
      this->d_objs.ImpModify(obj_name, mk_(rhs_v, rhs, args));
      this->d_objs_order.ImpAppend(obj_name);
    }
  }
  else {
    RTERR::Error(L"CreateUserDefinedObject", RTERR_INTERNAL_ERROR, Nil(), Nil(), Sequence());
  }
}

// UndefObjectRef
// semval : SEM`VAL
// ==> SEM`VAL
TYPE_SEM_VAL EvalState::UndefObjectRef( const TYPE_SEM_VAL & semval )
{
  switch(semval.GetTag()) {
    case TAG_TYPE_SEM_OBJ_uRef: {
      return TYPE_SEM_UNDEF();
    }
    case TAG_TYPE_SEM_SEQ: {
      SEQ<TYPE_SEM_VAL> s (semval.GetSequence(pos_SEM_SEQ_v));
      SEQ<TYPE_SEM_VAL> s_new;
      Generic g;
      for ( bool bb = s.First( g ); bb; bb = s.Next( g ) )
      {
        TYPE_SEM_VAL r (g);
        if( !r.Is(TAG_TYPE_SEM_OBJ_uRef) )
          s_new.ImpAppend( UndefObjectRef( g ) );
      }
      return TYPE_SEM_SEQ().Init(s_new);
    }
    case TAG_TYPE_SEM_SET: {
      SET<TYPE_SEM_VAL> s (semval.GetSet(pos_SEM_SET_v));
      SET<TYPE_SEM_VAL> s_new;
      Generic g;
      for ( bool bb = s.First( g ); bb; bb = s.Next( g ) )
      {
        TYPE_SEM_VAL r(g);
        if( !r.Is(TAG_TYPE_SEM_OBJ_uRef) )
          s_new.Insert( UndefObjectRef( g ) );
      }
      return TYPE_SEM_SET().Init(s_new);
    }
    case TAG_TYPE_SEM_MAP: {
      Map m (semval.GetMap(pos_SEM_MAP_v));
      MAP<TYPE_SEM_VAL,TYPE_SEM_VAL> m_new;
      Generic dom_g;
      Generic val_g;
      for ( bool bb = m.First( dom_g, val_g); bb; bb = m.Next( dom_g, val_g ) )
      {
        TYPE_SEM_VAL dom (dom_g);
        if( !dom.Is(TAG_TYPE_SEM_OBJ_uRef) )
        {
          Generic new_dom = UndefObjectRef( dom_g );
          Generic new_val = UndefObjectRef( val_g );
          m_new.ImpModify(new_dom, new_val);
        }
      }
      return TYPE_SEM_MAP().Init(m_new);
    }
    case TAG_TYPE_SEM_TUPLE: {
      SEQ<TYPE_SEM_VAL> s (semval.GetSequence(pos_SEM_TUPLE_v) );
      SEQ<TYPE_SEM_VAL> s_new;
      Generic g;
      for ( bool bb = s.First( g ); bb; bb = s.Next( g ) )
      {
        s_new.ImpAppend( UndefObjectRef( g ) );
      }
      return TYPE_SEM_TUPLE().Init(s_new);
    }
    case TAG_TYPE_DYNSEM_SEM_REC: {
      TYPE_DYNSEM_SEM_SemRecord srec (semval);
      Record value (srec.get_value());
      SEQ<TYPE_SEM_VAL> fields_v (value.GetFields());
      SEQ<TYPE_SEM_VAL> nfields_v;
      Generic g;
      for ( bool bb = fields_v.First( g ); bb; bb = fields_v.Next( g ) )
      {
        nfields_v.ImpAppend( UndefObjectRef( g ) );
      }
      value.SetFields(nfields_v);
      TYPE_DYNSEM_SEM_SemRecord new_srec (semval);
      new_srec.set_value(value);
      return new_srec;
    }
    default:
      return semval;
  }
}

// ClearObjectRef
// obj_ref : SEM`OBJ_Ref
// ==> ()
void EvalState::ClearObjectRef( const TYPE_SEM_OBJ_uRef & obj_ref )
{
  if (!this->obj_tab.DomExists(obj_ref)) return;

  TYPE_GLOBAL_OBJ_uDesc desc (this->obj_tab[obj_ref]);
  TYPE_SEM_OBJ sem (desc.GetRecord(pos_GLOBAL_OBJ_uDesc_sem));
  TYPE_SEM_InsStrct ins (sem.GetMap(pos_SEM_OBJ_ins)); // map AS`Name to GLOBAL`ValueMap

  SET<TYPE_AS_Name> ins_dom ( ins.Dom() ); // set of AS`Name
  Generic clnm;
  for (bool bb = ins_dom.First(clnm); bb; bb = ins_dom.Next(clnm))
  {
    MAP<TYPE_AS_Name,type_dU2P> valmap (ins[ clnm ]);  // map AS`Name to (SEM`VAL * AS`Access)
    SET<TYPE_AS_Name> valmap_dom( valmap.Dom() );
    Generic var_nm;
    for (bool cc = valmap_dom.First(var_nm); cc;
              cc = valmap_dom.Next(var_nm))
    {
      type_dU2P val (valmap[ var_nm ]);  // (SEM`VAL * AS`Access)
      TYPE_SEM_VAL semval ( val.GetField(1) );
      if ( semval.IsRecord() )
      {
        type_dU2P t_tmp;
//        t_tmp.SetField(1, UndefObjectRef(semval));
        t_tmp.SetField(1, TYPE_SEM_UNDEF());
        t_tmp.SetField(2, val.GetField(2));
        valmap.ImpModify( var_nm, t_tmp );
      }
    }
    ins.ImpModify( clnm, valmap );
  }

  sem.SetField(pos_SEM_OBJ_ins, ins);
  desc.SetField(pos_GLOBAL_OBJ_uDesc_sem, sem);
  this->obj_tab.ImpModify(obj_ref, desc);
}

// ClearObjectRefInMap
// m : GLOBAL`ValueMap
void EvalState::ClearObjectRefInMap( const TYPE_GLOBAL_ValueMap & m )
{
  Set dom_m (m.Dom());
  Generic name;
  for( bool bb = dom_m.First( name ); bb; bb = dom_m.Next( name ) )
  {
    type_dU2P val (m[name]);
    TYPE_SEM_VAL r (val.GetField( 1 ));
    Set ref_s;
    this->CreateObjectRefSet( r, ref_s );
    if( !ref_s.IsEmpty() )
    {
      Set ds (this->CreateDestroySet( ref_s ));
      if( !ds.IsEmpty() )
      {
        Generic ref;
        for( bool cc = ds.First( ref ); cc; cc = ds.Next( ref ) )
        {
          this->ClearObjectRef( ref );
        }
      }
    }
  }
}

// HasObjectRef
// semval : SEM`VAL
// obj_ref : SEM`OBJ_Ref
// ==> bool
bool EvalState::HasObjectRef( const TYPE_SEM_VAL & semval, const TYPE_SEM_OBJ_uRef & obj_ref )
{
  switch(semval.GetTag()) {
    case TAG_TYPE_SEM_OBJ_uRef: {
      TYPE_SEM_OBJ_uRef ref (semval);
      return (ref == obj_ref);
    }
    case TAG_TYPE_SEM_SEQ: {
      const SEQ<TYPE_SEM_VAL> & s (semval.GetSequence(pos_SEM_SEQ_v));
      size_t len_s = s.Length();
      bool exists = false;
      for (size_t idx = 1; (idx <= len_s) && !exists; idx++)
        exists = this->HasObjectRef( s[idx], obj_ref );
      return exists;
    }
    case TAG_TYPE_SEM_SET: {
      SET<TYPE_SEM_VAL> s (semval.GetSet(pos_SEM_SET_v));
      Generic g;
      bool exists = false;
      for (bool bb = s.First( g ); bb && !exists; bb = s.Next( g ))
        exists = this->HasObjectRef( g, obj_ref );
      return exists;
    }
    case TAG_TYPE_SEM_MAP: {
      const Map & m (semval.GetMap(pos_SEM_MAP_v));
      Set dom_m (m.Dom());
      Generic dom;
      bool exists = false;
      for (bool bb = dom_m.First(dom); bb && !exists; bb = dom_m.Next(dom))
        exists = this->HasObjectRef( dom, obj_ref ) || this->HasObjectRef( m[dom], obj_ref );
      return exists;
    }
    case TAG_TYPE_SEM_TUPLE: {
      const SEQ<TYPE_SEM_VAL> & s (semval.GetSequence(pos_SEM_TUPLE_v));
      size_t len_s = s.Length();
      bool exists = false;
      for (size_t idx = 1; (idx <= len_s) && !exists; idx++)
        exists = this->HasObjectRef( s[idx], obj_ref );
      return exists;
    }
    case TAG_TYPE_DYNSEM_SEM_REC: {
      TYPE_DYNSEM_SEM_SemRecord srec (semval);
      SEQ<TYPE_SEM_VAL> fields_v (srec.get_value().GetFields());
      size_t len_fields_v = fields_v.Length();
      bool exists = false;
      for (size_t idx = 1; (idx <= len_fields_v) && !exists; idx++)
        exists = this->HasObjectRef( fields_v[idx], obj_ref );
      return exists;
    }
    case TAG_TYPE_SEM_EXIT: {
      const Generic & v (semval.GetField(pos_SEM_EXIT_v));
      if (!v.IsNil())
      {
        return this->HasObjectRef( v, obj_ref );
      }
      return false;
    }
    case TAG_TYPE_SEM_CompExplFN: {
      const SEQ<TYPE_SEM_ExplFN> fl (semval.GetSequence(pos_SEM_CompExplFN_fl));
      const Generic & objref (semval.GetField(pos_SEM_CompExplFN_objref));
      size_t len_fl = fl.Length();
      bool exists = false;
      for (size_t idx = 1; (idx <= len_fl) && !exists; idx++)
      {
        exists = this->HasObjectRef( fl[idx], obj_ref );
      }
      if (!objref.IsNil() && !exists)
        exists = this->HasObjectRef( objref, obj_ref );
      return exists;
    }
    case TAG_TYPE_SEM_ExplFN: {
      const Generic & objref (semval.GetField(pos_SEM_ExplFN_objref));
      if (!objref.IsNil())
        return this->HasObjectRef( objref, obj_ref );
      return false;
    }
    case TAG_TYPE_SEM_ExplPOLY: {
      const Generic & objref (semval.GetField(pos_SEM_ExplPOLY_objref));
      if (!objref.IsNil())
        return this->HasObjectRef( objref, obj_ref );
      return false;
    }
    case TAG_TYPE_SEM_ExplOP: {
      const Generic & objref (semval.GetField(pos_SEM_ExplOP_objref));
      if (!objref.IsNil())
        return this->HasObjectRef( objref, obj_ref );
      return false;
    }
    case TAG_TYPE_SEM_OverOPFN: {
      const Map & overload (semval.GetMap(pos_SEM_OverOPFN_overload));
      const Generic & objref (semval.GetField(pos_SEM_OverOPFN_objref));
      Set dom_overload (overload.Dom());
      bool exists = false;
      Generic nm;
      for (bool bb = dom_overload.First(nm); bb && !exists; bb = dom_overload.Next(nm))
      {
        Tuple t (overload[nm]);
        Generic ref (t.GetField(3));
        if (!ref.IsNil())
          exists = this->HasObjectRef( ref, obj_ref );
      }
      if (!objref.IsNil() && !exists)
        exists = this->HasObjectRef( objref, obj_ref );
      return exists;
    }
    default:
      return false;
  }
}

// CreateRefCount
// semval : SEM`VAL
// ref_count : map SEM`OBJ_Ref to nat
// ==> map SEM`OBJ_Ref to nat
Map EvalState::CreateRefCount( const TYPE_SEM_VAL & semval, const Map & ref_count )
{
  switch(semval.GetTag()) {
    case TAG_TYPE_SEM_OBJ_uRef: {
      Map new_ref_count (ref_count);
      TYPE_SEM_OBJ_uRef obj_ref (semval);
      if (!new_ref_count.DomExists(obj_ref))
      {
        Int count(1);
        new_ref_count.Insert( obj_ref, count );
        return this->CreateObjectRefCount( obj_ref, new_ref_count );
      }
      else
      {
        Int count (new_ref_count[obj_ref]);
        count = count + Int(1);
        new_ref_count.ImpModify( obj_ref, count );
        return new_ref_count;
      }
    }
    case TAG_TYPE_SEM_SEQ: {
      Map new_ref_count (ref_count);
      const SEQ<TYPE_SEM_VAL> & s (semval.GetSequence(pos_SEM_SEQ_v));
      size_t len_s = s.Length();
      for (size_t idx = 1; idx <= len_s; idx++)
      {
        new_ref_count = this->CreateRefCount( s[idx], new_ref_count );
      }
      return new_ref_count;
    }
    case TAG_TYPE_SEM_SET: {
      Map new_ref_count (ref_count);
      SET<TYPE_SEM_VAL> s (semval.GetSet(pos_SEM_SET_v));
      Generic g;
      for ( bool bb = s.First( g ); bb; bb = s.Next( g ) )
      {
        new_ref_count = this->CreateRefCount( g, new_ref_count );
      }
      return new_ref_count;
    }
    case TAG_TYPE_SEM_MAP: {
      Map new_ref_count (ref_count);
      const Map & m (semval.GetMap(pos_SEM_MAP_v));
      Set dom_m (m.Dom());
      Generic dom;
      for ( bool bb = dom_m.First(dom); bb; bb = dom_m.Next(dom) )
      {
        new_ref_count = this->CreateRefCount( dom, new_ref_count );
        new_ref_count = this->CreateRefCount( m[dom], new_ref_count );
      }
      return new_ref_count;
    }
    case TAG_TYPE_SEM_TUPLE: {
      Map new_ref_count (ref_count);
      const Sequence & s (semval.GetSequence(pos_SEM_TUPLE_v) );
      size_t len_s = s.Length();
      for (size_t idx = 1; idx <= len_s; idx++)
      {
        new_ref_count = this->CreateRefCount( s[idx], new_ref_count );
      }
      return new_ref_count;
    }
    case TAG_TYPE_DYNSEM_SEM_REC: {
      Map new_ref_count (ref_count);
      TYPE_DYNSEM_SEM_SemRecord srec (semval);
      SEQ<TYPE_SEM_VAL> fields_v (srec.get_value().GetFields());
      size_t len_fields_v = fields_v.Length();
      for (size_t idx = 1; idx <= len_fields_v; idx++)
      {
        new_ref_count = this->CreateRefCount( fields_v[idx], new_ref_count );
      }
      return new_ref_count;
    }
    case TAG_TYPE_SEM_EXIT: {
      Map new_ref_count (ref_count);
      const Generic & v (semval.GetField(pos_SEM_EXIT_v));
      if (!v.IsNil())
      {
        new_ref_count = this->CreateRefCount( v, new_ref_count );
      }
      return new_ref_count;
    }
    case TAG_TYPE_SEM_CompExplFN: {
      Map new_ref_count (ref_count);
      const SEQ<TYPE_SEM_ExplFN> fl (semval.GetSequence(pos_SEM_CompExplFN_fl));
      const Generic & objref (semval.GetField(pos_SEM_CompExplFN_objref));
      size_t len_fl = fl.Length();
      for (size_t idx = 1; idx <= len_fl; idx++)
      {
        new_ref_count = this->CreateRefCount( fl[idx], new_ref_count );
      }
      if (!objref.IsNil())
        new_ref_count = this->CreateRefCount( objref, new_ref_count );
      return new_ref_count;
    }
    case TAG_TYPE_SEM_ExplFN: {
      Map new_ref_count (ref_count);
      const Generic & objref (semval.GetField(pos_SEM_ExplFN_objref));
      if (!objref.IsNil())
        new_ref_count = this->CreateRefCount( objref, new_ref_count );
      return new_ref_count;
    }
    case TAG_TYPE_SEM_ExplPOLY: {
      Map new_ref_count (ref_count);
      const Generic & objref (semval.GetField(pos_SEM_ExplPOLY_objref));
      if (!objref.IsNil())
        new_ref_count = this->CreateRefCount( objref, new_ref_count );
      return new_ref_count;
    }
    case TAG_TYPE_SEM_ExplOP: {
      Map new_ref_count (ref_count);
      const Generic & objref (semval.GetField(pos_SEM_ExplOP_objref));
      if (!objref.IsNil())
        new_ref_count = this->CreateRefCount( objref, new_ref_count );
      return new_ref_count;
    }
    case TAG_TYPE_SEM_OverOPFN: {
      Map new_ref_count (ref_count);
      const Map & overload (semval.GetMap(pos_SEM_OverOPFN_overload));
      const Generic & objref (semval.GetField(pos_SEM_OverOPFN_objref));
      Set dom_overload (overload.Dom());
      Generic nm;
      for (bool bb = dom_overload.First(nm); bb; bb = dom_overload.Next(nm))
      {
        Tuple t (overload[nm]);
        Generic ref (t.GetField(3));
        if (!ref.IsNil())
          new_ref_count = this->CreateRefCount( ref, new_ref_count );
      }
      if (!objref.IsNil())
        new_ref_count = this->CreateRefCount( objref, new_ref_count );
      return new_ref_count;
    }
    default:
      return ref_count;
  }
}

// CreateObjectRefCount
// obj_ref : SEM`OBJ_Ref
// ref_count : map SEM`OBJ_Ref to nat
// ==> map SEM`OBJ_Ref to nat
Map EvalState::CreateObjectRefCount( const TYPE_SEM_OBJ_uRef & obj_ref, const Map & ref_count )
{
  if (!this->obj_tab.DomExists(obj_ref)) return ref_count;
  const TYPE_SEM_OBJ & sem (this->obj_tab[obj_ref].GetRecord(pos_GLOBAL_OBJ_uDesc_sem));
  TYPE_SEM_InsStrct ins (sem.GetMap(pos_SEM_OBJ_ins)); // map AS`Name to GLOBAL`ValueMap

  Map new_ref_count (ref_count);
  Set dom_ins (ins.Dom());
  Generic clnm;
  for (bool bb = dom_ins.First(clnm); bb; bb = dom_ins.Next(clnm))
  {
    Map valmap (ins[clnm]);
    Set dom_valmap (valmap.Dom());
    Generic var_nm;
    for (bool cc = dom_valmap.First(var_nm); cc; cc = dom_valmap.Next(var_nm))
    {
      Tuple val (valmap[var_nm]);
      TYPE_SEM_VAL semval (val.GetRecord(1));
      if ( semval.IsRecord() )
      {
        new_ref_count = this->CreateRefCount( semval, new_ref_count );
      }
    }
  }
  return new_ref_count;
}

// CreateObjectsDirectRefCount
// ==> map SEM`OBJ_Ref to nat
Map EvalState::CreateObjectsDirectRefCount()
{
  Map ref_count; // map SEM`OBJ_Ref to nat

  // lastres
  Generic lastres = theStackMachine().GetLastRes();
  if( lastres.IsRecord() )
  {
    ref_count = this->CreateRefCount( lastres, ref_count );
  }

  // d_objs
  Set dom_d_objs (this->d_objs.Dom());
  Generic nm;
  for( bool bb = dom_d_objs.First(nm); bb; bb = dom_d_objs.Next(nm) )
  {
    Tuple tmp (this->d_objs[nm]);
    Record obj_ref (tmp.GetField(1));
    ref_count = this->CreateRefCount( obj_ref, ref_count );
  }

  Set dom_classes (this->classes.Dom());
  Generic clnm;
  for (bool cc = dom_classes.First(clnm); cc; cc = dom_classes.Next(clnm)) {
    TYPE_GLOBAL_SigmaClass sigmacl (this->classes[clnm]);

    // statics field of classes
    TYPE_GLOBAL_ValueMap statics (sigmacl.get_statics());
    Set dom_statics (statics.Dom());
    Generic statics_name;
    for( bool dd = dom_statics.First(statics_name); dd; dd = dom_statics.Next(statics_name) ) {
      Tuple val (statics[statics_name]);
      const TYPE_SEM_VAL & r (val.GetRecord(1));
      ref_count = this->CreateRefCount( r, ref_count );
    }

    // vls_init field of classes
    TYPE_GLOBAL_ValueMap vls_init (sigmacl.get_vls_uinit());
    Set dom_vls_init (vls_init.Dom());
    Generic vls_init_name;
    for( bool ee = dom_vls_init.First(vls_init_name); ee; ee = dom_vls_init.Next(vls_init_name) ) {
      Tuple val (vls_init[vls_init_name]);
      const TYPE_SEM_VAL & r (val.GetRecord(1));
      ref_count = this->CreateRefCount( r, ref_count );
    }
  }

// for thread info
  Map thm (theScheduler().GiveAllThreads());
  Set dom_thm (thm.Dom());
  Generic id;
  for (bool ff = dom_thm.First(id); ff; ff = dom_thm.Next(id)) {
    TYPE_SCHDTP_ThreadInfo ti (thm[id]);
    Generic obj (ti.GetField(pos_SCHDTP_ThreadInfo_obj));
    if (!obj.IsNil()) {
      Int count (ref_count.DomExists(obj) ? Int(ref_count[obj]) : Int(0));
      count = count + Int(1);
      ref_count.ImpModify(obj, count);
    }
  }

  return ref_count;
}

// CreateObjectsIndirectRefCount
// ==> map SEM`OBJ_Ref to nat
Map EvalState::CreateObjectsIndirectRefCount()
{
  Map ref_count; // map SEM`OBJ_Ref to nat

  // lastres
  Generic lastres = theStackMachine().GetLastRes();
  if( lastres.IsRecord() )
  {
    ref_count = this->CreateRefCount(lastres, Map());
  }

  // d_objs (map AS`Name to (SEM`OBJ_Ref * AS`Expr * seq of char))
  Set dom_d_objs (this->d_objs.Dom());
  Generic nm;
  for( bool bb = dom_d_objs.First(nm); bb; bb = dom_d_objs.Next(nm) )
  {
    Tuple tmp (this->d_objs[nm]); // (SEM`OBJ_Ref * AS`Expr * seq of char))
    const TYPE_SEM_OBJ_uRef & obj_ref (tmp.GetRecord( 1 ));

    Map rtmp (this->CreateRefCount( obj_ref, Map()));
    Set dom_rtmp (rtmp.Dom());
    Generic ref;
    for( bool cc = dom_rtmp.First(ref); cc; cc = dom_rtmp.Next(ref) )
    {
      Int count (rtmp[ref]);
      if( ref_count.DomExists(ref) )
      {
        Int new_count = count + Int(ref_count[ref]);
        ref_count.ImpModify(ref, new_count);
      }
      else
        ref_count.Insert(ref, count);
    }
  }

  Set dom_classes (this->classes.Dom());
  Generic clnm;
  for (bool dd = dom_classes.First(clnm); dd; dd = dom_classes.Next(clnm))
  {
    TYPE_GLOBAL_SigmaClass sigmacl (this->classes[clnm]);

    // statics field of classes
    TYPE_GLOBAL_ValueMap statics (sigmacl.get_statics());
    Set dom_statics (statics.Dom());
    Generic statics_name;
    for( bool ee = dom_statics.First(statics_name); ee; ee = dom_statics.Next(statics_name) )
    {
      type_dU2P val (statics[statics_name]);
      const TYPE_SEM_VAL & r (val.GetRecord( 1 ));
      Map rtmp (this->CreateRefCount( r, Map()));

      Set dom_rtmp (rtmp.Dom());
      Generic ref;
      for( bool ff = dom_rtmp.First(ref); ff; ff = dom_rtmp.Next(ref) )
      {
        Int count (rtmp[ref]);
        if( ref_count.DomExists(ref) )
        {
          Int new_count = count + Int(ref_count[ref]);
          ref_count.ImpModify(ref, new_count );
        }
        else
          ref_count.Insert(ref, count);
      }
    }

    // vls_init field of classes
    TYPE_GLOBAL_ValueMap vls_init (sigmacl.get_vls_uinit());
    Set dom_vls_init (vls_init.Dom());
    Generic vls_init_name;
    for( bool gg = dom_vls_init.First(vls_init_name); gg; gg = dom_vls_init.Next(vls_init_name) )
    {
      type_dU2P val (vls_init[vls_init_name]);
      const TYPE_SEM_VAL & r (val.GetRecord( 1 ));

      Map rtmp (this->CreateRefCount( r, Map()));

      Set dom_rtmp (rtmp.Dom());
      Generic ref;
      for( bool hh = dom_rtmp.First(ref); hh; hh = dom_rtmp.Next(ref) )
      {
        Int count (rtmp[ref]);
        if( ref_count.DomExists(ref) )
        {
          Int new_count = count + Int( ref_count[ref] );
          ref_count.ImpModify(ref, new_count );
        }
        else
          ref_count.Insert(ref, count);
      }
    }
  }

// for thread info
  Map thm (theScheduler().GiveAllThreads());
  Set dom_thm (thm.Dom());
  Generic id;
  for (bool ff = dom_thm.First(id); ff; ff = dom_thm.Next(id)) {
    TYPE_SCHDTP_ThreadInfo ti (thm[id]);
    Generic obj (ti.GetField(pos_SCHDTP_ThreadInfo_obj));
    if (!obj.IsNil()) {
      Int count (ref_count.DomExists(obj) ? Int(ref_count[obj]) : Int(0));
      count = count + Int(1);
      ref_count.ImpModify(obj, count);
    }
  }
  return ref_count;
}

// CreateObjectRefSet
// semval : SEM`VAL
// ref_s : set of SEM`OBJ_Ref
void EvalState::CreateObjectRefSet( const TYPE_SEM_VAL & semval, Set & ref_s )
{
  switch(semval.GetTag()) {
    case TAG_TYPE_SEM_OBJ_uRef: {
      if (!ref_s.InSet(semval)) ref_s.Insert(semval);
      break;
    }
    case TAG_TYPE_SEM_SEQ: {
      const SEQ<TYPE_SEM_VAL> & s (semval.GetSequence(pos_SEM_SEQ_v));
      size_t len_s = s.Length();
      for (size_t idx = 1; idx <= len_s; idx++) {
        this->CreateObjectRefSet(s[idx], ref_s );
      }
      break;
    }
    case TAG_TYPE_SEM_SET: {
      SET<TYPE_SEM_VAL> s (semval.GetSet(pos_SEM_SET_v));
      Generic g;
      for ( bool bb = s.First( g ); bb; bb = s.Next( g ) ) {
        this->CreateObjectRefSet( g, ref_s );
      }
      break;
    }
    case TAG_TYPE_SEM_MAP: {
      const Map & m (semval.GetMap(pos_SEM_MAP_v));
      Set dom_m(m.Dom());
      Generic dom;
      for ( bool bb = dom_m.First(dom); bb; bb = dom_m.Next(dom) ) {
        this->CreateObjectRefSet( dom, ref_s );
        this->CreateObjectRefSet( m[dom], ref_s );
      }
      break;
    }
    case TAG_TYPE_SEM_TUPLE: {
      const SEQ<TYPE_SEM_VAL> & s (semval.GetSequence(pos_SEM_TUPLE_v) );
      size_t len_s = s.Length();
      for (size_t idx = 1; idx <= len_s; idx++) {
        this->CreateObjectRefSet(s[idx], ref_s );
      }
      break;
    }
    case TAG_TYPE_DYNSEM_SEM_REC: {
      TYPE_DYNSEM_SEM_SemRecord srec (semval);
      SEQ<TYPE_SEM_VAL> fields_v (srec.get_value().GetFields());
      Generic g;
      for ( bool bb = fields_v.First( g ); bb; bb = fields_v.Next( g ) ) {
        this->CreateObjectRefSet( g, ref_s );
      }
      break;
    }
    case TAG_TYPE_SEM_CompExplFN: {
      const SEQ<TYPE_SEM_ExplFN> fl (semval.GetSequence(pos_SEM_CompExplFN_fl));
      const Generic & objref (semval.GetField(pos_SEM_CompExplFN_objref));
      size_t len_fl = fl.Length();
      for (size_t idx = 1; idx <= len_fl; idx++) {
        this->CreateObjectRefSet( fl[idx], ref_s );
      }
      if (!objref.IsNil())
        this->CreateObjectRefSet( objref, ref_s );
      break;
    }
    case TAG_TYPE_SEM_ExplFN: {
      const Generic & objref (semval.GetField(pos_SEM_ExplFN_objref));
      if (!objref.IsNil()) {
        this->CreateObjectRefSet( objref, ref_s );
      }
      break;
    }
    case TAG_TYPE_SEM_ExplPOLY: {
      const Generic & objref (semval.GetField(pos_SEM_ExplPOLY_objref));
      if (!objref.IsNil()) {
        this->CreateObjectRefSet( objref, ref_s );
      }
      break;
    }
    case TAG_TYPE_SEM_ExplOP: {
      const Generic & objref (semval.GetField(pos_SEM_ExplOP_objref));
      if (!objref.IsNil()) {
        this->CreateObjectRefSet( objref, ref_s );
      }
      break;
    }
    case TAG_TYPE_SEM_OverOPFN: {
      const Map & overload (semval.GetMap(pos_SEM_OverOPFN_overload));
      const Generic & objref (semval.GetField(pos_SEM_OverOPFN_objref));
      Set dom_overload (overload.Dom());
      Generic nm;
      for (bool bb = dom_overload.First(nm); bb; bb = dom_overload.Next(nm)) {
        Tuple t (overload[nm]);
        Generic ref (t.GetField(3));
        if (!ref.IsNil())
          this->CreateObjectRefSet( ref, ref_s );
      }
      if (!objref.IsNil()) {
        this->CreateObjectRefSet( objref, ref_s );
      }
      break;
    }
    default:
      break;
  }
}

// CreateDestroySet
// ref_s : set of SEM`OBJ_Ref
// ==> set of SEM`OBJ_Ref
Set EvalState::CreateDestroySet( const Set & ref_s )
{
  Map total_ref_count (CreateObjectsIndirectRefCount());
  Map ref_count;
  Set ref_s_q (ref_s);
  Generic r_g;
  for( bool bb = ref_s_q.First( r_g ); bb; bb = ref_s_q.Next( r_g ) ) {
    Map tmp (this->CreateRefCount( r_g, Map()));
    Set dom_tmp (tmp.Dom());
    Generic dom;
    for( bool cc = dom_tmp.First(dom); cc; cc = dom_tmp.Next(dom) ) {
      Int count (tmp[dom]);
      if( ref_count.DomExists(dom) ) {
        Int new_count = count + Int( ref_count[dom] );
        ref_count.ImpModify(dom, new_count );
      }
      else {
        ref_count.Insert(dom, count);
      }
    }
  }

  Set dom_ref_count (ref_count.Dom());
  Set ds;
  Generic ref;
  for( bool dd = dom_ref_count.First(ref); dd; dd = dom_ref_count.Next(ref) ) {
    Int count (ref_count[ref]);
    Int total_count;
    if( total_ref_count.DomExists(ref) ) {
      total_count = total_ref_count[ref];
    }
    else {
      total_count = Int(1);
    }
    if( count == total_count ) {
      ds.Insert(ref);
    }
  }
  return ds;
}

// CreateDanglingSet
// ==> set of SEM`OBJ_Ref
Set EvalState::CreateDanglingSet()
{
  Set dom_otab (this->obj_tab.Dom());
  Set dom_drm (this->CreateObjectsDirectRefCount().Dom());
  return dom_otab.ImpDiff(dom_drm);
}

// DestroyDanglingObjects
// ==> ()
void EvalState::DestroyDanglingObjects()
{
  Set s (this->CreateDanglingSet()); // set of SEM`OBJ_Ref
  if( !s.IsEmpty() ) {
    Generic ref;
    for (bool bb = s.First(ref); bb; bb = s.Next(ref)) {
      this->ClearObjectRef(ref);
    }
  }
}

// DestroyObjectRef
// obj_ref : SEM`OBJ_Ref
// ==> ()
void EvalState::DestroyObjectRef( const TYPE_SEM_OBJ_uRef & obj_ref )
{
  if( theStackMachine().GetLastRes() == obj_ref ) {
    theStackMachine().SetLastRes(TYPE_SEM_UNDEF());
  }
  Set ref_s;
  ref_s.Insert( obj_ref );
  Set s( this->CreateDestroySet( ref_s ) );
  Generic ref;
  for (bool bb = s.First(ref); bb; bb = s.Next(ref)) {
    this->ClearObjectRef(ref);
  }
}

// DestroyObject
bool EvalState::DestroyObject( const TYPE_AS_Name & obj_nm )
{
  bool res = false;
  if (this->d_objs.DomExists(obj_nm)) {
    Tuple t (this->d_objs[obj_nm]);
    TYPE_SEM_OBJ_uRef obj_ref (t.GetRecord(1));

    this->DestroyObjectRef( obj_ref );

    this->d_objs.RemElem(obj_nm);

    // remove the obj_nm from the d_obj_order list
    // we remove from the back so we can safely RemElem.
    for (size_t i = this->d_objs_order.Length(); i >= 1; i--) {
      if (obj_nm == this->d_objs_order.Index(i)) {
        this->d_objs_order.RemElem(i);
        // we do not break, since we want to delete all matching entries
        // (InitDObjs assumes a name in d_objs_order can be lookup in
        // d_objs.)
      }
    }
    res = true;
  }
  GC(true, false);
  return res;
}

// DestroyAllObjects
// ==> ()
void EvalState::DestroyAllObjects()
{
  Generic lastres = theStackMachine().GetLastRes();

  if ( lastres.IsRecord() && ((Record)lastres).Is(TAG_TYPE_SEM_OBJ_uRef) ) {
    theStackMachine().SetLastRes(TYPE_SEM_UNDEF());
  }
  // d_objs
  Set objs (this->d_objs.Dom());
  Generic nm;
  for( bool bb = objs.First(nm); bb; bb = objs.Next(nm) ) {
    this->DestroyObject(nm);
  }

  this->d_objs.Clear();
  this->d_objs_order.Clear();
}


// *** Exprimental ***
// Load & Save State Function

Tuple EvalState::GetState()
{
//  Set dom_obj_tab (this->obj_tab.GetMap().Dom());
  Set dom_obj_tab (this->obj_tab.Dom());
  // Check object
  Generic ref;
  for (bool bb = dom_obj_tab.First(ref); bb; bb = dom_obj_tab.Next(ref)) {
    TYPE_GLOBAL_OBJ_uDesc obj_desc (this->obj_tab[ref]);
    if( obj_desc.get_threadid() != Nil() ||
        obj_desc.GetField(pos_GLOBAL_OBJ_uDesc_DlClassInstancePtr) != Nil() ) {
      vdm_log << L"Warning: threads or dlclass is't supported." << endl << flush;
    }
    obj_desc.SetField(pos_GLOBAL_OBJ_uDesc_ref_ucount, Nil());
    obj_desc.SetField(pos_GLOBAL_OBJ_uDesc_DlClassInstancePtr, Nil());
    this->obj_tab.ImpModify( ref, obj_desc );
  }

  Map statics_m;
  Map vls_init_m;
  Set dom_classes (this->classes.Dom());
  Generic clsnm;
  for ( bool i2 = dom_classes.First(clsnm); i2; i2 = dom_classes.Next(clsnm) ) {
    TYPE_GLOBAL_SigmaClass sigmacl (this->classes[clsnm]);
    statics_m.Insert( clsnm, sigmacl.get_statics() );
    vls_init_m.Insert( clsnm, sigmacl.get_vls_uinit() );
  }

  // return state
  return mk_(this->d_objs_order,
             this->d_objs,
             this->obj_tab.GetMap(),
             theStackMachine().GetLastRes(),
             statics_m,
             vls_init_m);
}

// SetState
// objs_order :
// saved_objs :
// saved_obj_tab :
// saved_lastres : [SEM`VAL]
// saved_statics_m :
// saved_vls_init_m :
void EvalState::SetState(const SEQ<TYPE_AS_Name> & objs_order,
                         const Map & saved_objs,
                         const Map & saved_obj_tab,
                         const Generic & saved_lastres,
                         const Map & saved_statics_m,
                         const Map & saved_vls_init_m)
{
  // check contextinfo
  CheckObjectTabContext( saved_obj_tab );

  // initialize
  this->DestroyAllObjects();
  ClearStatics();

  // Restore Object and Create ObjectRef Map
  Map obj_ref_map (RestoreObjectTable( saved_obj_tab ));

  // Restore vls_init
  RestoreVlsInit( saved_vls_init_m, obj_ref_map );

  // Restore statics
  RestoreStatics( saved_statics_m, obj_ref_map );

  // Restore ObjectRef of semrec
  RestoreSemanticRecord( obj_ref_map );

  // Restore d_objs
  Set dom_saved_objs (saved_objs.Dom());
  Generic obj_name;
  for (bool bb = dom_saved_objs.First(obj_name); bb; bb = dom_saved_objs.Next(obj_name)) {
    Tuple inf (saved_objs[obj_name]);
    TYPE_SEM_OBJ_uRef old_obj_ref (inf.GetField(1));
    Int objid (old_obj_ref.GetInt(pos_SEM_OBJ_uRef_ref));

    Tuple tmp (mk_(obj_ref_map[ objid ], inf.GetField(2), inf.GetField(3)));
    this->d_objs.Insert(obj_name, tmp);
  }

  // Restore d_objs_order
  this->d_objs_order = objs_order;

  // Restore lastres
  if (saved_lastres.Is(TAG_TYPE_SEM_OBJ_uRef)) {
    Int objid (Record(saved_lastres).GetInt(pos_SEM_OBJ_uRef_ref));
    theStackMachine().SetLastRes(obj_ref_map[ objid ]);
  }
  else {
    theStackMachine().SetLastRes(saved_lastres);
  }
  // clear map
  obj_ref_map.Clear();
}

bool EvalState::CheckContext( const TYPE_AS_Name& var )
{
  TYPE_AS_Ids ids( var.get_ids() );
  wstring name (ids[ 1 ].GetString());
  TYPE_CI_ContextId cid( var.get_cid() );
  return GetCI().CheckToken( cid, name );
}

// CheckObjectTabContext
// obj_tab : map SEM`OBJ_Ref to GLOBAL`OBJ_Desc
void EvalState::CheckObjectTabContext( const Map & obj_tab )
{
  Set dom_obj_tab (obj_tab.Dom());
  Generic ref;
  for (bool bb = dom_obj_tab.First(ref); bb; bb = dom_obj_tab.Next(ref)) {
    TYPE_GLOBAL_OBJ_uDesc desc (obj_tab[ref]);
    TYPE_SEM_OBJ sem_val (desc.get_sem());
    Map ins (sem_val.get_ins());
    Set dom_ins (ins.Dom());
    Generic clnm;
    for (bool cc = dom_ins.First(clnm); cc; cc = dom_ins.Next(clnm)) {
      if( !CheckContext(clnm) ) {
        vdm_log << L"load failed ... context error." << endl;
        return;
      }
      Map valmap (ins[clnm]);
      Set dom_valmap (valmap.Dom());
      Generic var_nm;
      for (bool dd = dom_valmap.First(var_nm); dd; dd = dom_valmap.Next(var_nm)) {
        if( !CheckContext(var_nm) ) {
          vdm_log << L"load failed ... context error." << endl;
          return;
        }
      }
    }
  }
}

// RestoreObjRef
// semval : SEM`VAL
// obj_ref_map : map nat to SEM`OBJ_Ref
// ==> SEM`VAL
TYPE_SEM_VAL EvalState::RestoreObjRef(const TYPE_SEM_VAL & semval, const Map & obj_ref_map)
{
  switch(semval.GetTag()) {
    case TAG_TYPE_SEM_OBJ_uRef: {
      Int objid (semval.GetInt(pos_SEM_OBJ_uRef_ref));
      if( obj_ref_map.DomExists( objid ) ) {
        return TYPE_SEM_VAL( obj_ref_map[ objid ] );
      }
      else {
        vdm_log << L"Object not found ... Object ID: " << objid << endl;
        vdm_log << L"load failed ... internal error." << endl;
        return semval;
      }
    }
    case TAG_TYPE_SEM_SEQ: {
      SEQ<TYPE_SEM_VAL> s (((const TYPE_SEM_SEQ &)semval).get_v());
      SEQ<TYPE_SEM_VAL> s_new;
      Generic g;
      for ( bool bb = s.First( g ); bb; bb = s.Next( g ) ) {
        s_new.ImpAppend( RestoreObjRef( g, obj_ref_map ) );
      }
      return TYPE_SEM_SEQ().Init(s_new);
    }
    case TAG_TYPE_SEM_SET: {
      SET<TYPE_SEM_VAL> s (((const TYPE_SEM_SET &)semval).get_v());
      SET<TYPE_SEM_VAL> s_new;
      Generic g;
      for ( bool bb = s.First( g ); bb; bb = s.Next( g ) ) {
        s_new.Insert( RestoreObjRef( g, obj_ref_map ) );
      }
      return TYPE_SEM_SET().Init(s_new);
    }
    case TAG_TYPE_SEM_MAP: {
      Map m (((const TYPE_SEM_MAP &)semval).get_v());
      MAP<TYPE_SEM_VAL,TYPE_SEM_VAL> m_new;
      Generic dom_g;
      Generic val_g;
      for ( bool bb = m.First( dom_g, val_g); bb; bb = m.Next( dom_g, val_g ) ) {
        Generic new_dom (RestoreObjRef( dom_g, obj_ref_map ));
        Generic new_val (RestoreObjRef( val_g, obj_ref_map ));
        m_new.Insert( new_dom, new_val );
      }
      return TYPE_SEM_MAP().Init(m_new);
    }
    case TAG_TYPE_SEM_TUPLE: {
      SEQ<TYPE_SEM_VAL> s (((const TYPE_SEM_TUPLE &)semval).get_v());
      SEQ<TYPE_SEM_VAL> s_new;
      Generic g;
      for ( bool bb = s.First( g ); bb; bb = s.Next( g ) ) {
        s_new.ImpAppend( RestoreObjRef( g, obj_ref_map ) );
      }
      return TYPE_SEM_TUPLE().Init(s_new);
    }
    case TAG_TYPE_DYNSEM_SEM_REC: {
      TYPE_DYNSEM_SEM_SemRecord srec (semval);
      Record value (srec.get_value());
      SEQ<TYPE_SEM_VAL> fields_v (value.GetFields());
      SEQ<TYPE_SEM_VAL> nfields_v;
      Generic g;
      for ( bool bb = fields_v.First( g ); bb; bb = fields_v.Next( g ) ) {
        nfields_v.ImpAppend( RestoreObjRef( g, obj_ref_map ) );
      }
      value.SetFields(nfields_v);
      TYPE_DYNSEM_SEM_SemRecord new_srec (semval);
      new_srec.set_value(value);
      return new_srec;
    }
    case TAG_TYPE_SEM_CompExplFN: {
      const SEQ<TYPE_SEM_ExplFN> fl (semval.GetSequence(pos_SEM_CompExplFN_fl));
      const Generic & objref (semval.GetField(pos_SEM_CompExplFN_objref));
      SEQ<TYPE_SEM_ExplFN> new_fl;
      size_t len_fl = fl.Length();
      for (size_t idx = 1; idx <= len_fl; idx++) {
        new_fl.ImpAppend(this->RestoreObjRef( fl[idx], obj_ref_map ));
      }
      Generic new_objref (this->RestoreObjRef(objref, obj_ref_map));
      return TYPE_SEM_CompExplFN().Init(new_fl, new_objref);
    }
    case TAG_TYPE_SEM_ExplFN: {
      const Generic & objref (semval.GetField(pos_SEM_ExplFN_objref));
      TYPE_SEM_ExplFN new_semval (semval);
      new_semval.SetField(pos_SEM_ExplFN_objref, this->RestoreObjRef(objref, obj_ref_map));
      return new_semval;
    }
    case TAG_TYPE_SEM_ExplPOLY: {
      const Generic & objref (semval.GetField(pos_SEM_ExplPOLY_objref));
      TYPE_SEM_ExplFN new_semval (semval);
      new_semval.SetField(pos_SEM_ExplPOLY_objref, this->RestoreObjRef(objref, obj_ref_map));
      return new_semval;
    }
    case TAG_TYPE_SEM_ExplOP: {
      const Generic & objref (semval.GetField(pos_SEM_ExplOP_objref));
      TYPE_SEM_ExplFN new_semval (semval);
      new_semval.SetField(pos_SEM_ExplOP_objref, this->RestoreObjRef(objref, obj_ref_map));
      return new_semval;
    }
    case TAG_TYPE_SEM_OverOPFN: {
      const Map & overload (semval.GetMap(pos_SEM_OverOPFN_overload));
      const Generic & objref (semval.GetField(pos_SEM_OverOPFN_objref));
      Set dom_overload (overload.Dom());
      Map new_overload;
      Generic nm;
      for (bool bb = dom_overload.First(nm); bb; bb = dom_overload.Next(nm)) {
        Tuple t (overload[nm]);
        Generic ref (t.GetField(3));
        t.SetField(3, this->RestoreObjRef(ref, obj_ref_map) );
        new_overload.ImpModify(nm, t);
      }
      Generic new_objref (this->RestoreObjRef(objref, obj_ref_map));
      return TYPE_SEM_OverOPFN().Init(new_overload, new_objref);
    }
    default:
      return semval;
  }
}

// RestoreRecord
// valmap : map AS`Name to (SEM`VAL * AS`Access)
// obj_ref_map : map nat to SEM`OBJ_Ref
// ==> map AS`Name to (SEM`VAL * AS`Access)
MAP<TYPE_AS_Name,type_dU2P> EvalState::RestoreRecord(const MAP<TYPE_AS_Name,type_dU2P> & valmap,
                                                     const Map & obj_ref_map)
{
  Set dom_valmap (valmap.Dom());
  MAP<TYPE_AS_Name,type_dU2P> m;
  Generic name;
  for( bool bb = dom_valmap.First(name); bb; bb = dom_valmap.Next(name) ) {
    type_dU2P val (valmap[name]); // (SEM`VAL * AS`Access)
    const TYPE_SEM_VAL & semval( val.GetRecord( 1 ) );
    type_dU2P new_val;
    new_val.SetField( 1, RestoreObjRef( semval, obj_ref_map ) );
    new_val.SetField( 2, val.GetField( 2 ) );
    m.Insert( name, new_val );
  }
  return m;
}

// RestoreSemanticRecord
// obj_ref_map : map nat to SEM`OBJ_Ref
// ==> ()
void EvalState::RestoreSemanticRecord(const Map & obj_ref_map)
{
  Map tab (this->obj_tab.GetMap());
  Set dom_tab (tab.Dom());
  Generic ref;
  for (bool bb = dom_tab.First(ref); bb; bb = dom_tab.Next(ref)) {
    TYPE_GLOBAL_OBJ_uDesc desc (tab[ref]);
    TYPE_SEM_OBJ sem_val (desc.get_sem());
    TYPE_SEM_InsStrct ins (sem_val.get_ins()); // map AS`Name to GLOBAL`ValueMap
    SET<TYPE_AS_Name> dom_ins (ins.Dom());
    Generic clnm;
    for (bool cc = dom_ins.First(clnm); cc; cc = dom_ins.Next(clnm)) {
      MAP<TYPE_AS_Name,type_dU2P> valmap (ins[ clnm ]); // map AS`Name to (SEM`VAL * AS`Access)
      ins.ImpModify( clnm, RestoreRecord( valmap, obj_ref_map ) );
    }
    sem_val.set_ins(ins);

    TYPE_GLOBAL_OBJ_uDesc desc_new (desc);
    desc_new.set_sem(sem_val);
    this->obj_tab.ImpModify( ref, desc_new );
  }
}

// RestoreObjectTable
// obj_tab : map SEM`OBJ_Ref to GLOBAL`OBJ_Desc
// ==> map nat to SEM`OBJ_Ref
Map EvalState::RestoreObjectTable(const Map & obj_tab)
{
  Set dom_obj_tab (obj_tab.Dom());
  Map obj_ref_map;
  Generic ref;
  for (bool bb = dom_obj_tab.First(ref); bb; bb = dom_obj_tab.Next(ref)) {
    Int objid (Record(ref).GetInt(pos_SEM_OBJ_uRef_ref));
    TYPE_GLOBAL_OBJ_uDesc desc (obj_tab[ref]);
    TYPE_SEM_OBJ semobj (desc.get_sem());
    TYPE_SEM_OBJ_uRef new_ref (theStackMachine().MakeNewObj(semobj, Nil()));
    obj_ref_map.Insert(objid, new_ref);
  }
  return obj_ref_map;
}

void EvalState::ClearStatics()
{
  Set dom_classes( this->classes.Dom() );

  Generic clsnm;
  for ( bool bb = dom_classes.First(clsnm); bb; bb = dom_classes.Next(clsnm) ) {
    TYPE_GLOBAL_SigmaClass sigmacl (this->classes[clsnm]);

    // vls_init
    TYPE_GLOBAL_ValueMap vls_init( sigmacl.get_vls_uinit() );
    this->ClearObjectRefInMap( vls_init );

    // statics
    TYPE_GLOBAL_ValueMap statics( sigmacl.get_statics() );
    this->ClearObjectRefInMap( statics );
  }
}

// RestoreStatics
// statics_m : map AS`Name to (SEM`VAL * AS`Access)
// obj_ref_map : map nat to SEM`OBJ_Ref
// ==> ()
void EvalState::RestoreStatics(const Map & statics_m, const Map & obj_ref_map)
{
  Set dom_statics_m (statics_m.Dom());
  Generic clsnm;
  for (bool bb = dom_statics_m.First(clsnm); bb; bb = dom_statics_m.Next(clsnm)) {
    Map statics (RestoreRecord( statics_m[clsnm], obj_ref_map ));
    TYPE_GLOBAL_SigmaClass sigmacl( this->classes[ clsnm ] );
    sigmacl.set_statics((Generic)statics );
    this->classes.ImpModify( clsnm, sigmacl );
  }
}

// RestoreVlsInit
// vls_init_m : GLOBAL`ValueMap (map AS`Name to (SEM`VAL * AS`Access))
// obj_ref_map : map nat to SEM`OBJ_Ref
// ==> ()
void EvalState::RestoreVlsInit( const Map & vls_init_m, const Map & obj_ref_map)
{
  Set dom_vls_init_m (vls_init_m.Dom());
  Generic clsnm;
  for (bool bb = dom_vls_init_m.First(clsnm); bb; bb = dom_vls_init_m.Next(clsnm)) {
    Map vls_init (RestoreRecord( vls_init_m[clsnm], obj_ref_map ));
    TYPE_GLOBAL_SigmaClass sigmacl( this->classes[ clsnm ] );
    sigmacl.set_vls_uinit((Generic)vls_init);
    this->classes.ImpModify( clsnm, sigmacl );
  }
}

Tuple EvalState::GetObjState()
{
  return mk_(this->d_objs_order, this->d_objs, this->obj_tab.GetMap(), theStackMachine().GetLastRes());
}

void EvalState::DumpSigmaClass( const Generic & clsnm, wostream & wos )
{
  if ( clsnm.IsNil() ) {
    wos << this->classes << endl;
  }
  else {
    if( this->classes.DomExists(clsnm) ) {
      wos << INT2Q::h2gAS(this->classes[clsnm]) << endl;
    }
    else {
      wos << L"No such class is defined: " << ASTAUX::ASName2String (clsnm) << endl;
    }
  }
}

bool EvalState::HasObjectRef( const Map & valmap, const TYPE_SEM_OBJ_uRef& obj_ref, Generic & name )
{
  Set dom_valmap (valmap.Dom());
  Generic name_g;
  for( bool bb = dom_valmap.First(name_g); bb; bb = dom_valmap.Next(name_g) ) {
    Tuple val (valmap[name_g]);
    Record semval (val.GetField( 1 ));
    if( HasObjectRef( semval, obj_ref ) ) {
       name = name_g;
       return true;
    }
  }
  return false;
}

void EvalState::GetObjectReferenceByID( const Int & id, Set& refs )
{
  Set dom_obj_tab (this->obj_tab.GetMap().Dom());
  Generic ref;
  for( bool bb = dom_obj_tab.First(ref); bb; bb = dom_obj_tab.Next(ref) ) {
    if ( id == Record(ref).GetInt(pos_SEM_OBJ_uRef_ref) ) {
      if( !refs.InSet( ref ) ) {
        refs.Insert( ref );
      }
      return;
    }
  }
}

void EvalState::GetObjectReferenceByName( const TYPE_AS_Name & classname, Set & refs )
{
  refs.ImpUnion(GetObjRefsOfClass(classname));
}

void EvalState::DisplayObjectReference( const TYPE_SEM_OBJ_uRef& obj_ref, wostream & wos )
{
  Int id (obj_ref.GetInt(pos_SEM_OBJ_uRef_ref));
  //
  Sequence s;

  Set dom_d_objs (this->d_objs.Dom());
  Generic name_g;
  for (bool bb = dom_d_objs.First(name_g); bb; bb = dom_d_objs.Next(name_g)) {
    Tuple tmp(this->d_objs[name_g]);
    TYPE_SEM_OBJ_uRef ref( tmp.GetField( 1 ) );
    if ( id == ref.GetInt(pos_SEM_OBJ_uRef_ref) ) {
      Tuple tmp2(1);
      tmp2.SetField( 1, name_g );
      s.ImpAppend( tmp2 );
    }
  }

  Map otab (this->obj_tab.GetMap());
  Generic ref_g, desc_g;
  for( bool cc = otab.First( ref_g, desc_g ); cc; cc = otab.Next( ref_g, desc_g ) ) {
    TYPE_GLOBAL_OBJ_uDesc desc (desc_g);
    TYPE_SEM_OBJ semobj (desc.get_sem());
    TYPE_SEM_InsStrct ins (semobj.get_ins()); // map AS`Name to GLOBAL`ValueMap
    Generic clnm_g, valmap_g;
    for( bool dd = ins.First( clnm_g, valmap_g ); dd; dd = ins.Next( clnm_g, valmap_g ) ) {
      TYPE_GLOBAL_ValueMap valmap (valmap_g); // GLOBAL`ValueMap
      Generic name_g;
      if( HasObjectRef( valmap, obj_ref, name_g ) ) {
        s.ImpAppend( mk_(ref_g, name_g) );
        break;
      }
    }
  }

  Set dom_classes (this->classes.Dom());
  Generic clsnm_g;
  for (bool ee = dom_classes.First(clsnm_g); ee; ee = dom_classes.Next(clsnm_g)) {
    TYPE_GLOBAL_SigmaClass sigmacl(this->classes[clsnm_g]);
    Map statics (sigmacl.get_statics());
    Map vls_init (sigmacl.get_vls_uinit());
    Generic name_g;
    if( HasObjectRef( statics, obj_ref, name_g ) || HasObjectRef( vls_init, obj_ref, name_g ) ) {
      s.ImpAppend( mk_(clsnm_g, name_g) );
    }
  }

  if( !s.IsEmpty() ) {
    wos << L"Object " << id.GetValue() << L" ";
    const TYPE_AS_Name & clsnm (obj_ref.GetRecord(pos_SEM_OBJ_uRef_tp));
    wos << L"( instance of " << ASTAUX::ASName2String( clsnm ) << L" ) ";
    wos << L"is refered from :" << endl;

    Generic names_g;
    for( bool ff = s.First( names_g ); ff; ff = s.Next( names_g ) ) {
      wos << L"  ";
      Tuple names( names_g );
      TYPE_SEM_OBJ_uRef r( names.GetField( 1 ) );
      bool instance = false;
      if( r.Is(TAG_TYPE_SEM_OBJ_uRef) ) {
        instance = true;
        wos <<  r.GetInt(pos_SEM_OBJ_uRef_ref);
        const TYPE_AS_Name & clsnm (r.GetRecord(pos_SEM_OBJ_uRef_tp));
        wos << L"( instance of " << ASTAUX::ASName2String( clsnm ) << L" )";

      }
      else {
        wos << ASTAUX::ASName2String( r );
      }
      size_t length = names.Length();

      for( size_t jj = 2; jj <= length; jj++ ) {
        Record n( names.GetField( jj ) );
        wos << L"`" << ASTAUX::ASName2String( n );
      }
      if( !instance ) {
        if( names.Length() == 1 ) {
          wos << L" ( created by user ) ";
        }
        else {
          wos << L" ( static ) ";
        }
      }
      wos << endl << flush;
    }
  }
}

void EvalState::DisplayLiveObjects(wostream & wos)
{
  Map rc_m (this->CreateRefCountMap());
  Map m (this->CreateObjectsDirectRefCount());
  if( !m.IsEmpty() ) {
    Map m2 (this->CreateObjectsIndirectRefCount());

    wos << L"Objects Reference Count: " << m.Size() << endl << flush;
    wos << L"       OID   Count  IndCnt MVCount  Class Name" << endl << flush;
    Generic ref;
    for( bool bb = m.First(ref); bb; bb = m.Next(ref) ) {
      Int ref_count(m[ref]);
      Int ref_count2(m2[ref]);
      Int oid (Record(ref).GetInt(pos_SEM_OBJ_uRef_ref));
      Int rc (rc_m[oid]);
      const TYPE_AS_Name & clsnm (Record(ref).GetRecord(pos_SEM_OBJ_uRef_tp));

      wos << std::setw(10) << oid.ascii();
      wos << std::setw(8) << ref_count.ascii();
      wos << std::setw(8) << ref_count2.ascii();
      wos << std::setw(8) << rc.ascii();
      wos << L"  " << ASTAUX::ASName2String( clsnm );
      wos << endl << flush;
    }
  }
}

void EvalState::DisplayDanglingObjects(wostream & wos)
{
  Map rc_m (this->CreateRefCountMap());
  Set s(this->CreateDanglingSet());
  if( !s.IsEmpty() ) {
    wos << L"Dangling Objects: " << s.Card() << endl << flush;
    wos << L"       OID   Count MVCount  Class Name" << endl << flush;
    Map rc;
    Generic ref_g;
    for( bool bb = s.First( ref_g ); bb; bb = s.Next( ref_g ) ) {
      rc = this->CreateRefCount( ref_g, rc );
    }
    Generic dom_g;
    Generic reg_g;
    for( bool cc = rc.First( dom_g, reg_g ); cc; cc = rc.Next( dom_g, reg_g ) ) {
      Int ref_count( reg_g );
      Int oid (Record(dom_g).GetInt(pos_SEM_OBJ_uRef_ref));
      Int rc (rc_m[oid]);
      const TYPE_AS_Name & clsnm (Record(dom_g).GetRecord(pos_SEM_OBJ_uRef_tp));
      wos << std::setw(10) << oid.ascii();
      wos << std::setw(8) << ref_count.ascii();
      wos << std::setw(8) << rc.ascii();
      wos << L"  " << ASTAUX::ASName2String( clsnm );
      wos << endl << flush;
    }
  }
}

void EvalState::DisplayStaticObjects(wostream & wos)
{
  Set s;
  Set dom_classes (this->classes.Dom());
  Generic clsnm_g;
  for (bool bb = dom_classes.First(clsnm_g); bb; bb = dom_classes.Next(clsnm_g)) {
    TYPE_GLOBAL_SigmaClass sigmacl(this->classes[clsnm_g]);

    Generic name_g;
    Generic val_g;

    Map vls_init (sigmacl.get_vls_uinit());
    for( bool cc = vls_init.First( name_g, val_g ); cc; cc = vls_init.Next( name_g, val_g ) ) {
      Tuple val ( val_g );
      Record r ( val.GetField( 1 ) );
      if ( r.Is(TAG_TYPE_SEM_OBJ_uRef) ) {
        if( !s.InSet( r ) ) {
          s.Insert( r );
        }
      }
    }

    Map statics (sigmacl.get_statics());
    for( bool dd = statics.First( name_g, val_g ); dd; dd = statics.Next( name_g, val_g ) ) {
      Tuple val( val_g );
      Record r ( val.GetField( 1 ) );
      if ( r.Is(TAG_TYPE_SEM_OBJ_uRef) ) {
        if( !s.InSet( r ) ) {
          s.Insert( r );
        }
      }
    }
  }

  if( !s.IsEmpty() ) {
    wos << L"Static Objects: " << s.Card() << endl << flush;
    Generic g;
    for( bool ee = s.First( g ); ee; ee = s.Next( g ) )
    {
      DisplayObjectReference( g, wos );
    }
  }
}

void EvalState::DisplayUserObjects( wostream & wos )
{
  Set s;
  Set dom_d_objs (this->d_objs.Dom());
  Generic dom_g;
  for (bool bb = dom_d_objs.First(dom_g); bb; bb = dom_d_objs.Next(dom_g)) {
    Tuple tmp(this->d_objs[dom_g]);
    Record ref( tmp.GetField( 1 ) );
    if( !s.InSet( ref ) ) {
      s.Insert( ref );
    }
  }
  if( !s.IsEmpty() ) {
    wos << L"User Objects: " << s.Card() << endl << flush;
    Generic g;
    for( bool cc = s.First( g ); cc; cc = s.Next( g ) ) {
      DisplayObjectReference( g, wos );
    }
  }
}

void EvalState::DisplayObjectsDetail (const Sequence & args, wostream & wos)
{
  if( args.IsEmpty() ) {
    Tuple tmp (GetObjState());
    Sequence d_objs_order (tmp.GetField(1));
    Map d_objs (tmp.GetField(2));
    Map obj_tab (tmp.GetField(3));
    Generic lastres (tmp.GetField(4));

    wos << L"Dump objects:" << endl;
    wos << L"d_objs_order:" << endl;
    wos << INT2Q::h2gAS(d_objs_order) << endl;
    wos << L"d_objs:" << endl;
    wos << INT2Q::h2gAS(d_objs) << endl;
    wos << L"obj_tab:" << endl;
    wos << INT2Q::h2gAS(obj_tab) << endl;
    wos << L"lastres:" << endl;
    wos << INT2Q::h2gAS(lastres) << endl;
  }
  else {
    Set refs;
    size_t len_args = args.Length();
    for (size_t idx = 1; idx <= len_args; idx++) {
      Generic g (args[idx]);
      if( g.IsInt() ) {
        GetObjectReferenceByID( g, refs );
      }
      else {
        GetObjectReferenceByName( g, refs );
      }
    }
    Map obj_tab_m( this->obj_tab.GetMap() );
    Generic obj_ref_g;
    for( bool cc = refs.First( obj_ref_g ); cc; cc = refs.Next( obj_ref_g ) ) {
      Generic desc( obj_tab_m[ obj_ref_g ] );
      wos << INT2Q::h2gAS(desc) << endl;
    }
  }
}

Map EvalState::CreateRefCountMap()
{
  return this->obj_tab.GetRefCountMap();
}

Map EvalState::CreateRefMap(const Set & oid_s)
{
  Set s (oid_s);
  Map res;
  Map ot_m (this->obj_tab.GetMap());
  Generic r;
  for (bool bb = ot_m.First(r); bb; bb = ot_m.Next(r)) {
    Int oid (Record(r).GetInt(pos_SEM_OBJ_uRef_ref));
    if (s.InSet(oid)) {
      res.ImpModify(oid, r);
      s.RemElem(oid);
    }
  }
  if (!s.IsEmpty()) {
    Generic g;
    for (bool bb = s.First(g); bb; bb = s.Next(g)) {
      res.ImpModify(g, Nil());
    }
  }
  return res;
}

void EvalState::ShowAllObjects(wostream & wos)
{
  Set dom_obj_tab (this->obj_tab.Dom());
  Generic ref;
  for (bool bb = dom_obj_tab.First(ref); bb; bb = dom_obj_tab.Next(ref)) {
    VAL2X::val2stream(ref, wos, 0);
    wos << endl << flush;
  }
}

void EvalState::ShowUserObjects(wostream & wos)
{
  Set dom_d_objs (this->d_objs.Dom());
  Generic objnm_g;
  for (bool bb = dom_d_objs.First(objnm_g); bb; bb = dom_d_objs.Next(objnm_g)) {
    Tuple t (this->d_objs[objnm_g]);
    TYPE_SEM_OBJ_uRef obj_ref(t.GetRecord(1));
    if (this->obj_tab.DomExists(obj_ref)) {
      TYPE_SEM_OBJ obj (GetSemObjInTab(obj_ref));
      const TYPE_AS_Name & classtype (obj.GetRecord(pos_SEM_OBJ_tp));
      wos << L"\"" << ASTAUX::ASName2String(objnm_g) << L"\" of class \""
          << ASTAUX::ASName2String(classtype) << L"\"" << endl << flush;
    }
    else {
      wos << L"Internal Error in EvalObjects" << endl << flush;
    }
  }
}

int EvalState::ObjectTabSize()
{
  return this->obj_tab.Size();
}

#endif // VDMPP
