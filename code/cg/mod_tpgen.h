/***
*  * WHAT
*  *    Implementation of mod_tpgen.vdm r1.26
*  * FILE
*  *    $Source: /home/vdmtools/cvsroot/toolbox/code/cg/mod_tpgen.h,v $
*  * VERSION
*  *    $Revision: 1.14 $
*  * DATE
*  *    $Date: 2002/09/18 12:51:28 $
*  * STATUS
*  *    Under development $State: Exp $
*  * PROJECT
*  *    AFRODITE
*  * AUTHOR
*  *    Hanne Carlsen (HC) + $Author: mikhail $
*  * COPYRIHT
*  *    (C) Kyushu University
***/

#ifndef __mod_tpgen_h__
#define __mod_tpgen_h__

#define Anonym_Suffix     ASTAUX::MkId(L"_anonym")
#define Userdef_Suffix    ASTAUX::MkId(L"_userdef")
#define Userimpl_Suffix   ASTAUX::MkId(L"_userimpl")
#define Userimpl_Flag     ASTAUX::MkId(L"_USERIMPL")
#define DlClass_Name      ASTAUX::MkId(L"DlClass")
#define DlClassPtr_Suffix ASTAUX::MkId(L"_dlClassPtr")

private:
  Map cppenv;            // CppTpEnv (map Identifier to [CPP`CPPAS])
  Map known_types;       // TpEnv (map AS`Name to set of AS`Name)
  SET<TYPE_REP_TypeNameRep> known_union_types;
  Map union_types;       // map AS`Name to seq of AS`Name
  Map record_types;      // map AS`Name to seq of AS`Name
#ifdef VDMSL
  SET<TYPE_AS_Name> imported_types;
#endif //VDMSL
#ifdef VDMPP
  Map inh_tree;          // InhTree (map AS`Name to set of AS`Name)
  SET<TYPE_AS_Name> obj_refs;
  SET<TYPE_AS_Name> classTypes;
#endif //VDMPP
  SEQ<TYPE_CPP_Identifier> anonym_decl;
  SEQ<TYPE_CPP_Identifier> anonym_comp_decl;
  SEQ<TYPE_CPP_Identifier> mod_decl;
  TYPE_CPP_CPPAS anonym_tpdefs;
  TYPE_CPP_CPPAS anonym_tpdecl;
  TYPE_CPP_CPPAS anonym_fctdef;
  TYPE_CPP_CPPAS mod_fctdef;
  int tag;
  SEQ<TYPE_CPP_Identifier> quotes_l;
//  TYPE_CPP_CPPAS ifdefs; // not used
  Generic modnm;
  SET<TYPE_AS_Name> inclgh;
  SET<TYPE_AS_Name> inclh;
  Map typeDefs;

private:
  Set get_known_union_types ();
  Map get_known_types ();
#ifdef VDMSL
  Set get_imported_types();
#endif //VDMSL
  void InitState_TPGEN(const Generic &);

  TYPE_CPP_IntegerLit new_tag();
  void AddInclGH(const TYPE_AS_Name&);
  void AddInclH(const TYPE_AS_Name&);
  void DeclModCppTp(const TYPE_AS_Id &, const TYPE_CPP_CPPAS &);
  void DeclAnonymCppTp(const TYPE_AS_Id & id, const TYPE_CPP_CPPAS &);
  void AddNewCppTp(const TYPE_AS_Id &, const Sequence &);
#ifdef VDMPP
  void AddClassTypes(const TYPE_AS_Class &);
#endif //VDMPP
#ifdef VDMSL
  void AddModuleTypes(const TYPE_AS_Module&);
#endif //VDMSL
  Map GetImportetTypes(const TYPE_AS_Name&,
                       const TYPE_AS_Name &,
                       const Map &,
                       const MAP<TYPE_AS_Name,TYPE_AS_Name> &);
  TYPE_AS_Type GenQType(const TYPE_AS_Name &, const TYPE_AS_Type &);
