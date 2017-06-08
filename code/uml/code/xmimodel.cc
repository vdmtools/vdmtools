#include "xmimodel.h"
#include "xmiaux.h"
#include "XMI.h"

//
// Class
//
IXmiClass::IXmiClass(const Sequence & name,
                     const Sequence & id,
                     const Sequence & pid)
 : TYPE_XMI_UMLClass()
{
  Map attrs;
  attrs.ImpModify(TAG_XMI_id,           id);
  attrs.ImpModify(TAG_XMI_name,         name);
  attrs.ImpModify(TAG_XMI_version,      Sequence(L"0"));
  attrs.ImpModify(TAG_XMI_unSolvedFlag, TAG_XMI_FALSE);
  attrs.ImpModify(TAG_XMI_isRoot,       TAG_XMI_FALSE);
  attrs.ImpModify(TAG_XMI_isLeaf,       TAG_XMI_FALSE);
  attrs.ImpModify(TAG_XMI_isAbstract,   TAG_XMI_FALSE);
  attrs.ImpModify(TAG_XMI_isActive,     TAG_XMI_FALSE);
  
  Map ns_attrs;
  ns_attrs.ImpModify(TAG_XMI_idref, pid);

  this->Init(attrs,
             XmiAux::mk_UMLModelElement(Map(), ns_attrs, TAG_XMI_public),
             XmiAux::mk_UMLNamespace(),
             TYPE_XMI_UMLGeneralizableElement().Init(Map(), Sequence()),
             Nil());
}

IXmiClass::IXmiClass(const Map & attrs, const Map & ns_attrs)
 : TYPE_XMI_UMLClass()
{
  this->Init(attrs,
             XmiAux::mk_UMLModelElement(Map(), ns_attrs, TAG_XMI_public),
             XmiAux::mk_UMLNamespace(),
             TYPE_XMI_UMLGeneralizableElement().Init(Map(), Sequence()),
             Nil());
}

void IXmiClass::SetAbstract(const Bool & b)
{
  Map attrs (this->get_attrs());
  if (b)
    attrs.ImpModify(TAG_XMI_isAbstract, TAG_XMI_TRUE);
  else
    attrs.ImpModify(TAG_XMI_isAbstract, TAG_XMI_FALSE);
  this->set_attrs(attrs);
}

Sequence IXmiClass::GetFeatures() const
{
  Generic cfg (this->get_classifier());

  if (cfg.IsNil())
    return Sequence();
  
  TYPE_XMI_UMLClassifier cf (cfg);
  return cf.get_feature();
}

Sequence IXmiClass::GetAttributes() const
{
  Sequence cf_l (this->GetFeatures());
  Sequence res;
  Generic g;
  for (bool bb = cf_l.First(g); bb; bb = cf_l.Next(g))
  {
    if (Record(g).Is(TAG_TYPE_XMI_UMLAttribute))
      res.ImpAppend(g);
  }
  return res;
}

Sequence IXmiClass::GetOperations() const
{
  Sequence cf_l (this->GetFeatures());
  Sequence res;
  Generic g;
  for (bool bb = cf_l.First(g); bb; bb = cf_l.Next(g))
  {
    if (Record(g).Is(TAG_TYPE_XMI_UMLOperation))
      res.ImpAppend(g);
  }
  return res;
}

void IXmiClass::AppendGeneralization(const TYPE_XMI_UMLGeneralization & g)
{
  TYPE_XMI_UMLGeneralizableElement ge (this->get_generalizableElement ());
  Sequence g_l (ge.get_generalization());
  g_l.ImpAppend(g);
  ge.set_generalization(g_l);
  this->set_generalizableElement(ge); 
}

void IXmiClass::AddTaggedValue(const TYPE_XMI_UMLTaggedValue & tv)
{ 
  TYPE_XMI_UMLModelElement me (this->get_modelElement ());
  Sequence tv_l (me.get_taggedValue());
  tv_l.ImpAppend(tv);
  me.set_taggedValue(tv_l);
  this->set_modelElement(me);
} 
  
