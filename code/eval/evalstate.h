/***
*  * WHAT
*  *    Header for translation functions.
*  * FILE
*  *    $Source:$
*  * PROJECT
*  *    VDM-SL Toolbox
*  * COPYRIGHT
*  *    (C) Kyushu University
***/

#ifndef __evalstate_h__
#define __evalstate_h__

#include "metaiv.h"
#include "AS.h"
#include "GLOBAL.h"
#include "semvalues.h"
//#ifdef VDMPP
//#include "objtab.h"
//#endif // VDMPP
#include "stackeval.h"
#include "dlfactory.h"
#if __cplusplus >= 201103L
#include <unordered_map>
#endif

#ifdef VDMPP
class ObjTab
{
private:
#if __cplusplus >= 201103L
  typedef std::unordered_map<int64_t, TYPE_GLOBAL_OBJ_uDesc> ObjTabMapType;
#else
  typedef std::map<int64_t, TYPE_GLOBAL_OBJ_uDesc> ObjTabMapType;
#endif
  ObjTabMapType value;
  TYPE_GLOBAL_OBJ_uDesc dummy;

public:
  ObjTab() {};
  ~ObjTab() { this->value.clear(); };

  void Clear() { this->value.clear(); };

  bool DomExists(int64_t ref) const;
  const TYPE_GLOBAL_OBJ_uDesc & operator[](int64_t ref) const;
  void ImpModify(int64_t ref, const TYPE_GLOBAL_OBJ_uDesc & desc);
  void RemElem(int64_t ref);
  Map GetRefCountMap() const;
  Set GetGCSet() const;
  int64_t Size() const { return this->value.size(); };
  void ostream_out(wostream & o, const VDMFormatter & v) const;
};

int vdm_Compare(const ObjTab & v1, const ObjTab & v2);
void vdm_ostream_out(const ObjTab & value, wostream & o);

class ObjTabCt : public VDMContainer<ObjTab>
{
private:
  int gccount;
  SET<TYPE_SEM_OBJ_uRef> refs;

public:
  ObjTabCt() : gccount(0) {};
  ~ObjTabCt() { this->refs.Clear(); };

  void reset();
  bool DomExists(const TYPE_SEM_OBJ_uRef & ref) const;
  const TYPE_GLOBAL_OBJ_uDesc & operator[](const TYPE_SEM_OBJ_uRef & ref) const;
  const SET<TYPE_SEM_OBJ_uRef> & Dom() const { return this->refs; };
  ObjTabCt & ImpModify(const TYPE_SEM_OBJ_uRef & ref, const TYPE_GLOBAL_OBJ_uDesc & desc);
  ObjTabCt & RemElem(const TYPE_SEM_OBJ_uRef & ref);
  int64_t Size() const { return this->refs.Card(); };
  Map GetMap();
  Map GetRefCountMap() const;
  void GC(bool forced);
};
#endif // VDMPP

class EvalState
{
private:
#ifdef VDMSL
  MAP<TYPE_AS_Name, TYPE_GLOBAL_SigmaMO> mods;  // map AS`Name to GLOBAL`SigmaMO
  MAP<TYPE_AS_Name, TYPE_GLOBAL_SigmaIMO> imods; // map AS`Name to GLOBAL`SigmaIMO
  bool isTypeJudgement;            // bool
  DlClassFactory dlfactory;

