/***
*  * WHAT
*  *    mod_mod.cc: Implementation of mod_mod.vdm 1.35
*  *    I am totally confused about the status of the
*  *    mod_mod.vdm corresponding implementation.
*  *    This implmentation does contain the changes from 1.23 -> 1.23.2.1.
*  *    What about 1.21.4.3 -> 1.23?????????
*  *    
*  *    Note that changes made at the specification level to support
*  *    Java have not been implemented
*  *    
*  * FILE
*  *    $Source: /home/vdmtools/cvsroot/toolbox/code/cg/mod_mod.cc,v $
*  * VERSION
*  *    $Revision: 1.36 $
*  * DATE
*  *    $Date: 2006/03/17 08:29:12 $
*  * STATUS
*  *    Under development
*  * REFERENCES
*  *    
*  * PROJECT
*  *    IDERES/AFRODITE
*  * AUTHOR
*  *    $Author: vdmtools $
*  *    
*  * COPYRIGHT
*  *    (C) Kyushu University
***/

#include "vdmcg.h"
#include "astaux.h"
#include "asquotes.h"
#include "BC.h"
#include "statsem.h"
 
// MOD functions and operations

// GenModule
// md : AS`Module
// ==> CPP`Files
SET<TYPE_CPP_File> vdmcg::GenModule(const TYPE_AS_Module & md)
{
  InitState_TPGEN(md.GetRecord(pos_AS_Module_nm));
  InitState_CGAUX(md.GetRecord(pos_AS_Module_nm));
  vdm_BC_ResetVarno();

  InitState_FCTDEF();
  
  SET<TYPE_CPP_File> file_s;
  file_s.Insert(GenHFile(md));
  file_s.Insert(GenCCFile(md));
  file_s.ImpUnion(GenAnonymFiles(md));
  return file_s;
}

// GenHFile
// mod : AS`Module
// ==> CPP`File
TYPE_CPP_File vdmcg::GenHFile(const TYPE_AS_Module & mod)
{
  const TYPE_AS_Name & nm (mod.GetRecord(pos_AS_Module_nm));

  const TYPE_AS_Interface & intf (mod.GetRecord(pos_AS_Module_intf));
  const Map & imp_m (intf.GetMap(pos_AS_Interface_imp));     // map Name to [ImportSig]
  const Generic & exp (intf.GetField(pos_AS_Interface_exp)); // [ExportSig]

  const TYPE_AS_Definitions & defs (mod.GetRecord(pos_AS_Module_defs));
  const Map & type_m                 (defs.GetMap(pos_AS_Definitions_typem));     // map Name to TypeDef
  const SEQ<TYPE_AS_ValueDef> & vd_l (defs.GetSequence(pos_AS_Definitions_valuem)); // seq of ValueDef
  const Map & fn_m                   (defs.GetMap(pos_AS_Definitions_fnm));         // map Name to FnDef
  const Map & op_m                   (defs.GetMap(pos_AS_Definitions_opm));         // map Name to OpDef
  const Generic & sd                 (defs.GetField(pos_AS_Definitions_State));     // [ StateDef ]

  Generic vals = Nil();
  SET<TYPE_AS_Name> efn_s (fn_m.Dom());
  SET<TYPE_AS_Name> eop_s (op_m.Dom());

  if (! exp.IsNil()) {
    TYPE_AS_ExportSig exrec (exp);
    const Map & val_m (exrec.GetMap(pos_AS_ExportSig_val)); // map Name to Type
    const Map & efn_m (exrec.GetMap(pos_AS_ExportSig_fns)); // map Name to (TypeVarList * FnType)
    const Map & eop_m (exrec.GetMap(pos_AS_ExportSig_ops)); // map Name to OpType
    vals = val_m.Dom();
    efn_s = RemPrePost(efn_m);
    eop_s = RemPrePost(eop_m);
  }

  // IMPORTS

  SEQ<TYPE_CPP_Preprocessor> pp;
  pp.ImpConc(GenImports(imp_m.Dom()));
  pp.ImpConc(GenModuleHIncludes());

  TYPE_CPP_CPPAS cpp;
  cpp.ImpConc(GenTypes(nm, type_m, sd));

  // GENERATE VALUES

  if (cg_OptionGenValues()) {
    cpp.ImpConc(GenExportedValues(vals, vd_l));
    cpp.ImpAppend(GenInitDecl(nm));
  }

  // GENERATE TYPES
  TYPE_CPP_CPPAS gmodh (GenModuleHPart());
  cpp.ImpConc (gmodh);

  // FUNCTION/OPERATION DECLARATIONS
  Map m;
  Generic nm2;
  if (cg_OptionGenFctOps() ) {
    for (bool bb = efn_s.First(nm2); bb; bb = efn_s.Next(nm2)) {
      m.Insert(nm2, fn_m[nm2]);
    }
    for (bool cc = eop_s.First(nm2); cc; cc = eop_s.Next(nm2)) {
      m.Insert(nm2, op_m[nm2]);
    }
    cpp.ImpConc(GenExpSigs(m, sd, mod));
  }

  return vdm_BC_GenFile2(Nil(), GiveLastName(nm).ImpConc(ASTAUX::MkId(L".h")), pp, cpp,
                          mod.GetInt(pos_AS_Module_cid));
}

