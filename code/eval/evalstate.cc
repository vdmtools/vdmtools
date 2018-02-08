/***
*  * WHAT
*      Library management. Translation from AST to \Sigma.
*  * Id
*  *    $Id: evalstate.cc,v 1.17 2006/10/06 11:56:58 vdmtools Exp $
*  * SPECIFICATION VERSION
*  *    libman.cc: Implementation of eval_state.vdm
*  *
***/

#include "evalstate.h"
#include "stackeval.h"
#include "astaux.h"
#include "pattern.h"
#include "libman.h"
#include "compile.h"
#include "evalexpr.h"
#include "rt_errmsg.h"
#include "evaldef.h"
#include "evalclass.h"
#include "evalaux.h"
#include "asquotes.h"
#include "settings.h"
#include "tbutils.h"
#include "tools.h"
#include "intconvquotes.h"
#include "m4lib_errmsg.h"

//wcout << L"intertp: " << INT2Q::h2gAS(intertp) << endl;

#ifdef VDMPP
#include "MANGLE.h"
#include "Interface.h"
#include "CPP.h"
#ifdef VICE
#include "TIMETRACE.h"
#include "valcond.h"
#endif // VICE
#endif // VDMPP

#ifdef VDMSL
#include "evalmod.h"
#endif // VDMSL

#ifdef VDMPP
TYPE_INSTRTP_req EvalState::historyKind_req;
TYPE_INSTRTP_act EvalState::historyKind_act;
TYPE_INSTRTP_fin EvalState::historyKind_fin;
TYPE_INSTRTP_waiting EvalState::historyKind_waiting;
TYPE_INSTRTP_active EvalState::historyKind_active;
#endif // VDMPP

#ifdef VDMPP
wstring EvalState::PrintOID(const Generic & objref)
{
  if( objref.IsNil() )
    return L"nil";

  Int oid (Record(objref).GetInt(pos_SEM_OBJ_uRef_ref));
  const TYPE_AS_Name & clnm (Record(objref).GetRecord(pos_SEM_OBJ_uRef_tp));
  return oid.ascii()+L"(" + ASTAUX::ASName2String(clnm) + L")";
}
#endif // VDMPP

EvalState::EvalState()
{
  this->isTypeJudgement = false;
}

EvalState::~EvalState()
{
  dlclose();
}

void EvalState::GetInitSigma()
{
  SemRec::ResetSemRecTable();

#ifdef VDMSL
  this->mods.Clear();
  this->imods.Clear();
  this->dlfactory.clear(); //
// create top environment
  this->mods.Insert(ASTAUX::MkNameFromVoid(), DEF::EmptySigmaMO());
#endif // VDMSL

#ifdef VDMPP
  this->initclstack.Clear();
  this->classes.Clear();
  this->hchy.Clear();
  this->Init_flag = true;
  this->InitPool.Clear();
  this->inhstrct.Clear();
  this->abstracts.Clear();
  this->obj_ref_no = Int(0);
  this->clsnms.Clear();
  this->dlops.Clear();

  this->newargs.Clear(); // 0
  this->d_objs.Clear();         // 1
  this->d_objs_order.Clear();   // 2
  this->obj_tab.reset();        // 3
  this->dlfactory.clear();      // 4

  this->bindname.Clear();

  // cache
  this->initclasses.Clear();
  this->lookupOpFctPolyCache.Clear();
  this->isValueCache.Clear();
  this->objCache.Clear();
#endif // VDMPP
  this->tagCache.Clear();
  this->typeInvCache.Clear();
  this->typeDefCache.Clear();
  this->recSelCache.Clear();
#ifdef VDMSL
  this->renRecSelCache.Clear();
  this->funOpPolyCache.Clear();
  this->globalValCache.Clear();
#endif // VDMSL
  // 20070605
  this->dtc_typenm_l.Clear();

  this->isTypeJudgement = false;
}

// Init_Sigma
// ast_is_new : bool
// ==> ()
void EvalState::Init_Sigma (bool ast_is_new)
{
  if (ast_is_new) { 
    GetInitSigma();
  }
  else {
#ifdef VDMSL
    this->dtc_typenm_l.Clear();
#endif // VDMSL
#ifdef VDMPP
    // The semantic value of the user defined ojects
    // are set to mk_SEM`UNDEF(). The initialiasation
    // expression/stmt are preserved.
    // The type def of d_objs is:
    // map AS`Name to (SEM`VAL * (AS`Expr | AS`NewStmt | AS`InvokeStmt));
    //
    // This block is needed to ensure that objects d_obj_rng and
    // obj_nm are destroyed before the object table is reset().
    Set dom_d_objs (this->d_objs.Dom());
    Generic obj_nm;
    for (bool bb = dom_d_objs.First(obj_nm); bb; bb = dom_d_objs.Next(obj_nm)) {
      Tuple tmp (this->d_objs[obj_nm]);
      tmp.SetField(1, TYPE_SEM_UNDEF());
      this->d_objs.ImpModify(obj_nm, tmp);
    }
//    MAP<TYPE_AS_Name, Set> hchy_back (this->hchy);
    Map d_objs_back (this->d_objs);
    SEQ<TYPE_AS_Name> d_objs_order_back (this->d_objs_order);
//    SET<TYPE_AS_Name> InitPool_back (this->InitPool);
//    Map inhstrct_back (this->inhstrct);
//    SET<TYPE_AS_Name> abstracts_back (this->abstracts);
//    SET<TYPE_AS_Name> clsnms_back (this->clsnms);
//    MAP<TYPE_AS_Name, Set> sdep_back (this->sdep);

    GetInitSigma();

//    this->hchy = hchy_back;
    this->d_objs = d_objs_back;
    this->d_objs_order = d_objs_order_back;
//    this->InitPool = InitPool_back;
//    this->inhstrct = inhstrct_back;
//    this->abstracts = abstracts_back;
//    this->clsnms = clsnms_back;
//    this->sdep = sdep_back;

// 20090318 -->
/*
    this->initclstack.Clear();

    this->initclasses.Clear();

    // Reuse what is possible, reset the rest
//    RemoveClassValues(); // before obj_tab.reset() since values may contain classes.
//    obj_tab.reset();

//    Init_classes();

    Set orefs (this->obj_tab.Dom());
    Generic oref;
    for(bool bb = orefs.First(oref); bb; bb = orefs.Next(oref))
    {
      TYPE_GLOBAL_OBJ_uDesc desc (RemoveHist(this->obj_tab[oref]));
      this->obj_tab.ImpModify(oref, desc);
    }
    RemoveClassValues(); // before obj_tab.reset() since values may contain classes.

    Init_classes();

    this->Init_flag = true;

    this->dlfactory.DLClose();
*/
// <-20090318
#endif // VDMPP
  }
}

#ifdef VDMPP
// Get_obj_tab
// ==> GLOBAL`OBJ_tab * map AS`name to GLOBAL`SigmaClass
Tuple EvalState::Get_obj_tab()
{
  return mk_(this->obj_tab, this->classes);
}

// GetObjRefsOfClass
// clnm : AS`Name
// ==> set of SEM`OBJ_Ref
SET<TYPE_SEM_OBJ_uRef> EvalState::GetObjRefsOfClass(const TYPE_AS_Name & clnm) const
{
  Set dom_obj_tab (this->obj_tab.Dom());
  SET<TYPE_SEM_OBJ_uRef> res;
  Generic ref;
  for (bool bb = dom_obj_tab.First(ref); bb; bb = dom_obj_tab.Next(ref)) {
    if (Record(ref).GetRecord(pos_SEM_OBJ_uRef_tp) == clnm) {
      res.Insert(ref);
    }
  }
  return res;
}

// GetObjRefOfClass (not in spec)
// clnm : AS`Name
// ==> [SEM`OBJ_Ref]
Generic EvalState::GetObjRefOfClass(const TYPE_AS_Name & clnm) const
{
  Set dom_obj_tab (this->obj_tab.Dom());
  Generic ref;
  for (bool bb = dom_obj_tab.First(ref); bb; bb = dom_obj_tab.Next(ref)) {
    if (Record(ref).GetRecord(pos_SEM_OBJ_uRef_tp) == clnm) {
      return ref;
    }
  }
  return Nil();
}

// Lookup_obj_tab
// o : SEM`OBJ_Ref
// ==> GLOBAL`OBJ_Desc
TYPE_GLOBAL_OBJ_uDesc EvalState::Lookup_obj_tab(const TYPE_SEM_OBJ_uRef & o) const
{
  return this->obj_tab[o];
}

// IsInObjTab (not in spec)
// o : SEM`OBJ_Ref
// ==> bool
bool EvalState::IsInObjTab(const TYPE_SEM_OBJ_uRef & o) const
{
  return this->obj_tab.DomExists(o);
}

// Update_obj_tab (not in spec)
// o : SEM`OBJ_Ref
// desc : GLOBAL`OBJ_Desc
void EvalState::Update_obj_tab( const TYPE_SEM_OBJ_uRef & o, const TYPE_GLOBAL_OBJ_uDesc & desc)
{
  this->obj_tab.ImpModify(o, desc);
}

// LookupProcId
// o : SEM`OBJ_Ref
// ==> [AS`Name]
Generic EvalState::LookupProcId(const TYPE_SEM_OBJ_uRef& o) const
{
  if (this->obj_tab.DomExists(o)) {
    return this->obj_tab[o].GetField(pos_GLOBAL_OBJ_uDesc_procid);
  } else {
    vdm_iplog << L"LookupProcId: Unknown objref" << endl << flush;
    return Nil();
  }
}

void EvalState::SetNewArgs(const TYPE_SEM_OBJ_uRef & ref, const SEQ<TYPE_SEM_VAL> & args)
{
  SEQ<TYPE_AS_Expr> exprs;
  size_t len_args = args.Length();
  for (size_t idx = 1; idx <= len_args; idx++)
    exprs.ImpAppend(SemRec::VAL2Expr(args[idx]));

  this->newargs.ImpModify(ref.GetInt(pos_SEM_OBJ_uRef_ref), exprs);
}

SEQ<TYPE_AS_Expr> EvalState::GetNewArgs(const TYPE_SEM_OBJ_uRef & ref)
{
  return this->newargs[ref.GetInt(pos_SEM_OBJ_uRef_ref)];
}

void EvalState::SetBindName(const TYPE_SEM_OBJ_uRef & ref, const TYPE_AS_Name & nm)
{
  const Int & oid (ref.GetInt(pos_SEM_OBJ_uRef_ref));
  if (!this->bindname.DomExists(oid))
    this->bindname.ImpModify(oid, nm);
}

Generic EvalState::GetBindName(const TYPE_SEM_OBJ_uRef & ref)
{
  const Int & oid (ref.GetInt(pos_SEM_OBJ_uRef_ref));
  if (this->bindname.DomExists(oid))
    return this->bindname[oid];
  else
    return Nil();
}

#endif // VDMPP

#ifdef VDMSL
// TranslateAST
// doc_l : AS`Document
// ast_is_new : bool
// ==> ()
void EvalState::TranslateAST (const TYPE_AS_Document & doc_l, bool ast_is_new)
{
  if (ast_is_new) {

    if ((doc_l.Length () == 1) && (Record (doc_l.Hd ()).Is (TAG_TYPE_AS_Definitions))) {
      // we have a Definitions block. this block is converted into a module
      // for simplicity in the evaluation.

      const TYPE_AS_Definitions & defs (doc_l[1]);
      const TYPE_CI_ContextId & cid (defs.GetInt(pos_AS_Definitions_cid));

      // create an empty interface with all exported
      TYPE_AS_Interface new_int;
      new_int.Init(type_7AS_NameCUM(), Nil(), cid);

      // create module
      TYPE_AS_Name mod_nm (ASTAUX::GetDefaultModASName());

      TYPE_AS_Module new_mod;
      new_mod.Init (mod_nm, new_int, defs, cid);

      // insert this module
      OM_TransInsertModule (new_mod);

      // initialization needed to be able to evaluate expressions

      theStackMachine().PushEmptyEnv ();
      theStackMachine().PushTypeInst(Map());

      // initialize the global values
      InitializeGlobalVal (mod_nm);

      // remove intialization parts
      theStackMachine().PopEnvL ();
      theStackMachine().PopTypeInst();
    }
    else {
      // we have a number of modules
      // translate each of the modules and insert them into the proper maps
      size_t len_doc_l = doc_l.Length();
      for (size_t index = 1; index <= len_doc_l; index++) {
        OM_TransInsertModule (doc_l[index]);
      }

      SetSDeps(MOD::TransStaticRef(this->mods)); // 20070719
    }
  }
}
#endif //VDMSL

#ifdef VDMPP
// TranslateAST
// cs : AS`Document
// ast_is_new : bool (not used)
// ==> ()
//void EvalState::TranslateAST(const TYPE_AS_Document & cs, bool ast_is_new)
void EvalState::TranslateAST(const TYPE_AS_Document & cs_, bool)
{
  TYPE_AS_Document cs (Settings.VDM10() ? ASTAUX::ChangeDocumentToStatic(cs_) : cs_);  

  size_t len_cs = cs_.Length();

  for (size_t i = 1; i <= len_cs; i++) {
    // Skip CPP classes if any
    if(cs[i].Is(TAG_TYPE_CPP_Module)) continue;

    this->clsnms.Insert(cs[i].GetRecord(pos_AS_Class_nm));
  }

  // RegisterDLClass must be called before we return so DL libraries are (re)opened.

  // First register dlclass
  for (size_t j = 1; j <= len_cs; j++) {
    // Skip CPP classes if any
    if(cs[j].Is(TAG_TYPE_CPP_Module)) continue;

    // check 
    const TYPE_AS_Class & cl (cs[j]);

    if (! cl.GetField(pos_AS_Class_useslib).IsNil()) {
      RegisterDLClass(cl.GetRecord(pos_AS_Class_nm),
                      cl.GetRecord(pos_AS_Class_useslib),
                      cl.GetField(pos_AS_Class_defs));
    }
  }

  // insert SigmaClass
  for (size_t k = 1; k <= len_cs; k++) {
    // Skip CPP classes if any
    if(cs[k].Is(TAG_TYPE_CPP_Module)) continue;

    const TYPE_AS_Class & cl (cs[k]);

    // check default constructor
    const TYPE_AS_Name & cnm (cl.GetRecord(pos_AS_Class_nm));
    bool found = false;
    if (!cl.GetField(pos_AS_Class_defs).IsNil()) {
      Map opm (cl.GetRecord(pos_AS_Class_defs).GetMap(pos_AS_Definitions_opm));
      SET<TYPE_AS_Name> dom_opm (opm.Dom());

      Generic nm;
      for (bool bb = dom_opm.First(nm); bb && !found; bb = dom_opm.Next(nm)) {
        found = ASTAUX::GetConstr(opm[nm]) && ASTAUX::GetOpParms(opm[nm]).IsEmpty();
      }
    }

    TYPE_GLOBAL_SigmaClass thisSigmaCl (DEF::EmptySigmaClass());
    thisSigmaCl.SetField(pos_GLOBAL_SigmaClass_defaultcons, Bool(found));
    this->classes.ImpModify(cnm, thisSigmaCl);
  }

  // Translate Definitions
#ifdef VICE
  Map sys; // map AS`Name to AS`Definitions
#endif // VICE
  for (size_t l = 1; l <= len_cs; l++)
  {
    // Skip CPP classes if any
    if(cs[l].Is(TAG_TYPE_CPP_Module)) continue;

    const TYPE_AS_Class & cl (cs[l]);

#ifdef VICE
    if (cl.GetBoolValue(pos_AS_Class_sys))
      sys.Insert(cl.GetRecord(pos_AS_Class_nm), cl.GetField(pos_AS_Class_defs));
#endif // VICE

    Map tmp (DEF::ReadClasses(cl));
    this->classes.ImpOverride(tmp);
  }

  CLASS::TransHierarchy();

  SetSDeps(CLASS::TransStaticRef(this->classes)); // 20070719

  this->MergeClassAndStaticHierarchy(); // 20080520

  for (size_t m = 1; m <= len_cs; m++)
  {
    // Skip CPP classes if any
    if(cs[m].Is(TAG_TYPE_CPP_Module)) continue;

    const TYPE_AS_Class & cl (cs[m]);
    const TYPE_AS_Name & cls (cl.GetRecord(pos_AS_Class_nm));
    const SEQ<TYPE_AS_Name> & supercls (cl.GetSequence(pos_AS_Class_supercls));

    Map opm;
    if (!cl.GetField(pos_AS_Class_defs).IsNil())
      opm = cl.GetRecord(pos_AS_Class_defs).GetMap(pos_AS_Definitions_opm);

    TYPE_GLOBAL_SigmaClass classesCls (this->classes[cls]);
    const SEQ<TYPE_AS_InstAssignDef> & instvars (classesCls.GetSequence(pos_GLOBAL_SigmaClass_instvars));
    Map constrT (DEF::UpdateConstructors(cls, supercls, opm, instvars));
    classesCls.SetField(pos_GLOBAL_SigmaClass_constructor, (Generic)constrT);
    this->classes.ImpModify(cls, classesCls);
  }

  CLASS::EvalInhStrct();

  CLASS::TransSyncs(cs);

#ifdef VICE
  if (!sys.IsEmpty())
  {
    Generic sysnm;
    for (bool bb = sys.First(sysnm); bb; bb = sys.Next(sysnm))
    {
      TYPE_GLOBAL_SigmaClass sc (this->classes[sysnm]);
      sc.SetField(pos_GLOBAL_SigmaClass_sys, Bool(true));
      this->classes.ImpModify(sysnm, sc);
    }
    AddCPUAndBUSDefs(sys);
  }
#endif // VICE
}

// StaticInsts
// instdefs : seq of AS`InstanceVarDef
// ==> set of AS`Name
SET<TYPE_AS_Name> EvalState::StaticInsts(const SEQ<TYPE_AS_InstanceVarDef> & instdefs)
{
  SET<TYPE_AS_Name> res;
  size_t len = instdefs.Length();
  for (size_t i = 1; i <= len; i++)
  {
    const TYPE_AS_InstanceVarDef & ivd (instdefs[i]);
    if( ivd.Is(TAG_TYPE_AS_InstAssignDef) &&
        ivd.GetBoolValue(pos_AS_InstAssignDef_stat) )
    {
      res.Insert(ivd.GetRecord(pos_AS_InstAssignDef_ad).GetRecord(pos_AS_AssignDef_var));
    }
  }
  return res;
}

#ifdef VICE
// AddCPUAndBUSDefs
// sys_m : map AS`Name to AS`Definitions
// ==> ()
void EvalState::AddCPUAndBUSDefs( const Map & sys_m )
{
  TIMETRACE::AddCPU(Nil(), Nil(), Bool(false));

  Sequence sys_l (ASTAUX::SetToSequenceR(sys_m.Dom())); // for compare

  Generic sysnm;
//  for(bool bb = sys_m.First(sysnm); bb; bb = sys_m.Next(sysnm))
  for(bool bb = sys_l.First(sysnm); bb; bb = sys_l.Next(sysnm)) {
    TYPE_AS_Definitions defs (sys_m[sysnm]);

    Map opm (defs.get_opm());
    SEQ<TYPE_AS_InstanceVarDef> instvars (defs.get_instvars());
    Tuple tiv (DEF::TransInstVars(instvars));
    const MAP<TYPE_AS_Name,TYPE_AS_Type> & instvars_tp (tiv.GetMap(3));

    TYPE_GLOBAL_SigmaClass sigmacl (this->classes[sysnm]);
    sigmacl.SetField(pos_GLOBAL_SigmaClass_instvars_utp, instvars_tp);
    this->classes.ImpModify(sysnm, sigmacl);
    
    Set busdecls;    // set of (AS`Name * [AS`Expr])
    size_t len_instvars = instvars.Length();
    for (size_t i = 1; i <= len_instvars; i++) {
      const TYPE_AS_InstanceVarDef & ivd (instvars[i]);
      switch(ivd.GetTag()) {
        case TAG_TYPE_AS_InstAssignDef: {
          TYPE_AS_InstAssignDef iad (ivd);
          const TYPE_AS_AssignDef & assdef (iad.GetRecord(pos_AS_InstAssignDef_ad));
          const TYPE_AS_Name & var (assdef.GetRecord(pos_AS_AssignDef_var));
          const TYPE_AS_Type & tp (assdef.GetRecord(pos_AS_AssignDef_tp));
          const Generic & decl (assdef.GetField(pos_AS_AssignDef_dclinit));
          if( tp.Is(TAG_TYPE_AS_TypeName) ) {
            TYPE_AS_TypeName tptn (tp);
            if( ASTAUX::ASName2String(tptn.get_name()) == L"CPU" ) {
              theSystem().AddCPU(sysnm, var, decl);
            }
            else if( ASTAUX::ASName2String(tptn.get_name()) == L"BUS" )
              busdecls.Insert( mk_(var, decl) );
          } 
        }
        default:
          break;
      }
    }
// 20090212 -->
    theStackMachine().PushEmptyEnv();
    theStackMachine().PushBlkEnv(AUX::MkEmptyBlkEnv(sem_read_write));
    theStackMachine().PushClNmCurObj(sysnm, sysnm); // KOKO

    SEQ<TYPE_AS_ValueDef> vls_def (GetVlsDef(sysnm));
    TYPE_GLOBAL_ValueMap vls_init (CLASS::InitGV(vls_def, sysnm));

    sigmacl = this->classes[sysnm];
    sigmacl.SetField(pos_GLOBAL_SigmaClass_vls_uinit, vls_init);
    this->classes.ImpModify(sysnm, sigmacl);
// <- 20090212

    InitStaticInsts(sysnm, instvars, false);  // Note: modify class

// 20090212 -->
    theStackMachine().PopClNmCurObj();
    theStackMachine().PopEnvL();
// <- 20090212
    
    sigmacl = this->classes[sysnm];
    sigmacl.SetField(pos_GLOBAL_SigmaClass_sys, Bool(true));
    this->classes.ImpModify(sysnm, sigmacl);

    theSystem().AddVirtualBUS();

    Sequence busdecls_l (ASTAUX::SetToSequenceR(busdecls)); // for compare

    Generic t_g;
//    for(bool bbb = busdecls.First(t_g); bbb; bbb = busdecls.Next(t_g))
    for(bool bbb = busdecls_l.First(t_g); bbb; bbb = busdecls_l.Next(t_g)) {
      Tuple t (t_g);
      theSystem().AddBUS(t.GetField(1), t.GetField(2));
    }

    Generic od_g;
    if( !opm.DomExists(sysnm, od_g) ) continue;
 
    TYPE_AS_OpDef od (od_g);
    if( od.Is(TAG_TYPE_AS_ExplOpDef) ) {
      TYPE_AS_ExplOpDef eod (od_g);
      GetCI().IncTestCoverageInfo(eod.get_cid()); // 20081030
      TYPE_AS_OpBody ob (eod.get_body());
      Generic body (ob.get_body());
      if( body.Is(TAG_TYPE_AS_BlockStmt) ) {
        TYPE_AS_BlockStmt bs (body);
        GetCI().IncTestCoverageInfo(bs.get_cid()); // 20081030
        SEQ<TYPE_AS_Stmt> stmts (bs.get_stmts());
        Generic s_g;
        for(bool cc = stmts.First(s_g); cc; cc = stmts.Next(s_g)) {
          TYPE_AS_Stmt stmt (s_g);
          if( stmt.Is(TAG_TYPE_AS_CallStmt) ) {
            TYPE_AS_CallStmt cs (s_g);
            GetCI().IncTestCoverageInfo(cs.get_cid()); // 20081030
            if( !cs.get_obj().IsNil() && !cs.get_args().IsEmpty() ) {
              if( ASTAUX::ASName2String(cs.get_oprt()) == L"deploy" ) {
                TYPE_AS_Expr instnm (cs.get_args().Hd());
                if (instnm.Is(TAG_TYPE_AS_Name)) {
                  TYPE_AS_Name fullnm (ASTAUX::Combine2Names(sysnm,instnm));
                  // bool * [SEM`VAL] * *[AS`Type] * [As`Name] * [AS`Access]
                  Tuple lup (LookUpStatic(fullnm));
                  if( lup.GetBoolValue(1) ) {
                    GetCI().IncTestCoverageInfo(ASTAUX::GetCid(cs.get_obj())); // 20081030
                    GetCI().IncTestCoverageInfo(cs.get_oprt().get_cid());      // 20081030
                    GetCI().IncTestCoverageInfo(ASTAUX::GetCid(instnm));       // 20081030
                    theSystem().DeployInst(cs.get_obj(), lup.GetField(2));
                  }
                  else
                    RTERR::InitError (L"AddCPUAndBUSDefs", RTERR_DEPLOYINST, Nil(), Nil(), ASTAUX::GetCid(instnm), Sequence());
                }
                else
                  RTERR::InitError (L"AddCPUAndBUSDefs", RTERR_OBJ_REF_EXP_CALL, Nil(), Nil(), ASTAUX::GetCid(instnm), Sequence());
              }
              else if( ( ASTAUX::ASName2String(cs.get_oprt()) == L"setPriority" ) &&
                       ( cs.get_args().Length() == 2 ) ) {
                TYPE_AS_Expr opnm (cs.get_args().Hd());
                if (opnm.Is(TAG_TYPE_AS_Name)) {
                  TYPE_AS_Expr rl (cs.get_args().Index(2));
                  if (rl.Is(TAG_TYPE_AS_RealLit)) {
                    GetCI().IncTestCoverageInfo(ASTAUX::GetCid(cs.get_obj())); // 20081030
                    GetCI().IncTestCoverageInfo(cs.get_oprt().get_cid());      // 20081030
                    GetCI().IncTestCoverageInfo(ASTAUX::GetCid(opnm));         // 20081030
                    GetCI().IncTestCoverageInfo(ASTAUX::GetCid(rl));           // 20081030
                    const Int & prio (rl.GetInt(pos_AS_RealLit_val));
                    theSystem().AddPriorityEntry(cs.get_obj(), opnm, prio);
                  }
                  else if (rl.Is(TAG_TYPE_AS_NumLit)) {
                    GetCI().IncTestCoverageInfo(ASTAUX::GetCid(cs.get_obj())); // 20081030
                    GetCI().IncTestCoverageInfo(cs.get_oprt().get_cid());      // 20081030
                    GetCI().IncTestCoverageInfo(ASTAUX::GetCid(opnm));         // 20081030
                    GetCI().IncTestCoverageInfo(ASTAUX::GetCid(rl));           // 20081030
                    const Int & prio (rl.GetInt(pos_AS_NumLit_val));
                    theSystem().AddPriorityEntry(cs.get_obj(), opnm, prio);
                  }
                  else
                    RTERR::InitError (L"AddCPUAndBUSDefs", RTERR_INT_EXPECTED, Nil(), Nil(), ASTAUX::GetCid(rl), Sequence());
                }
                else
                  RTERR::InitError (L"AddCPUAndBUSDefs", RTERR_OP_NAME_EXPECTED, Nil(), Nil(), ASTAUX::GetCid(opnm), Sequence());
              }
              else
                RTERR::InitError (L"AddCPUAndBUSDefs", RTERR_ONLYDEPLOYORPRIO, Nil(), Nil(), stmt.GetInt(pos_AS_CallStmt_cid), Sequence());
            }
          }
        }
      }
    }
  }
}

// AddHistDeps
// clsnm : AS`Name
// opnm : AS`Name
// deps : set of AS`Name
// histdeps : set of AS`Name
// ==> ()
void EvalState::AddHistDeps(const TYPE_AS_Name & clsnm,
                            const TYPE_AS_Name & opnm,
                            const SET<TYPE_AS_Name> & deps,
                            const SET<TYPE_AS_Name> & histdeps)
{
  TYPE_GLOBAL_SigmaClass sigmaClass (this->classes[clsnm]);

  type_7AS_NameCUM depnms (sigmaClass.GetMap(pos_GLOBAL_SigmaClass_depnms));
  depnms.ImpModify(opnm, deps);
  sigmaClass.SetField(pos_GLOBAL_SigmaClass_depnms, depnms);

  type_7AS_NameC7AS_NameCSM depops (sigmaClass.GetMap(pos_GLOBAL_SigmaClass_depops));
  depops.ImpModify(opnm, histdeps);
  sigmaClass.SetField(pos_GLOBAL_SigmaClass_depops, depops);

  this->classes.ImpModify(clsnm, sigmaClass);
}
#endif // VICE
#endif //VDMPP

#ifdef VDMSL
// InitializeGlobalVal
// name : AS`Name
// ==> ()
void EvalState::InitializeGlobalVal (const TYPE_AS_Name & mod_name)
{
  TYPE_GLOBAL_SigmaMO sigmamo (GetModule(mod_name));
  
  if (sigmamo.GetField(pos_GLOBAL_SigmaMO_gv).IsNil ())
  {
    theStackMachine().PushModule (mod_name);
    sigmamo.SetField(pos_GLOBAL_SigmaMO_gv, MOD::InitGV (sigmamo.GetSequence(pos_GLOBAL_SigmaMO_gv_uinit), mod_name));
    this->mods.ImpModify(mod_name, sigmamo);
    theStackMachine().PopModule ();
  }
}

// InstantiationGVInit
// mod_name : AS`Name
// ==> ()
#endif //VDMSL

// InitializeGSGV
// bool : bool
// ==> ()
void EvalState::InitializeGSGV (bool ast_is_new)
{
#ifdef VDMPP
  // since obj_tab has been cleared we must run Init_classes
  // again as values may contain class references.
  Init_classes();
#endif // VDMPP

#ifdef VDMSL
  if (ast_is_new)
  {
    // the initialization of the global values of a module is a problem.
    // we do not allow mutual recursion between modules, therefore for
    // only normal modules, we have some tree of modules. initializing
    // is started at the bottom of the tree and works up.
    // for parameterized modules, the problem lies in the possibility
    // that global values can be used as parameter. if these parameter
    // values are used in global values in the parameterized module, we
    // cannot initialize these global values, because some form of mutual
    // recursion is created this way.

    // we first initialize the global values of the modules as defined
    // in the definitions part of a module.

    // initialization needed to be able to evaluate expressions
/*
    Generic mod_name;
    for(bool bb = this->mods.First(mod_name); bb;
             bb = this->mods.Next(mod_name))
    {
      theCompiler().SetClMod( mod_name );
      if (! AUX::IsRealName ( mod_name ) )
        InitializeGlobalVal ( mod_name );
    }
*/

// 20070719
    SET<TYPE_AS_Name> done;
    SET<TYPE_AS_Name> not_done (this->mods.Dom());

    if (not_done.InSet(ASTAUX::MkNameFromVoid()))
      not_done.RemElem(ASTAUX::MkNameFromVoid());

    while(!not_done.IsEmpty())
    {
      TYPE_AS_Name mod_name;
      bool exists = false;
      Generic nm;
      for (bool bb = not_done.First(nm); bb && !exists; bb = not_done.Next(nm))
      {
        SET<TYPE_AS_Name> sdep (GetAllSDeps(nm).ImpDiff(done));
        exists = sdep.IsEmpty();
        if (exists) 
          mod_name = nm;
      }

      if (exists) 
      {
        theCompiler().SetClMod( mod_name );
        if (! AUX::IsRealName ( mod_name ) )
        {
          InitializeGlobalVal ( mod_name );
          done.Insert(mod_name);
          not_done.RemElem(mod_name);
        }
      }
      else
      { // incomplete case
        TYPE_AS_Name n (not_done.GetElem());
        InitializeGlobalVal (n);
        done.Insert(n);
        not_done.RemElem(n);
      }
    }
  }

  // Done with values.
  // Initialize global state of modules.
  // This must always happen

  // Rebuild globalmap so state init can use functions, operations and
  // values.

/*
  SET<TYPE_AS_Name> mod_names (this->mods.Dom());
  Generic mod_name;
  for (bool bb = mod_names.First(mod_name); bb; bb = mod_names.Next(mod_name))
  {
    theStackMachine().PushModule(mod_name);

    TYPE_GLOBAL_SigmaMO sigmamo (this->mods[mod_name]);
    sigmamo.set_gs(theMod().InitGS(sigmamo.get_gs_uinit()));
    this->mods.ImpModify(mod_name, sigmamo);

    if (! CheckStateInvariant ()) {
      RTERR::Error (L"InitializeGSGV", RTERR_STATE_INVARIANT_ERROR, Nil(), Nil(), Sequence());
    }
    theStackMachine().PopModule();
  }
*/

// 20070719
  SET<TYPE_AS_Name> done;
  SET<TYPE_AS_Name> not_done (this->mods.Dom());
  while(!not_done.IsEmpty())
  {
    Generic mod_name;
    for (bool bb = not_done.First(mod_name); bb; bb = not_done.Next(mod_name))
    {
      SET<TYPE_AS_Name> sdep (GetAllSDeps(mod_name).ImpDiff(done));
      if (!sdep.IsEmpty()) continue;

      theStackMachine().PushModule(mod_name);

      TYPE_GLOBAL_SigmaMO sigmamo (this->mods[mod_name]);
      sigmamo.set_gs(MOD::InitGS(sigmamo.get_gs_uinit()));
      this->mods.ImpModify(mod_name, sigmamo);

      if (! CheckStateInvariant ()) {
        RTERR::Error (L"InitializeGSGV", RTERR_STATE_INVARIANT_ERROR, Nil(), Nil(), Sequence());
      }
      theStackMachine().PopModule();

      done.Insert(mod_name);
      not_done.RemElem(mod_name);
      break;
    }
  }
#endif //VDMSL
}

