/***
*  * WHAT
*  *  This file defines the interface from the intermediate UML
*  *  representation towards the VDM++ files.
*  *
*  *
*  **  * FILE
*  *    $Source: /home/vdmtools/cvsroot/toolbox/code/uml/code/vppinterface.cc,v $
*  * VERSION
*  *    $Revision: 1.32 $
*  * DATE
*  *    $Date: 2006/04/26 05:50:09 $
*  * FORMAT
*  *    $State: Exp $
*  * PROJECT
*  *    VDM-SL Toolbox.
*  * AUTHOR
*  *    Ole Storm + $Author: vdmtools $
*  * COPYRIGHT
*  *    (C) Kyushu University
***/

#include "metaiv.h"
#include "AUML.h"
#include "BUILD.h"
#include "MERGE.h"
#include "vppinterface.h"
#include "mini-p-print.h"
#include "projectval.h"
#include "specfile.h"
#include "tools.h"
#include "MANGLE.h"
#include "tbutils.h"
#include <string>

TYPE_AUML_InstanceVars VppInterface::Assoc2Instvars(const SEQ<Char> & nm, const Map &ass_m)
  // Converts associations in ass_m to instance variables.
{
  Set dom_ass_m (ass_m.Dom());
  TYPE_AUML_InstanceVars m;
  Generic gc;
  for (bool bb = dom_ass_m.First(gc); bb; bb = dom_ass_m.Next(gc))
  {
    TYPE_AUML_Role rl (ass_m[gc]);
    TYPE_AUML_ValDef val;
    val.set_tp(vdm_BUILD_RoleToAs(rl));
    val.set_expr(rl.get_initval());
    val.set_acc(rl.get_acc());
    val.set_stat(rl.get_stat());
    m.Insert(gc, val);
  }
  return m;
}

// ListEntities
// m : map Name to ValDef       (pos_AUML_Class_inst) |
//     map AS`Pattern to ValDef (pos_AUML_Class_val)  |
//     map Name to Signature    (pos_AUML_Class_meth) |
//     map Name to Signature    (pos_AUML_Class_func)
// type : pos_AUML_Class_inst | pos_AUML_Class_val | pos_AUML_Class_meth | pos_AUML_Class_func
// -> seq of char
wstring VppInterface::ListEntities(const Map & m, int type)
{
  wstring out;
  if(!m.IsEmpty()) {
    out += type == 0 ? L"" : ent_type2wstring(type) + L"\n";
    Set dom_m (m.Dom());
    Generic gc;
    for (bool bb = dom_m.First(gc); bb; bb = dom_m.Next(gc)) {
      out += PPEntity(type, gc, m[gc], L"  ");
      out += PPBody(type, gc, m[gc], L"  ");
    }
    out += L"\n";
  }
  return out;
}

// GetTypeString
wstring VppInterface::GetTypeString(int type, int plur)
{
  wstring s;
  switch(type) {
    case pos_AUML_Class_inst:
      s = L"instance variable";
      break;
//    case pos_AUML_Class_time:
//      s = L"time variable";
//      break;
    case pos_AUML_Class_val:
      s = L"value";
      break;
    case pos_AUML_Class_meth:
      s = L"method";
      break;
    case pos_AUML_Class_func:
      s = L"function";
      break;
  }
  s += plur > 1 ? L"s" : L"";
  return s;
}

// ent_type2wstring
// difftype : pos_AUML_Class_inst | pos_AUML_Class_val | pos_AUML_Class_meth | pos_AUML_Class_func
// -> seq of char
wstring VppInterface::ent_type2wstring(int difftype)
{
  switch(difftype) {
    case pos_AUML_Class_inst:
      return L"instance variables";
//    case pos_AUML_Class_time:
//      return L"time variables";
    case pos_AUML_Class_val:
      return L"values";
    case pos_AUML_Class_meth:
      return L"operations";
    case pos_AUML_Class_func:
      return L"functions";
    case UML_assiciation_type:
      return L"associations defined in this class";
    case UML_inheritance_type:
      return L"super classes";
  }
  return wstring();
}

