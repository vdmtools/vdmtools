/***
*  * WHAT
*  *    The common interface towards Rational Rose 4.0 and Rose 98
*  *    Methods specific for either Rose98 or Rose4.0 should be moved
*  *    from this base class to either class Rose40 or class Rose98
*  *    
*  *    Contains functionality to update the class repository in RR
*  *    and to create diagrams.
*  *    Furthermore functionality to create an abstract uml representation
*  *    based on the class repository is defined here.
*  *    
*  **  * FILE
*  *    $Source: /home/vdmtools/cvsroot/toolbox/code/uml/code/roseinterface.h,v $
*  * VERSION
*  *    $Revision: 1.21 $
*  * DATE
*  *    $Date: 2006/04/19 08:11:25 $
*  * FORMAT
*  *    $State: Exp $
*  * PROJECT
*  *    VDM-SL Toolbox.
*  * AUTHOR
*  *    $Author: vdmtools $
*  * COPYRIGHT
*  *    (C) Kyushu University
***/


#ifndef __roseinterface_h__
#define __roseinterface_h__

#include "metaiv.h"
#include "UmlToolInterface.h"
#include "AUML.h"

#ifdef _USE_ROSE

#define NOMINMAX
#include "stdafx.h"
#include "rationalrose.h"  // Rose 98 interface definitions
#include "rose_redefines.h"

class RoseInterface : public CObject, public UmlToolInterface
{

public:
  RoseInterface(); 
  virtual ~RoseInterface();
  
public:
  virtual bool Init();  
  // Establishes the connection to Rose
  // Returns true if it was possible to establish the connection.
  
  virtual void CloseDown();
  // Releases the handle to IRoseApplication

  virtual void GiveFocus(); 
  virtual void SelectModel(const wstring & name);  // Selects the model file used in Rose
  virtual const wstring GetModel();  
  virtual const wstring DoBackup(const wstring &);

  // Retrieves and sets the current UML model in Rational Rose. 
  // These functions modifies and reads the class repository in RR.
  virtual TYPE_AUML_Model GetCurModel(const Bool & ReportErr,
                                      const TYPE_AUML_Model &);
  virtual void SetCurModel(const TYPE_AUML_Model &, 
                           const TYPE_AUML_Model &);
  
  virtual void BuildClassDiagram(const wstring & name,
                                 const TYPE_AUML_Model & curmdl);

  const wstring ToolName() { return L"Rose"; };

  const wstring GetFileExtension() { return L".mdl"; };

protected:
  //
  // Auxilary functions used to implement the functions above.
  //
  void ParseAttributes(IRoseClass c,
                       CString c_nm,
                       TYPE_AUML_InstanceVars & iv_m,
                       TYPE_AUML_ValuesDef & val_m,
                       bool ReportErr);
  void UpdateInternals(IRoseClass c,
                       const TYPE_AUML_Class & cl,
                       const TYPE_AUML_Class & old_cl);
  void UpdateInternals(IRoseClass c, const TYPE_AUML_Class & cl);
  void RemoveObsoleteAttributes(IRoseClass, const Set &);
  void RemoveObsoleteMethods(IRoseClass c, const SET<TYPE_AUML_RoseId> & s);
  void DeleteAllAttributes(IRoseClass);
  void UpdateInstVars(IRoseClass,
                      const TYPE_AUML_InstanceVars & inst_m,
                      const TYPE_AUML_InstanceVars & old_m);
  void UpdateValues(IRoseClass,
                    const TYPE_AUML_ValuesDef & val_m,
                    const TYPE_AUML_ValuesDef & old_m);
  void UpdateFunctions(IRoseClass,
                       const TYPE_AUML_CommonSign & fn_m,
                       const TYPE_AUML_CommonSign & old_m);
  void UpdateMethods(IRoseClass,
                     const TYPE_AUML_CommonSign & meth_m,
                     const TYPE_AUML_CommonSign & old_m);

  void DeleteAllOperations(IRoseClass);
  void AddOperations(IRoseClass,
                     const TYPE_AUML_CommonSign &,
                     const TYPE_AUML_CommonSign &,
                     wchar_t *);
  void ParseOperations(IRoseClass,
                       CString c_nm,
                       TYPE_AUML_CommonSign & o_m,
                       TYPE_AUML_CommonSign & f_m,
                       const Bool & ReportErr);

  // Auxilary conversion functions.
  Quote string2card(IRoseRole, IRoseClass);
  CString card2wstring(Quote q);

  SET<TYPE_AUML_Name> MkNameSet(IRoseCollection l);
  LPDISPATCH FindByName(CString n, IRoseCollection l);
  
  // Primarily for debugging purposes
  void ListNames(IRoseCollection l);


  void UpdateFilename(IRoseClass c, wstring classname);
  
  // Functions for updating the classes in the Rose repository
  void CreateClasses(const TYPE_AUML_Classes & class_m,
                     const TYPE_AUML_Classes & orig_m);
  void RemoveObsoleteClasses(const SET<TYPE_AUML_Name> & cs);
  // Updates the internals of a class

  void SetMethodParameters(IRoseOperation op,
                           const SEQ<TYPE_AUML_PatType> & param_l);


