#include <stdio.h>

#include "UMLTool.h"
#include "tools.h"
#include "projectval.h"

#ifdef VDMPP
#include "uml-be.h"
UMLMapper *UML = NULL;   // The one instance of the UML mapper.
#endif // VDMPP

// SetMapperState
// ReportErr: nat
// ==> nat
Bool vdm_UMLTool::vdm_SetMapperState(const Bool & ReportErr)
{
#ifdef VDMPP
  return UML->SetMapperState(ToolMediator::GetVDMASTs(), ReportErr);
#else
  return Bool(false);
#endif // VDMPP
}

// ResetMapper
// ==> ()
void vdm_UMLTool::vdm_ResetMapper()
{
#ifdef VDMPP
  UML->ResetMapper();
#endif // VDMPP
}

// MapUML
// GuiSettings : map String to (bool * bool)
// ==> String
TYPE_ProjectTypes_String vdm_UMLTool::vdm_MapUML(const type_cLbb2PM & GuiSettings)
{
#ifdef VDMPP
  return (SEQ<Char>)UML->MapUML(GuiSettings);
#else
  return TYPE_ProjectTypes_String();
#endif // VDMPP
}  

// SetModelNames
// pname : String
// fname : String
// ==> ()
void vdm_UMLTool::vdm_SetModelNames(const TYPE_ProjectTypes_String & pname,
                                    const TYPE_ProjectTypes_String & fname)
{
#ifdef VDMPP
  UML->SetModelNames(pname, fname);
#endif // VDMPP
}

// GetModelName
// ==>  String
TYPE_ProjectTypes_String vdm_UMLTool::vdm_GetModelName()
{
#ifdef VDMPP
  return UML->GetModelName();
#else
  return TYPE_ProjectTypes_String();
#endif // VDMPP
}

// GetClassStatus
// ==> map String to (State * State)
type_cLdd2PM vdm_UMLTool::vdm_GetClassStatus()
{
#ifdef VDMPP
  return UML->GetClassStatus();
#else
  return type_cLdd2PM();
#endif // VDMPP
}

// GetDefaultSettings
// m : map String to (State * State)
// ==> map String to (bool * bool)
type_cLbb2PM vdm_UMLTool::vdm_GetDefaultSettings(const type_cLdd2PM & m)
{
#ifdef VDMPP
  return UML->GetDefaultSettings(m);
#else
  return type_cLbb2PM();
#endif // VDMPP
}

// Diff
// s : set of String
// ==> String
type_cL vdm_UMLTool::vdm_Diff(const type_cLS & s)
{
#ifdef VDMPP
  return UML->Diff(s);
#else
  return type_cL();
#endif // VDMPP
}

// Conflicts
// GuiSettings : map String to (bool * bool)
// ==> bool
Bool vdm_UMLTool::vdm_Conflicts(const type_cLbb2PM & GuiSettings)
{
#ifdef VDMPP
  return UML->Conflicts(GuiSettings);
#else
  return Bool(false);
#endif // VDMPP
}  

// GetConflictMsg
// ==> String
type_cL vdm_UMLTool::vdm_GetConflictMsg()
{
#ifdef VDMPP
  return UML->GetConflictMsg();
#else
  return type_cL();
#endif // VDMPP
}  

// InitUMLMapper: () ==> bool
Bool vdm_UMLTool::vdm_InitUMLMapper ()
{
#ifdef VDMPP
  if (UML == NULL)
    UML = new UMLMapper();
  return Bool(UML != NULL);
#else
  return Bool(false);
#endif // VDMPP
}

// TerminateUMLMapper: () ==> ()
void vdm_UMLTool::vdm_TerminateUMLMapper ()
{
#ifdef VDMPP
  if (UML != NULL) delete UML;
#endif // VDMPP
}

// SetRTFTemplate: seq of char ==> ()
void vdm_UMLTool::vdm_SetRTFTemplate (const type_cL & t)
{
#ifdef VDMPP
  if (UML == NULL) vdm_InitUMLMapper ();
  UML->SetRTFTemplate(t.GetString());
#endif // VDMPP
}

// SetCharSet: seq of char ==> ()
void vdm_UMLTool::vdm_SetCharSet (const type_cL & cs)
{
#ifdef VDMPP
  if (UML == NULL) vdm_InitUMLMapper ();
  UML->SetCharset(cs.GetString());
#endif // VDMPP
}

// SetToolInterface: int ==> ()
void vdm_UMLTool::vdm_SetToolInterface (const Int & n)
{
#ifdef VDMPP
  if (UML == NULL) vdm_InitUMLMapper ();
  UML->SetToolInterface(n.GetValue());
#endif // VDMPP
}

// SetFileType: int ==> ()
void vdm_UMLTool::vdm_SetNewFileType (const Int & n)
{
#ifdef VDMPP
  if (UML == NULL) vdm_InitUMLMapper ();
  UML->SetNewFileType(n.GetValue());
#endif // VDMPP
}