#ifdef VDMSL
// CheckStateInvariant
// ==> bool
bool EvalState::CheckStateInvariant ()
{
  if (!Settings.INV())
    return true;
  else {
    const TYPE_AS_Name & mod_nm (theStackMachine().CurrentModule ());
    Generic st_inv_g (GetModuleInvariant (mod_nm));
    if (st_inv_g.IsNil ())
      return true;

    // StateInv = AS`Name * seq of AS`Name * AS`Pattern * AS`Expr
    Tuple st_inv (st_inv_g);
    TYPE_AS_Name name (AUX::UniqueTagName (st_inv.GetField (1)));
    const SEQ<TYPE_AS_Name> & id_l (st_inv.GetSequence (2));
    const TYPE_AS_Pattern & pat_p (st_inv.GetRecord (3));
    const TYPE_AS_Expr & exp_e (st_inv.GetRecord (4));
    const TYPE_STKM_Pattern & pat (st_inv.GetRecord (5));
    const TYPE_STKM_SubProgram & code (st_inv.GetSequence (6));

    TYPE_GLOBAL_StateMap gs (GetModuleState(mod_nm));

    SEQ<TYPE_SEM_VAL> val_lv;
    size_t len_id_l = id_l.Length();
    for (size_t idx = 1; idx <= len_id_l; idx++)
      val_lv.ImpAppend (gs[id_l[idx]].GetRecord(pos_GLOBAL_State_val));

    TYPE_DYNSEM_SEM_SemRecord rec_val (SemRec::mk_SEM_REC(name, val_lv));

    SET<TYPE_SEM_BlkEnv> env_s (PAT::PatternMatch (pat, rec_val)); // one
    if (!env_s.IsEmpty ())
    {
      theStackMachine().PushOS();
      theStackMachine().PushEmptyEnv ();
      theStackMachine().PushBlkEnv (env_s.GetElem ());

      Tuple res (theStackMachine().EvalAuxProgram(code, SEQ<Char>(L"Invariant Check for ")
                                                         .ImpConc(AUX::SingleNameToString(name)), false));
      theStackMachine().PopEnvL ();
      theStackMachine().PopOS();

      const TYPE_STKM_EvaluationState & eval_state (res.GetRecord(1));
      if (eval_state.Is(TAG_TYPE_STKM_Success))
      {
        const TYPE_SEM_VAL & body_v (res.GetField(2));
        if (body_v.Is(TAG_TYPE_SEM_BOOL))
          return body_v.GetBoolValue(pos_SEM_BOOL_v);
        else
          RTERR::Error (L"CheckStateInvariant", RTERR_BOOL_EXPECTED, body_v, Nil(), Sequence());
      }
//      else
//        RTERR::Error(L"CheckStateInvariant", RTERR_INTERNAL_ERROR);
    }
    else
      RTERR::Error (L"CheckStateInvariant", RTERR_EMPTY_ENV_S, Nil(), Nil(), Sequence());
  }
  return false; //to avoid warnings from GCC
}

// GetModuleInvariant
// mod_name : AS`Name
// ==> [GLOBAL`StateInv]
Generic EvalState::GetModuleInvariant (const TYPE_AS_Name & mod_name)
{
  const TYPE_GLOBAL_SigmaMO & sigmamo (GetModule(mod_name));
  return sigmamo.GetField(pos_GLOBAL_SigmaMO_stateinv);
}

// ExtractTagNameCached (not in spec)
// name : AS`Name
// ==> [AS`Name] * bool
Tuple EvalState::ExtractTagNameCached(const TYPE_AS_Name & name)
{
  TYPE_AS_Name key (name);

  if (!AUX::IsRealName(name))
    key = ASTAUX::Combine2Names(theStackMachine().CurrentModule(), name);

  if (!this->tagCache.DomExists(key))
  {
    this->tagCache.ImpModify(key, AUX::ExtractTagName(name, Set()));
  }
  return this->tagCache[key];
}
#endif //VDMSL

#ifdef VDMPP
// CheckInstanceInvariant
// ==> bool
bool EvalState::CheckInstanceInvariant()
{
  if (!Settings.INV())
    return true;

  const TYPE_SEM_InsStrct & inststrct (theStackMachine().GetCurObj().GetMap(pos_SEM_OBJ_ins));

  SET<TYPE_AS_Name> dom_inststrct (inststrct.Dom());

// 20090306 -->
  // do not execute check for some classes
  dom_inststrct.ImpDiff(Settings.GetNoCheck());

  bool invok = true;
// <-- 20090306
  if (!dom_inststrct.IsEmpty())
  {
    theStackMachine().PushOS();
    TYPE_SEM_BlkEnv emptyBlkEnv (AUX::MkEmptyBlkEnv(sem_read_only));

    Generic cl;
//    for (bool bb = dom_inststrct.First(cl); bb; bb = dom_inststrct.Next(cl))
    for (bool bb = dom_inststrct.First(cl); bb && invok; bb = dom_inststrct.Next(cl))
    {
      TYPE_STKM_SubProgram inv_cl (GetInstInv(cl));

// wcout << INT2Q::h2gAS(inv_cl_l) << endl;

      //if (!inv_cl_l.IsEmpty())
      if (!inv_cl.IsEmpty())
      {
        theStackMachine().PushBlkEnv(emptyBlkEnv);
        theStackMachine().PushClNmCurObj(cl, cl);

// 20130531 -->
        //Tuple res (theStackMachine().EvalAuxProgram(inv_cl_l.Hd(), SEQ<Char>(L"Check of Instance Invariant"), false));
        SEQ<Char> id (L"Check of Instance Invariant: " + ASTAUX::ASName2String(cl));
        Tuple res (theStackMachine().EvalAuxProgram(inv_cl, id, false));
// <-- 20130531

        //theStackMachine().PopClNmCurObj();
        //theStackMachine().PopBlkEnv();
// 20100112 -->
        const TYPE_STKM_EvaluationState & eval_state (res.GetRecord(1));
        if (eval_state.Is(TAG_TYPE_STKM_Success))
        {
          const TYPE_SEM_VAL & expr_v (res.GetRecord(2));  // SEM`VAL
          if (expr_v.Is(TAG_TYPE_SEM_BOOL)) {
            invok = expr_v.GetBoolValue(pos_SEM_BOOL_v);
          }
// 20150106 -->
          else if (theStackMachine().IsRuntimeErrorException(expr_v)) {
            theStackMachine().PushRuntimeErrorVal();
            return false;
          }
// <-- 20150106
          else {
            RTERR::Error(L"CheckInstanceInvariant", RTERR_BOOL_EXPECTED, expr_v, Nil(), Sequence());
            return false;
          }
        }
        theStackMachine().PopClNmCurObj();
        theStackMachine().PopBlkEnv();
//        else
//          RTERR::Error(L"CheckInstanceInvariant", RTERR_INTERNAL_ERROR, Nil(), Nil(), Sequence());
// <-- 20100112
      }
// <-- 20090305
    } // end of loop
    theStackMachine().PopOS();
  }
//  return true;
  return invok;
}

// GetCachedOBJ (not in spec)
// clsnm : AS`Name
// ==> SEM`OBJ
TYPE_SEM_OBJ EvalState::GetCachedOBJ(const TYPE_AS_Name & clsnm)
{
  if (!this->objCache.DomExists(clsnm))
  {
    this->objCache.ImpModify(clsnm, TYPE_SEM_OBJ().Init(clsnm,
                                                        GetInstInitVal(clsnm),
                                                        MAP<TYPE_AS_Name,TYPE_SEM_History>()));
  }
  return this->objCache[clsnm];
}

// ExtractTagNameCached (not in spec)
// name : AS`Name
// ==> [AS`Name] * bool
Tuple EvalState::ExtractTagNameCached(const TYPE_AS_Name & name)
{
  TYPE_AS_Name key (name);
  if (name.GetSequence(pos_AS_Name_ids).Length() == 1)
  {
    if (!theStackMachine().HasCurCl())
    {
      theStackMachine().SetCid(name.GetInt(pos_AS_Name_cid));
      RTERR::Error(L"ExtractTagName", RTERR_TAG_UNKNOWN, Nil(), Nil(), Sequence());
      return mk_(Nil(), Bool(false));
    }
    else
    {
      key = AUX::ConstructDoubleName(theStackMachine().GetCurCl(), name);
    }
  }
  if (!this->tagCache.DomExists(key))
  {
    this->tagCache.ImpModify(key, AUX::ExtractTagName(name, Set()));
  }
  return this->tagCache[key];
}

// PushInitPool
// nm : AS`Name
void EvalState::PushInitPool(const TYPE_AS_Name & nm)
{
  this->InitPool.Insert(nm);
}

// PopInitPool
// nm : AS`Name
void EvalState::PopInitPool(const TYPE_AS_Name & nm)
{
  if (this->InitPool.InSet(nm)) {
    this->InitPool.RemElem(nm);
  }
  else {
    RTERR::Error(L"PopInitPool", RTERR_INTERNAL_ERROR, Nil(), Nil(), Sequence());
  }
}

// Init_classes (aka InitSigma)
// ==> ()
void EvalState::Init_classes()
{
  bool SavedSetting_DTC(Settings.DTC());
  bool SavedSetting_PRE(Settings.PreCheck());
  bool SavedSetting_POST(Settings.PostCheck());
  bool SavedSetting_INV(Settings.INV());
  bool SavedSetting_Assertion(Settings.Assertion());
  bool SavedSetting_RTEE(Settings.RTErrException());
  bool SavedSetting_UsesOldName(theStackMachine().UsesOldName());

  Settings.DtcOff();
  Settings.PreOff();
  Settings.PostOff();
  Settings.InvOff();
  Settings.AssertOff();
  Settings.RTErrExceptionOff();
  theStackMachine().SetUsesOldName(false);

  Init_flag = true;

  Set classes_s (this->classes.Dom());

  // clear the isinit flag.
  Generic nm;
  for (bool bb = classes_s.First(nm); bb; bb = classes_s.Next(nm)) {
    SetClassInit(nm, Bool(false));
  }

  if (!classes_s.IsEmpty()) {
    // for Status Report
    ToolMediator::Interf()->vdm_InitMeter(SEQ<Char>(L"Initializing Specification"), SEQ<Char>());
    ToolMediator::Interf()->vdm_SetMeterTotal(classes_s.Card());

    InitTheClasses( classes_s );

    // for Status Report
    ToolMediator::Interf()->vdm_DestroyMeter();
  }

  Init_flag = false;

  if(SavedSetting_DTC) Settings.DtcOn();
  if(SavedSetting_PRE) Settings.PreOn();
  if(SavedSetting_POST) Settings.PostOn();
  if(SavedSetting_INV) Settings.InvOn();
  if(SavedSetting_Assertion) Settings.AssertOn();
  if(SavedSetting_RTEE) Settings.RTErrExceptionOn();
  theStackMachine().SetUsesOldName(SavedSetting_UsesOldName);
}

//#ifdef VICE
// StillInit
// ==> bool
bool EvalState::StillInit()
{
  return Init_flag;
}
//#endif // VICE

// RemoveClassValues
// ==> ()
void EvalState::RemoveClassValues()
{
  Set dom_classes(this->classes.Dom());
  Generic nm;
  for (bool bb = dom_classes.First(nm); bb; bb = dom_classes.Next(nm))
  {
    // remove object ref
    TYPE_GLOBAL_SigmaClass sigmacl (this->classes[nm]);
    sigmacl.SetField(pos_GLOBAL_SigmaClass_vls_uinit, TYPE_GLOBAL_ValueMap());
    sigmacl.SetField(pos_GLOBAL_SigmaClass_statics, TYPE_GLOBAL_ValueMap());
    this->classes.ImpModify(nm, sigmacl);
  }
}

// Note, operation InitTheClasses is not implemented using
// recursion. That is why, the function in the implementation only
// takes one parameter.

// InitTheClasses
// cl : Set of AS`Name
// ==> ()
void EvalState::InitTheClasses(const SET<TYPE_AS_Name> & cl)
{
  Set not_done (cl);

  while (!not_done.IsEmpty()) {
    Sequence not_done_l (ASTAUX::SetToSequenceR(not_done));
    size_t len_not_done_l = not_done_l.Length();
    bool exists = false;
    for (size_t j = 1; j <= len_not_done_l; j++) {
      const TYPE_AS_Name & nm (not_done_l[j]);
      if (IsStaticalyDependingClassesInit(nm)) {
        InitClassName(nm);
        exists = true;
        not_done.RemElem(nm);
      }
    }
    if (!exists) {
      RTERR::Error(L"InitTheClasses", RTERR_INTERNAL_ERROR, Nil(), Nil(), Sequence());
    }
  }
}

// InitClassName
// nm : AS`Name
// ==> ()
void EvalState::InitClassName(const TYPE_AS_Name & nm)
{
  if (!IsClassInit(nm)) {
    if (IsStaticalyDependingClassesInit(nm)) {
      // All super class(es) is(are) initialized
      SetClassInit(nm, Bool(true));
      PushInitPool(nm);
      if (TOOLS::isBatchMode() && !TOOLS::isTracesMode())
        vdm_log << L"Initializing " << ASTAUX::ASName2String(nm) << endl;

      // for Status Report
      ToolMediator::Interf()->vdm_IncrementMeter(SEQ<Char>(L"Initializing ").ImpConc(AUX::SingleNameToString(nm)));

      Map localhchy (CLASS::TransLocalHchy( nm ) );

      TYPE_GLOBAL_SigmaClass sigmacl (this->classes[nm]);
      sigmacl.SetField(pos_GLOBAL_SigmaClass_localhchy, localhchy);
      this->classes.ImpModify(nm, sigmacl);

      SEQ<TYPE_AS_InstAssignDef> instvars (GetInstVars(nm));

      theStackMachine().PushEmptyEnv(); // FIXME(etn 991231: are the pushes needed?)
      theStackMachine().PushEmptyEnv();
      theStackMachine().PushBlkEnv(AUX::MkEmptyBlkEnv(sem_read_write));

      CLASS::GenInsMap(nm);

// 20090401-->
//      TYPE_SEM_InsStrct tmp_inst (GetInstInitVal(nm));
//      TYPE_SEM_OBJ semobj;
//      semobj.Init(nm, tmp_inst, MAP<TYPE_AS_Name,TYPE_SEM_History>());
//
//      TYPE_SEM_OBJ_uRef tmp_ref (theStackMachine().MakeNewObj(semobj, Nil()));
      TYPE_SEM_OBJ_uRef tmp_ref (theStackMachine().MakeNewObj(GetCachedOBJ(nm), Nil()));
// <--20090401

      theStackMachine().PushCurObj(tmp_ref, Nil(), Nil());

      SEQ<TYPE_AS_ValueDef> vls_def (GetVlsDef(nm));
      TYPE_GLOBAL_ValueMap vls_init (CLASS::InitGV(vls_def, nm));

      sigmacl = this->classes[nm];
      sigmacl.SetField(pos_GLOBAL_SigmaClass_vls_uinit, vls_init);
      this->classes.ImpModify(nm, sigmacl);

      theStackMachine().ResetGuard();

#ifdef VICE
      if (!sigmacl.GetBoolValue(pos_GLOBAL_SigmaClass_sys))
#endif // VICE
      InitStaticInsts(nm, instvars, false);

      theStackMachine().PopCurObj();
      theStackMachine().PopEnvL();
      theStackMachine().PopEnvL();

      SetClassInit(nm, Bool(true));
      PopInitPool(nm);
    }
    else { // All super class(es) is(are) not initialized
      SET<TYPE_AS_Name> supers (GetNotInitStaticalyDependingClasses(nm));
      InitTheClasses(supers);
      InitClassName(nm);
    }
  }
}

// InitStaticInsts
// clsnm : AS`Name
// instdef_l : seq of AS`InstAssignDef ???
// ast_is_new : bool
// ==> ()
void EvalState::InitStaticInsts(const TYPE_AS_Name& clsnm,
                                const SEQ<TYPE_AS_InstanceVarDef> & instdef_l,
                                bool ast_is_new)
{
  bool already = false;

  PushInitClass(clsnm);
  TYPE_GLOBAL_SigmaClass classClsnm (this->classes[clsnm]);

  TYPE_GLOBAL_ValueMap statics (classClsnm.GetMap(pos_GLOBAL_SigmaClass_statics));

  size_t len_instdef_l = instdef_l.Length();
  for (size_t i = 1; i <= len_instdef_l; i++) {
    const TYPE_AS_InstanceVarDef & ivd (instdef_l[i]);
    switch(ivd.GetTag()) {
      case TAG_TYPE_AS_InstAssignDef: {
        if (ivd.GetBoolValue(pos_AS_InstAssignDef_stat)) {
          const TYPE_AS_AssignDef & ad (ivd.GetRecord(pos_AS_InstAssignDef_ad));
          const TYPE_AS_Name & var (ad.GetRecord(pos_AS_AssignDef_var));
          if( statics.DomExists(var) && ast_is_new ) {
            already = true;
          }
          if (!already) {
            const TYPE_AS_Type & tp (ad.GetRecord(pos_AS_AssignDef_tp));
            type_dUU3P t ((Generic)mk_(TYPE_SEM_UNDEF(), tp, ivd.GetField(pos_AS_InstAssignDef_access)));
            statics.ImpModify(var, t);
          }
        }
      }
      default:
        break;
    }
  }
  classClsnm.SetField(pos_GLOBAL_SigmaClass_statics, statics);
  this->classes.ImpModify(clsnm, classClsnm);

  for (size_t j = 1; j <= len_instdef_l; j++) {
    const TYPE_AS_InstanceVarDef & ivd (instdef_l[j]);
    switch(ivd.GetTag()) {
      case TAG_TYPE_AS_InstAssignDef: {
        if (ivd.GetBoolValue(pos_AS_InstAssignDef_stat) && !already) {
          const TYPE_AS_AssignDef & ad (ivd.GetRecord(pos_AS_InstAssignDef_ad));
          const Generic & adinit (ad.GetField(pos_AS_AssignDef_dclinit));
          if (adinit.IsNil()) {
            RTERR::InitError(L"InitStaticInsts",
                             RTERR_STATIC_IV_NO_VALUE, Nil(), Nil(),
                             ad.GetRecord(pos_AS_AssignDef_var).GetInt(pos_AS_Name_cid), Sequence());
          }
          else {
            Tuple resTup (theStackMachine().EvalUninterruptedCmd(adinit,
                                                                 TYPE_STKM_SubProgram(),
                                                                 TYPE_STKM_SubProgram(),
                                                                 SEQ<Char>(L"Init of Static Instances")));
  
            const TYPE_STKM_EvaluationState & eval_state (resTup.GetRecord(1));
            if (eval_state.Is(TAG_TYPE_STKM_Success)) {
              const TYPE_SEM_VAL & exp_v (resTup.GetRecord(2));
  
// 02.06.2001 M.Lebedev See PGL's comments in spec
//              if (defaultV.GetValue())
//                if ((Record(adinit).Is(TAG_TYPE_AS_NewExpr)) &&
//                  (TYPE_AS_NewExpr(adinit).get_args().IsEmpty()) &&
//                  GetDefaultCons(TYPE_AS_NewExpr(adinit).get_cls()).GetValue())
//                Pop(1);
  
              const TYPE_AS_Type & tp (ad.GetRecord(pos_AS_AssignDef_tp));
              if (Settings.DTC() && !SubType(exp_v, tp)) {
                RTERR::InitError(L"InitStaticInsts", RTERR_TYPE_INCOMP, exp_v, ad.get_tp(),
                                 ad.GetRecord(pos_AS_AssignDef_var).GetInt(pos_AS_Name_cid),  Sequence());
              }
#ifdef VICE
              if (Record(this->classes[clsnm]).GetBoolValue(pos_GLOBAL_SigmaClass_sys)) {
                VC::AddInstDecl(clsnm, ad.GetRecord(pos_AS_AssignDef_var), exp_v);
                if (this->obj_tab.DomExists(exp_v)) {
                  // 20091124
                  //TYPE_SEM_InsStrct instm (this->obj_tab[exp_v].GetRecord(pos_GLOBAL_OBJ_uDesc_sem).GetMap(pos_SEM_OBJ_ins));
                  TYPE_SEM_InsStrct instm (GetSemObjInTab(exp_v).GetMap(pos_SEM_OBJ_ins));
                  Sequence clnm_l (ASTAUX::SetToSequenceR(instm.Dom()));
                  Generic clnm;
//                  for (bool dd = instm.First(clnm); dd; dd = instm.Next(clnm))
                  for (bool dd = clnm_l.First(clnm); dd; dd = clnm_l.Next(clnm)) {
                    Map vm (instm[clnm]);
                    Sequence instnm_l (ASTAUX::SetToSequenceR(vm.Dom()));
                    Generic instnm;
//                    for (bool ee = vm.First(instnm); ee; ee = vm.Next(instnm))
                    for (bool ee = instnm_l.First(instnm); ee; ee = instnm_l.Next(instnm)) {
                      Tuple t (vm[instnm]);
                      const TYPE_SEM_VAL & val_v (t.GetRecord(1));
                      VC::LogInstVarChange(instnm, val_v, exp_v, theSystem().GetCurThread());
                    }
                  }
                }
              }
#endif // VICE

              TYPE_GLOBAL_SigmaClass classClsnm (this->classes[clsnm]);
              TYPE_GLOBAL_ValueMap statics (classClsnm.GetMap(pos_GLOBAL_SigmaClass_statics));
              statics.ImpModify(ad.GetRecord(pos_AS_AssignDef_var),
                                (Generic)mk_(exp_v, tp, ivd.GetField(pos_AS_InstAssignDef_access)));
              classClsnm.SetField(pos_GLOBAL_SigmaClass_statics, statics);
              this->classes.ImpModify(clsnm, classClsnm);
            }
            else
              RTERR::Error(L"InitStaticInsts", RTERR_INTERNAL_ERROR, Nil(), Nil(), Sequence());
          }
        }
      }
      default:
        break;
    }
  }
  PopInitClass();
}

void EvalState::PopInitClass()
{
  if( !this->initclstack.IsEmpty() )
    this->initclstack.RemElem(1);
}

void EvalState::PushInitClass(const TYPE_AS_Name & clsnm)
{
  this->initclstack.ImpPrepend(clsnm);
}

// IsSuperClassesInit
// nm : AS`Name
// ==> bool
bool EvalState::IsSuperClassesInit(const TYPE_AS_Name & nm) const
{
  SET<TYPE_AS_Name> supers (GetAllSupers(nm));
  if (supers.IsEmpty()) return true;

  bool forall = true;
  Generic clnm;
  for (bool bb = supers.First(clnm); bb && forall; bb = supers.Next(clnm)) {
    forall = IsClassInit(clnm);
  }
  return forall;
}
#endif // VDMPP

#ifdef VDMSL
// OM_TransInsertModule
// mod_sig : (AS`Module|AS`DLModule)
// ==> ()
void EvalState::OM_TransInsertModule (const Record & mod_sig)
{
  TYPE_AS_Name mod_name (mod_sig.Is(TAG_TYPE_AS_DLModule)
                         ? mod_sig.GetRecord(pos_AS_DLModule_nm)
                         : mod_sig.GetRecord(pos_AS_Module_nm));

  theCompiler().SetClMod(mod_name);
  theCompiler().ResetProgramTable(mod_name);

  if (this->mods.DomExists (mod_name) || this->imods.DomExists (mod_name)) {
    RTERR::Error (L"OM_TransInsertModule",
                         RTERR_MOD_ALREADY_DEF,
                         M42Sem(AUX::SingleNameToString(mod_name), NULL),
                         Nil(), Sequence());
  }

  switch(mod_sig.GetTag()) {
    case TAG_TYPE_AS_DLModule: {
      this->imods.Insert(mod_name, MOD::TranslateDLModule(mod_sig));
      break;
    }
    case TAG_TYPE_AS_Module: {
      this->mods.Insert(mod_name, MOD::TranslateModule(mod_sig));
      break;
    }
    default:
      break;
  }
}

// GetModule
// mod_name : AS`Name
// ==> GLOBAL`SigmaMO | GLOBAL`SigmaIMO
const TYPE_GLOBAL_SigmaMO & EvalState::GetModule(const TYPE_AS_Name & mod_name) const
{
  if (this->mods.DomExists (mod_name)) {
    return this->mods[mod_name];
  }
  else 
  {
    RTERR::Error (L"GetModule", RTERR_MOD_NOT_DEFINED, M42Sem(AUX::SingleNameToString(mod_name), NULL), Nil(), Sequence());
    return this->nullSigmaMo;
  }
}

// GetAnyModule (not in spec)
// mod_name : AS`Name
// ==> GLOBAL`SigmaMO | GLOBAL`SigmaIMO
Record EvalState::GetAnyModule(const TYPE_AS_Name & mod_name) const
{
  if (this->mods.DomExists (mod_name)) {
    return this->mods[mod_name];
  }
  else if (this->imods.DomExists (mod_name)) {
    return this->imods[mod_name];
  }
  else 
  {
    RTERR::Error (L"GetAnyModule", RTERR_MOD_NOT_DEFINED,
                         M42Sem(AUX::SingleNameToString(mod_name), NULL),
                         //Nil());
                         Nil(), Sequence());
    return Record();
  }
}

// IsEmptyMods
// ==> bool
bool EvalState::IsEmptyMods()
{
  return this->mods.IsEmpty();
}

// PushModuleIfAny
// ==> ()
void EvalState::PushModuleIfAny()
{
  // pick a random module, and make this the default. in case of a
  // single definitions block, we automatically make the created
  // module the default. this means we do not have to preceed all
  // names with the name of a module in this case
// 20090123
//  if (! this->mods.IsEmpty()) {
//    TYPE_AS_Name mod_name (this->mods.Dom().GetElem());
//    theStackMachine().PushModule (mod_name);
  Set dom_mods (this->mods.Dom().ImpDiff(mk_set(ASTAUX::MkNameFromVoid())));
  if (! dom_mods.IsEmpty()) {
    Sequence name_l (dom_mods.ToSequence());
    theStackMachine().PushModule (name_l.Hd());
  }
}

//// UpdateRenameAndImport
//// mod_name : AS`Name
//// new_name : AS`Name
//// imp : [AS`ImportSig]
//void EvalState::UpdateRenameAndImport (const TYPE_AS_Name & mod_name, const TYPE_AS_Name & new_name,
//                                       const Generic & imp)
//{
//  Map tmp_m;
//  tmp_m.Insert (new_name, imp);
//
//  TYPE_GLOBAL_SigmaMO sigmamo (GetModule(mod_name));
//  sigmamo.SetField(pos_GLOBAL_SigmaMO_ren, MOD::BuildRenameMap(tmp_m));
//  this->mods.ImpModify(mod_name, sigmamo);
//
////-------------------
////  Record r (GetModule(mod_name));
////  if(r.Is(TAG_TYPE_DYNSEM_GLOBAL_SigmaMO))
//}

// IsGlobalState
// loc_name : AS`Name
// mod_name : AS`Name
// ==> bool * [GLOBAL`State]
Tuple EvalState::IsGlobalState (const TYPE_AS_Name & loc_name, const TYPE_AS_Name & mod_name)
{
  TYPE_GLOBAL_StateMap gs (GetModuleState(mod_name));

  if (gs.DomExists (loc_name))
    return mk_(Bool(true), gs[loc_name]);
  else
    return mk_(Bool(false), Nil());
}

// GetGlobalState
// loc_name : AS`Name
// mod_name : AS`Name
// ==> GLOBAL`State
TYPE_GLOBAL_State EvalState::GetGlobalState (const TYPE_AS_Name & loc_name, const TYPE_AS_Name & mod_name)
{
  TYPE_GLOBAL_StateMap gs (GetModuleState(mod_name));
  return gs[loc_name];
}

// SetGlobalState
// loc_name : AS`Name
// mod_name : AS`Name
// s_val : GLOBAL`State
void EvalState::SetGlobalState (const TYPE_AS_Name & loc_name, const TYPE_AS_Name & mod_name,
                                const TYPE_GLOBAL_State & s_val)
{
  // 20060510
  const TYPE_AS_Name & curmod (mod_name);
  if (curmod.GetSequence(pos_AS_Name_ids).IsEmpty()) return;

  TYPE_GLOBAL_SigmaMO sigmamo (GetModule(curmod));
  Map & gs = (Map &)(sigmamo.GetFieldRef(pos_GLOBAL_SigmaMO_gs));
//  TYPE_GLOBAL_StateMap gs (sigmamo.GetMap(pos_GLOBAL_SigmaMO_gs));
  gs.ImpModify(loc_name, s_val);
//  sigmamo.SetField(pos_GLOBAL_SigmaMO_gs, gs);
  this->mods.ImpModify(curmod, sigmamo);
}

// IsTheState
// name : AS`Name
// ==> bool
bool EvalState::IsTheState(const TYPE_AS_Name & name, const TYPE_AS_Name & mod_name)
{
  const TYPE_GLOBAL_SigmaMO & sigmamo (GetModule(mod_name));
  return (name == sigmamo.GetRecord(pos_GLOBAL_SigmaMO_stp_uid));
}

// GetTheState
// name : AS`Name
// ==> SEM`REC
TYPE_DYNSEM_SEM_SemRecord EvalState::GetTheState(const TYPE_AS_Name & name, const TYPE_AS_Name & mod_name)
{
  const TYPE_GLOBAL_SigmaMO & sigmamo (GetModule(mod_name));
  const Tuple & recsel (sigmamo.GetMap(pos_GLOBAL_SigmaMO_recsel)[name]);// RecSel
  MAP<TYPE_AS_Name, Int> namem (recsel.GetMap(2));

  const MAP<TYPE_AS_Name, TYPE_GLOBAL_State> & gs (sigmamo.GetMap(pos_GLOBAL_SigmaMO_gs));

  // Remark the implementation differs from the specification here,
  // because of the different implementation of the semantic value
  // of records.

  SEQ<TYPE_SEM_VAL> val_l;
  for (int i = 0; i < namem.Size(); i++)
    val_l.ImpAppend(TYPE_SEM_NIL());

  Generic elm;
  for (bool bb = namem.First(elm); bb; bb = namem.Next(elm))
    val_l.ImpModify(namem[elm], gs[elm].GetRecord(pos_GLOBAL_State_val));

  return SemRec::mk_SEM_REC(AUX::UniqueTagName(name), val_l);
}

// SetTheState
// name : AS`Name
// rec : SEM`REC
void EvalState::SetTheState(const TYPE_AS_Name & name, const TYPE_AS_Name & mod_name,
                            const TYPE_DYNSEM_SEM_SemRecord & rec)
{
  const TYPE_AS_Name & curmod (mod_name);

  TYPE_GLOBAL_SigmaMO sigmamo (GetModule(curmod));
  const Tuple & recsel (sigmamo.GetMap(pos_GLOBAL_SigmaMO_recsel)[name]); // RecSel
  const MAP<TYPE_AS_Name, Int> & namem (recsel.GetMap(2)); // (map AS`Name to nat)

  // Remark the implementation differs from the specification here,
  // because of the different implementation of the semantic value
  // of records.
  SEQ<TYPE_SEM_VAL> val (rec.GetRecord(pos_DYNSEM_SEM_SemRecord_value).GetFields());

  const TYPE_GLOBAL_StateMap & gs (sigmamo.GetMap(pos_GLOBAL_SigmaMO_gs));

  TYPE_GLOBAL_StateMap gs_new;
  Set dom_namem (namem.Dom());
  Generic key;
  for (bool bb = dom_namem.First(key); bb; bb = dom_namem.Next(key))
  {
    TYPE_GLOBAL_State elm (gs[key]);
    const Int & index (namem[key]);
    elm.SetField(pos_GLOBAL_State_val, val[index]);
    gs_new.ImpModify(key, elm);
  }
  sigmamo.SetField(pos_GLOBAL_SigmaMO_gs, gs_new);
  this->mods.ImpModify(curmod, sigmamo);
}

// GetCurrentState
// mod_name : AS`Name
// ==> GLOBAL`StateMap (map AS`Name to State)
TYPE_GLOBAL_StateMap EvalState::GetCurrentState (const TYPE_AS_Name & mod_name) const
{
  return GetModuleState (mod_name);
}

// GetModuleState
// AS`Name
// ==> GLOBAL`StateMap (map AS`Name to State)
TYPE_GLOBAL_StateMap EvalState::GetModuleState (const TYPE_AS_Name & name) const
{
//  return GetModule(name).GetMap(pos_GLOBAL_SigmaMO_gs);
  if (this->mods.DomExists (name)) {
    return this->mods[name].GetMap(pos_GLOBAL_SigmaMO_gs);
  }
  else 
  {
    RTERR::Error (L"GetModuleState", RTERR_MOD_NOT_DEFINED, M42Sem(AUX::SingleNameToString(name), NULL), Nil(), Sequence());
    return TYPE_GLOBAL_StateMap();
  }
}

// IsGlobalVal
// id : AS`Name
// ==> bool * [SEM`VAL] * [AS`Type]
Tuple EvalState::IsGlobalVal (const TYPE_AS_Name & id, const TYPE_AS_Name & mod_name)
{
  const TYPE_AS_Name & name (mod_name);
  if (this->mods.DomExists (name)) {
    const Generic & gv (this->mods[name].GetField(pos_GLOBAL_SigmaMO_gv));

    if (!gv.IsNil () && ((Map &)gv).DomExists(id)) {
      Tuple t (((Map &)gv)[id]);
      return mk_(Bool(true), t.GetRecord(1), t.GetField(2));
    }
    else {
      return mk_(Bool(false), Nil(), Nil());
    }
  }
  else {
    RTERR::Error (L"IsGlobalVal", RTERR_MOD_NOT_DEFINED, M42Sem(AUX::SingleNameToString(name), NULL), Nil(), Sequence());
    return mk_(Bool(false), Nil(), Nil());
  }
}
#endif // VDMSL

