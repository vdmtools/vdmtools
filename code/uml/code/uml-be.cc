/***
*  * WHAT
*  *  Backend for the UML mapper
*  *
*  *
*  **  * FILE
*  *    $Source: /mnt/cvsroot/toolbox/code/uml/code/uml-be.cc,v $
*  * VERSION
*  *    $Revision: 1.134 $
*  * DATE
*  *    $Date: 2007/03/24 06:36:07 $
*  * FORMAT
*  *    $State: Exp $
*  * PROJECT
*  *    VDM-SL Toolbox.
*  * AUTHOR
*  *    $Author: vdmtools $
*  * COPYRIGHT
*  *    (C) Kyushu University
***/

#define NOMINMAX  // In order to avoid redefinition of min and max.
#include "uml-be.h"
#include "metaiv.h"
#include "AS.h"
#include "nullinterface.h"
#include "xmiinterface.h"
#include "UmlToolInterface.h"
#include "vppinterface.h"
#include "rt_errmsg.h"
#include "mini-p-print.h"
#include "AUML.h"
#include "UMLERR.h"
#include "BUILD.h"
#include "MERGE.h"
#include "intconvquotes.h"
#include "tools.h"
#include "tbutils.h"
#include "projectval.h"
#include "contextinfo.h"  // to get GetFilePos
#include "astaux.h"       // to get GetCid
#include "tb_exceptions.h"
#include "BaseTools.h"
#include "xmiaux.h"
#include "CPP.h"

#ifdef _MSC_VER
#include "roseinterface.h"
#include "windows.h"
#endif // _MSC_VER

UMLMapper::UMLMapper()
{
  init_UMLERR();
  init_BUILD();
  init_MERGE();
  init_AUML();

  SetCurrentUmlInterface(NULL);

  this->ModelFile = L"";

  this->Cur_VPP_Model = EmptyModel();
  this->Cur_UML_Model = EmptyModel();
  this->Old_VPP_Model = EmptyModel();
  this->Old_UML_Model = EmptyModel();
  this->Current_Model = EmptyModel();

  this->rtftemplate = L"";
  this->charset = L"";
}

UMLMapper::~UMLMapper()
{
  UnLoadCurrentUmlInterface();
}

bool UMLMapper::SetToolInterface(int i)
{
  if (IsCurrentUmlInterfaceType(i))
  {
    if (IsXMIInterfaceLoaded()) // 20130209
      SetXMIMode(i);
    return true;
  }

  UnLoadCurrentUmlInterface();

  switch(i) {
    case INTERFACE_NULL: {
      SetCurrentUmlInterface(new NullInterface());
      return true;
    }

#ifdef _MSC_VER
    case INTERFACE_ROSE98: {
      SetCurrentUmlInterface(new RoseInterface());
      return true;
    }
#endif // _MSC_VER

    case INTERFACE_XMI11_UML14_ASTAH: {
      SetCurrentUmlInterface(new XmiInterface(MODE_ASTAH));
      return true;
    }
    case INTERFACE_XMI11_UML14_EA: {
      SetCurrentUmlInterface(new XmiInterface(MODE_EnterpriseArchitect));
      return true;
    }
    default:
      return false;
  }
  return false;
}

// Initializes the UML mapper
bool UMLMapper::InitMapper(const wstring & theModel)
{
  // Note in this function that we need to treat the various strings
  // with care.
  //
  // The class variable ModelFile *does not* have the file extension
  // so this needs to be added on (when passed to TI) or stripped off
  // (when a name received from TI is assigned to it). However we need
  // to ensure that a model file named e.g. model.mdl_old.mdl is handled
  // properly
  if (GetUmlInterface()->Init())
  {
    GetUmlInterface()->SelectModel(theModel);

    // Check whether we are analysing the same model as last time
    if (this->ModelFile != theModel)
    {
      this->ModelFile = theModel;

      // If we are analysing a different model to last time,
      // we reset the mapper state
      ResetMapper();
    }
    return true;
  }
  else
  {
    wstring title (wstring(L"Unable to connect to ") + GetUmlInterface()->ToolName() + L".");
#ifdef _MSC_VER
    wstring message (L"Check that the program is properly installed, and that the entry in the registry is correct.");
    const char * c = getenv("VDMERRMSG");
    if ( c != NULL)
      vdm_log << title << endl << message << endl << flush;
    else
      MessageBoxW (NULL, message.c_str(), title.c_str(), MB_OK | MB_ICONSTOP | MB_SETFOREGROUND);
#else
    vdm_log << title << endl << flush;
#endif // _MSC_VER
    return false;
  }
}

wstring UMLMapper::GetProjectName()
{
  Generic g (ToolMediator::GetProjectName());
  if(!g.IsNil())
    return PTAUX::ExtractFileName(g);
  else
    return L"";
}

wstring UMLMapper::BaseName(const wstring & name, const wstring & fext)
{
  wstring basename (name);
  std::string::size_type pos = name.rfind(fext.c_str());
  if (pos != wstring::npos)
    basename = name.substr(0, pos);

  return basename;
}