#ifdef VDMPP
  bool isclassType(const TYPE_AS_Name&);
  Generic LOTrec(const TYPE_AS_Name&, const SET<TYPE_AS_Name> &, const SET<TYPE_AS_Name> &);
  Generic GenQualifiedTypeName(const TYPE_AS_Name &);
  Generic GenRootType(const TYPE_AS_Name &);
#endif //VDMPP
  SEQ<TYPE_AS_Type> GenQTypeSeq(const TYPE_AS_Name&, const SEQ<TYPE_AS_Type> &);
//Sequence GenQFieldSeq(const TYPE_AS_Name&, const Sequence &);
  SEQ<TYPE_AS_Field> GenQFieldSeq(const TYPE_AS_Name&, const SEQ<TYPE_AS_Field> &);
  bool IsTpRecursive(const TYPE_AS_Name &, const TYPE_REP_TypeRep &, const SET<TYPE_AS_Name> &);
  TYPE_AS_Id ConcatIds(const TYPE_AS_Ids &);
  TYPE_AS_Id Name2Id(const TYPE_AS_Name &);
  Map ModuleTypes2TpEnv(const TYPE_AS_Name &, const MAP<TYPE_AS_Name,TYPE_AS_TypeDef> &);
  TYPE_CPP_Identifier Id2CppGTpId(const TYPE_AS_Id & str);
  TYPE_CPP_Identifier Id2JavaGTpId(const TYPE_AS_Id & str);
  TYPE_AS_Id PrefixAnonymType(const TYPE_AS_Id & id);
  TYPE_AS_Id PrefixModuleType(const TYPE_AS_Id & id);
  TYPE_AS_Id RemPrefixNum(const TYPE_AS_Id & i);
  TYPE_AS_Id ReplaceUnderscore(const TYPE_AS_Id &);
  TYPE_CPP_Identifier Name2CppTpId(const TYPE_AS_Name&);
  TYPE_CPP_Identifier GenQuoteId(const TYPE_AS_Id & q);
  TYPE_AS_Id GetQuoteId(const TYPE_CPP_Identifier&);
  TYPE_CPP_Identifier GenTypeTagCppId(const TYPE_CPP_Identifier &);
  TYPE_CPP_Identifier GenAnonymTypeTag();
  TYPE_CPP_Identifier GenModuleTypeTag();
  TYPE_AS_Name GenQName(const TYPE_AS_Name&);
  TYPE_AS_Name GenQName2(const TYPE_AS_Name&, const TYPE_AS_Name &);
  TYPE_AS_Name GenQReName2(const TYPE_AS_Name&, const TYPE_AS_Name&);
  Generic GenQComp(const TYPE_AS_Name&, const Generic&);
  SEQ<TYPE_AS_Type> GenQCompList(const TYPE_AS_Name &, const SEQ<TYPE_AS_Type> &type_l);
  Generic GenRQComp(const TYPE_AS_Name &, const Generic &);
  SET<TYPE_REP_TypeRep> GenRQCompSet(const TYPE_AS_Name &, const SET<TYPE_REP_TypeRep> &type_s);
  SEQ<TYPE_REP_TypeRep> GenRQCompList(const TYPE_AS_Name&, const SEQ<TYPE_REP_TypeRep> & type_l);
#ifdef VDMSL
  TYPE_CPP_CPPAS GenIfNotDef(const TYPE_CPP_Identifier&, const TYPE_CPP_Expr&);
  TYPE_CPP_CPPAS GenTypeDeclIfDef(const TYPE_CPP_Identifier&, const TYPE_CPP_CPPAS &);
  TYPE_CPP_CPPAS GenFctOpDefIfDef(const TYPE_AS_Name&, const TYPE_CPP_CPPAS &);