#ifdef VDMPP
// SetInstanceVar
// nm : AS`Name
// val_v : SEM`VAL
// iios : bool * bool * [SEM`VAL] * [AS`Type] * [AS`Name] * [AS`Access]
void EvalState::SetInstanceVar(const TYPE_AS_Name & nm, const TYPE_SEM_VAL & val_v, const Tuple & iios)
{
  const TYPE_SEM_OBJ_uRef & the_ref (theStackMachine().GetCurObjRef());

#ifdef VICE
  VC::LogInstVarChange(nm, val_v, the_ref, theSystem().GetCurThread());
#endif // VICE

  // ==> bool * bool * [SEM`VAL] * [AS`Type] * [AS`Name] * [AS`Access]
// 20090109
//  Tuple iios (IsInObjScope(nm, Nil()));
  //Tuple iios (lookup.IsNil() ? IsInObjScope(nm, Nil()) : Tuple(lookup));
  bool isit (iios.GetBoolValue(1));

// 20090311 -->
  TYPE_AS_Name clnm;
  if (isit) {
    clnm = iios.GetField(5);
  }
  else {
    // bool * [SEM`VAL] * *[AS`Type] * [As`Name] * [AS`Access]
    Tuple lus (LookUpStatic(nm));
    if (lus.GetBoolValue(1)) {
      isit = true;
      clnm = lus.GetField(4);
    }
  }
// <-- 20090311

  TYPE_AS_Name thename (AUX::ExtractName(nm));

  if (IsStatic(clnm, thename))
  { // static variable
    TYPE_GLOBAL_SigmaClass classClsnm (this->classes[clnm]);
    TYPE_GLOBAL_ValueMap statics (classClsnm.GetMap(pos_GLOBAL_SigmaClass_statics)); // map AS`Name to (SEM`VAL * AS`Access)
    type_dU2P t (statics[thename]); // (SEM`VAL * [AS`Type] * AS`Access)
    t.SetField(1, val_v);           // SEM`VAL
    statics.ImpModify(thename, t);
    classClsnm.SetField(pos_GLOBAL_SigmaClass_statics, statics);
    this->classes.ImpModify(clnm, classClsnm);
  }
  else
  { // dynamic variable
//    TYPE_SEM_InsStrct inststrct (theStackMachine().GetCurObj().GetMap(pos_SEM_OBJ_ins));
//    Generic g1;
//    if (isit && Map(inststrct).DomExists(clnm, g1) && Map(g1).DomExists(thename))

    //if (isit && ins.DomExists(clnm) && ins[clnm].DomExists(thename))
    if (isit)
    {
#ifdef VICE
      SET<TYPE_AS_Name> opnms (theScheduler().BlockedObjRef(the_ref));
      TYPE_GLOBAL_SigmaClass sigmaClass (this->classes[clnm]);
      const Map & deps (sigmaClass.GetMap(pos_GLOBAL_SigmaClass_depnms));   
      opnms.ImpIntersect(deps.Dom());
      Generic nm;
      for(bool bb = opnms.First(nm); bb; bb = opnms.Next(nm))
      {
        Generic dep (deps[nm]);
        if( dep.IsQuote() || Set(dep).InSet(thename) )
        {
          theScheduler().PotentialPerChange(the_ref);
          break;
        }
      }
#endif // VICE
      // Perhaps this should be a method in the ObjectTab class.
      TYPE_GLOBAL_OBJ_uDesc desc (this->obj_tab[the_ref]);
      TYPE_SEM_OBJ sem (desc.GetRecord(pos_GLOBAL_OBJ_uDesc_sem));
      TYPE_SEM_InsStrct ins (sem.GetMap(pos_SEM_OBJ_ins)); // map AS`Name to GLOBAL`ValueMap

      TYPE_GLOBAL_ValueMap gv (ins[clnm]);// map AS`Name to (SEM`VAL * [AS`Type] *AS`Access)
      type_dU2P t (gv[thename]);           // (SEM`VAL * [AS`Type] *AS`Access)
      TYPE_SEM_VAL old_val (t.GetRecord(1));  // SEM`VAL

      t.SetField(1, val_v);            // SEM`VAL
      gv.ImpModify(thename, t);
      ins.ImpModify(clnm, gv);

      sem.SetField(pos_SEM_OBJ_ins, ins);
      desc.SetField(pos_GLOBAL_OBJ_uDesc_sem, sem);
      this->obj_tab.ImpModify(the_ref, desc);
    }
    else
    {
      RTERR::Error(L"SetInstanceVar", RTERR_INIT_NOT_POSSIBLE, M42Sem(AUX::SingleNameToString(nm), NULL), Nil(), Sequence());
    }
  }
}

// IsDLClass
// n : AS`Name ==> bool
bool EvalState::IsDLClass(const TYPE_AS_Name & cls)
{
  return this->dlops.DomExists(cls);
}

// InsertDLClass
// cl : AS`Name
// ==> ()
void EvalState::InsertDLClass(const TYPE_AS_Name & cls)
{
  this->dlops.ImpModify(cls, Nil());
}

// InsertDLOp
// cls : AS`Name
// fnop : AS`Name
// ==> ()
void EvalState::InsertDLOp(const TYPE_AS_Name & cls, const TYPE_AS_Name & fnop)
{
  // Insert class`fnop in map
  this->dlops.ImpModify(ASTAUX::Combine2Names(cls, fnop), Nil());
}

// IsDLOp
// cl : AS`Name
// opnm : AS`Name
// ==> bool
bool EvalState::IsDLOp(const TYPE_AS_Name & cls, const TYPE_AS_Name & fnop)
{
  return this->dlops.DomExists(ASTAUX::Combine2Names(cls, fnop));
}

// RegisterDLClass
// clnm : AS`Name
// useslib : AS`TextLit
// gdefs : [AS`Definitions]
void EvalState::RegisterDLClass(const TYPE_AS_Name & clnm, const TYPE_AS_TextLit & useslib, const Generic & gdefs)
{
  InsertDLClass(clnm);
  if (! gdefs.IsNil()) {
    TYPE_AS_Definitions defs (gdefs);
    Map m (defs.GetMap(pos_AS_Definitions_fnm));
    m.ImpOverride (defs.GetMap(pos_AS_Definitions_opm));

    Set dom_m (m.Dom());
    Generic nm, fnop;
    for (bool bb = dom_m.First(nm); bb; bb = dom_m.Next(nm)) {
      Record fnop (m[nm]);
      switch(fnop.GetTag()) {
        case TAG_TYPE_AS_ExplOpDef:
        case TAG_TYPE_AS_ExtExplOpDef: {
          if (ASTAUX::GetOpBody(fnop).get_body() == Int(NOTYETSPEC))
            InsertDLOp(clnm, nm);
          break;
        }
        case TAG_TYPE_AS_ExplFnDef:
        case TAG_TYPE_AS_ExtExplFnDef: {
          if (ASTAUX::GetFnBody(fnop).get_body() == Int(NOTYETSPEC))
            InsertDLOp(clnm, nm);
          break;
        }
      }
    }
  }

  try
  {
    TYPE_AS_Name nulname (ASTAUX::MkNameFromVoid());
    this->dlfactory.InsertClassLib(clnm, useslib, nulname);
  }
  catch (DLException e)
  {
    vdm_iplog << e.GetErrorMessage() << endl << flush;
    RTERR::Error(L"RegisterDLClass", RTERR_DLCLASS_LOAD_ERROR, Nil(), Nil(), Sequence());
  }
}
#endif // VDMPP

// EvalFctTypeInstExpr
// fct_v : SEM`POLY
// inst : seq of AS`Type
// ==> SEM`VAL
TYPE_SEM_VAL EvalState::EvalFctTypeInstExpr (const TYPE_SEM_POLY & fct_v, const SEQ<TYPE_AS_Type> & inst )
{
  switch(fct_v.GetTag()) {
    case TAG_TYPE_SEM_ExplPOLY: {
      Map tm (theStackMachine().HdTypeInst());
      SEQ<TYPE_AS_Type> new_inst (ConvertTypeVarTypeList(inst, tm));

      return (EXPR::ConvertPolyToFn (fct_v, new_inst));
    }
    default: {
      return RTERR::ErrorVal (L"EvalFctTypeInstExpr", RTERR_NO_POLY_FUNC, fct_v, Nil(), Sequence());
    }
  }
}

#ifdef VDMPP
// EvalOverFctTypeInstExpr
// ovfct_v : SEM`OverOPFN
// inst : seq of AS`Type
// ==> SEM`VAL
TYPE_SEM_VAL EvalState::EvalOverFctTypeInstExpr (const TYPE_SEM_OverOPFN & ovfct_v, const SEQ<TYPE_AS_Type> & inst )
{
  const Map & overload (ovfct_v.GetMap(pos_SEM_OverOPFN_overload));
      // map (AS`Name * AS`Name) to ((seq of AS`Type) * AS`Access)

  // TODO: override & overload
  Map tm (theStackMachine().HdTypeInst());
  SEQ<TYPE_AS_Type> new_inst (ConvertTypeVarTypeList(inst, tm));
  
  Set dom_overload (overload.Dom()); // set of (AS`Name * AS`Name)

// 20111020 -->
  Map mnm_to_nm_s; // map AS`Name to (set of AS`Name)
  Map nt_to_v; // map (AS`Name * AS`Name) to SEM`VAL
  Map new_over; // map (AS`Name * AS`Name) to ((seq of AS`Type) * AS`Access * [SEM`VAL]))
  Generic g;
  for (bool bb = dom_overload.First(g); bb; bb = dom_overload.Next(g))
  {
    const Tuple & nt (g);           // (AS`Name * AS`Name)
    const Tuple & t (overload[g]); // ((seq of AS`Type) * AS`Access)
    
    const TYPE_AS_Name & clsnm (nt.GetRecord(2));
    const TYPE_AS_Name & overnm (nt.GetRecord(1));
    // bool * [ (SEM`CompExplFN | SEM`ExplOP | SEM`ExplPOLY) * AS`Access ]
    Tuple lafop (LookupAllFnsOpsPolys(clsnm, overnm));
    if (lafop.GetBoolValue(1))
    {
      Tuple tt (lafop.GetTuple(2));
      TYPE_SEM_ExplPOLY opval (tt.GetRecord(1));
      SEQ<TYPE_AS_TypeVar> tv_l (opval.GetSequence(pos_SEM_ExplPOLY_tpparms));
      if (tv_l.Length() == inst.Length())
      {
        SEQ<TYPE_AS_Type> tp_l (EXPR::SubstTypeList(t.GetSequence(1), tv_l, new_inst));
        new_over.Insert(nt, mk_(tp_l, t.GetField(2), Nil()));

        if (!mnm_to_nm_s.DomExists(overnm))
          mnm_to_nm_s.Insert(overnm, Set());
        Set nm_s (mnm_to_nm_s[overnm]);
        nm_s.Insert(clsnm);
        mnm_to_nm_s.ImpModify(overnm, nm_s);

        nt_to_v.Insert(nt, opval);
      }
    }
  }

  Map new_over2;
  Set dom_mnm_to_nm_s (mnm_to_nm_s.Dom());
  for (bool cc = dom_mnm_to_nm_s.First(g); cc; cc = dom_mnm_to_nm_s.Next(g))
  {
    Set nm_s (mnm_to_nm_s[g]);
    Tuple eoc (ExistsOneChild (nm_s, nm_s));
    if (eoc.GetBoolValue(1))
    {
      Tuple nt (mk_(g, eoc.GetRecord(2)));
      Tuple r (new_over[nt]);
// 20111021 -->
      r.SetField(3, EXPR::ConvertPolyToFn(nt_to_v[nt], new_inst));
// <--20111021
      new_over2.Insert(nt, r); 
    }
  }
  
  switch (new_over2.Dom().Card()) {
    case 0: {
      return RTERR::ErrorVal(L"EvalState::EvalOverFctTypeInstExpr", RTERR_NOOVERLOADED, ovfct_v,
                          (inst.IsEmpty() ? Nil() : inst.Hd()), Sequence());
    }
    case 1: {
      //const Tuple & nt (new_over2.Dom().GetElem());
      //return EXPR::ConvertPolyToFn(nt_to_v[nt], new_inst);
      Tuple t (new_over2.Rng().GetElem());
      return t.GetField(3);
    }
    default: {
      TYPE_SEM_OverOPFN new_ovfct_v(ovfct_v);
      new_ovfct_v.SetField(pos_SEM_OverOPFN_overload, new_over2);
      return new_ovfct_v;
    }
  }
}
#endif // VDMPP

// ConvertTypeVarTypeList
// type : seq of AS`Type
// tm : map AS`TyepVar to AS`Type
// ==> seq of AS`Type
SEQ<TYPE_AS_Type> EvalState::ConvertTypeVarTypeList(const SEQ<TYPE_AS_Type> & tp_l, const Map & tm) const
{
  SEQ<TYPE_AS_Type> res;
  size_t len_tp_l = tp_l.Length();
  for (size_t idx = 1; idx <= len_tp_l; idx++)
  {
    res.ImpAppend(ConvertTypeVarType(tp_l[idx], tm));
  }
  return res;
}

// ConvertTypeVarType
// type : AS`Type
// tm : map AS`TyepVar to AS`Type
// ==> AS`Type
TYPE_AS_Type EvalState::ConvertTypeVarType(const TYPE_AS_Type & type, const Map & tm) const
{
  switch(type.GetTag()) {
    case TAG_TYPE_AS_TypeVar: {
      if (tm.DomExists(type))
        return ConvertTypeVarType(tm[type], tm);
      else
      {
        RTERR::Error (L"EvalFctTypeInstExpr", RTERR_TYPE_UNKNOWN, Nil(), type, Sequence());
        return type;
      }
    }
    case TAG_TYPE_AS_CompositeType: {
      const SEQ<TYPE_AS_Field> & fields (type.GetSequence(pos_AS_CompositeType_fields));
      SEQ<TYPE_AS_Field> new_fls;
      size_t len_fields = fields.Length();
      for (size_t i = 1; i <= len_fields; i++)
      {
        TYPE_AS_Field f (fields[i]);
        f.set_type(ConvertTypeVarType(f.GetRecord(pos_AS_Field_type), tm));
        new_fls.ImpAppend(f);
      }
      return TYPE_AS_CompositeType().Init(type.GetRecord(pos_AS_CompositeType_name),
                                          new_fls,
                                          type.GetInt(pos_AS_CompositeType_cid));
    }

    case TAG_TYPE_AS_UnionType: {
      return TYPE_AS_UnionType().Init(ConvertTypeVarTypeList(type.GetSequence(pos_AS_UnionType_tps), tm),
                                      type.GetInt(pos_AS_UnionType_cid));
    }

    case TAG_TYPE_AS_ProductType: {
      return TYPE_AS_ProductType().Init(ConvertTypeVarTypeList(type.GetSequence(pos_AS_ProductType_tps), tm),
                                        type.GetInt(pos_AS_ProductType_cid));
    }
    case TAG_TYPE_AS_OptionalType: {
      return TYPE_AS_OptionalType().Init(ConvertTypeVarType(type.GetRecord(pos_AS_OptionalType_tp), tm),
                                         type.GetInt(pos_AS_OptionalType_cid));
    }
    case TAG_TYPE_AS_BracketedType: {
      return TYPE_AS_BracketedType().Init(ConvertTypeVarType(type.GetRecord(pos_AS_BracketedType_tp), tm),
                                          type.GetInt(pos_AS_BracketedType_cid));
    }
    case TAG_TYPE_AS_Set0Type: {
      return TYPE_AS_Set0Type().Init(ConvertTypeVarType(type.GetRecord(pos_AS_Set0Type_elemtp), tm),
                                    type.GetInt(pos_AS_Set0Type_cid));
    }
    case TAG_TYPE_AS_Set1Type: {
      return TYPE_AS_Set1Type().Init(ConvertTypeVarType(type.GetRecord(pos_AS_Set1Type_elemtp), tm),
                                    type.GetInt(pos_AS_Set1Type_cid));
    }
    case TAG_TYPE_AS_Seq0Type: {
      return TYPE_AS_Seq0Type().Init(ConvertTypeVarType(type.GetRecord(pos_AS_Seq0Type_elemtp), tm),
                                     type.GetInt(pos_AS_Seq0Type_cid));
    }
    case TAG_TYPE_AS_Seq1Type: {
      return TYPE_AS_Seq1Type().Init(ConvertTypeVarType(type.GetRecord(pos_AS_Seq1Type_elemtp), tm),
                                     type.GetInt(pos_AS_Seq1Type_cid));
    }
    case TAG_TYPE_AS_GeneralMap0Type: {
      return TYPE_AS_GeneralMap0Type().Init(ConvertTypeVarType(type.GetRecord(pos_AS_GeneralMap0Type_mapdom), tm),
                                           ConvertTypeVarType(type.GetRecord(pos_AS_GeneralMap0Type_maprng), tm),
                                           type.GetInt(pos_AS_GeneralMap0Type_cid));
    }
    case TAG_TYPE_AS_GeneralMap1Type: {
      return TYPE_AS_GeneralMap1Type().Init(ConvertTypeVarType(type.GetRecord(pos_AS_GeneralMap1Type_mapdom), tm),
                                           ConvertTypeVarType(type.GetRecord(pos_AS_GeneralMap1Type_maprng), tm),
                                           type.GetInt(pos_AS_GeneralMap1Type_cid));
    }
    case TAG_TYPE_AS_InjectiveMap0Type: {
      return TYPE_AS_InjectiveMap0Type().Init(ConvertTypeVarType(type.GetRecord(pos_AS_InjectiveMap0Type_mapdom), tm),
                                             ConvertTypeVarType(type.GetRecord(pos_AS_InjectiveMap0Type_maprng), tm),
                                             type.GetInt(pos_AS_InjectiveMap0Type_cid));
    }
    case TAG_TYPE_AS_InjectiveMap1Type: {
      return TYPE_AS_InjectiveMap1Type().Init(ConvertTypeVarType(type.GetRecord(pos_AS_InjectiveMap1Type_mapdom), tm),
                                             ConvertTypeVarType(type.GetRecord(pos_AS_InjectiveMap1Type_maprng), tm),
                                             type.GetInt(pos_AS_InjectiveMap1Type_cid));
    }
    case TAG_TYPE_AS_PartialFnType: {
      return TYPE_AS_PartialFnType().Init(ConvertTypeVarTypeList(type.GetSequence(pos_AS_PartialFnType_fndom), tm),
                                          ConvertTypeVarType(type.GetRecord(pos_AS_PartialFnType_fnrng), tm),
                                          type.GetInt(pos_AS_PartialFnType_cid));
    }
    case TAG_TYPE_AS_TotalFnType: {
      return TYPE_AS_TotalFnType().Init(ConvertTypeVarTypeList(type.GetSequence(pos_AS_TotalFnType_fndom), tm),
                                        ConvertTypeVarType(type.GetRecord(pos_AS_TotalFnType_fnrng), tm),
                                        type.GetInt(pos_AS_TotalFnType_cid));
    }
    case TAG_TYPE_AS_OpType: {
      return TYPE_AS_OpType().Init(ConvertTypeVarTypeList(type.GetSequence(pos_AS_OpType_opdom), tm),
                                   ConvertTypeVarType(type.GetRecord(pos_AS_OpType_oprng), tm),
                                   type.GetInt(pos_AS_OpType_cid));
    }
    case TAG_TYPE_AS_TypeName: {
      return type;
    }
    default: {
      return type;
      break;
    }
  }
}

// ConvertPattern
// p : STKM`Pattern
// tm : map AS`TypeVar to AS`Type
// ==> STKM`Pattern
TYPE_STKM_Pattern EvalState::ConvertPattern(const TYPE_STKM_Pattern & pat, const Map & tm) const
{
  switch (pat.GetTag()) {
    case TAG_TYPE_STKM_PatternName: {
      if (pat.GetField (pos_STKM_PatternName_tp).IsNil()) {
        return pat;
      }
      else {
        TYPE_AS_Type new_tp (ConvertTypeVarType(pat.GetField (pos_STKM_PatternName_tp), tm));
        TYPE_STKM_PatternName res (pat);
        res.SetField(pos_STKM_PatternName_tp, new_tp);
        return res;
      }
    }
    case TAG_TYPE_STKM_SetEnumPattern: {
      const SEQ<TYPE_STKM_Pattern> & els (pat.GetSequence (pos_STKM_SetEnumPattern_els));
      size_t len_els = els.Length();
      SEQ<TYPE_STKM_Pattern> new_els;
      for (size_t i = 1; i <= len_els; i++) {
        new_els.ImpAppend(ConvertPattern(els[i], tm));
      }
      TYPE_STKM_SetEnumPattern res (pat);
      res.SetField(pos_STKM_SetEnumPattern_els, new_els);
      return res;
    }
    case TAG_TYPE_STKM_SetUnionPattern: {
      const TYPE_STKM_Pattern & lp (pat.GetRecord(pos_STKM_SetUnionPattern_lp));
      const TYPE_STKM_Pattern & rp (pat.GetRecord(pos_STKM_SetUnionPattern_rp));
      TYPE_STKM_SetUnionPattern res (pat);
      res.SetField(pos_STKM_SetUnionPattern_lp, ConvertPattern(lp, tm));
      res.SetField(pos_STKM_SetUnionPattern_rp, ConvertPattern(rp, tm));
      return res;
    }
    case TAG_TYPE_STKM_SeqEnumPattern: {
      const SEQ<TYPE_STKM_Pattern> & els (pat.GetSequence (pos_STKM_SeqEnumPattern_els));
      size_t len_els = els.Length();
      SEQ<TYPE_STKM_Pattern> new_els;
      for (size_t i = 1; i <= len_els; i++)
      {
        new_els.ImpAppend(ConvertPattern(els[i], tm));
      }
      TYPE_STKM_SeqEnumPattern res (pat);
      res.SetField(pos_STKM_SeqEnumPattern_els, new_els);
      return res;
    }
    case TAG_TYPE_STKM_SeqConcPattern: {
      const TYPE_STKM_Pattern & lp (pat.GetRecord(pos_STKM_SeqConcPattern_lp));
      const TYPE_STKM_Pattern & rp (pat.GetRecord(pos_STKM_SeqConcPattern_rp));
      TYPE_STKM_SeqConcPattern res (pat);
      res.SetField(pos_STKM_SeqConcPattern_lp, ConvertPattern(lp, tm));
      res.SetField(pos_STKM_SeqConcPattern_rp, ConvertPattern(rp, tm));
      return res;
    }
    case TAG_TYPE_STKM_MapEnumPattern: {
      const SEQ<TYPE_STKM_MapletPattern> & mls (pat.GetSequence (pos_STKM_MapEnumPattern_mls));
      size_t len_mls = mls.Length();
      SEQ<TYPE_STKM_MapletPattern> new_mls;
      for (size_t i = 1; i <= len_mls; i++) {
        const TYPE_STKM_MapletPattern & mp (mls[i]);
        const TYPE_STKM_Pattern & dp (mp.GetRecord(pos_STKM_MapletPattern_dp));
        const TYPE_STKM_Pattern & rp (mp.GetRecord(pos_STKM_MapletPattern_rp));
        new_mls.ImpAppend(TYPE_STKM_MapletPattern().Init(ConvertPattern(dp, tm),
                                                         ConvertPattern(rp, tm)));
      }
      TYPE_STKM_MapEnumPattern res (pat);
      res.SetField(pos_STKM_MapEnumPattern_mls, new_mls);
      return res;
    }
    case TAG_TYPE_STKM_MapMergePattern: {
      const TYPE_STKM_Pattern & lp (pat.GetRecord(pos_STKM_MapMergePattern_lp));
      const TYPE_STKM_Pattern & rp (pat.GetRecord(pos_STKM_MapMergePattern_rp));
      TYPE_STKM_MapMergePattern res (pat);
      res.SetField(pos_STKM_MapMergePattern_lp, ConvertPattern(lp, tm));
      res.SetField(pos_STKM_MapMergePattern_rp, ConvertPattern(rp, tm));
      return res;
    }
    case TAG_TYPE_STKM_TuplePattern: {
      const SEQ<TYPE_STKM_Pattern> & fields (pat.GetSequence (pos_STKM_TuplePattern_fields));
      size_t len_fields = fields.Length();
      SEQ<TYPE_STKM_Pattern> new_fields;
      for (size_t i = 1; i <= len_fields; i++) {
        new_fields.ImpAppend(ConvertPattern(fields[i], tm));
      }
      TYPE_STKM_TuplePattern res (pat);
      res.SetField(pos_STKM_TuplePattern_fields, new_fields);
      return res;
    }
    case TAG_TYPE_STKM_RecordPattern: {
      const SEQ<TYPE_STKM_Pattern> & fields (pat.GetSequence (pos_STKM_RecordPattern_fields));
      size_t len_fields = fields.Length();
      SEQ<TYPE_STKM_Pattern> new_fields;
      for (size_t i = 1; i <= len_fields; i++) {
        new_fields.ImpAppend(ConvertPattern(fields[i], tm));
      }
      TYPE_STKM_RecordPattern res (pat);
      res.SetField(pos_STKM_RecordPattern_fields, new_fields);
      return res;
    }
#ifdef VDMPP
    case TAG_TYPE_STKM_ObjectPattern: {
      const SEQ<TYPE_STKM_FieldPattern> & fields (pat.GetSequence (pos_STKM_ObjectPattern_fields));
      size_t len_fields = fields.Length();
      SEQ<TYPE_STKM_FieldPattern> new_fields;
      for (size_t i = 1; i <= len_fields; i++) {
        const TYPE_STKM_FieldPattern & fp (fields[i]);
        TYPE_STKM_FieldPattern new_fp (fp);
        new_fp.SetField(pos_AS_FieldPattern_pat, ConvertPattern(fp.GetRecord(pos_AS_FieldPattern_pat), tm));
        new_fields.ImpAppend(new_fp);
      }
      TYPE_STKM_ObjectPattern res (pat);
      res.SetField(pos_STKM_ObjectPattern_fields, new_fields);
      return res;
    }
#endif // VDMPP
    case TAG_TYPE_STKM_MatchVal: {
      return pat;
    }
    default: {
      return pat;
    }
  }
}

#ifdef VDMSL
// IsSubTypeName
// val_v : SEM`VAL
// tp : AS`TypeName
// checkinv : bool
// ==> bool
bool EvalState::IsSubTypeName (const TYPE_SEM_VAL & val_v, const TYPE_AS_TypeName & tp, bool checkinv)
{
  TYPE_AS_Name name (tp.GetRecord(pos_AS_TypeName_name));
  Tuple t (AUX::LookUpRename(name));
  if (t.GetBoolValue(1)) {
    name = t.GetRecord(2);
  }

//  Tuple itd (AUX::IsTypeDef(name)); //bool * [AS`Type] * [AS`Invariant] * [AS`Equal] * [AS`Order] * 
  Tuple itd (GetCachedTypeDef(name)); //bool * [AS`Type] * [AS`Invariant] * [AS`Equal] * [AS`Order] * 
  if (!itd.GetBoolValue(1)) {
    RTERR::Error (L"IsSubTypeName", RTERR_TYPE_UNKNOWN, Nil(), tp, Sequence());
  }

  bool modulepushed = false; // We only push a new module if name is qualified.
  if (AUX::IsRealName (name)) {
    theStackMachine().PushModule (ExtractModule(name));
    modulepushed = true;
  }

  if (RealSubType (val_v, itd.GetRecord(2), checkinv)) {
    if ( checkinv && !itd.GetField(3).IsNil()) {
      const TYPE_AS_Invariant & Inv (itd.GetRecord(3));

      if( !this->dtc_typenm_l.IsEmpty() && this->dtc_typenm_l.Hd() == tp ) {
        this->dtc_typenm_l.Clear();
        RTERR::ReportError(L"IsSubTypeName",
                           RTERR_CIRCULAR_TYPE_INV_DEPENDENCY,
                           Nil(), tp, Inv.GetInt(pos_AS_Invariant_cid), Sequence());
      }

      SET<TYPE_SEM_BlkEnv> env_s (PAT::PatternMatch(theCompiler().P2P(Inv.GetRecord(pos_AS_Invariant_pat)), val_v)); // one

      if (!env_s.IsEmpty()) {
        TYPE_CI_ContextId lastcid (theStackMachine().GetCurCid());

        this->dtc_typenm_l.ImpPrepend(tp);

        theStackMachine().PushBlkEnv(env_s.GetElem());

// 20090306 -->
// cache compiled invariant
//        Tuple res (theStackMachine().EvalUninterruptedCmd(  // EvalAuxCmd
//                                   Inv.GetRecord(pos_AS_Invariant_expr),
//                                   TYPE_STKM_SubProgram(),
//                                   TYPE_STKM_SubProgram(),
//                                   SEQ<Char>(L"Invariant check for type")
//                                     .ImpConc(AUX::SingleNameToString(name))));

        wstring debugStr (L"Invariant check for type");
        if (Settings.CallLog()) {
            wstring typenm = ASTAUX::ASName2String(name);
            std::string::size_type n = typenm.find(L"`");
            while (n != std::string::npos) {
                typenm[n] = L'-';
                n = typenm.find(L"`");
            }
            debugStr += L" (" + typenm + L")";
        }

        Tuple res (theStackMachine().EvalAuxProgram(GetCachedTypeInv(ExtractModule(name), AUX::ExtractName(name), Inv),
                                                    SEQ<Char>(debugStr),
                                                    false));
        theStackMachine().PopBlkEnv();

        this->dtc_typenm_l.ImpTl();

        theStackMachine().SetCid(lastcid);

        if (modulepushed) {
          theStackMachine().PopModule();
        }

        const TYPE_STKM_EvaluationState & eval_state (res.GetRecord(1));
        if (eval_state.Is(TAG_TYPE_STKM_Success)) {
          const TYPE_SEM_VAL & Inv_v (res.GetRecord(2));
          if (Inv_v.Is(TAG_TYPE_SEM_BOOL)) {
            return Inv_v.GetBoolValue(pos_SEM_BOOL_v);
          }
          else {
            RTERR::Error(L"IsSubTypeName", RTERR_BOOL_EXPECTED, Inv_v, Nil(), Sequence());
          }
        }
        else {
          RTERR::Error(L"IsSubTypeName", RTERR_INTERNAL_ERROR, Nil(), Nil(), Sequence());
        }
      } // (!env_s.IsEmpty())
      else
        RTERR::Error(L"IsSubTypeName", RTERR_EMPTY_ENV_S, val_v, tp, Sequence());
    } // (checkinv && !td.GetField(pos_AS_TypeDef_Inv).IsNil())
    else {
      if (modulepushed)
        theStackMachine().PopModule ();
      return true;
    }
  }
  else {
    if (modulepushed)
      theStackMachine().PopModule ();
    return false;
  }

  return false; // Dummy return
}
#endif //VDMSL

#ifdef VDMPP
// IsSubTypeName
// val_v : SEM`VAL
// tp_r : AS`TypeName
// checkinv : bool
// ==> bool
bool EvalState::IsSubTypeName(const TYPE_SEM_VAL & val_v, const TYPE_AS_TypeName & tp_r, bool checkinv)
{
  const TYPE_AS_Name & tp (tp_r.GetRecord(pos_AS_TypeName_name));

//  Tuple itd (AUX::IsTypeDef(tp)); // bool * [GLOBAL`Type] * [AS`Invariant] * [AS`Equal] * [AS`Order] * [AS`Name] * [AS`Access];
  Tuple itd (GetCachedTypeDef(tp)); // bool * [GLOBAL`Type] * [AS`Invariant] * [AS`Equal] * [AS`Order] * [AS`Name] * [AS`Access];
  if (!itd.GetBoolValue(1)) {
    const Generic & access (itd.GetField(7)); // [AS`Access]
    if (access.IsNil())
      RTERR::Error(L"IsSubTypeName", RTERR_TYPE_UNKNOWN, Nil(), tp_r, Sequence());
    else
      RTERR::Error(L"IsSubTypeName", RTERR_TYPE_NOT_IN_SCOPE, Nil(), tp_r, Sequence());
    return false; // dummy
  }
  else {
    const Generic & typedef1 (itd.GetField(2));   // [GLOBAL`Type]
    const Generic & Inv (itd.GetField(3));        // [AS`Invariant]
    if (! RealSubType(val_v, typedef1, checkinv )) {
      return false;
    }
    else if (checkinv && !Inv.IsNil() ) {
      const TYPE_AS_Name & defcl (itd.GetRecord(6)); // [AS`Name]
      TYPE_AS_Invariant inv (Inv);

      if( !this->dtc_typenm_l.IsEmpty() && (this->dtc_typenm_l.Hd() == tp_r) ) {
        this->dtc_typenm_l.Clear();
        RTERR::ReportError(L"IsSubTypeName",
                           RTERR_CIRCULAR_TYPE_INV_DEPENDENCY,
                           Nil(), tp_r, inv.GetInt(pos_AS_Invariant_cid), Sequence());
      }

      theStackMachine().PushClNmCurObj(defcl, defcl);
      SET<TYPE_SEM_BlkEnv> env_s (PAT::PatternMatch(theCompiler().P2P(inv.GetRecord(pos_AS_Invariant_pat)), val_v)); // one
      theStackMachine().PopClNmCurObj();

      if (!env_s.IsEmpty()) {
        TYPE_CI_ContextId lastcid (theStackMachine().GetCurCid());

        this->dtc_typenm_l.ImpPrepend(tp_r);

        theStackMachine().PushBlkEnv(env_s.GetElem());
        theStackMachine().PushClNmCurObj(defcl, defcl);

// 20090304 -->
// cache compiled invariant
//        TYPE_STKM_SubProgram instr_pre, instr_post;
//        instr_pre.ImpAppend(TYPE_INSTRTP_NEWPOSABSOBJ().Init(defcl));
//        instr_post.ImpAppend(TYPE_INSTRTP_NEWCOMPL().Init(Bool(false)));
//        Tuple res (theStackMachine().EvalUninterruptedCmd(
//                                       inv.GetRecord(pos_AS_Invariant_expr),
//                                       instr_pre,
//                                       instr_post,
//                                       SEQ<Char>(L"Dynamic Type Check")));
//        theStackMachine().Pop(); // pop object reference

        wstring debugStr (L"Invariant check for type");
        //wstring debugStr (L"Dynamic Type Check");
        if (Settings.CallLog())
        {
            wstring typenm = ASTAUX::ASName2String(tp);
            std::string::size_type n = typenm.find(L"`");
            while (n != std::string::npos) {
                typenm[n] = L'-';
                n = typenm.find(L"`");
            }
            debugStr += L" (" + typenm + L")";
            //debugStr += L" (" + typenm + L" " + VAL2X::val2asc(val_v) + L")";
        }
        Tuple res (theStackMachine().EvalAuxProgram(GetCachedTypeInv(defcl, AUX::ExtractName(tp), inv),
                                                    SEQ<Char>(debugStr),
                                                    false));
        theStackMachine().PopClNmCurObj();
        theStackMachine().PopBlkEnv();

        this->dtc_typenm_l.ImpTl();

        theStackMachine().SetCid(lastcid);

        const TYPE_STKM_EvaluationState & eval_state (res.GetRecord(1));
        if (eval_state.Is(TAG_TYPE_STKM_Success)) {
          const TYPE_SEM_VAL & Inv_v (res.GetRecord(2));
          if (Inv_v.Is(TAG_TYPE_SEM_BOOL)) {
            return Inv_v.GetBoolValue(pos_SEM_BOOL_v);
          }
          else {
            RTERR::Error(L"IsSubTypeName", RTERR_BOOL_EXPECTED, Inv_v, Nil(), Sequence());
          }
        }
        else {
          RTERR::Error(L"IsSubTypeName", RTERR_INTERNAL_ERROR, Nil(), Nil(), Sequence());
        }
      }
      else {
        RTERR::Error(L"IsSubTypeName", RTERR_EMPTY_ENV_S, val_v, tp_r, Sequence());
      }
    }
    else
      return true;
  }
  return false; // dummy
}
#endif //VDMPP