bool UMLMapper::InitConnection()
{
  return (GetUmlInterface()->Init());
}

// Initializes the UML mapper to be hooked into ProjectName.mdl
bool UMLMapper::InitMapper(void)
{
  // return InitMapper(L"");
  // Necessary until Rose lets us read the filename of the current
  // model. Until then we can only hook into the current model :^(

  wstring model;
  wstring pjname (GetProjectName());
  if (!pjname.empty())
  {
    model = BaseName(pjname, wstring(L".prj")) + GetUmlInterface()->GetFileExtension();
  }
  else
  {
    // 
    SEQ<Char> mname (GetModelName());
    if (!mname.IsEmpty())
      model = mname.GetString();
  }
  return InitMapper(model);
}

void UMLMapper::OpenModel(const wstring & theModel)
{
  GetUmlInterface()->SelectModel(theModel);
}

void UMLMapper::BuildDiagram(const wstring & name)
{
}

//  ##############################################
//  #  The interface towards the UMLTool class   #
//  ##############################################

// Returns true if the current GUI-settings and the two models will
// result in conflicts during a merge.
Bool UMLMapper::Conflicts(const TYPE_MERGE_GuiSettings & gui_set)
{
  // It is not meaningful to compute conflicts if project has changed
  // in toolbox or Rose
  if (HasProjectChanged(false))
    return Bool(false);

  Bool b;
  try {
    vdm_MERGE_ClearConflicts();
    TYPE_AUML_Model newmdl (vdm_MERGE_Merge(this->Cur_VPP_Model, this->Cur_UML_Model, (Generic)gui_set));
    b = !vdm_MERGE_GetConflictClasses().IsEmpty();
    if(b.GetValue()) {
      vdm_log << L"Error: The models can not be merged due to conflicts in the following classes:" << endl
              << MPP::ListSetOfSeq(vdm_MERGE_GetConflictClasses())
              << endl << flush;
    }
    // The following call to LockedFiles() is added as a last >hack<
    // before the release. The check for locked files should be moved to
    // a seperate function after the release.
    else if(VppInterface::LockedFiles(newmdl, this->Cur_VPP_Model)){
      return Bool(true);
    }
  }
  catch (TB_Exception & e) {
    switch (e.GetExType()) {
    default:
      vdm_log << L"Internal error in Conflicts: " << e.GetExType() << endl << flush;
      break;
    }
  }
  return b;
}

SEQ<Char> UMLMapper::GetConflictMsg()
{
  // No longer needed. Should be removed completely after the release.
  return SEQ<Char>();
}

TYPE_AUML_Model UMLMapper::ModelMerge(const TYPE_AUML_Model & oldModel,
                                      const TYPE_AUML_Model & newModel,
                                      const Set & deleted_)
{
  TYPE_AUML_Classes mergedClasses (oldModel.get_classes());
  TYPE_AUML_Inheritance mergedInh (oldModel.get_inheritance());
  TYPE_AUML_Associations mergedAssoc (oldModel.get_associations());

  mergedClasses.ImpOverride(newModel.get_classes());
  mergedInh.ImpOverride(newModel.get_inheritance());
  mergedAssoc.ImpOverride(newModel.get_associations());

  Set deleted (deleted_);
  Generic g;
  for (bool bb = deleted.First(g); bb; bb = deleted.Next(g))
    if (mergedClasses.DomExists(g))
      mergedClasses.RemElem(g);

  return TYPE_AUML_Model().Init(mergedClasses, mergedInh, mergedAssoc);
}

