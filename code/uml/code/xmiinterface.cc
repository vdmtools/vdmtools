#include "metaiv.h"
#include "xmiinterface.h"
#include "xmiwriter.h"
#include "xmireader.h"
#include "tools.h"
#include "tbutils.h"
#include "xmimodel.h"
#include "mini-p-print.h"
#include "xmiaux.h"
#include "MANGLE.h"
#include "parse_string.h"

XmiInterface::XmiInterface()
{
  this->mode = MODE_UNKNOWN;
}

XmiInterface::XmiInterface(int m)
{
  this->mode = m;
}

XmiInterface::~XmiInterface()
{
}

// Establishes the connection to the CASE Tool
// Returns true if it was possible to establish the connection.
bool XmiInterface::Init()
{
  this->modified = false;
  this->currentModelName = L"";
  this->currentModel = TYPE_AUML_Model().Init(TYPE_AUML_Classes(),
                                              TYPE_AUML_Inheritance(),
                                              TYPE_AUML_Associations());
  init_XMI();
  return true;
}

// Closes down the connection to the CASE Tool.
void XmiInterface::CloseDown()
{
}

// Gives the CASE Tool application window the focus.
void XmiInterface::GiveFocus()
{
}

// Gives the name of the file current used by the CASE Tool
const wstring XmiInterface::GetModel()
{
  return this->currentModelName; 
}

// Selects the model file used by the CASE Tool.
void XmiInterface::SelectModel(const wstring & name)
{
  if (!name.empty()) {
    if (TBUTILS::file_exists(name)) { // The file exists, ok to open it.
      if(name != this->currentModelName) {
        XmiReader reader (this->mode); 
        Tuple t (reader.ReadXMIDocument(name));
        if (t.GetBool(1)) {
          TYPE_XMI_Document doc (t.GetRecord(2));
          this->currentModel = XMI2AUML(doc, t.GetMap(3));
        }
        else {
          vdm_log << t.GetSequence(5).GetString() << endl;
          this->currentModel = TYPE_AUML_Model().Init(TYPE_AUML_Classes(),
                                                      TYPE_AUML_Inheritance(),
                                                      TYPE_AUML_Associations());
        }
      }
    }
    else {
      this->currentModel = TYPE_AUML_Model().Init(TYPE_AUML_Classes(),
                                                  TYPE_AUML_Inheritance(),
                                                  TYPE_AUML_Associations());
    }
  }
  this->currentModelName = name; 
  this->modified = false;
}

// Creates a backup file of the current model. 
const wstring XmiInterface::DoBackup(const wstring & where)
{
  XmiWriter writer (this->mode);
  wstring revert_to (this->currentModelName);
  wstring back_name;
  wstring model_name;
  if(revert_to.empty()) {
    revert_to = where + L"/untitled.xml";
    back_name = where + L"/untitled_old.xml";
    model_name = L"untitled";

    if (!writer.SaveAs(revert_to, AUML2XMI(Sequence(model_name), this->currentModel))) {
      vdm_err << L"Unable to save XMI file " << revert_to << endl << flush;
    }
  }
  else {
    std::string::size_type pos = revert_to.rfind(L".xml");
    if(pos != wstring::npos) {
      model_name = revert_to.substr(0, pos);
    }
    else {
      model_name = revert_to;
    }
    back_name = model_name + L"_old.xml";
  }

  if (this->modified) {
    if (!writer.SaveAs(revert_to, AUML2XMI(TBUTILS::tb_getbasename(model_name), this->currentModel))) {
      vdm_err << L"Unable to save XMI file " << revert_to << endl << flush;
    }
  }

  if (!TBUTILS::file_copy_overwrite(back_name, revert_to)) {
    vdm_err << L"Unable to backup XMI file \'" << revert_to
            << L"\' to \'" << back_name << L"\'" << endl << flush;
  }

  this->SelectModel(revert_to);
  return back_name;
}

// Retrieves the current UML model defined in the CASE Tool.
TYPE_AUML_Model XmiInterface::GetCurModel(const Bool & ReportErr, const TYPE_AUML_Model & vppmodel)
{
  return this->currentModel;
}

// Set the current UML model of the CASE Tool to be equal to curmdl
void XmiInterface::SetCurModel(const TYPE_AUML_Model & newmdl, const TYPE_AUML_Model & umlmodel)
{
  this->currentModel = newmdl;

  wstring model_name;
  wstring fname (TBUTILS::tb_getbasename(this->currentModelName).GetString());
  std::string::size_type pos = fname.rfind(L".xml");
  if(pos != wstring::npos) {
    model_name = fname.substr(0, pos);
  }
  else {
    model_name = fname;
  }
  XmiWriter writer (this->mode);
  if (!writer.SaveAs(this->currentModelName, AUML2XMI(Sequence(model_name), this->currentModel))) {
    vdm_err << L"Unable to save Rose model " << this->currentModelName << endl << flush;
  }
}

// Creates a class diagram with name name for the model in curmdl.
void XmiInterface::BuildClassDiagram(const wstring & name, const TYPE_AUML_Model & curmdl)
{
}

