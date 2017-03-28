/***
*  * WHAT
*  *    Implementation of mod_aux.vdm 1.99
*  * FILE
*  *    $Source: /home/vdmtools/cvsroot/toolbox/code/cg/mod_cgaux.h,v $
*  * VERSION
*  *    $Revision: 1.42 $
*  * DATE
*  *    $Date: 2006/04/14 08:14:40 $
*  * STATUS
*  *    Under development
*  * REFERENCES
*  *    
*  * PROJECT
*  *    AFRODITE
*  * AUTHOR
*  *    Henrik Voss + $Author: vdmtools $
*  *
*  * COPYRIGHT
*  *    (C) Kyushu University
***/

#ifndef __mod_aux_h__
#define __mod_aux_h__

private:
// Module state
  TYPE_AS_Name cc;
  TYPE_AS_Name cm;
  int sc;
  bool auxfct;
  bool isabstract;
  SET<TYPE_AS_Name> c_s;
  TYPE_REP_TypeRep r_tp;
  Sequence nametab;        // Env (seq of BlkEnv)
  Map globalenv;           // BlkEnv (map AS`Name to (AS`Name*nat))
//  Map lastenv;             // BlkEnv (map AS`Name to (AS`Name*nat))
  bool val;
  bool throwException;
  bool throwNotSupported;
  SET<TYPE_AS_Name> var_in_expr;
  SET<TYPE_AS_Id> quotes_s;
  SET< SEQ<Char> > import_s;
  Map abstractMethods;     // map AS`Name to set of AS`Ids
  Map nonAbstractMethods;  // map AS`Name to set of AS`Ids
  Map allNonAbstractMethods; // map AS`Name to set of AS`Ids
  Map superClasses;        // map AS`Name to set of AS`Name
  Map orderedSupers;       // map AS`Name to seq of AS`Name
  Map instanceVariables;   // map AS`Name to (map AS`Name to REP`TypeRep)
  SEQ<TYPE_AS_Name> classVisitOrder;
  Map methodNames;         // map AS`Name to set of (AS`Name * AS`Access * AS`Type)
  Map unhiddenNames;       // map AS`Name to set of (AS`Name * AS`Access * AS`Type)
  Map packageNames;
  bool isConstr;
  bool isAtomic;
  bool hasInv;
  int alwaysLevel;
  bool classHasDefaultConstructor;
  bool usesSuper;
  bool isStatic;
  bool usesThis;
  Sequence cause;
  Map primitiveType; // map ((seq of char) | ((seq of char) * (seq of char))) to CPP`Identifier
  Set noCheckSeqApply;

private:
#ifdef VDMPP
  TYPE_CPP_Modifier JavaAccess (const Int & acc);
