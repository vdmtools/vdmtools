/***
*  * WHAT
*  *    The interface towards Rational Rose 98
*  *    Contains functionality to update the class repository in RR
*  *    and to create diagrams.
*  *    Furthermore functionality to create an abstract uml representation
*  *    based on the class repository is defined here.
*  *
*  **  * FILE
*  *    $Source: /home/vdmtools/cvsroot/toolbox/code/uml/code/roseinterface.cc,v $
*  * VERSION
*  *    $Revision: 1.37 $
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

#define NOMINMAX
#include "roseinterface.h"
#include "rose_aux.h"

#include "mini-p-print.h"
#include "rt_errmsg.h"
#include "parse_string.h"
#include "projectval.h"
#include "tools.h"
#include "asquotes.h"
#include "MANGLE.h"
#include "tbutils.h"
#include "Repository.h"
#include "Interface.h"
#include "MERGE.h"
#include "tb_wstring.h"

#include "objbase.h"

#undef SetType

// #####################################################################
// ##  Methods handling the interface towards Rational Rose           ##
// #####################################################################

RoseInterface::RoseInterface()
{
  this->Initialized = false;
}

RoseInterface::~RoseInterface()
{
  if (this->Initialized)
    CloseDown();
}

bool RoseInterface::Init()
  // Initializes or restores the connection to Rose. The restoring of
  // the connection is necessary because Rose does not handle its
  // clients properly! If Rose services a client process and the user
  // chooses to exit Rose, the Rose process exits completely, leaving
  // its client(s) alone. This behavior is wrong. In stead Rose should
  // simply become invisible but stay L"alive" to service its
  // client(s).
{
  if (!this->Initialized)
    ::CoInitialize(NULL);

//  static bool Initialized = false;
  TRY {
    // Just some dummy operation to check if the connection was lost:
    rose.GetVisible();
  }
  CATCH(COleException, ex) {
    this->Initialized = false;
  }
  AND_CATCH(CException, ex) {
    this->Initialized = false;
  }
  END_CATCH;

  if(this->Initialized) {
    if(!rose.GetVisible())
      rose.SetVisible(TRUE);
    return true;
  }

  ToolMediator::Interf()->vdm_UpdateMeter(0.50, (SEQ<Char>) Sequence(wstring(L"Opening connection to ") + this->ToolName() ));
  CLSID roseId;
  COleException e;
  if (CLSIDFromProgID(OLESTR("Rose.Application"), &roseId) != NOERROR){
    return false;
  }

  LPUNKNOWN lpUnk;
  LPDISPATCH lpDispatch;
  if (GetActiveObject(roseId, NULL, &lpUnk) == NOERROR) {
    HRESULT hr = lpUnk->QueryInterface(IID_IDispatch,
                                       (LPVOID*)&lpDispatch);
    lpUnk->Release();
    if (hr == NOERROR)
      rose.AttachDispatch(lpDispatch, TRUE);
  }
  else{
    // Start instance of RR
    if (!rose.CreateDispatch(roseId, &e)) {
      return false;
    }
  }
  ToolMediator::Interf()->vdm_UpdateMeter(1.0, (SEQ<Char>) Sequence(wstring(L"Opening connection to ") + this->ToolName() ));
  if(!rose.GetVisible())
    GiveFocus();
  this->Initialized = true;
  return true;
}

// Gives the CASE Tool application window the focus.
void RoseInterface::GiveFocus()
{
  ::CoInitialize(NULL);
  rose.SetVisible(TRUE);
  if (!rose.GetVisible()) {
    wstring title (L"");
    wstring message (wstring(L"Unable to give focus to ") + this->ToolName());
//    AfxMessageBox(CString(L"Unable to give focus to ") + CString(this->ToolName().c_str()));
    MessageBoxW (NULL, message.c_str(), title.c_str(), MB_OK | MB_ICONSTOP | MB_SETFOREGROUND);
  }
  ::CoUninitialize();
}

// Return the filename of the model currently in Rose
const wstring RoseInterface::GetModel()
{
  ::CoInitialize(NULL);
  IRoseModel curModel = rose.GetCurrentModel();
  CString curModelName = curModel.GetName();
  ::CoUninitialize();

  wstring wname (curModelName); 
  wstring mname;
  for (int i = 0; i < wname.length(); i++)
  {
    if (L'\\' == wname[i])
      mname += L'/';
    else
      mname += wname[i];
  }
  return mname;
}

void RoseInterface::SelectModel(const wstring & piName)
  // Chooses a model in Rose based on the following L"rules":
  //  If name equals the empty wstring, the current model in Rose is used
  //  Otherwise, a model with the name is read in.
  // piName is a platform-independent name i.e. it uses forward
  // slashes as separators (from QT)
{
  ::CoInitialize(NULL);
  // Default value of current model, used when name == L""
  theModel = rose.GetCurrentModel();
  RootCat = theModel.GetRootCategory();

  CString name (TBUTILS::MakeWindowsName(piName).c_str());
  if(!name.IsEmpty())
  {
    // Open and connect to the model given by name
    FILE * exists;
    bool res = ((exists = _wfopen((LPCTSTR) name, L"r")) != NULL);
    if(res) { // The file exists, ok to open it.
      fclose(exists);

      CString current_model = theModel.GetName();
      if(name != current_model) {
        if(theModel.IsModified()) {
          rose.SetVisible(TRUE);
        }
        theModel = rose.OpenModel(name);
      }
      else  // model open already:
        theModel = rose.GetCurrentModel();
    }
    else
    {
      if(theModel.IsModified())
        rose.SetVisible(TRUE);
      theModel = rose.NewModel();

      TRY {
        // Just some dummy operation to check if the connection was lost:
        rose.SaveAs(name, FALSE);
      }
      CATCH(COleException, ex) {
        vdm_err << L"Unable to save Rose model " << name << endl << flush;
      }
      AND_CATCH(CException, ex) {
        vdm_err << L"Unable to save Rose model " << name << endl << flush;
      }
      END_CATCH;
    }
  }
  // To make sure that theModel points to something sensible. It may
  // not if the user presses L"cancel" in the Open dialog of Rose:
  theModel = rose.GetCurrentModel();
  RootCat = theModel.GetRootCategory();

  IRoseCollection cts = theModel.GetAllCategories();
  if( (ClassesCat = FindByName(L"Generated classes", cts)) == NULL)
    ClassesCat = RootCat.AddCategory(L"Generated classes");

  // Find a component that supports VDM++:
  IRoseSubsystemCollection subs = theModel.GetAllSubsystems();
  IRoseSubsystem sub = subs.GetAt(1);
  IRoseModuleCollection modules = sub.GetAllModules();
  int n = modules.GetCount(), found = 0;
  for(int i = 1; i <= n; i++) {
    if(IRoseModule(modules.GetAt(i)).GetAssignedLanguage() == L"VDM++")
      found = i;
  }

  if(found > 0)
    VDMComponent = modules.GetAt(found);
  else {
    VDMComponent = sub.AddModule(L"VDM++");
    VDMComponent.SetStereotype(L"");
    VDMComponent.SetAssignedLanguage(L"VDM++");
  }
  ::CoUninitialize();
}

void RoseInterface::CloseDown()
{
  TRY {
    rose.ReleaseDispatch();
  }
  CATCH(COleException, ex) {
    vdm_err << L"Unable to release Rose model " << endl << flush;
  }
  AND_CATCH(CException, ex) {
    vdm_err << L"Unable to release Rose model " << endl << flush;
  }
  END_CATCH;

  ::CoUninitialize();
  this->Initialized = false;
}

const wstring RoseInterface::DoBackup(const wstring & where)
{
  ::CoInitialize(NULL);
  wstring revert_to (theModel.GetName());
  wstring back_namePi;
  if(revert_to == L"")
  {
    revert_to = wstring(where) + L"\\untitled.mdl";
    back_namePi = wstring(where) + L"\\untitled_old.mdl";
    CString revertWin (TBUTILS::MakeWindowsName(revert_to).c_str());
    TRY {
      rose.SaveAs((LPCTSTR) revertWin, FALSE);
    }
    CATCH(COleException, ex) {
      vdm_err << L"Unable to save Rose model " << (LPCTSTR) revertWin
              << endl << flush;
    }
    AND_CATCH(CException, ex) {
      vdm_err << L"Unable to save Rose model " << (LPCTSTR) revertWin
              << endl << flush;
    }
    END_CATCH;
  }
  else {
//    int pos;
//    if(pos = revert_to.rfind(L".mdl"))
//      back_namePi = revert_to.substr(0, pos) + L"_old.mdl";
    std::string::size_type pos = revert_to.rfind(L".mdl");
    if(pos != wstring::npos)
      back_namePi = revert_to.substr(0, pos) + L"_old.mdl";
    else
      back_namePi = revert_to + L"_old.mdl";
  }

  CString back_name (TBUTILS::MakeWindowsName(back_namePi).c_str());

  // Save current model, so that we can revert to this one.
  if(theModel.IsModified())
  {
    TRY {
      rose.Save(FALSE);
    }
    CATCH(COleException, ex) {
      vdm_err << L"Unable to save existing Rose model " << endl << flush;
    }
    AND_CATCH(CException, ex) {
      vdm_err << L"Unable to save existing Rose model " << endl << flush;
    }
    END_CATCH;
  }
  // remove already existing backup file:
  FILE *exists;
  bool res = ((exists = _wfopen((LPCTSTR) back_name, L"r")) != NULL);
  if(res) { // The file exists, ok to remove it.
    fclose(exists);
    remove(TBWSTR::wstring2string((LPCTSTR) back_name).c_str());
  }
  TRY {
    rose.SaveAs((LPCTSTR) back_name, FALSE);
  }
  CATCH(COleException, ex) {
    vdm_err << L"Unable to save Rose model " << (LPCTSTR) back_name
            << endl << flush;
  }
  AND_CATCH(CException, ex) {
    vdm_err << L"Unable to save Rose model " << (LPCTSTR) back_name
            << endl << flush;
  }
  END_CATCH;

  this->SelectModel((LPCTSTR) TBUTILS::MakeWindowsName(revert_to).c_str());
  ::CoUninitialize();

  return (LPCTSTR) back_name;
}

void RoseInterface::SetCurModel(const TYPE_AUML_Model & new_mdl, const TYPE_AUML_Model & curmdl)
  // Updates the Rose repository to contain exactly the model mdl.
  // Overrides all definitions in the Rose repository such that
  // subsequently the model in the Rose repository is equivalent to
  // the model mdl.
  // In order to update the repository without affecting the diagrams
  // already created, the current model in the repository is
  // calculated, and used to calculate exactly which classes,
  // associations and inheritance relations to remove in RR.  This
  // function is called whenever the RR repository should be updated,
  // whether the original action was to merge or override.
{
  ::CoInitialize(NULL);

  TYPE_AUML_Classes rose_m (curmdl.get_classes());
  TYPE_AUML_Classes new_m (new_mdl.get_classes());

  RemoveObsoleteClasses(new_m.Dom());

  // Extract the classes that have been modified:
  TYPE_AUML_Classes modified_m;
  Set dom_new_m (new_m.Dom());
  Generic gc;
  for (bool bb = dom_new_m.First(gc); bb; bb = dom_new_m.Next(gc)) {
    if(!vdm_MERGE_IsEqual(gc, new_mdl, curmdl).GetValue())
      modified_m.Insert(gc, new_m[gc]);
  }
  // Pass original and new model to allow modification rather
  // than overwriting of existing classes
  CreateClasses(modified_m, rose_m);

  UpdateInheritance(new_mdl.get_inheritance(), curmdl.get_inheritance());
  UpdateAssociations(new_mdl.get_associations(), curmdl.get_associations());

  ::CoUninitialize();
}

TYPE_AUML_Model RoseInterface::GetCurModel(const Bool & ReportErr, const TYPE_AUML_Model & vppmdl)
  // Traverses the Rose repository and builds an abstact UML model.
{
  TYPE_AUML_Model r;
  // The maps should be constructed properly in due time...
  r.set_classes(GenClassMap(ReportErr, vppmdl.get_classes()));
  r.set_inheritance(GenInhMap());
  r.set_associations(GenAssMap(vppmdl.get_associations()));
  return r;
}

// #####################################################################
// ##  Methods for creating local views.                              ##
// #####################################################################


void RoseInterface::BuildView(const TYPE_AUML_Name & nm,
                              const SET<TYPE_AUML_Name> & cl_s,
                              const IRoseCategory & cat)
  // Builds a view containing the classes in set cl_s. In a later
  // version this function could be used to let the user create
  // diagrams from the GUI of the Rose-VDM++ Link. Currently it is
  // only used when creating local views. See function
  // BuildClassDiagram().
{
  IRoseClassDiagram d = CreateDiagram(seq2cstr(nm), cat);
  IRoseCollection classes = theModel.GetAllClasses();
  IRoseClass c;
  Set n_cl_s(cl_s);
  d.SetVisible(TRUE);
  n_cl_s.Insert(nm);
  Generic gc;
  for (bool bb = n_cl_s.First(gc); bb; bb = n_cl_s.Next(gc)) {
    if( (c = FindByName(seq2cstr(gc), classes)) != NULL)
      // Ignoring classes not defined in RR
      d.AddClass(c);
  }
  d.Layout();
  d.SetVisible(FALSE);  // Maybe this could be an option from the user
}

LPDISPATCH RoseInterface::CreateDiagram(CString nm, IRoseCategory cat)
  // Creates an empty class diagram with name nm in the category cat.
  // If a diagram with name nm already exists, it is removed.
{
  IRoseCollection diag = cat.GetClassDiagrams();
  LPDISPATCH dt;
  Generic gc;
  if ((dt=FindByName(nm, diag)) != NULL)
    // remove already existing diagram
    cat.DeleteClassDiagram(IRoseClass(dt));
  return cat.AddClassDiagram(nm);
}

void RoseInterface::BuildClassDiagram(const wstring & name, const TYPE_AUML_Model & curmdl)
  // This function is not used currently. In a future version it could
  // maybe be possible to construct class diagrams from the GUI of the
  // Rose-VDM++ Link.
{
  BuildView(cstr2seq(name.c_str()), curmdl.get_classes().Dom(), DiagramsCat);
}

// #####################################################################
// ##  Misc. functions used by the mapping in both directions.        ##
// ##  Some functions are use-full for debugging purposes.            ##
// #####################################################################

void RoseInterface::DisplayWarning(CString c_nm, CString str)
{
  wstring wstr (wstring(L"Warning: Rose repository, class ") + wstring(c_nm) + L". " + wstring(str));
  type_cLL ms_l;
  ms_l.ImpAppend(SEQ<Char>(wstr));
  AddError(PTAUX::mk_WarnMsg(Int(0), Int(0), Int(0), ms_l));
}

LPDISPATCH RoseInterface::FindByName(CString name, IRoseCollection l)
  // If the collection l contains an item with name, a pointer to it is
  // returned. Otherwise NULL is returned.
{
  int n = l.FindFirst(name);
  if(n)
    return l.GetAt(n);
  else
    return NULL;
}

void RoseInterface::ListNames(IRoseCollection l)
{
  int n = l.GetCount();
  for(int i=1; i<=n; i++) {
    IRoseItem c = l.GetAt(i);
//    AfxMessageBox(c.GetName());
    wstring title (L"");
    wstring message (c.GetName());
    MessageBoxW (NULL, message.c_str(), title.c_str(), MB_OK | MB_ICONSTOP | MB_SETFOREGROUND);
  }
}

SET<TYPE_AUML_Name> RoseInterface::MkNameSet(IRoseCollection l)
{
  int m = l.GetCount();
  SET<TYPE_AUML_Name> s;
  for(int j = 1; j <= m; j++)
    s.Insert((Generic)cstr2seq(IRoseItem(l.GetAt(j)).GetName()));
  return s;
}

// #####################################################################
// ##  Functions implementing the mapping from AUML to UML.           ##
// #####################################################################

void RoseInterface::CreateClasses(const TYPE_AUML_Classes & class_m, const TYPE_AUML_Classes & orig_m)
  // Creates or updates the classes in the map (map Name to Class)
{
  IRoseCollection classes = theModel.GetAllClasses();
  IRoseClass c;
  int n = class_m.Dom().Card(), j=0;
  Set dom_class_m (class_m.Dom());
  Generic gc;
  for(bool bb = dom_class_m.First(gc); bb; bb = dom_class_m.Next(gc))
  {
    TYPE_AUML_Name nm (gc);
    wstring nm_str;
    nm.GetString(nm_str);

    wstring label = L"Updating: " + nm_str;
    ToolMediator::Interf()->vdm_UpdateMeter((float)j++/n,
                              (SEQ<Char>)Sequence(label));

    IRoseClass c;
    if( (c = FindByName(nm_str.c_str(), classes)) == NULL) {
      // The class does not exist, need to create it...
      c = ClassesCat.AddClass(nm_str.c_str());
      //      c.SetFundamentalType(TRUE);
      c.AddAssignedModule(VDMComponent);
      UpdateInternals(c, class_m[nm]);
    }
    else
    {
      // Class already exists, so modify it
      UpdateInternals(c, class_m[nm], orig_m[nm]);
    }
    UpdateFilename(c, nm_str);
  }
}

// UpdateInternals
void RoseInterface::UpdateInternals(IRoseClass c, const TYPE_AUML_Class & cl, const TYPE_AUML_Class & old_cl)
  // This function performs three tasks:
  //    (a) all attributes and operations present in the old model but
  //        absent in the new one are removed from IRoseClass c
  //    (b) all attributes and operations present in the new model but
  //        absent in the old one are added to IRoseClass c
  //    (c) all attributes and operations that have been modified between
  //        the two models are updated in IRoseClass c
{
  TYPE_AUML_ValuesDef old_vals (old_cl.get_val()), new_vals (cl.get_val());     // map AS`Pattern to ValDef
  TYPE_AUML_InstanceVars old_inst (old_cl.get_inst()), new_inst (cl.get_inst()); // map Name to AUML`ValDef

  SET<TYPE_AS_Pattern> obsolete_vals (old_vals.Dom().ImpDiff(new_vals.Dom()));
  SET<TYPE_AUML_Name> obsolete_inst (old_inst.Dom().ImpDiff(new_inst.Dom()));

  Set obsolete_attribute_ids;
  Generic g;
  for (bool bb = obsolete_vals.First(g); bb; bb = obsolete_vals.Next(g))
    obsolete_attribute_ids.Insert(old_vals[g].get_id());

  for (bool cc = obsolete_inst.First(g); cc; cc = obsolete_inst.Next(g))
    obsolete_attribute_ids.Insert(old_inst[g].get_id());

  if (!obsolete_attribute_ids.IsEmpty())
    RemoveObsoleteAttributes(c, obsolete_attribute_ids);

  // Update remaining attributes as needed
  UpdateInstVars(c, cl.get_inst(), old_cl.get_inst());
  UpdateValues(c, cl.get_val(), old_cl.get_val());

  // Compute method names in old model
  TYPE_AUML_CommonSign old_ops (old_cl.get_meth());
  TYPE_AUML_CommonSign old_funcs (old_cl.get_func());
  TYPE_AUML_CommonSign new_ops (cl.get_meth());
  TYPE_AUML_CommonSign new_funcs (cl.get_func());


  SET<TYPE_AUML_Name> obsolete_op_names (old_ops.Dom().ImpDiff(new_ops.Dom()));
  SET<TYPE_AUML_Name> obsolete_func_names (old_funcs.Dom().ImpDiff(new_funcs.Dom()));

  SET<TYPE_AUML_RoseId> obsolete_method_ids;

  for (bool dd = obsolete_op_names.First(g); dd;
            dd = obsolete_op_names.Next(g))
    obsolete_method_ids.Insert(old_ops[g].get_id());

  for (bool ee = obsolete_func_names.First(g); ee;
            ee = obsolete_func_names.Next(g))
    obsolete_method_ids.Insert(old_funcs[g].get_id());

  // Remove obsolete methods
  if (!obsolete_method_ids.IsEmpty())
    RemoveObsoleteMethods(c, obsolete_method_ids);

  UpdateMethods(c, cl.get_meth(), old_cl.get_meth());
  UpdateFunctions(c, cl.get_func(), old_cl.get_func());
  c.SetAbstract(cl.get_abstract().GetValue() ? TRUE : FALSE);
}

void RoseInterface::UpdateInternals(IRoseClass c, const TYPE_AUML_Class & cl)
  // This function adds the operations and attributes defined in cl
  // to the new class c. Since there are no existing operations or
  // attributes (because the class is new), nothing needs to be removed
  // from c.
{
  UpdateInstVars(c, cl.get_inst(), TYPE_AUML_InstanceVars());
  UpdateValues(c, cl.get_val(), TYPE_AUML_ValuesDef());
  UpdateMethods(c, cl.get_meth(), TYPE_AUML_CommonSign());
  UpdateFunctions(c, cl.get_func(), TYPE_AUML_CommonSign());
  c.SetAbstract(cl.get_abstract().GetValue() ? TRUE : FALSE);
}

void RoseInterface::RemoveObsoleteClasses(const SET<TYPE_AUML_Name> & cs)
  // Removes all classes from Rose that is not mentioned in set cs
{
  IRoseClassCollection classes = theModel.GetAllClasses();
  IRoseClass c;

  int n = classes.GetCount(), j = 0;
  LPDISPATCH *tbd = new LPDISPATCH[n];
  for(int i = 1; i <= n; i++) {
    c = classes.GetAt(i);
    if(!cs.InSet(cstr2seq(c.GetName())) )
      tbd[j++] = classes.GetAt(i);
  }
  for(int ii = 0; ii < j; ii++) {
    IRoseClass c = tbd[ii];
    wstring label(L"Deleting: ");
    label += wstring(c.GetName());
    ToolMediator::Interf()->vdm_UpdateMeter((float)ii/j,
                              (SEQ<Char>)Sequence(label));
    IRoseCategory parent = c.GetParentCategory();
    parent.DeleteClass(c);
  }
  delete[] tbd;
}

void RoseInterface::UpdateFilename(IRoseClass c, wstring classname)
  // Ensures that files containing VDM++ specification for this class
  // are added as external documents for the class.
{
  SET<TYPE_ProjectTypes_FileName> classFiles =
    ToolMediator::Repos()->vdm_FileOfModule(PTAUX::mk_ModuleName(classname));
  // Note that although classFiles is a set, this is purely for flat
  // VDM-SL specifications, which is not relevant here. Thus normally
  // this set will contain exactly one element.

  if (classFiles.IsEmpty()) {
    vdm_log << L"Warning: No filename found for class " << classname
            << L": File component in Rose class unchanged" << endl;
  }
  else {
    // Algorithm is that any file names in classFiles that are already
    // external documents for UML class c are removed from
    // classFiles. This way only new file names are added as external
    // documents.


    // First remove those file names that are already external documents
    IRoseExternalDocumentCollection externalDocs = c.GetExternalDocuments();
    int n = externalDocs.GetCount();
    for (int i = 1; i <= n; i++) {
      IRoseExternalDocument ed = externalDocs.GetAt(i);
      if(!ed.IsURL()) {
        CString path(ed.GetPath());
        TYPE_ProjectTypes_FileName fileName (PTAUX::mk_FileName(wstring((LPCTSTR) path)));
        if (classFiles.InSet(fileName))
          classFiles.RemElem(fileName);
      }
    }

    // Now add the remaining file names in class files as external
    // documents
    Generic g;
    for (bool bb = classFiles.First(g); bb; bb = classFiles.Next(g)) {
      wstring s (PTAUX::ExtractFileName(g));
      CString filenameString (s.c_str());

      // Value 1 indicates path, 2 indicates URL
      c.AddExternalDocument(filenameString, 1);
    }
  }
}



void RoseInterface::SetMethodParameters(IRoseOperation op, const SEQ<TYPE_AUML_PatType> & pattype_l)
  // Sets the parameters of operation op as defined in sequence
  // pattype_l
{
  int n = 1;
  // First, remove existing parameters. Of course if we wanted
  // to be really slick, we would only remove parameters that
  // are no longer in the signature, but there is no point
  // in being that complicated.
  op.RemoveAllParameters();

  // Now add all of the items in the given sequence.
  int len = pattype_l.Length();
  for (int i = 1; i <= len; i++)
  {
    TYPE_AUML_PatType pt (pattype_l[i]);
    op.AddParameter(MPP::MiniPP(pt.get_pat()).c_str(),
                    MPP::MiniPP(pt.get_tp()).c_str(),
                    L"", n++);
  }
}



// The following methods deal with all kinds of relations between
// classes, i.e. creation/modification of inheritance relations and
// associations.


void RoseInterface::UpdateInheritance(const TYPE_AUML_Inheritance & inh_m, const TYPE_AUML_Inheritance & cur_inh_m)
  // Creates new and removes obsolete inheritance relations between classes.
{
  Generic gc;
  IRoseCollection classes = theModel.GetAllClasses(), super;
  Set dom_inh_m (inh_m.Dom()), dom_cur_inh_m (cur_inh_m.Dom());
  Set common_dom (dom_inh_m), inter_dom (dom_inh_m);
  common_dom.ImpUnion(dom_cur_inh_m);
  inter_dom.ImpIntersect(dom_cur_inh_m);

  // Since this is called after CreateClasses, any class in
  // dom cur_inh_m \ dom_inh_m has already been removed from
  // the Rose repository. Thus we only need worry about dom_inh_m

  int n = dom_inh_m.Card(), j = 0;
  wstring label(L"Updating Inheritance Relations");
  for (bool bb = dom_inh_m.First(gc); bb; bb = dom_inh_m.Next(gc)) {
    ToolMediator::Interf()->vdm_UpdateMeter((float)++j/n,
                              (SEQ<Char>)Sequence(label));
    CString clnm (seq2cstr(Sequence(gc)));
    IRoseClass c;
    if ((c = FindByName(clnm, classes)) == NULL) {
      vdm_log << L"Warning: Unable to find " << wstring(clnm).c_str()
              << L" to add inheritance relation" << endl;
    }
    else if (inter_dom.InSet(gc)) {
      SET<TYPE_AUML_Name> add (inh_m[gc]),
                          del(cur_inh_m[gc]);
      add.ImpDiff(cur_inh_m[gc]);
      if(!add.IsEmpty())
        AddInheritance(add, c);

      del.ImpDiff(inh_m[gc]);
      if(!del.IsEmpty())
        DeleteInheritance(del, c);
    }
    else { // inh_m_Dom.InSet(gc) must be true
      SET<TYPE_AUML_Name> add (inh_m[gc]);
      if(!add.IsEmpty())
        AddInheritance(add, c);
    }
  }
}

// SelectForDeletion
int RoseInterface::SelectForDeletion(IRoseCollection l,
                                     const SET<TYPE_AUML_Name> nm_s,
                                     LPDISPATCH *tbd, int mode, IRoseClass cc)
  // Inserts references to items which are in set nm_s into the table
  // tbd.
{
  int n = l.GetCount(), j = 0;
  IRoseRole role;
  for (int i = 1; i <= n; i++) {
    switch(mode) {
      case RI_INHERITANCE: {
        if(nm_s.InSet(cstr2seq(IRoseInheritRelation(l.GetAt(i)).GetSupplierName())))
          tbd[j++] = l.GetAt(i);
        break;
      }
      case RI_ITEM: {
        if(nm_s.InSet(cstr2seq(IRoseItem(l.GetAt(i)).GetName())))
          tbd[j++] = l.GetAt(i);
        break;
      }
      case RI_ASSOCIATION: {
        role = IRoseAssociation(l.GetAt(i)).GetOtherRole(cc);
        if(role.GetName() != L"" &&  nm_s.InSet(cstr2seq(role.GetClassName())) )
          tbd[j++] = l.GetAt(i);
        break;
      }
      default: {
        //      RunTimeError(L"SelectForDeletion", "Illegal mode", Generic());
        break;
      }
    }
  }
  return j;
}



void RoseInterface::DeleteInheritance(const SET<TYPE_AUML_Name> & del, IRoseClass c)
  // Removes obsolete inheritance relations
{
  IRoseCollection inh_l = c.GetInheritRelations();
  LPDISPATCH *tbd = new LPDISPATCH[inh_l.GetCount()];
  int j = SelectForDeletion(inh_l, del, tbd, RI_INHERITANCE,c );

  for(int i = 0; i < j; i++)
    c.DeleteInheritRel(IRoseInheritRelation(tbd[i]));
  delete[] tbd;
}

void RoseInterface::DeleteAssociations(const TYPE_AUML_Associations & del, IRoseCollection classes)
{
  wstring label(L"Deleting Obsolete Associations");
  int n = del.Size(), j = 0;
  Set dom_del (del.Dom());
  Generic g;
  for (bool bb = dom_del.First(g); bb; bb = dom_del.Next(g)) {
    ToolMediator::Interf()->vdm_UpdateMeter((float)++j/n,
                              (SEQ<Char>)Sequence(label));
    CString clnm (seq2cstr(Sequence(g)));
    IRoseClass c;
    if ((c = FindByName(clnm, classes)) == NULL) {
      vdm_log << L"Warning: Unable to find " << wstring(clnm).c_str()
              << L" to delete associations" << endl;
    }
    else
      DeleteUniAssociations(del[g], c);
  }
}

void RoseInterface::DeleteUniAssociations(const TYPE_AUML_UniAssoc & del, IRoseClass c)
  // Deletes obsolete associations.
{
  IRoseCollection ass_l = c.GetAssociations();
  LPDISPATCH *tbd = new LPDISPATCH[del.Size()];
  int j = 0;
 Set dom_del (del.Dom());
  Generic g;
  for (bool bb = dom_del.First(g); bb; bb = dom_del.Next(g)) {
    TYPE_AUML_Role auml_role (del[g]);
    CString id_str (seq2cstr(auml_role.get_id()));
    IRoseRole rose_role (IRoseAssociation(ass_l.GetWithUniqueID(id_str)).GetOtherRole(c));

    if(rose_role.GetName() != L"")
    {
      if(auml_role.get_server() == cstr2seq(rose_role.GetClassName()))
      {
        // OK to remove association. Role name and server name match.
        tbd[j++] = ass_l.GetWithUniqueID(id_str);

        // Note here that the Rose Extensibility interface seems to
        // require a reference to the association directly yielded by
        // ass_l. That is, if the association is stored in a temporary
        // variable and then this is assigned to tbd[j++], an
        // exception is raised when this association is deleted.
        // PM 270400
      }
    }
  }


  for(int i = 0; i < j; i++)
      c.DeleteAssociation(IRoseAssociation(tbd[i]));

  delete[] tbd;
}

void RoseInterface::AddInheritance(const SET<TYPE_AUML_Name> & add_, IRoseClass c)
  // Adds inheritance from class c towards the classes in set add
{
  Set add (add_);
  Generic gc;
  for (bool bb = add.First(gc); bb; bb = add.Next(gc))
    c.AddInheritRel(L"", seq2cstr(Sequence(gc)));
}


void RoseInterface::UpdateAssociations(const TYPE_AUML_Associations & ass_m, const TYPE_AUML_Associations & cur_ass_m)
  // ass_m is the new associations, cur_ass_m is the map defining the
  // associations currently defined in Rose.
{
  IRoseCollection classes = theModel.GetAllClasses(), super;
  SET<TYPE_AUML_Name> dom_new (ass_m.Dom().ImpDiff(cur_ass_m.Dom())),
                      dom_old (cur_ass_m.Dom().ImpDiff(ass_m.Dom())),
                      dom_common (ass_m.Dom().ImpIntersect(cur_ass_m.Dom()));

  Generic g;
  TYPE_AUML_Associations new_assocs;
  for (bool bb = dom_new.First(g); bb; bb = dom_new.Next(g)) {
    new_assocs.Insert(g, ass_m[g]);
  }

  TYPE_AUML_Associations obsolete_assocs;
  for (bool cc = dom_old.First(g); cc; cc = dom_old.Next(g)) {
    obsolete_assocs.Insert(g, cur_ass_m[g]);
  }

  TYPE_AUML_Associations modified_assocs;
  wstring label = L"Calculating new associations...";
  int n = dom_common.Card();
  int j = 0;

  for (bool dd = dom_common.First(g); dd; dd = dom_common.Next(g)) {
    ToolMediator::Interf()->vdm_UpdateMeter((float)++j/n,
                              (SEQ<Char>)Sequence(label));
    if (ass_m[g] != cur_ass_m[g]) {
      Generic h;

      TYPE_AUML_UniAssoc g_uniassoc(ass_m[g]),
                         cur_g_uniassoc(cur_ass_m[g]);

      SET<TYPE_AUML_Name>
          dom_newu (g_uniassoc.Dom().ImpDiff(cur_g_uniassoc.Dom())),
          dom_oldu (cur_g_uniassoc.Dom().ImpDiff(g_uniassoc.Dom())),
          dom_commonu (g_uniassoc.Dom().ImpIntersect(cur_g_uniassoc.Dom()));

      TYPE_AUML_UniAssoc new_uniassocs;
      for (bool j1 = dom_newu.First(h); j1; j1 = dom_newu.Next(h))
        new_uniassocs.Insert(h, g_uniassoc[h]);

      TYPE_AUML_UniAssoc obsolete_uniassocs;
      for (bool j2 = dom_oldu.First(h); j2; j2 = dom_oldu.Next(h))
        obsolete_uniassocs.Insert(h, cur_g_uniassoc[h]);

      TYPE_AUML_UniAssoc modified_uniassocs;
      for (bool j3 = dom_commonu.First(h); j3; j3 = dom_commonu.Next(h)) {
        TYPE_AUML_Role new_rl (g_uniassoc[h]),
                       old_rl (cur_g_uniassoc[h]);
        if (new_rl.get_server() != old_rl.get_server()) {
          obsolete_uniassocs.Insert(h, cur_g_uniassoc[h]);
          new_uniassocs.Insert(h, g_uniassoc[h]);
        }
        else
          modified_uniassocs.Insert(h, g_uniassoc[h]);
      }

      new_assocs.Insert(g, new_uniassocs);
      obsolete_assocs.Insert(g, obsolete_uniassocs);
      modified_assocs.Insert(g, modified_uniassocs);
    }
  }

  DeleteAssociations(obsolete_assocs, classes);
  AddAssociations(new_assocs, classes);
  ModifyAssociations(modified_assocs, cur_ass_m, classes);
}

void RoseInterface::RemoveKeys(IRoseRole role)
  // Removes all keys (qualifiers) defined for the role.
{
  IRoseCollection rl = role.GetKeys();
  while(rl.GetCount()){
    role.DeleteKey(rl.GetAt(1));
    rl = role.GetKeys();
  }
}

void RoseInterface::AddAssociations(const TYPE_AUML_Associations & add, IRoseCollection classes)
{
  wstring label(L"Adding New Associations");
  int n = add.Size(), j=0;
  Set dom_add (add.Dom());
  Generic g;
  for (bool bb = dom_add.First(g); bb; bb = dom_add.Next(g)) {
    ToolMediator::Interf()->vdm_UpdateMeter((float)++j/n,
                              (SEQ<Char>)Sequence(label));
    CString clnm (seq2cstr(Sequence(g)));
    IRoseClass c;
    if ((c = FindByName(clnm, classes)) == NULL) {
      vdm_log << L"Warning: Unable to find " << wstring(clnm).c_str()
              << L" to add associations" << endl;
    }
    else
      AddUniAssociations(add[g], c);
  }
}

void RoseInterface::AddUniAssociations(const TYPE_AUML_UniAssoc & add, IRoseClass c)
  // Adds associations from class c towards the classes in set add
{
  IRoseAssociation assoc;
  IRoseRole role;
  Set dom_add (add.Dom());
  Generic gc;
  for (bool bb = dom_add.First(gc); bb; bb = dom_add.Next(gc))
  {
    TYPE_AUML_Role rl = add[gc];
    assoc = c.AddAssociation(L"", seq2cstr(rl.get_server()));
    role = assoc.GetOtherRole(c);

    // Set the role name:
    CString rlnm = seq2cstr(Sequence(gc));
    role.SetName(rlnm);

    if(Quote(rl.get_role_ut()) == RI_ORDERED_Q) {
      // Add an {ordered} constraint to the role:
      role.SetConstraints(RI_ORDERED);
    }

    // Set export control (access modifier)
    IRoseRichType rtp = role.GetExportControl();
    CString acc = MapAUMLAccessToRose(rl.get_acc());
    rtp.SetName(acc);

    // Set static property
    if (rl.get_static().GetValue())
      role.SetStatic(ROSE_STATIC);


    role.SetCardinality(card2wstring(rl.get_cd()));
    RemoveKeys(role);
    if(!rl.get_quali().IsNil()){
      TYPE_AS_Type tp = rl.get_quali();
      role.AddKey(MPP::MiniPP(tp).c_str(), L"");
    }
    role = assoc.GetCorrespondingRole(c);
    role.SetNavigable(FALSE);
  }
}

void RoseInterface::ModifyAssociations(const TYPE_AUML_Associations & modified,
                                       const TYPE_AUML_Associations & orig,
                                       IRoseCollection classes)
{
  wstring label(L"Modifying Existing Associations");
  int n = modified.Size(), j = 0;
  Set dom_modified (modified.Dom());
  Generic g;
  for (bool bb = dom_modified.First(g); bb; bb = dom_modified.Next(g)) {
    ToolMediator::Interf()->vdm_UpdateMeter((float)++j/n,
                              (SEQ<Char>)Sequence(label));
    CString clnm (seq2cstr(Sequence(g)));
    IRoseClass c;
    if ((c = FindByName(clnm, classes)) == NULL) {
      vdm_log << L"Warning: Unable to find " << wstring(clnm).c_str()
              << L" to delete associations" << endl;
    }
    else
      ModifyUniAssociations(modified[g], orig[g], c);
  }
}


// For those associations in both the old and new models which
// have been changed, modify them accordingly.
void RoseInterface::ModifyUniAssociations(const TYPE_AUML_UniAssoc & modified,
                                          const TYPE_AUML_UniAssoc & orig,
                                          IRoseClass c)
  // pre: dom modified subset dom orig
{
  IRoseCollection ass_l = c.GetAssociations();
  int n = ass_l.GetCount();

  // For each role modified in the merged model, modify
  // corresponding role in the original UML model
  Set dom_modified (modified.Dom());
  Generic gc;
  for (bool bb = dom_modified.First(gc); bb; bb = dom_modified.Next(gc))
  {
    TYPE_AUML_Role new_rl = modified[gc],
                   old_rl = orig[gc];

    IRoseRole role;
    int j = 1;
    bool found = false;

    // Find the UML role corresponding to this abstract role
    while((j <= n) && !found)
    {
      role = IRoseAssociation(ass_l.GetAt(j)).GetOtherRole(c);
      found = (GetRoleName(role.GetName()) == seq2cstr(Sequence(gc)));
      j++;
    }

    if(found)
    {
      // Modify those fields in the new role that differ
      // from the original

      // Role
      if(new_rl.get_role_ut() != old_rl.get_role_ut())
      {
        if (Quote(new_rl.get_role_ut()) == RI_ORDERED_Q)
          role.SetConstraints(RI_ORDERED);
        else
          role.SetConstraints((LPCTSTR)L"");
      }

      // Cardinality
      if(new_rl.get_cd() != old_rl.get_cd())
        role.SetCardinality(card2wstring(new_rl.get_cd()));

      // Access modifier
      if (new_rl.get_acc() != old_rl.get_acc())
      {
        // Set export control (access modifier)
        IRoseRichType rtp = role.GetExportControl();
        CString acc = MapAUMLAccessToRose(new_rl.get_acc());
        if (!((rtp.GetName() == ROSE_IMPLEMENTATION) &&
          (acc == ROSE_PRIVATE)) && (acc != L""))
          rtp.SetName(acc);
      }

      if (new_rl.get_static().GetValue())
        role.SetStatic(ROSE_STATIC);

      //Qualifiers
      if(new_rl.get_quali() != old_rl.get_quali())
      {
        RemoveKeys(role);
        if(!new_rl.get_quali().IsNil())
        {
          TYPE_AS_Type tp = new_rl.get_quali();
          role.AddKey(MPP::MiniPP(tp).c_str(), L"");
        }
      }
    }
    else
    {
      // Panic! Can't find this association in the Rose Repository,
      // even though it is in the merged model.
      CString str = L"Unable to find association " + seq2cstr(Sequence(gc));
      DisplayWarning(c.GetName(), str);
    }
  }
}

void RoseInterface::RemoveObsoleteAttributes(IRoseClass c, const Set & obsolete_attributes)
{
  Generic gc;
  IRoseCollection atts = c.GetAttributes();
  IRoseAttribute att;

  for (bool bb = obsolete_attributes.First(gc); bb;
            bb = obsolete_attributes.Next(gc)) {
    CString id_str (seq2cstr(Sequence(gc)));
    att = atts.GetWithUniqueID(id_str);
    if (att)
      c.DeleteAttribute(att);
    else
      vdm_log << L"Warning: unable to delete attribute id "
              << wstring(id_str).c_str() << endl;
  }
}


// Remove all those methods in class c which do not appear in the
// new model
void RoseInterface::RemoveObsoleteMethods(IRoseClass c, const SET<TYPE_AUML_RoseId> & obsolete_methods)
{
  // This function goes through each method in obsolete_methods, and
  // checks to see whether there is a method in c with the same name.
  // If so, that method is deleted from c.
  Generic gc;
  IRoseOperation op;
  IRoseCollection ops = c.GetOperations();

  // Go through each name in the set of method names
  for (bool bb = obsolete_methods.First(gc); bb;
           bb = obsolete_methods.Next(gc))
  {
    CString id_str (seq2cstr(Sequence(gc)));
    op = ops.GetWithUniqueID(id_str);
    if (op)
      c.DeleteOperation(op);
    else
      vdm_log << L"Warning: unable to delete operation id "
              << wstring(id_str).c_str() << endl;
  }
}

void RoseInterface::DeleteAllAttributes(IRoseClass c)
  // Removes all attributes in c
{
  IRoseCollection atts = c.GetAttributes();
  int n = atts.GetCount();
  LPDISPATCH *tbd = new LPDISPATCH[n];
  for (int i = 1; i <= n; i++)
    tbd[i-1] = atts.GetAt(i);
  for(int j = 0; j < n; j++) {
    IRoseAttribute att = tbd[j];
    c.DeleteAttribute(att);
  }
  delete[] tbd;
}


void RoseInterface::DeleteAllOperations(IRoseClass CurrentClass)
  // Removes all operations in CurrentClass
{
  IRoseCollection ops = CurrentClass.GetOperations();
  int n = ops.GetCount();
  LPDISPATCH *tbd = new LPDISPATCH[n];
  for(int i = 1; i <= n; i++)
    tbd[i-1] = ops.GetAt(i);
  for(int j = 0; j < n; j++) {
    IRoseOperation op = tbd[j];
    CurrentClass.DeleteOperation(op);
  }
  delete[] tbd;
}


void RoseInterface::UpdateInstVars(IRoseClass CurrentClass,
                                   const TYPE_AUML_InstanceVars & inst_m,
                                   const TYPE_AUML_InstanceVars & old_m)
{
  // Restrict interest to those instance variables which
  // were not in the original model, or have been updated since then
  Map new_instvars = vdm_MERGE_MapDiff((Generic)inst_m,
                                       (Generic)old_m);
  Set name_s (new_instvars.Dom());
  Generic gc;

  IRoseCollection atts = CurrentClass.GetAttributes();
  for(bool bb = name_s.First(gc); bb; bb = name_s.Next(gc)) {
    Sequence s(gc);
    TYPE_AUML_ValDef iv_def = new_instvars[s];
    IRoseAttribute att;

    // Need to consider whether this is a new name,
    // or a modification of an existing name.

    if (iv_def.get_id().IsNil()) {
      att = CurrentClass.AddAttribute(seq2cstr(s),
                                    MPP::MiniPP(iv_def.get_tp()).c_str(),
                                    MPP::MiniPP(iv_def.get_expr()).c_str());
    }
    else {
      CString id_str (seq2cstr(iv_def.get_id()));
      att = atts.GetWithUniqueID(id_str);
      if (att){
        att.SetType(MPP::MiniPP(iv_def.get_tp()).c_str());
        att.SetInitValue(MPP::MiniPP(iv_def.get_expr()).c_str());
      }
      else
        vdm_log << L"Warning: unable to modify instance variable id "
                << id_str << endl;
    }

    if (iv_def.get_static().GetValue())
      att.SetStatic(ROSE_STATIC);

    // Set the sterotype to be an instance variable. Thus
    // an attribute which was a value in the original model
    // is modified to be an instance variable.
    att.SetStereotype(ROSE_INSTANCE_VAR);

    // Set the export control:
    IRoseRichType rtp = att.GetExportControl();
    CString acc = MapAUMLAccessToRose(iv_def.get_acc());
    if (!((rtp.GetName() == ROSE_IMPLEMENTATION) &&
          (acc == ROSE_PRIVATE)) && (acc != L""))
      rtp.SetName(acc);
  }
}

// Updates attributes corresponding to value definitions in the VDM++ model.
// This is similar to UpdateInstVars, but is sufficiently different to
// warrant a separate function.
void RoseInterface::UpdateValues(IRoseClass CurrentClass,
                                 const TYPE_AUML_ValuesDef & val_m,
                                 const TYPE_AUML_ValuesDef & old_m)
{
  // Restrict interest to those values which were not in the
  // original model, or have been updated since then
  Map new_values = vdm_MERGE_MapDiff((Generic)val_m,
                                     (Generic)old_m);
  Set name_s (new_values.Dom());
  IRoseCollection atts = CurrentClass.GetAttributes();

  Generic gc;
  for (bool bb = name_s.First(gc); bb; bb = name_s.Next(gc)) {

    // The truncation of expressions is commented out until we come up
    // with a better way to deal with long expressions:
    // ... comments deleted by PM 060799

    TYPE_AUML_ValDef v_def (new_values[gc]);
    IRoseAttribute att;

    if (v_def.get_id().IsNil()) {
      att = CurrentClass.AddAttribute(
                                MPP::MiniPP(gc).c_str(),             //AS`Pattern
                                MPP::MiniPP(v_def.get_tp()).c_str(), //AS`Type
                                MPP::MiniPP(v_def.get_expr()).c_str());
    }
    else {
      CString id_str (seq2cstr(v_def.get_id()));
      att = atts.GetWithUniqueID(id_str);
      if (att) {
        att.SetType(MPP::MiniPP(v_def.get_tp()).c_str());
        att.SetInitValue(MPP::MiniPP(v_def.get_expr()).c_str());
      }
      else {
        vdm_log << L"Warning: unable to modify value id "
                << id_str << endl;
        return;
      }
    }
    if (v_def.get_static().GetValue())
      att.SetStatic(ROSE_STATIC);


    // Set the sterotype to be a value. Thus an attribute which
    // was an instance variable in the original model is modified
    // to be a value.
    att.SetStereotype(ROSE_VALUE);

    // Set the export control:
    IRoseRichType rtp = att.GetExportControl();
    CString acc = MapAUMLAccessToRose(v_def.get_acc());
    if (!((rtp.GetName() == ROSE_IMPLEMENTATION) &&
          (acc == ROSE_PRIVATE)) && (acc != L""))
      rtp.SetName(acc);
  }
}

void RoseInterface::UpdateMethods(IRoseClass CurrentClass,
                                  const TYPE_AUML_CommonSign & meth_m,
                                  const TYPE_AUML_CommonSign & old_m)
{
  AddOperations(CurrentClass, meth_m, old_m, ROSE_METH);
}

void RoseInterface::UpdateFunctions(IRoseClass CurrentClass,
                                    const TYPE_AUML_CommonSign & fn_m,
                                    const TYPE_AUML_CommonSign & old_m)
{
  AddOperations(CurrentClass, fn_m, old_m, ROSE_FUNC);
}

void RoseInterface::AddOperations(IRoseClass CurrentClass,
                                  const TYPE_AUML_CommonSign & m,
                                  const TYPE_AUML_CommonSign & old_m,
                                  wchar_t * stype)
  // Inserts UML operations representing either operations
  // or functions in VDM++.
{
  // Restrict interest to those operations which are new
  // or have been modified with respect to the original model
  Map new_ops (vdm_MERGE_MapDiff((Generic)m, (Generic)old_m));
  IRoseCollection ops = CurrentClass.GetOperations();
  int param, rttype;
  Set dom_new_ops (new_ops.Dom());
  Generic gc;
  for (bool bb = dom_new_ops.First(gc); bb; bb = dom_new_ops.Next(gc)) {
    TYPE_AUML_Signature sign(new_ops[gc]);

    IRoseOperation theOp;

    if (sign.get_id().IsNil()) {
      Sequence unMangledSeq (MANGLE::unmangleSeqStem(Sequence(gc)));
      theOp = CurrentClass.AddOperation(seq2cstr(unMangledSeq), MPP::MiniPP(sign.get_returntype()).c_str());
    }
    else {
      CString id_str (seq2cstr(sign.get_id()));
      theOp = ops.GetWithUniqueID(id_str);
      if (theOp) {
        theOp.SetReturnType(MPP::MiniPP(sign.get_returntype()).c_str() );
      }
      else {
        vdm_log << L"Warning: unable to modify operation id " << id_str << endl;
        return;
      }
    }

    SetMethodParameters(theOp, sign.get_param());
    theOp.SetStereotype(stype);

    // Add pre and postconditions
    CString s = MPP::MiniPP(sign.get_precond()).c_str();
    theOp.SetPreconditions(MPP::MiniPP(sign.get_precond()).c_str() );
    theOp.SetPostconditions(MPP::MiniPP(sign.get_postcond()).c_str() );

    // Set the export control:
    IRoseRichType rtp = theOp.GetExportControl();
    CString acc = MapAUMLAccessToRose(sign.get_acc());
    if (!((rtp.GetName() == ROSE_IMPLEMENTATION) &&
          (acc == ROSE_PRIVATE)) && (acc != L""))
      rtp.SetName(acc);
  }
}

// #####################################################################
// ##  END OF Functions implementing the mapping from AUML to UML.    ##
// #####################################################################




// #####################################################################
// ##  Functions implementing the mapping from UML to AUML.           ##
// #####################################################################

TYPE_AUML_Classes RoseInterface::GenClassMap(const Bool & ReportErr, const TYPE_AUML_Classes & vpp_m)
  // Generates a map from class names to class definitions, i.e.:
  // map Name to Class
{
  IRoseCollection classes = theModel.GetAllClasses();
  IRoseClass c;
  TYPE_AUML_Classes class_map;
  int n = classes.GetCount();
  for (int i = 1; i <= n; i++) {
    c = classes.GetAt(i);
    wstring label(L"Reading ");
    CString nm = c.GetName();

    label += wstring(nm);
    ToolMediator::Interf()->vdm_UpdateMeter(Real((float)(i-1)/n), (Generic)Sequence(label));


    if(!UMLPARSE::parse_identifier((LPCTSTR)nm) && ReportErr){
      CString str = L"Syntax error in name of class: "
        + nm + L"\nClass ignored.";
      DisplayWarning(nm, str);
    }
    else {
      // Get instance variables, values and time variables in one go:
      TYPE_AUML_InstanceVars iv_m;
      TYPE_AUML_ValuesDef val_m;
      ParseAttributes(c, nm, iv_m, val_m, ReportErr);

      TYPE_AUML_Class cl;
      cl.set_inst(iv_m);
      cl.set_val(val_m);
      cl.set_time(tv_m);

      TYPE_AUML_CommonSign f_m, o_m;
      ParseOperations(c, nm, o_m, f_m, ReportErr);
      cl.set_meth(o_m);
      cl.set_func(f_m);

      // Get abstract value
      cl.set_abstract(Bool(c.GetAbstract()));

      // Get id
      cl.set_id(cstr2seq(c.GetUniqueID()));

      // Put the class information into the map...
      //!!!      class_map.Insert(cstr2seq(nm), cl);
      if(!class_map.DomExists(cstr2seq(nm)))
        class_map.ImpModify(cstr2seq(nm), cl);
      else vdm_err << L"Class is already added " << cstr2seq(nm) << endl << flush;
    }
  }
  return class_map;
}


CString RoseInterface::Operation2Cstr(CString nm, CString type, IRoseCollection args)
  // Returns a wstring representing the operation. Used when
  // displaying warnings to the user.
{
  CString res = nm + L"(";
  int n = args.GetCount();
  IRoseParameter pm;
  for(int i = 1; i <= n; i++) {
    pm = args.GetAt(i);
    res += pm.GetName() + L" : " + pm.GetType();
    res += i == n ? L"" : L", ";
  }
  res += L")";
  res += type == L"" ? "" : " : " + type;
  return res;
}

void RoseInterface::ParseValue(CString c_nm,
                               TYPE_AUML_ValuesDef &val_m,
                               CString nm,
                               CString type,
                               CString initval,
                               CString acc,
                               BOOL stat,
                               CString id,
                               const Bool & ReportErr)
{
  TYPE_AUML_ValDef val;
  TYPE_AS_Pattern pat;
  if(UMLPARSE::str2pat((LPCTSTR)nm, pat) &&
     ParseValuePart((LPCTSTR)type, (LPCTSTR)initval, val) ) {
    // The value definition is syntax correct:
    if(val_m.Dom().InSet(pat)){
      CString str = L"Value " + nm +
        L" was previously defined in this class. Ignoring value definition.";
      DisplayWarning(c_nm, str);
    }
    else {
      val.set_acc(MapRoseAccessToAUML(acc));
      val.set_id(cstr2seq(id));
      val.set_static(BOOL2bool(stat));
      val_m.Insert(pat, val);
    }
  }
  else if (ReportErr) {
    CString str = L"Syntax error in value definition:\n"
      + Attribute2Cstr(nm, type, initval) + L"\nValue ignored.";
    DisplayWarning(c_nm, str);
  }
}

Quote RoseInterface::MapRoseAccessToAUML(const CString & acc)
{
  if (acc == ROSE_PUBLIC)
    return QUOTE_PUBLIC;
  if ((acc == ROSE_PRIVATE) || (acc == ROSE_IMPLEMENTATION))
    return QUOTE_PRIVATE;
  if (acc == ROSE_PROTECTED)
    return QUOTE_PROTECTED;
  return QUOTE_PUBLIC; // dummy
}


CString RoseInterface::MapAUMLAccessToRose(const Quote & acc)
{
  if (acc == QUOTE_PUBLIC)
    return CString(ROSE_PUBLIC);
  if (acc == QUOTE_PRIVATE)
    return CString(ROSE_PRIVATE);
  if (acc == QUOTE_PROTECTED)
    return CString(ROSE_PROTECTED);
  return L""; // dummy
}


void RoseInterface::ParseInstanceVar(const CString & c_nm,
                                     TYPE_AUML_InstanceVars &iv_m,
                                     const CString & nm,
                                     const CString & type,
                                     const CString & initval,
                                     const CString & acc,
                                     const BOOL & stat,
                                     const CString & id,
                                     const Bool & ReportErr)
{
  TYPE_AUML_ValDef val;
  if(UMLPARSE::parse_identifier((LPCTSTR)nm) &&
     !type.IsEmpty() &&
     ParseInstVarInit((LPCTSTR)type, (LPCTSTR)initval, val) ) {
    // The instance variable is syntax correct:
    if(iv_m.Dom().InSet(cstr2seq(nm))) {
      CString str = L"Instance variable " + nm +
        L" was previously defined in this class. Ignoring instance variable.";
      DisplayWarning(c_nm, str);
    }
    else {
      val.set_acc(MapRoseAccessToAUML(acc));
      val.set_id(cstr2seq(id));
      val.set_static(BOOL2bool(stat));
      iv_m.Insert(cstr2seq(nm), val);
    }
  }
  else if (ReportErr) {
    CString str = L"Syntax error in instance variable definition:\n"
      + Attribute2Cstr(nm, type,initval) + L"\nInstance variable ignored.";
    DisplayWarning(c_nm, str);
  }
}

void RoseInterface::ParseSignature(CString c_nm,
                                   int state,
                                   TYPE_AUML_CommonSign &m_m,
                                   CString nm,
                                   CString type,
                                   IRoseCollection param,
                                   CString precondstr,
                                   CString postcondstr,
                                   CString acc,
                                   CString id,
                                   const Bool & ReportErr)
  // Parses the signatures of operations and functions. The argument
  // state indicated whether it is an operation or a function.
{
  int success = UMLPARSE::parse_identifier((LPCTSTR)nm),
    n = param.GetCount(),
    i=1;
  CString m_type = (state == FUNCTION_DEF) ? L"Function" : L"Operation";

  SEQ<TYPE_AUML_PatType> pt_l; // Sequence of PatType
  SEQ<TYPE_AS_Type> argTps;    // Sequence of argument types used for mangling

  if((state == FUNCTION_DEF) && (type == L"")) {
    // Check if the return type of functions is empty --> error
    CString str = L"Function " + nm +
      L" must have a returntype. Function ignored:\n" +
      Operation2Cstr(nm, type, param);
    DisplayWarning(c_nm, str);
  }
  else {
    while(success && i<=n) {
      IRoseParameter par = param.GetAt(i);
      TYPE_AS_Pattern pat;
      TYPE_AS_Type as_tp;
      TYPE_AUML_PatType pt;
      // Parse each individual argument
      success = success &&
                UMLPARSE::str2pat((LPCTSTR)par.GetName(), pat) &&
                UMLPARSE::str2type((LPCTSTR)par.GetType(), as_tp);

      if(success) {
        pt.set_pat(pat);
        pt.set_tp(as_tp);
        pt_l.ImpAppend(pt);
        argTps.ImpAppend(as_tp);
      }
      i++;
    }

    // Parse the return type:
    TYPE_AS_Type ret_tp;
    if(type != L"") {
      TYPE_AS_Type tp;
      success = success && UMLPARSE::str2type((LPCTSTR)type, tp);
      ret_tp = tp;
    }
    else
      ret_tp = TYPE_AS_VoidType().Init(Int(-1));

    // Parse precondition
    Generic precond;
    if(precondstr==L"")
    {
      precond = Nil();
    }
    else
    {
      TYPE_AS_Expr ex;
      success = success && UMLPARSE::str2expr((LPCTSTR)precondstr, ex);
      precond = ex;
    }

    // Parse postcondition
    Generic postcond;
    if(postcondstr==L"")
    {
      postcond = Nil();
    }
    else
    {
      TYPE_AS_Expr ex;
      success = success && UMLPARSE::str2expr((LPCTSTR)postcondstr, ex);
      postcond = ex;
    }

    if(success){
      wstring mangledString (MANGLE::MangleString((LPCTSTR) nm, argTps));
      CString uniqueName = (LPCTSTR) (mangledString).c_str();
      if(!m_m.Dom().InSet(cstr2seq(uniqueName))){
        TYPE_AUML_Signature sign;
        sign.set_param(pt_l);
        sign.set_returntype(ret_tp);
        sign.set_precond(precond);
        sign.set_postcond(postcond);
        // The following is the default settings for the don't cares
        // of operations and functions. The default value is
        // automatically overridden by the value from the VDM++
        // specificaton if a function of the same name is defined
        // there. See module MERGE for further details.
        sign.set_kind(RI_EXPL_Q);
        sign.set_static(Bool(false));
        // Only makes sense for functions:
        sign.set_fntype(RI_TOTAL_Q);
        sign.set_acc(MapRoseAccessToAUML(acc));
        sign.set_id(cstr2seq(id));
        CString syntaxOkName;
        m_m.Insert(cstr2seq(uniqueName), sign);
      }
      else {
        CString str = m_type + L" " + nm + " was previously defined in this class. Ignoring " + m_type + ":\n";
        str += Operation2Cstr(nm, type, param);
        DisplayWarning(c_nm, str);
      }
    }
    else { // Errors during parsing:
      CString str = L"Ignoring " + m_type + " due to errors in its arguments or return type:\n";
      str += Operation2Cstr(nm, type, param);
      DisplayWarning(c_nm, str);
    }
  }
}

// The following functions handle mapping of associations and inheritance
// from UML to VDM++


TYPE_AUML_Associations RoseInterface::GenAssMap(const TYPE_AUML_Associations & a_m)
  // Traverses the associations defined in Rose and constructs a map
  // corresponding to this.
{
  TYPE_AUML_Associations ass_m;
  IRoseCollection classes = theModel.GetAllClasses();
  IRoseClass c;
  wstring label(L"Reading associations...");
  ToolMediator::Interf()->vdm_UpdateMeter(Real(0), (Generic)Sequence(label));
  int n = classes.GetCount();
  for(int i = 1; i <= n; i++) {
    c = classes.GetAt(i);
    ToolMediator::Interf()->vdm_UpdateMeter(Real((float)(i-1)/n), (Generic)Sequence(label));
    //!!!    ass_m.Insert(cstr2seq(c.GetName()),
    //             MkAssocMap(c.GetAssociations(), c, a_m));
    if(!ass_m.DomExists(cstr2seq(c.GetName()))) {
      ass_m.ImpModify(cstr2seq(c.GetName()),
                      MkAssocMap(c.GetAssociations(), c, a_m));
    } else {
      vdm_err << L"Class is already added " << cstr2seq(c.GetName()) << endl << flush;
    }
  }
  return ass_m;
}

TYPE_AUML_UniAssoc RoseInterface::MkAssocMap(IRoseCollection l, IRoseClass c, const TYPE_AUML_Associations & a_m)
  // Returns  map Name to Role, where Name is the role name and Role is a
  // Record representing the role.
{
  int m = l.GetCount();
  TYPE_AUML_UniAssoc am;
  for(int j = 1; j <= m; j++) {
    IRoseAssociation a = l.GetAt(j);
    if(a.GetName() != RI_TBD) {
      IRoseRole role = a.GetOtherRole(c);
      if(role.GetName() != L"") {
        TYPE_AUML_Name rlnm ((Generic)cstr2seq(GetRoleName(role.GetName())));
        if(!am.Dom().InSet(rlnm)) { // Does the role name already exist?
          if(UMLPARSE::parse_identifier((LPCTSTR)GetRoleName(role.GetName())))
            am.Insert(rlnm, MkRole(role, c, a_m));
          else {
            CString str = L"Invalid role name: " + role.GetName() +
              L"\nAssociation is ignored.";
            DisplayWarning(c.GetName(), str);
          }
        }
      }
    }
  }
  return am;
}



TYPE_AUML_Role RoseInterface::MkRole(IRoseRole r, IRoseClass c, const TYPE_AUML_Associations & a_m)
{
  TYPE_AUML_Role role;
  role.set_server(cstr2seq(r.GetClassName()));
  role.set_cd(string2card(r, c));
  role.set_quali(MkQualifier(r.GetKeys(), c, r));
  role.set_initval(Nil()); // The default init value for roles in Rose.
  role.set_tp(Nil());
  role.set_unftp(Nil());
  role.set_static(BOOL2bool(r.GetStatic()));

  IRoseRichType expc = r.GetExportControl();
  CString acc = Trim(expc.GetName());
  role.set_acc(MapRoseAccessToAUML(acc));

  CString id = Trim(IRoseAssociation(r.GetAssociation()).GetUniqueID());
  role.set_id(cstr2seq(id));

  if(IsOrdered(r.GetConstraints()))
    role.set_role_ut(RI_ORDERED_Q);
  else
    role.set_role_ut(Quote(L"NORMAL"));
  return role;
}

Generic RoseInterface::MkQualifier(IRoseCollection att, IRoseClass c, IRoseRole rl)
  // Returns the FIRST attribute in the collection as a qualifier.
  // If an attribute exists, its name is converted to AS`Type and returned.
  // Otherwise Nil is returned.
{
  TYPE_AS_Type tp;
  if(att.GetCount()) {
    //    if(!UMLPARSE::str2type((LPCTSTR)IRoseAttribute(att.GetAt(1)).GetType(), g)){
    if(UMLPARSE::str2type((LPCTSTR)IRoseAttribute(att.GetAt(1)).GetName(), tp))
      return (Generic) tp;
    else {
      CString str = L"Syntax error in the type of the qualifier for role: " +
        rl.GetName();
      DisplayWarning(c.GetName(), str);
      return Nil();
    }
  }
  else
    return Nil();
}



Quote RoseInterface::string2card(IRoseRole rl, IRoseClass c)
  // Translates the cardinality specifier s into the right Quote
{
  CString s = rl.GetCardinality();
  if(s==L"0..1")
    return Quote(L"ZERO_ONE");
  else if(s==L"0..n")
    return Quote(L"MANY");
  else if(s==L"1..n")
    return Quote(L"ONE_TO_MANY");
  else if(s==L"" || s=="1")
    return Quote(L"ONE");
  else{
    CString str = L"The cardinality specified for the role " + rl.GetName() +
      L" is not supported. Defaulting to a cardinality of 1..*.";
    DisplayWarning(c.GetName(), str);
    return Quote(L"MANY"); // (L"ONE_TO_MANY"); See item 3) http://zeus.ifad.dk/bugzilla/show_bug.cgi?id=3
  }
  return Quote();  // In order to keep the Microsoft compiler happy...
}

CString RoseInterface::card2wstring(Quote q)
{
  wstring c = q.GetValue();
  if(c==L"ONE")
    return CString(L"") ;
  else if(c==L"MANY")
    return CString(L"0..n");
  else if(c==L"ONE_TO_MANY")
    return CString(L"1..n");
  else if(c==L"ZERO_ONE")
    return CString(L"0..1");
  else
    return CString(L"");
}

TYPE_AUML_Inheritance RoseInterface::GenInhMap()
  // Generates a mapping representing the inheritance relations defined
  // inside RR.
{
  TYPE_AUML_Inheritance inh_m;
  IRoseCollection classes = theModel.GetAllClasses();
  IRoseClass c;
  int n = classes.GetCount();
  wstring label(L"Reading inheritance relations...");
  ToolMediator::Interf()->vdm_UpdateMeter(Real(0), (Generic)Sequence(label));
  for(int i = 1; i <= n; i++) {
    c = classes.GetAt(i);
    ToolMediator::Interf()->vdm_UpdateMeter(Real((float)(i-1)/n),
                              (Generic)Sequence(label));
    //!!!    inh_m.Insert(cstr2seq(c.GetName()), MkNameSet(c.GetSuperclasses()));
    if(!inh_m.DomExists(cstr2seq(c.GetName()))) {
      inh_m.ImpModify(cstr2seq(c.GetName()), MkNameSet(c.GetSuperclasses()));
    } else {
      vdm_err << L"Class is already added " << cstr2seq(c.GetName()) << endl << flush;
    }
  }
  return inh_m;
}


void RoseInterface::ParseAttributes(IRoseClass c, CString c_nm,
                                    TYPE_AUML_InstanceVars & iv_m,
                                    TYPE_AUML_ValuesDef & val_m,
                                    bool ReportErr)
{
  IRoseCollection atts = c.GetAttributes();

  int n = atts.GetCount();
  for (int i = 1; i <= n; i++) {
    int state = INSTANCE_VAR;  // The default.
    IRoseAttribute att = atts.GetAt(i);
    IRoseRichType expc = att.GetExportControl();
    CString acc     = Trim(expc.GetName()),
            stype   = Trim(att.GetStereotype()),
            nm      = Trim(att.GetName()),
            type    = Trim(att.GetType()),
            initval = Trim(att.GetInitValue()),
            id      = Trim(att.GetUniqueID());
    BOOL stat = att.GetStatic();

    if (nm == ROSE_INSTANCE_VAR_DEF)
      state = INSTANCE_VAR;
    else if (nm == ROSE_VALUES_DEF)
      state = VALUE_DEF;
    else if (nm == ROSE_TIME_VAR_DEF)
      state = TIME_VAR;
    else {
      // Now, check if stype holds a stereotype that will override the
      // stereotype defined in nm:
      if(stype == ROSE_INSTANCE_VAR)
        state = INSTANCE_VAR;
      else if(stype == ROSE_VALUE)
        state = VALUE_DEF;
      else if(stype == ROSE_TIME_VAR)
        state = TIME_VAR;

      switch (state) {
        case INSTANCE_VAR: {
          ParseInstanceVar(c_nm, iv_m, nm, type, initval, acc, stat, id, ReportErr);
          break;
        }
        case VALUE_DEF: {
          ParseValue(c_nm, val_m, nm, type, initval, acc, stat, id, ReportErr);
          break;
        }
        case TIME_VAR: {
          break;
        }
      }
    }
  }
}

void RoseInterface::ParseOperations(IRoseClass c,
                                    CString c_nm,
                                    TYPE_AUML_CommonSign & o_m,
                                    TYPE_AUML_CommonSign & f_m,
                                    const Bool & ReportErr)
{
  int state = OPERATION_DEF; // The default
  IRoseCollection ops = c.GetOperations();

  int n = ops.GetCount();
  for (int i = 1; i <= n; i++) {
    IRoseOperation op = ops.GetAt(i);
    CString stype = Trim(op.GetStereotype());
    IRoseRichType expc = op.GetExportControl();
    CString nm       = Trim(op.GetName()),
            type     = Trim(op.GetReturnType()),
            precond  = Trim(op.GetPreconditions()),
            postcond = Trim(op.GetPostconditions()),
            acc      = Trim(expc.GetName()),
            id       = Trim(op.GetUniqueID());
    if(nm == ROSE_METH_DEF)
      state = OPERATION_DEF;
    else if(nm == ROSE_FUNC_DEF)
      state = FUNCTION_DEF;
    else{
      // Now, check if stype holds a stereotype that will override the
      // stereotype defined in nm:
      if(stype == ROSE_METH)
        state = OPERATION_DEF;
      else if(stype == ROSE_FUNC)
        state = FUNCTION_DEF;

      switch (state) {
        case OPERATION_DEF: {
          // Check that an operation with the same name has not been defined
          // in the model already
          if (f_m.Dom().InSet(cstr2seq(nm)))
          {
            CString str = L"UML Method" + nm +
                          L"defined as both operation and function." +
                          L"Operation ignored.\n";
            DisplayWarning(c_nm, str);
          }
          else
            ParseSignature(c_nm, state, o_m, nm, type, op.GetParameters(),
                           precond, postcond, acc, id, ReportErr);
          break;
        }
        case FUNCTION_DEF: {
          // Check that a function with the same name has not been defined
          // in the model already
          if (o_m.Dom().InSet(cstr2seq(nm)))
          {
            CString str = L"UML Method" + nm +
                          L"defined as both operation and function." +
                          L"Function ignored.\n";
            DisplayWarning(c_nm, str);
          }
          else
            ParseSignature(c_nm, state, f_m, nm, type, op.GetParameters(),
                           precond, postcond, acc, id, ReportErr);
          break;
        }
      }
    }
  }
}

// #####################################################################
// ##  END OF Functions implementing the mapping from UML to AUML.    ##
// #####################################################################