// Pretty print pre and post conditions
// PPConditions
// type : pos_AUML_Class_meth | pos_AUML_Class_func
// dom : Name
// rng : Signature
// indent : seq of char
// -> seq of char
wstring VppInterface::PPConditions(int type, const Generic & dom, const Generic & rng, const wstring & indent)
{
  wstring out;
  if(type == pos_AUML_Class_meth || type == pos_AUML_Class_func){
    TYPE_AUML_Signature sign(rng);
    if (!sign.get_precond().IsNil())
      out+= L"\n" + indent + L"pre " + MPP::MiniPP(sign.get_precond());
    if (!sign.get_postcond().IsNil())
      out+= L"\n" + indent + L"post " + MPP::MiniPP(sign.get_postcond());
  }
  return out;
}

// PPBody
// type : pos_AUML_Class_inst | pos_AUML_Class_val | pos_AUML_Class_meth | pos_AUML_Class_func
// dom : AUML`Name      (pos_AUML_Class_inst) |
//       AS`Pattern     (pos_AUML_Class_val)  |
//       AUML`Name      (pos_AUML_Class_meth) |
//       AUML`Name      (pos_AUML_Class_func)
// rng : AUML`ValDef    (pos_AUML_Class_inst) |
//       AUML`ValDef    (pos_AUML_Class_val)  |
//       AUML`Signature (pos_AUML_Class_meth) |
//       AUML`Signature (pos_AUML_Class_func)
// indent : seq of char
// -> seq of char
wstring VppInterface::PPBody(int type, const Generic & dom, const Generic & rng, const wstring & indent)
  // Returns a wstring representing the body of a function or an operation.
{
  wstring out;
  if(type == pos_AUML_Class_meth || type == pos_AUML_Class_func) {
    TYPE_AUML_Signature sign(rng);
    if(sign.get_kind() != Quote(L"IMPL"))
    {
      out += L" ==\n" + indent + L"  is not yet specified";
    }
    out += PPConditions(type, dom, rng, indent);
  }
  out +=L";\n";
  if(type == pos_AUML_Class_meth || type == pos_AUML_Class_func)
    out += L"\n";
  return out;
}

// PPAccess
wstring VppInterface::PPAccess(const Quote & acc)
{
  if (acc == Quote(L"PUBLIC"))
    return L"public ";
  if (acc == Quote(L"PRIVATE"))
    return L"private ";
  if (acc == Quote(L"PROTECTED"))
    return L"protected ";
  return L"";
}

wstring VppInterface::DoImplSign(const wstring & nm,
                                 const TYPE_AUML_Signature &sign,
                                 int ent_type,
                                 const wstring & indent)
{
  wstring s = PPAccess(sign.get_acc());
  if (sign.get_stat().GetValue())
    s += wstring(L"static ");
  s += nm + L"(";

  SEQ<TYPE_AUML_PatType> pt_l (sign.get_param());
  Generic gc;
  bool bb = pt_l.First(gc);
  while(bb) { // Traverse the sequence of parameter types:
    s += MPP::MiniPP(TYPE_AUML_PatType(gc).get_pat());
    s += L" : " + MPP::MiniPP(TYPE_AUML_PatType(gc).get_tp());
    bb = pt_l.Next(gc);
    if(bb)
      s += L", ";
  }
  s += L") ";
  s += L"RESULT";
  s += L" : ";
  s += MPP::MiniPP( sign.get_returntype());

  return s;
}