#endif // VDMPP
  Generic FindType(const Record&);
  MAP<TYPE_AS_Name, TYPE_REP_TypeRep> FindPatIdMap(const TYPE_AS_Expr& expr);
  bool IsSeqOfCPPStmt(const Generic& );

  TYPE_REP_TypeRep CleanFlatType(const TYPE_REP_TypeRep & tp);
  TYPE_REP_TypeRep ExpandTypeRep(const TYPE_REP_TypeRep & , const SET<TYPE_REP_TypeNameRep> &);
  TYPE_REP_TypeRep CleanAndFlattenType (const TYPE_REP_TypeRep &);
  SET<TYPE_REP_TypeRep> CleanAndFlattenTypeAux (const TYPE_REP_TypeRep &);
  static SET<TYPE_REP_TypeRep> OptimizeType(const SET<TYPE_REP_TypeRep> &);

  TYPE_REP_TypeRep CombineSetTypeRep(const SET<TYPE_REP_TypeRep>&);
  TYPE_REP_TypeRep CombineSeqTypeRep(const SET<TYPE_REP_TypeRep>&);
  TYPE_REP_TypeRep CombineInjectiveMapTypeRep(const SET<TYPE_REP_TypeRep>&);
  TYPE_REP_TypeRep CombineGeneralMapTypeRep(const SET<TYPE_REP_TypeRep>&);
  TYPE_REP_TypeRep FindPossibleMapType(const TYPE_REP_TypeRep&);
  TYPE_REP_SetTypeRep FindPossibleSetType(const TYPE_REP_UnionTypeRep&);
  TYPE_REP_TypeRep FindPosResSetTypeInSetDistrUnion(const TYPE_REP_TypeRep&);
  TYPE_REP_TypeRep FindPosResSeqTypeInSeqDistr(const TYPE_REP_TypeRep&);
  static SET<TYPE_REP_SetTypeRep> CombineSetTypes(const SET<TYPE_REP_SetTypeRep>&);
  static SET<TYPE_REP_SeqTypeRep> CombineSeqTypes(const SET<TYPE_REP_SeqTypeRep>&);
  static SET<TYPE_REP_GeneralMapTypeRep> CombineGenMapTypes(const SET<TYPE_REP_GeneralMapTypeRep>&);
  static SET<TYPE_REP_InjectiveMapTypeRep> CombineInjMapTypes(const SET<TYPE_REP_InjectiveMapTypeRep>&);
  TYPE_REP_TypeRep FromAS2RepType(const Generic&);
  TYPE_REP_TypeRep FromAS2RepTypeAux(const Generic&);
  static TYPE_CPP_Stmt RunTime(const wstring &);
  static TYPE_CPP_Expr RunTimeExpr(const TYPE_AS_Id &);
  TYPE_CPP_Stmt NotSupported(const wstring &, const Record&);
  void CannotCG(const wstring &, const TYPE_AS_Name &);

public: 
  TYPE_AS_Id GiveLastName(const Record &);
  static TYPE_AS_Name GiveFirstName(const TYPE_AS_Name &);
  static TYPE_AS_Name GiveSecondName(const TYPE_AS_Name &);
  TYPE_AS_Id CleanIdentifier(const TYPE_AS_Id&);
  static TYPE_AS_Id StringNumber(const Int &);
  static TYPE_AS_Id GenFileExt(const TYPE_AS_Id &);
  Int FindScope(const Record &);
  TYPE_AS_Name GiveCurCASName();
  SEQ<Char> GiveOrgnFileName();

#ifdef VDMPP
  Generic ConvAsAccess2CPP (const TYPE_AS_Access &);
  void IncludeClass(const TYPE_AS_Name&);
  Bool IsInstVar(const TYPE_AS_Name&);
#endif //VDMPP

private:
  bool AreOfSameType(const TYPE_REP_TypeRep&, const TYPE_REP_TypeRep&);
  bool IsMapType(const TYPE_REP_TypeRep&) const;
  bool IsPosMapType(const TYPE_REP_TypeRep&) const;
  bool IsSeqType(const TYPE_REP_TypeRep&) const;
  bool IsPosSeqType(const TYPE_REP_TypeRep&) const;
  bool IsStringType(const TYPE_REP_TypeRep&) const;
  bool IsPossibleStringType(const TYPE_REP_TypeRep&) const;
  bool IsSetType(const TYPE_REP_TypeRep&) const;
  bool IsPosSetType(const TYPE_REP_TypeRep&) const;
  bool IsNumType(const TYPE_REP_TypeRep&) const;
  bool IsIntType(const TYPE_REP_TypeRep&) const;
  bool IsRealType(const TYPE_REP_TypeRep&) const;
  bool IsPosNumType(const TYPE_REP_TypeRep&) const;
  bool IsPosIntType(const TYPE_REP_TypeRep&) const;
  bool IsBoolType(const TYPE_REP_TypeRep&) const;
  bool IsCharType(const TYPE_REP_TypeRep&) const;
  bool IsPosCharType(const TYPE_REP_TypeRep&) const;
  bool IsCompositeType(const TYPE_REP_TypeRep&);
  bool IsPosCompositeType (const TYPE_REP_TypeRep&);
  bool IsProductType(const TYPE_REP_TypeRep&);
  bool IsPosProductType(const TYPE_REP_TypeRep&);
  bool IsQuoteType(const TYPE_REP_TypeRep &);
  bool PossibleFnType(const TYPE_REP_TypeRep &) const;
  bool IsFctType(const TYPE_REP_TypeRep &) const;
  bool IsUnionFunctionType (const TYPE_REP_TypeRep &) const;
  bool IsSubType (const TYPE_REP_TypeRep &, const TYPE_REP_TypeRep &) const;