// RemPrePost
// fnop_m : (map AS`Name to (AS`FnType | AS`OpType)) |
//          (map AS`Name to  [(AS`TypeVarList * AS`FnType)])
// ==>  set of AS`Name
SET<TYPE_AS_Name> vdmcg::RemPrePost(const Map & fnop_m)
{
  Set dom_fnop_m (fnop_m.Dom());
  SET<TYPE_AS_Name> res;
  Generic name_g;
  for (bool bb = dom_fnop_m.First(name_g); bb; bb = dom_fnop_m.Next(name_g)) {
    TYPE_AS_Name name (name_g);
    const TYPE_AS_Ids & ids (name.GetSequence(pos_AS_Name_ids));
    TYPE_AS_Id id (ids.Index(ids.Length()));
    if ( (ASTAUX::MkId(L"pre_") != id.SubSequence(1,4)) &&
         (ASTAUX::MkId(L"post_") != id.SubSequence(1,5)) &&
         (ASTAUX::MkId(L"inv_") != id.SubSequence(1,4)) )
      res.Insert(name);
  }
  return res;
}

// GenCCFile
// mod : AS`Module
// ==> CPP`File
TYPE_CPP_File vdmcg::GenCCFile(const TYPE_AS_Module & mod)
{
  const TYPE_AS_Name & nm (mod.GetRecord(pos_AS_Module_nm));

  const TYPE_AS_Interface & intf (mod.GetRecord(pos_AS_Module_intf));
  const Map & imp_m (intf.GetMap(pos_AS_Interface_imp));     // map Name to [ImportSig]
  const Generic & exp (intf.GetField(pos_AS_Interface_exp)); // [ExportSig]

  const TYPE_AS_Definitions & defs (mod.GetRecord(pos_AS_Module_defs));
  const MAP<TYPE_AS_Name, TYPE_AS_TypeDef> & type_m (defs.GetMap(pos_AS_Definitions_typem));
  const MAP<TYPE_AS_Name, TYPE_AS_FnDef> & fn_m (defs.GetMap(pos_AS_Definitions_fnm));
  const MAP<TYPE_AS_Name, TYPE_AS_OpDef> & op_m (defs.GetMap(pos_AS_Definitions_opm));
  const Generic & sd (defs.GetField(pos_AS_Definitions_State)); // [ StateDef ]

//  TYPE_AS_Id id_h (id);

  SEQ<TYPE_CPP_Preprocessor> pp;
  pp.ImpAppend(vdm_BC_GenInclusion(GiveLastName(nm).ImpConc(ASTAUX::MkId(L".h"))));
  pp.ImpConc(GenModuleCCIncludes());
  pp.ImpConc(GenRename(nm, imp_m));

  TYPE_CPP_CPPAS cpp;
  cpp.ImpConc(GenStaticVars(sd));

  Set efn_s (fn_m.Dom());
  Set eop_s (op_m.Dom());

  if (! exp.IsNil()) {
    TYPE_AS_ExportSig exrec (exp);
    const Map & efn_m (exrec.GetMap(pos_AS_ExportSig_fns));
    const Map & eop_m (exrec.GetMap(pos_AS_ExportSig_ops));
    efn_s = efn_m.Dom();
    eop_s = eop_m.Dom();
  }
  
  // GENERATE TYPES
  cpp.ImpConc(GenModuleCCPart());

  // GENERATE FUNCTIONS AND OPERATIONS
  if ( cg_OptionGenFctOps() ) {
    Map m;
    Set dom_fn_m (fn_m.Dom().Diff(efn_s));
    Generic nm2;
    for (bool bb = dom_fn_m.First(nm2); bb; bb = dom_fn_m.Next(nm2)) {
        m.Insert(nm2, fn_m[nm2]);
    }
    Set dom_op_m (op_m.Dom().Diff(eop_s));
    for (bool cc = dom_op_m.First(nm2); cc; cc = dom_op_m.Next(nm2)) {
        m.Insert(nm2, op_m[nm2]);
    }
    cpp.ImpConc(GenLocalSigs(m, sd));
  }

  // GENERATE TYPE INVARIANT FUNCTIONS
  if (cg_OptionGenTpInv()) {
    cpp.ImpConc(GenTypeInvFcts(type_m, sd));
  }

  // INITIALISATION OF THE VDM LIB W.R.T RECORDS
  //cpp.ImpConc(GenVDMLibInit(nm, type_m, sd));
  SEQ<TYPE_CPP_FunctionDefinition> fd (GenVDMLibInit(nm, type_m, sd));
  if ( !fd.IsEmpty() ) {
    cpp.ImpConc( fd );
  }
  
  // MODULE FUNCTIONS/OPERATIONS
  TYPE_CPP_CPPAS def_l;
  if (cg_OptionGenFctOps()) {
    def_l.ImpAppend(GenInitFct(nm, !fd.IsEmpty()));
    def_l.ImpConc(GenFctDef_MOD(fn_m, sd));
    def_l.ImpConc(GenOpDef_MOD(op_m, sd));
    cpp.ImpConc(IncludeExternalCC());
    cpp.ImpConc(def_l);
  }
   
  return vdm_BC_GenFile2(Nil(), GenFileExt(GiveLastName(nm)), pp, cpp, mod.GetInt(pos_AS_Module_cid));
}