wstring VppInterface::DoExplSign(const wstring & nm,
                                 const TYPE_AUML_Signature & sign,
                                 int ent_type,
                                 const wstring & indent)
{
  wstring s = PPAccess(sign.get_acc());
  if (sign.get_stat().GetValue())
    s += wstring(L"static ");
  s += nm + L" : ";

  SEQ<TYPE_AUML_PatType> pt_l (sign.get_param());
  Generic gc;
  bool bb = pt_l.First(gc);
  if(!bb)
    s += L"()";
  while(bb) { // Traverse the sequence of parameter types:
    s += MPP::MiniPP(TYPE_AUML_PatType(gc).get_tp());
    bb = pt_l.Next(gc);
    if(bb)
      s += L" * ";
  }
  // Operation or Function
  s += (ent_type == pos_AUML_Class_meth ? L" ==> "
                                        : (sign.get_fntype() == Quote(L"PARTIAL") ? L" -> " : L" +> " ));
  s += MPP::MiniPP( sign.get_returntype() );
  s += L"\n" + indent + nm + L"(";

  bb = pt_l.First(gc);
  while(bb) { // The parameter names:
    s += MPP::MiniPP(TYPE_AUML_PatType(gc).get_pat());
    bb = pt_l.Next(gc);
    if(bb)
      s += L", ";
  }
  s += L")";
  return s;
}

// PPEntity
// type : pos_AUML_Class_inst | pos_AUML_Class_val | pos_AUML_Class_meth | pos_AUML_Class_func | UML_assiciation_type
// dom : AUML`Name      (pos_AUML_Class_inst) |
//       AS`Pattern     (pos_AUML_Class_val)  |
//       AUML`Name      (pos_AUML_Class_meth) |
//       AUML`Name      (pos_AUML_Class_func) |
//       AUML`Name      (UML_assiciation_type)
// rng : AUML`ValDef    (pos_AUML_Class_inst) |
//       AUML`ValDef    (pos_AUML_Class_val)  |
//       AUML`Signature (pos_AUML_Class_meth) |
//       AUML`Signature (pos_AUML_Class_func) |
//       AUML`Role      (UML_assiciation_type)
// indent : seq of char
// -> seq of char
wstring VppInterface::PPEntity(int ent_type, const Generic & dom, const Generic & rng, const wstring & indent)
  // Returns a wstring representing an entity, i.e. an instance
  // variable, a value, a time variable, etc.
  // Uses MiniPP().
{
  wstring s;
  s += indent;
  switch(ent_type){
    case pos_AUML_Class_inst: {
      wstring temp;
      Sequence(dom).GetString(temp);
      TYPE_AUML_ValDef val(rng);
      s += PPAccess(val.get_acc());
      if (val.get_stat().GetValue())
        s += L" static ";
      s += temp + L" : " + MPP::MiniPP(val.get_tp());
      s += val.get_expr().IsNil() ? L"" : L" := " + MPP::MiniPP(val.get_expr());
      break;
    }
//    case pos_AUML_Class_time: {
//      wstring temp;
//      Sequence(dom).GetString(temp);
//      TYPE_AUML_TVarDef tvd(rng);
//      s += tvd.get_kind().IsNil() ? L"" : L"input ";
//      s += temp + L" : " + MPP::MiniPP(tvd.get_tp());
//      break;
//    }
    case pos_AUML_Class_val: {
      TYPE_AUML_ValDef val(rng);
      s += PPAccess(val.get_acc());
      if (val.get_stat().GetValue())
        s += wstring(L"static ");
      s += MPP::MiniPP(dom); // The pattern,
      s += val.get_tp().IsNil() ? L" = " : L" : " + MPP::MiniPP(val.get_tp()) + L" = ";
      s += MPP::MiniPP(val.get_expr());
      break;
    }
    case pos_AUML_Class_meth:
    case pos_AUML_Class_func: {
      wstring nm;
      Sequence(dom).GetString(nm);
      wstring unmangledNm (MANGLE::unmangleStem(nm));
      TYPE_AUML_Signature sign(rng);
      if(sign.get_kind() == Quote(L"EXPL"))
        s+= DoExplSign(unmangledNm, sign,ent_type, indent);
      else
        s+= DoImplSign(unmangledNm, sign,ent_type, indent);
      break;
    }
    case UML_assiciation_type: {
      // Construct a wstring representing an association.
      // This branch is used when presenting the difference between two classes.
      TYPE_AUML_Role rl(rng);
      TYPE_AUML_ValDef val;
      val.set_tp(vdm_BUILD_RoleToAs(rl)); // AS`Type
      val.set_expr(rl.get_initval());     // [AS`Expr]
      val.set_acc(rl.get_acc());          // AUML`Access
      val.set_stat(rl.get_stat());        // bool
      return PPEntity(pos_AUML_Class_inst, dom, val, indent);
    }
  }
  return s;
}