#ifdef VDMPP
  TYPE_REP_ObjRefTypeRep UnQClassType (const TYPE_REP_ObjRefTypeRep &);
  bool IsObjRefType(const TYPE_REP_TypeRep &);
  Record ExchangeDefAccess (const Record &);
#endif // VDMPP
  TYPE_AS_ExplFnDef MakePreFct(const Record&, const Generic &);
  TYPE_AS_ExplFnDef MakePostFct(const Record&, const Generic &);
  Tuple MakeReturnPattern(const SEQ<TYPE_AS_NameType> & nt_l);
  SEQ<TYPE_AS_Type> StateDefToType(const Generic &);
  SEQ<TYPE_AS_Pattern> StateDefToPattern(const Generic &, int);
  TYPE_AS_PatternName Field2Pattern(const TYPE_AS_Field &, int);
  Tuple SplitParameterTypes(const SEQ<TYPE_AS_PatTypePair> &);
  Generic QuotePMExprWithClNm(const Generic &, const TYPE_AS_Name &, const SET<TYPE_AS_Name> &);
  Generic QuotePMExprWithTemp(const Generic &, const TYPE_AS_Name &, const SET<TYPE_AS_Name> &);
  SET<TYPE_AS_Name> FindAllNamesInExpr(const TYPE_AS_Expr &);
  SET<TYPE_AS_Name> FindAllNamesInPatternBind(const TYPE_AS_PatternBind &);
  void InitState_CGAUX(const Generic &);
  void InitEnv_CGAUX();
  Map CurrentEnv_CGAUX();
  void PushEnv_CGAUX();
  void PopEnv_CGAUX();
  bool PushThisEnv_CGAUX(const Map &);
  bool PopLastEnv_CGAUX();
  bool InsertNamesinEnv_CGAUX(const Set &);
  void InsertNamesinEnv2_CGAUX(const Set &);
  TYPE_AS_Name InsertGlobalName(const TYPE_AS_Name &);
  TYPE_AS_Name InsertName_CGAUX(const Record &);
  Tuple FindName(const Record &);
  Tuple GiveCurMName();
  TYPE_AS_Name GiveCurMASName();
  void CurrentMethod(const TYPE_AS_Name &);

#ifdef VDMPP
  SEQ<TYPE_CPP_Preprocessor> GenClassIncls();
  void StoreClasses(const SET<TYPE_AS_Name> &);
  void UseAuxFct();

  bool IsAbstract();
  bool ThrowsException() const;
  bool ThrowsNotSupportedException() const;
  SET<TYPE_AS_Id> UsedQuotes() const;
  bool IsSuperClass(const TYPE_AS_Name&, const TYPE_AS_Name&) const;
  SET<TYPE_AS_Name> VariablesinValDef() const;
  void DefineClassAsAbstract();
  void SetException(bool);
  void SetNotSupportedException(bool);
  void InitVariables();
  void InitQuotes();
  void InsertVariable(const TYPE_AS_Name &);
  void InsertQuote(const TYPE_AS_Id &);
  void InsertImport(const SEQ<Char> & ch_l);
  void InsertJDKImport(const wstring & ch_l);
  bool GetValState() const;
  void SetValState();
  void UnsetValState();