//
// Attribute
//
IXmiAttribute::IXmiAttribute(const Sequence & class_name,
                             const Sequence & id,
                             const Sequence & pid)
 : TYPE_XMI_UMLAttribute()
{
  Map attrs;
  attrs.ImpModify(TAG_XMI_id,            id);
  attrs.ImpModify(TAG_XMI_name,          class_name);
  attrs.ImpModify(TAG_XMI_version,       Sequence(L"0"));
  attrs.ImpModify(TAG_XMI_unSolvedFlag,  TAG_XMI_FALSE);
  attrs.ImpModify(TAG_XMI_ownerScope,    TAG_XMI_instance);
  attrs.ImpModify(TAG_XMI_targetScope,   TAG_XMI_instance);
  attrs.ImpModify(TAG_XMI_changeability, TAG_XMI_changeable);
  
  Map ns_attrs;
  ns_attrs.ImpModify(TAG_XMI_idref, pid);

  TYPE_XMI_UMLClassifier owner;
  owner.Init(ns_attrs, Sequence());

  this->Init(attrs,
             XmiAux::mk_UMLModelElement(Map(), ns_attrs, TAG_XMI_public),
             TYPE_XMI_UMLFeature().Init(ns_attrs, owner, Nil()),
             XmiAux::mk_UMLStructuralFeature(),
             Nil(), Nil());
}

/*
Bool IXmiAttribute::IsStatic() const
{
  Map attrs (this->get_attrs());
  if (attrs.DomExists(TAG_XMI_ownerScope))
    return Bool(attrs[TAG_XMI_ownerScope] == TAG_XMI_classifier);
  else
    return Bool(false);
}
*/

void IXmiAttribute::SetStatic(const Bool & b)
{
  Map attrs (this->get_attrs());
  if (b)
    attrs.ImpModify(TAG_XMI_ownerScope, TAG_XMI_classifier);
  else
    attrs.ImpModify(TAG_XMI_ownerScope, TAG_XMI_instance);
  this->set_attrs(attrs);
}

/*
Bool IXmiAttribute::IsChangeable() const
{
  Map attrs (this->get_attrs());
  if (attrs.DomExists(TAG_XMI_changeability))
    return Bool(attrs[TAG_XMI_changeability] == TAG_XMI_changeable);
  else
    return Bool(true);
}
*/

void IXmiAttribute::SetChangeable(const Bool & b)
{
  Map attrs (this->get_attrs());
  if (b)
    attrs.ImpModify(TAG_XMI_changeability, TAG_XMI_changeable);
  else
    attrs.ImpModify(TAG_XMI_changeability, TAG_XMI_frozen);
  this->set_attrs(attrs);
}

/*
Sequence IXmiAttribute::GetVisibility() const
{
  Map attrs (this->get_attrs());
  if (attrs.DomExists(TAG_XMI_visibility))         // EA
    return attrs[TAG_XMI_visibility];
  else
    return this->get_feature().get_visibility ();  // ASTAH
}
*/

void IXmiAttribute::SetVisibility(const Sequence & v)
{
  // for EA
  Map attrs (this->get_attrs());
  attrs.ImpModify(TAG_XMI_visibility, v);
  this->set_attrs(attrs);

  // for ASTAH
  TYPE_XMI_UMLModelElement me (this->get_modelElement ());
  me.set_visibility(v);
  this->set_modelElement(me);
  TYPE_XMI_UMLFeature fe (this->get_feature ());
  fe.set_visibility(v);
  this->set_feature(fe);
}

Generic IXmiAttribute::GetTypeRef() const
{
  TYPE_XMI_UMLStructuralFeature sf (this->get_structuralFeature ());
  return sf.get_type();
}