// GetCachedTypeInv (not in spec)
// clmod : AS`Name
// name : AS`Name
// int : AS`Invariant
// ==> STKM`SubProgram
TYPE_STKM_SubProgram EvalState::GetCachedTypeInv(const TYPE_AS_Name & clmod,
                                                 const TYPE_AS_Name & name,
                                                 const TYPE_AS_Invariant & inv)
{
  TYPE_AS_Name fullnm (ASTAUX::Combine2Names(clmod, name));
  if (this->typeInvCache.DomExists(fullnm)) {
    return this->typeInvCache[fullnm];
  }
  else {
#ifdef VICE
    bool old_compiling = theCompiler().GetCompilingTime();
    theCompiler().SetCompilingTime(false);
#endif //VICE
    TYPE_STKM_SubProgram sp (theCompiler().E2I(inv.GetRecord(pos_AS_Invariant_expr)));
#ifdef VICE
    theCompiler().SetCompilingTime(old_compiling);
#endif //VICE
    this->typeInvCache.ImpModify(fullnm, sp);
    return sp;
  } 
}

#ifdef VDMSL
// GetCachedTypeDef (not in spec)
// name : AS`Name
// ==> bool * [AS`Type] * [AS`Invariant]
Tuple EvalState::GetCachedTypeDef(const TYPE_AS_Name & name)
{
  TYPE_AS_Name key (name);

  if (!AUX::IsRealName(name)) {
    key = ASTAUX::Combine2Names(theStackMachine().CurrentModule(), name);
  }

  if (!this->typeDefCache.DomExists(key)) {
    this->typeDefCache.ImpModify(key, AUX::IsTypeDef(name));
  }
  return this->typeDefCache[key];
}
#endif // VDMSL
#ifdef VDMPP
// GetCachedTypeDef (not in spec)
// name : AS`Name
// ==> bool * [GLOBAL`Type] * [AS`Invariant] * [AS`Equal] * [AS`Order] * [AS`Name] * [AS`Access]
Tuple EvalState::GetCachedTypeDef(const TYPE_AS_Name & name)
{
  if (IsAClass(name)) {
    if (!this->typeDefCache.DomExists(name)) {
      this->typeDefCache.ImpModify(name, AUX::IsTypeDef(name));
    }
    return Tuple(this->typeDefCache[name]);
  }
  else {
    const TYPE_AS_Ids & ids (name.GetSequence(pos_AS_Name_ids));
    if ((ids.Length() == 1) && !theStackMachine().HasCurCl()) {
      RTERR::Error(L"IsTypeDef", RTERR_TYPE_UNKNOWN, Nil(), Nil(), Sequence());
      return mk_(Bool(false), Nil(), Nil(), Nil(), Nil(), Nil(), Nil());
    }

    Tuple key (mk_(AUX::ExtractName(name),
                   ((ids.Length() == 1) ? TYPE_AS_Name(theStackMachine().GetCurCl())
                                        : ASTAUX::GetFirstName(name)),
                   theStackMachine().GetOrigCl()));
    if (!this->typeDefCache.DomExists(key)) {
      this->typeDefCache.ImpModify(key, AUX::IsTypeDef(name));
    }
    return Tuple(this->typeDefCache[key]);
  }
}
#endif // VDMPP

Map SubTypeCache;

// SubType
// val_v : SEM`VAL
// gtp : GLOBAL`Type
// ==> bool
bool EvalState::SubType (const TYPE_SEM_VAL & val_v, const TYPE_GLOBAL_Type & gtp)
{
  //if (!Settings.DTC() && !this->isTypeJudgement)
  if (!Settings.DTC()) {
    return true;
  }
  else {
    return RealSubType(val_v, gtp, Settings.INV());
  }
}

// SetTypeJudgement
void EvalState::SetTypeJudgement()
{
  this->isTypeJudgement = true;
}

// UnsetTypeJudgement
void EvalState::UnsetTypeJudgement()
{
  this->isTypeJudgement = false;
}

// RealSubType
// val_v : SEM`VAL
// gtp : GLOBAL`Type
// checkinv : bool
// ==> bool
bool EvalState::RealSubType(const TYPE_SEM_VAL & val_v, const TYPE_GLOBAL_Type & gtp, bool checkinv) /*notconst*/
{
  if (val_v.Is(TAG_TYPE_SEM_EXIT) || val_v.Is(TAG_TYPE_SEM_UNDEF)) {
    return true;
  }

  TYPE_GLOBAL_Type tp (gtp);

  switch (tp.GetTag()) {
    case TAG_TYPE_AS_TypeName: { return IsSubTypeName(val_v, tp, checkinv); }
    case TAG_TYPE_AS_BracketedType: { return RealSubType (val_v, tp.GetRecord(pos_AS_BracketedType_tp), checkinv); }
    case TAG_TYPE_AS_OptionalType: {
      return (val_v.Is(TAG_TYPE_SEM_NIL) ? true : RealSubType (val_v, tp.GetRecord(pos_AS_OptionalType_tp), checkinv));
    }
    case TAG_TYPE_AS_UnionType: {
      const SEQ<TYPE_AS_Type> & tp_l (tp.GetSequence(pos_AS_UnionType_tps));
      bool exists = false;
      size_t len_tp_l = tp_l.Length();
      for (size_t i = 1; (i <= len_tp_l) && !exists; i++) {
        exists = RealSubType (val_v, tp_l[i], checkinv);
      }
      return exists;
    }
    case TAG_TYPE_AS_VoidType: { return val_v.Is(TAG_TYPE_SEM_CONT) || val_v.Is(TAG_TYPE_SEM_RETURN); }
    case TAG_TYPE_AS_TokenType: { return val_v.Is(TAG_TYPE_SEM_TOKEN); }
    case TAG_TYPE_AS_AllType: { return true; }
    case TAG_TYPE_AS_BooleanType: { return val_v.Is(TAG_TYPE_SEM_BOOL); }
    case TAG_TYPE_AS_NumericType: {
      if (val_v.Is(TAG_TYPE_SEM_NUM)) {
        switch (tp.GetIntValue(pos_AS_NumericType_qtp)) {
          case NAT:     { return val_v.GetReal(pos_SEM_NUM_v).IsNat(); }
          case NATONE:  { return val_v.GetReal(pos_SEM_NUM_v).IsNatOne(); }
          case INTEGER: { return val_v.GetReal(pos_SEM_NUM_v).IsInt(); }
          case REAL:    { return true; }
          case RAT:     { return true; }
          default:      { return false; } // should never happen
        }
      }
      else {
        return false;
      }
    }
    case TAG_TYPE_AS_CharType: { return val_v.Is(TAG_TYPE_SEM_CHAR); }

    case TAG_TYPE_AS_QuoteType: {
      if (val_v.Is(TAG_TYPE_SEM_QUOTE)) {
        const type_cL & tmp1_l (tp.GetRecord(pos_AS_QuoteType_lit).GetSequence(pos_AS_QuoteLit_val));
        const type_cL & tmp2_l (val_v.GetSequence(pos_SEM_QUOTE_v));
        return (tmp1_l == tmp2_l);
      }
      else {
        return false;
      }
    }
    case TAG_TYPE_AS_CompositeType: {
      if (val_v.Is(TAG_TYPE_DYNSEM_SEM_REC)) {
        const TYPE_AS_Name & tag (tp.GetRecord(pos_AS_CompositeType_name));
        const TYPE_AS_Name & rtag (val_v.GetRecord(pos_DYNSEM_SEM_SemRecord_tag));
//        Tuple etn (AUX::ExtractTagName(tag, Set())); // [AS`Name] * bool
        Tuple etn (ExtractTagNameCached(tag)); // [AS`Name] * bool
//        Tuple the_etn (AUX::ExtractTagName(rtag, Set())); // [AS`Name] * bool
        Tuple the_etn (ExtractTagNameCached(rtag)); // [AS`Name] * bool

        if (etn.GetBoolValue(2) && the_etn.GetBoolValue(2)) {
          const TYPE_AS_Name & the_tag (etn.GetRecord(1));
          const TYPE_AS_Name & the_rtag (the_etn.GetRecord(1));
          bool forall = (the_tag == the_rtag);
          if (!val_v.GetBoolValue(pos_DYNSEM_SEM_SemRecord_checked)) {
            const SEQ<TYPE_AS_Field> & fields_l (tp.GetSequence(pos_AS_CompositeType_fields));
            SEQ<TYPE_SEM_VAL> val_rec (val_v.GetRecord(pos_DYNSEM_SEM_SemRecord_value).GetFields());
            size_t len_val_rec = val_rec.Length();
            forall = (len_val_rec == (size_t)(fields_l.Length()));
            for (size_t i = 1; (i <= len_val_rec) && forall; i++) {
              forall = RealSubType(val_rec[i], fields_l[i].GetRecord(pos_AS_Field_type), checkinv);
            }
          }
          return forall;
        }
        else {
          RTERR::Error(L"RealSubType", RTERR_TAG_UNKNOWN, Nil(), Nil(), Sequence());
          return false;
        }
      }
      else {
        return false;
      }
    }
    case TAG_TYPE_AS_ProductType: {
      if (val_v.Is(TAG_TYPE_SEM_TUPLE)) {
        const SEQ<TYPE_AS_Type> & tp_l (tp.GetSequence(pos_AS_ProductType_tps));
        const SEQ<TYPE_SEM_VAL> & v_tup (val_v.GetSequence(pos_SEM_TUPLE_v));
        size_t len_tp_l = tp_l.Length ();
        //bool forall = ((v_tup.Length () >= 2) && (len_tp_l >= 2) && ((size_t)(v_tup.Length ()) == len_tp_l));
        bool forall = ((size_t)(v_tup.Length ()) == len_tp_l);
        for (size_t i = 1; (i <= len_tp_l) && forall; i++) {
          forall = RealSubType (v_tup[i], tp_l[i], checkinv);
        }
        return forall;
      }
      else {
        return false;
      }
    }
    case TAG_TYPE_AS_Set0Type:
    case TAG_TYPE_AS_Set1Type: {
      if (val_v.Is(TAG_TYPE_SEM_SET)) {
        const TYPE_AS_Type & ttp (tp.GetRecord(pos_AS_Set1Type_elemtp));
        SET<TYPE_SEM_VAL> val_sv (val_v.GetSet(pos_SEM_SET_v));
        bool forall = tp.Is(TAG_TYPE_AS_Set0Type) || !val_sv.IsEmpty();
        Generic elm;
        for (bool bb = val_sv.First (elm); bb && forall; bb = val_sv.Next (elm)) {
          forall = RealSubType (elm, ttp, checkinv);
        }
        return forall;
      }
      else {
        return false;
      }
    }
    case TAG_TYPE_AS_Seq0Type:
    case TAG_TYPE_AS_Seq1Type: {
      if (val_v.Is(TAG_TYPE_SEM_SEQ)) {
        const TYPE_AS_Type & ttp (tp.GetRecord(pos_AS_Seq1Type_elemtp));
        const SEQ<TYPE_SEM_VAL> & v_seq (val_v.GetSequence(pos_SEM_SEQ_v));

        bool forall = tp.Is(TAG_TYPE_AS_Seq0Type) || !v_seq.IsEmpty();
        size_t len_v_seq = v_seq.Length();
        for (size_t i = 1; (i <= len_v_seq) && forall; i++) {
          forall = RealSubType (v_seq[i], ttp, checkinv);
        }
        return forall;
      }
      else {
        return false;
      }
    }
    case TAG_TYPE_AS_GeneralMap0Type:
    case TAG_TYPE_AS_GeneralMap1Type: {
      if (val_v.Is(TAG_TYPE_SEM_MAP)) {
        const MAP<TYPE_SEM_VAL, TYPE_SEM_VAL> & val (val_v.GetMap(pos_SEM_MAP_v));
        const TYPE_AS_Type & dtp (tp.GetRecord(pos_AS_GeneralMap1Type_mapdom));
        const TYPE_AS_Type & rtp (tp.GetRecord(pos_AS_GeneralMap1Type_maprng));
        SET<TYPE_SEM_VAL> dom_val (val.Dom ());

        bool forall = tp.Is(TAG_TYPE_AS_GeneralMap0Type) || !val.IsEmpty();
        Generic elm;
        for (bool bb = dom_val.First (elm); bb && forall; bb = dom_val.Next (elm)) {
          forall = RealSubType (elm, dtp, checkinv) && RealSubType (val[elm], rtp, checkinv);
        }
        return forall;
      }
      else
        return false;
    }
    case TAG_TYPE_AS_InjectiveMap0Type:
    case TAG_TYPE_AS_InjectiveMap1Type: {
      if (val_v.Is(TAG_TYPE_SEM_MAP)) {
        const MAP<TYPE_SEM_VAL, TYPE_SEM_VAL> & val (val_v.GetMap(pos_SEM_MAP_v));
        const TYPE_AS_Type & dtp (tp.GetRecord(pos_AS_InjectiveMap1Type_mapdom));
        const TYPE_AS_Type & rtp (tp.GetRecord(pos_AS_InjectiveMap1Type_maprng));
        SET<TYPE_SEM_VAL> dom_val (val.Dom ());

        bool forall = (tp.Is(TAG_TYPE_AS_InjectiveMap0Type) || !val.IsEmpty() )
                                 && (dom_val.Card () == val.Rng ().Card ());
        Generic dval_v;
        for (bool bb = dom_val.First (dval_v); bb && forall; bb = dom_val.Next (dval_v)) {
          forall = RealSubType (dval_v, dtp, checkinv) && RealSubType (val[dval_v], rtp, checkinv);
        }
        return forall;
      }
      else
        return false;
    }
#ifdef VDMPP
    case TAG_TYPE_GLOBAL_ObjRefType: {
      if (val_v.Is(TAG_TYPE_SEM_OBJ_uRef)) {
        const TYPE_AS_Name & nm (tp.GetRecord(pos_GLOBAL_ObjRefType_nm));
        const TYPE_AS_Name & nm_v (val_v.GetRecord(pos_SEM_OBJ_uRef_tp));
        return ((nm_v == nm) || IsSubClass(nm_v, nm));
      }
      else {
        return false;
      }
    }
#endif // VDMPP
    case TAG_TYPE_AS_PartialFnType:
    case TAG_TYPE_AS_TotalFnType: {
      switch (val_v.GetTag()) {
        case TAG_TYPE_SEM_CompExplFN: {
          const SEQ<TYPE_SEM_ExplFN> & fl (val_v.GetSequence(pos_SEM_CompExplFN_fl));
          const TYPE_SEM_ExplFN & ffn (fl[1]);
          TYPE_AS_Type ftp (theStackMachine().UpdateTypeInfo(ffn.GetRecord(pos_SEM_ExplFN_tp),
                                                             ffn.GetRecord(pos_SEM_ExplFN_modName)));
          if (fl.Length() > 1) {
            const TYPE_SEM_ExplFN & lfn (fl[fl.Length()]);
            TYPE_AS_Type ltp (theStackMachine().UpdateTypeInfo(lfn.GetRecord(pos_SEM_ExplFN_tp),
                                                               lfn.GetRecord(pos_SEM_ExplFN_modName)));
            switch (ftp.GetTag()) {
              case TAG_TYPE_AS_PartialFnType: {
                switch (ltp.GetTag()) {
                  case TAG_TYPE_AS_PartialFnType: {
                    ftp.SetField(pos_AS_PartialFnType_fnrng, ltp.GetRecord(pos_AS_PartialFnType_fnrng));
                    break;
                  }
                  case TAG_TYPE_AS_TotalFnType: {
                    ftp.SetField(pos_AS_PartialFnType_fnrng, ltp.GetRecord(pos_AS_TotalFnType_fnrng));
                    break;
                  }
                }
                break;
              }
              case TAG_TYPE_AS_TotalFnType: {
                switch (ltp.GetTag()) {
                  case TAG_TYPE_AS_PartialFnType: {
                    ftp.SetField(pos_AS_TotalFnType_fnrng, ltp.GetRecord(pos_AS_PartialFnType_fnrng));
                    break;
                  }
                  case TAG_TYPE_AS_TotalFnType: {
                    ftp.SetField(pos_AS_TotalFnType_fnrng, ltp.GetRecord(pos_AS_TotalFnType_fnrng));
                    break;
                  }
                }
                break;
              }
            }
          }
          return SubTypeAS(ftp, tp);
        }
        case TAG_TYPE_SEM_ExplPOLY: {
          //const TYPE_AS_Type & ptp (val_v.GetRecord(pos_SEM_ExplPOLY_tp));
          TYPE_AS_Type ptp (theStackMachine().UpdateTypeInfo(val_v.GetRecord(pos_SEM_ExplPOLY_tp),
                                                             val_v.GetRecord(pos_SEM_ExplPOLY_modName)));
          return SubTypeAS(ptp, tp);
        }
        default: {
          return false;
        }
      }
    }
    case TAG_TYPE_AS_OpType: {
      switch (val_v.GetTag()) {
        case TAG_TYPE_SEM_ExplOP: {
          TYPE_AS_Type otp (theStackMachine().UpdateTypeInfo(val_v.GetRecord(pos_SEM_ExplOP_tp),
                                                             val_v.GetRecord(pos_SEM_ExplOP_modName)));
          return SubTypeAS(otp, tp);
        }
        default: {
          return false;
        }
      }
      break;
    }
    case TAG_TYPE_AS_TypeVar: {
      Map tm (theStackMachine().HdTypeInst());
      return (tm.DomExists(tp) ? RealSubType (val_v, tm[tp], checkinv) : false);
    }
    default: {
      RTERR::Error (L"RealSubType", RTERR_TYPE_UNKNOWN, Nil(), tp, Sequence());
      return false;
    }
  }
//  return false;//to avoid warnings from gcc;
}

// SubTypeAS
// ltp : GLOBAL`Type
// rtp : GLOBAL`Type
// ==> bool
bool EvalState::SubTypeAS(const TYPE_GLOBAL_Type & ltp, const TYPE_GLOBAL_Type & rtp)
{
//wcout << theStackMachine().GetCurCl() << endl;
//wcout << L"SubTypeAS: " << INT2Q::h2gAS(ltp) << L" : " << INT2Q::h2gAS(rtp) << endl;
  if (ltp == rtp) {
    return true;
  }
  else {
    switch (ltp.GetTag()) {
      case TAG_TYPE_AS_BracketedType: {
        return SubTypeAS (ltp.GetRecord(pos_AS_BracketedType_tp), rtp);
      }
      case TAG_TYPE_AS_UnionType: {
        const SEQ<TYPE_AS_Type> & ltp_l (ltp.GetSequence(pos_AS_UnionType_tps));
        bool forall = true;
        size_t len_ltp_l = ltp_l.Length();
        for (size_t i = 1; (i <= len_ltp_l) && forall; i++) {
          forall = SubTypeAS (ltp_l[i], rtp);
        }
        return forall;
      }
      case TAG_TYPE_AS_TypeVar: {
        Map tm (theStackMachine().HdTypeInst());
        return (tm.DomExists(ltp) ? SubType (tm[ltp], rtp) : false);
      }
      case TAG_TYPE_AS_TypeName: {
#ifdef VDMSL
        TYPE_AS_Name name (ltp.GetRecord(pos_AS_TypeName_name));
        Tuple t (AUX::LookUpRename(name));
        if (t.GetBoolValue(1)) {
          name = t.GetRecord(2);
        }

//        Tuple itd (AUX::IsTypeDef(name)); //bool * [AS`Type] * [AS`Invariant] * [AS`Equal] * [AS`Order] * 
        Tuple itd (GetCachedTypeDef(name)); //bool * [AS`Type] * [AS`Invariant] * [AS`Equal] * [AS`Order] * 
        if (!itd.GetBoolValue(1)) {
          RTERR::Error (L"SubTypeAS", RTERR_TYPE_UNKNOWN, Nil(), ltp, Sequence());
        }
        return SubTypeAS (itd.GetRecord(2), rtp);
#endif // VDMSL
#ifdef VDMPP
        const TYPE_AS_Name & tp (ltp.GetRecord(pos_AS_TypeName_name));

//        Tuple itd (AUX::IsTypeDef(tp)); // bool * [GLOBAL`Type] * [AS`Invariant] * [AS`Equal] * [AS`Order] * [AS`Name] * [AS`Access];
        Tuple itd (GetCachedTypeDef(tp)); // bool * [GLOBAL`Type] * [AS`Invariant] * [AS`Equal] * [AS`Order] * [AS`Name] * [AS`Access];

        if (!itd.GetBoolValue(1)) {
          const Generic & access (itd.GetField(7)); // [AS`Access]
          if (access.IsNil()) {
            RTERR::Error(L"SubTypeAS", RTERR_TYPE_UNKNOWN, Nil(), ltp, Sequence());
          }
          else {
            RTERR::Error(L"SubTypeAS", RTERR_TYPE_NOT_IN_SCOPE, Nil(), ltp, Sequence());
          }
          return false; // dummy
        }
        else {
          const Generic & typedef1 (itd.GetField(2));   // [GLOBAL`Type]
          //const TYPE_AS_Name & defcl (itd.GetRecord(6)); // [AS`Name]
          //const Generic & Inv (itd.GetField(3));        // [AS`Invariant]
          return SubTypeAS(typedef1, rtp);
        }
#endif // VDMPP
      }
    }
    switch (rtp.GetTag()) {
      case TAG_TYPE_AS_TypeName: {
#ifdef VDMSL
        TYPE_AS_Name name (rtp.GetRecord(pos_AS_TypeName_name));
        Tuple t (AUX::LookUpRename(name));
        if (t.GetBoolValue(1)) {
          name = t.GetRecord(2);
        }
//        Tuple itd (AUX::IsTypeDef(name)); //bool * [AS`Type] * [AS`Invariant] * [AS`Equal] * [AS`Order] * 
        Tuple itd (GetCachedTypeDef(name)); //bool * [AS`Type] * [AS`Invariant] * [AS`Equal] * [AS`Order] * 
        if (!itd.GetBoolValue(1)) {
          RTERR::Error (L"SubTypeAS", RTERR_TYPE_UNKNOWN, Nil(), rtp, Sequence());
        }
        return SubTypeAS (ltp, itd.GetRecord(2));
#endif // VDMSL
#ifdef VDMPP
        const TYPE_AS_Name & tp (rtp.GetRecord(pos_AS_TypeName_name));

//        Tuple itd (AUX::IsTypeDef(tp)); // bool * [GLOBAL`Type] * [AS`Invariant] * [AS`Equal] * [AS`Order] * [AS`Name] * [AS`Access];
        Tuple itd (GetCachedTypeDef(tp)); // bool * [GLOBAL`Type] * [AS`Invariant] * [AS`Equal] * [AS`Order] * [AS`Name] * [AS`Access];
        if (!itd.GetBoolValue(1)) {
          const Generic & access (itd.GetField(7)); // [AS`Access]
          if (access.IsNil()) {
            RTERR::Error(L"SubTypeAS", RTERR_TYPE_UNKNOWN, Nil(), rtp, Sequence());
          }
          else {
            RTERR::Error(L"SubTypeAS", RTERR_TYPE_NOT_IN_SCOPE, Nil(), rtp, Sequence());
          }
          return false; // dummy
        }
        else {
          const Generic & typedef1 (itd.GetField(2));   // [GLOBAL`Type]
          //const TYPE_AS_Name & defcl (itd.GetRecord(6)); // [AS`Name]
          //const Generic & Inv (itd.GetField(3));        // [AS`Invariant]
          return SubTypeAS(ltp, typedef1);
        }
#endif // VDMPP
        break;
      }
      case TAG_TYPE_AS_BracketedType: {
        return SubTypeAS (ltp, rtp.GetRecord(pos_AS_BracketedType_tp));
        break;
      }
      case TAG_TYPE_AS_NumericType: {
        if (ltp.Is(TAG_TYPE_AS_NumericType)) {
          int lttp (ltp.GetIntValue(pos_AS_NumericType_qtp)); 
          int rttp (rtp.GetIntValue(pos_AS_NumericType_qtp)); 
          switch (rttp) {
            case NATONE:  { return (lttp == NATONE); }
            case NAT:     { return (lttp == NATONE) || (lttp == NAT); }
            case INTEGER: { return (lttp == NATONE) || (lttp == NAT) || (lttp == INTEGER); }
            case RAT:     { return (lttp == NATONE) || (lttp == NAT) || (lttp == INTEGER) ||
                                   (lttp == RAT) ; }
            case REAL:    { return (lttp == NATONE) || (lttp == NAT) || (lttp == INTEGER) ||
                                   (lttp == RAT) || (lttp == REAL); }
            default:      { return false; } // should never happen
          }
        }
        else {
          return false;
        }
        break;
      }
      case TAG_TYPE_AS_CompositeType: {
        if (ltp.Is(TAG_TYPE_AS_CompositeType)) {
          const TYPE_AS_Name & rtag (ltp.GetRecord(pos_AS_CompositeType_name));
          const TYPE_AS_Name & tag (rtp.GetRecord(pos_AS_CompositeType_name));
//          Tuple etn (AUX::ExtractTagName(tag, Set())); // [AS`Name] * bool
          Tuple etn (ExtractTagNameCached(tag)); // [AS`Name] * bool
//          Tuple the_etn (AUX::ExtractTagName(rtag, Set())); // [AS`Name] * bool
          Tuple the_etn (ExtractTagNameCached(rtag)); // [AS`Name] * bool

          if (etn.GetBoolValue(2) && the_etn.GetBoolValue(2)) {
            const TYPE_AS_Name & the_tag (etn.GetRecord(1));
            const TYPE_AS_Name & the_rtag (the_etn.GetRecord(1));
            return (the_tag == the_rtag);
          }
          else {
            RTERR::Error(L"SubTypeAS", RTERR_TAG_UNKNOWN, Nil(), Nil(), Sequence());
            return false;
          }
        }
        else {
          return false;
        }
        break;
      }
      case TAG_TYPE_AS_UnionType: {
        const SEQ<TYPE_AS_Type> & rtp_l (rtp.GetSequence(pos_AS_UnionType_tps));
        bool exists = false;
        size_t len_rtp_l = rtp_l.Length();
        for (size_t i = 1; (i <= len_rtp_l) && !exists; i++) {
          exists = SubTypeAS (ltp, rtp_l[i]);
        }
        return exists;
        break;
      }
      case TAG_TYPE_AS_ProductType: {
        if (ltp.Is(TAG_TYPE_AS_ProductType)) {
          const SEQ<TYPE_AS_Type> & ltp_l (ltp.GetSequence(pos_AS_ProductType_tps));
          const SEQ<TYPE_AS_Type> & rtp_l (rtp.GetSequence(pos_AS_ProductType_tps));
          size_t len_ltp_l = ltp_l.Length ();
          bool forall = ((size_t)(rtp_l.Length ()) == len_ltp_l);
          for (size_t i = 1; (i <= len_ltp_l) && forall; i++) {
            forall = SubTypeAS (ltp_l[i], rtp_l[i]);
          }
          return forall;
        }
        else {
          return false;
        }
        break;
      }
      case TAG_TYPE_AS_OptionalType: {
        return SubTypeAS (ltp, rtp.GetRecord(pos_AS_OptionalType_tp));
        break;
      }
      case TAG_TYPE_AS_Set0Type: {
        switch(ltp.GetTag()) {
          case TAG_TYPE_AS_Set0Type: {
            return SubTypeAS(ltp.GetRecord(pos_AS_Set0Type_elemtp),
                             rtp.GetRecord(pos_AS_Set0Type_elemtp));
          }
          case TAG_TYPE_AS_Set1Type: {
            return SubTypeAS(ltp.GetRecord(pos_AS_Set1Type_elemtp),
                             rtp.GetRecord(pos_AS_Set0Type_elemtp));
          }
          default: {
            return false;
          }
        }
        break;
      }
      case TAG_TYPE_AS_Set1Type: {
        switch(ltp.GetTag()) {
          case TAG_TYPE_AS_Set1Type: {
            return SubTypeAS(ltp.GetRecord(pos_AS_Set1Type_elemtp),
                             rtp.GetRecord(pos_AS_Set1Type_elemtp));
          }
          default: {
            return false;
          }
        }
        break;
      }
      case TAG_TYPE_AS_Seq0Type: {
        switch(ltp.GetTag()) {
          case TAG_TYPE_AS_Seq0Type: {
            return SubTypeAS(ltp.GetRecord(pos_AS_Seq0Type_elemtp),
                             rtp.GetRecord(pos_AS_Seq0Type_elemtp));
          }
          case TAG_TYPE_AS_Seq1Type: {
            return SubTypeAS(ltp.GetRecord(pos_AS_Seq1Type_elemtp),
                             rtp.GetRecord(pos_AS_Seq0Type_elemtp));
          }
          default: {
            return false;
          }
        }
        break;
      }
      case TAG_TYPE_AS_Seq1Type: {
        switch(ltp.GetTag()) {
          case TAG_TYPE_AS_Seq1Type: {
            return SubTypeAS(ltp.GetRecord(pos_AS_Seq1Type_elemtp),
                             rtp.GetRecord(pos_AS_Seq1Type_elemtp));
          }
          default: {
            return false;
          }
        }
        break;
      }
      case TAG_TYPE_AS_GeneralMap0Type: {
        switch (ltp.GetTag()) {
          case TAG_TYPE_AS_GeneralMap0Type: {
            return SubTypeAS(ltp.GetRecord(pos_AS_GeneralMap0Type_mapdom),
                             rtp.GetRecord(pos_AS_GeneralMap0Type_mapdom)) &&
                   SubTypeAS(ltp.GetRecord(pos_AS_GeneralMap0Type_maprng),
                             rtp.GetRecord(pos_AS_GeneralMap0Type_maprng));
          }
          case TAG_TYPE_AS_GeneralMap1Type: {
            return SubTypeAS(ltp.GetRecord(pos_AS_GeneralMap1Type_mapdom),
                             rtp.GetRecord(pos_AS_GeneralMap0Type_mapdom)) &&
                   SubTypeAS(ltp.GetRecord(pos_AS_GeneralMap1Type_maprng),
                             rtp.GetRecord(pos_AS_GeneralMap0Type_maprng));
          }
          case TAG_TYPE_AS_InjectiveMap0Type: {
            return SubTypeAS(ltp.GetRecord(pos_AS_InjectiveMap0Type_mapdom),
                             rtp.GetRecord(pos_AS_GeneralMap0Type_mapdom)) &&
                   SubTypeAS(ltp.GetRecord(pos_AS_InjectiveMap0Type_maprng),
                             rtp.GetRecord(pos_AS_GeneralMap0Type_maprng));
          }
          case  TAG_TYPE_AS_InjectiveMap1Type: {
            return SubTypeAS(ltp.GetRecord(pos_AS_InjectiveMap1Type_mapdom),
                             rtp.GetRecord(pos_AS_GeneralMap0Type_mapdom)) &&
                   SubTypeAS(ltp.GetRecord(pos_AS_InjectiveMap1Type_maprng),
                             rtp.GetRecord(pos_AS_GeneralMap0Type_maprng));
          }
          default: {
            return false;
          }
        }
        break;
      }
      case TAG_TYPE_AS_GeneralMap1Type: {
        switch (ltp.GetTag()) {
          case TAG_TYPE_AS_GeneralMap1Type: {
            return SubTypeAS(ltp.GetRecord(pos_AS_GeneralMap1Type_mapdom),
                             rtp.GetRecord(pos_AS_GeneralMap1Type_mapdom)) &&
                   SubTypeAS(ltp.GetRecord(pos_AS_GeneralMap1Type_maprng),
                             rtp.GetRecord(pos_AS_GeneralMap1Type_maprng));
          }
          case TAG_TYPE_AS_InjectiveMap1Type: {
            return SubTypeAS(ltp.GetRecord(pos_AS_InjectiveMap1Type_mapdom),
                             rtp.GetRecord(pos_AS_GeneralMap1Type_mapdom)) &&
                   SubTypeAS(ltp.GetRecord(pos_AS_InjectiveMap1Type_maprng),
                             rtp.GetRecord(pos_AS_GeneralMap1Type_maprng));
          }
          default: {
            return false;
          }
        }
        break;
      }
      case TAG_TYPE_AS_InjectiveMap0Type: {
        switch (ltp.GetTag()) {
          case TAG_TYPE_AS_InjectiveMap0Type: {
            return SubTypeAS(ltp.GetRecord(pos_AS_InjectiveMap0Type_mapdom),
                             rtp.GetRecord(pos_AS_InjectiveMap0Type_mapdom)) &&
                   SubTypeAS(ltp.GetRecord(pos_AS_InjectiveMap0Type_maprng),
                             rtp.GetRecord(pos_AS_InjectiveMap0Type_maprng));
          }
          case TAG_TYPE_AS_InjectiveMap1Type: {
            return SubTypeAS(ltp.GetRecord(pos_AS_InjectiveMap1Type_mapdom),
                             rtp.GetRecord(pos_AS_InjectiveMap0Type_mapdom)) &&
                   SubTypeAS(ltp.GetRecord(pos_AS_InjectiveMap1Type_maprng),
                             rtp.GetRecord(pos_AS_InjectiveMap0Type_maprng));
          }
          default: {
             return false;
          }
        }
        break;
      }
      case TAG_TYPE_AS_InjectiveMap1Type: {
        switch (ltp.GetTag()) {
          case TAG_TYPE_AS_InjectiveMap1Type: {
            return SubTypeAS(ltp.GetRecord(pos_AS_InjectiveMap1Type_mapdom),
                             rtp.GetRecord(pos_AS_InjectiveMap0Type_mapdom)) &&
                   SubTypeAS(ltp.GetRecord(pos_AS_InjectiveMap1Type_maprng),
                             rtp.GetRecord(pos_AS_InjectiveMap0Type_maprng));
          }
          default: {
            return false;
          }
        }
        break;
      }
      case TAG_TYPE_AS_PartialFnType: {
        switch (ltp.GetTag()) {
          case TAG_TYPE_AS_PartialFnType: {
            const SEQ<TYPE_AS_Type> & ldtp (ltp.GetSequence(pos_AS_PartialFnType_fndom));
            const SEQ<TYPE_AS_Type> & rdtp (rtp.GetSequence(pos_AS_PartialFnType_fndom));
            bool forall = (ldtp.Length() == rdtp.Length()) &&
                   SubTypeAS(ltp.GetRecord(pos_AS_PartialFnType_fnrng),
                             rtp.GetRecord(pos_AS_PartialFnType_fnrng));
            size_t len_ldtp = ldtp.Length();
            for (size_t i = 1; (i <= len_ldtp) && forall; i++) {
              forall = SubTypeAS (ldtp[i], rdtp[i]);
            }
            return forall;
          }
          case TAG_TYPE_AS_TotalFnType: {
            const SEQ<TYPE_AS_Type> & ldtp (ltp.GetSequence(pos_AS_TotalFnType_fndom));
            const SEQ<TYPE_AS_Type> & rdtp (rtp.GetSequence(pos_AS_PartialFnType_fndom));
            bool forall = (ldtp.Length() == rdtp.Length()) &&
                  (ltp.GetRecord(pos_AS_TotalFnType_fnrng).Is(TAG_TYPE_AS_AllType) || // LabmdaExpr
                   SubTypeAS(ltp.GetRecord(pos_AS_TotalFnType_fnrng),
                             rtp.GetRecord(pos_AS_PartialFnType_fnrng)));
            size_t len_ldtp = ldtp.Length();
            for (size_t i = 1; (i <= len_ldtp) && forall; i++) {
              forall = SubTypeAS (ldtp[i], rdtp[i]);
            }
            return forall;
          }
          default: {
            return false;
          }
        }
        break;
      }
      case TAG_TYPE_AS_TotalFnType: {
        switch (ltp.GetTag()) {
          case TAG_TYPE_AS_TotalFnType: {
            const SEQ<TYPE_AS_Type> & ldtp (ltp.GetSequence(pos_AS_TotalFnType_fndom));
            const SEQ<TYPE_AS_Type> & rdtp (rtp.GetSequence(pos_AS_TotalFnType_fndom));
            bool forall = (ldtp.Length() == rdtp.Length()) &&
                  (ltp.GetRecord(pos_AS_TotalFnType_fnrng).Is(TAG_TYPE_AS_AllType) || // LabmdaExpr
                   SubTypeAS(ltp.GetRecord(pos_AS_TotalFnType_fnrng),
                             rtp.GetRecord(pos_AS_TotalFnType_fnrng)));
            size_t len_ldtp = ldtp.Length();
            for (size_t i = 1; (i <= len_ldtp) && forall; i++) {
              forall = SubTypeAS (ldtp[i], rdtp[i]);
            }
            return forall;
          }
          default: {
            return false;
          }
        }
        break;
      }
      case TAG_TYPE_AS_OpType: {
        switch (ltp.GetTag()) {
          case TAG_TYPE_AS_OpType: {
            const SEQ<TYPE_AS_Type> & ldtp (ltp.GetSequence(pos_AS_OpType_opdom));
            const SEQ<TYPE_AS_Type> & rdtp (rtp.GetSequence(pos_AS_OpType_opdom));
            bool forall = (ldtp.Length() == rdtp.Length()) &&
                           SubTypeAS(ltp.GetRecord(pos_AS_OpType_oprng),
                                     rtp.GetRecord(pos_AS_OpType_oprng));
            size_t len_ldtp = ldtp.Length();
            for (size_t i = 1; (i <= len_ldtp) && forall; i++) {
              forall = SubTypeAS (ldtp[i], rdtp[i]);
            }
            return forall;
          }
          default: {
            return false;
          }
        }
        break;
      }
      case TAG_TYPE_AS_TypeVar: {
        Map tm (theStackMachine().HdTypeInst());
        return (tm.DomExists(rtp) ? SubType (ltp, tm[rtp]) : false);
      }
#ifdef VDMPP
      case TAG_TYPE_GLOBAL_ObjRefType: {
        if (ltp.Is(TAG_TYPE_GLOBAL_ObjRefType)) {
          const TYPE_AS_Name & nm (rtp.GetRecord(pos_GLOBAL_ObjRefType_nm));
          const TYPE_AS_Name & lnm (ltp.GetRecord(pos_GLOBAL_ObjRefType_nm));
          return ((lnm == nm) || IsSubClass(lnm, nm));
        }
        else {
          return false;
        }
      }
#endif // VDMPP
      case TAG_TYPE_AS_AllType: { return true; }
      default: {
        return false;
        break;
      }
    }
  }
}