// MapUML
// GuiSettings : map String to (bool * bool)
// ==> String
SEQ<Char> UMLMapper::MapUML(const TYPE_MERGE_GuiSettings & gui_set)
{
  // Can not map models if project has changed in toolbox or Rose
  if (HasProjectChanged(true))
  {
    return SEQ<Char>();
  }

  ToolMediator::Interf()->vdm_InitMeter(SEQ<Char>(L"Writing definitions..."), type_cL());

  TYPE_AUML_Model newmdl;
  SEQ<TYPE_ProjectTypes_FileName> to_be_parsed;

  if (InitMapper(this->ModelFile))
  {
    vdm_log << L"Merging VDM++ and UML Models..." << endl << flush;
    try {

      // Separate changed and unchanged classes
      TYPE_MERGE_GuiSettings changed (gui_set);

      if (this->Current_Model != EmptyModel()) {
        Set dom_classStatus (this->classStatus.Dom());
        Generic g;
        for (bool bb = dom_classStatus.First(g); bb; bb = dom_classStatus.Next(g)) {
          Tuple t (this->classStatus[g]); // (State * State)
          TYPE_MERGE_State f1 (t.GetRecord(1)), f2(t.GetRecord(2));
          if ((f1.Is(TAG_TYPE_MERGE_None)) && (f2.Is(TAG_TYPE_MERGE_None)) )
            changed.RemElem(g);
        }
      }

      TYPE_AUML_Model mergedmdl = vdm_MERGE_Merge(this->Cur_VPP_Model, this->Cur_UML_Model, changed);
      Tuple deletedTup (mk_(Bool(false), Bool(false)));
      Set deleted;
      Set dom_gui_set (gui_set.Dom());
      Generic g;
      for (bool bb = dom_gui_set.First(g); bb; bb = dom_gui_set.Next(g)) {
        Tuple guiTup (gui_set[g]);
        if (guiTup == deletedTup)
          deleted.Insert(g);
      }

      newmdl = ModelMerge(this->Current_Model, mergedmdl, deleted);

// 20120518 -->
      Map inh (newmdl.get_inheritance());
      Set dom_inh (inh.Dom());
      Set cls_s;
      Generic sup;
      for (bool bb = dom_inh.First(sup); bb; bb = dom_inh.Next(sup))
        cls_s.ImpUnion(inh[sup]);
      cls_s.ImpDiff(dom_inh);
      if (!cls_s.IsEmpty())
      {
        Generic g;
        for (bool cc = cls_s.First(g); cc; cc =cls_s.Next(g))
          vdm_err << L"Class: " << g.ascii() << L" is't defined." << endl;
        vdm_err << L"Error(s) detected in mapping UML model to files." << endl << flush;
        return SEQ<Char>(L"Error");
      }
// <-- 20120518

      this->Current_Model = newmdl;

      wstring cwd (TBUTILS::tb_getcwd());

      // Remember the set of modified files. Used to issue a warning
      // later in the process. The set is extracted here and not later,
      // because the call to MapModelToFiles() is likely to modify
      // several files!!!
      SET<TYPE_ProjectTypes_FileName> modified_s (ToolMediator::CheckForModifiedFiles());

      if(newmdl != this->Cur_VPP_Model) {
        // Update the specifiaction files:
        wstring charset (GetCharset());
        if(!VppInterface::MapModelToFiles(newmdl,
                                          this->Cur_VPP_Model,
                                          to_be_parsed,
                                          charset,
                                          GetNewFileType(),
                                          GetRTFTemplate())) {
          ToolMediator::Interf()->vdm_DestroyMeter();
          vdm_err << L"Error(s) detected in mapping UML model to files." << endl << flush;
          return SEQ<Char>(L"Error");
        }
      }
      else
        vdm_log << L"  No changes necessary in the VDM++ Model" << endl << flush;

      if(newmdl != this->Cur_UML_Model) {
        // Write the newly calculated model to the CASE tool
        if(!modified_s.IsEmpty()) {
          // Some files were changed but not parsed - issue a warning
          vdm_log << L"  Warning: The following files have been modified but not parsed:" << endl;
          Generic gc;
          for (bool bb = modified_s.First(gc); bb; bb = modified_s.Next(gc)) {
            vdm_log << L"    " << PTAUX::ExtractFileName (gc) << endl;
          }
          vdm_log <<L"  You should consider parsing them and then re-invoke the UML-VDM++ Link." << endl << flush;
        }

        if (this->Cur_UML_Model != EmptyModel())
        {
          const wstring backedup = GetUmlInterface()->DoBackup(cwd);
          vdm_log << L"  UML Model was backed up in file: " << backedup << endl << flush;
        }
        GetUmlInterface()->SetCurModel(newmdl, this->Cur_UML_Model);
        vdm_log << L"  UML Model updated." << endl;
      }
      else
        vdm_log << L"  No changes necessary in the UML Model" << endl << flush;
    }
    catch (TB_Exception & e) {
      switch (e.GetExType()) {
      default:
        vdm_log << L"Internal error in MapUML: " << e.GetExType() << endl << flush;
        break;
      }
    }
  }
  vdm_log << L"Done." << endl << flush;
  ToolMediator::Interf()->vdm_DestroyMeter();
  this->Old_VPP_Model = newmdl;
  this->Old_UML_Model = newmdl;

  // And finally parse the updated and new files:
  if(to_be_parsed.Length())
    ToolMediator::BTools()->vdm_SyntaxCheck(to_be_parsed);

  return SEQ<Char>();
}