TYPE_XMI_Document XmiInterface::AUML2XMI(const Sequence & model_name, const TYPE_AUML_Model & model)
{
// 
  this->type_m.Clear();
  this->stype_l.Clear();
  this->gene_l.Clear();
//
  Sequence modelid (XmiAux::UniqueID());
  this->mid = modelid;

  TYPE_AUML_Classes classes (model.get_classes());
  TYPE_AUML_Inheritance inheritance (model.get_inheritance ());
  TYPE_AUML_Associations associations (model.get_associations());
  Set clsnm_s (classes.Dom());

  Sequence el_l;
  // update id
  Generic clnm;
  for (bool bb = clsnm_s.First(clnm); bb; bb = clsnm_s.Next(clnm)) {
    TYPE_AUML_Class cls (classes[clnm]);
    if (cls.get_id().IsNil() || cls.get_id() == Sequence()) {
      cls.set_id(XmiAux::UniqueID());
    }
    classes.ImpModify(clnm, cls);
  }

  // create class
  this->cls_m = classes;

  for (bool cc = clsnm_s.First(clnm); cc; cc = clsnm_s.Next(clnm)) {
    TYPE_XMI_UMLClass c (AUMLClass2XMIClass(clnm, classes[clnm], modelid));
  
    if (inheritance.DomExists(clnm)) {
      Set sc_s (inheritance[clnm]);
      if (!sc_s.IsEmpty()) {
        Sequence ge_l;
        Generic g;
        for (bool gg = sc_s.First(g); gg; gg = sc_s.Next(g)) {
          //ge_l.ImpAppend(GetGeneralizationRef(clnm, g, modelid));
          if (this->cls_m.DomExists(g)) {
            ge_l.ImpAppend(GetGeneralizationRef(clnm, g, modelid));
          }
          else {
            vdm_log << L"Class: " << g.ascii() << " is't defined." << endl << flush;
          }
        }
        c.set_generalizableElement (TYPE_XMI_UMLGeneralizableElement().Init(Map(), ge_l));
      }
    }
    el_l.ImpAppend(c);

    // Append Association
    TYPE_AUML_UniAssoc ua (associations[clnm]);
    if (!ua.IsEmpty()) {
      Sequence a_l (AUMLAssoc2XMIAssoc(clnm, ua, modelid));
      size_t len_a_l = a_l.Length();
      for (size_t idx = 1; idx <= len_a_l; idx++) {
        el_l.ImpAppend(a_l[idx]);
      }
    }
  }

  // Append Generalization
  size_t len_gene_l = this->gene_l.Length();
  for (size_t idx = 1; idx <= len_gene_l; idx++) {
    el_l.ImpAppend(this->gene_l[idx]);
  }

  // Append generated Class
  Set dom_type_m (this->type_m.Dom());
  Generic g;
  for (bool dd = dom_type_m.First(g); dd; dd = dom_type_m.Next(g)) {
    if (this->type_m[g].Is(TAG_TYPE_XMI_UMLClass))
      el_l.ImpAppend(this->type_m[g]);
  }

  // Append Stereotype
  size_t len_stype_l = this->stype_l.Length(); for (size_t idx2 = 1; idx2 <= len_stype_l; idx2++)
  {
    el_l.ImpAppend(this->stype_l[idx2]);
  }

  Sequence pr_l;
  // Append Primitive
  for (bool ff = dom_type_m.First(g); ff; ff = dom_type_m.Next(g)) {
    if (this->type_m[g].Is(TAG_TYPE_XMI_UMLPrimitive)) {
      pr_l.ImpAppend(this->type_m[g]);
    }
  }
// 
  this->type_m = Map();
  this->stype_l = Sequence();
  this->cls_m = Map();
//

  TYPE_XMI_UMLModel m (XmiAux::mk_UMLModel(model_name, modelid));
  TYPE_XMI_UMLNamespace ns (m.get_namespace());
  ns.set_ownedElement(el_l);
  m.set_namespace(ns);

  return XmiAux::mk_Document(m, pr_l, this->mode);
}

TYPE_XMI_UMLClass XmiInterface::AUMLClass2XMIClass(const Sequence & clnm,
                                                   const TYPE_AUML_Class & cls,
                                                   const Sequence & pid)
{
  Generic id = cls.get_id();
  if (id.IsNil() || id == Sequence()) {
    id = XmiAux::UniqueID();
  } 
  IXmiClass res (clnm, id, pid);
  res.SetAbstract(cls.get_abstract());

  Sequence feature;
  TYPE_AUML_InstanceVars iv (cls.get_inst());
  Generic g, h;
  for (bool bb = iv.First(g, h); bb; bb = iv.Next(g, h)) {
    feature.ImpAppend(AUMLInstValue2XMIAttribute(g, h, id));
  }

  TYPE_AUML_ValuesDef vd (cls.get_val());
  for (bool cc = vd.First(g, h); cc; cc = vd.Next(g, h)) {
    feature.ImpAppend(AUMLValue2XMIAttribute(g, h, id));
  }

  TYPE_AUML_CommonSign meth (cls.get_meth());
  for (bool dd = meth.First(g, h); dd; dd = meth.Next(g, h)) {
    feature.ImpAppend(AUMLFnOp2XMIOperation(g, h, id, true));
  }

  TYPE_AUML_CommonSign func (cls.get_func());
  for (bool ee = func.First(g, h); ee; ee = func.Next(g, h)) {
    feature.ImpAppend(AUMLFnOp2XMIOperation(g, h, id, false));
  }
  res.set_classifier(TYPE_XMI_UMLClassifier().Init(Map(), feature));

  res.AddTaggedValue(XmiAux::mk_UMLTaggedValue(TAG_XMI_isClass, TAG_XMI_TRUE, id));
#if __cplusplus >= 201103L
  return std::move(res);
#else
  return res;
#endif
}

TYPE_XMI_UMLAttribute XmiInterface::AUMLValue2XMIAttribute(const TYPE_AS_Pattern & pat,
                                                           const TYPE_AUML_ValDef & vd,
                                                           const Sequence & pid)
{
  // id
  Generic id = vd.get_id();
  if (id.IsNil() || id == Sequence()) {
    id = XmiAux::UniqueID();
  }
  // create attribute with name
  Sequence patSeq (MPP::MiniPP(pat));
  IXmiAttribute res (patSeq, id, pid);

  // type
  if (!vd.get_tp().IsNil()) {
    res.SetTypeRef(GetTypeRef(MPP::MiniPP(vd.get_tp()))); 
  }

  // initial value expression
  if (!vd.get_expr().IsNil()) {
    res.set_initialValue(XmiAux::mk_UMLExpression(Sequence(MPP::MiniPP(vd.get_expr()))));
  }

  // scope (instance, class) 
  res.SetStatic(vd.get_stat());

  // changeability (instance variable or value)
  res.SetChangeable(Bool(false));

  // visibility (public, protected, private)
  res.SetVisibility(MapAUMLAccessToXMI(vd.get_acc()));

#if __cplusplus >= 201103L
  return std::move(res);
#else
  return res;
#endif
}

TYPE_XMI_UMLAttribute XmiInterface::AUMLInstValue2XMIAttribute(const Sequence & name,
                                                               const TYPE_AUML_ValDef & vd,
                                                               const Sequence & pid)
{
  // id
  Generic id = vd.get_id();
  if (id.IsNil() || id == Sequence()) {
    id = XmiAux::UniqueID();
  }
  // create attribute with name
  IXmiAttribute res (name, id, pid);

  // type
  if (!vd.get_tp().IsNil()) {
    res.SetTypeRef(GetTypeRef(MPP::MiniPP(vd.get_tp()))); 
  }

  // initial value
  if (!vd.get_expr().IsNil()) {
    res.set_initialValue(XmiAux::mk_UMLExpression(Sequence(MPP::MiniPP(vd.get_expr()))));
  }

  // scope (instance, class) 
  res.SetStatic(vd.get_stat());

  // changeability (instance variable or value)
  res.SetChangeable(Bool(true));

  // visibility (public, protected, private)
  res.SetVisibility(MapAUMLAccessToXMI(vd.get_acc()));

#if __cplusplus >= 201103L
  return std::move(res);
#else
  return res;
#endif
}