  // Functions for updating the inheritance and association relations
  void UpdateInheritance(const TYPE_AUML_Inheritance & inh_m,
                         const TYPE_AUML_Inheritance & cur_inh_m);
  void UpdateAssociations(const TYPE_AUML_Associations & ass_m,
                          const TYPE_AUML_Associations & cur_ass_m);

  void AddAssociations(const TYPE_AUML_Associations &,
                       IRoseCollection);
  void ModifyAssociations(const TYPE_AUML_Associations &,
                          const TYPE_AUML_Associations &,
                          IRoseCollection);
  void DeleteAssociations(const TYPE_AUML_Associations &,
                          IRoseCollection);

  void AddUniAssociations(const TYPE_AUML_UniAssoc & add,
                          IRoseClass c);
  void ModifyUniAssociations(const TYPE_AUML_UniAssoc & modified,
                             const TYPE_AUML_UniAssoc & orig,
                             IRoseClass c);
  void DeleteUniAssociations(const TYPE_AUML_UniAssoc & del,
                             IRoseClass c);

  void AddInheritance(const SET<TYPE_AUML_Name> & add,
                      IRoseClass c);
  void DeleteInheritance(const SET<TYPE_AUML_Name> & del,
                         IRoseClass c);
  int SelectForDeletion(IRoseCollection l,
                        const SET<TYPE_AUML_Name> nm_s, 
                        LPDISPATCH *tbd,
                        int mode,
                        IRoseClass cc);

  // Functions for retrieving the current model in the Rose repository
  void ParseInstanceVar(const CString & c_nm,
                        TYPE_AUML_InstanceVars &iv_m,
                        const CString & nm,
                        const CString & type,
                        const CString & initval,
                        const CString & acc,
                        const BOOL & stat,
                        const CString & id,
                        const Bool & ReportErr);

  Quote MapRoseAccessToAUML(const CString & acc);
  CString MapAUMLAccessToRose(const Quote & acc);

  void ParseValue(CString c_nm,
                  TYPE_AUML_ValuesDef &val_m,
                  CString nm,
                  CString type,
                  CString initval,
                  CString acc,
                  BOOL stat,
                  CString id,
                  const Bool & ReportErr);

  void ParseSignature(CString, int,
                      TYPE_AUML_CommonSign &, 
                      CString, CString, 
                      IRoseCollection, 
                      CString, CString,
                      CString, CString,
                      const Bool &);
  
  CString Operation2Cstr(CString nm,
                         CString type, 
                         IRoseCollection args);

  TYPE_AUML_Classes GenClassMap(const Bool &,
                                const TYPE_AUML_Classes &);
  TYPE_AUML_Inheritance GenInhMap();
  TYPE_AUML_Associations GenAssMap(const TYPE_AUML_Associations &);

  void RemoveKeys(IRoseRole role);
  TYPE_AUML_UniAssoc MkAssocMap(IRoseCollection l,
                                IRoseClass c,
                                const TYPE_AUML_Associations &);
  TYPE_AUML_Role MkRole(IRoseRole,
                        IRoseClass,
                        const TYPE_AUML_Associations &);
  Generic MkQualifier(IRoseCollection att, IRoseClass, IRoseRole);

  LPDISPATCH CreateDiagram(CString nm, IRoseCategory cat);
  void BuildView(const TYPE_AUML_Name & nm,
                 const SET<TYPE_AUML_Name> & cl_s,
                 const IRoseCategory & cat);

  void DisplayWarning(CString c_nm, CString str);

  IRoseApplication rose;
  IRoseModel theModel;
  IRoseCategory DiagramsCat, ClassesCat, RootCat;
  IRoseModule VDMComponent; 

private:
  bool Initialized;
};

#else

class RoseInterface : public UmlToolInterface
{

public:
  RoseInterface() {};
  virtual ~RoseInterface() {};

public:
  // Establishes the connection to the CASE Tool
  // Returns true if it was possible to establish the connection.
  bool Init() { return false; };

  // Closes down the connection to the CASE Tool.
  void CloseDown() {};

  // Gives the CASE Tool application window the focus.
  void GiveFocus() {};

  // Gives the name of the file current used by the CASE Tool
  const wstring GetModel() { return this->model_dummy; };

  // Selects the model file used by the CASE Tool.
  void SelectModel(const wstring & name) { this->model_dummy = name; };
  
  // Creates a backup file of the current model. 
  const wstring DoBackup(const wstring & cwd) { return L""; };
  
  // Retrieves the current UML model defined in the CASE Tool.
  TYPE_AUML_Model GetCurModel(const Bool & ReportErr, 
                              const TYPE_AUML_Model & vppmodel)
  { return vppmodel; };  // return Cur_VPP_Model as Cur_UML_Model

  // Set the current UML model of the CASE Tool to be equal to curmdl
  void SetCurModel(const TYPE_AUML_Model & newmdl,
                   const TYPE_AUML_Model & umlmodel) {};

  // Creates a class diagram with name name for the model in curmdl.
  void BuildClassDiagram(const wstring & name,
                         const TYPE_AUML_Model & curmdl) {};

  const wstring ToolName() { return L"NullInterface"; };

  const wstring GetFileExtension() { return L".dummy"; };

private:
  wstring model_dummy;
};
#endif // _USE_ROSE

#endif // __roseinterface_h__