wstring VppInterface::FileHeader()
{
  wstring out =  L"--\n";
          out += L"-- THIS FILE IS AUTOMATICALLY GENERATED!!\n";
          out += L"--\n";
          out += L"-- Generated at ";
          out += TBUTILS::GetDateString( "%d-%b-%y" );
          out += L" by the UML-VDM++ Link\n";
          out += L"--\n";
  return out;
}

// ClassToString
// nm : seq of char
// cl : AUML`Class
// super : set of seq of char
// ass_m : AUML`UniAssoc
// ==> seq of char
wstring VppInterface::ClassToString(const SEQ<Char> & nm,
                                    const TYPE_AUML_Class & cl,
                                    const SET< SEQ<Char> > & super,
                                    const TYPE_AUML_UniAssoc & ass_m)
{
  wstring s;
  wstring nm_str;

  nm.GetString(nm_str);
  s = L"class " + nm_str;
  if(!super.IsEmpty())
    s+= L" is subclass of " + MPP::ListSetOfSeq(super);
  s  += L"\n\n";

  // First L"merge" the associations and the instance variables into the same mapping:
  TYPE_AUML_InstanceVars m(cl.get_inst()); // map Name to ValDef
  m.ImpOverride(Assoc2Instvars(nm, ass_m));
  s += ListEntities(m,             pos_AUML_Class_inst); // map Name to ValDef
//  s += ListEntities(cl.get_time(), pos_AUML_Class_time);
  s += ListEntities(cl.get_val(),  pos_AUML_Class_val);  // map AS`Pattern to ValDef
  s += ListEntities(cl.get_meth(), pos_AUML_Class_meth); // map Name to Signature
  s += ListEntities(cl.get_func(), pos_AUML_Class_func); // map Name to Signature
  s += L"end " + nm_str + L"\n\n";
  //  wstring s(os.str(), os.pcount());
  //  os.rdbuf()->freeze(0);    // Undo freeze in os.str() method.
  return s;
}

// ClassesToString
// file_r : ProjectTypes`FileName
// new_mdl : AUML`Model
// ==> seq of char
wstring VppInterface::ClassesToString(const TYPE_ProjectTypes_FileName & file_r,
                                      const TYPE_AUML_Model & new_mdl)
{
  SET<TYPE_ProjectTypes_ModuleName> nm_s (ToolMediator::ModulesInFile(file_r));
  Set str_nm_s;
  Generic gc;
  for (bool bb = nm_s.First(gc); bb; bb = nm_s.Next(gc))
  {
    // construct a set of names that is compatible with the names used
    // in the UML representation:
    str_nm_s.Insert(SEQ<Char>(PTAUX::ExtractModuleName(gc)));
  }
  return ClassesToString(str_nm_s, new_mdl);
}

// ClassToString
// cl_s : seq of char
// new_mdl : AUML`Model
// +> seq of char
wstring VppInterface::ClassToString(const SEQ<Char> & cl_nm, const TYPE_AUML_Model & new_mdl)
{
  Set cl_s;
  cl_s.Insert(cl_nm);
  return ClassesToString(cl_s, new_mdl);
}

