/***
*  * WHAT
*  *  Backend for the UML mapper
*  *  
*  *    
*  **  * FILE
*  *    $Source: /home/vdmtools/cvsroot/toolbox/code/uml/code/uml-be.h,v $
*  * VERSION
*  *    $Revision: 1.20 $
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

#ifndef __uml_be_h__
#define __uml_be_h__

#include "metaiv.h"
#include "AS.h"
#include "AUML.h"
#include "MERGE.h"
#include "UmlToolInterface.h"

enum {
  INTERFACE_NULL,
  INTERFACE_ROSE98,
  INTERFACE_XMI11_UML14_ASTAH,
  INTERFACE_XMI11_UML14_EA,
  INTERFACE_UNKNOWN
};

enum {
  FILETYPE_VPP,
  FILETYPE_RTF,
  FILETYPE_TEX,
  FILETYPE_UNKNOWN
};
  
class UMLMapper
{
public:
  UMLMapper();
  ~UMLMapper();

public:

  void SetRTFTemplate(const wstring & t) { this->rtftemplate = t; };
  wstring GetRTFTemplate() const { return this->rtftemplate; };

  bool SetToolInterface(int);

  bool InitConnection();

  // This method must be passed the ASTs representing the currently
  // parsed VPP classes. The method sets the internal state of the 
  // UMLMapper class. That is, the abstract UML models corresponding
  // to the VPP model and the model contained in the CASE tool are calculated.
  // The return value is true if it was possible to esablish a connection
  // to the CASE tool
  Bool SetMapperState(const TYPE_AS_Document & doc_l, const Bool & ReportErr);

  void ResetMapper();

  // Performs the UML mapping.
  // Returns a log of the mapping to be displayed to the user. 
  SEQ<Char> MapUML(const TYPE_MERGE_GuiSettings &);

  // Sets and gets the name of the model file to work with.
  void SetModelNames(const Sequence & pname, const Sequence & fname);
  SEQ<Char> GetModelName();

  // Returns a map from class names to (State * State), where State 
  // is one of: <MODIFIED> | <DELETED> | <ADDED> | <NONE> | <UNKNOWN>
  TYPE_MERGE_StateMaps GetClassStatus();

  // Returns a map from class names to (bool * bool), where the tuple
  // states the default settings for merging the given class.
  // I.e. {L"A" |-> (true, false)} means that the VDM++ version of A is
  // to be included in the merge whereas the UML version of A is not.
  // The map passed to this function must be the map returned by 
  // the function GetClassStatus()
  TYPE_MERGE_DefaultMap GetDefaultSettings(const TYPE_MERGE_StateMaps &);

  // Returns the difference between the UML and VDM++ version of all
  // classes in the set.
  SEQ<Char> Diff(const SET<TYPE_AUML_Name> &);

  // No longer needed. Should be removed completely after the release.!!
  SEQ<Char> GetConflictMsg();

  // Returns true if the VDM++ and UML has conflicting entities.
  Bool Conflicts(const TYPE_MERGE_GuiSettings &);

  // Sets and gets the name of the charcter code set.
  void SetCharset(const wstring & cs ) { this->charset = cs; };
  wstring GetCharset() const { return this->charset; };

  void SetNewFileType(int t) { this->newfiletype = t; };
  wstring GetNewFileType();

private:
  Sequence Merge(const Sequence &, const Sequence &);
  Sequence MergeSort(const Sequence &);

  void BuildDiagram(const wstring & name);

  bool InitMapper(const wstring & theModel);
  bool InitMapper(void);
  void OpenModel(const wstring & theModel);
  wstring BaseName(const wstring & name, const wstring & ext);
  wstring GetProjectName();

  void ReportUMLWarnings();
  void ReportVPPErrors (const Map &);
  void ReportVPPWarnings();
  void ReportErrors();
  bool HasProjectChanged(bool);

  wstring ClassDiff(const TYPE_AUML_Name &);
  wstring ListDiff(int, const Generic &, const Generic &);
  TYPE_AUML_Model EmptyModel();
  TYPE_AUML_Model ModelMerge(const TYPE_AUML_Model&, 
                             const TYPE_AUML_Model&,
                             const Set &);
  bool LoadDefaultUmlInterface();
  void UnLoadCurrentUmlInterface();
  UmlToolInterface * GetUmlInterface();
  bool IsUmlInterfaceLoaded();
  bool IsCurrentUmlInterfaceType(int i); 
  void SetCurrentUmlInterface(UmlToolInterface * itrf);
  bool IsXMIInterfaceLoaded();
  bool IsRoseInterfaceLoaded();
  bool IsNullInterfaceLoaded();
  void SetXMIMode(int i);
  
private:
  // The ast's used to compute Cur_VPP_Model. Updated by aech call to
  // SetMapperState.
//  Sequence asts;

  wstring ModelFile;
  wstring rtftemplate;
  bool reset;

  UmlToolInterface *TI;

  TYPE_AUML_Model Cur_VPP_Model;
  TYPE_AUML_Model Cur_UML_Model;

  TYPE_AUML_Model Old_VPP_Model;
  TYPE_AUML_Model Old_UML_Model;

  TYPE_MERGE_StateMaps classStatus;
  TYPE_AUML_Model Current_Model;

  wstring charset;
  int newfiletype;
};

#endif // __uml_be_h__