#endif // VDMPP
  TYPE_AS_Id GiveCurCName();
  void CurrentRType(const TYPE_REP_TypeRep &);
  TYPE_REP_TypeRep GiveCurrentRType() const;
  void UpdateSortAuxFct(const Record &);
  void UpdatePermAuxFct(const Set &);
  SET<TYPE_REP_TypeRep> GiveFlatUnionType(const SET<TYPE_REP_TypeRep> &);
  Generic FindSeqElemType(const TYPE_REP_TypeRep &);
  Generic FindSetElemType(const TYPE_REP_TypeRep &);
  Generic FindMapDomType(const TYPE_REP_TypeRep &);
  Generic FindMapRngType(const TYPE_REP_TypeRep &);
  Generic FindProductElemType(const TYPE_REP_TypeRep &, size_t len);
  TYPE_REP_TypeRep posFieldApplyTypes(const TYPE_REP_TypeRep &, const TYPE_AS_Name &);
  TYPE_REP_TypeRep posRngMapOrIndsSeqType(const TYPE_REP_TypeRep &);
  TYPE_REP_TypeRep LOT (const Generic & tp);
  SET<TYPE_REP_TypeRep> LOTset (const SET<TYPE_REP_TypeRep> & tps);

  void SetUsesSuper();
  void ResetUsesSuper();
  bool UsesSuper() const;

  void SetUsesThis();
  void ResetUsesThis();
  bool UsesThis() const;

  void SetIsStatic();
  void ResetIsStatic();
  bool IsStatic() const;

#ifdef VDMPP
  SET<TYPE_CPP_File> GenJavaQuoteFiles();
  SEQ<TYPE_CPP_SingleTypeImportDeclaration> GenTypeImportDeclarations();
  static TYPE_CPP_MemberDeclaration GenHashMember();
  TYPE_CPP_FunctionDefinition GenQuoteConstructor(const TYPE_AS_Id & ch_l);
  static TYPE_CPP_FunctionDefinition GenQuoteHashcodeMethod(const TYPE_AS_Id & ch_l);
  static TYPE_CPP_FunctionDefinition GenQuoteEqualsMethod(const TYPE_AS_Id &);
  static TYPE_CPP_FunctionDefinition GenQuoteToStringMethod(const TYPE_AS_Id &);
#endif // VDMPP

// Local auxiliary functions (vdmcg members)

  TYPE_REP_TypeRep LOThelp (const TYPE_REP_TypeRep & tp, const SET<TYPE_REP_TypeRep> & dones);

  void SetAtomic();
  void UnsetAtomic();
  bool GetAtomic() const;
  void UpAlwaysLevel();
  void DownAlwaysLevel();
  bool IsInAlways() const;
  void PushCause(const Generic & cause);
  void PopCause();
  Generic GetCause() const;