TYPE_XMI_UMLOperation XmiInterface::AUMLFnOp2XMIOperation(const Sequence & name,
                                                          const TYPE_AUML_Signature & fnop,
                                                          const Sequence & clid,
                                                          bool op)
{
  // id
  Generic fnopid = fnop.get_id();
  if (fnopid.IsNil() || fnopid == Sequence()) {
    fnopid = XmiAux::UniqueID();
  }
  // name
  // unmangle
  IXmiOperation res (MANGLE::unmangleSeqStem(name), fnopid, clid);

  // return type
  IXmiParameter rp (Sequence(L""), XmiAux::UniqueID(), fnopid, clid);
  rp.SetReturn();
  rp.SetTypeRef(GetTypeRef(MPP::MiniPP(fnop.get_returntype()))); 

  Sequence parameter;
//  res.AppendParameter(rp);
  parameter.ImpAppend(rp);

  // parameter
  SEQ<TYPE_AUML_PatType> pt_l (fnop.get_param());
  Generic g;
  for (bool bb = pt_l.First(g); bb; bb = pt_l.Next(g)) {
    TYPE_AUML_PatType pt (g);
    IXmiParameter rp (MPP::MiniPP(pt.get_pat()), XmiAux::UniqueID(), fnopid, clid);
    rp.SetTypeRef(GetTypeRef(MPP::MiniPP(pt.get_tp()))); 
//    res.AppendParameter(rp);
    parameter.ImpAppend(rp);
  }

  res.set_behavioralFeature(TYPE_XMI_UMLBehavioralFeature().Init(Map(), parameter));

  // scope (instance, class) 
  res.SetStatic(fnop.get_stat());

  // streotype (function, operation)
  if (op) {
    res.AppendStereotype(GetStereotypeOpRef(fnopid));
  }
  else {
    res.AppendStereotype(GetStereotypeFnRef(fnopid));
  }
  // additional info total/partial
  if (!op) {
    if (fnop.get_fntype() == QUOTE_PARTIAL)
      res.AddTaggedValue(XmiAux::mk_UMLTaggedValue(TAG_XMI_fntype,
                                                   TAG_XMI_partial,
                                                   fnopid));
    else if (fnop.get_fntype() == QUOTE_TOTAL)
      res.AddTaggedValue(XmiAux::mk_UMLTaggedValue(TAG_XMI_fntype,
                                                   TAG_XMI_total,
                                                   fnopid));
  }

  // additional info impl/expl/extexpl
  if (fnop.get_kind() == QUOTE_IMPL) {
    res.AddTaggedValue(XmiAux::mk_UMLTaggedValue(TAG_XMI_deftype,
                                                 TAG_XMI_impl,
                                                 fnopid));
  }
  else if (fnop.get_kind() == QUOTE_EXT) {
    res.AddTaggedValue(XmiAux::mk_UMLTaggedValue(TAG_XMI_deftype,
                                                 TAG_XMI_extexpl,
                                                 fnopid));
  }
  else if (fnop.get_kind() == QUOTE_EXPL) {
    res.AddTaggedValue(XmiAux::mk_UMLTaggedValue(TAG_XMI_deftype,
                                                 TAG_XMI_expl,
                                                 fnopid));
  }
  // pre condition
  if (!fnop.get_precond().IsNil()) {
    if (IsEA()) {
      res.AddConstraint(XmiAux::mk_UMLConstraint(MPP::MiniPP(fnop.get_precond()),
                                                 TAG_XMI_precondition,
                                                 fnopid));  
    }
    else {
      res.AddTaggedValue(XmiAux::mk_UMLTaggedValue(TAG_XMI_precondition,
                                                   MPP::MiniPP(fnop.get_precond()),
                                                   fnopid));
    }
  }

  // post condition
  if (!fnop.get_postcond().IsNil()) {
    if (IsEA()) {
      res.AddConstraint(XmiAux::mk_UMLConstraint(MPP::MiniPP(fnop.get_postcond()),
                                                 TAG_XMI_postcondition,
                                                 fnopid));  
    }
    else {
      res.AddTaggedValue(XmiAux::mk_UMLTaggedValue(TAG_XMI_postcondition,
                                                   MPP::MiniPP(fnop.get_postcond()),
                                                   fnopid));
    }
  }

  // visibility (public, protected, private)
  res.SetVisibility(MapAUMLAccessToXMI(fnop.get_acc()));

#if __cplusplus >= 201103L
  return std::move(res);
#else
  return res;
#endif
}

TYPE_AUML_Model XmiInterface::XMI2AUML(const TYPE_XMI_Document & doc,
                                       const Map & idm)
{
  
  Generic mod_g (doc.get_content().get_model());
  if (!mod_g.IsNil()) {
    TYPE_XMI_UMLModel model (mod_g);
    TYPE_AUML_Model m;
    m.Init(CreateClasses(model, idm),
           CreateInheritance(model, idm),
           CreateAssociations(model, idm));
    return m;
  }
  else {
    TYPE_AUML_Model dummy;
    dummy.Init(TYPE_AUML_Classes(),
               TYPE_AUML_Inheritance(),
               TYPE_AUML_Associations());
    return dummy;
  }
}

TYPE_AUML_Classes XmiInterface::CreateClasses (const TYPE_XMI_UMLModel & m, const Map & idm)
{
  TYPE_AUML_Classes clss;
  Sequence c_l (XmiAux::GetClasses(m));
  Generic g;
  for ( bool bb = c_l.First(g); bb; bb = c_l.Next(g)) {
    TYPE_XMI_UMLClass c (g);
    Sequence nm (XmiAux::GetName(c));

    TYPE_AUML_Class nc;
    nc.Init(CreateInstanceVars(c, idm),
            CreateValuesDef(c, idm),
            CreateOperations(c, idm),
            CreateFunctions(c, idm),
            XmiAux::IsAbstract(c),
            XmiAux::GetID(c));
    clss.ImpModify(nm, nc);  
  }

  Set elems (idm.Rng());
  Generic e;
  for (bool bb = elems.First(e); bb; bb = elems.Next(e)) {
    if (e.Is(TAG_TYPE_XMI_EAStub)) {
      Map m (XmiAux::GetAttribute(e));
      if (m.DomExists(Sequence(L"UMLType"))) {
        if (m[Sequence(L"UMLType")] == Sequence(L"Class")) {
          Sequence nm (XmiAux::GetName(e));
          if (0 == nm.Find(Char(L' '))) {
            if (!clss.DomExists(nm)) {
              TYPE_AUML_Class nc;
              nc.Init(TYPE_AUML_InstanceVars(),
                      TYPE_AUML_ValuesDef(),
                      TYPE_AUML_CommonSign(),
                      TYPE_AUML_CommonSign(),
                      Bool(false),
                      XmiAux::GetID(e));
              clss.ImpModify(nm, nc);  
            }
          }
        }
      }
    }
  }

  return clss;
}