#endif // VDMSL
#ifdef VDMPP
  Sequence GenIfNotDef(const TYPE_CPP_Identifier&, const TYPE_CPP_Expr&);
  Sequence GenTypeDeclIfDef(const TYPE_CPP_Identifier&, const Sequence &);
  Sequence GenFctOpDefIfDef(const TYPE_AS_Name&, const Sequence &);
#endif // VDMPP
  TYPE_CPP_Identifier PrefixANONYMDecl(const TYPE_CPP_Identifier &);
  TYPE_CPP_Identifier PrefixANONYMDef(const TYPE_CPP_Identifier &);
#ifdef VDMSL
  TYPE_CPP_CPPAS GenTypeDefIfDef(const TYPE_CPP_Identifier &, const TYPE_CPP_CPPAS &);
#endif // VDMSL
#ifdef VDMPP
  SEQ<TYPE_CPP_MemberDeclaration> GenTypeDefIfDef(const TYPE_CPP_Identifier &, const SEQ<TYPE_CPP_MemberDeclaration> &);
#endif // VDMPP
  TYPE_CPP_CPPAS GenForwardDecl(const TYPE_CPP_Identifier &);
  SEQ<TYPE_CPP_MemberDeclaration> GenMemFctMacros(const Int &);
  TYPE_CPP_FunctionDefinition GenGetFunctionDef(const Int&,
                                                const TYPE_CPP_Identifier&,
                                                const TYPE_CPP_Identifier&,
                                                int,
                                                const TYPE_CPP_QualifiedClassName &,
                                                const TYPE_REP_TypeRep &);
  TYPE_CPP_MemberSpecifier GenGetFunctionDecl(const TYPE_CPP_Identifier&, const TYPE_CPP_Identifier&);
  TYPE_CPP_MemberSpecifier GenSetFunctionDecl(const TYPE_CPP_Identifier &, const TYPE_CPP_Identifier &);
  TYPE_CPP_FunctionDefinition GenSetFunctionDef(const TYPE_CPP_Identifier&,
                                                const TYPE_CPP_Identifier&,
                                                int,
                                                const TYPE_CPP_QualifiedClassName&);
  TYPE_CPP_FunctionDefinition GenGetTypeNameDecl(const TYPE_CPP_Identifier &);
  TYPE_CPP_FunctionDefinition GenCommonConstrDecl(const TYPE_CPP_Identifier &, const Record&);
  TYPE_CPP_FunctionDefinition GenVDMBasePConstrDecl(const TYPE_CPP_Identifier &, const Record &);
  TYPE_CPP_FunctionDefinition GenTemplateConstrDecl(const TYPE_CPP_Identifier&, const TYPE_CPP_TemplateClassName &);
  TYPE_CPP_FunctionDefinition GenDefaultConstrDecl(const TYPE_CPP_Identifier &, const Record & supid);
  TYPE_CPP_MemberDeclaration GenClassDefaultConstructorDecl(const Int &,
                                                            const TYPE_CPP_Identifier cnm,
                                                            const SEQ<TYPE_CPP_Identifier> &);
  SEQ<TYPE_CPP_MemberDeclaration> GenIdentDecls(const SEQ<TYPE_CPP_Identifier> &,
                                                const MAP<TYPE_CPP_Identifier,TYPE_CPP_Name>&);
  TYPE_CPP_FunctionDefinition GenClassConstrDecl(const TYPE_CPP_Identifier &,
                                                 const SEQ<TYPE_CPP_Identifier> &,
                                                 const MAP<TYPE_CPP_Identifier,TYPE_CPP_Name> &);
  SET<TYPE_CPP_Expr> GetBasicTypes();
  TYPE_CPP_FunctionDefinition GenCloneMethod(const TYPE_CPP_Identifier &,
                                             const SEQ<TYPE_CPP_Identifier> &,
                                             const MAP<TYPE_CPP_Identifier,TYPE_CPP_Name> &);
  TYPE_CPP_FunctionDefinition GenAsciiMethod(const TYPE_CPP_Identifier &,
                                             const SEQ<TYPE_CPP_Identifier> &,
                                             const MAP<TYPE_CPP_Identifier,TYPE_CPP_Name> &);
  TYPE_CPP_FunctionDefinition GenEqualsMethod(const TYPE_CPP_Identifier &,
                                              const SEQ<TYPE_CPP_Identifier> &,
                                              const MAP<TYPE_CPP_Identifier,TYPE_CPP_Name> &,
                                              const MAP<TYPE_CPP_Identifier,Bool> &);
  TYPE_CPP_FunctionDefinition GenHashMethod(const TYPE_CPP_Identifier &,
                                            const SEQ<TYPE_CPP_Identifier> &,
                                            const MAP<TYPE_CPP_Identifier,TYPE_CPP_Name> &,
                                            const MAP<TYPE_CPP_Identifier,Bool> &);