// Note that GenJavaFile is not currently supported

// GenImports
// imp_s : set of AS`Name
// ==> CPP`Preprocessors
SEQ<TYPE_CPP_Preprocessor> vdmcg::GenImports(const SET<TYPE_AS_Name> & imp_s)
{
  SEQ<TYPE_CPP_Preprocessor> pp;
  pp.ImpAppend(vdm_BC_GenSqIncl(ASTAUX::MkId(L"math.h")));
  pp.ImpAppend(vdm_BC_GenInclusion(GiveLibName()));
  pp.ImpAppend(vdm_BC_GenInclusion(ASTAUX::MkId(L"cg.h")));
  pp.ImpAppend(vdm_BC_GenInclusion(ASTAUX::MkId(L"cg_aux.h")));

  Set imp_s_q (imp_s);
  Generic nm;
  for (bool bb = imp_s_q.First(nm); bb; bb = imp_s_q.Next(nm)) {
    pp.ImpAppend(vdm_BC_GenInclusion(GiveLastName(nm).ImpConc(ASTAUX::MkId(L".h"))));
  }
  return pp;
}

// GenRename
// curmod : AS`Name
// imp_m : map AS`Name to [AS`ImportSig]
// ==> CPP`Preprocessors
SEQ<TYPE_CPP_Preprocessor> vdmcg::GenRename(const TYPE_AS_Name & curmod, const Map & imp_m)
{
  Set dom_imp_m (imp_m.Dom());
  TYPE_AS_Id modid (ASTAUX::MkId(L"vdm_").ImpConc(GiveLastName(curmod)).ImpConc(ASTAUX::MkId(L"_")));
  SEQ<TYPE_CPP_Preprocessor> pp;
  Generic nm;
  for (bool bb = dom_imp_m.First(nm); bb; bb = dom_imp_m.Next(nm)) {
    Generic isig (imp_m[nm]);
    if (! isig.IsNil()) {
      TYPE_AS_ImportSig irec (isig);
      const Map & tp_m  (irec.GetMap(pos_AS_ImportSig_tps)); // map Name to [TypeDef]
      const Map & ren_m (irec.GetMap(pos_AS_ImportSig_ren)); // map Name to Name

      TYPE_AS_Id imod (ASTAUX::MkId(L"vdm_").ImpConc((GiveLastName(nm)).ImpConc(ASTAUX::MkId(L"_"))));
      Set dom_ren_m (ren_m.Dom());
      Generic newnm;
      for (bool cc = dom_ren_m.First(newnm); cc; cc = dom_ren_m.Next(newnm)) {
        TYPE_AS_Name oldnm (ren_m[newnm]);
        TYPE_AS_Id newmac (modid);
        newmac.ImpConc(GiveLastName(newnm));
        TYPE_AS_Id oldmac (imod);
        oldmac.ImpConc(GiveLastName(oldnm));

        // TODO: nm in fns or ops
        pp.ImpAppend(vdm_BC_GenIdMacroDef(newmac, oldmac));

        if (tp_m.DomExists(oldnm)) {
          Generic gtd (tp_m[oldnm]);
          if (!gtd.IsNil()) {
            TYPE_AS_TypeDef td (gtd);
            TYPE_AS_Type tp (td.GetRecord(pos_AS_TypeDef_shape));
            if (tp.Is(TAG_TYPE_AS_CompositeType)) {
              pp.ImpConc(GenRenameRecord(nm, newnm, tp));
              pp.ImpConc(GenRenameInv(nm, newnm, td));
            }
          }
          else {
            //mk_CPP`TypeNameRep

            TYPE_REP_TypeNameRep tpnm;
            tpnm.set_nm(oldnm);

            // Use LOT rather than TPGEN_LOT

            TYPE_REP_TypeRep type (LOT(tpnm));

            if ( type.Is(TAG_TYPE_REP_CompositeTypeRep)) {
              // NOT IMPL ??? LTO
              // pp.ImpConc(GenRenameRecord(nm,newnm,TPGEN_Rep2AsType(type)));
            }
          }
        }
      }
    }
  }
  return pp;
}