  TYPE_GLOBAL_SigmaMO nullSigmaMo;
#endif // VDMSL

#ifdef VDMPP
  ObjTabCt obj_tab;
  MAP<TYPE_AS_Name, Record> classes; // Representation of read classes
                              // map AS`Name to GLOBAL`SigmaClass
  MAP<TYPE_AS_Name, Set> hchy; // Global hierarchy
                              // GLOBAL`Hierarchy
                              // Hierarchy = map AS`Name to set of AS`Name
//  Bool Init
  bool Init_flag;             // Flag for initialisation.
  Map d_objs;                 // User defined objects in the debugger.
                              // map AS`Name to
                              //    (SEM`OBJ_Ref * AS`Expr * seq of char)
  SEQ<TYPE_AS_Name> d_objs_order; // order in which d_objs was created
  SET<TYPE_AS_Name> InitPool; // set of AS`Name
  Map inhstrct;               // inheritance structure for initialisation
                              // purpose.
                              // GLOBAL`InhStrct
                              // InhStrct = 
                              //      map AS`Name to (seq of set of AS`Name)
  SET<TYPE_AS_Name> abstracts;// set of AS`Name
  Int obj_ref_no;              // nat
  SET<TYPE_AS_Name> clsnms;   // Set of available class names
  Map dlops;                  // map AS`Name to token
  DlClassFactory dlfactory;
  bool isTypeJudgement;

  SEQ<TYPE_AS_Name> initclstack;      // seq of AS`Name

  MAP<TYPE_AS_Name, Set> alldeps;

  SET<TYPE_AS_Name> initclasses;

  Map lookupOpFctPolyCache;
  Map isValueCache;
  Map objCache;
#endif // VDMPP
  Map tagCache;
  Map typeInvCache;          // map AS`Name to map AS`Name to STKM`SubProgram
  Map typeDefCache;
  Map recSelCache;
#ifdef VDMSL
  Map renRecSelCache;
  Map funOpPolyCache;
  Map globalValCache;
#endif // VDMSL

  MAP<TYPE_AS_Name, Set> sdep;        // static reference dependency
                                      // map AS`Name to set of AS`Name
  SEQ<TYPE_AS_TypeName> dtc_typenm_l; // loop check buffer for dtc

#ifdef VDMPP
  MAP<Int, SEQ<TYPE_AS_Expr> > newargs;
  MAP<Int, TYPE_AS_Name> bindname;
#endif // VDMPP

public:
  EvalState();
  ~EvalState();

#ifdef VDMPP
public:
  wstring PrintOID(const Generic & objref);
#endif // VDMPP

private:
  void GetInitSigma();

public:
  void Init_Sigma(bool ast_is_new);

#ifdef VDMPP
  Tuple Get_obj_tab();
  SET<TYPE_SEM_OBJ_uRef> GetObjRefsOfClass(const TYPE_AS_Name & clnm) const;
  Generic GetObjRefOfClass(const TYPE_AS_Name & clnm) const;
  bool IsInObjTab(const TYPE_SEM_OBJ_uRef &) const;
  TYPE_GLOBAL_OBJ_uDesc Lookup_obj_tab(const TYPE_SEM_OBJ_uRef &) const;
  void Update_obj_tab(const TYPE_SEM_OBJ_uRef &, const TYPE_GLOBAL_OBJ_uDesc &);
  Generic LookupProcId(const TYPE_SEM_OBJ_uRef & o) const;
  void SetNewArgs(const TYPE_SEM_OBJ_uRef & ref, const SEQ<TYPE_SEM_VAL> & args);
  SEQ<TYPE_AS_Expr> GetNewArgs(const TYPE_SEM_OBJ_uRef & ref);
  void SetBindName(const TYPE_SEM_OBJ_uRef & ref, const TYPE_AS_Name & nm);
  Generic GetBindName(const TYPE_SEM_OBJ_uRef & ref);
#endif // VDMPP

  void TranslateAST (const TYPE_AS_Document & ast, bool ast_is_new);

#ifdef VDMPP
  SET<TYPE_AS_Name> StaticInsts(const SEQ<TYPE_AS_InstanceVarDef> & instdefs);
#ifdef VICE
  void AddCPUAndBUSDefs(const Map & sys_m);
  void AddHistDeps(const TYPE_AS_Name & clsnm,
                   const TYPE_AS_Name & opnm,
                   const SET<TYPE_AS_Name> & deps,
                   const SET<TYPE_AS_Name> & histdeps);
#endif // VICE
#endif // VDMPP
#ifdef VDMSL
  void InitializeGlobalVal (const TYPE_AS_Name & name);
#endif //VDMSL