// SetMapperState
// 
Bool UMLMapper::SetMapperState(const TYPE_AS_Document & doc_l, const Bool & ReportErr)
  // This method is passed a sequence of AST's. One for each class defined
  // in VDM++
  // This method must be called in order to set the state of the mapper,
  // i.e. calculate the two UML models.
  // The method returns true if it was possible to establish a connection to
  // the case tool.
  // If ReportErr is positive, syntax errors encountered in the UML model
  // are reported.
{
  Sequence cs_l;
  size_t len_doc_l = doc_l.Length();
  for (size_t idx = 1; idx <= len_doc_l; idx++)
  {
     Generic cs (doc_l[idx]);
    if (!cs.Is(TAG_TYPE_CPP_Module))
      cs_l.ImpAppend(cs);
  }

  Bool res (true);
  ToolMediator::Interf()->vdm_InitMeter(SEQ<Char>(L"Reading definitions..."), type_cL());

  if(InitMapper())
  {
    try {
      this->reset = false;
      SEQ<TYPE_AS_Class> asts (INT2Q::h2gAS(cs_l)); // Convert handwritten ASs to generated ASs
      GetUmlInterface()->ClearErrors();
      // Call DEP Update in order to calculate the associations.
      vdm_DEP_Update ();
      Map assoc_err (vdm_DEP_CheckStatGraph ());
      if (!assoc_err.IsEmpty()) {
        ReportVPPErrors (assoc_err);
        res = Bool(false);
      }
      else {
        // The UML model based on the VDM++ spec.
        this->Cur_VPP_Model = vdm_BUILD_BuildUmlModel(asts);
        // The UML model from the CASE tool repos.
        this->Cur_UML_Model = GetUmlInterface()->GetCurModel(ReportErr, this->Cur_VPP_Model);
        // Report errors detected:
        if(ReportErr.GetValue())
          ReportErrors();
        res = Bool(true);
      }
    }
    catch (TB_Exception & e) {
      switch (e.GetExType()) {
      default:
        vdm_log << L"Internal error in SetMapperState: " << e.GetExType() << endl << flush;
        break;
      }
    }
  }
  else
    res = Bool(false);
  ToolMediator::Interf()->vdm_DestroyMeter();
  return res;
}

// ResetMapper
void UMLMapper::ResetMapper()
{
  this->Cur_VPP_Model = EmptyModel();
  this->Cur_UML_Model = EmptyModel();
  this->Old_VPP_Model = EmptyModel();
  this->Old_UML_Model = EmptyModel();
  this->Current_Model = EmptyModel();

  this->classStatus = TYPE_MERGE_StateMaps(); // StateMaps = map AUML`Name to (State * State)
  this->reset = true;
}

void UMLMapper::ReportErrors()
{
  //ReportVPPWarnings();
  ReportUMLWarnings();
  ToolMediator::Errs()->vdm_AllDone();
}

void UMLMapper::ReportVPPErrors(const Map & assoc_err)
{
  // Reports the errors when checking the static associations,
  // i.e. if an instance variable refers to more than one
  // object reference.
  // assoc_err is a map from class name to set of names
  // of instance variables.
//  int lastfile = 0, lastline = 0, lastcol = 0;
  Int lastfile(0), lastline(0), lastcol(0);
  vdm_log << L"Could not map VDM model to UML" << endl << flush;
  ToolMediator::Errs()->vdm_ClearAll();
  Set dom_assoc_err (assoc_err.Dom());
  SEQ<TYPE_ProjectTypes_Message> err_l;
  Generic g;
  for (bool bb = dom_assoc_err.First (g); bb; bb = dom_assoc_err.Next (g))
  {
    Set ref (assoc_err[g]);
    Generic id; // an instance variable
    for (bool cc = ref.First (id); cc; cc = ref.Next (id))
    {
      Tuple gfp (GetCI().GetFilePos(ASTAUX::GetCid(id)));
      if (gfp.GetBoolValue(1))
      {
        const TYPE_CI_FileId & fid (gfp.GetInt(2));
        const TYPE_CI_TokenPos & astpos (gfp.GetRecord(4));
        const TYPE_CI_Line & line (astpos.GetInt(pos_CI_TokenPos_abs_uline));
        const TYPE_CI_Column & column (astpos.GetInt(pos_CI_TokenPos_column));
        type_cLL descr_l;
        descr_l.ImpAppend (SEQ<Char>(L"Instance variable refers to multiple object references"));

        err_l.ImpAppend( PTAUX::mk_ErrMsg(fid, line, column, descr_l) );

        lastfile = fid;
        lastline = line;
        lastcol = column;
      }
      else
      {
        type_cLL descr_l;
        descr_l.ImpAppend (SEQ<Char>(L"(Imprecise position information.)"));
        descr_l.ImpAppend (SEQ<Char>(L"Instance variable refers to multiple object references"));

        err_l.ImpAppend( PTAUX::mk_ErrMsg(lastfile, lastline, lastcol, descr_l) );
      }
    }
    SEQ<TYPE_ProjectTypes_Message> sort_l (PTAUX::QS(err_l, false));
    int len_sort_l = sort_l.Length();
    for (int index = 1; index <= len_sort_l; index++)
      ToolMediator::Errs()->vdm_AddMsg(sort_l[index]);
    ToolMediator::Errs()->vdm_Done();
  }
  ToolMediator::Errs()->vdm_AllDone();
}