// GenRenameRecord
// oldmod : AS`Name
// newnm : AS`Name
// tp : AS`CompositeType
// ==> CPP`Preprocessors
SEQ<TYPE_CPP_Preprocessor> vdmcg::GenRenameRecord(const TYPE_AS_Name & oldmod,
                                                  const TYPE_AS_Name & newnm,
                                                  const TYPE_AS_CompositeType & tp)
{
  SEQ<TYPE_CPP_Preprocessor> pp;
  const TYPE_AS_Name & oldnm (tp.GetRecord(pos_AS_CompositeType_name));
  const SEQ<TYPE_AS_Field> & field_l (tp.GetSequence(pos_AS_CompositeType_fields));
  const TYPE_CI_ContextId & cid (oldnm.GetInt(pos_AS_Name_cid));
  TYPE_AS_Ids oldmod_l (oldmod.GetSequence(pos_AS_Name_ids));
  oldmod_l.ImpConc(oldnm.GetSequence(pos_AS_Name_ids));
  TYPE_AS_Name oldnm2 (ASTAUX::MkNameFromIds(oldmod_l, cid));

  pp.ImpAppend(vdm_BC_GenIdMacroDef(vdm_BC_GenLengthId(newnm), vdm_BC_GenLengthId(oldnm2)));

  size_t len_field_l = field_l.Length();
  for (size_t idx = 1; idx <= len_field_l; idx++) {
    const TYPE_AS_Field & field (field_l[idx]);
    const Generic & sel (field.GetField(pos_AS_Field_sel));
    if (!sel.IsNil()) {
      pp.ImpAppend(vdm_BC_GenIdMacroDef(vdm_BC_GenPosId(newnm, sel), vdm_BC_GenPosId(oldnm2, sel)));
    }
  }

  TYPE_AS_Id oldname (ASTAUX::MkId(L"TYPE_").ImpConc(GiveLastName(oldmod)).ImpConc(ASTAUX::MkId(L"_"))
                                            .ImpConc(GiveLastName(oldnm)));
  TYPE_AS_Id newname (ASTAUX::MkId(L"TYPE_").ImpConc(GiveCurCName()).ImpConc(ASTAUX::MkId(L"_"))
                                            .ImpConc(GiveLastName(newnm)));
  pp.ImpAppend(vdm_BC_GenIdMacroDef(newname, oldname));
  return pp;
}