// ClassesToString
// cl_s : set of seq of char
// new_mdl : AUML`Model
// +> seq of char
wstring VppInterface::ClassesToString(const Set & cl_s, const TYPE_AUML_Model & new_mdl)
  // Creates a wstring representation of the classes in set cl_s based on the UML model in new_mdl.
{
  Set n_cl_s (cl_s);
  wstring s;
  s = FileHeader();

  TYPE_AUML_Classes c_m(new_mdl.get_classes());
  TYPE_AUML_Inheritance i_m(new_mdl.get_inheritance());
  TYPE_AUML_Associations a_m(new_mdl.get_associations());

  // cl_s is the set of classes defined in a particular file. If
  // classes have been removed by the mapping, they should be excluded
  // from the set before generating classes for them:
  n_cl_s.ImpIntersect(c_m.Dom());

  Generic gc;
  for (bool bb = n_cl_s.First(gc); bb; bb = n_cl_s.Next(gc))
  {
    SEQ<Char> nm(gc);
    s += ClassToString(nm, c_m[nm], i_m[nm], a_m[nm]) + L"\n\n";
  }
  return s;
}

// NeedFileUpdate
// file_r : ProjectTypes`FileName
// new_mdl : AUML`Model
// old_mdl : AUML`Model
// +> bool
bool VppInterface::NeedFileUpdate(const TYPE_ProjectTypes_FileName & file_r,
                                  const TYPE_AUML_Model & new_mdl,
                                  const TYPE_AUML_Model & old_mdl)
  // Returns true if the classes of file_r has changed from new_mdl to
  // old_mdl
{
  SET<TYPE_ProjectTypes_ModuleName> nm_s (ToolMediator::ModulesInFile(file_r));
  bool exists = false;
  Generic gc;
  for (bool bb = nm_s.First(gc); bb && !exists; bb = nm_s.Next(gc)) {
    SEQ<Char> cl_nm (PTAUX::ExtractModuleName(gc));
    exists = !vdm_MERGE_IsEqual(cl_nm, new_mdl, old_mdl).GetValue();
  }
  return exists;
}

// NeedsUpdate
// file_s : set of ProjectTypes`FileName
// new_mdl : AUML`Model
// old_mdl : AUML`Model
// +> set of ProjectTypes`FileName
Set VppInterface::NeedsUpdate(const SET<TYPE_ProjectTypes_FileName> & file_s,
                              const TYPE_AUML_Model & new_mdl,
                              const TYPE_AUML_Model & old_mdl)
{
  SET<TYPE_ProjectTypes_FileName> file_s_q (file_s);
  SET<TYPE_ProjectTypes_FileName> update;
  Generic gc;
  for (bool bb = file_s_q.First(gc); bb; bb = file_s_q.Next(gc)) {
    if(NeedFileUpdate(gc, new_mdl, old_mdl))
      update.Insert(gc);
  }
  return update;
}

// The following removes any obsolete files from the project.
// A file is obsolete if it was in the project before merging models,
// but after the merge all of the classes that were in that file
// have been deleted from the model

// RemoveObsoleteFiles
// obsolete_classes_ : set of ProjectTypes`ModuleName
void VppInterface::RemoveObsoleteFiles(const Set & obsolete_classes_)
{
  Set obsolete_classes (obsolete_classes_);
  Set files_of_obsolete_classes;
  Set obsolete_modules;
  Generic gc;

  // Collect files corresponding to obsolete classes
  for(bool bb = obsolete_classes.First(gc); bb; bb = obsolete_classes.Next(gc))
  {
    TYPE_ProjectTypes_ModuleName modnm;
    modnm.set_nm(gc);

    // Accumulate class names wrapped up as ModuleNames
    obsolete_modules.Insert(modnm);

    // Add files for this module
    files_of_obsolete_classes.ImpUnion(ToolMediator::FileOfModule(modnm));
  }

  // Go through file with an obsolete class, and check whether
  // file contains any non-obsolete classes
  SET<TYPE_ProjectTypes_FileName> obsolete_files;
  for(bool cc = files_of_obsolete_classes.First(gc); cc; cc = files_of_obsolete_classes.Next(gc))
  {
    TYPE_ProjectTypes_FileName flnm(gc);

    // If modules for this file are all obsolete, add to set
    if (ToolMediator::ModulesInFile(flnm).SubSet(obsolete_modules))
      obsolete_files.Insert(flnm);
  }

  // Check that some files may be deleted, as calls to
  // ToolMediator::RemoveFiles with an empty set cause the
  // project modified flag to be set
  if(!obsolete_files.IsEmpty())
  {
    for(bool dd = obsolete_files.First(gc); dd; dd = obsolete_files.Next(gc))
    {
      wstring fnmstr (PTAUX::ExtractFileName(gc));
      wstring mesg = L"  Removing Following File From Project: " + fnmstr;
      vdm_log << mesg << endl << flush;
    }

    // Back up old file
    // Following is too simplistic, but kept for testing purposes
    // e.g. doesn't work if current project hasn't been saved etc
    // ***** Should be replaced before final release
    Generic gprojfile (ToolMediator::GetProjectName());
    if(!gprojfile.IsNil())
    {
        wstring projfilestr (PTAUX::ExtractFileName(gprojfile));
        wstring oldprojfilestr = projfilestr + L"_old.prj";
        vdm_log << L"Backing up project to " + oldprojfilestr
                << endl << flush;
        TBUTILS::file_copy(oldprojfilestr, projfilestr);
    }
    ToolMediator::RemoveFiles(obsolete_files);
  }
}