//TYPE_CPP_FunctionDefinition GenLengthMethod(const TYPE_CPP_Identifier&,
//                                            const Sequence &,
//                                            const Map &);
  TYPE_CPP_MemberSpecifier GenInitFunctionDecl(const TYPE_CPP_Identifier&,
                                               const SEQ<TYPE_CPP_Identifier> &,
                                               const MAP<TYPE_CPP_Identifier,TYPE_CPP_Name> &);
  TYPE_CPP_FunctionDefinition GenInitFunctionDef(const TYPE_CPP_QualifiedClassName&,
                                                 const SEQ<TYPE_CPP_Identifier> &,
                                                 const MAP<TYPE_CPP_Identifier,TYPE_CPP_Name> &);
  TYPE_CPP_Identifier GenCompositeTypeTagId(const TYPE_AS_Name &);
  SEQ<TYPE_CPP_IdentDeclaration> GenCompositeClassDecl(const TYPE_AS_Name &,
                                                       const Generic &,
                                                       const TYPE_CPP_Identifier &,
                                                       const SEQ<TYPE_CPP_Identifier> &,
                                                       const MAP<TYPE_CPP_Identifier,TYPE_CPP_Name> &,
                                                       const MAP<TYPE_CPP_Identifier,Bool> &,
                                                       const SEQ<TYPE_REP_TypeRep> &);
  SEQ<TYPE_CPP_IdentDeclaration> GenProductClassDecl(const TYPE_CPP_Identifier &,
                                                     const SEQ<TYPE_CPP_Identifier> &,
                                                     const MAP<TYPE_CPP_Identifier,TYPE_CPP_Name> &,
                                                     const SEQ<TYPE_REP_TypeRep> &);
  SEQ<TYPE_CPP_IdentDeclaration> GenProductDecl(const TYPE_CPP_Identifier&, const TYPE_REP_ProductTypeRep &);
  TYPE_AS_Id GenProductType(const TYPE_REP_ProductTypeRep &, const SET<TYPE_AS_Name> &);
  SEQ<TYPE_CPP_IdentDeclaration> GenCompositeDecl(const TYPE_AS_Name&,
                                                  const Generic &,
                                                  const TYPE_CPP_Identifier &,
                                                  const TYPE_REP_CompositeTypeRep &);
  TYPE_AS_Id GetUniqueName(const TYPE_AS_Id & stem, const SET<TYPE_AS_Id> & existingNames);
  TYPE_AS_Id GenCompositeType(const TYPE_REP_CompositeTypeRep &, const Generic &, const SET<TYPE_AS_Name> &);
#ifdef VDMPP
  TYPE_AS_Id GenObjRefType(const TYPE_REP_ObjRefTypeRep &, const SET<TYPE_AS_Name> &);