  void InitializeGSGV (bool ast_is_new);
#ifdef VDMSL
  bool CheckStateInvariant ();
  Generic GetModuleInvariant (const TYPE_AS_Name & mod_name);
#endif // VDMSL
  Tuple ExtractTagNameCached(const TYPE_AS_Name & name);
#ifdef VDMPP
  bool CheckInstanceInvariant() /*const*/;

  TYPE_SEM_OBJ GetCachedOBJ(const TYPE_AS_Name & clsnm);

private:
  void PushInitPool(const TYPE_AS_Name & nm);
  void PopInitPool(const TYPE_AS_Name & nm);
  void Init_classes();

public:
  bool StillInit();

private:
  void RemoveClassValues();
  void InitTheClasses(const SET<TYPE_AS_Name> & cl);

public:
  void InitClassName(const TYPE_AS_Name & nm);

private:
  void InitStaticInsts(const TYPE_AS_Name &, const SEQ<TYPE_AS_InstanceVarDef> &, bool ast_is_new);
  void PopInitClass();
  void PushInitClass(const TYPE_AS_Name &);
  bool IsSuperClassesInit(const TYPE_AS_Name & nm) const;
  bool IsStaticalyDependingClassesInit(const TYPE_AS_Name & nm) const;
  SET<TYPE_AS_Name> GetNotInitStaticalyDependingClasses(const TYPE_AS_Name & nm) const;
#endif // VDMPP

#ifdef VDMSL
private:
  void OM_TransInsertModule (const Record & mod_sig);
public:
  const TYPE_GLOBAL_SigmaMO & GetModule(const TYPE_AS_Name & name) const;
  Record GetAnyModule(const TYPE_AS_Name & name) const;
  bool IsEmptyMods();
  void PushModuleIfAny();
private:
//  void UpdateRenameAndImport (const TYPE_AS_Name & mod_name, const TYPE_AS_Name & new_id, const Generic & imp);
  Tuple IsGlobalState (const TYPE_AS_Name & loc_name, const TYPE_AS_Name & mod_name);
  TYPE_GLOBAL_State GetGlobalState (const TYPE_AS_Name & loc_name, const TYPE_AS_Name & mod_name);
  void SetGlobalState (const TYPE_AS_Name & loc_name, const TYPE_AS_Name & mod_name, const TYPE_GLOBAL_State & s_val);
public:
  bool IsTheState(const TYPE_AS_Name & name, const TYPE_AS_Name & mod_name);
  TYPE_DYNSEM_SEM_SemRecord GetTheState(const TYPE_AS_Name & name, const TYPE_AS_Name & mod_name);
  void SetTheState(const TYPE_AS_Name & name, const TYPE_AS_Name & mod_name, const TYPE_DYNSEM_SEM_SemRecord & rec);
  TYPE_GLOBAL_StateMap GetCurrentState (const TYPE_AS_Name & mod_name) const;
  TYPE_GLOBAL_StateMap GetModuleState (const TYPE_AS_Name & name) const;
  Tuple IsGlobalVal (const TYPE_AS_Name & loc_name, const TYPE_AS_Name & mod_name);
#endif // VDMSL

#ifdef VDMPP
public:
  void SetInstanceVar(const TYPE_AS_Name &, const TYPE_SEM_VAL & val_v, const Tuple & lookup);
  bool IsDLClass(const TYPE_AS_Name & cls);
private:
  void InsertDLClass(const TYPE_AS_Name & cls);
  void InsertDLOp(const TYPE_AS_Name & cls, const TYPE_AS_Name & fnop);
public:
  bool IsDLOp(const TYPE_AS_Name & cls, const TYPE_AS_Name & fnop);
private:
  void RegisterDLClass(const TYPE_AS_Name & modnm, const TYPE_AS_TextLit & textlit, const Generic & definitions);
#endif // VDMPP
public:
  TYPE_SEM_VAL EvalFctTypeInstExpr (const TYPE_SEM_POLY & fct_v, const SEQ<TYPE_AS_Type> & inst );
#ifdef VDMPP
  TYPE_SEM_VAL EvalOverFctTypeInstExpr (const TYPE_SEM_OverOPFN & fct_v, const SEQ<TYPE_AS_Type> & inst );
#endif // VDMPP
  bool IsSubTypeName (const TYPE_SEM_VAL & val_v, const TYPE_AS_TypeName & tp, bool checkinv) /*const*/;
  TYPE_STKM_SubProgram GetCachedTypeInv(const TYPE_AS_Name & clmod,
                                        const TYPE_AS_Name & name,
                                        const TYPE_AS_Invariant & inv);
  Tuple GetCachedTypeDef(const TYPE_AS_Name & name);
  bool SubType (const TYPE_SEM_VAL & val_v, const TYPE_GLOBAL_Type & gtp) /*notconst*/;