// GenRenameInv
// oldname : AS`Name
// newnm : AS`Name
// td : AS`TypeDef
// ==> CPP`Preprocessors
SEQ<TYPE_CPP_Preprocessor> vdmcg::GenRenameInv(const TYPE_AS_Name & oldmod,
                                               const TYPE_AS_Name & newnm,
                                               const TYPE_AS_TypeDef & td)
{
  const Generic & Inv (td.GetField(pos_AS_TypeDef_Inv));

  if (Inv.IsNil()) {
    return SEQ<TYPE_CPP_Preprocessor>();
  }
  else {
    const TYPE_AS_Name & oldnm (td.GetRecord(pos_AS_TypeDef_nm));
    TYPE_AS_Name ppnm (vdm_BC_GivePrePostName(oldnm, ASTAUX::MkId(L"inv")));
    TYPE_CI_ContextId cid (ppnm.get_cid());
    TYPE_AS_Ids oldmod_l (oldmod.get_ids());
    oldmod_l.ImpConc(ppnm.get_ids());
    TYPE_AS_Name oldnm_q (ASTAUX::MkNameFromIds(oldmod_l, cid));
    TYPE_AS_Name newnm_q (vdm_BC_GivePrePostName(newnm, ASTAUX::MkId(L"inv")));
    TYPE_AS_Id oldid (vdm_BC_Rename(oldnm_q).GetField(1));
    TYPE_AS_Id newid (vdm_BC_Rename(newnm_q).GetField(1));

    SEQ<TYPE_CPP_Preprocessor> res;
    res.ImpAppend(vdm_BC_GenIdMacroDef(newid, oldid));
    return res;
  }
}

// This function is not in the specification, but is used to simplify GenTypes below.
// TypeMapToSeq 
// type_m : map Name to TypeDef
// sdg : [AS`StateDef]
SEQ<TYPE_AS_TypeDef> vdmcg::TypeMapToSeq(const MAP<TYPE_AS_Name, TYPE_AS_TypeDef> & type_m,
                                         const Generic & sdg) const
{
  SEQ<TYPE_AS_TypeDef> type_l;

  if (!sdg.IsNil()) {
    TYPE_AS_StateDef sd (sdg);
    const TYPE_AS_CompositeType & tp (sd.GetRecord(pos_AS_StateDef_tp));
    const Generic & Inv (sd.GetField(pos_AS_StateDef_Inv));
    type_l.ImpAppend(TYPE_AS_TypeDef().Init(tp.get_name(), tp, Inv, Nil(), Nil(), Int(DEFAULT_AS), NilContextId));
  }

  Set dom_type_m (type_m.Dom());
  Generic tpnm;
  for (bool bb = dom_type_m.First(tpnm); bb; bb = dom_type_m.Next(tpnm)) {
    type_l.ImpAppend(type_m[tpnm]);
  }
  return type_l;
}

// GenTypes
// nm s: AS`Name
// type_m : map AS`Name to AS`TypeDef
// sd : [AS`StateDef]
// ==> CPP`CPPAS
TYPE_CPP_CPPAS vdmcg::GenTypes(const TYPE_AS_Name & nm, const MAP<TYPE_AS_Name, TYPE_AS_TypeDef> & type_m,
                               const Generic & sd)
{
  SEQ<TYPE_AS_TypeDef> type_l (TypeMapToSeq(type_m, sd));
  TYPE_CPP_CPPAS cpp;
  cpp.ImpConc (GenTypeDef_DS(nm, type_l));
  cpp.ImpConc (GenInvEqOrdDecl(type_l));
  return cpp;
}