TYPE_AUML_InstanceVars XmiInterface::CreateInstanceVars(const TYPE_XMI_UMLClass & c,
                                                        const Map & idm)
{
  TYPE_AUML_InstanceVars ivs;
  Sequence a_l (XmiAux::GetAllAttributes(c));
  Generic g;
  for (bool bb = a_l.First(g); bb; bb = a_l.Next(g)) {
    IXmiAttribute a (g);
    if (!XmiAux::IsChangeable(a)) continue;

    Sequence nm (XmiAux::GetName(a));

    Sequence type (XmiAux::GetElementName(a.GetTypeRef(), idm));
    Tuple t (CreateValDef(type,                    // type
                          a.get_initialValue(),    // initial value expression
                          XmiAux::GetVisibility(a),// visibility (public, protected, private)
                          XmiAux::IsStatic(a),     // scope (instance, class)
                          XmiAux::GetID(a),        // id
                          false));                 // values or instance variables
    if (!type.IsEmpty() && t.GetBool(1) && UMLPARSE::ParseIdentifierSeq(nm)) {
      if (ivs.DomExists(nm)) {
        vdm_log << L"Instance variable " + nm.GetString() +
        L" was previously defined in this class. Ignoring instance variable." << endl;
      }
      else {
        ivs.ImpModify(nm, t.GetRecord(2));
      }
    }
    else  {// if (ReportErr)
      vdm_log << L"Syntax error in instance variable definition:" << endl;
      vdm_log << XmiAux::PrintAttribute(a, idm).GetString() << endl;
      vdm_log << L"Instance variable ignored." << endl;
    }
  }
  return ivs;
}

TYPE_AUML_ValuesDef XmiInterface::CreateValuesDef(const TYPE_XMI_UMLClass & c,
                                                  const Map & idm)
{
  TYPE_AUML_ValuesDef vds;
  Sequence a_l (XmiAux::GetAllAttributes(c));
  Generic g;
  for (bool bb = a_l.First(g); bb; bb = a_l.Next(g)) {
    IXmiAttribute a (g);
    if (XmiAux::IsChangeable(a)) continue;

    Sequence nm (XmiAux::GetName(a));
    Tuple t1 (UMLPARSE::Seq2Pat(nm));

    Sequence type (XmiAux::GetElementName(a.GetTypeRef(), idm));
    Tuple t2  (CreateValDef(type,                    // type
                            a.get_initialValue(),    // initial value expression
                            XmiAux::GetVisibility(a),// visibility (public, protected, private)
                            XmiAux::IsStatic(a),     // scope (instance, class)
                            XmiAux::GetID(a),        // id
                            true));                  // values or instance variables
 
    if (t1.GetBool(1) && t2.GetBool(1)) {
      TYPE_AS_Pattern pat (t1.GetRecord(2));
      TYPE_AUML_ValDef vd (t2.GetRecord(2));
      
      if (pat.Is(TAG_TYPE_AS_PatternName)) {
        pat.SetField(pos_AS_PatternName_tp, vd.get_tp());   
      }
      if (vds.DomExists(pat)) {
        vdm_log << L"Value " + nm.GetString() +
        L" was previously defined in this class. Ignoring value definition." << endl;
      }
      else {
        vds.ImpModify(pat, t2.GetRecord(2));
      }
    }
    else  {// if (ReportErr) {
      vdm_log << L"Syntax error in value definition:" << endl;
      vdm_log << XmiAux::PrintAttribute(a, idm).GetString() << endl;
      vdm_log << L"Value ignored." << endl;
    }
  }

  return vds;
}

Tuple XmiInterface::CreateValDef(const Sequence & type,
                                 const Generic & initVal,
                                 const Sequence & acc,
                                 const Bool & stat,
                                 const Sequence & id,
                                 bool isValue)
{
  Generic tp = Nil();
  if (!type.IsEmpty()) {
    Tuple t (UMLPARSE::Seq2Type(type));
    if (!t.GetBool(1)) {
      return mk_(Bool(false), Nil());
    }
    tp = t.GetField(2);
  }

  Generic body = Nil();
  if (!initVal.IsNil()) {
    TYPE_XMI_UMLExpression exp (initVal);
    Map attrs (exp.get_attrs());
    if (attrs.DomExists(TAG_XMI_body)) {
      body = attrs[TAG_XMI_body];     // EA
    }
    else  {
      body = (exp.get_body());        // ASTAH
    }
  }

  Generic expr = Nil();
  if (!body.IsNil()) {
    if (body.IsSequence()) {
      Tuple t (UMLPARSE::Seq2Expr(body));
      if (!t.GetBool(1)) {
        return mk_(Bool(false), Nil());
      }
      expr = t.GetField(2);
    }
    else {
      return mk_(Bool(false), Nil());
    }
  }
  else {
    if (isValue) {
      expr = TYPE_AS_UndefinedExpr();
    }
  }
    
  TYPE_AUML_ValDef vd;
  vd.Init(tp,
          expr,
          MapXMIAccessToAUML(acc),
          stat,
          id);
  return mk_(Bool(true), vd);
}

TYPE_AUML_CommonSign XmiInterface::CreateOperations(const TYPE_XMI_UMLClass & c, const Map & idm)
{
  TYPE_AUML_CommonSign ops;
  Sequence o_l (XmiAux::GetAllOperations(c));
  Generic g;
  for (bool bb = o_l.First(g); bb; bb = o_l.Next(g)) {
    TYPE_XMI_UMLOperation o (g);
    if (!XmiAux::IsOperation(o, idm)) continue;

    Sequence nm (XmiAux::GetName(o));

    IXmiParameter rtp (XmiAux::GetReturnType(o));
    Sequence rt (XmiAux::GetElementName(rtp.GetTypeRef(), idm));

    Tuple tr ( (!rt.IsEmpty() && (rt != Sequence(L"void"))) ? UMLPARSE::Seq2Type(rt)
                                           : mk_(Bool(true), TYPE_AS_VoidType().Init(Int(-1))));
    Tuple tp (XMIParameters2AUMLParams(XmiAux::GetParam(o), idm));

    Sequence pre (XmiAux::GetPreCondition(o));
    Sequence post (XmiAux::GetPostCondition(o));

    Tuple tpre (pre.IsEmpty() ? mk_(Bool(true), Nil()) : UMLPARSE::Seq2Expr(pre));
    Tuple tpost (post.IsEmpty() ? mk_(Bool(true), Nil()) : UMLPARSE::Seq2Expr(post));

    if (tr.GetBool(1) && tp.GetBool(1) && 
        tpre.GetBool(1) && tpost.GetBool(1)) {
      Sequence mname (MANGLE::MangleString(nm.GetString(), tp.GetField(3)));

      Quote kind = QUOTE_EXPL;
      if (XmiAux::IsImpl(o, idm)) {
        kind = QUOTE_IMPL;
      }
      else if (XmiAux::IsExtExpl(o, idm)) {
        kind = QUOTE_EXT;
      }
      TYPE_AUML_Signature op; 
      op.Init(tp.GetField(2),                               // param
              tr.GetField(2),                               // returntype
              kind,                                         // kind
              Nil(),                                        // fntype
              tpre.GetField(2),                             // precond
              tpost.GetField(2),                            // postcond
              MapXMIAccessToAUML(XmiAux::GetVisibility(o)), // acc
              XmiAux::IsStatic(o),                          // static
              XmiAux::GetID(o));                            // id

      ops.ImpModify(mname, op);
    }
    else {
      vdm_log << L"Ignoring operation due to errors in its arguments or return type:" << endl;
      vdm_log << XmiAux::PrintOperation(o, idm) << endl;
    }
  }
  return ops;
}

