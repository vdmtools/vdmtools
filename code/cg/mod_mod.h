/***
*  * WHAT
*  *    Implementation of mod_mod.vdm
*  * FILE
*  *    $Source: /home/vdmtools/cvsroot/toolbox/code/cg/mod_mod.h,v $
*  * VERSION
*  *    $Revision: 1.6 $
*  * DATE
*  *    $Date: 2006/03/17 08:29:12 $
*  * STATUS
*  *    Under development
*  * REFERENCES
*  *    
*  * PROJECT
*  *    IDERS
*  * AUTHOR
*  *    Henrik Voss + $Author: vdmtools $
*  *
*  * COPYRIGHT
*  *    (C) Kyushu University
***/

#ifndef __mod_mod_h__
#define __mod_mod_h__

private:
  SEQ<TYPE_CPP_Stmt> vi_l;     // seq of CPP`Stmt
  SEQ<TYPE_CPP_IdentDeclaration> idecl_l; // seq of CPP`IdentDeclaration

private:
  SET<TYPE_CPP_File> GenModule(const TYPE_AS_Module &);
  TYPE_CPP_File GenHFile(const TYPE_AS_Module &);
  SET<TYPE_AS_Name> RemPrePost(const Map &);
  TYPE_CPP_File GenCCFile(const TYPE_AS_Module &);
  SEQ<TYPE_CPP_Preprocessor> GenImports(const SET<TYPE_AS_Name> &);
  SEQ<TYPE_CPP_Preprocessor> GenRename(const TYPE_AS_Name &, const Map &);
  SEQ<TYPE_CPP_Preprocessor> GenRenameRecord(const TYPE_AS_Name &, const TYPE_AS_Name &, const TYPE_AS_CompositeType &);
  SEQ<TYPE_CPP_Preprocessor> GenRenameInv(const TYPE_AS_Name &, const TYPE_AS_Name &, const TYPE_AS_TypeDef &);
  TYPE_CPP_CPPAS GenTypes(const TYPE_AS_Name & nm, const MAP<TYPE_AS_Name, TYPE_AS_TypeDef> & type_m,
                          const Generic & sd);
  TYPE_CPP_CPPAS GenTypeInvFcts(const MAP<TYPE_AS_Name, TYPE_AS_TypeDef> &, const Generic &);
  SEQ<TYPE_CPP_IdentDeclaration> GenExportedValues(const Generic &, const SEQ<TYPE_AS_ValueDef> &);
  TYPE_CPP_FunctionDefinition GenInitFct(const TYPE_AS_Name &, bool);
  TYPE_CPP_IdentDeclaration GenInitDecl(const TYPE_AS_Name &);
  SEQ<TYPE_CPP_IdentDeclaration> GenStaticVars(const Generic &);
  TYPE_CPP_CPPAS GenExpSigs(const Map &, const Generic &, const TYPE_AS_Module &);
  TYPE_CPP_CPPAS GenLocalSigs(const Map &, const Generic &);
  TYPE_CPP_CPPAS GenFctDef_MOD(const MAP<TYPE_AS_Name, TYPE_AS_FnDef> &, const Generic &);
  TYPE_CPP_CPPAS GenOpDef_MOD(const MAP<TYPE_AS_Name, TYPE_AS_OpDef> &, const Generic &);

  SEQ<TYPE_AS_TypeDef> TypeMapToSeq(const MAP<TYPE_AS_Name, TYPE_AS_TypeDef> &, const Generic &) const;
  Generic GenInitStateFnDef(const TYPE_AS_StateDef & sd);

#endif // __mod_mod_h__