// GenTypeInvFcts
// type_m : map AS`Name to AS`TypeDef
// sd : [AS`StateDef]
// ==> CPP`CPPAS
TYPE_CPP_CPPAS vdmcg::GenTypeInvFcts(const MAP<TYPE_AS_Name, TYPE_AS_TypeDef> & type_m,
                                     const Generic & sd)
{
  TYPE_CPP_CPPAS cpp;
  cpp.ImpConc (GenInvEqOrdDef(TypeMapToSeq(type_m, sd)));
  return cpp;
}

// GenExportedValues
// nms : [set of AS`Name]
// vd_l : seq of AS`ValueDef
// ==> seq of CPP`IdentDeclaration
SEQ<TYPE_CPP_IdentDeclaration> vdmcg::GenExportedValues(const Generic & nms, const SEQ<TYPE_AS_ValueDef> & vd_l)
{
  // ==> (seq of (CPP`DeclSpecifier * CPP`Name)) * (seq of CPP`Stmt);
  PushEnv();
  Tuple tmp (GenValDef(vd_l));
  PopEnv();
  const Sequence & decl_l (tmp.GetSequence(1)); // seq of (CPP`DeclSpecifier * CPP`Name))
  const SEQ<TYPE_CPP_Stmt> & stmt_l (tmp.GetSequence(2)); // (seq of CPP`Stmt)

  SEQ<TYPE_CPP_IdentDeclaration> res_l;

  this->idecl_l.Clear();

  if (! decl_l.IsEmpty()) {
    this->vi_l = stmt_l;
  }
  else {
    this->vi_l.Clear();
  }
  if (nms.IsNil()) {
    size_t len_decl_l = decl_l.Length();
    for (size_t idx = 1; idx <= len_decl_l; idx++) {
      const Tuple & t (decl_l[idx]); // (CPP`DeclSpecifier * CPP`Name)
      SEQ<TYPE_CPP_DeclSpecifier> d_l;
      d_l.ImpAppend(t.GetRecord(1));
      const TYPE_CPP_Name & nm (t.GetRecord(2));
      this->idecl_l.ImpAppend(vdm_BC_GenIdentDecl(SEQ<TYPE_CPP_Annotation>(), d_l, nm, Nil()));
      TYPE_CPP_Modifier ext (vdm_BC_GenModifier(quote_EXTERN));
      d_l.ImpPrepend(ext);
      res_l.ImpAppend(vdm_BC_GenIdentDecl(SEQ<TYPE_CPP_Annotation>(), d_l, nm, Nil()));
    }
  }
  else {
    Set id_s, nm_s (nms);
    Generic nm;
    for (bool bb = nm_s.First(nm); bb; bb = nm_s.Next(nm)) {
      id_s.Insert(vdm_BC_Rename(nm));
    }
    size_t len_decl_l = decl_l.Length();
    for (size_t idx = 1; idx <= len_decl_l; idx++) {
      const Tuple & t (decl_l[idx]); // (CPP`DeclSpecifier * CPP`Name)
      SEQ<TYPE_CPP_DeclSpecifier> d_l;
      d_l.ImpAppend(t.GetRecord(1));
      const TYPE_CPP_Name & nm (t.GetRecord(2));
      if (id_s.InSet(nm)) {
        this->idecl_l.ImpAppend(vdm_BC_GenIdentDecl(SEQ<TYPE_CPP_Annotation>(), d_l, nm, Nil()));
        TYPE_CPP_Modifier ext(vdm_BC_GenModifier(quote_EXTERN));
        d_l.ImpPrepend(ext);
        res_l.ImpAppend(vdm_BC_GenIdentDecl(SEQ<TYPE_CPP_Annotation>(), d_l, nm, Nil()));
      }
      else {
        TYPE_CPP_Modifier sta(vdm_BC_GenModifier(quote_STATIC));
        d_l.ImpPrepend(sta);
        this->idecl_l.ImpAppend(vdm_BC_GenIdentDecl(SEQ<TYPE_CPP_Annotation>(), d_l, nm, Nil()));
      }
    }
  }
  return res_l;
}