// GetCachedRecSel (not in spec)
// name : AS`Name
// ==> bool * [GLOBAL`RecSel]
Tuple EvalState::GetCachedRecSel(const TYPE_AS_Name & name)
{
  if (!this->recSelCache.DomExists(name)) {
    this->recSelCache.ImpModify(name, AUX::LookUpRecSel(name));
  } 
  return this->recSelCache[name];
}

#ifdef VDMSL
// GetCachedRenRecSel (not in spec)
// name : AS`Name
// ==> bool * [(GLOBAL`RecSel * AS`Name * AS`Name)]
Tuple EvalState::GetCachedRenRecSel(const TYPE_AS_Name & name)
{
  if (!this->renRecSelCache.DomExists(name)) {
    this->renRecSelCache.ImpModify(name, AUX::LookUpRenRecSel(name));
  } 
  return this->renRecSelCache[name];
}

// GetCachedFunOpPolyVal (not in spec)
// name : AS`Name
// ==> bool * [SEM`VAL]
Tuple EvalState::GetCachedFunOpPolyVal(const TYPE_AS_Name & name, const TYPE_AS_Name & modnm)
{
  Tuple key (mk_(name, modnm));
  if (!this->funOpPolyCache.DomExists(key)) {
    if (EXPR::IsFunction(name, modnm)) {
      this->funOpPolyCache.ImpModify(key, mk_(Bool(true), EXPR::GetFunctionVal(name, modnm)));
    }
    else if (EXPR::IsOperation(name, modnm)) {
      this->funOpPolyCache.ImpModify(key, mk_(Bool(true), EXPR::GetOperationVal(name, modnm)));
    }
    else if (EXPR::IsPoly(name, modnm)) {
      this->funOpPolyCache.ImpModify(key, mk_(Bool(true), EXPR::GetPolyVal(name, modnm)));
    }
    else {
      this->funOpPolyCache.ImpModify(key, mk_(Bool(false), Nil()));
    }
  }
  return this->funOpPolyCache[key];
}

// GetCachedGlobalVal (not in spec)
// name : AS`Name
// ==> bool * [SEM`VAL]
Tuple EvalState::GetCachedGlobalVal(const TYPE_AS_Name & name, const TYPE_AS_Name & modnm)
{
  Tuple key (mk_(name, modnm));
  if (!this->globalValCache.DomExists(key)) {
    this->globalValCache.ImpModify(key, IsGlobalVal(name, modnm));
  }
  return this->globalValCache[key];
}

// LookUp
// name : AS`Name
// ==> SEM`ValTp
TYPE_SEM_ValTp EvalState::LookUp (const TYPE_AS_Name & name)
{
//wcout << L"LookUp: " << ASTAUX::ASName2String(name) << endl;
  if ((name.GetSequence(pos_AS_Name_ids).Length() == 1) && !theStackMachine().IsEmptyEnvL ()) {
    Tuple ilv (theStackMachine().IsLocalVal(name));
    if (ilv.GetBoolValue(1)) {
      const TYPE_SEM_VAL & res_v (Record(ilv.GetRecord(2)).GetRecord(pos_SEM_ValTp_val));
      if (res_v.Is(TAG_TYPE_SEM_UNDEF)) {
        return RTERR::ErrorValTp (L"LookUp", RTERR_UNDEF_ENCOUNTERED,
                             M42Sem(AUX::SingleNameToString(name), NULL), Nil(), Sequence());
      }
      else {
        return ilv.GetRecord(2);
      }
    }
  }

  if (theStackMachine().CurrentModule().GetSequence(pos_AS_Name_ids).IsEmpty()) {
    return RTERR::ErrorValTp (L"LookUp", RTERR_ID_UNKNOWN,
                         M42Sem(AUX::SingleNameToString(name), NULL), Nil(), Sequence());
  }

  TYPE_AS_Name loc_nm (AUX::ExtractName (name));

  TYPE_AS_Name mod_nm (AUX::IsRealName(name) ? ExtractModule(name)
                                             : theStackMachine().CurrentModule());

  if (AUX::IsRealName(name)) {
    switch(GetAnyModule(mod_nm).GetTag()) {
      case TAG_TYPE_GLOBAL_SigmaMO: {
        if (!AUX::IsConstructExported (loc_nm, mod_nm)) {
          return RTERR::ErrorValTp (L"LookUp", RTERR_CONSTRUCT_NOT_EXPORTED,
                               M42Sem(AUX::SingleNameToString(name), NULL), Nil(), Sequence());
        }
        // Do notthing.
        break;
      }
      case TAG_TYPE_GLOBAL_SigmaIMO: {
        if (!AUX::IsInDLDeclared(loc_nm, mod_nm)) {
          return RTERR::ErrorValTp (L"LookUp", RTERR_LIB_NOT_DECLARED,
                               M42Sem(AUX::SingleNameToString(name), NULL), Nil(), Sequence());
        }
        Tuple idlfo (IsDLFnOP(loc_nm, mod_nm));
        if( idlfo.GetBoolValue(1) ) {
          return TYPE_SEM_ValTp().Init(idlfo.GetRecord(2), Nil());
        }
        Tuple idlv (IsDLVal (loc_nm, mod_nm));
        if( idlv.GetBoolValue(1) ) {
          return TYPE_SEM_ValTp().Init(idlv.GetRecord(2), Nil());
        }
        return RTERR::ErrorValTp (L"LookUp", RTERR_MOD_NOT_DEFINED,
                             M42Sem(AUX::SingleNameToString(name), NULL), Nil(), Sequence());
        break;
      }
      default: {
        return RTERR::ErrorValTp (L"LookUp", RTERR_MOD_NOT_DEFINED,
                             M42Sem(AUX::SingleNameToString(name), NULL), Nil(), Sequence());
        break;
      }
    }
  }

//  if (EXPR::IsFunction (loc_nm, mod_nm))
//    return EXPR::GetFunctionVal (loc_nm, mod_nm);
//  if(EXPR::IsOperation (loc_nm, mod_nm))
//    return EXPR::GetOperationVal (loc_nm, mod_nm);
//  if(EXPR::IsPoly (loc_nm, mod_nm))
//    return EXPR::GetPolyVal (loc_nm, mod_nm);
  Tuple gfopv (GetCachedFunOpPolyVal (loc_nm, mod_nm));
  if (gfopv.GetBoolValue(1)) {
    return TYPE_SEM_ValTp().Init(gfopv.GetRecord(2), Nil());
  }

  //Tuple igv (IsGlobalVal(loc_nm, mod_nm));
  Tuple igv (GetCachedGlobalVal(loc_nm, mod_nm));
  if ( igv.GetBoolValue(1) ) {
    return TYPE_SEM_ValTp().Init(igv.GetRecord(2), igv.GetRecord(3));
  }

  if (theStackMachine().HdContext() != Int(AS_PURE)) {
    Tuple igs (IsGlobalState (loc_nm, mod_nm)); // bool * [GLOBAL`State]
    if (igs.GetBoolValue(1)) {
      return TYPE_SEM_ValTp().Init(igs.GetRecord(2).GetRecord(pos_GLOBAL_State_val),  // SEM`VAL
                                   igs.GetRecord(2).GetRecord(pos_GLOBAL_State_tp)); // AS`Type
    }

    if (IsTheState(name, mod_nm)) {
      return TYPE_SEM_ValTp().Init(GetTheState(name, mod_nm), Nil());
    }
  }

  Tuple t (AUX::LookUpRename(name));
  if (t.GetBoolValue(1)) {
    return LookUp(t.GetRecord(2));
  }
  return RTERR::ErrorValTp (L"LookUp", RTERR_ID_UNKNOWN,
                          M42Sem(AUX::SingleNameToString(loc_nm), NULL), Nil(), Sequence());
}
#endif //VDMSL

#ifdef VDMPP
// LookUp
// name : AS`Name
// ==> SEM`ValTp
TYPE_SEM_ValTp EvalState::LookUp(const TYPE_AS_Name & name_)
{
  TYPE_AS_Name name = name_;
  if (name.get_ids().Length() == 2) {
    TYPE_AS_Name clsnm (ASTAUX::GetFirstName(name));
    TYPE_AS_Name memnm (ASTAUX::GetSecondName(name));
    Tuple t (ExpandClassName(clsnm, theStackMachine().GetCurCl(), Set()));
    if (t.GetBoolValue(1)) {
      name = AUX::ConstructDoubleName(t.GetRecord(2), memnm);
    }
  }

  Tuple ilv (theStackMachine().IsLocalVal(name));
  if (ilv.GetBoolValue(1)) {
    return ReturnLookUp(Record(ilv.GetRecord(2)).GetRecord(pos_SEM_ValTp_val),
                        Record(ilv.GetRecord(2)).GetRecord(pos_SEM_ValTp_tp), RTERR_INTERNAL_ERROR);
  }
  else {
    if (!theStackMachine().HasCurObjRef()) {
      Tuple iso (IsDObjs(name));
      const Bool & isit_d_objs (iso.GetBool(1));
      const Generic & val_obj (iso.GetField(2));

      if (isit_d_objs) {
        return ReturnLookUp(val_obj, Nil(), RTERR_INTERNAL_ERROR);
      }
      else {
        // bool * [SEM`VAL] * *[AS`Type] * [As`Name] * [AS`Access]
        Tuple lus (LookUpStatic(name));
        const Generic & staticval (lus.GetField(2));
        if (staticval.IsNil()) {
          Generic cl = Nil();
          TYPE_AS_Name nm;
          if (name.get_ids().Length() == 2) {
            cl = ASTAUX::GetFirstName(name);
            nm = ASTAUX::GetSecondName(name);
          }
          else if(!this->initclstack.IsEmpty()) {
            cl = this->initclstack.Hd();
            nm = name;
          }
          if (!cl.IsNil()) {
            if(this->classes.DomExists(cl)) {
              TYPE_GLOBAL_ValueMap vls_init (this->classes[cl].GetMap(pos_GLOBAL_SigmaClass_vls_uinit));
              if(vls_init.DomExists(nm)) {
                if (AccessOk(vls_init[nm].GetRecord(3), theStackMachine().GetCurCl(), cl)) {
                  return TYPE_SEM_ValTp().Init(vls_init[nm].GetRecord(1),vls_init[nm].GetField(2));
                }
              }  

              if (this->classes[cl].GetMap(pos_GLOBAL_SigmaClass_instvars_utp).DomExists(nm)) {
                // not static
                vdm_iplog << ASTAUX::ASName2String(cl) << L"`" << ASTAUX::ASName2String(nm)
                          << L" must be defined as \'static\'." << endl;
              }
            }
          }
          return RTERR::ErrorValTp (L"LookUp", RTERR_ID_UNKNOWN,
                              M42Sem(AUX::SingleNameToString(name), NULL), Nil(), Sequence());
        }
        else {
          //return staticval;
          return TYPE_SEM_ValTp().Init(staticval, lus.GetField(3));
        }
      }
    }
    else {
      Tuple lofp (LookOpFctPoly(name));
      const Bool & isit_opfct (lofp.GetBool(1));
//      const Bool & local_opfct (lofp.GetBool(2));
      const Generic & val_opfct (lofp.GetField(3));

      if (isit_opfct) {
        return ReturnLookUp(val_opfct, Nil(), RTERR_OP_OR_FUN_NOT_IN_SCOPE);
      }

      // bool * [SEM`VAL] * *[AS`Type] * [As`Name] * [AS`Access]
      Tuple lus (LookUpStatic(name));
      const Bool & isstatic (lus.GetBool(1));
      const Generic & staticval (lus.GetField(2));

      if (theStackMachine().HdContext() != Int(AS_PURE)) {
        // ==> bool * bool * [SEM`VAL] * [AS`Type] * [AS`Name] * [AS`Access]
        Tuple iios (IsInObjScope(name, Nil()));
        const Bool & isit_inst (iios.GetBool(1));     // bool
        const Bool & local_inst (iios.GetBool(2));    // bool
        const Generic & val_inst (iios.GetField(3));  // [SEM`VAL]
//      Generic cl_scope (iios.GetField(5));          // [AS`Name]
//      TYPE_AS_Access access (iios.GetField(6));     // [AS`Access]

        if (isit_inst && (local_inst || !isstatic)) {
          return ReturnLookUp(val_inst, Nil(), RTERR_INSTVAR_NOT_IN_SCOPE);
        }
      }

      if (isstatic) {
        if (staticval.IsNil()) {
          return RTERR::ErrorValTp(L"LookUp", RTERR_STATIC_NOT_IN_SCOPE,
                              M42Sem(AUX::SingleNameToString(name), NULL), Nil(), Sequence());
        }
        else {
          return TYPE_SEM_ValTp().Init(staticval, lus.GetField(3));
        }
      }

//      Tuple iv (IsValue(name));
      Tuple iv (CachedIsValue(name));
      const Bool & isit_val (iv.GetBool(1));
      //const Bool & local_val (iv.GetBool(2));
      const Generic & val_val (iv.GetField(3));

      if (isit_val) {
        return ReturnLookUp(val_val, iv.GetField(4), RTERR_VAL_NOT_IN_SCOPE);
      }

      if (theStackMachine().HdContext() != Int(AS_PURE)) {
        Tuple iso (IsDObjs(name));
        const Bool & isit_d_objs (iso.GetBool(1));
        const Generic & val_obj (iso.GetField(2));

        if (isit_d_objs) {
          return ReturnLookUp(val_obj, Nil(), RTERR_INTERNAL_ERROR);
        }
      }

      return RTERR::ErrorValTp(L"LookUp", RTERR_NAME_UNKNOWN,
                          M42Sem(AUX::SingleNameToString(name), NULL), Nil(), Sequence());
    }
  }
  return TYPE_SEM_VAL(); // Dummy return.
}

// IsInObjScope
// name : AS`Name
// oldstate : [SEM`OBJ]
// ==> bool * bool * [SEM`VAL] * [AS`Type] * [AS`Name] * [AS`Access]
Tuple EvalState::IsInObjScope(const TYPE_AS_Name& name, const Generic & oldstate)
{
//  if (! theStackMachine().HasCurCl()) {
  if (! theStackMachine().HasCurObjRef()) { // for user pushed class
    return mk_(Bool(false), Bool(false), Nil(), Nil(), Nil(), Nil());
  }

  const TYPE_AS_Name & origcl (theStackMachine().GetOrigCl());
  TYPE_AS_Name thename (AUX::ExtractName(name));

  bool exists_l (name.GetSequence(pos_AS_Name_ids).Length() == 2);
  TYPE_AS_Name clnm (exists_l ? ASTAUX::GetFirstName(name)
                              : TYPE_AS_Name(theStackMachine().GetCurCl()));

// 20090311 -->
  if (oldstate.IsNil()) {
    Map istrct (theStackMachine().GetCurObj().GetMap(pos_SEM_OBJ_ins)); // map AS`Name to GLOBAL`ValueMap
    if (istrct.DomExists(clnm)) {
      TYPE_GLOBAL_ValueMap vm (istrct[clnm]); // map AS`Name to (SEM`VAL * AS`Access)
      if (vm.DomExists(thename)) {
        Tuple t (vm[thename]); // SEM`VAL * AS`Type * AS`Access
        const TYPE_AS_Type & tp (t.GetRecord(2)); // AS`Type
        const TYPE_AS_Access & access (t.GetField(3)); // AS`Access

        if ((origcl == clnm) && !IsStatic(clnm, thename)) {
          return mk_(Bool(true), Bool(true), t.GetRecord(1), tp, clnm, access);
        }
        else if (AccessOk(access, origcl, clnm)) {
          return mk_(Bool(true), Bool(false), t.GetRecord(1), tp, clnm, access);
        }
        else {
          return mk_(Bool(true), Bool(false), Nil(), tp, clnm, access);
        }
      }
    }
  }
// <-- 20090311

  //  isstatic = thename in set dom classes(clnm).statics
  //             and let mk_(isopfct,-,-) = LookStaticOpFctPoly(clnm,thename)
  //                 in not isopfct ,

  TYPE_SEM_OBJ obj; // SEM`OBJ
  if (oldstate.IsNil()) {
    obj = theStackMachine().GetCurObj();
  }
  else {
    obj = oldstate;
  }

  Map istrct (obj.GetMap(pos_SEM_OBJ_ins)); // map AS`Name to GLOBAL`ValueMap
// 20090311
//  if (!istrct.DomExists(clnm) && !isstatic) {
  if (!istrct.DomExists(clnm)) {
    return mk_(Bool(false), Bool(false), Nil(), Nil(), Nil(), Nil());
  }

//  if (!exists_l || isstatic)
  if (!exists_l) {
    Map local_inst (istrct[clnm]);
     
    if (local_inst.DomExists(thename)) {
      Tuple t (local_inst[thename]); // SEM`VAL * AS`Type * AS`Access
      const TYPE_AS_Type & tp (t.GetRecord(2)); // AS`Type
      const TYPE_AS_Access & access (t.GetField(3)); // AS`Access
      if (AccessOk(access, origcl, clnm)) {
        return mk_(Bool(true), Bool(true), t.GetRecord(1), tp, clnm, access);
      }
      else {
        return mk_(Bool(true), Bool(false), Nil(), tp, clnm, access);
      }
    }
  }

  // search instance variable
  TYPE_AS_Name classname (exists_l ? ASTAUX::GetFirstName(name) : clnm);

  if (! istrct.DomExists(classname)) {
    return mk_(Bool(false), Bool(false), Nil(), Nil(), Nil(), Nil());
  }

  // Search super classes
  SET<TYPE_AS_Name> allsupers (GetAllSupers(classname));
  allsupers.Insert(classname);

  //  inst_vls = { clname |-> istrct(clname) |
  //               clname in set allsupers &
  //               thename in set dom istrct(clname)}
  MAP<TYPE_AS_Name, Map> inst_vls; // map AS`Name to (map AS`Name to (SEM`VAL * AS`Access))
  Generic clname;
  for (bool hh = allsupers.First(clname); hh; hh = allsupers.Next(clname)) {
    TYPE_GLOBAL_ValueMap vm (istrct[clname]); // map AS`Name to (SEM`VAL * AS`Access)
    if (vm.DomExists(thename)) {
      inst_vls.ImpModify(clname, vm);
    }
  }

  switch(inst_vls.Size()) {
    case 0: { // {} -> return mk_(fa ....
      return mk_(Bool(false), Bool(false), Nil(), Nil(), Nil(), Nil());
    }
    case 1: { //  {cl} -> let mk_(va .....
      TYPE_AS_Name cl (inst_vls.Dom().GetElem());
      Tuple t (inst_vls[cl][thename]); // SEM`VAL * AS`Type * AS`Access
      const TYPE_AS_Type & tp (t.GetRecord(2)); // AS`Type
      const TYPE_AS_Access & access (t.GetField(3));     // AS`Access
      if (AccessOk(access, origcl, cl)) {
        return mk_(Bool(true), Bool(false), t.GetRecord(1), tp, cl, access);
      }
      else {
        return mk_(Bool(true), Bool(false), Nil(), tp, cl, access);
      }
    }
    default: { //  - -> if classname in set dom inst_vls
      TYPE_AS_Name cls;
      bool doesthere = false;
      if ( inst_vls.DomExists(classname) ) {
        cls = classname;
        doesthere = true;
      }
      else {
        SET<TYPE_AS_Name> dom_inst_vls (inst_vls.Dom());
        SET<TYPE_AS_Name> rem_priv;
        Generic clnm;
        for (bool cc = dom_inst_vls.First(clnm); cc; cc = dom_inst_vls.Next(clnm)) {
          TYPE_GLOBAL_ValueMap vm (inst_vls[clnm]);
          Set rng (vm.Rng());
          bool forall = true;
          Generic p;
          for (bool dd = rng.First(p); dd && forall; dd = rng.Next(p)) {
            const TYPE_AS_Access & acc (Tuple(p).GetField(3));
            forall = ((acc != Int(PRIVATE_AS)) && (acc != Int(NOT_INITIALISED_AS))); // 20080908
          }
          if (forall) {
            rem_priv.Insert(clnm);
          }
        }
        Tuple dc (ExistsOneChild (rem_priv, dom_inst_vls));
        if (dc.GetBoolValue(1)) {
          cls = dc.GetRecord(2);
          doesthere = true;
        }
      }
      if (doesthere) {
        Tuple t (inst_vls[cls][thename]); // SEM`VAL * AS`Type * AS`Access
        const TYPE_AS_Type & tp (t.GetRecord(2)); // AS`Type
        const TYPE_AS_Access & access (t.GetField(3));   // AS`Access
        if (AccessOk(access, origcl, classname) ) {
          return mk_(Bool(true), Bool(false), t.GetRecord(1), tp, cls, access);
        }
        else {
          return mk_(Bool(true), Bool(false), Nil(), tp, cls, access);
        }
      }
      else {
        RTERR::Error(L"IsInObjScope", RTERR_MULT_DEF,
                            M42Sem(AUX::SingleNameToString(name), NULL), Nil(), Sequence());
        return mk_(Bool(false), Bool(false), Nil(), Nil(), Nil(), Nil());
      }
      break;
    }
  }
  return Tuple();  // dummy. to keep compiler silent.
}

// ExistsOneChild
// cl_s : set of AS`Name
// full_s : set of AS`Name
// ==> bool * [AS`Name]
Tuple EvalState::ExistsOneChild(const SET<TYPE_AS_Name> & cl_s_, const SET<TYPE_AS_Name> & full_s) const
{
  SET<TYPE_AS_Name> cl_s (cl_s_);
  SET<TYPE_AS_Name> all_priv_cls;
  if (!full_s.IsEmpty())
  {
    Set priv_cls (full_s);
    priv_cls.ImpDiff(cl_s);
    Generic priv;
    for (bool bb = priv_cls.First(priv); bb; bb = priv_cls.Next(priv))
    {
      all_priv_cls.ImpUnion(GetAllSupers(priv));
    }
    cl_s.ImpDiff(all_priv_cls);
  }

  switch(cl_s.Card()) {
    case 0: {
      return mk_(Bool (false), Nil());
    }
    case 1: {
      return mk_(Bool (true), cl_s.GetElem());
    }
    default: {
      Set cl_s_org (cl_s_);
      Generic cl;
      for (bool bb = cl_s.First(cl); bb; bb = cl_s.Next(cl))
      {
        if (cl_s_org.ImpDiff(GetAllSupers(cl)).Card() == 1)
          return mk_(Bool (true), cl);
      }
      return mk_(Bool (false), Nil());
    }
  }
}

// CachedIsValue (not in spec)
// nm : AS`Name
// ==> bool * bool * [SEM`VAL]
Tuple EvalState::CachedIsValue(const TYPE_AS_Name & nm)
{
  TYPE_AS_Name fullnm (nm);
  if (nm.GetSequence(pos_AS_Name_ids).Length() == 1) {
    if (! theStackMachine().HasCurCl()) {
      return mk_(Bool (false), Bool (false), Nil());
    }
    else {
      const TYPE_AS_Name & clnm (theStackMachine().GetCurCl()); // HasCurCl() == true
      fullnm = AUX::ConstructDoubleName(clnm, nm);
    }
  }
  if (!this->isValueCache.DomExists(fullnm)) {
    this->isValueCache.ImpModify(fullnm, IsValue(fullnm));
  }
  return this->isValueCache[fullnm];
}

// IsValue
// nm : AS`Name
// ==> bool * bool * [SEM`VAL] * [AS`Type]
Tuple EvalState::IsValue(const TYPE_AS_Name & nm)
{
  // determine target class name
  TYPE_AS_Name clnm;
  if (nm.GetSequence(pos_AS_Name_ids).Length() == 1) {
    if (! theStackMachine().HasCurCl()) {
      RTERR::Error(L"IsValue", RTERR_ID_UNKNOWN,
                        M42Sem(AUX::SingleNameToString(nm), NULL), Nil(), Sequence());
      return mk_(Bool (false), Bool (false), Nil(), Nil());
    }
    else {
      clnm = (theStackMachine().GetCurCl()); // HasCurCl() == true
    }
  }
  else
  {
    clnm = ASTAUX::GetFirstName(nm);
// 20120905 -->
    if (theStackMachine().HasCurCl()) {
      Tuple t (ExpandClassName(clnm, theStackMachine().GetCurCl(), Set()));
      if (t.GetBoolValue(1)) {
        clnm = t.GetRecord(2);
      }
    }
// <-- 20120905
  }

  TYPE_AS_Name thename (AUX::ExtractName(nm));
  const TYPE_GLOBAL_OrigCl & origcl (theStackMachine().GetOrigCl());

  // search value in local class
  TYPE_GLOBAL_ValueMap local_vls (GetVlsInit(clnm));
  if (local_vls.DomExists(thename)) {
    Tuple t (local_vls[thename]); // (SEM`VAL * [AS`Type] * AS`Access)
    if (AccessOk(t.GetField(3), origcl, clnm)) {
      return mk_(Bool (true), Bool (true), t.GetRecord(1), t.GetField(2));
    }
    else {
      return mk_(Bool (true), Bool (false), Nil(), Nil());
    }
  }

  // search value in super classes
  SET<TYPE_AS_Name> allsupers (GetAllSupers(clnm));

  MAP<TYPE_AS_Name, Map> spcl_vls;
  Generic clname;
  for (bool bb = allsupers.First(clname); bb; bb = allsupers.Next(clname)) {
    if (GetVlsInit(clname).DomExists(thename)) {
      spcl_vls.ImpModify(clname, GetVlsInit(clname));
    }
  }

  //cases dom spcl_vls:
  switch(spcl_vls.Size()) {
    case 0: {
      return mk_(Bool (false), Bool (false), Nil(), Nil());
    }
    case 1: {
      TYPE_AS_Name cl (spcl_vls.Dom().GetElem());
      Tuple t (spcl_vls[cl][thename]);  // SEM`VAL * [AS`Type] * AS`Access
      if (AccessOk(t.GetField(3), origcl, cl))
        return mk_(Bool (true), Bool (true), t.GetField(1), t.GetField(2));
      else
        return mk_(Bool (true), Bool (false), Nil(), Nil());
    }
    default: {
      // - -> if classname in set dom spcl_vls
      SET<TYPE_AS_Name> dom_spcl_vls (spcl_vls.Dom());
      SET<TYPE_AS_Name> rem_priv;
      Generic clnm;
      for (bool cc = dom_spcl_vls.First(clnm); cc; cc = dom_spcl_vls.Next(clnm)) {
        TYPE_GLOBAL_ValueMap vls_init (spcl_vls[clnm]);
        Set rng (vls_init.Rng());
        bool forall = true;
        Generic p;
        for (bool dd = rng.First(p); dd && forall; dd = rng.Next(p)) {
          TYPE_AS_Access acc (Tuple(p).GetField(3));
          forall = ((acc != Int(PRIVATE_AS)) && (acc != Int(NOT_INITIALISED_AS)));  // 20080908
        }
        if (forall) {
          rem_priv.Insert(clnm);
        }
      }
      Tuple eoc (ExistsOneChild(rem_priv, dom_spcl_vls) );
      if (eoc.GetBoolValue(1))
      {
        const TYPE_AS_Name & child (eoc.GetField(2));
        Tuple t (spcl_vls[child][thename]);   // SEM`VAL * [AS`Type] * AS`Access
        if (AccessOk(t.GetField(3), origcl, child))
          return mk_(Bool (true), Bool (true), t.GetField(1), t.GetField(2));
        else
          return mk_(Bool (true), Bool (false), Nil(), Nil());
      }
      else
      {
        RTERR::Error(L"IsValue", RTERR_MULT_DEF,
                            M42Sem(AUX::SingleNameToString(nm), NULL), Nil(), Sequence());
        return mk_(Bool (false), Bool (false), Nil(), Nil());
      }
      break;
    }
  }
  return Tuple(); // dummy
}