void IXmiAttribute::SetTypeRef(const TYPE_XMI_UMLClassifier & t)
{
  TYPE_XMI_UMLStructuralFeature sf (this->get_structuralFeature ());
  sf.set_type(t);
  this->set_structuralFeature (sf);
}

//
// Operation
//
IXmiOperation::IXmiOperation(const Sequence & name,
                             const Sequence & id,
                             const Sequence & pid)
 : TYPE_XMI_UMLOperation()
{
  Map attrs;
  attrs.ImpModify(TAG_XMI_id,           id);
  attrs.ImpModify(TAG_XMI_name,         name);
  attrs.ImpModify(TAG_XMI_version,      Sequence(L"0"));
  attrs.ImpModify(TAG_XMI_unSolvedFlag, TAG_XMI_FALSE);
  attrs.ImpModify(TAG_XMI_ownerScope,   TAG_XMI_instance);
  attrs.ImpModify(TAG_XMI_isQuery,      TAG_XMI_FALSE);
  attrs.ImpModify(TAG_XMI_concurrency,  TAG_XMI_sequential);
  attrs.ImpModify(TAG_XMI_isRoot,       TAG_XMI_FALSE);
  attrs.ImpModify(TAG_XMI_isLeaf,       TAG_XMI_FALSE);
  attrs.ImpModify(TAG_XMI_isAbstract,   TAG_XMI_FALSE);
  
  Map ns_attrs;
  ns_attrs.ImpModify(TAG_XMI_idref, pid);

  TYPE_XMI_UMLClassifier owner;
  owner.Init(ns_attrs, Sequence());

  this->Init(attrs,
             XmiAux::mk_UMLModelElement(Map(), ns_attrs, TAG_XMI_public),
             TYPE_XMI_UMLFeature().Init(ns_attrs, owner, Nil()),
             TYPE_XMI_UMLBehavioralFeature().Init(Map(), Sequence()));
}

Sequence IXmiOperation::GetParameters() const
{
  return this->get_behavioralFeature ().get_parameter ();
}

IXmiParameter IXmiOperation::GetReturnType() const
{
  Sequence p_l (this->GetParameters());
  Generic g;
  for (bool bb = p_l.First(g); bb; bb = p_l.Next(g))
  {
    IXmiParameter p (g);
    if (p.IsReturn())
      return p;
  }
  return TYPE_XMI_UMLParameter();
}

Sequence IXmiOperation::GetParam() const
{
  Sequence p_l (this->GetParameters());
  Sequence res;
  Generic g;
  for (bool bb = p_l.First(g); bb; bb = p_l.Next(g))
  {
    IXmiParameter p (g);
    if (!p.IsReturn())
      res.ImpAppend(p);
  }
  return res;
}

Sequence IXmiOperation::GetStereotypes() const
{
  TYPE_XMI_UMLModelElement me (this->get_modelElement ());
  return me.get_stereotype ();
}

void IXmiOperation::AppendStereotype(const TYPE_XMI_UMLStereotype & st)
{
  TYPE_XMI_UMLModelElement me (this->get_modelElement ());
  Sequence st_l (me.get_stereotype());
  st_l.ImpAppend(st);
  me.set_stereotype(st_l);
  this->set_modelElement(me);
}

void IXmiOperation::SetStatic(const Bool & b)
{
  Map attrs (this->get_attrs());
  if (b)
    attrs.ImpModify(TAG_XMI_ownerScope, TAG_XMI_classifier);
  else
    attrs.ImpModify(TAG_XMI_ownerScope, TAG_XMI_instance);
  this->set_attrs(attrs);
}

Bool IXmiOperation::IsAbstract()
{
  Map attrs (this->get_attrs());
  if (attrs.DomExists(TAG_XMI_isAbstract))
    return Bool(attrs[TAG_XMI_isAbstract] == TAG_XMI_TRUE);
  else
    return Bool(false);
}

