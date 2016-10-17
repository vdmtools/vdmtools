/***
*  * WHAT
*  *  This file defines the interface from the intermediate UML 
*  *  representation towards the VDM++ files.
*  *  
*  *    
*  **  * FILE
*  *    $Source: /home/vdmtools/cvsroot/toolbox/code/uml/code/vppinterface.h,v $
*  * VERSION
*  *    $Revision: 1.11 $
*  * DATE
*  *    $Date: 2006/04/26 05:50:09 $
*  * FORMAT
*  *    $State: Exp $
*  * PROJECT
*  *    VDM-SL Toolbox.
*  * AUTHOR
*  *    $Author: vdmtools $
*  * COPYRIGHT
*  *    (C) Kyushu University
***/

#ifndef __vppinterface_h__
#define __vppinterface_h__

#include "metaiv.h"
#include "ProjectTypes.h"
#include "AUML.h"

#define UML_assiciation_type pos_AUML_Class_func + 1
#define UML_inheritance_type UML_assiciation_type + 1

class VppInterface
{
public:
  //Set CheckForLockedFiles();
  static bool LockedFiles(const TYPE_AUML_Model & merged_mdl, 
                          const TYPE_AUML_Model & old_mdl);

  // This function makes an automatic update of the VDM++ files.
  static bool MapModelToFiles(const TYPE_AUML_Model &, 
                            const TYPE_AUML_Model &, 
                            SEQ<TYPE_ProjectTypes_FileName> &,
                            const wstring & charset,
                            const wstring & filetype,
                            const wstring & rtftemplate);

  // Returns a wstring naming the type of entity, i.e. "instance
  // variablesL", "values", "time variables", etc.
  static wstring ent_type2wstring(int);

  // Returns a wstring representing an entity, i.e. an instance variable,
  // a value, a time variable, etc.  Uses MiniPP.
  static wstring PPEntity(int, const Generic &, const Generic &, const wstring &);

  // Returns a wstring representing the body of an entity, 
  // i.e. an instance variable, a value, a time variable, etc.  
  // Uses MiniPP.
  static wstring PPBody(int, const Generic &, const Generic &, const wstring &);

  // Returns a wstring representing the pre and post conditions of
  // a function or operation. Uses MiniPP.
  static wstring PPConditions(int type,
                              const Generic & dom,
                              const Generic & rng,
                              const wstring & indent);

private:
  static TYPE_AUML_InstanceVars Assoc2Instvars(const SEQ<Char> & nm, const Map &ass_m);
  static wstring ListEntities(const Map & m, int type);
  static wstring GetTypeString(int type, int plur);
  static wstring PPAccess(const Quote & acc);
  static wstring DoImplSign(const wstring & nm,
                            const TYPE_AUML_Signature & sign,
                            int ent_type,
                            const wstring & indent);
  static wstring DoExplSign(const wstring & nm,
                            const TYPE_AUML_Signature & sign,
                            int ent_type,
                            const wstring & indent);
  static wstring FileHeader();
  static wstring ClassToString(const SEQ<Char> & nm,
                               const TYPE_AUML_Class & cl,
                               const SET< SEQ<Char> > & super,
                               const TYPE_AUML_UniAssoc & ass_m);
  static wstring ClassesToString(const TYPE_ProjectTypes_FileName & file_r,
                                 const TYPE_AUML_Model & new_mdl);
  static wstring ClassToString(const SEQ<Char> & cl_nm,
                               const TYPE_AUML_Model & new_mdl);
  static wstring ClassesToString(const Set & cl_s,
                                 const TYPE_AUML_Model & new_mdl);
  static bool NeedFileUpdate(const TYPE_ProjectTypes_FileName & file_r,
                             const TYPE_AUML_Model & new_mdl,
                             const TYPE_AUML_Model & old_mdl);
  static Set NeedsUpdate(const SET<TYPE_ProjectTypes_FileName> & file_s,
                         const TYPE_AUML_Model & new_mdl,
                         const TYPE_AUML_Model & old_mdl);
  static void RemoveObsoleteFiles(const Set & obsolete_classes);
};

#endif //__vppinterface_h__