TYPE_AUML_CommonSign XmiInterface::CreateFunctions(const TYPE_XMI_UMLClass & c, const Map & idm)
{
  TYPE_AUML_CommonSign fns;
  Sequence o_l (XmiAux::GetAllOperations(c));
  Generic g;
  for (bool bb = o_l.First(g); bb; bb = o_l.Next(g)) {
    TYPE_XMI_UMLOperation o (g);
    if (!XmiAux::IsFunction(o, idm)) continue;

    Sequence nm (XmiAux::GetName(o));

    IXmiParameter rtp (XmiAux::GetReturnType(o));
    Sequence rt (XmiAux::GetElementName(rtp.GetTypeRef(), idm));

    if ((rt.IsEmpty() || (rt == Sequence(L"void")))) {
      vdm_log << L"Function " <<  nm.GetString() << L" must have a returntype. Function ignored:" << endl;
      vdm_log << XmiAux::PrintOperation(o, idm) << endl;
    }
    else {
      Tuple tr (UMLPARSE::Seq2Type(rt));
      Tuple tp (XMIParameters2AUMLParams(XmiAux::GetParam(o), idm));
 
      Sequence pre (XmiAux::GetPreCondition(o));
      Sequence post (XmiAux::GetPostCondition(o));
      Tuple tpre (pre.IsEmpty() ? mk_(Bool(true), Nil()) : UMLPARSE::Seq2Expr(pre));
      Tuple tpost (post.IsEmpty() ? mk_(Bool(true), Nil()) : UMLPARSE::Seq2Expr(post));

      if (tr.GetBool(1) && tp.GetBool(1) &&
          tpre.GetBool(1) && tpost.GetBool(1)) {
        Sequence mname (MANGLE::MangleString(nm.GetString(), tp.GetField(3)));

        Quote kind = QUOTE_EXPL;
        if (XmiAux::IsImpl(o, idm)) {
          kind = QUOTE_IMPL;
        }
        else if (XmiAux::IsExtExpl(o, idm)) {
          kind = QUOTE_EXT;
        }
        Quote fntype = QUOTE_TOTAL;
        if (XmiAux::IsPartial(o, idm)) {
          fntype = QUOTE_PARTIAL;
        }
        TYPE_AUML_Signature fn; 
        fn.Init(tp.GetField(2),                               // param
                tr.GetField(2),                               // returntype
                kind,                                         // kind
                fntype,                                       // fntype
                tpre.GetField(2),                             // precond
                tpost.GetField(2),                            // postcond
                MapXMIAccessToAUML(XmiAux::GetVisibility(o)), // acc
                XmiAux::IsStatic(o),                          // static
                XmiAux::GetID(o));                            // id

        fns.ImpModify(mname, fn);
      }
      else {
        vdm_log <<  L"Ignoring function due to errors in its arguments or return type:" << endl;
        vdm_log << XmiAux::PrintOperation(o, idm) << endl;
      }
    }
  }
  return fns;
}

Tuple XmiInterface::XMIParameters2AUMLParams(const Sequence & p_l, const Map & idm)
{
  SEQ<TYPE_AUML_PatType> pt_l;
  SEQ<TYPE_AS_Type> tp_l;
  size_t len_p_l = p_l.Length();
  for (size_t idx = 1; idx <= len_p_l; idx++) {
    const TYPE_XMI_UMLParameter & p (p_l[idx]);
    Tuple tn (UMLPARSE::Seq2Pat(XmiAux::GetName(p)));
    Tuple tt (UMLPARSE::Seq2Type(XmiAux::GetElementName(XmiAux::GetTypeRef(p), idm)));
    if (tn.GetBool(1) && tt.GetBool(1)) {
      pt_l.ImpAppend(TYPE_AUML_PatType().Init(tn.GetField(2), tt.GetField(2)));
      tp_l.ImpAppend(tt.GetField(2));
    }
    else {
      return mk_(Bool(false), Nil(), Nil());
    }
  }
  return mk_(Bool(true), pt_l, tp_l);
}

TYPE_AUML_Inheritance XmiInterface::CreateInheritance(const TYPE_XMI_UMLModel & m,
                                                      const Map & idm)
{
  Map tm;
  SEQ<TYPE_XMI_UMLGeneralization> g_l (XmiAux::GetAllGeneralizations(m));

  Generic g;
  for (bool bb = g_l.First(g); bb; bb = g_l.Next(g)) {
    TYPE_XMI_UMLGeneralization ge (g);
    // child
    Sequence cid (XmiAux::GetChildId(ge));
    if (cid.IsEmpty()) continue;
    
    Sequence cname (XmiAux::GetElementName(cid, idm));
    if (cname.IsEmpty()) continue;

    // parent
    Sequence pid (XmiAux::GetParentId(ge));
    if (pid.IsEmpty()) continue;

    Sequence pname (XmiAux::GetElementName(pid, idm));
    if (pname.IsEmpty()) continue;

    // map
    Set s;
    if (tm.DomExists(cname)) {
      s = tm[cname];
    }
    s.Insert(pname);
    tm.ImpModify(cname, s);
  } 

  TYPE_AUML_Inheritance inh;
  Sequence c_l (XmiAux::GetClasses(m));
  for (bool cc = c_l.First(g); cc; cc = c_l.Next(g)) {
    TYPE_XMI_UMLClass c (g);
    Sequence nm (XmiAux::GetName(c));
    if (tm.DomExists(nm)) {
      inh.ImpModify(nm, tm[nm]);
    }
    else {
      inh.ImpModify(nm, Set());
    }
  }

  // 
  Set elems (idm.Rng());
  Generic e;
  for (bool bb = elems.First(e); bb; bb = elems.Next(e)) {
    if (e.Is(TAG_TYPE_XMI_EAStub)) {
      Map m (XmiAux::GetAttribute(e));
      if (m.DomExists(Sequence(L"UMLType"))) {
        if (m[Sequence(L"UMLType")] == Sequence(L"Class")) {
          Sequence nm (XmiAux::GetName(e));
          if (0 == nm.Find(Char(L' '))) {
            if (!inh.DomExists(nm)) {
              inh.ImpModify(nm, Set());
            }
          }
        }
      }
    }
  }

  return inh;
}


