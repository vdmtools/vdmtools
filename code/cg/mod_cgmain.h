/***
*  * WHAT
*  *    Implementation of mod_cgmain.vdm 1.17
*  * FILE
*  *    $Source: /home/vdmtools/cvsroot/toolbox/code/cg/mod_cgmain.h,v $
*  * VERSION
*  *    $Revision: 1.11 $
*  * DATE
*  *    $Date: 2001/11/09 08:22:16 $
*  * STATUS
*  *    Under development
*  * REFERENCES
*  *    
*  * PROJECT
*  *    IDERS
*  * AUTHOR
*  *    Henrik Voss + $Author: paulm $
*  *
*  * COPYRIGHT
*  *    (C) Kyushu University
***/

#ifndef __mod_cg_h__
#define __mod_cg_h__

private:
  bool skeleton_option;
  bool preandpost_option;
  bool onlytypes_option;
  bool smalltypes_option;
  Generic package_name;
  bool conc_option;
  bool testpreandpost_option;
  bool longs_option;
  SET<TYPE_AS_Name> interfaces;
  bool wchar_option;
  bool j2v_option;
  bool generics_option;
  bool java_rename_option;

  bool verbose_mode;
  Set imports_names;
  Set entries_names;
  Set throws_names;

public:
  void InitState_CGMAIN();

  bool cg_OptionGenValues();
  bool cg_OptionGenFctOps();
  bool cg_OptionGenTpInv();
  bool cg_OptionGenInitFct();

public:
  void set_skeleton_option(bool); 
  bool get_skeleton_option();
  void set_conc_option(bool); 
  bool get_conc_option();
//void set_jdktypes_option(bool);
  bool get_jdktypes_option() const;
  void set_smalltypes_option(bool); 
  bool get_smalltypes_option() const;
  void set_package_option(const Generic &); 
  Generic get_package_option();
  void set_preandpost_option(bool); 
  bool get_preandpost_option() const;
  void set_testpreandpost_option(bool); 
  bool get_testpreandpost_option();
  void set_onlytypes_option(bool); 
  bool get_onlytypes_option();
  bool get_longs_option();
  void set_longs_option(bool); 
  void set_interfaces(const SET<TYPE_AS_Name> &);
  SET<TYPE_AS_Name> get_interfaces();
  bool isInterface(const TYPE_AS_Name &);
  bool get_wchar() const;
  void set_wchar(bool);
  Set get_imports_names() const;
  void set_imports_names(const Set &);
  Set get_entries_names() const;
  void set_entries_names(const Set &);
  bool isEntries(const TYPE_AS_Name &);
  Set get_throws_names() const;
  void set_throws_names(const Set &);
  bool isThrows(const TYPE_AS_Name &);
  void set_j2v_option(bool);
  bool get_j2v_option() const;
  void set_generics_option(bool);
  bool get_generics_option() const;
  void set_java_rename_option(bool);
  bool get_java_rename_option() const;

  void set_verbose_mode(bool);
  bool get_verbose_mode() const;

#ifdef VDMSL
private:
  SET<TYPE_CPP_File> cg_CG(const TYPE_AS_Document &, const Map &);
#endif // VDMSL

#endif // __mod_cg_h__