// Reports the warnings collected during BuildUmlModel().
void UMLMapper::ReportVPPWarnings()
{
  Int lastfile(0), lastline(0), lastcol(0);
  Map warn (vdm_UMLERR_GetWarnings());
  // warn is a mapping from class name to a set of AS`Name

  Set dom_warn (warn.Dom());
  ToolMediator::Errs()->vdm_ClearAll();
  SEQ<TYPE_ProjectTypes_Message> err_l;
  Generic gc;
  for(bool bb = dom_warn.First(gc); bb; bb = dom_warn.Next(gc))
  {
    Set ids (warn[gc]); // The set of warnings associated with a class
    Generic id;
    for(bool cc = ids.First(id); cc; cc = ids.Next(id))
    {
      Tuple gfp (GetCI().GetFilePos(ASTAUX::GetCid(id)));
      if (gfp.GetBoolValue(1))
      {
        const TYPE_CI_FileId & fid (gfp.GetInt(2));
        const TYPE_CI_TokenPos & astpos (gfp.GetRecord(4));
        const TYPE_CI_Line & line (astpos.GetInt(pos_CI_TokenPos_abs_uline));
        const TYPE_CI_Column & column (astpos.GetInt(pos_CI_TokenPos_column));
        type_cLL descr_l;
        descr_l.ImpAppend (SEQ<Char>(L"Warning: Instance variable will not be translated to an association in UML. The object reference is too complex."));
        err_l.ImpAppend( PTAUX::mk_WarnMsg(fid, line, column, descr_l) );
        lastfile = fid;
        lastline = line;
        lastcol = column;
      }
      else{
        type_cLL descr_l;
        descr_l.ImpAppend (SEQ<Char>(L"(Imprecise position information.)"));
        descr_l.ImpAppend (SEQ<Char>(L"Warning: Instance variable will not be translated to an association in UML. The object reference is too complex."));
        err_l.ImpAppend( PTAUX::mk_WarnMsg(lastfile, lastline, lastcol, descr_l) );
      }
    }

    SEQ<TYPE_ProjectTypes_Message> sort_l (PTAUX::QS(err_l, false));
    int len_sort_l = sort_l.Length();
    for (int index = 1; index <= len_sort_l; index++)
      ToolMediator::Errs()->vdm_AddMsg(sort_l[index]);
    ToolMediator::Errs()->vdm_Done();
  }
}

void UMLMapper::ReportUMLWarnings()
{
  SEQ<TYPE_ProjectTypes_Message> err_l (PTAUX::QS(GetUmlInterface()->GetErrors(), false));
  int len_err_l = err_l.Length();
  for (int index = 1; index <= len_err_l; index++)
    ToolMediator::Errs()->vdm_AddMsg(err_l[index]);
  ToolMediator::Errs()->vdm_Done();
}

// This function checks to see whether the project has changed
// while the mapper window was open
bool UMLMapper::HasProjectChanged(bool closewin)
{
  wstring projname (BaseName(GetProjectName(), wstring(L".prj")));
  wstring curModelName (GetUmlInterface()->GetModel());
  wstring fext (GetUmlInterface()->GetFileExtension());

  wstring restart_msg;
  if(closewin)
    restart_msg = L"Restart UML-VDM++ Link";
  else
    restart_msg = L"Close UML-VDM++ Window and restart UML-VDM++ Link";

  // Project has changed if
  // (a) Project name is different to model name,
  // (b) Reset flag has been set. This means that the mapper state
  //     has not been updated since the mapper was last reset
  // (c) Name of model currently in Rose is different to model name
  //     and current model is non-empty (to allow the case in which
  //     mapper is started with empty project, linking to default
  //     project in Rose
  if ((!projname.empty() && (BaseName(projname, wstring(L".prj")) != BaseName(this->ModelFile, fext)))
      || this->reset)
  {
    vdm_log << L"Project has changed since mapper was started." << endl;
    vdm_log << restart_msg << endl << flush;
    return true;
  }

#ifdef _MSC_VER
//  else if (!this->ModelFile.empty() && (curModelName != MakeWindowsName(this->ModelFile)))
  else if (!this->ModelFile.empty() && (curModelName != this->ModelFile))
#else
  else if (!this->ModelFile.empty() && (curModelName != this->ModelFile))
#endif // _MSC_VER
  {
    vdm_log <<  L"Model File in " << GetUmlInterface()->ToolName() << L" has changed." << endl;
    vdm_log << restart_msg << endl << flush;
    return true;
  }
  else
  {
    return false;
  }
}

void UMLMapper::SetModelNames(const Sequence & pname, const Sequence & fname)
{
//  pname.GetString(this->ModelFile);
  this->ModelFile = pname.GetString();
}

SEQ<Char> UMLMapper::GetModelName()
{
  return SEQ<Char>(this->ModelFile);
}

TYPE_MERGE_StateMaps UMLMapper::GetClassStatus()
{
  try {
    // If we are mapping for the first time, we calculate initial
    // states rather than comparing to previous (empty) ones
    if ((this->Old_VPP_Model == EmptyModel()) && (this->Old_UML_Model == EmptyModel()))
      this->classStatus = vdm_MERGE_CalcInitState(this->Cur_VPP_Model, this->Cur_UML_Model);
    else
      this->classStatus = vdm_MERGE_MergeStates(
                            vdm_MERGE_CalcState(this->Old_VPP_Model, this->Cur_VPP_Model),
                            vdm_MERGE_CalcState(this->Old_UML_Model, this->Cur_UML_Model));
  }
  catch (TB_Exception & e)  {
    switch (e.GetExType()) {
    default:
      vdm_log << L"Internal error in GetClassStatus: " << e.GetExType() << endl << flush;
      break;
    }
  }
  return this->classStatus;
}