#ifdef VDMPP
  void AddSuperClasses(const SEQ<TYPE_AS_Name> &);
  void AddAbstractMethods(const TYPE_AS_Class &);
  bool IsAbstractFunction(const TYPE_AS_FnDef &);
  bool IsAbstractOperation(const TYPE_AS_OpDef &);
  bool IsNotYetSpecifiedOperation(const TYPE_AS_OpDef &);
  void ProcessAbstractMethods();
  void GenerateOrderedSupers();
  void ProcessMethodNames();
  Set GetUnhiddenNames(const TYPE_AS_Name &);
  bool IsInClassNamespace(const TYPE_AS_Name &, const TYPE_AS_Name &) const;
  bool HasAbstractMethods();
  bool IsAbstractClass(const TYPE_AS_Name &);
  void SetConstr();
  void UnsetConstr();
  bool GetConstr() const;
  void SetHasInv();
  void UnsetHasInv();
  bool GetHasInv() const;
  void SetHasDefaultConstr();
  bool GetHasDefaultConstr() const;
  bool IsClass(const TYPE_AS_Name &);
  SET<TYPE_AS_Name> GetAllClasses();
  void AddInstanceVars(const TYPE_AS_Name &, const SEQ<TYPE_AS_InstanceVarDef> &);
  Generic GetInstanceVarTp(const TYPE_AS_Name&, const TYPE_AS_Name &);
  SEQ<TYPE_AS_Name> GetOrderedSupers(const TYPE_AS_Name &);
  Tuple ExpandClassName(const TYPE_AS_Name & nm, const TYPE_AS_Name & clnm, const SET<TYPE_AS_Name> & nm_s);
  void AddPackageName(const TYPE_AS_Name &, const SEQ<TYPE_AS_ValueDef> & vals);
  void InitJavaPrimitiveType();
  void AddJavaPrimitiveBoolType(const SEQ<Char> & cnm, const SEQ<Char> & mnm);
  void AddJavaPrimitiveIntType(const SEQ<Char> & cnm, const SEQ<Char> & mnm);
  void AddJavaPrimitiveRealType(const SEQ<Char> & cnm, const SEQ<Char> & mnm);
  void AddJavaPrimitiveType(const SEQ<Char> & cnm, const SEQ<Char> & mnm, const TYPE_CPP_Identifier & id);
  TYPE_CPP_Expr CheckJavaPrimitiveType(const TYPE_CPP_Expr & e, const SEQ<Char> & cnm, const SEQ<Char> & mnm);
#endif //VDMPP

  void AddNoCheckSeqApply(const Set & s);
  void RemNoCheckSeqApply(const Set & s);
  bool CheckSeqApply(const Tuple & e);
  bool IsSeqIndicesSet(const TYPE_AS_Expr & e);
  bool IsBoolExpr(const TYPE_CPP_Expr & e);
  bool IsIntExpr(const TYPE_CPP_Expr & e);
  bool IsRealExpr(const TYPE_CPP_Expr & e);
  bool IsStringExpr(const TYPE_CPP_Expr & e);

#ifdef VDMPP
  static TYPE_CPP_PackageName GenPackageName(const TYPE_AS_Ids &);
  static Generic PackageToDir(const Generic &);
#endif //VDMPP

  TYPE_REP_TypeRep RemoveNil(const TYPE_REP_TypeRep & type);
  TYPE_REP_TypeRep RemoveExitType(const TYPE_REP_TypeRep & type);
  static bool NoReturnValue(const TYPE_REP_TypeRep & tp);

  static TYPE_CPP_Expr StripBracketedExpr (const TYPE_CPP_Expr & expr);
  static TYPE_CPP_Expr StripCastExpr (const TYPE_CPP_Expr & expr);
  static TYPE_CPP_Expr StripBracketedAndCastExpr (const TYPE_CPP_Expr & expr);

  SEQ<TYPE_AS_Type> GetOpParms(const TYPE_AS_OpDef &);
  SEQ<TYPE_AS_Type> GetFnParms(const TYPE_AS_FnDef &);
  TYPE_AS_Type GetOpRestype(const TYPE_AS_OpDef &);
  TYPE_AS_Type GetFnRestype(const TYPE_AS_FnDef &);
  static SEQ<TYPE_AS_Type> ConstructImplDomType(const SEQ<TYPE_AS_PatTypePair>&);
  static TYPE_AS_Type ConstructImplRngType(const SEQ<TYPE_AS_NameType> &);
  Generic GetFnMeasu(const TYPE_AS_FnDef &);
  static TYPE_AS_Name QualiName(const TYPE_AS_Name &, const TYPE_AS_Name &);
  static TYPE_AS_Name UnqualiName(const TYPE_AS_Name &);
  static TYPE_AS_Id NameToId(const TYPE_AS_Name &);

  static TYPE_AS_Type AddClMod(const TYPE_AS_Type &, const TYPE_AS_Name &, const SET<TYPE_AS_Name> &);
#endif // __mod_aux_h__