  void SetTypeJudgement();
  void UnsetTypeJudgement();
  bool RealSubType(const TYPE_SEM_VAL & val_v, const TYPE_GLOBAL_Type & gtp, bool checkinv) /*notconst*/;
  bool SubTypeAS(const TYPE_GLOBAL_Type & ltp, const TYPE_GLOBAL_Type & rtp);
  TYPE_SEM_VAL LookUp (const TYPE_AS_Name & name);

  Tuple GetCachedRecSel(const TYPE_AS_Name & name);

#ifdef VDMSL
  Tuple GetCachedRenRecSel(const TYPE_AS_Name & name);
  Tuple GetCachedFunOpPolyVal(const TYPE_AS_Name & name, const TYPE_AS_Name & modnm);
  Tuple GetCachedGlobalVal(const TYPE_AS_Name & name, const TYPE_AS_Name & modnm);
#endif // VDMSL

#ifdef VDMPP
public:
  Tuple IsInObjScope(const TYPE_AS_Name& name, const Generic & oldstate);
  Tuple ExistsOneChild(const SET<TYPE_AS_Name> & cl_s, const SET<TYPE_AS_Name> & full_s) const;
private:
  Tuple IsValue(const TYPE_AS_Name & nm);
public:
  Tuple CachedIsValue(const TYPE_AS_Name & nm);
  Tuple LookOpFctPoly(const TYPE_AS_Name & name);
  Tuple LookStaticOpFctPoly(const TYPE_AS_Name & clnm, const TYPE_AS_Name & name);
private:
  Tuple LookOpFctPoly_(const TYPE_AS_Name & clnm, const TYPE_AS_Name & objnm,
                       const TYPE_AS_Name & name, const TYPE_AS_Name & thename);
  Tuple LookCachedOpFctPoly_(const TYPE_AS_Name & clnm, const TYPE_AS_Name & objnm,
                             const TYPE_AS_Name & name, const TYPE_AS_Name & thename);
  SEQ<TYPE_AS_Type> ConvertTypeNameList(const SEQ<TYPE_AS_Type> & tp_l, const SET<TYPE_AS_Name> & nm_s);
  TYPE_AS_Type ConvertTypeName(const TYPE_AS_Type & type, const SET<TYPE_AS_Name> & nm_s);
  TYPE_AS_Type FindType(const TYPE_SEM_VAL &) const;
  SEQ<TYPE_AS_Type> FindTypeDom (const TYPE_SEM_VAL &) const;
  Generic LookUpOverloaded(const TYPE_AS_Name &, const TYPE_AS_Name &, const Set &);
public:
  Generic LookUpOverInClass(const TYPE_AS_Name &, const TYPE_AS_Name &);
  Tuple LookUpStatic(const TYPE_AS_Name &);
private:
  TYPE_SEM_VAL ReturnLookUp(const Generic & val, const int err);
  Tuple IsDObjs(const TYPE_AS_Name & name);
#endif //VDMPP

#ifdef VDMSL
public:
  TYPE_AS_Name ExtractModule (const TYPE_AS_Name & nm) const;
#endif // VDMSL

public:
  Generic GetPreCond(const TYPE_AS_Name &, const TYPE_AS_Name &);
  void EvalStateDesignator (const TYPE_STKM_StateDesignator & sd, const TYPE_SEM_VAL & val_v);
private:
  Tuple ExpandTypeDef(const TYPE_AS_Type & name);
#ifdef VDMPP
  TYPE_SEM_OBJ_uRef ModifyInstanceVar(const TYPE_SEM_OBJ_uRef &, const SEQ<Record> & _l, const TYPE_SEM_VAL &);
#endif //VDMPP
  Tuple LookUpSD (const TYPE_STKM_StateDesignator & sd);
  TYPE_SEM_VAL CreateUndefVal (const TYPE_GLOBAL_Type & type);
  TYPE_SEM_VAL ModifyValue(const TYPE_SEM_VAL & scomp_v, const SEQ<Record> & index_l, const TYPE_SEM_VAL & val_v);
  void ModifyValueId(const TYPE_AS_Name & id, const TYPE_SEM_VAL & val_v, const Generic & glres, const Generic & llres);

