/***
*  * WHAT
*  *    mod_cgmain.cc: Implementation of mod_cgmain.vdm 1.26
*  * FILE
*  *    $Source: /home/vdmtools/cvsroot/toolbox/code/cg/mod_cgmain.cc,v $
*  * VERSION
*  *    $Revision: 1.30 $
*  * DATE
*  *    $Date: 2006/03/17 08:29:12 $
*  * STATUS
*  *    Under development
*  * REFERENCES
*  *    
*  * PROJECT
*  *    IDERES/AFRODITE
*  * AUTHOR
*  *    Henrik Voss + $Author: vdmtools $
*  *    
*  * COPYRIGHT
*  *    (C) Kyushu University
***/

#include "vdmcg.h"
#include "UTIL.h"
#include "BC.h"
#include "statsem.h"
#include "codegen_tools.h"
#include "contextinfo.h"
#include "astaux.h"
 
Bool vdm_CGMAIN_get_uwchar()
{
  return GetVDMCG().get_wchar();
}

// FUNCTIONS FOR HANDLING OPTIONS HANDED TO THE CG

bool vdmcg::cg_OptionGenValues()  {return true;}
bool vdmcg::cg_OptionGenFctOps()  {return true;}
bool vdmcg::cg_OptionGenTpInv()   {return true;}
bool vdmcg::cg_OptionGenInitFct() {return true;}

void vdmcg::InitState_CGMAIN()
{
  this->skeleton_option = false;
  this->preandpost_option = false;
  this->onlytypes_option = false;
  this->smalltypes_option = false;
  this->package_name = Nil();
  this->conc_option = false;
  this->testpreandpost_option = false;
  this->longs_option = false;
  this->interfaces.Clear();
  this->wchar_option = true;
  this->imports_names.Clear();
  this->entries_names.Clear();
  this->throws_names.Clear();
// 20081114
//  this->j2v_option = true;
  this->j2v_option = false;
  this->generics_option = false;
  this->java_rename_option = true;

  this->verbose_mode = false;
}

// CG functions and operations

#ifdef VDMPP
void vdmcg::set_skeleton_option(bool s)
{
  this->skeleton_option = s;
}

bool vdmcg::get_skeleton_option()
{
  return this->skeleton_option;
}

void vdmcg::set_onlytypes_option(bool s)
{
  this->onlytypes_option = s;
}

bool vdmcg::get_onlytypes_option()
{
  return this->onlytypes_option;
}

bool vdmcg::get_jdktypes_option() const
{
  return (vdm_CPP_isJAVA()).GetValue();
}

void vdmcg::set_preandpost_option(bool s)
{
  this->preandpost_option = s;
}

bool vdmcg::get_preandpost_option() const
{
  return this->preandpost_option;
}
#endif // VDMPP

void vdmcg::set_testpreandpost_option(bool s)
{
  this->testpreandpost_option = s;
}

bool vdmcg::get_testpreandpost_option()
{
  return this->testpreandpost_option;
}

#ifdef VDMPP
void vdmcg::set_conc_option(bool s)
{
  this->conc_option = s;
}

bool vdmcg::get_conc_option()
{
  return this->conc_option;
}

void vdmcg::set_smalltypes_option(bool s)
{
  if (s) {set_skeleton_option(s);};
  this->smalltypes_option = s;
}

bool vdmcg::get_smalltypes_option() const
{
  return this->smalltypes_option;
}

void vdmcg::set_package_option(const Generic & ch_l)
{
  if (ch_l.IsNil())
    this->package_name = Nil();
  else
    //this->package_name = GenPackageName((Generic)UTIL::split(ch_l, Char('.')));
    this->package_name = GenPackageName(UTIL::split(ch_l, Char('.')));
}

Generic vdmcg::get_package_option()
{
  return this->package_name;
}

void vdmcg::set_longs_option(bool s)
{
  this->longs_option = s;
}

