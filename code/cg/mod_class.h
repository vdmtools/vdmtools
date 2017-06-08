/***
*  * WHAT
*  *    Implementation of mod_class.vdm
*  * FILE
*  *    $Source: /home/vdmtools/cvsroot/toolbox/code/cg/mod_class.h,v $
*  * VERSION
*  *    $Revision: 1.22 $
*  * DATE
*  *    $Date: 2006/03/27 07:18:41 $
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

#ifndef __mod_class_h__
#define __mod_class_h__

private:
// Module state
  MAP<TYPE_AS_Name,TYPE_AS_Class> classDefs; // map AS`Name to AS`Class
  MAP<TYPE_AS_Name,TYPE_CONC_ClassInfo> classInfos; // map AS`Name to Class Info
  MAP<TYPE_AS_Name,Int> operConst;        // map AS`Name to nat
  TYPE_AS_Name defaultCast;               // AS`Name
  TYPE_AS_Name currentClass;              // AS`Name

private:
  SET<TYPE_CPP_File> GenClasses(const SEQ<TYPE_AS_Class> &,
                                const SET<TYPE_AS_Name> &,
                                const Bool &,
                                const Bool &,
                                const Bool &,
                                const Bool &, 
                                const Generic &,
                                const Bool &,
                                const Bool &);
//Set GenIndExtension(const Set&);
  TYPE_CPP_File GenHFile(const TYPE_AS_Class &);
  TYPE_CPP_File GenJavaFile(const TYPE_AS_Class &, bool);
  SEQ<TYPE_CPP_Preprocessor> GenInclusions(const SET<TYPE_AS_Name> &);
  TYPE_CPP_File GenCCFile(const TYPE_AS_Class &);
  TYPE_CPP_File GenJavaExternalFile(const TYPE_AS_Class &);
  SEQ<TYPE_CPP_ImportDeclaration> GenImportDeclarations();
  TYPE_CPP_CPPAS GenClassInit(const TYPE_AS_Name &, const SEQ<TYPE_AS_TypeDef> &);
  SEQ<TYPE_CPP_Declaration> GenStaticInstVarInit(const SEQ<TYPE_AS_InstanceVarDef> &, const TYPE_AS_Name &);
  SEQ<TYPE_CPP_MemberDeclaration> GenLibMethods(const TYPE_AS_Name &);
  Record GenHead(const TYPE_AS_Name&, const SEQ<TYPE_AS_Name> &, bool);
  TYPE_CPP_ClassHead GenExternalClassHead();
  SEQ<TYPE_CPP_PackageName> GenInterfaceInheritance(const SEQ<TYPE_AS_Name> & nms);
  SEQ<TYPE_CPP_BaseSpecifier> GenInheritance(const SEQ<TYPE_AS_Name> &);
  SEQ<TYPE_CPP_MemberDeclaration> GenConstructorDef(const TYPE_AS_Name &);
//Sequence GenConstructorExternalJavaDef(const TYPE_AS_Name&);
  SEQ<TYPE_CPP_FunctionDefinition> GenConstructorExternalJavaDef();
  SEQ<TYPE_CPP_FunctionDefinition> GenJavaConstructorDef(const TYPE_AS_Name &, const SEQ<TYPE_AS_InstanceVarDef> &);
  SEQ<TYPE_CPP_MemberDeclaration> GenConstructorDef_cc(
                                    const TYPE_AS_Name &,
                                    const SEQ<TYPE_AS_InstanceVarDef> &,
                                    int, bool);
  SEQ<TYPE_CPP_MemberDeclaration> GenJavaClassInit();
  SEQ<TYPE_CPP_Stmt> GenInitBaseVars(bool);
  TYPE_CPP_Stmt GenDlInit(const TYPE_AS_Name &);
  Generic GenInstVars(const SEQ<TYPE_AS_InstanceVarDef> &);
  TYPE_CPP_MemberDeclaration GenInstVar(const TYPE_AS_InstAssignDef &);
  SEQ<TYPE_CPP_Stmt> GenVarInits(const SEQ<TYPE_AS_InstanceVarDef> &);
  SEQ<TYPE_CPP_Stmt> GenVarInit(const TYPE_AS_InstAssignDef &);
  bool IsCandidateInterface(const TYPE_AS_Class & cl);
  bool IsJ2VAbstractOp(const TYPE_AS_OpDef &) const;
  bool IsPossibleInterface(const TYPE_AS_Class & cl);

public:
  SET<TYPE_AS_Name> GetPossibleInterfaces(const SEQ<TYPE_AS_Class> & classes);

private:
  Int GetFnDefAccess (const TYPE_AS_FnDef & fd) const;
  Int GetOpDefAccess (const TYPE_AS_OpDef & fd) const;
  Map MergeMemberDecl (const Map & cur, const Map & orig) const;

  TYPE_CPP_FunctionDefinition GenInstanceVarInit(const TYPE_AS_Name& nm, const SEQ<TYPE_AS_InstanceVarDef> & iInit_l);
  SEQ<TYPE_CPP_FunctionDefinition> GenInstanceInv(const TYPE_AS_Name& nm, const SEQ<TYPE_AS_InstanceVarDef> & iInit_l);

  SEQ<TYPE_AS_ValueDef> GetJCGOption (const SEQ<TYPE_AS_ValueDef> & vals);
#endif // __mod_class_h__