  Tuple CheckMap (const Tuple & lusd) const;
  TYPE_SEM_VAL UpdateVal (const TYPE_SEM_VAL & val, const TYPE_AS_Type & tp, const Sequence & index_l) const;
  bool IsMapType(const TYPE_AS_Type & tp) const;
  TYPE_AS_Type GetRngType(const TYPE_AS_Type & tp) const;

public:
  bool CheckGlobInv() /*notconst*/;
  Tuple GetFirstCIDOfFctOp(const TYPE_AS_Name & name);

#ifdef VDMSL
public:
  // used by functions in evaldl.cc: IsDLFnOP, ReadDLRecInfoMap, IsDLVal:
  TYPE_GLOBAL_SigmaIMO GetDLModule(const TYPE_AS_Name & mod_name);
private:
#endif // VDMSL
#ifdef VDMPP
public:
  Generic LookUpPermis(const TYPE_AS_Name& nm) const;
  void SetPermission(const TYPE_AS_Name & clnm, const TYPE_AS_Name & opnm, const TYPE_STKM_SubProgram & code);
  Generic LookUpThread(const TYPE_AS_Name & clnm) const;
#ifdef VICE
  void SetPerThreadDef (const TYPE_AS_Name & clnm, const TYPE_AS_PerObl & perobl);
  void SetSpoThreadDef (const TYPE_AS_Name & clnm, const TYPE_AS_SpoObl & perobl);
#endif // VICE
  void SetThreadField(const TYPE_AS_Name & clnm, const Generic & threadfield);
  void SetThreadDef(const TYPE_AS_Name & clnm, const Generic & code);
  Generic GetThreadDef(const TYPE_AS_Name & clnm) const;
  TYPE_SEM_OBJ GetSemObjInTab(const TYPE_SEM_OBJ_uRef & nm) const;
  //TYPE_SEM_OBJ_uRef GiveNextRefCountInTab();
  TYPE_SEM_OBJ_uRef GiveNextRefCountInTab(const TYPE_AS_Name & clnm);
  TYPE_SEM_OBJ_uRef global_obj_tab_insert(const TYPE_SEM_OBJ & semobj, const Generic & dlclassp);
  void SetObjectThreadId(const TYPE_SEM_OBJ_uRef & objref, const TYPE_SCHDTP_ThreadId & thrid);
  SET<TYPE_AS_Name> GetAllClassNms() const;
  bool IsClient(const TYPE_AS_Name & origcl, const TYPE_AS_Name & cloffct) const;
  bool IsSubClass(const TYPE_AS_Name & origcl, const TYPE_AS_Name & cloffct) const;
  bool IsBaseClass(const TYPE_AS_Name & clnm) const;
  bool AccessOk (const TYPE_AS_Access & access, const TYPE_GLOBAL_OrigCl & origcl, const TYPE_AS_Name & cloffct) const;
  Generic GetCPUOfObjRef(const TYPE_SEM_OBJ_uRef & objref) const;
  void SetCPUOfObjRef(const TYPE_AS_Name & cpunm, const TYPE_SEM_OBJ_uRef & objref);
  TYPE_SEM_NUM LookUpHistory(const TYPE_INSTRTP_HistoryKind & kind, const TYPE_AS_Name & opnm);
  TYPE_SEM_NUM CounterCalc(const TYPE_INSTRTP_HistoryKind & kind, const TYPE_SEM_History & history) const;
  int CalcOverload(const TYPE_INSTRTP_HistoryKind & kind, const TYPE_AS_Name & nm, const Map & histm) const;
  bool IsStatic(const TYPE_AS_Name & clnm, const TYPE_AS_Name & opnm) const;
  void RemoveStaticHist();
#ifdef VICE
  void UpdateHistCount(const TYPE_AS_Name & opnm,
                       const TYPE_INSTRTP_HistoryKind & kind,
                       const Generic & objref,
                       const Bool &,
                       const SEQ<TYPE_SEM_VAL> &);
#else
  void UpdateHistCount(const TYPE_AS_Name & opnm,
                       const TYPE_INSTRTP_HistoryKind & kind,
                       const Generic & objref);
#endif // VICE
private:
  TYPE_SEM_History AddHist(const TYPE_AS_Name & origopnm,
                           const Map & hist_m,
                           const TYPE_INSTRTP_HistoryKind & kind) const;
  TYPE_GLOBAL_OBJ_uDesc RemoveHist(const TYPE_GLOBAL_OBJ_uDesc &) const;
public:
  Tuple LookupHchy(const TYPE_AS_Name &nm);
  void SetHchy(const Map & clhchy);
  Sequence GetInhStrct(const TYPE_AS_Name & nm);
  void SetInhStrct (const Map & new_inhstrct);
  SET<TYPE_AS_Name> GetAllSupers(const TYPE_AS_Name & nm) const;
  SEQ<TYPE_AS_Name> AllSuperList(int, const SEQ<TYPE_AS_Name> &);
  MAP<TYPE_AS_Name,TYPE_GLOBAL_SigmaClass> GetClasses() const;
  Bool GetDefaultCons(const TYPE_AS_Name&) const;
  void AddAbstract(const TYPE_AS_Name & clsid);
  bool CheckIfAbstractClass(const TYPE_AS_Name & name) const;
  bool IsAClass(const TYPE_AS_Name & name) const;
  SET<TYPE_AS_Name> GetInhCon(const TYPE_AS_Name & nm) const;
  SEQ<TYPE_AS_InstAssignDef> GetInstVars(const TYPE_AS_Name & nm) const;
  MAP<TYPE_AS_Name,TYPE_AS_Type> GetInstVarsTp(const TYPE_AS_Name & nm) const;
  TYPE_STKM_SubProgram GetInstInv(const TYPE_AS_Name & nm) const;
  TYPE_SEM_InsStrct GetInstInitVal(const TYPE_AS_Name & nm) const;
  void SetInstInitVal(const TYPE_AS_Name & name, const TYPE_SEM_InsStrct & instval);
  SEQ<TYPE_AS_ValueDef> GetVlsDef(const TYPE_AS_Name & nm) const;
  TYPE_GLOBAL_ValueMap GetVlsInit(const TYPE_AS_Name & nm) const;
  Map GetAllOps(const TYPE_AS_Name & nm) const;
  SET<TYPE_AS_Name> GetAllOpsNmsSupers(const TYPE_AS_Name & name) const;
  Tuple LookupAllFnsOpsPolys(const TYPE_AS_Name & objnm, const TYPE_AS_Name & fnnm) const;
  MAP<TYPE_AS_Name,TYPE_AS_TypeDef> GetLocalTps(const TYPE_AS_Name & nm) const;
  Map GetRecSel(const TYPE_AS_Name & nm) const;
  Map GetLocalHchy(const TYPE_AS_Name & nm) const;
  bool IsClassInit(const TYPE_AS_Name & nm) const;
  Tuple ExpandClassName(const TYPE_AS_Name & name, const TYPE_GLOBAL_OrigCl & orgcl, const SET<TYPE_AS_Name> & nm_s);

private:
  SET<TYPE_AS_Name> InitClasses()const ;
  void SetClassInit(const TYPE_AS_Name & nm, const Bool & val);

public:
  SET<TYPE_AS_Name> GetSupers(const TYPE_AS_Name & nm) const;
  TYPE_STKM_DebugCmd LookUpConstructor(const TYPE_AS_Name &, const SEQ<TYPE_SEM_VAL> &);
private:
  Generic LookUpConstructorLocal(const TYPE_AS_Name &, const SEQ<TYPE_SEM_VAL> &);
public:
  bool AClass(const TYPE_AS_Name & nm) const;
  Map GetAllFns(const TYPE_AS_Name & nm) const;