// GenInitFct
// nm : AS`Name
// genvdmlib : bool
// ==> CPP`FunctionDefinition
TYPE_CPP_FunctionDefinition vdmcg::GenInitFct(const TYPE_AS_Name & nm, bool genvdmlib)
{
  SEQ<TYPE_CPP_Stmt> rb_l;
  if (genvdmlib) {
    TYPE_AS_Id id_lib (ASTAUX::MkId(L"init_").ImpConc(GiveLastName(nm)).ImpConc(ASTAUX::MkId(L"_VDMLib")));
    TYPE_CPP_Identifier id_lib_init (vdm_BC_GenIdentifier(id_lib));
    TYPE_CPP_Stmt f_lib_init (vdm_BC_GenExpressionStmt(vdm_BC_GenFctCall(id_lib_init, type_dL())));
    rb_l.ImpAppend(f_lib_init);
  }
  rb_l.ImpConc (this->vi_l);

  TYPE_CPP_Stmt ini (vdm_BC_GenBlock(rb_l));
  SEQ<TYPE_CPP_DeclSpecifier> ds_l;
  ds_l.ImpAppend(vdm_BC_GenTypeSpecifier(vdm_BC_GenVoid()));
  TYPE_CPP_Identifier id (vdm_BC_GenIdentifier( ASTAUX::MkId(L"init_").ImpConc(GiveLastName(nm))));
  TYPE_CPP_FctDecl fdecl (vdm_BC_GenFctDecl(id, type_dL()));

  return vdm_BC_GenFctDef(ds_l, fdecl, Nil(), ini);
}

// GenInitDecl
// nm : AS`Name
// ==> CPP`IdentDeclaration
TYPE_CPP_IdentDeclaration vdmcg::GenInitDecl(const TYPE_AS_Name & nm)
{
  TYPE_CPP_Identifier id (vdm_BC_GenIdentifier(ASTAUX::MkId(L"init_").ImpConc(GiveLastName(nm))));
  SEQ<TYPE_CPP_DeclSpecifier> ds_l;
  ds_l.ImpAppend(vdm_BC_GenTypeSpecifier(vdm_BC_GenVoid()));

  return vdm_BC_GenIdentDecl(SEQ<TYPE_CPP_Annotation>(), ds_l, vdm_BC_GenFctDecl(id, type_dL()), Nil());
}

// GenStaticVars
// sd : [AS`StateDef]
// ==> seq of CPP`IdentDeclaration
SEQ<TYPE_CPP_IdentDeclaration> vdmcg::GenStaticVars(const Generic & sd)
{
  if (sd.IsNil()) {
    return this->idecl_l;
  }
  else {
    Tuple t (GenStateDef(sd)); // CPP`CPPAS * seq of CPP`Stmt
    this->vi_l.ImpConc(t.GetSequence(2));
    return this->idecl_l.ImpConc(t.GetSequence(1));
  }
}

// GenExpSigs
// def_m : map AS`Name to (AS`FnDef | AS`OpDef)
// sd : [AS` StateDef]
// md : AS`Module
// ==> CPP`CPPAS
TYPE_CPP_CPPAS vdmcg::GenExpSigs(const Map & def_m, const Generic & sd, const TYPE_AS_Module & md)
{
  TYPE_CPP_CPPAS cpp;
  Set dom_def_m (def_m.Dom());
  Generic nm;
  for (bool bb = dom_def_m.First(nm); bb; bb = dom_def_m.Next(nm)) {
    cpp.ImpConc(GenExpFctOpDecl(nm, def_m[nm], Nil(), sd, md));
  }
  if (!sd.IsNil()) {
    Generic fn (GenInitStateFnDef(sd));
    if (!fn.IsNil()) {
      TYPE_AS_Name inm (Record(fn).GetRecord(pos_AS_ExplFnDef_nm));
      cpp.ImpConc(GenExpFctOpDecl(inm, fn, Nil(), sd, md));
    }
  }
  return cpp;
}