// CreateAssociations
// m : XMI`UMLModel
// idm : map 
// ==> AUML`Associations
TYPE_AUML_Associations XmiInterface::CreateAssociations(const TYPE_XMI_UMLModel & m,
                                                        const Map & idm)
{
  Map tm;

  SEQ<TYPE_XMI_UMLAssociation> as_l (XmiAux::GetAllAssociation(m));
  Generic ag;
  for (bool cc = as_l.First(ag); cc; cc = as_l.Next(ag)) {
//
    TYPE_XMI_UMLAssociation as (ag);
    int mode = XmiAux::GetModeOfAssociation(as);

    Sequence ae_l (as.get_connection());
    if (ae_l.Length() == 2) {
      Tuple t1 (CreateAssociation(ae_l[1], ae_l[2], idm, mode));
      if (t1.GetBool(1)) {
        Sequence nm (t1.GetSequence(2));
        Sequence client (t1.GetSequence(3));
        Map rm;
        if (tm.DomExists(client)) {
          rm = tm[client];
        }
        rm.ImpModify(nm, t1.GetRecord(4));
        tm.ImpModify(client, rm);
      }
      Tuple t2 (CreateAssociation(ae_l[2], ae_l[1], idm, mode));
      if (t2.GetBool(1)) {
        Sequence nm (t2.GetSequence(2));
        Sequence client (t2.GetSequence(3));
        Map rm;
        if (tm.DomExists(client)) {
          rm = tm[client];
        }
        rm.ImpModify(nm, t2.GetRecord(4));
        tm.ImpModify(client, rm);
      }
    }
//    
  }
  
  TYPE_AUML_Associations ass;
  Sequence c_l (XmiAux::GetClasses(m));
  Generic g;
  for (bool cc = c_l.First(g); cc; cc = c_l.Next(g)) {
    TYPE_XMI_UMLClass c (g);
    Sequence nm (XmiAux::GetName(c));
    if (tm.DomExists(nm)) {
      ass.ImpModify(nm, tm[nm]);
    }
    else {
      ass.ImpModify(nm, Map());
    }
  }

  // 
  Set elems (idm.Rng());
  Generic e;
  for (bool bb = elems.First(e); bb; bb = elems.Next(e)) {
    if (e.Is(TAG_TYPE_XMI_EAStub)) {
      Map m (XmiAux::GetAttribute(e));
      if (m.DomExists(Sequence(L"UMLType"))) {
        if (m[Sequence(L"UMLType")] == Sequence(L"Class")) {
          Sequence nm (XmiAux::GetName(e));
          if (0 == nm.Find(Char(L' '))) {
            if (!ass.DomExists(nm)) {
              ass.ImpModify(nm, Map());
            }
          }
        }
      }
    }
  }
  return ass;
}

// CreateAssociation
// se : XMI`UMLAssociationEnd
// ce : XMI`UMLAssociationEnd
// idm : 
// mode : 
// ==> bool * [seq of char] * [seq of char] * [AUML`Role]
Tuple XmiInterface::CreateAssociation(const TYPE_XMI_UMLAssociationEnd & se, 
                                      const TYPE_XMI_UMLAssociationEnd & ce, 
                                      const Map & idm,
                                      int mode)
{
  Sequence nm (XmiAux::GetElementName(se, idm));

  Sequence server;
  Sequence client;
  Sequence name;

  switch(mode) {
    case MODE_EnterpriseArchitect: {
      if(nm.IsEmpty())
        return mk_(Bool(false), Nil(), Nil(), Nil());

      server = (XmiAux::GetClassNameFromAssoc(se, idm));
      client = (XmiAux::GetClassNameFromAssoc(ce, idm));

      break;
    }
    case MODE_ASTAH: {
      if (nm.IsEmpty())
        return mk_(Bool(false), Nil(), Nil(), Nil());

      server = (XmiAux::GetClassNameFromAssoc(ce, idm));
      client = (XmiAux::GetClassNameFromAssoc(se, idm));

      break;
    }
    default: {
      break;
    }
  }

  if (server.IsEmpty() || client.IsEmpty()) {
    return mk_(Bool(false), Nil(), Nil(), Nil());
  } 
  Quote role_t (XmiAux::IsOrdered(se, idm) ? QUOTE_ORDERED : QUOTE_NORMAL);

  Quote cd (XMIMult2AUMLCard(XmiAux::GetMultiplicity(se, idm)));

  Generic qualifer (XmiAux::GetQualiferSeq(se, ce, idm, mode));

  Generic tp (XmiAux::GetInitType(se, server, cd, qualifer, role_t, idm));

  TYPE_AUML_Role r;
  r.Init(server,                                             // server
         XMIMult2AUMLCard(XmiAux::GetMultiplicity(se, idm)), // cd
         XmiAux::GetQualifer(se, ce, idm, mode),             // quali
         XmiAux::GetInitValue(se, idm),                      // initval // TODO
         tp,                                                 // tp      // TODO
         tp,                                                 // unftp   // TODO
         role_t,                                             // role_ut
         MapXMIAccessToAUML(XmiAux::GetVisibility(se, idm)), // acc
         XmiAux::IsStatic(se, idm),                          // static
         XmiAux::GetID(se));                                 // id

  return mk_(Bool(true), nm, client, r);
}

Quote XmiInterface::MapXMIAccessToAUML(const Sequence & acc)
{
  if (acc == TAG_XMI_public) {
    return QUOTE_PUBLIC;
  }
  if ((acc == TAG_XMI_private) || (acc == TAG_XMI_implementation)) {
    return QUOTE_PRIVATE;
  }
  if (acc == TAG_XMI_protected) {
    return QUOTE_PROTECTED;
  }
  return QUOTE_PUBLIC; // dummy
}

Sequence XmiInterface::MapAUMLAccessToXMI(const Quote & acc)
{
  if (acc == QUOTE_PUBLIC) {
    return TAG_XMI_public;
  }
  if (acc == QUOTE_PRIVATE) {
    return TAG_XMI_private;
  }
  if (acc == QUOTE_PROTECTED) {
    return TAG_XMI_protected;
  }
  return L""; // dummy
}

