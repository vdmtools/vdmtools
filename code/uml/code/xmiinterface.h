
#ifndef __xmiinterface_h__
#define __xmiinterface_h__

#include "metaiv.h"
#include "UmlToolInterface.h"
#include "AUML.h"
#include "MERGE.h"
#include "AS.h"
#include "XMI.h"

class XmiInterface : public UmlToolInterface
{
public:
  XmiInterface();  
  XmiInterface(int m);  
  virtual ~XmiInterface();  
  
  // Establishes the connection to the CASE Tool
  // Returns true if it was possible to establish the connection.
  bool Init();  
  
  // Closes down the connection to the CASE Tool.
  void CloseDown();

  // Gives the CASE Tool application window the focus.
  void GiveFocus(); 

  // Gives the name of the file current used by the CASE Tool
  const wstring GetModel();  

  // Selects the model file used by the CASE Tool.
  void SelectModel(const wstring & name);  // Selects the model file used in Rose

  // Creates a backup file of the current model. 
  const wstring DoBackup(const wstring &);

  // Retrieves the current UML model defined in the CASE Tool.
  TYPE_AUML_Model GetCurModel(const Bool & ReportErr,
                              const TYPE_AUML_Model & vppmodel);

  // Set the current UML model of the CASE Tool to be equal to curmdl
  void SetCurModel(const TYPE_AUML_Model & newmdl, 
                   const TYPE_AUML_Model & umlmodel);
  
  // Creates a class diagram with name name for the model in curmdl.
  void BuildClassDiagram(const wstring & name,
                         const TYPE_AUML_Model & curmdl);

  const wstring ToolName() { return L"XMI"; };

  const wstring GetFileExtension() { return L".xml"; };

  void SetMode(int m) { this->mode = m; };
  bool IsASTAH();
  bool IsEA();

private:
  TYPE_XMI_Document AUML2XMI(const Sequence & name, const TYPE_AUML_Model & model);
  TYPE_XMI_UMLClass AUMLClass2XMIClass(const Sequence & name,
                                       const TYPE_AUML_Class & cls,
                                       const Sequence & pid);
  TYPE_XMI_UMLAttribute AUMLValue2XMIAttribute(const TYPE_AS_Pattern & pat,
                                               const TYPE_AUML_ValDef & vd,
                                               const Sequence & pid);
  TYPE_XMI_UMLAttribute AUMLInstValue2XMIAttribute(const Sequence & name,
                                                   const TYPE_AUML_ValDef & vd,
                                                   const Sequence & pid);
  TYPE_XMI_UMLOperation AUMLFnOp2XMIOperation(const Sequence & name,
                                              const TYPE_AUML_Signature & sig,
                                              const Sequence & pid,
                                              bool op);
  TYPE_AUML_Model XMI2AUML(const TYPE_XMI_Document & doc, const Map & idm);
  TYPE_AUML_Classes CreateClasses (const TYPE_XMI_UMLModel & m, const Map & idm);
  TYPE_AUML_Inheritance CreateInheritance(const TYPE_XMI_UMLModel & m, const Map & idm);
  TYPE_AUML_Associations CreateAssociations(const TYPE_XMI_UMLModel & m, const Map & idm);
  Tuple CreateAssociation(const TYPE_XMI_UMLAssociationEnd & se,
                          const TYPE_XMI_UMLAssociationEnd & de,
                          const Map & idm,
                          int mode);
  TYPE_AUML_InstanceVars CreateInstanceVars(const TYPE_XMI_UMLClass & c, const Map & idm);
  TYPE_AUML_ValuesDef CreateValuesDef(const TYPE_XMI_UMLClass & c, const Map & idm);
  Tuple CreateValDef(const Sequence & type,
                     const Generic & initVal,
                     const Sequence & acc,
                     const Bool & stat,
                     const Sequence & id,
                     bool isValue);
                     
  TYPE_AUML_CommonSign CreateOperations(const TYPE_XMI_UMLClass & c, const Map & idm);
  TYPE_AUML_CommonSign CreateFunctions(const TYPE_XMI_UMLClass & c, const Map & idm);
  Tuple XMIParameters2AUMLParams(const Sequence & p_l, const Map & idm);

  Quote MapXMIAccessToAUML(const Sequence & acc);
  Sequence MapAUMLAccessToXMI(const Quote & acc);

  TYPE_XMI_UMLClassifier GetTypeRef(const Sequence & t);
  TYPE_XMI_UMLStereotype GetStereotypeOpRef(const Sequence & pid);
  TYPE_XMI_UMLStereotype GetStereotypeFnRef(const Sequence & pid);

  TYPE_XMI_UMLStereotype GetStereotypeRef(const Sequence & name,
                                          const Sequence & pid,
                                          const Sequence & base);
  TYPE_XMI_UMLGeneralization GetGeneralizationRef(const Sequence & child,
                                                  const Sequence & parent,
                                                  const Sequence & pid);
  Sequence AUMLAssoc2XMIAssoc(const Sequence & clnm,
                              const TYPE_AUML_UniAssoc & ua,
                              const Sequence & modelid);

  TYPE_XMI_UMLMultiplicity AUMLCard2XMIMultASTAH(const Quote & q);
//  Quote XMIMult2AUMLCardASTAH(const TYPE_XMI_UMLMultiplicity & mu);
  Sequence AUMLCard2XMIMult(const Quote & q);
  Quote XMIMult2AUMLCard(const Sequence & mu);

  wstring currentModelName;
  TYPE_AUML_Model currentModel;
  bool modified;
  Map type_m;
  Sequence stype_l;
  Map cls_m;
  Sequence gene_l;
  Sequence mid;

  int mode;
};

#endif // __xmiinterface_h__