//--- 07.06.2001 M.Lebedev  Static Poly functions/operations lookup
// LookOpFctPoly
// name : AS`Name
// ==> bool * bool * [SEM`VAL]
Tuple EvalState::LookOpFctPoly(const TYPE_AS_Name & name)
{
  if (!theStackMachine().HasCurCl())
    return mk_(Bool(false), Bool(false), Nil());

  const TYPE_AS_Name & clnm (theStackMachine().GetCurCl()); // HasCurCl() == true

//  return LookOpFctPoly_(clnm, theStackMachine().GetCurObjName(), name, AUX::ExtractName(name));
  return LookCachedOpFctPoly_(clnm, theStackMachine().GetCurObjName(), name, AUX::ExtractName(name));
}

// LookStaticOpFctPoly
// clnm : AS`Name
// name : AS`Name
// ==> bool * bool * [SEM`VAL]
Tuple EvalState::LookStaticOpFctPoly(const TYPE_AS_Name & clnm, const TYPE_AS_Name & name)
{
// 20160315 -->
//  return LookOpFctPoly_(clnm, clnm, name, name);
  Tuple lcofp (LookCachedOpFctPoly_ (clnm, clnm, name, name));
  if (theStackMachine().HasCurCl()) {
    const TYPE_AS_Name & curclnm (theStackMachine().GetCurCl()); // HasCurCl() == true
    if ( ( curclnm == clnm ) || IsSubClass( curclnm, clnm ) ) {
      return LookCachedOpFctPoly_(clnm, clnm, name, name);
    }
  }
  if (this->classes.DomExists(clnm))
  {
    // search static member in instance variables section LookUp Static Variable
    const TYPE_GLOBAL_SigmaClass & classesClsnm (this->classes[clnm]);
    const TYPE_GLOBAL_ValueMap & statics (classesClsnm.GetMap(pos_GLOBAL_SigmaClass_statics));
    if (statics.DomExists( name )) {
      return LookCachedOpFctPoly_(clnm, clnm, name, name);
    }
    else {
      Tuple lcofp (LookCachedOpFctPoly_ (clnm, clnm, name, name));
      if ( lcofp.GetBoolValue(1) ) {
        if ( lcofp.GetBoolValue(2) ) {
          const TYPE_SEM_VAL & value (lcofp.GetRecord(3));
          if (value.Is(TAG_TYPE_SEM_OverOPFN) ) {
            const type_7AS_NameC7AS_NameC2PdLUU3PM & overload (value.GetMap(pos_SEM_OverOPFN_overload));
            Set dom_overload(overload.Dom());
            type_7AS_NameC7AS_NameC2PdLUU3PM new_overload;
            Generic g;
            for (bool bb = dom_overload.First(g); bb; bb = dom_overload.Next(g)) {
              Tuple t (g);
              const TYPE_AS_Name & mnm (t.GetRecord(1));
              //const TYPE_AS_Name & cnm (t.GetRecord(2));
              if ( statics.DomExists( mnm ) ) {
                new_overload.Insert( t, overload[t] );
              }
            }
            if ( !new_overload.IsEmpty() ) { 
              TYPE_SEM_OverOPFN new_value(value);
              new_value.SetField(pos_SEM_OverOPFN_overload, new_overload);
              return mk_(Bool(true), Bool(true), new_value);
            }
          }
        }
        return mk_(Bool(true), Bool(false), Nil());
      }
    }
  }
  return mk_(Bool(false), Bool(false), Nil());
// <-- 20160315
}

// LookCachedOpFctPoly_ (not in spec)
// clnm : AS`Name
// objnm : AS`Name
// name : AS`Name
// thename : AS`Name
// ==> bool * bool * [SEM`VAL]
Tuple EvalState::LookCachedOpFctPoly_(const TYPE_AS_Name & clnm, const TYPE_AS_Name & objnm,
                                      const TYPE_AS_Name & name, const TYPE_AS_Name & thename)
{
// 20120723 -->
  //Tuple key (mk_(clnm, objnm, name, thename));   
  const TYPE_AS_Name & origcl (theStackMachine().GetOrigCl());
  Tuple key (mk_(clnm, objnm, name, thename, origcl));   
// <-- 20120723
  if (!this->lookupOpFctPolyCache.DomExists(key)) {
    this->lookupOpFctPolyCache.ImpModify(key, LookOpFctPoly_(clnm, objnm, name, thename));
  }
  return this->lookupOpFctPolyCache[key];
}

// LookOpFctPoly_
// clnm : AS`Name
// objnm : AS`Name
// name : AS`Name
// thename : AS`Name
// ==> bool * bool * [SEM`VAL]
Tuple EvalState::LookOpFctPoly_(const TYPE_AS_Name & clnm, const TYPE_AS_Name & objnm,
                                const TYPE_AS_Name & name, const TYPE_AS_Name & thename)
{
//  TYPE_CI_ContextId cid (name.GetInt(pos_AS_Name_cid));
  const TYPE_AS_Name & origcl (theStackMachine().GetOrigCl());

//wcout << L"LookOpFctPoly_" << endl;
//wcout << L"origcl: " << origcl << endl;
//wcout << L"clnm: " << clnm << endl;
//wcout << L"objnm: " << objnm << endl;
//wcout << L"name: " << name << endl;
//wcout << L"thename: " << thename << endl;

  bool exists_l (name.GetSequence(pos_AS_Name_ids).Length() == 2);

  TYPE_AS_Name classname;
  SET<TYPE_AS_Name> allclasses;
  if (!exists_l) {
    classname = objnm;
    allclasses = GetAllSupers(objnm);
    allclasses.Insert(objnm);
  }
  else {
    classname = ASTAUX::GetFirstName(name);
    if ((clnm == classname) || IsSubClass(clnm, classname)) {
      allclasses = GetAllSupers(classname);
      allclasses.Insert(classname);
    }
    else {
      allclasses = SET<TYPE_AS_Name>();
    }
  }

  MAP<TYPE_AS_Name, Tuple> opsfcts;
  Generic clname;
  for (bool bb = allclasses.First(clname); bb; bb = allclasses.Next(clname)) {
    // bool * [ (SEM`CompExplFN | SEM`ExplOP | SEM`ExplPOLY) * AS`Access ]
    Tuple lafop (LookupAllFnsOpsPolys(clname, thename));
    if (lafop.GetBoolValue(1)) {
      opsfcts.ImpModify(clname, lafop.GetTuple(2));
    }
  }

  Generic overopfn (LookUpOverloaded(clnm, name, allclasses));

//wcout << L"allclasses: " << allclasses << endl;
//wcout << L"opsfcts: " << INT2Q::h2gAS(opsfcts) << endl;
//wcout << INT2Q::h2gAS(overopfn) << endl;

  if (overopfn.IsNil()) {
    // cases dom opsfcts :
    switch(opsfcts.Size()) {
      case 0: { // {} -> ...
        return mk_(Bool(false), Bool(false), Nil());
      }
      case 1: { // {cl} -> let mk_(fnval,
        TYPE_AS_Name cl (opsfcts.Dom().GetElem());
        Tuple fnacc (opsfcts[cl]);

        if (AccessOk(fnacc.GetField(2), origcl, cl)) {
          return mk_(Bool(true), Bool(true), fnacc.GetField(1));
        }
        else {
          return mk_(Bool(true), Bool(false), Nil());
        }
      }
      default: { // default -> // opsfcts.Size() > 1
        SET<TYPE_AS_Name> dom_opsfcts (opsfcts.Dom());
        Map m; // map (seq of AS`Type) to (set of AS`Name)
        Generic clnm;
        for (bool cc = dom_opsfcts.First(clnm); cc; cc = dom_opsfcts.Next(clnm)) {
          Tuple t (opsfcts[clnm]); 
          //Sequence tp_l (MANGLE::MethType2Seq(FindType(t.GetField(1))));
          Sequence tp_l (ConvertTypeNameList(MANGLE::MethType2Seq(FindType(t.GetField(1))), Set()));
          const TYPE_AS_Access & acc (t.GetField(2));
          if (AccessOk(acc, origcl, clnm)) {
            Set nm_s;
            if (m.DomExists(tp_l)) {
              nm_s = Set(m[tp_l]);
            }
            nm_s.Insert(clnm);
            m.ImpModify(tp_l, nm_s);
          }
        }
        switch (m.Size()) {
          case 0: {
            return mk_(Bool(true), Bool(false), Nil());
          }
          case 1: {
            Tuple eoc (ExistsOneChild(m.Rng().GetElem(), dom_opsfcts));
            if (eoc.GetBoolValue(1)) {
              const TYPE_AS_Name & child (eoc.GetRecord(2));
              Tuple fnvalacc (opsfcts[child]);
              return mk_(Bool(true), Bool(true), fnvalacc.GetField(1));
            }
            else {
              RTERR::Error(L"IsOpFctPoly", RTERR_MULT_DEF,
                                  M42Sem(AUX::SingleNameToString(thename), NULL), Nil(), Sequence());
              return mk_(Bool(false), Bool(false), Nil()); // dummy
            }
          }
          default: {
            Map newover; // map (AS`Name * AS`Name) to ((seq of AS`Type) * AS`Access * [SEM`CompExplFN])
            SET<TYPE_AS_Name> dom_opsfcts (opsfcts.Dom());
            Generic clnm;
            for (bool cc = dom_opsfcts.First(clnm); cc; cc = dom_opsfcts.Next(clnm)) {
              Tuple t (opsfcts[clnm]); 
              const TYPE_AS_Access & acc (t.GetField(2));
              if (AccessOk(acc, origcl, clnm)) {
                newover.ImpModify(mk_(name, clnm),
                              //mk_(MANGLE::MethType2Seq(FindType(t.GetField(1))), t.GetField(2), Nil()));
                              mk_(ConvertTypeNameList(MANGLE::MethType2Seq(FindType(t.GetField(1))), Set()),
                                  t.GetField(2), Nil()));
              }
            }
            if (newover.IsEmpty()) {
              return mk_(Bool(true), Bool(false), Nil());
            }
            else {
              return mk_(Bool(true), Bool(true), TYPE_SEM_OverOPFN().Init(newover, Nil()));
            }
          }
        }
      }
    }
  }
  else
  { // overopfn <> nil
    Map newover (Record(overopfn).GetMap(1)); // map (AS`Name * AS`Name) to
                                              //   ((seq of AS`Type) * AS`Access * [SEM`CompExplFN])
    if (!opsfcts.IsEmpty())
    {
      SET<TYPE_AS_Name> dom_opsfcts (opsfcts.Dom());
      Generic clnm;
      for (bool cc = dom_opsfcts.First(clnm); cc; cc = dom_opsfcts.Next(clnm))
      {
        Tuple t (opsfcts[clnm]); 
        newover.ImpModify(mk_(name, clnm),
                      //mk_(MANGLE::MethType2Seq(FindType(t.GetField(1))), t.GetField(2), Nil()));
                      mk_(ConvertTypeNameList(MANGLE::MethType2Seq(FindType(t.GetField(1))), Set()),
                          t.GetField(2), Nil()));
      }
    }
    return mk_(Bool(true), Bool(true), TYPE_SEM_OverOPFN().Init(newover, Nil()));
  }
  return Tuple(); // dummy
}

// ConvertTypeNameList
// tp_l : seq of AS`Type
// nm_s : set of AS`Name
// ==> seq of AS`Type
SEQ<TYPE_AS_Type> EvalState::ConvertTypeNameList(const SEQ<TYPE_AS_Type> & tp_l, const SET<TYPE_AS_Name> & nm_s)
{
  SEQ<TYPE_AS_Type> res;
  size_t len_tp_l = tp_l.Length();
  for (size_t idx = 1; idx <= len_tp_l; idx++)
  {
    res.ImpAppend(ConvertTypeName(tp_l[idx], nm_s));
  }
  return res;
}

// ConvertTypeName
// type : AS`Type
// nm_s : set of AS`Name
// ==> AS`Type
TYPE_AS_Type EvalState::ConvertTypeName(const TYPE_AS_Type & type, const SET<TYPE_AS_Name> & nm_s)
{
  switch(type.GetTag()) {
    case TAG_TYPE_AS_TypeName: {
      const TYPE_AS_Name & tn (type.GetRecord(pos_AS_TypeName_name));
      if (IsAClass(tn))
        return type;
      //Tuple itd (AUX::IsTypeDef(tp)); // bool * [GLOBAL`Type] * [AS`Invariant] * [AS`Equal] * [AS`Order] * [AS`Name] * [AS`Access];
      Tuple itd (GetCachedTypeDef(tn)); // bool * [GLOBAL`Type] * [AS`Invariant] * [AS`Equal] * [AS`Order] * [AS`Name] * [AS`Access];
      if (itd.GetBoolValue(1)) {
        const TYPE_AS_Name & clnm (itd.GetRecord(6));
        TYPE_AS_TypeName newtn (type);
        newtn.SetField( pos_AS_TypeName_name, AUX::ConstructDoubleName(clnm, tn));
        return newtn;
      }
      return type;
    }
    case TAG_TYPE_AS_TypeVar: {
        return type;
    }
    case TAG_TYPE_AS_CompositeType: {
      const SEQ<TYPE_AS_Field> & fields (type.GetSequence(pos_AS_CompositeType_fields));
      SEQ<TYPE_AS_Field> new_fls;
      size_t len_fields = fields.Length();
      for (size_t i = 1; i <= len_fields; i++)
      {
        TYPE_AS_Field f (fields[i]);
        f.set_type(ConvertTypeName(f.GetRecord(pos_AS_Field_type), nm_s));
        new_fls.ImpAppend(f);
      }
      return TYPE_AS_CompositeType().Init(type.GetRecord(pos_AS_CompositeType_name),
                                          new_fls,
                                          type.GetInt(pos_AS_CompositeType_cid));
    }

    case TAG_TYPE_AS_UnionType: {
      return TYPE_AS_UnionType().Init(ConvertTypeNameList(type.GetSequence(pos_AS_UnionType_tps), nm_s),
                                      type.GetInt(pos_AS_UnionType_cid));
    }

    case TAG_TYPE_AS_ProductType: {
      return TYPE_AS_ProductType().Init(ConvertTypeNameList(type.GetSequence(pos_AS_ProductType_tps), nm_s),
                                        type.GetInt(pos_AS_ProductType_cid));
    }
    case TAG_TYPE_AS_OptionalType: {
      return TYPE_AS_OptionalType().Init(ConvertTypeName(type.GetRecord(pos_AS_OptionalType_tp), nm_s),
                                         type.GetInt(pos_AS_OptionalType_cid));
    }
    case TAG_TYPE_AS_BracketedType: {
      return TYPE_AS_BracketedType().Init(ConvertTypeName(type.GetRecord(pos_AS_BracketedType_tp), nm_s),
                                          type.GetInt(pos_AS_BracketedType_cid));
    }
    case TAG_TYPE_AS_Set0Type: {
      return TYPE_AS_Set0Type().Init(ConvertTypeName(type.GetRecord(pos_AS_Set0Type_elemtp), nm_s),
                                    type.GetInt(pos_AS_Set0Type_cid));
    }
    case TAG_TYPE_AS_Set1Type: {
      return TYPE_AS_Set1Type().Init(ConvertTypeName(type.GetRecord(pos_AS_Set1Type_elemtp), nm_s),
                                    type.GetInt(pos_AS_Set1Type_cid));
    }
    case TAG_TYPE_AS_Seq0Type: {
      return TYPE_AS_Seq0Type().Init(ConvertTypeName(type.GetRecord(pos_AS_Seq0Type_elemtp), nm_s),
                                     type.GetInt(pos_AS_Seq0Type_cid));
    }
    case TAG_TYPE_AS_Seq1Type: {
      return TYPE_AS_Seq1Type().Init(ConvertTypeName(type.GetRecord(pos_AS_Seq1Type_elemtp), nm_s),
                                     type.GetInt(pos_AS_Seq1Type_cid));
    }
    case TAG_TYPE_AS_GeneralMap0Type: {
      return TYPE_AS_GeneralMap0Type().Init(ConvertTypeName(type.GetRecord(pos_AS_GeneralMap0Type_mapdom), nm_s),
                                           ConvertTypeName(type.GetRecord(pos_AS_GeneralMap0Type_maprng), nm_s),
                                           type.GetInt(pos_AS_GeneralMap0Type_cid));
    }
    case TAG_TYPE_AS_GeneralMap1Type: {
      return TYPE_AS_GeneralMap1Type().Init(ConvertTypeName(type.GetRecord(pos_AS_GeneralMap1Type_mapdom), nm_s),
                                           ConvertTypeName(type.GetRecord(pos_AS_GeneralMap1Type_maprng), nm_s),
                                           type.GetInt(pos_AS_GeneralMap1Type_cid));
    }
    case TAG_TYPE_AS_InjectiveMap0Type: {
      return TYPE_AS_InjectiveMap0Type().Init(ConvertTypeName(type.GetRecord(pos_AS_InjectiveMap0Type_mapdom), nm_s),
                                             ConvertTypeName(type.GetRecord(pos_AS_InjectiveMap0Type_maprng), nm_s),
                                             type.GetInt(pos_AS_InjectiveMap0Type_cid));
    }
    case TAG_TYPE_AS_InjectiveMap1Type: {
      return TYPE_AS_InjectiveMap1Type().Init(ConvertTypeName(type.GetRecord(pos_AS_InjectiveMap1Type_mapdom), nm_s),
                                             ConvertTypeName(type.GetRecord(pos_AS_InjectiveMap1Type_maprng), nm_s),
                                             type.GetInt(pos_AS_InjectiveMap1Type_cid));
    }
    case TAG_TYPE_AS_PartialFnType: {
      return TYPE_AS_PartialFnType().Init(ConvertTypeNameList(type.GetSequence(pos_AS_PartialFnType_fndom), nm_s),
                                          ConvertTypeName(type.GetRecord(pos_AS_PartialFnType_fnrng), nm_s),
                                          type.GetInt(pos_AS_PartialFnType_cid));
    }
    case TAG_TYPE_AS_TotalFnType: {
      return TYPE_AS_TotalFnType().Init(ConvertTypeNameList(type.GetSequence(pos_AS_TotalFnType_fndom), nm_s),
                                        ConvertTypeName(type.GetRecord(pos_AS_TotalFnType_fnrng), nm_s),
                                        type.GetInt(pos_AS_TotalFnType_cid));
    }
    case TAG_TYPE_AS_OpType: {
      return TYPE_AS_OpType().Init(ConvertTypeNameList(type.GetSequence(pos_AS_OpType_opdom), nm_s),
                                   ConvertTypeName(type.GetRecord(pos_AS_OpType_oprng), nm_s),
                                   type.GetInt(pos_AS_OpType_cid));
    }
    default: {
      return type;
      break;
    }
  }
}

// FindType
// fnval : SEM`CompExplFN | SEM`ExplOP | SEM`ExplPOLY
// -> AS`FnType |AS`OpType
TYPE_AS_Type EvalState::FindType(const TYPE_SEM_VAL & fnval) const
{
  switch(fnval.GetTag()) {
    case TAG_TYPE_SEM_CompExplFN: { TYPE_SEM_ExplFN fn (fnval.GetSequence(pos_SEM_CompExplFN_fl).Hd());
                                    return fn.GetField(pos_SEM_ExplFN_tp); }
    case TAG_TYPE_SEM_ExplOP:     { return fnval.GetField(pos_SEM_ExplOP_tp); }
    case TAG_TYPE_SEM_ExplPOLY:   { return fnval.GetField(pos_SEM_ExplPOLY_tp); }
    default:                      { return TYPE_AS_Type(); }
  }
}

// FindTypeDom
// fnval : SEM`CompExplFN | SEM`ExplOP | SEM`ExplPOLY
// -> seq of AS`Type  
SEQ<TYPE_AS_Type> EvalState::FindTypeDom(const TYPE_SEM_VAL & fnval) const
{
  TYPE_AS_Type fnoptype (FindType(fnval));
  switch(fnoptype.GetTag()) {
    case TAG_TYPE_AS_PartialFnType: { return fnoptype.GetSequence(pos_AS_PartialFnType_fndom); }
    case TAG_TYPE_AS_TotalFnType:   { return fnoptype.GetSequence(pos_AS_TotalFnType_fndom); }
    case TAG_TYPE_AS_OpType:        { return fnoptype.GetSequence(pos_AS_OpType_opdom); }
    default:                        { return SEQ<TYPE_AS_Type>(); }
  }
}

// LookUpOverloaded
// clsnm : AS`Name
// name : AS`Name
// allclasses : set of AS`Name
// ==> [SEM`OverOPFN]
Generic EvalState::LookUpOverloaded(const TYPE_AS_Name & clsnm, const TYPE_AS_Name & name, const Set & allclasses)
{
//wcout << L"clsnm: " << clsnm << endl;
//wcout << L"allclasses: " << allclasses << endl;

  Set allclasses_q (allclasses);
  Map overloads; // map AS`Name to SEM`OverOPFN
  Generic clnm;
  for (bool bb = allclasses_q.First(clnm); bb; bb = allclasses_q.Next(clnm)) {
    Generic opfn (LookUpOverInClass(clnm, name));
    if (!opfn.IsNil()) {
      overloads.Insert(clnm, opfn);
    }
  }

  if (overloads.IsEmpty()) {
    return Nil();
  }
  else {
    SET<TYPE_AS_Name> dom_overloads (overloads.Dom());
    Map over; // map (AS`Name * AS`Name) to ((seq of AS`Type) * AS`Access * [SEM`CompExplFN])
    Generic clnm;
    for (bool bb = dom_overloads.First(clnm); bb; bb = dom_overloads.Next(clnm)) {
      TYPE_SEM_OverOPFN oopfn (overloads[clnm]);
      over.ImpOverride(oopfn.GetMap(pos_SEM_OverOPFN_overload));
    }
    return TYPE_SEM_OverOPFN().Init(over, Nil());
  }
}

// LookUpOverInClass
// clsnm : AS`Name
// name : AS`Name
// ==> [SEM`OverOPFN]
Generic EvalState::LookUpOverInClass(const TYPE_AS_Name & clsnm, const TYPE_AS_Name & name)
{
  const TYPE_GLOBAL_SigmaClass & classesNM (this->classes[clsnm]);
  const Map & overloaded (classesNM.GetMap(pos_GLOBAL_SigmaClass_overloaded));
  if (overloaded.DomExists(name)) {
    const TYPE_GLOBAL_Overloaded & over (overloaded[name]); // map nat to map AS`Name to ((seq of AS`Type) * AS`Access)
    SET<Int> dom_over (over.Dom());
    Map load; // map (AS`Name * AS`Name) to ((seq of AS`Type) * AS`Access)
    Generic arit;
    for (bool bb = dom_over.First(arit); bb; bb = dom_over.Next(arit)) {
      MAP<TYPE_AS_Name, Tuple> over_arit (over[arit]); // map AS`Name to ((seq of AS`Type) * AS`Access)
      SET<TYPE_AS_Name> dom_over_arit (over_arit.Dom());
      Generic nm;
      for (bool cc = dom_over_arit.First(nm); cc; cc = dom_over_arit.Next(nm)) {
        load.ImpModify(mk_(nm, clsnm), over_arit[nm]);
      }
    }
    return TYPE_SEM_OverOPFN().Init(load, Nil());
  }
  else {
    return Nil();
  }
}

// LookUpStatic
// name : AS`Name
// ==> bool * [SEM`VAL] * *[AS`Type] * [As`Name] * [AS`Access]
Tuple EvalState::LookUpStatic(const TYPE_AS_Name & name)
{
  if (name.GetSequence(pos_AS_Name_ids).Length() == 2) {
    TYPE_AS_Name clsnm (ASTAUX::GetFirstName(name));
    if (theStackMachine().HasCurCl()) {
      Tuple t (ExpandClassName(clsnm, theStackMachine().GetCurCl(), Set()));
      if (t.GetBoolValue(1)) {
        clsnm = t.GetRecord(2);
      }
    }

    TYPE_AS_Name memnm (ASTAUX::GetSecondName(name));

    if (this->classes.DomExists(clsnm)) {
      // search static member in instance variables section LookUp Static Variable
      const TYPE_GLOBAL_SigmaClass & classesClsnm (this->classes[clsnm]);
      Generic tupG;
      if (classesClsnm.GetMap(pos_GLOBAL_SigmaClass_statics).DomExists(memnm, tupG)) {
        type_dUU3P tup (tupG);                     // (SEM`VAL * AS`Type *  AS`Access)
        const TYPE_GLOBAL_OrigCl & curcls (theStackMachine().GetCurCl());
        const Generic & tp (tup.GetField(2));
        const TYPE_AS_Access & access (tup.GetField(3));
        if (AccessOk(access, curcls, clsnm))
#ifdef VICE
          return mk_(Bool(true), theSystem().UpdateObjRef(tup.GetRecord(1)), tp, clsnm, access);
#else
          return mk_(Bool(true), tup.GetRecord(1), tp, clsnm, access); // SEM`VAL
#endif // VICE
        else
          //return mk_(Bool(false), Nil(), Nil(), Nil());
          return mk_(Bool(true), Nil(), Nil(), Nil(), Nil());
      }

// 20070910
// TODO : instance method will be returned

      // LookUp Static Function/Operation
      Tuple lsofp (LookStaticOpFctPoly(clsnm, memnm)); // bool* bool* [SEM`VAL]
      if(lsofp.GetBoolValue(1)) {
        if(lsofp.GetBoolValue(2)) {
          return mk_(Bool(true), lsofp.GetRecord(3), Nil(), clsnm, Int(PUBLIC_AS));  // [SEM`VAL]
        }
        else {
          //return mk_(Bool(true), Nil(), clsnm, Int(PUBLIC_AS));
          return mk_(Bool(true), Nil(), Nil(), Nil(), Nil());
        }
      }
 
      // LookUp Super Classes
      SET<TYPE_AS_Name> inhcon (classesClsnm.GetSet(pos_GLOBAL_SigmaClass_inhcon));
      if( !inhcon.IsEmpty() ) {
        Generic clnm;
        for(bool bb = inhcon.First(clnm); bb; bb = inhcon.Next(clnm)) {
          Tuple lus (LookUpStatic(AUX::ConstructDoubleName(clnm, name)));
          if( lus.GetBoolValue(1) ) {
            return lus;
          }
        }
      }

      // not exists
      return mk_(Bool(false), Nil(), Nil(), Nil(), Nil());
    }
    else {
      RTERR::Error(L"LookUpStatic", RTERR_CLNM_NOT_DEFINED,
                          M42Sem(AUX::SingleNameToString(name), NULL), Nil(),
                          Sequence().ImpAppend(AUX::SingleNameToString(clsnm)));
      return mk_(Bool(false), Nil(), Nil(), Nil(), Nil());
    }
  }
  else
  { // name.get_ids().Length() != 2
    const TYPE_GLOBAL_OrigCl & curcls (theStackMachine().GetCurCl());
    switch(curcls.GetTag()) {
      case TAG_TYPE_AS_Name: {
        if (curcls == ASTAUX::MkNameFromVoid()) {
          return mk_(Bool(false), Nil(), Nil(), Nil(), Nil());
        }
        return LookUpStatic(AUX::ConstructDoubleName(curcls, name));
      }
      case TAG_TYPE_GLOBAL_Start: {
        if (!this->initclstack.IsEmpty()) {
          return LookUpStatic(AUX::ConstructDoubleName(this->initclstack.Hd(), name));
        }
        else {
          return mk_(Bool(false), Nil(), Nil(), Nil(), Nil());
        }
      }
    }
  }
  return mk_(Bool(false), Nil(), Nil(), Nil(), Nil()); // dummy
}

// ReturnLookUp
// val : [SEM`VAL]
// tp : [AS`Type]
// err_str : RTERR`ERR
// ==> SEM`ValTp
TYPE_SEM_ValTp EvalState::ReturnLookUp(const Generic & val, const Generic & tp, const int errnum)
{
  if (val.Is(TAG_TYPE_SEM_UNDEF)) {
    return RTERR::ErrorValTp(L"ReturnLookUp", RTERR_UNDEF_ENCOUNTERED, Nil(), Nil(), Sequence());
  }
  else if (val.IsNil()) {
    return RTERR::ErrorValTp(L"ReturnLookUp", errnum, Nil(), Nil(), Sequence());
  }
  else {
    return TYPE_SEM_ValTp().Init(val, tp);
  }
}

// IsDObjs
// name : AS`Name
// ==> bool * [SEM`VAL]
Tuple EvalState::IsDObjs(const TYPE_AS_Name & name)
{
  Generic g;
  if (this->d_objs.DomExists(name, g)) {
    return mk_(Bool (true), Tuple(g).GetField(1));
  }
  else {
    return mk_(Bool (false), Nil());
  }
}

// GetPreCond
// clnm : AS`Name
// fnnm : AS`Name
// ==> [SEM`CompExplFN]
Generic EvalState::GetPreCond(const TYPE_AS_Name & clnm, const TYPE_AS_Name & fnnm)
{
  if (this->classes.DomExists(clnm)) {
    TYPE_AS_Name prenm (AUX::PreName(fnnm));
    const TYPE_GLOBAL_SigmaClass & sigmacl (this->classes[clnm]);
    const Map & explfns (sigmacl.GetMap(pos_GLOBAL_SigmaClass_explfns));
    if (explfns.DomExists(prenm)) {
      return explfns[prenm];
    }
    else {
      return Nil();
    }
  }
  else {
    return Nil();
  }
}
#endif // VDMPP

#ifdef VDMSL
// GetPreCond
// clnm : AS`Name
// fnnm : AS`Name
// ==> [SEM`CompExplFN]
Generic EvalState::GetPreCond(const TYPE_AS_Name & modnm, const TYPE_AS_Name & fnnm)
{
  if (this->mods.DomExists(modnm))
  {
    TYPE_AS_Name prenm (AUX::PreName(fnnm));
    const TYPE_GLOBAL_SigmaMO & sigmamo (this->mods[modnm]);
    const Map & explfns (sigmamo.GetMap(pos_GLOBAL_SigmaMO_explfns));
    if (explfns.DomExists(prenm))
      return explfns[prenm];
    else
      return Nil();
  }
  else
    return Nil();
}

// ExtractModule
// nm : AS`Name ==> AS`Name
// ==> AS`Name
TYPE_AS_Name EvalState::ExtractModule (const TYPE_AS_Name & nm) const
{
  switch (nm.GetSequence(pos_AS_Name_ids).Length()) {
    case 1: {
      return theStackMachine().CurrentModule ();
    }
    case 2: {
      TYPE_AS_Name new_name (ASTAUX::GetFirstName(nm));
      if (this->mods.DomExists (new_name) || this->imods.DomExists (new_name))
        return new_name;
      else
      {
        RTERR::Error (L"ExtractModule", RTERR_MOD_NOT_DEFINED,
                             M42Sem(AUX::SingleNameToString(nm), NULL), Nil(), Sequence());
        return ASTAUX::MkNameFromVoid();
      }
    }
    default: {
      // Warning to catch parameterized modules until we know what to do
      // exactly. There are reported bugs for these kind of modules.
      RTERR::Error (L"ExtractModule", RTERR_MOD_NOT_DEFINED,
                           M42Sem(AUX::SingleNameToString(nm), NULL), Nil(), Sequence());
      return ASTAUX::MkNameFromVoid();
    }
  }
}
#endif //VDMSL

// CheckGlobInv
// ==> bool
bool EvalState::CheckGlobInv() /*const*/
{
#ifdef VDMSL
  return CheckStateInvariant();
#endif //VDMSL
#ifdef VDMPP
  // not Init => CheckInstanceInvariant() <=> Init or CheckInstanceInvariant()
  return Init_flag || CheckInstanceInvariant();
#endif //VDMPP
}