TYPE_XMI_UMLClassifier XmiInterface::GetTypeRef(const Sequence & tp)
{
  if (this->cls_m.DomExists(tp)) {
    TYPE_AUML_Class cls (this->cls_m[tp]);
    Map m;
    m.ImpModify(TAG_XMI_idref, cls.get_id());
    return TYPE_XMI_UMLClassifier().Init(m, Sequence());
  }

  if (!this->type_m.DomExists(tp)) {
    if (XmiAux::IsPrimitive(tp) && false) {
      this->type_m.ImpModify(tp, XmiAux::mk_UMLPrimitive(tp));
    }
    else {
      Sequence clid (XmiAux::UniqueID());
      IXmiClass gc (tp, clid, this->mid);
      gc.AddTaggedValue(XmiAux::mk_UMLTaggedValue(TAG_XMI_isClass, TAG_XMI_FALSE, clid));
      this->type_m.ImpModify(tp, gc);
    }
  }

  Record r (this->type_m[tp]);
  switch (r.GetTag()) {
    case TAG_TYPE_XMI_UMLPrimitive: {
      Map attrs (r.GetMap(pos_XMI_UMLPrimitive_attrs));
      Sequence id (attrs[TAG_XMI_id]);
      Map m;
      m.ImpModify(TAG_XMI_idref, id);
      return TYPE_XMI_UMLClassifier().Init(m, Sequence());
    }
    case TAG_TYPE_XMI_UMLClass: {
      Map attrs (r.GetMap(pos_XMI_UMLClass_attrs));
      Sequence id (attrs[TAG_XMI_id]);
      Map m;
      m.ImpModify(TAG_XMI_idref, id);
      return TYPE_XMI_UMLClassifier().Init(m, Sequence());
    }
    default: {
      return TYPE_XMI_UMLClassifier().Init(Map(), Sequence());
    }
  }
}

TYPE_XMI_UMLStereotype XmiInterface::GetStereotypeOpRef(const Sequence & pid)
{
  return GetStereotypeRef(TAG_XMI_operation, pid, Sequence(L"Operation"));
}

TYPE_XMI_UMLStereotype XmiInterface::GetStereotypeFnRef(const Sequence & pid)
{
  return GetStereotypeRef(TAG_XMI_function, pid, Sequence(L"Operation"));
}

TYPE_XMI_UMLStereotype XmiInterface::GetStereotypeRef(const Sequence & name,
                                                      const Sequence & pid,
                                                      const Sequence & base)
{
  Sequence sid (XmiAux::UniqueID());
  TYPE_XMI_UMLStereotype st (XmiAux::mk_UMLStereotype(name, sid, pid));
  st.set_baseClass(base);
  
  this->stype_l.ImpAppend(st);

  Map attrs;
  attrs.ImpModify(TAG_XMI_idref, sid);
  return TYPE_XMI_UMLStereotype().Init(attrs, Sequence(), Sequence()); 
}

TYPE_XMI_UMLGeneralization XmiInterface::GetGeneralizationRef(const Sequence & child,
                                                              const Sequence & parent,
                                                              const Sequence & pid)
{
  TYPE_XMI_UMLGeneralization ge (XmiAux::mk_UMLGeneralization(pid));

  TYPE_AUML_Class c_c (this->cls_m[child]);
  Map c_attrs;
  c_attrs.ImpModify(TAG_XMI_idref, c_c.get_id());
  ge.set_child(TYPE_XMI_UMLGeneralizableElement().Init(c_attrs, Sequence()));

  TYPE_AUML_Class p_c (this->cls_m[parent]);
  Map p_attrs;
  p_attrs.ImpModify(TAG_XMI_idref, p_c.get_id());
  ge.set_parent(TYPE_XMI_UMLGeneralizableElement().Init(p_attrs, Sequence()));

  Sequence gid (XmiAux::GetID(ge));

  this->gene_l.ImpAppend(ge);

  return XmiAux::mk_UMLGeneralizationRef(gid);
}

Sequence XmiInterface::AUMLAssoc2XMIAssoc(const Sequence & clnm,
                                          const TYPE_AUML_UniAssoc & ua,
                                          const Sequence & modelid)
{
  TYPE_AUML_Class s_c (this->cls_m[clnm]);

  Map sat;
  sat.ImpModify(TAG_XMI_idref, s_c.get_id()); 
  TYPE_XMI_UMLClassifier scref;
  scref.Init(sat, Sequence());

  int count = 0;
  Sequence res;
  Set nm_s (ua.Dom());
  Generic nm;
  for (bool bb = nm_s.First(nm); bb; bb = nm_s.Next(nm)) {
    TYPE_AUML_Role rl (ua[nm]);

    Sequence asid (XmiAux::UniqueID());
    IXmiAssociation ass (Sequence(L""), asid, modelid);

    Map asat;
    asat.ImpModify(TAG_XMI_idref, asid);
    IXmiAssociation assref (asat, Map());

    Sequence connection;
// target side
    TYPE_XMI_UMLClassifier tcref (GetTypeRef(rl.get_server()));

    Sequence tid (XmiAux::UniqueID());
    IXmiAssociationEnd te (Sequence(L""), tid, modelid); 

    te.SetOwner(tcref);
    te.SetMultiplicity(AUMLCard2XMIMultASTAH(Quote(L"ONE")));
    te.set_participant(scref);
    te.set_association(assref);

    te.SetMultiplicityEA(AUMLCard2XMIMult(Quote(L"ONE")));
    te.SetVisibility(Sequence(L"public"));

    if (IsEA()) {
      TYPE_XMI_UMLModelElement me (te.get_modelElement());
      Sequence taggedValue (me.get_taggedValue());
      taggedValue.ImpAppend(XmiAux::mk_UMLTaggedValue(TAG_XMI_containment, Sequence(L"Unspecified")));
      if (!rl.get_quali().IsNil()) {
        taggedValue.ImpAppend(XmiAux::mk_UMLTaggedValue(TAG_XMI_qualifier, Sequence(MPP::MiniPP(rl.get_quali()))));
      }
      taggedValue.ImpAppend(XmiAux::mk_UMLTaggedValue(TAG_XMI_sourcestyle, Sequence(L"Navigable=Unspecified;Union=0;Derived=0;AllowDuplicates=0;Owned=0;")));
      taggedValue.ImpAppend(XmiAux::mk_UMLTaggedValue(TAG_XMI_ea_end, Sequence(L"source")));
      me.set_taggedValue(taggedValue);
      te.set_modelElement(me);

      Map attrs (te.get_attrs());
      attrs.ImpModify(TAG_XMI_type, XmiAux::GetID(scref));
      te.set_attrs(attrs);
    }
//    ass.AppendConnection(te);
    connection.ImpAppend(te);

// source side
    // role name
    Sequence sid (XmiAux::UniqueID());
    IXmiAssociationEnd se (nm, sid, modelid); 

    se.SetOwner(scref);
    se.SetMultiplicity(AUMLCard2XMIMultASTAH(rl.get_cd()));
    se.set_participant(tcref);
    se.set_association(assref);

    se.SetMultiplicityEA(AUMLCard2XMIMult(rl.get_cd()));

    // ordered
    if (Quote(rl.get_role_ut()) == QUOTE_ORDERED) {
      se.SetOrdered(Bool(true));
    }
    else {
      se.SetOrdered(Bool(false));
    }
    // access
    se.SetVisibility(MapAUMLAccessToXMI(rl.get_acc()));

    // statis
    se.SetStatic(rl.get_stat());

    // navigable
    se.SetNavigable(Bool(false));

    if (!rl.get_quali().IsNil()) {
//    for ASTAH
      if (IsASTAH()) {
        Generic c (GetTypeRef(MPP::MiniPP(rl.get_quali()))); 
        Sequence id (XmiAux::UniqueID());

        Map attrs;
        attrs.ImpModify(TAG_XMI_id,            id);
        attrs.ImpModify(TAG_XMI_name,          Sequence(L"key").ImpConc(Sequence(Int(count).ascii())));
        attrs.ImpModify(TAG_XMI_version,       Sequence(L"0"));
        attrs.ImpModify(TAG_XMI_unSolvedFlag,  TAG_XMI_FALSE);
        attrs.ImpModify(TAG_XMI_ownerScope,    TAG_XMI_instance);
        attrs.ImpModify(TAG_XMI_targetScope,   TAG_XMI_instance);
        attrs.ImpModify(TAG_XMI_changeability, TAG_XMI_changeable);

        count++;

        TYPE_XMI_UMLAttribute a;
        a.Init(attrs,
               XmiAux::mk_UMLModelElement(),
               XmiAux::mk_UMLFeature(TAG_XMI_private),
               TYPE_XMI_UMLStructuralFeature().Init(Map(), Nil(), c),
               Nil(),
               XmiAux::mk_UMLAssociationEndRef(tid));

        se.set_qualifier(a);
      }
    }

    if (!rl.get_initval().IsNil()) {
      if (IsASTAH()) {
        se.set_initialValue(XmiAux::mk_UMLExpression(Sequence(MPP::MiniPP(rl.get_initval()))));
      }
    }
    if (IsEA()) {
      TYPE_XMI_UMLModelElement me (se.get_modelElement());
      Sequence taggedValue (me.get_taggedValue());
      taggedValue.ImpAppend(XmiAux::mk_UMLTaggedValue(TAG_XMI_containment, Sequence(L"Unspecified")));
      taggedValue.ImpAppend(XmiAux::mk_UMLTaggedValue(TAG_XMI_deststyle, Sequence(L"Navigable=Unspecified;Union=0;Derived=0;AllowDuplicates=0;Owned=0;")));
      taggedValue.ImpAppend(XmiAux::mk_UMLTaggedValue(TAG_XMI_ea_end, Sequence(L"target")));
      me.set_taggedValue(taggedValue);
      se.set_modelElement(me);

      Map attrs (se.get_attrs());
      attrs.ImpModify(TAG_XMI_type, XmiAux::GetID(tcref));
      se.set_attrs(attrs);
    }

//    ass.AppendConnection(se);
    connection.ImpAppend(se);
    ass.set_connection(connection);

    if (IsEA()) {
      TYPE_XMI_UMLModelElement me (ass.get_modelElement());
      Sequence taggedValue (me.get_taggedValue());
      taggedValue.ImpAppend(XmiAux::mk_UMLTaggedValue(TAG_XMI_ea_type, Sequence(L"Association")));
      me.set_taggedValue(taggedValue);
      ass.set_modelElement(me);
    }

    // append Association
    res.ImpAppend(ass);

    // append AssociationEnd
    Map tae_at;
    tae_at.ImpModify(TAG_XMI_idref, tid);
    res.ImpAppend(IXmiAssociationEnd(tae_at, Map()));

    // append AssociationEnd
    Map sae_at;
    sae_at.ImpModify(TAG_XMI_idref, sid);
    res.ImpAppend(IXmiAssociationEnd(sae_at, Map()));
  }
  return res;
}