void IXmiOperation::SetAbstract(const Bool & b)
{
  Map attrs (this->get_attrs());
  if (b)
    attrs.ImpModify(TAG_XMI_isAbstract, TAG_XMI_TRUE);
  else
    attrs.ImpModify(TAG_XMI_isAbstract, TAG_XMI_FALSE);
  this->set_attrs(attrs);
}

Sequence IXmiOperation::GetVisibility()
{
  Map attrs (this->get_attrs());
  if (attrs.DomExists(TAG_XMI_visibility))        // EA
    return attrs[TAG_XMI_visibility];
  else
    return this->get_feature().get_visibility (); // ASTAH
}

void IXmiOperation::SetVisibility(const Sequence & v)
{
  // for EA
  Map attrs (this->get_attrs());
  attrs.ImpModify(TAG_XMI_visibility, v);
  this->set_attrs(attrs);

  // for ASTAH
  TYPE_XMI_UMLModelElement me (this->get_modelElement ());
  me.set_visibility(v);
  this->set_modelElement(me);
  TYPE_XMI_UMLFeature fe (this->get_feature ());
  fe.set_visibility(v);
  this->set_feature(fe);
}

void IXmiOperation::AddTaggedValue(const TYPE_XMI_UMLTaggedValue & tv)
{
  TYPE_XMI_UMLModelElement me (this->get_modelElement ());
  Sequence tv_l (me.get_taggedValue());
  tv_l.ImpAppend(tv);
  me.set_taggedValue(tv_l);
  this->set_modelElement(me);
}

void IXmiOperation::AddConstraint(const TYPE_XMI_UMLConstraint & co)
{
  TYPE_XMI_UMLModelElement me (this->get_modelElement ());
  Sequence co_l (me.get_constraint());
  co_l.ImpAppend(co);
  me.set_constraint(co_l);
  this->set_modelElement(me);
}

//
// Parameter
//
IXmiParameter::IXmiParameter(const Sequence & name, const Sequence & id,
                             const Sequence & opid, const Sequence & clid)
 : TYPE_XMI_UMLParameter()
{
  Map attrs;
  attrs.ImpModify(TAG_XMI_id,           id);
  attrs.ImpModify(TAG_XMI_name,         name);
  attrs.ImpModify(TAG_XMI_version,      Sequence(L"0"));
  attrs.ImpModify(TAG_XMI_unSolvedFlag, TAG_XMI_FALSE);
  attrs.ImpModify(TAG_XMI_kind,         TAG_XMI_in);

  Map ns_attrs;
  ns_attrs.ImpModify(TAG_XMI_idref, clid);

  Map bf_attrs;
  bf_attrs.ImpModify(TAG_XMI_idref, opid);

  this->Init(attrs,
             XmiAux::mk_UMLModelElement(Map(), ns_attrs, TAG_XMI_public),
             TYPE_XMI_UMLBehavioralFeature().Init(bf_attrs, Sequence()), // Parameter.behavioralFeature
             Nil(),  // Parameter.type
             Nil()); // Parameter.defaultValue
}

Bool IXmiParameter::IsReturn() const
{
  Map attrs (this->get_attrs());
  if (attrs.DomExists(TAG_XMI_kind))
    return Bool(attrs[TAG_XMI_kind] == TAG_XMI_return);
  else
    return Bool(false);
}

void IXmiParameter::SetReturn()
{
  Map attrs (this->get_attrs());
  attrs.ImpModify(TAG_XMI_kind, TAG_XMI_return);
  this->set_attrs(attrs);
}

Generic IXmiParameter::GetTypeRef() const
{
  return this->get_type();
}

void IXmiParameter::SetTypeRef(const TYPE_XMI_UMLClassifier & t)
{
  this->set_type(t);
}

