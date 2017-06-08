
#ifndef __nullinterface_h__
#define __nullinterface_h__

#include "metaiv.h"
#include "UmlToolInterface.h"
#include "AUML.h"


class NullInterface : public UmlToolInterface
{

public:
  NullInterface() {};  
  ~NullInterface() {};  
  
public:
  // Establishes the connection to the CASE Tool
  // Returns true if it was possible to establish the connection.
  bool Init() { return true; };  
  
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

  const wstring ToolName() { return L"Null"; };

  const wstring GetFileExtension() { return L".dummy"; };

private:
  wstring model_dummy;
};

#endif // __nullinterface_h__

