/***
*  * WHAT
*  *    A general interface towards UML CASE Tools. 
*  *    This class defines various functions used when commumicating with
*  *    the UML CASE Tool. 
*  *    Interfaces to specific CASE tools are created by inheriting from
*  *    this class and implementing the virtual functions.
*  *    
*  **  * FILE
*  *    $Source: /home/vdmtools/cvsroot/toolbox/code/uml/code/UmlToolInterface.h,v $
*  * VERSION
*  *    $Revision: 1.9 $
*  * DATE
*  *    $Date: 2006/04/18 07:45:42 $
*  * FORMAT
*  *    $State: Exp $
*  * PROJECT
*  *    VDM-SL Toolbox.
*  * AUTHOR
*  *    $Author: vdmtools $
*  * COPYRIGHT
*  *    (C) Kyushu University
***/

#ifndef __umltoolinterface_h__
#define __umltoolinterface_h__

#include "AUML.h"

class UmlToolInterface
{

public:
  UmlToolInterface();
  virtual ~UmlToolInterface();
  
public:
  virtual const wstring ToolName() {
    return L"UNKNOWN";
  };

  // Establishes the connection to the CASE Tool
  // Returns true if it was possible to establish the connection.
  virtual bool Init() = 0;  

  // Closes down the connection to the CASE Tool.
  virtual void CloseDown() = 0;

  // Gives the CASE Tool application window the focus.
  virtual void GiveFocus() = 0; 

  // Gives the name of the file current used by the CASE Tool
  virtual const wstring GetModel() {
    return L"";
  };

  // Selects the model file used by the CASE Tool.
  virtual void SelectModel(const wstring & name) = 0;  

  // Creates a backup file of the current model. 
  virtual const wstring DoBackup(const wstring &) = 0;

  // Retrieves the current UML model defined in the CASE Tool.
  virtual TYPE_AUML_Model GetCurModel(const Bool &,
                                      const TYPE_AUML_Model &) = 0;

  // Set the current UML model of the CASE Tool to be equal to curmdl
  virtual void SetCurModel(const TYPE_AUML_Model &,
                           const TYPE_AUML_Model &) = 0;

  // Creates a class diagram with name name for the model in curmdl.
  virtual void BuildClassDiagram(const wstring & name,
                                 const TYPE_AUML_Model & curmdl) = 0;

  virtual const wstring GetFileExtension() = 0;

  virtual void ClearErrors(){Errors = Sequence();};
  virtual Sequence GetErrors(){return Errors;};
  virtual void AddError(Record err){ Errors.ImpAppend(err); };

protected:
  Sequence Errors;
};

#endif // __umltoolinterface_h__