//
// AssociationEnd
//
IXmiAssociationEnd::IXmiAssociationEnd(const Map & attrs, const Map & ns_attrs)
 : TYPE_XMI_UMLAssociationEnd()
{
  this->Init(attrs, 
             XmiAux::mk_UMLModelElement(Map(), ns_attrs, TAG_XMI_public),
             XmiAux::mk_UMLFeature(),
             XmiAux::mk_UMLStructuralFeature(),
             Nil(), Nil(), Nil(), Nil(), Nil());
}

IXmiAssociationEnd::IXmiAssociationEnd(const Sequence & name, const Sequence & id, const Sequence & pid)
 : TYPE_XMI_UMLAssociationEnd()
{
  Map attrs;
  attrs.ImpModify(TAG_XMI_id,            id);
  attrs.ImpModify(TAG_XMI_name,          name);
  attrs.ImpModify(TAG_XMI_version,       Sequence(L"0"));
  attrs.ImpModify(TAG_XMI_unSolvedFlag,  TAG_XMI_FALSE);
  attrs.ImpModify(TAG_XMI_ownerScope,    TAG_XMI_instance);
  attrs.ImpModify(TAG_XMI_targetScope,   TAG_XMI_instance);
  attrs.ImpModify(TAG_XMI_changeability, TAG_XMI_changeable);
  attrs.ImpModify(TAG_XMI_aggregation,   TAG_XMI_none);
  attrs.ImpModify(TAG_XMI_ordering,      TAG_XMI_unordered);
  attrs.ImpModify(TAG_XMI_navigableType, TAG_XMI_unspecified_navigable);
  attrs.ImpModify(TAG_XMI_isNavigable,   TAG_XMI_FALSE);

  // for EA
  attrs.ImpModify(TAG_XMI_visibility, TAG_XMI_private);
  
  Map ns_attrs;
  ns_attrs.ImpModify(TAG_XMI_idref, pid);

  Sequence visibility (TAG_XMI_private);

  this->Init(attrs, 
             XmiAux::mk_UMLModelElement(Map(), ns_attrs, visibility),
             XmiAux::mk_UMLFeature(visibility),
             XmiAux::mk_UMLStructuralFeature(),
             Nil(), Nil(), Nil(), visibility, Nil());

//  this->SetVisibility(TAG_XMI_private);
}

void IXmiAssociationEnd::SetStatic(const Bool & b)
{
  Map attrs (this->get_attrs());
  if (b)
    attrs.ImpModify(TAG_XMI_targetScope, TAG_XMI_classifier);
  else
    attrs.ImpModify(TAG_XMI_targetScope, TAG_XMI_instance);
  this->set_attrs(attrs);
}

void IXmiAssociationEnd::SetVisibility(const Sequence & v)
{
  // for EA
  Map attrs (this->get_attrs());
  attrs.ImpModify(TAG_XMI_visibility, v);
  this->set_attrs(attrs);

  // for ASTAH
  TYPE_XMI_UMLModelElement me (this->get_modelElement ());
  me.set_visibility(v);
  this->set_modelElement(me);
  TYPE_XMI_UMLFeature fe (this->get_feature ());
  fe.set_visibility(v);
  this->set_feature(fe);
  this->set_visibility(v);
}

void IXmiAssociationEnd::SetOwner(const TYPE_XMI_UMLClassifier & c)
{
  TYPE_XMI_UMLFeature fe (this->get_feature());
  fe.set_owner(c);
  this->set_feature(fe);
}

void IXmiAssociationEnd::SetMultiplicity(const TYPE_XMI_UMLMultiplicity & m)
{
  TYPE_XMI_UMLStructuralFeature sf (this->get_structuralFeature());
  sf.set_multiplicity(m);
  this->set_structuralFeature(sf);
}

void IXmiAssociationEnd::SetMultiplicityEA(const Sequence & m)
{
  Map attrs (this->get_attrs());
  attrs.ImpModify(TAG_XMI_multiplicity, m);
  this->set_attrs(attrs);
}