// LockedFiles
// merged_mdl : AUML`Model
// old_mdl : AUML`Model
// -> bool
bool VppInterface::LockedFiles(const TYPE_AUML_Model & merged_mdl, const TYPE_AUML_Model & old_mdl)
  // Returns true if any of the files that should be updated are locked.
  //
  // This function is called from UMLMapper::Conflicts(). Should be
  // moved into its own function after the release!!!

{
  // Check for locked files:
  Set locked;
  if( (locked = TBUTILS::CheckForLockedFiles(NeedsUpdate(ToolMediator::ParsedFiles(),
                                                merged_mdl,
                                                old_mdl)) ).Card() ) {
    // Unable to lock one or more files:
    vdm_log << L"Error: The following files are locked and can not be updated:" << endl;
    Generic gc;
    for (bool bb = locked.First(gc); bb; bb = locked.Next(gc))
      vdm_log << L"  " << PTAUX::ExtractFileName (gc).c_str() << endl;
    vdm_log << L"Check if they are being used by another application." << endl << flush;
    return true;
  }
  return false;
}

// MapModelToFiles
// merged_mdl : AUML`Model
// old_mdl : AUML`Model
// to_be_parsed : seq of ProjectTypes`FileName (out)
// charset : seq of char
// newfiletype : seq of char
// rtftemplate : seq of chsr
// ==> bool
bool VppInterface::MapModelToFiles(const TYPE_AUML_Model & merged_mdl,
                                   const TYPE_AUML_Model & old_mdl,
                                   SEQ<TYPE_ProjectTypes_FileName> & to_be_parsed,
                                   const wstring & charset,
                                   const wstring & newfiletype,
                                   const wstring & rtftemplate)
  // Based on the original UML model of the VDM++ spec, and the model
  // merged from the VDM++ and Rose model this function updates or
  // creates the physical files for the VDM++ specification.
{
  // The set of files that must be updated:
  Set to_be_updated = NeedsUpdate(ToolMediator::ParsedFiles(), merged_mdl, old_mdl);

  bool succ = true;

  // Check that no files was modified without being parsed:
  Set modified_s (ToolMediator::CheckForModifiedFiles());
  modified_s.ImpIntersect(to_be_updated);
  if(!modified_s.IsEmpty()) {
    vdm_log << L"  Error: Unable to update the specification files. The following files "
            << L"have been modified but not parsed:" << endl;
    Generic gc;
    for (bool bb = modified_s.First(gc); bb; bb = modified_s.Next(gc)) {
      vdm_log << L"    " << PTAUX::ExtractFileName (gc).c_str() << endl;
    }
//    vdm_log << L"  Parse these files and re-invoke the Rose-VDM++ Link." << endl << flush;
    vdm_log << L"  Parse these files and re-invoke the UML-VDM++ Link." << endl << flush;
    return false;
  }

  // Remove files which are no longer relevant to the model, from the project
  Set obsolete_classes (old_mdl.get_classes().Dom().ImpDiff(merged_mdl.get_classes().Dom()));
  to_be_updated.ImpDiff(obsolete_classes);
  RemoveObsoleteFiles(obsolete_classes);

  // First update all existing files:
  SET<TYPE_ProjectTypes_FileName> file_s (ToolMediator::ParsedFiles());
  Generic gc;
  for (bool bb = file_s.First(gc); bb; bb = file_s.Next(gc)) {
    if(NeedFileUpdate(gc, merged_mdl, old_mdl)) {
      ContextInfo temp_context_info;
      SpecFile sf(gc, GetCI());  // SpecFile for file gc

      if( ! sf.init() ) {
        vdm_log << L"  Couldn't open file '" << sf.get_short_name() << L"'" << endl;
        succ = false;
      }

      vdm_log << L"  Updating file \"" << sf.get_fname() << L"\" ... " <<flush;

      sf.set_merged_classes(ClassesToString(sf.get_filename(), merged_mdl));

      // Need to create a _Sequence_ of asts, since GetAstsOfFileName
      // returns a set of asts. The reason to use a sequence of asts is
      // that this is what is used by the SpecFile and the parser as
      // well:
      Set ast_s (ToolMediator::GetAstsOfFileName(gc));
      Sequence ast_l;
      Generic g;
      for (bool cc = ast_s.First(g); cc; cc = ast_s.Next(g) )
        ast_l.ImpAppend(g);

      wstring err;
      if(!sf.file_update(err, ast_l)){
        vdm_log << L"  Unable to update file \""
                << sf.get_short_name()
                << L"\". " << err << endl << flush;
        succ = false;
      }
      else {
        vdm_log << L"done." << endl << flush;
        to_be_parsed.ImpAppend(gc);
      }
    }
    else {
      vdm_log << L"  No need to update file: \""
              << PTAUX::ExtractFileName(gc)
              << L"\"" << endl << flush;
    }
  }
  // Then create new files for all new classes:
  TYPE_AUML_Classes n (merged_mdl.get_classes());
  TYPE_AUML_Classes o (old_mdl.get_classes());
  Set new_s (n.Dom());
  new_s.ImpDiff(o.Dom()); // The set of new classes.

  wstring path (TBUTILS::tb_getcwd()); // Where to generate new files

  SEQ<TYPE_ProjectTypes_FileName> new_files;

  if(!new_s.IsEmpty())
    vdm_log << L"  Working directory is: " << path << endl << flush;

  for (bool dd = new_s.First(gc); dd; dd = new_s.Next(gc)) {
    wstring nm_str;
    SEQ<Char>(gc).GetString(nm_str);
    ContextInfo temp_context_info;

    // NewClass.rtf
    wstring default_file (rtftemplate);

    SpecFile sf(path, nm_str + newfiletype, default_file, charset, temp_context_info);
    if( ! sf.init() ) {
      vdm_log << L"  Warning: Couldn't create file '"
              << sf.get_short_name()
              << L"'. Check that this file does not exist already." << endl;
    }
    else {
      vdm_log << L"  Creating new file \"" << sf.get_short_name()
              << L"\" ... " << flush;

      sf.set_merged_classes(ClassToString(gc, merged_mdl));

      // TODO: if Japanese ENV & RTF format
      //       to set to SHIFT-JIS encoding
      wstring err;
      if(!sf.file_create(err)) {
        vdm_log << L" Error occurred for file \""
                << sf.get_short_name() << L"\". " << err << endl << flush;
	succ = false;
      }
      else {
        vdm_log << L"done." << endl << flush;
        new_files.ImpAppend(PTAUX::mk_FileName(sf.get_fname()));
      }
    }
  }

  // Add all new files. Only do this if there are files to add, since
  // otherwise the L"project modified" flag is set
  if (!new_files.IsEmpty())
  {
    // They are already added!!!    ToolMediator::AddFiles((Generic) new_files.Elems());
    to_be_parsed.ImpConc(new_files); // The sequence of files to be parsed
  }
  return succ;
}