#ifdef VDMSL
// GetFirstCIDOfFctOp
// name : AS`Name
// ==> bool * seq of char * [CI`ContextId] * [AS`Name] * [AS`Name]
Tuple EvalState::GetFirstCIDOfFctOp(const TYPE_AS_Name& name)
{
  TYPE_AS_Name loc_nm (AUX::ExtractName(name));
  TYPE_AS_Name mod_nm (ExtractModule(name));

  if (this->imods.DomExists(mod_nm)) {
    return mk_(Bool(false), SEQ<Char>(L"It is not possible to set breakpoints in a DL module"), Nil(), Nil(), Nil());
  }
  else if (! this->mods.DomExists(mod_nm)) {
    return mk_(Bool(false), SEQ<Char>(L"No such function or operation"), Nil(), Nil(), Nil());
  }

  TYPE_STKM_SubProgramId sem;
  if (EXPR::IsFunction(loc_nm, mod_nm)) {
    TYPE_SEM_CompExplFN cfn (EXPR::GetFunctionVal(loc_nm, mod_nm));
    TYPE_SEM_ExplFN efn (cfn.GetSequence(pos_SEM_CompExplFN_fl).Hd());
    sem = efn.GetInt(pos_SEM_ExplFN_instr);
  }
  else if (EXPR::IsOperation(loc_nm, mod_nm)) {
    TYPE_SEM_ExplOP eop (EXPR::GetOperationVal(loc_nm, mod_nm));
    sem = eop.GetInt(pos_SEM_ExplOP_instr);
  }
  else if (EXPR::IsPoly(loc_nm, mod_nm)) {
    TYPE_SEM_ExplPOLY epoly (EXPR::GetPolyVal(loc_nm, mod_nm));
    sem = epoly.GetInt(pos_SEM_ExplPOLY_instr);
  }
  else {
    return mk_(Bool(false), SEQ<Char>(L"No such function or operation"), Nil(), Nil(), Nil());
  }

  TYPE_STKM_SubProgram instr (theCompiler().GetProgram(mod_nm, sem));

  size_t length = instr.Length();
  for (size_t i = 1; i <= length; i++) {
    if (instr.Index(i).Is(TAG_TYPE_INSTRTP_CONTEXT)) {
      return mk_(Bool(true), SEQ<Char>(), TYPE_INSTRTP_CONTEXT(instr.Index(i)).get_cid(), mod_nm, loc_nm);
    }
  }
  return mk_(Bool(false), SEQ<Char>(L"Internal error"), Nil(), Nil(), Nil());
}
#endif // VDMSL

#ifdef VDMPP
// GetFirstCIDOfFctOp
// nm : AS`Name
// ==> bool * seq of char * [CI`ContextId] * [AS`Name] * [AS`Name]
Tuple EvalState::GetFirstCIDOfFctOp(const TYPE_AS_Name & nm)
{
  if (nm.GetSequence(pos_AS_Name_ids).Length() != 2) {
    return mk_(Bool(false), SEQ<Char>(L"Internal error"), Nil(), Nil(), Nil());
  }

  TYPE_AS_Name clnm (ASTAUX::GetFirstName(nm));
  if (this->classes.DomExists(clnm))
  {
    TYPE_AS_Name opfn (ASTAUX::GetSecondName(nm));
    const TYPE_GLOBAL_SigmaClass & sigmacl (this->classes[clnm]);

    TYPE_STKM_SubProgramId sem;
    Generic g;
    if (sigmacl.GetMap(pos_GLOBAL_SigmaClass_explfns).DomExists(opfn)) {
      const Map & explfns (sigmacl.GetMap(pos_GLOBAL_SigmaClass_explfns));
      TYPE_SEM_ExplFN efn (Record(explfns[opfn]).GetSequence(pos_SEM_CompExplFN_fl).Hd());
      sem = efn.GetInt(pos_SEM_ExplFN_instr);
    }
    else
    {
      // 20081003 for overload operations
      const Map & explops (sigmacl.GetMap(pos_GLOBAL_SigmaClass_explops));
      Set nm_s (explops.Dom());
      bool exists = false;
      Generic nm;
      for (bool bb = nm_s.First(nm); bb && !exists; bb = nm_s.Next(nm))
      {
        if (MANGLE::IsMangled(nm))
        {
          exists = (MANGLE::GetUnmangledName(nm) == opfn);
        }
        else
        {
          exists = (nm == opfn);
        }
        if (exists)
        {
          //Tuple tt (explops[nm]);
          //sem = tt.GetRecord(1).GetInt(pos_SEM_ExplOP_instr);
          sem = Record(explops[nm]).GetInt(pos_SEM_ExplOP_instr);
        }
      }
      if (!exists)
      {
        if (sigmacl.GetMap(pos_GLOBAL_SigmaClass_explpolys).DomExists(opfn, g)) {
          const TYPE_SEM_ExplPOLY & epoly (Tuple(g).GetRecord(1));
          sem = epoly.GetInt(pos_SEM_ExplPOLY_instr);
        }
        else {
          return mk_(Bool(false), SEQ<Char>(L"No such method"), Nil(), Nil(), Nil());
        }
      }
    }

    TYPE_STKM_SubProgram instr (theCompiler().GetProgram(clnm, sem));

    size_t length = instr.Length();
    for (size_t i = 1; i <= length; i++) {
      if (instr.Index(i).Is(TAG_TYPE_INSTRTP_CONTEXT)) {
        return mk_(Bool(true), SEQ<Char>(L""), TYPE_INSTRTP_CONTEXT(instr.Index(i)).get_cid(), clnm, opfn);
      }
    }
    return mk_(Bool(false), SEQ<Char>(L"Internal error"), Nil(), Nil(), Nil());
  }
  else {
    return mk_(Bool(false), SEQ<Char>(L"No such class"), Nil(), Nil(), Nil());
  }
}
#endif //VDMPP

#ifdef VDMSL
// GetDLModule
// AS`Name
// ==> GLOBAL`SigmaIMO
TYPE_GLOBAL_SigmaIMO EvalState::GetDLModule(const TYPE_AS_Name & mod_name)
{
  if (this->imods.DomExists (mod_name)) {
    return this->imods[mod_name];
  }
  else {
    RTERR::Error (L"GetDLModule", RTERR_MOD_NOT_DEFINED,
                         M42Sem(AUX::SingleNameToString(mod_name), NULL), Nil(), Sequence());
    return TYPE_GLOBAL_SigmaIMO();
  }
}
#endif // DVMSL

// APPROXVal
// loc : SEM`LOC
// arg_l : seq of SEM`VAL
// ==> SEM`VAL

#ifdef VDMPP
// LookUpPermis
// nm : AS`Name
// ==> [STKM`SubProgram]
Generic EvalState::LookUpPermis(const TYPE_AS_Name & nm) const
{
  if (nm.GetSequence(pos_AS_Name_ids).Length() != 2)
    return Nil();

  TYPE_AS_Name clnm (ASTAUX::GetFirstName(nm));
  TYPE_AS_Name opnm (ASTAUX::GetSecondName(nm));

  const TYPE_GLOBAL_SigmaClass & sigmacl (this->classes[clnm]); // 

  Generic pred;
  if (sigmacl.GetMap(pos_GLOBAL_SigmaClass_perm_upred).DomExists(opnm, pred))
  {
    if (TYPE_STKM_SubProgram(pred).IsEmpty())
      return Nil();
    else
      return pred;
  }
  else
    return Nil();
}

// SetPermission
// clnm : AS`Name
// opnm : AS`Name
// code : STKM`SubProgram
void EvalState::SetPermission(const TYPE_AS_Name & clnm, const TYPE_AS_Name & opnm, const TYPE_STKM_SubProgram & code)
{
  TYPE_GLOBAL_SigmaClass sigmacl (this->classes[clnm]);
  type_7AS_NameCdLM perm_pred (sigmacl.GetMap(pos_GLOBAL_SigmaClass_perm_upred));
  perm_pred.ImpModify(opnm, code);
  sigmacl.SetField(pos_GLOBAL_SigmaClass_perm_upred, perm_pred);
  this->classes.ImpModify(clnm, sigmacl);
}

// LookUpThread
// clnm : AS`Name
// ==> [STKM`SubProgram * [nat * nat * nat * nat]]
Generic EvalState::LookUpThread(const TYPE_AS_Name & clnm) const
{
  return this->classes[clnm].GetField(pos_GLOBAL_SigmaClass_thread);
}

#ifdef VICE
// SetPerThreadDef
// clnm : AS`Name
// perobl : AS`PerObl
void EvalState::SetPerThreadDef (const TYPE_AS_Name & clnm, const TYPE_AS_PerObl & perobl)
{
  const TYPE_AS_Expr & period (perobl.GetRecord(pos_AS_PerObl_period));
  const TYPE_AS_Expr & jitter (perobl.GetRecord(pos_AS_PerObl_jitter));
  const TYPE_AS_Expr & delay  (perobl.GetRecord(pos_AS_PerObl_delay));
  const TYPE_AS_Expr & offset (perobl.GetRecord(pos_AS_PerObl_offset));
  const TYPE_AS_Name & mtd (perobl.GetRecord(pos_AS_PerObl_mtd));

  TYPE_STKM_SubProgram sp;
  sp.ImpConc(theCompiler().SetContext(perobl.GetInt(pos_AS_PerObl_cid), true));
  sp.ImpAppend(TYPE_INSTRTP_PUSHCONTEXT().Init(Int(PTHREAD)));  //
  sp.ImpAppend(TYPE_INSTRTP_PUSH().Init(SEQ<TYPE_SEM_VAL>()));
  sp.ImpAppend(TYPE_INSTRTP_CALLGUARD().Init(false, mtd));
  sp.ImpAppend(TYPE_INSTRTP_PPCALL());
  sp.ImpAppend(TYPE_INSTRTP_EOCL());

  TYPE_GLOBAL_SigmaClass sigmacl (this->classes[clnm]);
  sigmacl.set_thread(TYPE_STKM_PerThread().Init(sp, period, jitter, delay, offset));
  this->classes.ImpModify(clnm, sigmacl);
}

// SetSpoThreadDef
// clnm : AS`Name
// perobl : AS`PerObl
void EvalState::SetSpoThreadDef (const TYPE_AS_Name & clnm, const TYPE_AS_SpoObl & perobl)
{
  const TYPE_AS_Expr & delay  (perobl.GetRecord(pos_AS_SpoObl_delay));
  const TYPE_AS_Expr & bound  (perobl.GetRecord(pos_AS_SpoObl_bound));
  const TYPE_AS_Expr & offset (perobl.GetRecord(pos_AS_SpoObl_offset));
  const TYPE_AS_Name & mtd (perobl.GetRecord(pos_AS_SpoObl_mtd));

  TYPE_STKM_SubProgram sp;
  sp.ImpConc(theCompiler().SetContext(perobl.GetInt(pos_AS_SpoObl_cid), true));
  sp.ImpAppend(TYPE_INSTRTP_PUSHCONTEXT().Init(Int(PTHREAD)));  //
  sp.ImpAppend(TYPE_INSTRTP_PUSH().Init(SEQ<TYPE_SEM_VAL>()));
  sp.ImpAppend(TYPE_INSTRTP_CALLGUARD().Init(false, mtd));
  sp.ImpAppend(TYPE_INSTRTP_PPCALL());
  sp.ImpAppend(TYPE_INSTRTP_EOCL());

  TYPE_GLOBAL_SigmaClass sigmacl (this->classes[clnm]);
  sigmacl.set_thread(TYPE_STKM_SpoThread().Init(sp, delay, bound, offset));
  this->classes.ImpModify(clnm, sigmacl);
}
#endif //VICE

// SetThreadField
// clnm : AS`Name
// threadfield : [STKM`Thread]
void EvalState::SetThreadField(const TYPE_AS_Name & clnm, const Generic & threadfield)
{
  TYPE_GLOBAL_SigmaClass sigmacl (this->classes[clnm]);
  sigmacl.SetField(pos_GLOBAL_SigmaClass_thread, threadfield);
  this->classes.ImpModify(clnm, sigmacl);
}

// SetThreadDef
// clnm : AS`Name
// thread : [STKM`Thread]
void EvalState::SetThreadDef(const TYPE_AS_Name & clnm, const Generic & thread)
{
  SetThreadField(clnm, thread); 
}

// GetThreadDef
// clnm : AS`Name
// ==> [STKM`Thread]
Generic EvalState::GetThreadDef(const TYPE_AS_Name & clnm) const
{
  return this->classes[clnm].GetField(pos_GLOBAL_SigmaClass_thread);
}

// GetSemObjInTab
// nm : SEM`OBJ_Ref
// ==> SEM`OBJ
TYPE_SEM_OBJ EvalState::GetSemObjInTab(const TYPE_SEM_OBJ_uRef & obj_ref) const
{
  if (this->obj_tab.DomExists(obj_ref))
    return this->obj_tab[obj_ref].GetRecord(pos_GLOBAL_OBJ_uDesc_sem);
  else
    RTERR::Error(L"GetSemObjInTab", RTERR_INTERNAL_ERROR, obj_ref, Nil(), Sequence());
  return TYPE_SEM_OBJ(); // dummy
}

// GiveNextRefCountInTab
// ==> SEM`OBJ_Ref
TYPE_SEM_OBJ_uRef EvalState::GiveNextRefCountInTab(const TYPE_AS_Name & clnm)
{
  this->obj_ref_no.Incr();
  return TYPE_SEM_OBJ_uRef().Init(this->obj_ref_no, clnm);
}

// global_obj_tab_insert
// semobj : SEM`OBJ
// dlclassp : [DlClassInstance(VDMContainer<DlClassInstanceHolder>)]
// ==> SEM`OBJ_Ref
TYPE_SEM_OBJ_uRef EvalState::global_obj_tab_insert(const TYPE_SEM_OBJ & semobj, const Generic & dlclassp)
{
  TYPE_SEM_OBJ_uRef ref (GiveNextRefCountInTab(semobj.GetRecord(pos_SEM_OBJ_tp)));

  TYPE_GLOBAL_OBJ_uDesc desc;
  desc.SetField(pos_GLOBAL_OBJ_uDesc_ref_ucount,         DRPtr(ref.get_mvp()));
  desc.SetField(pos_GLOBAL_OBJ_uDesc_sem,                semobj);
  desc.SetField(pos_GLOBAL_OBJ_uDesc_threadid, Nil());  // optional threadid
  desc.SetField(pos_GLOBAL_OBJ_uDesc_DlClassInstancePtr, dlclassp);
  desc.SetField(pos_GLOBAL_OBJ_uDesc_procid,             Nil());

  this->obj_tab.ImpModify(ref, desc);
  return ref;
}

// SetObjectThreadId
// obj_ref : SEM`OBJ_Ref
// threadid : SCHDTP`ThreadId
// ==> ()
void EvalState::SetObjectThreadId(const TYPE_SEM_OBJ_uRef & objref, const TYPE_SCHDTP_ThreadId & thrid)
{
  if ( this->obj_tab.DomExists(objref) )
  {
    TYPE_GLOBAL_OBJ_uDesc desc (this->obj_tab[objref]); // OBJ_Desc
    desc.SetField(pos_GLOBAL_OBJ_uDesc_threadid, thrid);
    this->obj_tab.ImpModify(objref, desc);
  }
  else
  {
    vdm_iplog << L"SetObjTabThreadId: Unknown semobj" << endl << flush;
  }
}

// GetAllClassNms
// ==> set of AS`Name
//Set EvalState::GetAllClassNms() const
SET<TYPE_AS_Name> EvalState::GetAllClassNms() const
{
  return this->classes.Dom();
}

// IsClient
// origcl : AS`Name
// cloffct :  AS`Name
// ==> bool
bool EvalState::IsClient(const TYPE_AS_Name & origcl, const TYPE_AS_Name & cloffct) const
{
  return !((origcl == cloffct) || IsSubClass(cloffct, origcl) || IsSubClass(origcl, cloffct));
}

// IsSubClass
// return true if origcl is subclass of cloffct
// origcl : AS`Name
// cloffct : AS`Name
// ==> bool
bool EvalState::IsSubClass(const TYPE_AS_Name & origcl, const TYPE_AS_Name & cloffct) const
{
  if (this->hchy.DomExists(origcl))
    return this->hchy[origcl].InSet(cloffct);
  else {
    theStackMachine().SetCid(ASTAUX::GetCid(origcl));
    RTERR::Error(L"IsSubClass", RTERR_CLNM_NOT_DEFINED,
                 M42Sem(AUX::SingleNameToString(origcl), NULL), Nil(),
                 Sequence().ImpAppend(AUX::SingleNameToString(origcl)));
    return false; // dummy
  }
}

// IsBaseClass
// clnm : AS`Name
// ==> bool
bool EvalState::IsBaseClass(const TYPE_AS_Name & clnm) const
{
  if (this->hchy.DomExists(clnm))
    return this->hchy[clnm].IsEmpty();
  else {
    theStackMachine().SetCid(ASTAUX::GetCid(clnm));
    RTERR::Error(L"IsBaseClass", RTERR_CLNM_NOT_DEFINED,
                        M42Sem(AUX::SingleNameToString(clnm), NULL), Nil(),
                        Sequence().ImpAppend(AUX::SingleNameToString(clnm)));
    return false; // dummy
  }
}

// AccessOk
// access : AS`Access
// origcl : GLOBAL`OrigCl (AS`Name | GLOBAL`Start)
// cloffct : AS`Name ==> bool
bool EvalState::AccessOk (const TYPE_AS_Access & access,
                          const TYPE_GLOBAL_OrigCl & origcl,
                          const TYPE_AS_Name & cloffct) const
{
  int access_value = (((Int) access).GetValue());

  if (origcl.Is(TAG_TYPE_GLOBAL_Start)) {
    return (access_value == PUBLIC_AS);
  }
  else
  {
// 20090311 -->
    if (origcl == cloffct)
      return true;
// <-- 20090311

    switch(access_value) {
      case PUBLIC_AS: {
        return true;
      }
      case PRIVATE_AS: {
        //return !IsClient(origcl, cloffct) && ((origcl == cloffct) || !GetAllSupers(origcl).InSet(cloffct));
        return IsSubClass(cloffct, origcl); // abstract function/operation
      }
      case PROTECTED_AS: {
        //return !IsClient(origcl, cloffct);
        return IsSubClass(cloffct, origcl) || IsSubClass(origcl, cloffct);
      }
      case DEFAULT_AS: {
        //return !IsClient(origcl, cloffct) && (origcl == cloffct);
        return false;
      }
      default: {
        //return (origcl == cloffct); // dummy
        return false;
      }
    }
  }
}

#ifdef VICE
// GetCPUOfObjRef
// objref : SEM`OBJ_Ref
// ==> [AS`Name]
Generic EvalState::GetCPUOfObjRef(const TYPE_SEM_OBJ_uRef & objref) const
{
  if (this->obj_tab.DomExists(objref))
    return this->obj_tab[objref].GetField(pos_GLOBAL_OBJ_uDesc_procid);
  else
    vdm_iplog << L"GetProcId: Unknown objref" << endl << flush;
  return Nil();  // dummy;
}

// SetCPUOfObjRef
// cpunm : AS`Name
// objref : SEM`OBJ_Ref
// ==> ()
void EvalState::SetCPUOfObjRef(const TYPE_AS_Name & cpunm, const TYPE_SEM_OBJ_uRef & objref)
{
  if (this->obj_tab.DomExists(objref))
  {
    TYPE_GLOBAL_OBJ_uDesc desc (this->obj_tab[objref]); // OBJ_Desc
    desc.SetField (pos_GLOBAL_OBJ_uDesc_procid, cpunm);
    this->obj_tab.ImpModify(objref, desc);
  }
  else
    vdm_iplog << L"SetProcId: Unknown objref" << endl << flush;
}
#endif // VICE

// LookUpHistory
// kind : INSTRTP`HistoryKind
// opnm : AS`Name
// ==> SEM`NUM
TYPE_SEM_NUM EvalState::LookUpHistory(const TYPE_INSTRTP_HistoryKind & kind, const TYPE_AS_Name & opnm)
{
// 20090121
  if (!theStackMachine().HasCurObjRef())
    return mk_SEM_NUM(Real(0));

  const TYPE_SEM_OBJ_uRef & the_ref (theStackMachine().GetCurObjRef());

  if (!this->obj_tab.DomExists(the_ref))
    return mk_SEM_NUM(Real(0));

  TYPE_AS_Name realopnm ((opnm.GetSequence(pos_AS_Name_ids).Length() == 1)
                            ? AUX::ConstructDoubleName(theStackMachine().GetCurCl(), opnm)
                            : opnm);

  TYPE_SEM_OBJ sem (GetSemObjInTab(the_ref));
  const Map & hist_m (sem.GetMap(pos_SEM_OBJ_hist)); // map AS`Name to SEM`History

  if (hist_m.DomExists(realopnm))
    return CounterCalc(kind, hist_m[realopnm]);
  else
  {
    TYPE_AS_Name clnm ((opnm.GetSequence(pos_AS_Name_ids).Length() == 2)
                           ? ASTAUX::GetFirstName(opnm)
                           : the_ref.GetRecord(pos_SEM_OBJ_uRef_tp));

    if (IsStatic(clnm, ASTAUX::UnqualiName(opnm)))
    {
      Map statichist (this->classes[clnm].GetMap(pos_GLOBAL_SigmaClass_statichist));
      if (statichist.DomExists(opnm))
        return CounterCalc(kind, statichist[opnm]);
      else
        return mk_SEM_NUM(Real(0)); // dummy
    }
    else
    {
      int overnum = CalcOverload(kind, opnm, hist_m);
      return mk_SEM_NUM(Real(overnum));
    }
  }
}

// CounterCalc
// kind : INSTRTP`HistoryKind
// history : SEM`History
// ==> SEM`NUM
TYPE_SEM_NUM EvalState::CounterCalc(const TYPE_INSTRTP_HistoryKind & kind, const TYPE_SEM_History & history) const
{
  switch (kind.GetTag()) {
    case TAG_TYPE_INSTRTP_req : {
      return TYPE_SEM_NUM().Init(Real(history.GetInt(pos_SEM_History_req)));
    }
    case TAG_TYPE_INSTRTP_act : {
      return TYPE_SEM_NUM().Init(Real(history.GetInt(pos_SEM_History_act)));
    }
    case TAG_TYPE_INSTRTP_fin : {
      return TYPE_SEM_NUM().Init(Real(history.GetInt(pos_SEM_History_fin)));
    }
    case TAG_TYPE_INSTRTP_active : {
      return TYPE_SEM_NUM().Init(Real(history.GetInt(pos_SEM_History_act) - history.GetInt(pos_SEM_History_fin)));
    }
    case TAG_TYPE_INSTRTP_waiting : {
      return TYPE_SEM_NUM().Init(Real(history.GetInt(pos_SEM_History_req) - history.GetInt(pos_SEM_History_act)));
    }
  }
  return TYPE_SEM_NUM().Init(Real(0)); // dummy
}

// CalcOverload
// kind : INSTRTP`HistoryKind
// nm : AS`Name
// histm : map AS`Name to SEM`History
// ==> nat
int EvalState::CalcOverload(const TYPE_INSTRTP_HistoryKind & kind, const TYPE_AS_Name & nm, const Map & histm) const
{
  int res = 0;
  Set dom_histm (histm.Dom());
  Generic opnm;
  for (bool bb = dom_histm.First(opnm); bb; bb = dom_histm.Next(opnm)) {
    if (MANGLE::IsMangled(opnm)) {
      if(MANGLE::GetUnmangledName(opnm) == nm) {
        res += (int)(CounterCalc(kind, histm[opnm]).GetReal(pos_SEM_NUM_v).GetValue());
      }
    }
  }
  return res;
}

// IsStatic
// clnm : AS`Name
// opnm : AS`Name
// ==> bool
bool EvalState::IsStatic(const TYPE_AS_Name & clnm, const TYPE_AS_Name & opnm) const
{
  if( this->classes.DomExists(clnm) )
    return this->classes[clnm].GetMap(pos_GLOBAL_SigmaClass_statics).DomExists(opnm);
  else
    return false;
}

// RemoveStaticHist
// ==> ()
void EvalState::RemoveStaticHist()
{
  SET<TYPE_AS_Name> clnm_s (this->classes.Dom());
  Generic clnm;
  for( bool bb = clnm_s.First(clnm); bb; bb = clnm_s.Next(clnm)) {
    TYPE_GLOBAL_SigmaClass sigmacl (this->classes[clnm]);
    MAP<TYPE_AS_Name,TYPE_SEM_History> statichist (sigmacl.GetField(pos_GLOBAL_SigmaClass_statichist));
    SET<TYPE_AS_Name> nm_s (statichist.Dom());
    Generic nm;
    for(bool cc = nm_s.First(nm); cc; cc = nm_s.Next(nm))
    {
      statichist.ImpModify(nm, TYPE_SEM_History().Init(Int(0), Int(0), Int(0)));
    }
    sigmacl.SetField(pos_GLOBAL_SigmaClass_statichist, statichist);
    this->classes.ImpModify(clnm, sigmacl);
  }
}

#ifdef VICE
// UpdateHistCount
// opnm : AS`Name
// kind : INSTRTP`HistoryKind
// objref : [SEM`OBJ_Ref]
// async : bool
// vals : [seq of SEM`VAL]
void EvalState::UpdateHistCount(const TYPE_AS_Name & opnm,
                                const TYPE_INSTRTP_HistoryKind & kind,
                                const Generic & objref,
                                const Bool & async,
                                const SEQ<TYPE_SEM_VAL> & vals)
#else
// UpdateHistCount
// opnm : AS`Name
// kind : INSTRTP`HistoryKind
// objref : [SEM`OBJ_Ref]
void EvalState::UpdateHistCount(const TYPE_AS_Name & opnm,
                                const TYPE_INSTRTP_HistoryKind & kind,
                                const Generic & objref)
#endif // VICE
{
#ifndef VICE
  if(!theStackMachine().UsesThreads()) return;
#endif // VICE

//  Generic oid (objref.IsNil() ? Generic(Nil()) : Record(objref).GetField(pos_SEM_OBJ_uRef_ref));
//  wstring k;
//  switch (kind.GetTag()) {
//    case TAG_TYPE_INSTRTP_req : { k = L"req"; break; }
//    case TAG_TYPE_INSTRTP_act : { k = L"act"; break; }
//    case TAG_TYPE_INSTRTP_fin : { k = L"fin"; break; }
//  }
//  wcout << L"oid: " << oid << L" op: " << ASTAUX::ASName2String(opnm) << L" kind: " << k << endl;

//  if ( theStackMachine().IsEmptyObjL() &&
  if ( !theStackMachine().HasCurObjRef() &&
       (objref.IsNil() || (!objref.IsNil() && !IsInObjTab(objref))) ) return;

  TYPE_SEM_OBJ_uRef the_ref (objref.IsNil() ? theStackMachine().GetCurObjRef()
                                            : (const TYPE_SEM_OBJ_uRef &) objref);

  TYPE_AS_Name clnm ((opnm.GetSequence(pos_AS_Name_ids).Length() == 2) ? ASTAUX::GetFirstName(opnm)
                                                                       : the_ref.GetRecord(pos_SEM_OBJ_uRef_tp));

//  TYPE_AS_Name realopnm (ASTAUX::UnqualiName(opnm));
//
//  if(IsStatic(clnm, realopnm))
  if(IsStatic(clnm, ASTAUX::UnqualiName(opnm)))
  {
    TYPE_GLOBAL_SigmaClass sigmacl (this->classes[clnm]);
    MAP<TYPE_AS_Name,TYPE_SEM_History> hist_m (sigmacl.GetMap(pos_GLOBAL_SigmaClass_statichist));

    TYPE_SEM_History new_hist (AddHist(opnm, hist_m, kind));

    hist_m.ImpModify(opnm, new_hist);
    sigmacl.SetField(pos_GLOBAL_SigmaClass_statichist, hist_m);
    this->classes.ImpModify(clnm, sigmacl);
  }
  else
  {
    if (!IsInObjTab(the_ref)) return;

    TYPE_GLOBAL_OBJ_uDesc desc (Lookup_obj_tab(the_ref));
    TYPE_SEM_OBJ sem (desc.GetRecord(pos_GLOBAL_OBJ_uDesc_sem));
    MAP<TYPE_AS_Name,TYPE_SEM_History> hist_m (sem.GetMap(pos_SEM_OBJ_hist));

    TYPE_SEM_History new_hist (AddHist(opnm, hist_m, kind));

    hist_m.ImpModify(opnm, new_hist);

    sem.SetField(pos_SEM_OBJ_hist, hist_m);
    desc.SetField(pos_GLOBAL_OBJ_uDesc_sem, sem);
    Update_obj_tab(the_ref, desc);
  }

#ifdef VICE
  const TYPE_AS_Name & clm (the_ref.GetRecord(pos_SEM_OBJ_uRef_tp));
  TIMETRACE::LogHistEvent(theScheduler().CurThreadId(), clm, opnm, kind, the_ref, !async, vals);
  Set opnms (theScheduler().BlockedObjRef(the_ref));
  const Map & depops (this->classes[clm].GetMap(pos_GLOBAL_SigmaClass_depops));

  TYPE_AS_Name unm (ASTAUX::UnqualiName(opnm));
  opnms.ImpIntersect(depops.Dom());
  Generic nm;
  for (bool bb = opnms.First(nm); bb; bb = opnms.Next(nm))
  {
    const SET<TYPE_AS_Name> & nms (depops[nm]);
    if (nms.InSet(unm))
    {
      theScheduler().PotentialPerChange(the_ref);
      break;
    }
  }
#endif //VICE
}

// AddHist
// origopnm : AS`Name
// hist_m : (map AS`Name to SEM`History)
// kind : INSTRTP`HistoryKind
// +> SEM`History
TYPE_SEM_History EvalState::AddHist(const TYPE_AS_Name & origopnm,
                                    const Map & hist_m, 
                                    const TYPE_INSTRTP_HistoryKind & kind) const
{
  if (hist_m.DomExists(origopnm))
  {
    TYPE_SEM_History histrec (hist_m[origopnm]);
    switch ( kind.GetTag() ) {
      case TAG_TYPE_INSTRTP_req :
      {
        int r = histrec.GetIntValue(pos_SEM_History_req);
        histrec.SetField(pos_SEM_History_req, Int(r + 1));
        break;
      }
      case TAG_TYPE_INSTRTP_act :
      {
        int a = histrec.GetIntValue(pos_SEM_History_act);
        histrec.SetField(pos_SEM_History_act, Int(a + 1));
        break;
      }
      case TAG_TYPE_INSTRTP_fin :
      {
        int f = histrec.GetIntValue(pos_SEM_History_fin);
        histrec.SetField(pos_SEM_History_fin, Int(f + 1));
        break;
      }
    }
    return histrec;
  }
  else
  {
    TYPE_SEM_History histrec;
    switch ( kind.GetTag() )
    {
      case TAG_TYPE_INSTRTP_req :
      {
        histrec.Init(Int(1), Int(0), Int(0));
        break;
      }
      default:
      {
        histrec.Init(Int(1), Int(1), Int(0));
        break;
      }
    }
    return histrec;
  }
}

// RemoveHist
// desc : GLOBAL`OBJ_Desc
// +> GLOBAL`OBJ_Desc
TYPE_GLOBAL_OBJ_uDesc EvalState::RemoveHist(const TYPE_GLOBAL_OBJ_uDesc & desc) const
{
  TYPE_SEM_OBJ sem (desc.GetRecord(pos_GLOBAL_OBJ_uDesc_sem));
//  TYPE_AS_Name tp (sem.GetField(pos_SEM_OBJ_tp));
  TYPE_SEM_InsStrct ins (sem.GetMap(pos_SEM_OBJ_ins)); // map AS`Name to GLOBAL`ValueMap;
  SET<TYPE_AS_Name> dom_ins (ins.Dom());
  Generic nm;
  for(bool bb = dom_ins.First(nm); bb; bb = dom_ins.Next(nm)) {
    TYPE_GLOBAL_ValueMap vm (ins[nm]); // map AS`Name to (SEM`VAL * AS`Access)
    SET<TYPE_AS_Name> dom_vm (vm.Dom());
    Generic id;
    for(bool cc = dom_vm.First(id); cc; cc = dom_vm.Next(id)) {
      type_dUU3P t (vm[id]);     // (SEM`VAL * [AS`Type] * AS`Access)
      t.SetField(1, sem_undef);
      vm.ImpModify(id, t);
    }
    ins.ImpModify(nm, vm);
  }
  sem.SetField(pos_SEM_OBJ_ins, ins);

  TYPE_GLOBAL_OBJ_uDesc newdesc (desc);
  newdesc.SetField(pos_GLOBAL_OBJ_uDesc_sem, sem);
  newdesc.SetField(pos_GLOBAL_OBJ_uDesc_threadid, Nil());
  return newdesc;
}

// LookupHchy
// nm : AS`Name
// ==> bool * set of AS`Name
Tuple EvalState::LookupHchy(const TYPE_AS_Name & nm)
{
  if( this->hchy.DomExists(nm) )
    return mk_(Bool(true), this->hchy[nm]);
  else
    return mk_(Bool(false), Nil());
}

// SetHchy
// clhchy : GLOBAL`Hierarchy
void EvalState::SetHchy(const Map & clhchy)
{
  // *hchy = clhchy; //
  this->hchy.Clear();
  this->hchy.ImpOverride(clhchy);
}

// GetInhStrct
// nm : AS`Name
// ==> seq of set of AS`Name
Sequence EvalState::GetInhStrct(const TYPE_AS_Name & nm)
{
  return this->inhstrct[nm];
}

// SetInhStrct
// new_inhstrct : GLOBAL`InhStrct
void EvalState::SetInhStrct (const Map & new_inhstrct)
{
  this->inhstrct = new_inhstrct;
}

// GetAllSupers
// nm : AS`Name
// ==> set of AS`Name
SET<TYPE_AS_Name> EvalState::GetAllSupers(const TYPE_AS_Name & nm) const
{
  if (this->hchy.DomExists(nm))
    return this->hchy[nm];
  else {
    theStackMachine().SetCid(ASTAUX::GetCid(nm)); // 20090210
    RTERR::Error(L"GetAllSupers", RTERR_CLNM_NOT_DEFINED,
                        M42Sem(AUX::SingleNameToString(nm), NULL), Nil(),
                        Sequence().ImpAppend(AUX::SingleNameToString(nm)));
    return SET<TYPE_AS_Name>(); // dummy
  }
}