Bool IXmiAssociationEnd::IsNavigable()
{
  Map attrs (this->get_attrs());
  if (attrs.DomExists(TAG_XMI_isNavigable))
    return Bool(attrs[TAG_XMI_isNavigable] == TAG_XMI_TRUE);
  else
    return Bool(false);
}

void IXmiAssociationEnd::SetNavigable(const Bool & b)
{
  Map attrs (this->get_attrs());
  if (b)
  {
    attrs.ImpModify(TAG_XMI_isNavigable, TAG_XMI_TRUE);
    attrs.ImpModify(TAG_XMI_navigableType, TAG_XMI_navigable);
  }
  else
  {
    attrs.ImpModify(TAG_XMI_isNavigable, TAG_XMI_FALSE);
    attrs.ImpModify(TAG_XMI_navigableType, TAG_XMI_non_navigable);
  }
  this->set_attrs(attrs);
}

void IXmiAssociationEnd::SetOrdered(const Bool & b)
{
  Map attrs (this->get_attrs());
  if (b)
    attrs.ImpModify(TAG_XMI_ordering, TAG_XMI_ordered);
  else
    attrs.ImpModify(TAG_XMI_ordering, TAG_XMI_unordered);
  this->set_attrs(attrs);
}

//
// Association
//
IXmiAssociation::IXmiAssociation(const Map & attrs, const Map & ns_attrs)
{
  this->Init(attrs, 
             XmiAux::mk_UMLModelElement(Map(), ns_attrs, TAG_XMI_public),
             Sequence());
}

IXmiAssociation::IXmiAssociation(const Sequence & name, const Sequence & id, const Sequence & pid)
{
  Map attrs;
  attrs.ImpModify(TAG_XMI_id,            id);
  attrs.ImpModify(TAG_XMI_name,          name);
  attrs.ImpModify(TAG_XMI_version,       Sequence(L"0"));
  attrs.ImpModify(TAG_XMI_unSolvedFlag,  TAG_XMI_FALSE);
  
  Map ns_attrs;
  ns_attrs.ImpModify(TAG_XMI_idref, pid);

  this->Init(attrs, 
             XmiAux::mk_UMLModelElement(Map(), ns_attrs, TAG_XMI_public),
             Sequence());
}

/*
void IXmiAssociation::AppendConnection(const TYPE_XMI_UMLAssociationEnd & ae)
{
  Sequence co (this->get_connection());
  co.ImpAppend(ae);
  this->set_connection(co);
}
*/

//
// MultiplicityRange
//
IXmiMultiplicityRange::IXmiMultiplicityRange(const Sequence & id)
 : TYPE_XMI_UMLMultiplicityRange()
{
  Map attrs;
  attrs.ImpModify(TAG_XMI_id,            id);
  attrs.ImpModify(TAG_XMI_lowerValue,    Sequence(L""));
  attrs.ImpModify(TAG_XMI_upperValue,    Sequence(L""));

  this->Init(attrs);
}

Sequence IXmiMultiplicityRange::GetLowerValue()
{
  Map attrs (this->get_attrs());
  if (attrs.DomExists(TAG_XMI_lowerValue))
    return attrs[TAG_XMI_lowerValue];
  else
    return Sequence(L"");
}

void IXmiMultiplicityRange::SetLowerValue(const Sequence & v)
{
  Map attrs (this->get_attrs());
  attrs.ImpModify(TAG_XMI_lowerValue, v);
  this->set_attrs(attrs);
}

Sequence IXmiMultiplicityRange::GetUpperValue()
{
  Map attrs (this->get_attrs());
  if (attrs.DomExists(TAG_XMI_upperValue))
    return attrs[TAG_XMI_upperValue];
  else
    return Sequence(L"");
}

void IXmiMultiplicityRange::SetUpperValue(const Sequence & v)
{
  Map attrs (this->get_attrs());
  attrs.ImpModify(TAG_XMI_upperValue, v);
  this->set_attrs(attrs);
}