TYPE_MERGE_DefaultMap UMLMapper::GetDefaultSettings(const TYPE_MERGE_StateMaps & m)
{
  TYPE_MERGE_DefaultMap ds;
  try {
    ds = vdm_MERGE_DefaultSettings(m);
  }
  catch (TB_Exception & e) {
    switch (e.GetExType()) {
    default:
      vdm_log << L"Internal error in GetDefaultSettings: " << e.GetExType() << endl << flush;
      break;
    }
  }
  return ds;
}

//
// The following functions are used to print out the difference
// between two models.
//

Sequence UMLMapper::Merge(const Sequence & s1, const Sequence & s2)
{
  if (s1.IsEmpty())
    return s2;
  else if (s2.IsEmpty())
    return s1;
  else {
    Generic gc1(s1.Hd()), gc2(s2.Hd());
    wstring str1, str2;
    Sequence seq1(gc1), seq2(gc2);
    seq1.GetString(str1);
    seq2.GetString(str2);
    if (str1 < str2) {
      return Merge(s1.Tl(), s2).ImpPrepend(gc1);
    }
    else {
      return Merge(s1, s2.Tl()).ImpPrepend(gc2);
    }
  }
}

Sequence UMLMapper::MergeSort(const Sequence & s)
{
  if (s.Length() < 2)
    return s;
  else {
    Sequence left, right(s);
    int size = s.Length();

    for(int i=0; i<size/2; i++) {
      left.ImpPrepend(right.Hd());
      right.ImpTl();
    }
    return Merge(MergeSort(left), MergeSort(right));
  }
}

SEQ<Char> UMLMapper::Diff(const SET<TYPE_AUML_Name> & nm_s)
{
  // This comparison is meaningless (and possibly could lead to
  // to a segmentation violation) if the project has changed
  if(HasProjectChanged(false))
  {
    return SEQ<Char>();
  }

  Sequence sorted, unsorted;
  SET<TYPE_AUML_Name> nm_s_q (nm_s);
  Generic gc;
  for (bool bb = nm_s_q.First(gc); bb; bb = nm_s_q.Next(gc)) {
    unsorted.ImpPrepend(gc);
  }

  sorted = MergeSort(unsorted);

  wstring s;
  for (bool cc = sorted.First(gc); cc; cc = sorted.Next(gc)) {
    s += ClassDiff(gc);
  }
  vdm_log <<  L"*** Listing differences:" << endl << endl << s << endl << flush;
  return SEQ<Char>();
}

wstring UMLMapper::ClassDiff(const TYPE_AUML_Name & nm_l)
  // Returns a wstring representing the difference
  // between the VDM++ and UML representation of a class.
{
  wstring name, thediff, cldiff;
  TYPE_AUML_Classes uml_cm (this->Cur_UML_Model.get_classes());
  TYPE_AUML_Classes vpp_cm (this->Cur_VPP_Model.get_classes());

  nm_l.GetString(name);
  if(uml_cm.Dom().InSet(nm_l) && vpp_cm.Dom().InSet(nm_l)) {
    try {
    // Compute differences between the two representations.
    TYPE_AUML_Class uml_vpp (vdm_MERGE_ClassDiff(vdm_MERGE_GetClass(nm_l, uml_cm),
                                                 vdm_MERGE_GetClass(nm_l, vpp_cm)));
    TYPE_AUML_Class vpp_uml (vdm_MERGE_ClassDiff(vdm_MERGE_GetClass(nm_l, vpp_cm),
                                                 vdm_MERGE_GetClass(nm_l, uml_cm)));

    // construct diff-wstring:
    cldiff += ListDiff(pos_AUML_Class_inst, uml_vpp, vpp_uml);
    //cldiff += ListDiff(pos_AUML_Class_time, uml_vpp, vpp_uml);
    cldiff += ListDiff(pos_AUML_Class_val, uml_vpp, vpp_uml);
    cldiff += ListDiff(pos_AUML_Class_meth, uml_vpp, vpp_uml);
    cldiff += ListDiff(pos_AUML_Class_func, uml_vpp, vpp_uml);

    // AUML`UniAssoc = map AUML`Name to AUML`Role
    TYPE_AUML_UniAssoc vpp_assoc (vdm_MERGE_GetAssoc(nm_l, this->Cur_VPP_Model.get_associations()));
    TYPE_AUML_UniAssoc uml_assoc (vdm_MERGE_GetAssoc(nm_l, this->Cur_UML_Model.get_associations()));
    cldiff += ListDiff(UML_assiciation_type,
                        vdm_MERGE_MapDiff((Generic)uml_assoc, (Generic)vpp_assoc),
                        vdm_MERGE_MapDiff((Generic)vpp_assoc, (Generic)uml_assoc));

    Set inh_uml(vdm_MERGE_GetInh(nm_l, this->Cur_UML_Model.get_inheritance()));
    Set inh_vpp(vdm_MERGE_GetInh(nm_l, this->Cur_VPP_Model.get_inheritance()));
    if(inh_uml != inh_vpp) {
      cldiff += L"inheritance relations\n\n";
      wstring temp;
      nm_l.GetString(temp);
      cldiff += L">>> UML\nclass " + temp;
      if(inh_uml.Card())
        cldiff += L" is subclass of " + MPP::ListSetOfSeq(inh_uml) + L"\n";
      else
        cldiff += L" has no super classes\n";
      cldiff += L"=== VDM++\nclass " + temp;
      if(inh_vpp.Card())
        cldiff += L" is subclass of " + MPP::ListSetOfSeq(inh_vpp) + L"\n";
      else
        cldiff += L" has no super classes\n";
    }
    if(cldiff != L"")
      thediff = L"Class " + name + L"\n" + cldiff + L"\nend " + name + L"\n";
    else
      thediff = L"The versions of " + name + L" do not differ\n";
    }
    catch (TB_Exception & e) {
	switch (e.GetExType()) {
	default:
	  vdm_log << L"Internal error in ClassDiff: "
		  << e.GetExType() << endl << flush;
	  break;
	}
    }
  }
  else if(uml_cm.Dom().InSet(nm_l) && !vpp_cm.Dom().InSet(nm_l))
    thediff = L"The class " + name + L" is not defined in VDM++\n";
  else if(!uml_cm.Dom().InSet(nm_l) && vpp_cm.Dom().InSet(nm_l))
    thediff = L"The class " + name + L" is not defined in UML\n";

  return thediff;
}