#endif //VDMPP
  TYPE_CPP_CPPAS GenTemplateClass(const Int &, const TYPE_CPP_Identifier &, const TYPE_CPP_TemplateClassName &);
  TYPE_CPP_CPPAS GenMapDecl(const TYPE_CPP_Identifier &, const TYPE_CPP_Identifier &, const TYPE_CPP_Identifier &);
  TYPE_AS_Id GenMapType(const TYPE_REP_TypeRep &, const TYPE_REP_TypeRep &, const SET<TYPE_AS_Name> &);
  TYPE_CPP_CPPAS GenSetDecl(const TYPE_CPP_Identifier &, const TYPE_CPP_Identifier &);
  TYPE_AS_Id GenSetType(const TYPE_REP_SetTypeRep &, const SET<TYPE_AS_Name> &);
  TYPE_CPP_CPPAS GenSeqDecl(const TYPE_CPP_Identifier &, const TYPE_CPP_Identifier &);
  TYPE_AS_Id GenSeqType(const TYPE_REP_SeqTypeRep &, const SET<TYPE_AS_Name> &);
  bool StringAsBefore(wstring &, const wstring & s);
  TYPE_AS_Id GenUnionType(const TYPE_REP_UnionTypeRep &, const TYPE_REP_UnionTypeRep &, const SET<TYPE_AS_Name> &);
  TYPE_REP_TypeRep RemoveInvType(const TYPE_REP_TypeRep & p);
  void AddUnionType(const TYPE_AS_Name &, const TYPE_AS_Name &);
  void AddRecordType(const TYPE_AS_Name &, const TYPE_AS_Name &);
  SEQ<TYPE_CPP_IdentDeclaration> GenUnionInterfaceDecl(const TYPE_AS_Name &);
  TYPE_CPP_CPPAS GenObjRefClass(const TYPE_AS_Name &, const SET<TYPE_AS_Name> &);
  TYPE_CPP_CPPAS GenQuoteDecl(const TYPE_CPP_Identifier &);
  TYPE_CPP_CPPAS GenQuoteDef(const TYPE_CPP_Identifier &);
  TYPE_AS_Id GenCppTpDecl(const TYPE_REP_TypeRep &, const SET<TYPE_AS_Name> &);
#ifdef VDMSL
  TYPE_CPP_CPPAS GenCppTypeDef(const TYPE_AS_Name &, const TYPE_AS_Type &, const Generic &);
#endif //VDMSL
#ifdef VDMPP
  Sequence GenCppTypeDef(const TYPE_AS_Name &, const TYPE_AS_Type &, const Generic &);
#endif //VDMPP
  TYPE_CPP_Name GenVDMTpName(const TYPE_REP_TypeRep &, const Int &, const SET<TYPE_AS_Name> &);
  TYPE_CPP_TypeSpecifier Rep2CppTypeSpecifier(const TYPE_REP_TypeRep &, const Generic &);
#ifdef VDMSL
  SET<TYPE_AS_Name> GetImpModuleNames(const TYPE_AS_Module &);
#endif //VDMSL
#ifdef VDMPP
  SET<TYPE_AS_Name> GetImpModuleNames(const TYPE_AS_Class &);
#endif //VDMPP
  SEQ<TYPE_CPP_Preprocessor> GenModuleCCIncludes();
  SEQ<TYPE_CPP_Preprocessor> GenModuleHIncludes();
  SEQ<TYPE_CPP_Preprocessor> GenHIncludes(const SET<TYPE_AS_Name> &);
  TYPE_CPP_CPPAS GenModuleCCPart();
  TYPE_CPP_CPPAS GenModuleHPart();
#ifdef VDMSL
  TYPE_CPP_Files GenAnonymFiles(const TYPE_AS_Module &); 
#endif //VDMSL
#ifdef VDMPP
  TYPE_CPP_Files GenAnonymFiles(const TYPE_AS_Class &);
#endif //VDMPP

  TYPE_CPP_Identifier GenDlMember();
     
  Generic GetTypeDef(const TYPE_AS_Name & nm);
#endif // __mod_tpgen_h__