bool vdmcg::get_longs_option()
{
  return this->longs_option;
}

void vdmcg::set_interfaces(const SET<TYPE_AS_Name> & ifs)
{
  this->interfaces = ifs;
}

SET<TYPE_AS_Name> vdmcg::get_interfaces()
{
  return this->interfaces;
}

bool vdmcg::isInterface(const TYPE_AS_Name & clnm)
{
  return this->interfaces.InSet(clnm);
}
#endif // VDMPP

bool vdmcg::get_wchar() const
{
  return this->wchar_option;
}

void vdmcg::set_wchar(bool pWchar)
{
  this->wchar_option = pWchar;
}

#ifdef VDMPP
Set vdmcg::get_imports_names() const
{
  return this->imports_names;
}

void vdmcg::set_imports_names(const Set & nm_s)
{
  this->imports_names = nm_s;
}

Set vdmcg::get_entries_names() const
{
  return this->entries_names;
}

void vdmcg::set_entries_names(const Set & nm_s)
{
  this->entries_names = nm_s;
}

bool vdmcg::isEntries(const TYPE_AS_Name & nm)
{
  return this->entries_names.InSet(nm);
}

Set vdmcg::get_throws_names() const
{
  return this->throws_names;
}

void vdmcg::set_throws_names(const Set & nm_s)
{
  this->throws_names = nm_s;
}

bool vdmcg::isThrows(const TYPE_AS_Name & nm)
{
  return this->throws_names.InSet(nm);
}

bool vdmcg::get_j2v_option() const
{
  return this->j2v_option;
}

void vdmcg::set_j2v_option(bool b)
{
  this->j2v_option = b;
}

bool vdmcg::get_generics_option() const
{
  return this->generics_option;
}

void vdmcg::set_generics_option(bool b)
{
  this->generics_option = b;
}

bool vdmcg::get_java_rename_option() const
{
  return this->java_rename_option;
}

void vdmcg::set_java_rename_option(bool b)
{
  this->java_rename_option = b;
}
#endif // VDMPP

bool vdmcg::get_verbose_mode() const
{
  return this->verbose_mode;
}

void vdmcg::set_verbose_mode(bool b)
{
  this->verbose_mode = b;
}

#ifdef VDMSL
// cg_CG
// doc_l : AS`Document
// tc_state : set of AS`Name
// ==> CPP`Files
SET<TYPE_CPP_File> vdmcg::cg_CG(const TYPE_AS_Document & doc_l, const Map & tc_state,
                                const Bool & testcond)
{
  set_testpreandpost_option(testcond);

  TYPE_AS_Document mod_l;  // seq of (AS`Module | AS`DLModule)
  if ((doc_l.Length() == 1) && doc_l[1].Is(TAG_TYPE_AS_Definitions))
  {
    TYPE_AS_Module mod;
    mod.Init(ASTAUX::MkNameFromId(ASTAUX::MkId(L"DefaultMod"), NilContextId),
             TYPE_AS_Interface().Init(type_7AS_NameCUM(), Nil(), NilContextId),
             doc_l[1],
             NilContextId);

    mod_l.ImpAppend(mod);
  }
  else {
    mod_l = (Generic)doc_l;
  }

  SET<TYPE_CPP_File> file_s;
  size_t len_mod_l = mod_l.Length();
  for (size_t i = 1; i <= len_mod_l; i++)
  {
    const Record & md (mod_l[i]);
    switch (md.GetTag()) {
      case TAG_TYPE_AS_Module: {
        GetStatSem().SetTCState(tc_state[md.GetRecord(pos_AS_Module_nm)]);
        file_s.ImpUnion(GenModule(md));
        break;
      }
      case TAG_TYPE_AS_DLModule: {
        GetStatSem().GenErr(md.GetRecord(pos_AS_DLModule_nm), StatSem::ERR, 360, Sequence());
        break;
      }
    }
  }
  return file_s;
}
#endif // VDMSL