// GenLocalSigs
// fn_m : map AS`Name to (AS`FnDef | AS`OpDef)
// sd : [AS`StateDef]
// ==> CPP`CPPAS
TYPE_CPP_CPPAS vdmcg::GenLocalSigs(const Map & fn_m, const Generic & sd)
{
  TYPE_CPP_CPPAS cpp;
  TYPE_CPP_Modifier sta(vdm_BC_GenModifier(quote_STATIC));
  Set dom_fn_m (fn_m.Dom());
  Generic nm;
  for (bool bb = dom_fn_m.First(nm); bb; bb = dom_fn_m.Next(nm)) {
    cpp.ImpConc(GenFctOpDecl(nm, fn_m[nm], sta, sd));
  }
  return cpp;
}

// GenFctDef_MOD
// fn_m : map AS`Name to AS`FnDef
// sd : [AS`StateDef]
// ==> CPP`CPPAS
TYPE_CPP_CPPAS vdmcg::GenFctDef_MOD(const MAP<TYPE_AS_Name, TYPE_AS_FnDef> & fn_m, const Generic & sd)
{
  TYPE_CPP_CPPAS cpp;
  Set dom_fn_m (fn_m.Dom());
  Generic nm;
  for (bool bb = dom_fn_m.First(nm); bb; bb = dom_fn_m.Next(nm)) {
    cpp.ImpConc(GenFctDef_FD(fn_m[nm], false));
  }
  if (!sd.IsNil()) {
    Generic fn (GenInitStateFnDef(sd));
    if (!fn.IsNil()) {
      cpp.ImpConc(GenFctDef_FD(fn, false));
    }
  }
  return cpp;
}

// GenOpDef_MOD
// op_m : map AS`Name to AS`OpDef
// eop_s : set of AS`Name
// sd : [AS`StateDef]
//==> CPP`CPPAS
TYPE_CPP_CPPAS vdmcg::GenOpDef_MOD(const MAP<TYPE_AS_Name, TYPE_AS_OpDef> & op_m, const Generic & sd)
{
  TYPE_CPP_CPPAS cpp;
  Set dom_op_m (op_m.Dom());
  Generic nm;
  for (bool bb = dom_op_m.First(nm); bb; bb = dom_op_m.Next(nm)) {
    CurrentMethod (nm);
    cpp.ImpConc(GenOpDef_FD(nm, op_m[nm], sd, false));
  }
  return cpp;
}

// GenInitStateFnDef
// sd : AS`StateDef
// -> [AS`ExplFnDef]
Generic vdmcg::GenInitStateFnDef(const TYPE_AS_StateDef & sd)
{
  const Generic & Init (Record(sd).GetField(pos_AS_StateDef_Init));
  if (!Init.IsNil()) {
    const TYPE_AS_CompositeType & tp (Record(sd).GetRecord(pos_AS_StateDef_tp));
    const TYPE_AS_Name & snm (tp.GetRecord(pos_AS_CompositeType_name));
    const TYPE_AS_Pattern & pattern (Record(Init).GetRecord(pos_AS_StateInit_pat));
    const TYPE_AS_Expr & expr (Record(Init).GetRecord(pos_AS_StateInit_expr));

    TYPE_AS_Name inm (vdm_BC_GivePrePostName(snm, ASTAUX::MkId(L"init")));
    TYPE_AS_TotalFnType tfp;
    tfp.Init(mk_sequence(tp), TYPE_AS_BooleanType().Init(NilContextId),NilContextId);
    TYPE_AS_ExplFnDef func;
    func.Init(inm,
              SEQ<TYPE_AS_TypeVar>(),
              tfp,
              mk_sequence(mk_sequence(pattern)),
              TYPE_AS_FnBody().Init(expr, NilContextId),
              Nil(),
              Nil(),
              Int(PUBLIC_AS),
              Bool(false),
              Nil(),
              NilContextId);
    return func;
  }
  return Nil();
}