  TYPE_SEM_OBJ_uRef CreateNEWFromDLObject(const DLObject & obj);
  DLObject GetDLObjectFromSemObjRef(const TYPE_SEM_OBJ_uRef & objref);
#endif // VDMPP

public:
  void SetSDeps(const Map & sdep);
  SET<TYPE_AS_Name> GetAllSDeps(const TYPE_AS_Name & nm) const;
#ifdef VDMPP
  void MergeClassAndStaticHierarchy();
  SET<TYPE_AS_Name> GetAllDeps(const TYPE_AS_Name & nm) const;
#endif // VDMPP

private:
#ifdef VDMSL
  Tuple IsDLFnOP(const TYPE_AS_Name & id, const TYPE_AS_Name & mod_nm);
  Tuple IsDLVal(const TYPE_AS_Name & id, const TYPE_AS_Name & mod_nm);
#endif // VDMSL

public:
  SEQ<TYPE_AS_Type> ConvertTypeVarTypeList(const SEQ<TYPE_AS_Type> & tp_l, const Map & tm) const;
  TYPE_AS_Type ConvertTypeVarType(const TYPE_AS_Type & tp, const Map & tm) const;
  TYPE_STKM_Pattern ConvertPattern(const TYPE_STKM_Pattern & p, const Map & tm) const;
public:
  void dlclose();

// state utility
//bool ExistsExplFnOp(const TYPE_AS_Name & mod_name,
//                    const TYPE_AS_Name & fnop_name,
//                    wostream & wos);
#ifdef VDMSL
  bool UserPushModule(const TYPE_AS_Name & mod_name, wostream & wos);
  void DumpSigmaModule( const Generic & modnm, wostream & wos );
#endif // VDMSL

#ifdef VDMPP
public:
  bool ReCreateDObjs(wostream & wos);
  void CreateUserDefinedObject(const TYPE_AS_AssignStmt & stmt, const SEQ<Char> & args, wostream & wos);