TYPE_XMI_UMLMultiplicity XmiInterface::AUMLCard2XMIMultASTAH(const Quote & q)
{
  Sequence rid (XmiAux::UniqueID());
  IXmiMultiplicityRange range (rid);

  if ( q == QUOTE_ONE ) {
    range.SetLowerValue(Sequence(L"")); 
    range.SetUpperValue(Sequence(L"")); 
  }
  else if ( q == QUOTE_MANY ) {
    range.SetLowerValue(Sequence(L"0")); 
    range.SetUpperValue(Sequence(L"*")); 
  }
  else if ( q == QUOTE_ONE_TO_MANY ) {
    range.SetLowerValue(Sequence(L"1")); 
    range.SetUpperValue(Sequence(L"*")); 
  }
  else if ( q == QUOTE_ZERO_ONE ) {
    range.SetLowerValue(Sequence(L"0")); 
    range.SetUpperValue(Sequence(L"1")); 
  }
  else {
    range.SetLowerValue(Sequence(L"")); 
    range.SetUpperValue(Sequence(L"")); 
  }

//  Sequence mid (XmiAux::UniqueID());
//  IXmiMultiplicity m (mid);
//  m.AppendRange(range);
//  TYPE_XMI_UMLMultiplicitykk
//  return m;
  return XmiAux::mk_UMLMultiplicity(range);
}

/*
Quote XmiInterface::XMIMult2AUMLCardASTAH(const TYPE_XMI_UMLMultiplicity & mu)
{
  IXmiMultiplicity m (mu);
  Generic rg (m.GetRange());
  if (rg.IsNil())
    return QUOTE_ONE;

  IXmiMultiplicityRange r ((const TYPE_XMI_UMLMultiplicityRange & )rg);
  Sequence lower (r.GetLowerValue());
  Sequence upper (r.GetUpperValue());

  if ((lower == Sequence(L"0")) && (upper == Sequence(L"*")))
    return QUOTE_MANY;
  if ((lower == Sequence(L"1")) && (upper == Sequence(L"*")))
    return QUOTE_ONE_TO_MANY;
  if ((lower == Sequence(L"0")) && (upper == Sequence(L"1")))
    return QUOTE_ZERO_ONE;

  return QUOTE_ONE;
}
*/

Sequence XmiInterface::AUMLCard2XMIMult(const Quote & q)
{
  if ( q == QUOTE_ONE ) {
    return Sequence(L"1");
  }
  else if ( q == QUOTE_MANY ) {
    return Sequence(L"0..*");
  }
  else if ( q == QUOTE_ONE_TO_MANY ) {
    return Sequence(L"1..*");
  }
  else if ( q == QUOTE_ZERO_ONE ) {
    return Sequence(L"0..1");
  }
  else {
    return Sequence(L"1");
  }
}

Quote XmiInterface::XMIMult2AUMLCard(const Sequence & mu)
{
  if (mu == Sequence(L"0..*")) {
    return QUOTE_MANY;
  }
  if (mu == Sequence(L"1..*")) {
    return QUOTE_ONE_TO_MANY;
  }
  if (mu == Sequence(L"0..1")) {
    return QUOTE_ZERO_ONE;
  }
  return QUOTE_ONE;
}

bool XmiInterface::IsASTAH()
{
  return (this->mode == MODE_ASTAH);
}

bool XmiInterface::IsEA()
{
  return (this->mode == MODE_EnterpriseArchitect);
}