// difftype : pos_AUML_Class_inst | pos_AUML_Class_val | pos_AUML_Class_meth | pos_AUML_Class_func |
//            UML_assiciation_type
// uml_r : AUML`Class (pos_AUML_Class_inst) |
//         AUML`Class (pos_AUML_Class_val)  |
//         AUML`Class (pos_AUML_Class_meth) |
//         AUML`Class (pos_AUML_Class_func) |
//         (map AUML`Name to AUML`ValDef |
//          map AS`Pattern to AUML`ValDef |
//          map AUML`Name to AUML`Signature (UML_assiciation_type))
// vpp_r : AUML`Class (pos_AUML_Class_inst) |
//         AUML`Class (pos_AUML_Class_val)  |
//         AUML`Class (pos_AUML_Class_meth) |
//         AUML`Class (pos_AUML_Class_func) |
//         (map AUML`Name to AUML`ValDef |
//          map AS`Pattern to AUML`ValDef |
//          map AUML`Name to AUML`Signature (UML_assiciation_type))
// -> seq of char
wstring UMLMapper::ListDiff(int difftype, const Generic & uml_r, const Generic & vpp_r)
  // Constructs a diff-wstring for the class entities in the maps uml_vpp and
  // vpp_uml.
  // This function is also used to list differences in the definition of
  // associations.
  // uml_vpp is the uml representation L"minus" the vpp representation, and
  // vpp_uml is the vpp representation L"minus" the uml representation.
{
//  Map uml_vpp(uml_r.GetField(difftype)),
//    vpp_uml(vpp_r.GetField(difftype));
  Map uml_vpp, vpp_uml;
  if(difftype == UML_assiciation_type) {  // the passed arguments are maps
    uml_vpp = Map(uml_r);
    vpp_uml = Map(vpp_r);
  }
  else { // The arguments are records representing a class.
    uml_vpp = TYPE_AUML_Class(uml_r).GetMap(difftype);
    vpp_uml = TYPE_AUML_Class(vpp_r).GetMap(difftype);
  }
  wstring thediff;
  if(!uml_vpp.Dom().IsEmpty() || !vpp_uml.Dom().IsEmpty()) {
    Set uml = uml_vpp.Dom(), vpp = vpp_uml.Dom(), cfl;
    // Set of names of conflicting entities
    cfl = vpp;
    cfl.ImpIntersect(uml);
    // Constructs from UML not in VPP:
    Set unpp = uml;
    unpp.ImpDiff(vpp);
    thediff += L"\n" + VppInterface::ent_type2wstring(difftype) + L"\n";
    if(!unpp.IsEmpty() || !cfl.IsEmpty()) {
      thediff += L">>> UML\n";
      Generic gc;
      for (bool bb = unpp.First(gc); bb; bb = unpp.Next(gc))
      {
        thediff += VppInterface::PPEntity(difftype, gc, uml_vpp[gc], L"");
        thediff += VppInterface::PPConditions(difftype, gc, uml_vpp[gc], L"");
        thediff += L"\n";
      }
      // List conflicts
      for (bool cc = cfl.First(gc); cc; cc = cfl.Next(gc))
      {
        thediff += VppInterface::PPEntity(difftype, gc, uml_vpp[gc], L"");
        thediff += VppInterface::PPConditions(difftype, gc, uml_vpp[gc], L"");
        thediff += L"\n";
      }
      thediff += L"===";
    }
    // Constructs from VPP not in UML:
    Set ppnu = vpp;
    ppnu.ImpDiff(uml);
    if(!ppnu.IsEmpty() || !cfl.IsEmpty())
    {
      if(unpp.IsEmpty() || cfl.IsEmpty())
        thediff += L">>> VDM++\n";
      else
        thediff += L" VDM++\n";

      Generic gc;
      for (bool dd = ppnu.First(gc); dd; dd = ppnu.Next(gc))
      {
        thediff += VppInterface::PPEntity(difftype, gc, vpp_uml[gc], L"");
        thediff += VppInterface::PPConditions(difftype, gc, vpp_uml[gc], L"");
        thediff += L"\n";
      }
      // List conflicts
      for (bool ee = cfl.First(gc); ee ; ee = cfl.Next(gc))
      {
        thediff += VppInterface::PPEntity(difftype, gc, vpp_uml[gc], L"");
        thediff += VppInterface::PPConditions(difftype, gc, vpp_uml[gc], L"");
        thediff += L"\n";
      }

      if(unpp.IsEmpty() && cfl.IsEmpty())
        thediff += L"===\n";
      else
        thediff += L"<<<\n";
    }
    else {
      thediff += L"\n";
    }
  }
  return thediff;
}