  TYPE_SEM_VAL UndefObjectRef( const TYPE_SEM_VAL & semval ); //
  void ClearObjectRef( const TYPE_SEM_OBJ_uRef & obj_ref ); //
  void ClearObjectRefInMap( const TYPE_GLOBAL_ValueMap & valmap ); //
  bool HasObjectRef( const TYPE_SEM_VAL& semval, const TYPE_SEM_OBJ_uRef & obj_ref );
  Map CreateRefCount( const TYPE_SEM_VAL& semval, const Map & ref_count );
  Map CreateObjectRefCount( const TYPE_SEM_OBJ_uRef & obj_ref, const Map & ref_count );
  Map CreateObjectsDirectRefCount();
  Map CreateObjectsIndirectRefCount();
  void CreateObjectRefSet( const TYPE_SEM_VAL& semval, Set& ref_s );
  Set CreateDestroySet( const Set& ref_s );
  Set CreateDanglingSet();
  void DestroyDanglingObjects();
  void DestroyObjectRef( const TYPE_SEM_OBJ_uRef& obj_ref );
  bool DestroyObject( const TYPE_AS_Name & objnm );
  void DestroyAllObjects();

public:
// Load & Save
  Tuple GetState();
  void SetState(const SEQ<TYPE_AS_Name> & objs_order,
                const Map & saved_objs,
                const Map & saved_obj_tab,
                const Generic & saved_lastres,
                const Map & saved_statics_m,
                const Map & saved_vls_init_m);
  Tuple GetObjState();
private:
  bool CheckContext( const TYPE_AS_Name & var );
  void CheckObjectTabContext(const Map & obj_tab);
  TYPE_SEM_VAL RestoreObjRef(const TYPE_SEM_VAL & semval, const Map & obj_ref_map);
  MAP<TYPE_AS_Name,type_dU2P> RestoreRecord(const MAP<TYPE_AS_Name,type_dU2P> & valmap, const Map & obj_ref_map);
  void RestoreSemanticRecord( const Map & obj_ref_map );
  Map RestoreObjectTable( const Map & obj_tab );
  void ClearStatics();
  void RestoreStatics( const Map & statics_m, const Map & obj_ref_map );
  void RestoreVlsInit( const Map & vls_init_m, const Map & obj_ref_map );
  Map CreateRefCountMap();

public:
  void DumpSigmaClass( const Generic & clsnm, wostream & wos );
  bool HasObjectRef( const Map& valmap, const TYPE_SEM_OBJ_uRef& obj_ref, Generic& name );
  void GetObjectReferenceByID( const Int & objid, Set& refs );
  void GetObjectReferenceByName( const TYPE_AS_Name & clsnm, Set& refs );
  void DisplayObjectReference( const TYPE_SEM_OBJ_uRef& obj_ref, wostream & wos );
  void DisplayLiveObjects(wostream & wos);
  void DisplayDanglingObjects(wostream & wos);
  void DisplayStaticObjects(wostream & wos);
  void DisplayUserObjects( wostream & wos );
  void DisplayObjectsDetail (const Sequence & args, wostream & wos);
  Map CreateRefMap(const Set & oid_s);
  void ShowAllObjects(wostream & wos);
  void ShowUserObjects(wostream & wos);
  int ObjectTabSize();

#endif // VDMPP 

public:
  void DLInsertClassLib (const TYPE_AS_Name& clnm, const TYPE_AS_TextLit& useslib, const TYPE_AS_Name& name);
  Generic DLCreateNEW(const TYPE_AS_Name & name, const Generic & dlobject);
#ifdef VDMSL
  Generic DLCallOp(const TYPE_AS_Name& module, const TYPE_AS_Name& op, bool isvoid, const Generic & args);
#endif // VDMSL
#ifdef VDMPP
  Tuple DLCallOp(const TYPE_AS_Name& clname,
                 const TYPE_AS_Name& op,
                 const SEQ<TYPE_SEM_VAL> & args,
                 const TYPE_SEM_OBJ_uRef & curobj);
#endif //VDMPP

public:
  static Sequence Sem2M4(const SEQ<TYPE_SEM_VAL> & arg_l, DLRecInfoData * dldata);
  static TYPE_SEM_VAL M42Sem(const Generic & val, DLRecInfoData * dldata);
  static Generic Convert2M4(const TYPE_SEM_VAL & arg, DLRecInfoData * dldata);
#ifdef VDMPP
public:
  void GC(bool forced, bool more);
#endif //VDMPP

#ifdef VDMPP
public:
  static TYPE_INSTRTP_req historyKind_req;
  static TYPE_INSTRTP_act historyKind_act;
  static TYPE_INSTRTP_fin historyKind_fin;
  static TYPE_INSTRTP_waiting historyKind_waiting;
  static TYPE_INSTRTP_active historyKind_active;
#endif // VDMPP
};

#endif // __evalstate_h__