// AllSuperList
// index : nat
// supers : seq of AS`Name
// -> seq of AS`Name
SEQ<TYPE_AS_Name> EvalState::AllSuperList(int index, const SEQ<TYPE_AS_Name> & supers)
{
  if (index == supers.Length() + 1)
    return supers;
  else {
    SET<TYPE_AS_Name> news (GetAllSupers(supers[index]));
    news.ImpDiff(supers.Elems());
    SEQ<TYPE_AS_Name> newlist (news.ToSequence());
    SEQ<TYPE_AS_Name> supersArg (supers);
    supersArg.ImpConc(newlist);
    return AllSuperList(index + 1, supersArg);
  }
}

// GetClasses
// ==> map AS`Name to GLOBAL`SigmaClass
MAP<TYPE_AS_Name,TYPE_GLOBAL_SigmaClass> EvalState::GetClasses() const
{
  return this->classes;
}

// GetDefaultCons
// nm : AS`Name
// ==> bool
Bool EvalState::GetDefaultCons(const TYPE_AS_Name & nm) const
{
  return this->classes[nm].GetBool(pos_GLOBAL_SigmaClass_defaultcons);
}

// AddAbstract
// clsid : AS`Name
void EvalState::AddAbstract(const TYPE_AS_Name & clsid)
{
  this->abstracts.Insert(clsid);
}

// CheckIfAbstractClass
// name : AS`Name
// ==> bool
bool EvalState::CheckIfAbstractClass(const TYPE_AS_Name & name) const
{
  return this->abstracts.InSet(name);
}

// IsAClass
// name : AS`Name
// ==> bool
bool EvalState::IsAClass(const TYPE_AS_Name & name) const
{
  //return this->classes.DomExists(name);
  return this->clsnms.InSet(name);
}

// GetInhCon
// nm : AS`Name
// ==> GLOBAL`InhCon (set of AS`Name)
SET<TYPE_AS_Name> EvalState::GetInhCon(const TYPE_AS_Name & nm) const
{
  if (! this->classes.DomExists(nm)) {
    RTERR::Error(L"GetInhCon", RTERR_ID_UNKNOWN,
                        M42Sem(AUX::SingleNameToString(nm), NULL), Nil(), Sequence());
  }
  return this->classes[nm].GetSet(pos_GLOBAL_SigmaClass_inhcon);
}

// GetInstVars
// nm : AS`Name
// ==> seq of AS`InstAssignDef
SEQ<TYPE_AS_InstAssignDef> EvalState::GetInstVars(const TYPE_AS_Name & nm) const
{
  if (! this->classes.DomExists(nm)) {
    RTERR::Error(L"GetInstVars", RTERR_ID_UNKNOWN,
                        M42Sem(AUX::SingleNameToString(nm), NULL), Nil(), Sequence());
  }
  return this->classes[nm].GetSequence(pos_GLOBAL_SigmaClass_instvars);
}

// GetInstVarsTp
// nm : AS`Name
// ==> map AS`Name to AS`Type
MAP<TYPE_AS_Name,TYPE_AS_Type> EvalState::GetInstVarsTp(const TYPE_AS_Name & nm) const
{
  if (! this->classes.DomExists(nm)) {
    RTERR::Error(L"GetInstInit", RTERR_ID_UNKNOWN,
                        M42Sem(AUX::SingleNameToString(nm), NULL), Nil(), Sequence());
  }
  return this->classes[nm].GetMap(pos_GLOBAL_SigmaClass_instvars_utp);
}

// GetInstInv
// nm : AS`Name
// ==> STKM`SubProgram
TYPE_STKM_SubProgram EvalState::GetInstInv(const TYPE_AS_Name & nm) const
{
  if (! this->classes.DomExists(nm)) {
    RTERR::Error(L"GetInstInv", RTERR_ID_UNKNOWN,
                        M42Sem(AUX::SingleNameToString(nm), NULL), Nil(), Sequence());
  }
  return this->classes[nm].GetTuple(pos_GLOBAL_SigmaClass_inst_uinv).GetSequence(2);
}

// GetInstInitVal
// nm : AS`Name
// ==> SEM`InsStrct
TYPE_SEM_InsStrct EvalState::GetInstInitVal(const TYPE_AS_Name & nm) const
{
// 20100525 -->
// for no class environment
//  if (nm.GetSequence(pos_AS_Name_ids).IsEmpty())
//    return TYPE_SEM_InsStrct();
// <-- 20100525

  if (! this->classes.DomExists(nm)) {
    RTERR::Error(L"GetInstInitVal", RTERR_ID_UNKNOWN,
                        M42Sem(AUX::SingleNameToString(nm), NULL), Nil(), Sequence());
  }
  return this->classes[nm].GetMap(pos_GLOBAL_SigmaClass_inst_uinit_uval);
}

// SetInstInitVal
// name : AS`Name
// instval : SEM`InsStrct
void EvalState::SetInstInitVal(const TYPE_AS_Name & name, const TYPE_SEM_InsStrct & instval)
{
  TYPE_SEM_InsStrct inst_init_val; // map AS`Name to GLOBAL`ValueMap;
  SET<TYPE_AS_Name> dom_instval (instval.Dom());
  Generic nm;
  for (bool bb = dom_instval.First(nm); bb; bb = dom_instval.Next(nm)) {
    TYPE_GLOBAL_ValueMap instval_nm (instval[nm]); // map AS`Name to (SEM`VAL * AS`Access)
    SET<TYPE_AS_Name> dom_instval_nm (instval_nm.Dom()); 
    Generic inm;
    for (bool cc = dom_instval_nm.First(inm); cc; cc = dom_instval_nm.Next(inm)) {
      type_dUU3P t (instval_nm[inm]);   // SEM`VAL * [AS`Type] * AS`Access
      const TYPE_AS_Access & a (t.GetField(3));
      t.SetField(3, DEF::RealAccess(a, DEF::ACC_INST));
      instval_nm.ImpModify(inm, t);
    }
    inst_init_val.Insert(nm, instval_nm);
  }

  TYPE_GLOBAL_SigmaClass sigmacl (this->classes[name]);
  sigmacl.SetField(pos_GLOBAL_SigmaClass_inst_uinit_uval, inst_init_val);
  this->classes.ImpModify(name, sigmacl);
}

// GetVlsDef
// nm : AS`Name
// ==> seq of AS`ValueDef
SEQ<TYPE_AS_ValueDef> EvalState::GetVlsDef(const TYPE_AS_Name & nm) const
{
  if (! this->classes.DomExists(nm)) {
    RTERR::Error(L"GetVlsDef", RTERR_ID_UNKNOWN,
                        M42Sem(AUX::SingleNameToString(nm), NULL), Nil(), Sequence());
  }
  return this->classes[nm].GetSequence(pos_GLOBAL_SigmaClass_vls_udef);
}

// GetVlsInit
// nm : AS`Name
// ==> GLOBAL`ValueMap (map AS`Name to (SEM`VAL * AS`Access))
TYPE_GLOBAL_ValueMap EvalState::GetVlsInit(const TYPE_AS_Name & nm) const
{
  if (! this->classes.DomExists(nm)) {
    RTERR::Error(L"GetVlsInit", RTERR_ID_UNKNOWN,
                        M42Sem(AUX::SingleNameToString(nm), NULL), Nil(), Sequence());
  }
  return this->classes[nm].GetMap(pos_GLOBAL_SigmaClass_vls_uinit);
}

// GetAllOps
// nm : AS`Name
// ==> map AS`Name to SEM`ExplOP
Map EvalState::GetAllOps(const TYPE_AS_Name & nm) const
{
  if (! this->classes.DomExists(nm)) {
    RTERR::Error(L"GetAllOps", RTERR_ID_UNKNOWN,
                        M42Sem(AUX::SingleNameToString(nm), NULL), Nil(), Sequence());
  }

  // Used to call GetExplOps and GetImplOps.
  const TYPE_GLOBAL_SigmaClass & sigmacl (this->classes[nm]);
  return sigmacl.GetMap(pos_GLOBAL_SigmaClass_explops);
}

// GetAllOpsNmsSupers
// name : AS`Name
// ==> set of AS`Name
SET<TYPE_AS_Name> EvalState::GetAllOpsNmsSupers(const TYPE_AS_Name & name) const
{
  SET<TYPE_AS_Name> res (GetAllOps(name).Dom());
  SET<TYPE_AS_Name> allsupers (GetAllSupers(name));
  Generic opnms;
  for (bool bb = allsupers.First(opnms); bb; bb = allsupers.Next(opnms) )
    res.ImpUnion(GetAllOps(opnms).Dom());
  return res;
}

// LookupAllFnsOpsPolys
// clnm : AS`Name
// fnnm : AS`Name
// ==> bool * [ (SEM`CompExplFN | SEM`ExplOP | SEM`ExplPOLY) * AS`Access ]
Tuple EvalState::LookupAllFnsOpsPolys(const TYPE_AS_Name & clnm, const TYPE_AS_Name & fnnm) const
{
// TODO: overloaded
  if (! this->classes.DomExists(clnm)) {
    RTERR::Error(L"LookupAllFnsOpsPolys", RTERR_CLNM_NOT_DEFINED,
                        M42Sem(AUX::SingleNameToString(clnm), NULL), Nil(),
                        Sequence().ImpAppend(AUX::SingleNameToString(clnm)));
  }

  Generic res;
  if(this->classes[clnm].GetMap(pos_GLOBAL_SigmaClass_all_ufns_uops_upolys).DomExists(fnnm, res))
    return mk_(Bool(true), res);
  else
    return mk_(Bool(false), Nil());
}

// GetLocalTps
// AS`Name
// ==> map AS`Name to AS`TypeDef
MAP<TYPE_AS_Name,TYPE_AS_TypeDef> EvalState::GetLocalTps(const TYPE_AS_Name & nm) const
{
  if (! this->classes.DomExists(nm)) {
    RTERR::Error(L"GetLocalTps", RTERR_ID_UNKNOWN,
                        M42Sem(AUX::SingleNameToString(nm), NULL), Nil(), Sequence());
  }
  return this->classes[nm].GetMap(pos_GLOBAL_SigmaClass_localtps);
}

// GetRecSel
// nm : AS`Name
// ==> map AS`Name to GLOBAL`RecSel
Map EvalState::GetRecSel(const TYPE_AS_Name & nm) const
{
  if (! this->classes.DomExists(nm)) {
    RTERR::Error(L"GetRecSel", RTERR_ID_UNKNOWN,
                        M42Sem(AUX::SingleNameToString(nm), NULL), Nil(), Sequence());
  }
  return this->classes[nm].GetMap(pos_GLOBAL_SigmaClass_recsel);
}

// GetLocalHchy
// nm : AS`Name
// ==> map AS`Name to set of AS`Name
Map EvalState::GetLocalHchy(const TYPE_AS_Name & nm) const
{
  if (! this->classes.DomExists(nm)) {
    RTERR::Error(L"GetLocalHchy", RTERR_ID_UNKNOWN,
                        M42Sem(AUX::SingleNameToString(nm), NULL), Nil(), Sequence());
  }
  return this->classes[nm].GetMap(pos_GLOBAL_SigmaClass_localhchy);
}

// IsClassInit
// nm : AS`Name
// ==> bool
bool EvalState::IsClassInit(const TYPE_AS_Name & nm) const
{
/*
  if (this->classes.DomExists(nm)) {
    return this->classes[nm].GetBoolValue(pos_GLOBAL_SigmaClass_isinit);
  }
  else {
    return true;
  }
*/
  return this->initclasses.InSet(nm);
}

// ExpandClassName
// name : AS`Name
// orgcl : GLOBAL`OrigCl
// nm_s : set of AS`Name
// ==> bool * AS`Name
Tuple EvalState::ExpandClassName(const TYPE_AS_Name & name, const TYPE_GLOBAL_OrigCl & orgcl,
                                 const SET<TYPE_AS_Name> & nm_s)
{
  if (nm_s.InSet(name)) {
    return mk_(Bool(false), name);
  }

  if (IsAClass(name)) {
    return mk_(Bool(true), name);
  }
  else {
    TYPE_AS_Name clsnm;
    TYPE_AS_Name tpnm;
    if (name.GetSequence(pos_AS_Name_ids).Length() == 2) {
      clsnm = ASTAUX::GetFirstName(name);
      tpnm = ASTAUX::GetSecondName(name);
    }
    else {
      if (orgcl.Is(TAG_TYPE_GLOBAL_Start))
        return mk_(Bool(false), name);
      clsnm = orgcl;
      tpnm = name;
    }

    MAP<TYPE_AS_Name,TYPE_AS_TypeDef> tdm (GetLocalTps(clsnm));
    if (tdm.DomExists(tpnm)) {
      TYPE_AS_TypeDef td (tdm[tpnm]);
      TYPE_AS_Type tp (td.GetRecord(pos_AS_TypeDef_shape));
      if (tp.Is(TAG_TYPE_AS_TypeName)) {
        return ExpandClassName(tp.GetRecord(pos_AS_TypeName_name), clsnm, Set(nm_s).Insert(name));
      }
    }
    return mk_(Bool(false), name);
  }
}

// InitClasses
// ==> set of AS`Name
SET<TYPE_AS_Name> EvalState::InitClasses() const
{
/*
  SET<TYPE_AS_Name> initcls;
  SET<TYPE_AS_Name> dom_classes (this->classes.Dom());
  Generic clnm;
  for (bool bb = dom_classes.First(clnm); bb; bb = dom_classes.Next(clnm)) {
    if (IsClassInit(clnm)) {
      initcls.Insert(clnm);
    }
  }
  return initcls;
*/
  return this->initclasses;
}

// SetClassInit
// nm : AS`Name
// val : bool
void EvalState::SetClassInit(const TYPE_AS_Name & nm, const Bool & val)
{
  TYPE_GLOBAL_SigmaClass sigmacl (this->classes[nm]);
  sigmacl.SetField(pos_GLOBAL_SigmaClass_isinit, val);
  this->classes.ImpModify(nm, sigmacl);

  // init class name cache
  if (val) {
    this->initclasses.Insert(nm);
  }
  else {
    this->initclasses.ImpDiff(mk_set(nm));
  }
}

// GetSupers
// nm : AS`Name
// ==> set of AS`Name
SET<TYPE_AS_Name> EvalState::GetSupers(const TYPE_AS_Name & nm) const
{
  return (GetInhCon(nm));
}

// LookUpConstructor
// nm : AS`Name
// valL : seq of SEM`VAL
// ==> STKM`DebugCmd
TYPE_STKM_DebugCmd EvalState::LookUpConstructor(const TYPE_AS_Name & nm, const SEQ<TYPE_SEM_VAL> & valL)
{
  Generic constlocal (LookUpConstructorLocal(nm, valL));
  if (!constlocal.IsNil())
    return constlocal;
  else
  {
    // TODO: constructor must be local
// 20130530 -->
//    theStackMachine().SetCid(ASTAUX::GetCid(nm));
//    RTERR::Error(L"LookUpConstructor", RTERR_NOCONSTRUCTOR,
//                    M42Sem(AUX::SingleNameToString(nm), NULL), Nil(), Sequence());
// <-- 20130530
    SET<TYPE_AS_Name> supers (GetAllSupers(nm));
    Map conssup;
    Generic cl;
    for (bool bb = supers.First(cl); bb; bb = supers.Next(cl))
    {
      Generic thisLocal (LookUpConstructorLocal(cl, valL));
      if (!thisLocal.IsNil())
        conssup.Insert(cl, thisLocal);
    }

    switch(conssup.Size()) {
      case 0: {
        theStackMachine().SetCid(ASTAUX::GetCid(nm)); // 20090210
        RTERR::Error(L"LookUpConstructor", RTERR_NOCONSTRUCTOR,
                            M42Sem(AUX::SingleNameToString(nm), NULL), Nil(), Sequence());
        break;
      }
      case 1: {
        TYPE_AS_Name cl (conssup.Dom().GetElem()); 
// 20080818 -->
        vdm_log << L"*** WARNING ***" << endl;
        vdm_log << L"Class \'" << ASTAUX::ASName2String(nm) << L"\' hasn't the needed constructor." << endl;
        vdm_log << L"Constructor of Class \'" << ASTAUX::ASName2String(cl) << "\' is used." << endl;
        vdm_log << L"Instance variable(s) defined in subclass of Class \'" << ASTAUX::ASName2String(cl)
                << "\' may not be initialized." << endl;
// <-- 20080818
        return conssup[cl];
      }
      default: {
        Tuple tupExists (ExistsOneChild(conssup.Dom(), conssup.Dom()));
        if (tupExists.GetBoolValue(1))
        {
          const TYPE_AS_Name & child (tupExists.GetRecord(2));
// 20080818 -->
          vdm_log << L"*** WARNING ***" << endl;
          vdm_log << L"Class \'" << ASTAUX::ASName2String(nm) << L"\' hasn't the needed constructor." << endl;
          vdm_log << L"Constructor of Class \'" << ASTAUX::ASName2String(child) << "\' is used." << endl;
          vdm_log << L"Instance variable(s) defined in subclass of Class \'" << ASTAUX::ASName2String(child)
                  << "\' may not be initialized." << endl;
// <-- 20080818
          return conssup[child];
        }
        else
          RTERR::Error(L"LookUpConstructor", RTERR_MULT_DEF,
                              M42Sem(AUX::SingleNameToString(nm), NULL), Nil(), Sequence());
        break;
      }
    }
  }
  return TYPE_STKM_SubProgram(); // dummy
}

// LookUpConstructorLocal
// nm : AS`Name
// valL : seq of SEM`VAL
// ==> [STKM`DebugCmd]
Generic EvalState::LookUpConstructorLocal(const TYPE_AS_Name & nm, const SEQ<TYPE_SEM_VAL> & valL)
{
  const TYPE_GLOBAL_SigmaClass & sigmacl (this->classes[nm]);
  const type_dL13STKM_DebugCmdCU2PM & consts (sigmacl.GetMap(pos_GLOBAL_SigmaClass_constructor));
                // map seq of AS`Type to (STKM`DebugCmd * AS`Access)

  switch(valL.Length()) {
    case 0: {
      // default constructor always exists.
      Tuple t (consts[SEQ<TYPE_AS_Type>()]);  // (STKM`DebugCmd * AS`Access)
      if (AccessOk(t.GetInt(2), theStackMachine().GetPrevCl(), nm))
        return t.GetField(1); // STKM`DebugCmd
      else
      {
        RTERR::Error(L"LookUpConstructorLocal", RTERR_NOT_IN_SCOPE,
                            M42Sem(AUX::SingleNameToString(nm), NULL),
                            Nil(), Sequence());
        return Nil();
      }
    }
    default: {
      Set dom_consts (consts.Dom());
      size_t curlen = valL.Length();

      Generic g;
      for (bool bb = dom_consts.First(g); bb; bb = dom_consts.Next(g))
      {
        SEQ<TYPE_AS_Type> typeL (g); // seq of AS`Type
        if ((size_t)(typeL.Length()) == curlen)
        {
          bool allSubType = true;
          for (size_t i = 1; i <= curlen && allSubType; i++)
            allSubType = (RealSubType(valL[i], typeL[i], false));
      
          if (allSubType)
          {
            Tuple t (consts[typeL]);              // (STKM`DebugCmd * AS`Access)
            if (AccessOk(t.GetInt(2), theStackMachine().GetPrevCl(), nm))
              return t.GetField(1); // STKM`DebugCmd
            else
            {
              RTERR::Error(L"LookUpConstructorLocal", RTERR_NOT_IN_SCOPE,
                                  M42Sem(AUX::SingleNameToString(nm), NULL),
                                  Nil(), Sequence());
              return Nil();
            }
          }
        }
      }
      return Nil();
    }
  }
  return Nil(); // dummy
}

// AClass
// nm : AS`Name
// ==> bool
bool EvalState::AClass(const TYPE_AS_Name & nm) const
{
  return this->clsnms.InSet(nm);
}

// GetAllFns (not in spec)
// nm : AS`Name
// ==> map AS`Name to SEM`CompExplFN
Map EvalState::GetAllFns(const TYPE_AS_Name & nm) const
{
  if (! this->classes.DomExists(nm)) {
    RTERR::Error(L"GetAllFns", RTERR_ID_UNKNOWN, M42Sem(AUX::SingleNameToString(nm), NULL), Nil(), Sequence());
  }

  TYPE_GLOBAL_SigmaClass sigmacl (this->classes[nm]);
  return sigmacl.GetMap(pos_GLOBAL_SigmaClass_explfns);
}

// IsStaticalyDependingClassesInit
// nm : AS`Name
// ==> bool
bool EvalState::IsStaticalyDependingClassesInit(const TYPE_AS_Name & nm) const
{
  return GetNotInitStaticalyDependingClasses(nm).IsEmpty();
}

// GetNotInitStaticalyDependingClasses
// nm : AS`Name
// ==> set of AS`Name
SET<TYPE_AS_Name> EvalState::GetNotInitStaticalyDependingClasses(const TYPE_AS_Name & nm) const
{
  return GetAllDeps(nm).ImpDiff(InitClasses());
}

// GetAllDeps
// nm : AS`Name
// ==> set of AS`Name
SET<TYPE_AS_Name> EvalState::GetAllDeps(const TYPE_AS_Name & nm) const
{
  if (this->alldeps.DomExists(nm))
    return this->alldeps[nm];
  else {
    RTERR::Error(L"GetAllDeps", RTERR_CLNM_NOT_DEFINED,
                        M42Sem(AUX::SingleNameToString(nm), NULL), Nil(),
                        Sequence().ImpAppend(AUX::SingleNameToString(nm)));
  }
  return SET<TYPE_AS_Name>();
}

void EvalState::MergeClassAndStaticHierarchy()
{
  Set classes_s (this->classes.Dom());
  Map deps;

  Generic nm;
  for (bool bb = classes_s.First(nm); bb; bb = classes_s.Next(nm)) {
    SET<TYPE_AS_Name> allsupers (GetAllSupers(nm));
    SET<TYPE_AS_Name> dnm_s (GetAllSDeps(nm));
    Generic dnm;
    for (bool dd = dnm_s.First(dnm); dd; dd = dnm_s.Next(dnm)) {
      Tuple t (ExpandClassName(dnm, nm, Set()));
      if (t.GetBoolValue(1)) {
        allsupers.Insert(t.GetRecord(2));
      }
      else {
        allsupers.Insert(dnm);
      }
    }

    SET<TYPE_AS_Name> supers;

    while(!allsupers.IsEmpty()) {
      SET<TYPE_AS_Name> more_supers;
      Generic clnm;
      for (bool cc = allsupers.First(clnm); cc; cc = allsupers.Next(clnm)) {
        supers.Insert(clnm);
        more_supers.ImpUnion(GetAllSupers(clnm));
        more_supers.ImpUnion(GetAllSDeps(clnm));
      }
      if (more_supers.InSet(nm)) {
        RTERR::Error(L"MergeClassAndStaticHierarchy", RTERR_CIRC_CL_DEPENDENCY, Nil(), Nil(), Sequence());
      }

      allsupers.Clear();
      allsupers.ImpUnion(more_supers);
    }
    deps.ImpModify(nm, supers);
  }
  this->alldeps.Clear();
  this->alldeps.ImpOverride(deps);
}
#endif // VDMPP

// SetSDeps
// sdep : map AS`Name to set of AS`Name
void EvalState::SetSDeps(const Map & sdep)
{
  this->sdep.Clear();
  this->sdep.ImpOverride(sdep);
}

// GetAllSDeps
// nm : AS`Name
// ==> set of AS`Name
SET<TYPE_AS_Name> EvalState::GetAllSDeps(const TYPE_AS_Name & nm) const
{
  if (this->sdep.DomExists(nm))
    return this->sdep[nm];
  else {
    RTERR::Error(L"GetAllSDeps", RTERR_CLNM_NOT_DEFINED,
                        M42Sem(AUX::SingleNameToString(nm), NULL), Nil(),
                        Sequence().ImpAppend(AUX::SingleNameToString(nm)));
  }
  return SET<TYPE_AS_Name>();
}

// DLInsertClassLib
// mod_nm : AS`Name
// useslib : AS`TextLit
// name : AS`Name
void EvalState::DLInsertClassLib (const TYPE_AS_Name & mod_nm,
                                  const TYPE_AS_TextLit & useslib,
                                  const TYPE_AS_Name & name)
{
  try {
    this->dlfactory.InsertClassLib(mod_nm, useslib, name);
  }
  catch (DLException e)
  {
    vdm_iplog << e.GetErrorMessage() << endl << flush;
    RTERR::Error(L"DLInsertClassLib", RTERR_DLMODULE_LOAD_ERROR, Nil(), Nil(), Sequence());
  }
}

// DLCreateNEW
// name : AS`Name
// dlobject : [DLObject]
// ==> [VDMContainer<DlClassInstanceHolder>]
Generic EvalState::DLCreateNEW(const TYPE_AS_Name & name, const Generic & dlobject)
{
  return this->dlfactory.GetInstance(name, dlobject);
}

#ifdef VDMSL
Generic EvalState::DLCallOp(const TYPE_AS_Name& mod_nm, const TYPE_AS_Name& op, bool isvoid, const Generic & args)
{
  return this->dlfactory.CallOp(mod_nm, op, isvoid, args);
}
#endif // VDMSL

#ifdef VDMPP
Tuple EvalState::DLCallOp(const TYPE_AS_Name & clname,
                          const TYPE_AS_Name & op,
                          const SEQ<TYPE_SEM_VAL> & args,
                          const TYPE_SEM_OBJ_uRef & curobj)
{
  if (this->obj_tab.DomExists(curobj))
  {
    Generic dlclassp (this->obj_tab[curobj].GetField(pos_GLOBAL_OBJ_uDesc_DlClassInstancePtr));
    if (!dlclassp.IsNil())
      return DlClassFactory::DlMethodCall(dlclassp, clname, op, args);
  }
  vdm_iplog << L"Calling dlclass method " << ASTAUX::ASName2String(clname) << L"`" << ASTAUX::ASName2String(op)
            << L" after dlclose is impossible" << endl
            << L"since the external object was deleted" << endl << flush;
  RTERR::Error(L"DLCallOp", RTERR_DLCLASS_APPLY_AFTER_DLCLOSE, Nil(), Nil(), Sequence());
  return Tuple(); // dummy
}
#endif // VDMPP

#ifdef VDMPP
void EvalState::GC(bool forced, bool more)
{
  // TODO:
  // SCHDTP`Blocked.objref
  // SCHDTP`ThreadInfo.obj
  // STKM`EvaluationState.call_stack EOP...
  this->obj_tab.GC(forced);

  if (more)
  {
#ifndef VICE
    if (theStackMachine().GetObjLLen() > 0) return;
    if (theStackMachine().EvalStackLevel() > 0) return;

    Set ds (CreateDanglingSet());
    if (!ds.IsEmpty())
    {
      //vdm_iplog << ds << endl;
      Generic r;
      for (bool bb = ds.First(r); bb; bb = ds.Next(r))
      {
        this->obj_tab.RemElem(r);
      }
    }
#endif // VICE
  }
}
#endif // VDMPP

#ifdef VDMPP
void ObjTabCt::reset() 
{
  this->refs.Clear();
  this->get_shared_ref().Clear();
}

bool ObjTabCt::DomExists(const TYPE_SEM_OBJ_uRef & ref) const
{
  return this->get_const_ref().DomExists(ref.GetIntValue(pos_SEM_OBJ_uRef_ref));
}

const TYPE_GLOBAL_OBJ_uDesc & ObjTabCt::operator[](const TYPE_SEM_OBJ_uRef & ref) const
{
  return this->get_const_ref().operator[](ref.GetIntValue(pos_SEM_OBJ_uRef_ref));
}

ObjTabCt & ObjTabCt::ImpModify(const TYPE_SEM_OBJ_uRef & ref, const TYPE_GLOBAL_OBJ_uDesc & desc)
{
  this->get_oneref_ref().ImpModify(ref.GetIntValue(pos_SEM_OBJ_uRef_ref), desc);
  if (!this->refs.InSet(ref))
    this->refs.Insert(ref);
  return *this;
}

ObjTabCt & ObjTabCt::RemElem(const TYPE_SEM_OBJ_uRef & ref)
{
  this->get_oneref_ref().RemElem(ref.GetIntValue(pos_SEM_OBJ_uRef_ref));
  if (this->refs.InSet(ref))
    this->refs.RemElem(ref);
  return *this;
}

Map ObjTabCt::GetMap()
{
  Map m;
  SET<TYPE_SEM_OBJ_uRef> r_s(this->refs);
  Generic ref;
  for (bool bb = r_s.First(ref); bb; bb = r_s.Next(ref))
    m.Insert(ref, this->get_const_ref().operator[](Record(ref).GetIntValue(pos_SEM_OBJ_uRef_ref)));
  return m;
}

Map ObjTabCt::GetRefCountMap() const
{
  return this->get_const_ref().GetRefCountMap();
}

void ObjTabCt::GC(bool forced)
{
  if (!forced)
  {
    this->gccount++;
    this->gccount = (this->gccount % 500);
    if (this->gccount != 0) return;
  }
  else
    this->gccount = 0;

  int remcount = 0;
  bool stop = this->refs.IsEmpty();
  while(!stop)
  {
    Set r_s (this->refs);
    int64_t size = r_s.Card();
    Generic r;
    for (bool bb = r_s.First(r); bb; bb = r_s.Next(r))
    {
      //wcout << Record(r).GetIntValue(pos_SEM_OBJ_uRef_ref) << L" " << r.GetRefCount() << endl;
      if (r.GetRefCount() == 2) // this->refs and r
      {
        this->refs.RemElem(r);
        this->get_shared_ref().RemElem(Record(r).GetIntValue(pos_SEM_OBJ_uRef_ref));
        remcount++;
        r = Nil();
      }
    }
    stop = (size == this->refs.Card()) || (remcount > 20);
/*
    Set ref_s (this->get_const_ref().GetGCSet());
    if (ref_s.IsEmpty()) break;
    Generic r;
    for (bool bb = ref_s.First(r); bb; bb = ref_s.Next(r))
    {
      this->get_shared_ref().RemElem(Record(r).GetIntValue(pos_SEM_OBJ_uRef_ref));
      remcount++;
    }
    this->refs.ImpDiff(ref_s);
    stop = (remcount > 20);
*/
  }
}

template <>
#ifdef _MSC_VER
inline
#endif // _MSC_VER
void VDMContainerVal<ObjTab>::ostream_out(wostream & o, const VDMFormatter & v) const
{
  this->value.ostream_out(o, v);
}

bool ObjTab::DomExists(int64_t ref) const
{
  return (this->value.find(ref) != this->value.end());
}

void ObjTab::ostream_out(wostream & o, const VDMFormatter & v) const
{
  Map m;
  for (ObjTabMapType::const_iterator miter(this->value.begin()); miter != this->value.end(); miter++)
  {
    TYPE_GLOBAL_OBJ_uDesc desc (miter->second);
    const TYPE_AS_Name & clnm (desc.GetRecord(pos_GLOBAL_OBJ_uDesc_sem).GetField(pos_SEM_OBJ_tp));
    m.ImpModify(TYPE_SEM_OBJ_uRef().Init(Int(miter->first),clnm), desc);
  }
  INT2Q::h2gAS(m).ostream_out(o, v);
}

const TYPE_GLOBAL_OBJ_uDesc & ObjTab::operator[](int64_t ref) const
{
  ObjTabMapType::const_iterator miter (this->value.find(ref));
  if (miter != this->value.end()) {
    return miter->second;
  } else {
    M4LibError::ReportError(ML_NOT_IN_DOM, L"ObjTab::Get: key not in the domain of map.");
    return this->dummy;
  }
}

void ObjTab::ImpModify(int64_t ref, const TYPE_GLOBAL_OBJ_uDesc & desc)
{
  pair<ObjTabMapType::iterator, bool> p (this->value.insert(ObjTabMapType::value_type(ref, desc)));
  if (!p.second) { // Existed!
    // This is reentrant safe since we keep a ref to the value to be overwritten.
    Generic keepref (p.first->second);
    p.first->second = desc;
  }
}

void ObjTab::RemElem(int64_t ref)
{
  const ObjTabMapType::iterator miter (this->value.find(ref));
  if (miter != this->value.end()) {
    // This is reentrant safe since we keep a ref to the value to be erased.
    ObjTabMapType::value_type keepref (*miter);
    this->value.erase(miter);
  } else {
    M4LibError::ReportError(ML_NOT_IN_DOM, L"ObjTab::RemElem: key not in the domain of map");
  }
}

Map ObjTab::GetRefCountMap() const
{
  Map m;
  for (ObjTabMapType::const_iterator miter(this->value.begin()); miter != this->value.end(); miter++)
  {
    DRPtr p (Record(miter->second).GetField(pos_GLOBAL_OBJ_uDesc_ref_ucount));
    m.ImpModify(Int(miter->first), Int(p.GetRC()));
  }
  return m;
}

Set ObjTab::GetGCSet() const
{
  Set s;
  for (ObjTabMapType::const_iterator miter(this->value.begin()); miter != this->value.end(); miter++)
  {
    DRPtr p (Record(miter->second).GetField(pos_GLOBAL_OBJ_uDesc_ref_ucount));
    if (p.GetRC() == 1)
    {
      s.Insert(TYPE_SEM_OBJ_uRef().Init(Int(miter->first),
                  Record(miter->second).GetRecord(pos_GLOBAL_OBJ_uDesc_sem).GetRecord(pos_SEM_OBJ_tp)));
    }
  }
  return s;
}

int vdm_Compare(const ObjTab & v1, const ObjTab & v2)
{
  return 0;
}
#endif // VDMPP