TYPE_AUML_Model UMLMapper::EmptyModel()
{
  TYPE_AUML_Model r;
  // The maps should be constructed properly in due time...
  r.Init(TYPE_AUML_Classes(), TYPE_AUML_Inheritance(), TYPE_AUML_Associations());
  return r;
}

bool UMLMapper::LoadDefaultUmlInterface()
{
#ifdef _MSC_VER
#ifdef _USE_ROSE
  // for Windows with _USE_ROSE flag
//  int default_interface = INTERFACE_ROSE98;
  int default_interface = INTERFACE_XMI11_UML14_ASTAH;
#else
  // for Windows without _USE_ROSE flag
  int default_interface = INTERFACE_XMI11_UML14_ASTAH;
#endif // _USE_ROSE
#else
  // for Mac OS X, Linux, Solaris
  int default_interface = INTERFACE_XMI11_UML14_ASTAH;
#endif // _MSC_VER

  return SetToolInterface(default_interface);
}

void UMLMapper::UnLoadCurrentUmlInterface()
{
  if (this->TI != NULL)
  {
    this->TI->CloseDown();
    delete this->TI;
    this->TI = NULL;
  }
}

UmlToolInterface * UMLMapper::GetUmlInterface()
{
  if (!IsUmlInterfaceLoaded())
    LoadDefaultUmlInterface();
  
  return this->TI;
}

bool UMLMapper::IsUmlInterfaceLoaded()
{
  return (this->TI != NULL);
}

bool UMLMapper::IsXMIInterfaceLoaded()
{
  if (!IsUmlInterfaceLoaded())
    return false;

  return (this->TI->ToolName() == wstring(L"XMI"));
}

bool UMLMapper::IsRoseInterfaceLoaded()
{
  if (!IsUmlInterfaceLoaded())
    return false;

  return (this->TI->ToolName() == wstring(L"Rose"));
}

bool UMLMapper::IsNullInterfaceLoaded()
{
  if (!IsUmlInterfaceLoaded())
    return false;

  return (this->TI->ToolName() == wstring(L"Null"));
}

bool UMLMapper::IsCurrentUmlInterfaceType(int i)
{
  switch (i) {
    case INTERFACE_ROSE98: {
      return IsRoseInterfaceLoaded();
      break;
    }
    case INTERFACE_XMI11_UML14_ASTAH:
    case INTERFACE_XMI11_UML14_EA: {
      return IsXMIInterfaceLoaded();
      break;
    }
    case INTERFACE_NULL: {
      return IsNullInterfaceLoaded();
      break;
    }
  }
  return false;
}

void UMLMapper::SetCurrentUmlInterface(UmlToolInterface * itrf)
{
  this->TI = itrf;
}

void UMLMapper::SetXMIMode(int i)
{
  if (!IsXMIInterfaceLoaded())
    return;

  switch (i) {
    case INTERFACE_XMI11_UML14_ASTAH: {
      ((XmiInterface *)(this->TI))->SetMode(MODE_ASTAH);
      break;
    }
    case INTERFACE_XMI11_UML14_EA: {
      ((XmiInterface *)(this->TI))->SetMode(MODE_EnterpriseArchitect);
      break;
    }
  }
}

wstring UMLMapper::GetNewFileType()
{
  switch (this->newfiletype) {
    case FILETYPE_VPP: {
      return L".vpp";
    }
    case FILETYPE_RTF: {
      return L".rtf";
    }
    case FILETYPE_TEX: {
      return L".tex";
    }
    default: {
      return L".vpp";
    }
  }
}


