#include "metaiv.h"
#include "XMI.h"
#include "xmiaux.h"
#include "tb_wstring.h"
#include "parse_string.h"

#include <time.h>
#ifndef _MSC_VER
#include <sys/time.h>
#endif // _MSC_VER

unsigned long XmiAux::uid = 0;

Sequence XmiAux::UniqueID()
{
  time_t tm;
  time(&tm);
  char buf [40];
  sprintf(buf, "VDMT%010lx%010lx", uid, tm);
  XmiAux::uid++;
  return Sequence(TBWSTR::string2wstring(string(buf)));
}

Sequence XmiAux::GetName(const Generic & g)
{
  if (g.IsRecord())
  {
    Map attrs (GetAttribute(g));
    if (attrs.DomExists(TAG_XMI_name))
      return attrs[TAG_XMI_name];
    else
      return Sequence (L"");
  }
  return Sequence(L"");
}

bool XmiAux::IsRef(const Generic & g)
{
  if (g.IsRecord())
  {
    Map attrs (GetAttribute(g));
    return attrs.DomExists(TAG_XMI_idref);
  }
  return false;
}

Sequence XmiAux::GetID(const Generic & g)
{
  if (g.IsRecord())
  {
    Map attrs (GetAttribute(g));
    if (attrs.DomExists(TAG_XMI_id))
      return attrs[TAG_XMI_id];
    else if (attrs.DomExists(TAG_XMI_idref))
      return attrs[TAG_XMI_idref];
    else
      return Sequence (L"");
  }

  return Sequence(L"");
}

Map XmiAux::GetAttribute(const Record & r)
{
  Map attrs;
  switch (r.GetTag()) {
    case TAG_TYPE_XMI_UMLModel: {
      attrs = r.GetMap(pos_XMI_UMLModel_attrs);
      break;
    }
    case TAG_TYPE_XMI_UMLClass: {
      attrs = r.GetMap(pos_XMI_UMLClass_attrs);
      break;
    }
/*
    case TAG_TYPE_XMI_UMLPackage: {
      attrs = r.GetMap(pos_XMI_UMLPackage_attrs);
      break;
    }
    case TAG_TYPE_XMI_UMLInterface: {
      attrs = r.GetMap(pos_XMI_UMLInterface_attrs);
      break;
    }
*/
    case TAG_TYPE_XMI_UMLAttribute: {
      attrs = r.GetMap(pos_XMI_UMLAttribute_attrs);
      break;
    }
    case TAG_TYPE_XMI_UMLOperation: {
      attrs = r.GetMap(pos_XMI_UMLOperation_attrs);
      break;
    }
    case TAG_TYPE_XMI_UMLPrimitive: {
      attrs = r.GetMap(pos_XMI_UMLPrimitive_attrs);
      break;
    }
    case TAG_TYPE_XMI_UMLStereotype: {
      attrs = r.GetMap(pos_XMI_UMLStereotype_attrs);
      break;
    }
    case TAG_TYPE_XMI_UMLParameter: {
      attrs = r.GetMap(pos_XMI_UMLParameter_attrs);
      break;
    }
    case TAG_TYPE_XMI_UMLAssociationEnd: {
      attrs = r.GetMap(pos_XMI_UMLAssociationEnd_attrs);
      break;
    }
    case TAG_TYPE_XMI_EAStub: {
      attrs = r.GetMap(pos_XMI_EAStub_attrs);
      break;
    }
    default: {
      attrs = r.GetMap(1);
      break;
    }
  }
  return attrs;
}

Sequence XmiAux::GetElementName(const Generic & rg, const Map & idm)
{ 
  if (rg.IsNil())
  {
     return Sequence(L"");
  }
  else if (rg.IsSequence())
  {
    if (idm.DomExists(rg))
    {
      return GetName(idm[rg]);
    }
    return Sequence();
  }
  else
  {
    if (IsRef(rg))
    {
      Sequence id (GetID(rg));

      if (idm.DomExists(id))
      {
        return GetName(idm[id]);
      }
      return Sequence();
    }
    else
      return GetName(rg);
  }
}

bool XmiAux::IsPrimitive(const Sequence & t)
{
  if ((Sequence(L"int") == t) ||
      (Sequence(L"bool") == t) ||
      (Sequence(L"char") == t) ||
      (Sequence(L"void") == t))
    return true;
  else
    return false;
}

Sequence XmiAux::GetAllElements(const TYPE_XMI_UMLModel & m)
{
  if (IsEAModel(m))
  {
    // Enterprise Architect
    Sequence s (m.get_namespace().get_ownedElement());
    if (s.Length() >= 2)
    {
      Record r (s[2]);
      if (r.Is(TAG_TYPE_XMI_UMLPackage))
      {
        TYPE_XMI_UMLPackage p (r);
        return (p.get_namespace().get_ownedElement());
      }
    }
    return Sequence();
  }
  else
  {
    // ASTAH
    return m.get_namespace().get_ownedElement();
  }
}

SEQ<TYPE_XMI_UMLClass> XmiAux::GetAllClasses(const TYPE_XMI_UMLModel & m)
{
  Sequence elm_l (GetAllElements(m));
  
  SEQ<TYPE_XMI_UMLClass> res;
  Generic elm;
  for (bool bb = elm_l.First(elm); bb; bb = elm_l.Next(elm))
  {
    if (Record(elm).Is(TAG_TYPE_XMI_UMLClass))
    {
        res.ImpAppend(elm);
    }
  }
  return res;
}

SEQ<TYPE_XMI_UMLGeneralization> XmiAux::GetAllGeneralizations(const TYPE_XMI_UMLModel & m)
{
  Sequence elm_l (GetAllElements(m));

  SEQ<TYPE_XMI_UMLGeneralization> res;
  Generic elm;
  for (bool bb = elm_l.First(elm); bb; bb = elm_l.Next(elm))
  {
    if (Record(elm).Is(TAG_TYPE_XMI_UMLGeneralization))
    {
      res.ImpAppend(elm);
    }
  }
  return res;
}

SEQ<TYPE_XMI_UMLAssociation> XmiAux::GetAllAssociation(const TYPE_XMI_UMLModel & m)
{
  Sequence elm_l (GetAllElements(m));
  
  SEQ<TYPE_XMI_UMLAssociation> res;
  Generic elm;
  for (bool bb = elm_l.First(elm); bb; bb = elm_l.Next(elm))
  {
    if (Record(elm).Is(TAG_TYPE_XMI_UMLAssociation))
    {
      res.ImpAppend(elm);
    }
  }
  return res;
}

SEQ<TYPE_XMI_UMLAssociationEnd> XmiAux::GetAllAssociationEnds(const TYPE_XMI_UMLModel & m)
{
  Sequence elm_l (GetAllElements(m));
  
  SEQ<TYPE_XMI_UMLAssociationEnd> res;
  Generic elm;
  for (bool bb = elm_l.First(elm); bb; bb = elm_l.Next(elm))
  {
/*
    if (Record(elm).Is(TAG_TYPE_XMI_UMLAssociationEnd))
    {
      res.ImpAppend(elm);
    }
*/
    if (Record(elm).Is(TAG_TYPE_XMI_UMLAssociation))
    {
      TYPE_XMI_UMLAssociation as (elm);
      Sequence ae_l (as.get_connection());
      Generic aeg;
      for (bool cc = ae_l.First(aeg); cc; cc = ae_l.Next(aeg))
      {
        if (Record (aeg).Is(TAG_TYPE_XMI_UMLAssociationEnd))
        {
          TYPE_XMI_UMLAssociationEnd ae (aeg);
          Map attrs (ae.get_attrs());
          if (attrs.DomExists(TAG_XMI_name))
          {
            Sequence nm (attrs[TAG_XMI_name]);
            if (!nm.IsEmpty())
            {
              res.ImpAppend(ae);
            }
          }
        }
      }
    }
  }
  return res;
}

SEQ<TYPE_XMI_UMLAttribute> XmiAux::GetAllAttributes(const TYPE_XMI_UMLClass & c)
{
  Generic cfg (c.get_classifier());

  if (cfg.IsNil())
    return Sequence();
 
  TYPE_XMI_UMLClassifier cf (cfg);
  Sequence cf_l (cf.get_feature());

  SEQ<TYPE_XMI_UMLAttribute> res;
  Generic g;
  for (bool bb = cf_l.First(g); bb; bb = cf_l.Next(g))
  {
    if (Record(g).Is(TAG_TYPE_XMI_UMLAttribute))
      res.ImpAppend(g);
  }
  return res;
}

SEQ<TYPE_XMI_UMLOperation> XmiAux::GetAllOperations(const TYPE_XMI_UMLClass & c)
{
  Generic cfg (c.get_classifier());

  if (cfg.IsNil())
    return Sequence();
 
  TYPE_XMI_UMLClassifier cf (cfg);
  Sequence cf_l (cf.get_feature());

  SEQ<TYPE_XMI_UMLOperation> res;
  Generic g;
  for (bool bb = cf_l.First(g); bb; bb = cf_l.Next(g))
  {
    if (Record(g).Is(TAG_TYPE_XMI_UMLOperation))
      res.ImpAppend(g);
  }
  return res;
}

Sequence XmiAux::GetClasses(const TYPE_XMI_UMLModel & m)
{ 
  Sequence c_l (GetAllClasses(m));
  Sequence res;
  Generic g;
  for (bool bb = c_l.First(g); bb; bb = c_l.Next(g))
  { 
    TYPE_XMI_UMLClass c (g);
    Sequence nm (GetName(c));
    if ((nm != Sequence(L"void")) && IsRealClassName(nm) && IsClass(c))
      res.ImpAppend(c);
  }
  return res;
}

bool XmiAux::IsRealClassName(const Sequence & nm)
{
  Tuple t (UMLPARSE::Seq2Type(nm));
  if (t.GetBool(1))
  {
    TYPE_AS_Type tp (t.GetRecord(2));
    if (tp.Is(TAG_TYPE_AS_TypeName))
    {
      TYPE_AS_Name name (tp.GetRecord(pos_AS_TypeName_name));
      return (name.get_ids().Length() == 1);
    }
  }
  return false;
}

bool XmiAux::IsFunction(const TYPE_XMI_UMLOperation & o, const Map & idm)
{
  Sequence st_l (o.get_modelElement().get_stereotype());
  Generic g;
  for (bool bb = st_l.First(g); bb; bb = st_l.Next(g))
  {
    if (GetElementName(g, idm) == TAG_XMI_function)
      return true;
    else if (GetElementName(g, idm) == TAG_XMI_operation)
      return false;
  }
  return false; // default
}

bool XmiAux::IsOperation(const TYPE_XMI_UMLOperation & o, const Map & idm)
{
  Sequence st_l (o.get_modelElement().get_stereotype());
  Generic g;
  for (bool bb = st_l.First(g); bb; bb = st_l.Next(g))
  {
    if (GetElementName(g, idm) == TAG_XMI_operation)
      return true;
    else if (GetElementName(g, idm) == TAG_XMI_function)
      return false;
  }
//  return false;
  return true; // default
}

bool XmiAux::IsImpl(const TYPE_XMI_UMLOperation & o, const Map & idm)
{
  return (GetDefType(o) == TAG_XMI_impl);
}

bool XmiAux::IsExtExpl(const TYPE_XMI_UMLOperation & o, const Map & idm)
{
  return (GetDefType(o) == TAG_XMI_extexpl);
}

bool XmiAux::IsPartial(const TYPE_XMI_UMLOperation & o, const Map & idm)
{
  return (GetFnType(o) == TAG_XMI_partial);
}

TYPE_XMI_UMLClassifier XmiAux::GetTypeRef(const TYPE_XMI_UMLAttribute & a)
{
  TYPE_XMI_UMLStructuralFeature sf (a.get_structuralFeature ());
  if (!sf.get_type().IsNil())
    return sf.get_type();
  else
    return TYPE_XMI_UMLClassifier().Init(Map(), Sequence());  // not occur
}

TYPE_XMI_UMLClassifier XmiAux::GetTypeRef(const TYPE_XMI_UMLParameter & p)
{
  if (!p.get_type().IsNil())
    return p.get_type();
  else
    return TYPE_XMI_UMLClassifier().Init(Map(), Sequence()); // not occur
}

bool XmiAux::IsReturnType(const TYPE_XMI_UMLParameter & p)
{
  Map attrs (p.get_attrs());
  if (attrs.DomExists(TAG_XMI_kind))
    return (attrs[TAG_XMI_kind] == TAG_XMI_return);
  else
    return false;
}

SEQ<TYPE_XMI_UMLParameter> XmiAux::GetParam(const TYPE_XMI_UMLOperation & o)
{
  SEQ<TYPE_XMI_UMLParameter> p_l (o.get_behavioralFeature().get_parameter());
  SEQ<TYPE_XMI_UMLParameter> res;
  Generic g;
  for (bool bb = p_l.First(g); bb; bb = p_l.Next(g))
  {
    TYPE_XMI_UMLParameter p (g);
    if (!IsReturnType(p))
      res.ImpAppend(p);
  }
  return res;
}

TYPE_XMI_UMLParameter XmiAux::GetReturnType(const TYPE_XMI_UMLOperation & o)
{
  SEQ<TYPE_XMI_UMLParameter> p_l (o.get_behavioralFeature().get_parameter());
  Generic g;
  for (bool bb = p_l.First(g); bb; bb = p_l.Next(g))
  {
    TYPE_XMI_UMLParameter p (g);
    if (IsReturnType(p))
      return p;
  }
  return TYPE_XMI_UMLParameter();
}

Sequence XmiAux::GetPreCondition(const TYPE_XMI_UMLOperation & o)
{
  // EA
  Sequence c_l (o.get_modelElement().get_constraint());
  if (!c_l.IsEmpty())
  {
    Generic g;
    for (bool bb = c_l.First(g); bb; bb = c_l.Next(g))
    {
      TYPE_XMI_UMLConstraint co (g);
      if (SearchTagValue(TAG_XMI_constraint_type, co.get_modelElement())
            == Sequence(L"precondition"))
      {
        Map attrs (co.get_attrs());
        if (attrs.DomExists(TAG_XMI_name))
          return attrs[TAG_XMI_name];
        else
          return Sequence();
      }
    }
    return Sequence();
  }
  else
    return SearchTagValue(TAG_XMI_precondition, o.get_modelElement()); // ASTAH
}

Sequence XmiAux::GetPostCondition(const TYPE_XMI_UMLOperation & o)
{
  // EA
  Sequence c_l (o.get_modelElement().get_constraint());
  if (!c_l.IsEmpty())
  {
    Generic g;
    for (bool bb = c_l.First(g); bb; bb = c_l.Next(g))
    {
      TYPE_XMI_UMLConstraint co (g);
      if (SearchTagValue(TAG_XMI_constraint_type, co.get_modelElement())
            == Sequence(L"postcondition"))
      {
        Map attrs (co.get_attrs());
        if (attrs.DomExists(TAG_XMI_name))
          return attrs[TAG_XMI_name];
        else
          return Sequence();
      }
    }
    return Sequence();
  }
  else
  return SearchTagValue(TAG_XMI_postcondition, o.get_modelElement());
}

Sequence XmiAux::GetFnType(const TYPE_XMI_UMLOperation & o)
{
  return SearchTagValue(TAG_XMI_fntype, o.get_modelElement());
}

Sequence XmiAux::GetDefType(const TYPE_XMI_UMLOperation & o)
{
  return SearchTagValue(TAG_XMI_deftype, o.get_modelElement());
}

Sequence XmiAux::SearchTagValue(const Sequence & key, const TYPE_XMI_UMLModelElement & me)
{
  Sequence tv_l (me.get_taggedValue());
  Generic g;
  for (bool bb = tv_l.First(g); bb; bb = tv_l.Next(g))
  {
    TYPE_XMI_UMLTaggedValue tv (g);
    Map attrs (tv.get_attrs());
    if (attrs.DomExists(TAG_XMI_tag))
      if (attrs[TAG_XMI_tag] == key)
        return attrs[TAG_XMI_value];
  }
  return Sequence(L"");
}

bool XmiAux::HasTagValue(const Sequence & key, const TYPE_XMI_UMLModelElement & me)
{
  Sequence tv_l (me.get_taggedValue());
  Generic g;
  for (bool bb = tv_l.First(g); bb; bb = tv_l.Next(g))
  {
    TYPE_XMI_UMLTaggedValue tv (g);
    Map attrs (tv.get_attrs());
    if (attrs.DomExists(TAG_XMI_tag))
      if (attrs[TAG_XMI_tag] == key)
        return true;
  }
  return false;
}

bool XmiAux::IsClass(const TYPE_XMI_UMLClass & c)
{
  Sequence ic (SearchTagValue(TAG_XMI_isClass, c.get_modelElement()));
  return !(ic == TAG_XMI_FALSE);
}

bool XmiAux::IsAbstract(const TYPE_XMI_UMLClass & c)
{
  Map attrs (c.get_attrs());
  if (attrs.DomExists(TAG_XMI_isAbstract))
    return Bool(attrs[TAG_XMI_isAbstract] == TAG_XMI_TRUE);
  else
    return Bool(false);
}

Bool XmiAux::IsStatic(const TYPE_XMI_UMLOperation & o)
{
  Map attrs (o.get_attrs());
  if (attrs.DomExists(TAG_XMI_ownerScope))
    return Bool(attrs[TAG_XMI_ownerScope] == TAG_XMI_classifier);
  else
    return Bool(false);
}

Sequence XmiAux::GetVisibility(const TYPE_XMI_UMLOperation & o)
{
  Map attrs (o.get_attrs());
  if (attrs.DomExists(TAG_XMI_visibility))
    return attrs[TAG_XMI_visibility];         // EA
  else
    return o.get_feature().get_visibility (); // ASTAH
}

bool XmiAux::IsVoid(const TYPE_XMI_UMLParameter & p, const Map & idm)
{
  return (GetElementName(GetTypeRef(p), idm) == Sequence(L"void"));
}

Bool XmiAux::IsStatic(const TYPE_XMI_UMLAttribute & a)
{
  Map attrs (a.get_attrs());
  if (attrs.DomExists(TAG_XMI_ownerScope))
    return Bool(attrs[TAG_XMI_ownerScope] == TAG_XMI_classifier);
  else
    return Bool(false);
}

Sequence XmiAux::GetVisibility(const TYPE_XMI_UMLAttribute & a)
{
  Map attrs (a.get_attrs());
  if (attrs.DomExists(TAG_XMI_visibility))
    return attrs[TAG_XMI_visibility];         // EA
  else
    return a.get_feature().get_visibility (); // ASTAH
}

bool XmiAux::IsChangeable(const TYPE_XMI_UMLAttribute & a)
{
  Map attrs (a.get_attrs());
  if (attrs.DomExists(TAG_XMI_changeability))
    return (attrs[TAG_XMI_changeability] == TAG_XMI_changeable);
  else
    return true;
}

Bool XmiAux::IsStatic(const TYPE_XMI_UMLAssociationEnd & ae,
                      const Map & idm)
{
  Map attrs (ae.get_attrs());
  if (attrs.DomExists(TAG_XMI_targetScope))
    return Bool(attrs[TAG_XMI_targetScope] == TAG_XMI_classifier);

  Sequence id (GetID(ae));
  if (idm.DomExists(id))
  {
    TYPE_XMI_UMLAssociationEnd nae (idm[id]);
    Map attrs (nae.get_attrs());
    if (attrs.DomExists(TAG_XMI_targetScope))
      return Bool(attrs[TAG_XMI_targetScope] == TAG_XMI_classifier);
  }
   
  return Bool(false);
}

Sequence XmiAux::GetVisibility(const TYPE_XMI_UMLAssociationEnd & ae,
                               const Map & idm)
{
  Map attrs (ae.get_attrs());
  if (attrs.DomExists(TAG_XMI_visibility))        // EA
    return attrs[TAG_XMI_visibility];

  // ASTAH
  Sequence id (GetID(ae));
  if (idm.DomExists(id))
  {
    TYPE_XMI_UMLAssociationEnd nae (idm[id]);
    return nae.get_feature().get_visibility ();
  }
  return Sequence();
}

Sequence XmiAux::GetMultiplicity(const TYPE_XMI_UMLAssociationEnd & ae,
                                 const Map & idm)
{
  // for EA
  Map attrs (ae.get_attrs());
  if (attrs.DomExists(TAG_XMI_multiplicity))
    return attrs[TAG_XMI_multiplicity];

  // for ASTAH
  Sequence id (GetID(ae));
  if (idm.DomExists(id))
  {
    TYPE_XMI_UMLAssociationEnd nae (idm[id]);
    TYPE_XMI_UMLStructuralFeature sf (nae.get_structuralFeature());
    Generic m_g (sf.get_multiplicity());
  
    if (m_g.IsNil()) return Sequence(L"1");

    TYPE_XMI_UMLMultiplicity m (m_g);
    Generic rg (m.get_range());
    if (rg.IsNil())
      return Sequence(L"1"); 

    TYPE_XMI_UMLMultiplicityRange r (rg);
    Map rg_attrs (r.get_attrs());
    Sequence lower;
    if (rg_attrs.DomExists(TAG_XMI_lowerValue))
      lower = rg_attrs[TAG_XMI_lowerValue];
    if (lower.IsEmpty())
    {
      if (rg_attrs.DomExists(TAG_XMI_lower))
      lower = rg_attrs[TAG_XMI_lower];
    }
    Sequence upper;
    if (rg_attrs.DomExists(TAG_XMI_upperValue))
      upper = rg_attrs[TAG_XMI_upperValue];
    if (upper.IsEmpty())
    {
      if (rg_attrs.DomExists(TAG_XMI_upper))
        upper = rg_attrs[TAG_XMI_upper];
    }

    if ((lower == Sequence(L"0")) && (upper == Sequence(L"*")))
      return Sequence(L"0..*");
    if ((lower == Sequence(L"0")) && (upper == Sequence(L"-1")))
      return Sequence(L"0..*");
    if ((lower == Sequence(L"1")) && (upper == Sequence(L"*")))
      return Sequence(L"1..*");
    if ((lower == Sequence(L"1")) && (upper == Sequence(L"-1")))
      return Sequence(L"1..*");
    if ((lower == Sequence(L"0")) && (upper == Sequence(L"1")))
      return Sequence(L"0..1");
  }
  return Sequence(L"1");
}

bool XmiAux::IsOrdered(const TYPE_XMI_UMLAssociationEnd & ae,
                       const Map & idm)
{
  // for EA
  Map attrs (ae.get_attrs());
  if (attrs.DomExists(TAG_XMI_isOrdered))
    return (attrs[TAG_XMI_isOrdered] == TAG_XMI_TRUE);

  // for ASTAH
  Sequence id (GetID(ae));
  if (idm.DomExists(id))
  {
    TYPE_XMI_UMLAssociationEnd nae (idm[id]);
    Map attrs (nae.get_attrs());
    if (attrs.DomExists(TAG_XMI_ordering))
      return (attrs[TAG_XMI_ordering] == TAG_XMI_ordered);
  }
  return false;
}

Generic XmiAux::GetQualiferSeq(const TYPE_XMI_UMLAssociationEnd & se,
                               const TYPE_XMI_UMLAssociationEnd & ce,
                               const Map & idm,
                               int mode)
{
  switch(mode) {
    case MODE_EnterpriseArchitect: {
      // for EA
      Sequence qualifier (SearchTagValue(TAG_XMI_qualifier, ce.get_modelElement()));
      return qualifier;
    }
    case MODE_ASTAH: {
      // for ASTAH
      Sequence id (GetID(se));
      if (idm.DomExists(id))
      {
        TYPE_XMI_UMLAssociationEnd nae (idm[id]);
        if (!nae.get_qualifier().IsNil())
        {
          TYPE_XMI_UMLAttribute at (nae.get_qualifier());
          Sequence type (GetElementName(GetTypeRef(at), idm));
          return type;
        }
      }
      return Sequence();
    }
    default: {
      return Sequence();
    }
  }
}

Generic XmiAux::GetQualifer(const TYPE_XMI_UMLAssociationEnd & se,
                            const TYPE_XMI_UMLAssociationEnd & ce,
                            const Map & idm,
                            int mode)
{
/*
  switch(mode) {
    case MODE_EnterpriseArchitect: {
      // for EA
      Sequence qualifier (SearchTagValue(TAG_XMI_qualifier, ce.get_modelElement()));
      if (!qualifier.IsEmpty())
      {
        Tuple t (UMLPARSE::Seq2Type(qualifier));
        if (t.GetBool(1))
          return t.GetRecord(2);
        else
          return Nil();
      }
      return Nil();
    }
    case MODE_ASTAH: {
      // for ASTAH
      Sequence id (GetID(se));
      if (idm.DomExists(id))
      {
        TYPE_XMI_UMLAssociationEnd nae (idm[id]);
        if (!nae.get_qualifier().IsNil())
        {
          TYPE_XMI_UMLAttribute at (nae.get_qualifier());
          Sequence type (GetElementName(GetTypeRef(at), idm));
          if (!type.IsEmpty())
          {
            Tuple t (UMLPARSE::Seq2Type(type));
            if (t.GetBool(1))
            return t.GetRecord(2);
        }
      }
      return Nil();
    }
    default: {
      return Nil();
    }
  }
*/
  Sequence qualifer (GetQualiferSeq(se, ce, idm, mode));
  if (!qualifer.IsEmpty())
  {
    Tuple t (UMLPARSE::Seq2Type(qualifer));
    if (t.GetBool(1))
      return t.GetRecord(2);
  }
  return Nil();
}

Generic XmiAux::GetQualifer(const TYPE_XMI_UMLAssociation & a,
                            const Map & idm)
{
  // for EA
  TYPE_XMI_UMLAssociationEnd dest (GetRoleDest(a, idm));
  Sequence qualifier (SearchTagValue(TAG_XMI_qualifier, dest.get_modelElement()));
  if (!qualifier.IsEmpty())
  {
    Tuple t (UMLPARSE::Seq2Type(qualifier));
    if (t.GetBool(1))
      return t.GetRecord(2);
    else
      return Nil();
  }
  // for ASTAH
  TYPE_XMI_UMLAssociationEnd source (GetRoleSource(a, idm));
  if (!source.get_qualifier().IsNil())
  {
    TYPE_XMI_UMLAttribute at (source.get_qualifier());
    Sequence type (GetElementName(GetTypeRef(at), idm));
    if (!type.IsEmpty())
    {
      Tuple t (UMLPARSE::Seq2Type(type));
      if (t.GetBool(1))
        return t.GetRecord(2);
    }
  }
  return Nil();
}

Generic XmiAux::GetInitValue(const TYPE_XMI_UMLAssociationEnd & ae, const Map & idm)
{
  // for EA
  // TODO
  // for ASTAH
  Sequence id (GetID(ae));
  if (idm.DomExists(id))
  {
    TYPE_XMI_UMLAssociationEnd nae (idm[id]);
    if (!nae.get_initialValue().IsNil())
    {
      TYPE_XMI_UMLExpression e (nae.get_initialValue());
      Generic body (e.get_body());
      if (body.IsSequence())
      {
        Tuple t (UMLPARSE::Seq2Expr(body));
        if (t.GetBool(1))
          return t.GetRecord(2);
      }
    }
  }
  return Nil();
}

Generic XmiAux::GetInitType(const TYPE_XMI_UMLAssociationEnd & ae,
                            const Sequence & server,
                            const Quote & cd,
                            const Sequence & qualifer,
                            const Quote & role_t,
                            const Map & idm)
{
  // for EA
  // TODO
  // for ASTAH
  Sequence id (GetID(ae));
  if (idm.DomExists(id))
  {
    TYPE_XMI_UMLAssociationEnd nae (idm[id]);
    if (!nae.get_initialValue().IsNil())
    {
      Sequence tpstr;
      if ((cd == QUOTE_ONE) || (cd == QUOTE_ZERO_ONE))
        tpstr.ImpConc(server);
      else if ((cd == QUOTE_MANY) || (cd == QUOTE_ONE_TO_MANY))
      {
        if (role_t == QUOTE_ORDERED) 
          tpstr.ImpConc(Sequence(L"seq of ")).ImpConc(server);
        else
          tpstr.ImpConc(Sequence(L"set of ")).ImpConc(server);
      }

/*
      if (!nae.get_qualifier().IsNil())
      {
        TYPE_XMI_UMLAttribute a (nae.get_qualifier());
        Sequence type (GetElementName(GetTypeRef(a), idm));
        if (!type.IsEmpty())
        {
          tpstr = Sequence(L"map ").ImpConc(type).ImpConc(L" to ").ImpConc(tpstr);
        }
      }
*/
      if (!qualifer.IsEmpty())
      {
        tpstr = Sequence(L"map ").ImpConc(qualifer).ImpConc(L" to ").ImpConc(tpstr);
      }

      Tuple t (UMLPARSE::Seq2Type(tpstr));
      if (t.GetBool(1))
        return t.GetRecord(2);
    }
  }
  return Nil();
}

Sequence XmiAux::GetChildId(const TYPE_XMI_UMLGeneralization & ge)
{
  // for EA
  Map attrs (ge.get_attrs());
  if (attrs.DomExists(TAG_XMI_subtype))
    return attrs[TAG_XMI_subtype];
  
  // ASTAH
  Generic child (ge.get_child());
  if (!child.IsNil())
  {
    return GetID(child);
  }
  return Sequence(L"");
}

Sequence XmiAux::GetParentId(const TYPE_XMI_UMLGeneralization & ge)
{
  // EA
  Map attrs (ge.get_attrs());
  if (attrs.DomExists(TAG_XMI_supertype))
    return attrs[TAG_XMI_supertype];
  
  // ASTAH
  Generic parent (ge.get_parent());
  if (!parent.IsNil())
  {
    return GetID(parent);
  }
  return Sequence(L"");
}

TYPE_XMI_UMLAssociationEnd XmiAux::GetRoleSource(const TYPE_XMI_UMLAssociation & a,
                                                 const Map & idm)
{
  Sequence ae_l (a.get_connection());
  Generic g;
  for (bool bb = ae_l.First(g); bb; bb = ae_l.Next(g))
  {
    // for EA
    TYPE_XMI_UMLAssociationEnd ae (g);
    if (HasTagValue(TAG_XMI_sourcestyle, ae.get_modelElement()))
      return g;
    else if (HasTagValue(TAG_XMI_deststyle, ae.get_modelElement()))
      continue;
    else
    {
      // for ASTAH
      Sequence id (GetID(g));
      if (idm.DomExists(id))
      {
        Generic ng (idm[id]);
        Sequence nm (GetName(ng));
        if (!nm.IsEmpty()) 
          return ng;
      } 
    }
  }
  return mk_UMLAssociationEnd();
}

TYPE_XMI_UMLAssociationEnd XmiAux::GetRoleDest(const TYPE_XMI_UMLAssociation & a,
                                               const Map & idm)
{
  Sequence ae_l (a.get_connection());
  Generic g;
  for (bool bb = ae_l.First(g); bb; bb = ae_l.Next(g))
  {
    // for EA
    TYPE_XMI_UMLAssociationEnd ae (g);
    if (HasTagValue(TAG_XMI_deststyle, ae.get_modelElement()))
      return g;
    else if (HasTagValue(TAG_XMI_sourcestyle, ae.get_modelElement()))
      continue;
    else
    {
      // for ASTAH
      Sequence id (GetID(g));
      if (idm.DomExists(id))
      {
        Generic ng (idm[id]);
        Sequence nm (GetName(ng));
        if (nm.IsEmpty()) 
          return ng;
      } 
    }
  }
  return mk_UMLAssociationEnd();
}

/*
Sequence XmiAux::GetClientClassName(const TYPE_XMI_UMLAssociation & a, const Map & idm)
{
  Sequence ae_l (a.get_connection());
  Generic g;
  for (bool bb = ae_l.First(g); bb; bb = ae_l.Next(g))
  {
    // for EA
    TYPE_XMI_UMLAssociationEnd ae (g);
    if (HasTagValue(TAG_XMI_deststyle, ae.get_modelElement()))
      return GetClassName(g, idm);
    else if (HasTagValue(TAG_XMI_sourcestyle, ae.get_modelElement()))
      continue;
    else
    {
      // for ASTAH
      Sequence id (GetID(g));
      if (idm.DomExists(id))
      {
        Generic ng (idm[id]);
        Sequence nm (GetName(ng));
        if (!nm.IsEmpty()) 
          return GetClassName(ng, idm);
      }
    }
  }
  return Sequence(L"");
}

Sequence XmiAux::GetServerClassName(const TYPE_XMI_UMLAssociation & a, const Map & idm)
{
  Sequence ae_l (a.get_connection());
  Generic g;
  for (bool bb = ae_l.First(g); bb; bb = ae_l.Next(g))
  {
    // for EA
    TYPE_XMI_UMLAssociationEnd ae (g);
    if (HasTagValue(TAG_XMI_sourcestyle, ae.get_modelElement()))
      return GetClassName(g, idm);
    else if (HasTagValue(TAG_XMI_deststyle, ae.get_modelElement()))
      continue;
    else
    {
      // for ASTAH
      Sequence id (GetID(g));
      if (idm.DomExists(id))
      {
        Generic ng (idm[id]);
        Sequence nm (GetName(ng));
        if (nm.IsEmpty()) 
          return GetClassName(ng, idm);
      }
    }
  }
  return Sequence(L"");
}
*/

Sequence XmiAux::GetClassNameFromAssoc(const TYPE_XMI_UMLAssociationEnd & ae, const Map & idm)
{
  // EA
  Map attrs (ae.get_attrs());
  if (attrs.DomExists(TAG_XMI_type))
    return GetElementName(attrs[TAG_XMI_type], idm);

  // ASTAH
  Sequence id (GetID(ae));
  if (idm.DomExists(id))
  {
    TYPE_XMI_UMLAssociationEnd nae (idm[id]);
    return GetElementName(nae.get_feature().get_owner(), idm);
  }
  return Sequence();
}

Sequence XmiAux::PrintAttribute(const TYPE_XMI_UMLAttribute & a,
                                const Map & idm)
{
  Sequence res (GetName(a));

  Sequence type (GetElementName(GetTypeRef(a), idm));
  if (!type.IsEmpty())
    res.ImpConc(Sequence(L" : "))
       .ImpConc(type);

  Generic initv (a.get_initialValue());
  if (!initv.IsNil())
  {
    TYPE_XMI_UMLExpression exp (initv);
    Generic body (exp.get_body());
    if (body.IsSequence())
    {
      res.ImpConc(Sequence(L" = "))
         .ImpConc(body);
    }
  }
  return res;
}

Sequence XmiAux::PrintOperation(const TYPE_XMI_UMLOperation & o,
                                const Map & idm)
{
  Sequence res (GetName(o));
  res.ImpAppend(Char(L'('));
  
  SEQ<TYPE_XMI_UMLParameter> p_l (GetParam(o));
  bool first = true;
  Generic g;
  for (bool bb = p_l.First(g); bb; bb = p_l.Next(g))
  { 
    TYPE_XMI_UMLParameter p (g);
    if (first)
      first = false;
    else
      res.ImpConc(Sequence(L", "));
    
    res.ImpConc(GetName(p))
       .ImpConc(Sequence(L" : "))
       .ImpConc(GetElementName(GetTypeRef(p), idm));
  } 
  res.ImpAppend(Char(L')'));
  
  TYPE_XMI_UMLParameter rp (GetReturnType(o));
  if (!IsVoid(rp, idm))
  { 
    Sequence rt (GetElementName(GetTypeRef(rp), idm));
    res.ImpConc(Sequence(L" : "))
       .ImpConc(rt);
  }
  return res;
}

TYPE_XMI_UMLModel XmiAux::mk_UMLModel(const Sequence & name,
                                      const Sequence & id)
{
  Map attrs;
  attrs.ImpModify(TAG_XMI_id,                id);
  attrs.ImpModify(TAG_XMI_name,              name);
  attrs.ImpModify(TAG_XMI_version,           Sequence(L"0"));
  attrs.ImpModify(TAG_XMI_unSolvedFlag,      TAG_XMI_FALSE);
  attrs.ImpModify(TAG_XMI_isLeaf,            TAG_XMI_FALSE);
  attrs.ImpModify(TAG_XMI_isRoot,            TAG_XMI_TRUE);
  attrs.ImpModify(TAG_XMI_isLeaf,            TAG_XMI_FALSE);
  attrs.ImpModify(TAG_XMI_isAbstract,        TAG_XMI_FALSE);
  attrs.ImpModify(Sequence(L"xmlns:UML"),    Sequence(L"org.omg.xmi.namespace.UML"));

  return TYPE_XMI_UMLModel().Init(attrs,
                                  mk_UMLModelElement(),
                                  mk_UMLNamespace(Map()));
}

TYPE_XMI_UMLClass XmiAux::mk_UMLClass(const Sequence & name,
                                      const Sequence & id,
                                      const Sequence & pid,
                                      const Bool & abstract)
{
  Map attrs;
  attrs.ImpModify(TAG_XMI_id,           id);
  attrs.ImpModify(TAG_XMI_name,         name);
  attrs.ImpModify(TAG_XMI_version,      Sequence(L"0"));
  attrs.ImpModify(TAG_XMI_unSolvedFlag, TAG_XMI_FALSE);
  attrs.ImpModify(TAG_XMI_isRoot,       TAG_XMI_FALSE);
  attrs.ImpModify(TAG_XMI_isLeaf,       TAG_XMI_FALSE);

  attrs.ImpModify(TAG_XMI_isAbstract,   (abstract ? TAG_XMI_TRUE : TAG_XMI_FALSE));

  attrs.ImpModify(TAG_XMI_isActive,     TAG_XMI_FALSE);

  Map ns_attrs;
  ns_attrs.ImpModify(TAG_XMI_idref, pid);

  return TYPE_XMI_UMLClass().Init(attrs,
                                  mk_UMLModelElement(Map(), ns_attrs, TAG_XMI_public),
                                  mk_UMLNamespace(),
                                  TYPE_XMI_UMLGeneralizableElement().Init(Map(), Sequence()),
                                  Nil());
}

TYPE_XMI_UMLStereotype XmiAux::mk_UMLStereotype(const Sequence & name, 
                                                const Sequence & id,
                                                const Sequence & fnopid)
{
  Map attrs;
  attrs.ImpModify(TAG_XMI_id,           id);
  attrs.ImpModify(TAG_XMI_name,         name);
  attrs.ImpModify(TAG_XMI_version,      Sequence(L"0"));
  attrs.ImpModify(TAG_XMI_unSolvedFlag, TAG_XMI_FALSE);
  attrs.ImpModify(TAG_XMI_isRoot,       TAG_XMI_FALSE);
  attrs.ImpModify(TAG_XMI_isLeaf,       TAG_XMI_FALSE);
  attrs.ImpModify(TAG_XMI_isAbstract,   TAG_XMI_FALSE);
 
  Map me_attrs;
  me_attrs.ImpModify(TAG_XMI_idref, fnopid);

  Sequence me_l;
  me_l.ImpAppend(mk_UMLModelElement(me_attrs, Map(), TAG_XMI_public));

  return TYPE_XMI_UMLStereotype().Init(attrs, Sequence(), me_l);
}

TYPE_XMI_UMLNamespace XmiAux::mk_UMLNamespace()
{
  return TYPE_XMI_UMLNamespace().Init(Map(), Sequence());
}

TYPE_XMI_UMLNamespace XmiAux::mk_UMLNamespace(const Map & attrs)
{
  return TYPE_XMI_UMLNamespace().Init(attrs, Sequence());
}

TYPE_XMI_UMLFeature XmiAux::mk_UMLFeature()
{
  return TYPE_XMI_UMLFeature().Init(Map(), Nil(), Nil());
}

TYPE_XMI_UMLFeature XmiAux::mk_UMLFeature(const Generic & visibility)
{
  return TYPE_XMI_UMLFeature().Init(Map(), Nil(), visibility);
}

TYPE_XMI_UMLFeature XmiAux::mk_UMLFeature(const Map & attrs, const Generic & visibility)
{
  return TYPE_XMI_UMLFeature().Init(attrs, Nil(), visibility);
}

TYPE_XMI_UMLStructuralFeature XmiAux::mk_UMLStructuralFeature()
{
  return TYPE_XMI_UMLStructuralFeature().Init(Map(), Nil(), Nil());
}

TYPE_XMI_UMLStructuralFeature XmiAux::mk_UMLStructuralFeature(const Map & attrs)
{
  return TYPE_XMI_UMLStructuralFeature().Init(attrs, Nil(), Nil());
}

TYPE_XMI_UMLAttribute XmiAux::mk_UMLAttribute(const Map & attrs)
{
  return TYPE_XMI_UMLAttribute().Init(attrs,
                                      mk_UMLModelElement(),
                                      mk_UMLFeature(),
                                      mk_UMLStructuralFeature(),
                                      Nil(), Nil());
}

TYPE_XMI_UMLModelElement XmiAux::mk_UMLModelElement()
{
  return mk_UMLModelElement(Nil());
}

TYPE_XMI_UMLModelElement XmiAux::mk_UMLModelElement(const Generic & visibility)
{
  return TYPE_XMI_UMLModelElement().Init(Map(),
                                         Sequence(),     // namespace
                                         visibility,     // visibility
                                         Nil(),          // definition
                                         Sequence(),     // stereotype
                                         Sequence(),     // taggedValue
                                         Sequence());    // constraint
}

TYPE_XMI_UMLModelElement XmiAux::mk_UMLModelElement(const Map & attrs, 
                                                    const Map & ns_attrs,
                                                    const Generic & visibility)
{
  Sequence ns;
  if (!ns_attrs.IsEmpty())
    ns.ImpAppend(mk_UMLNamespace(ns_attrs));
  
  return TYPE_XMI_UMLModelElement().Init(attrs,
                                         ns,             // namespace
                                         //TAG_XMI_public, // visibility
                                         visibility,     // visibility
                                         Nil(),          // definition
                                         Sequence(),     // stereotype
                                         Sequence(),     // taggedValue
                                         Sequence());    // constraint
}

TYPE_XMI_UMLTaggedValue XmiAux::mk_UMLTaggedValue(const Sequence & tag,
                                                  const Sequence & value)
{
  return mk_UMLTaggedValue(tag, value, Sequence());
}

TYPE_XMI_UMLTaggedValue XmiAux::mk_UMLTaggedValue(const Sequence & tag,
                                                  const Sequence & value,
                                                  const Sequence & pid)
{
  Map attrs;
  attrs.ImpModify(TAG_XMI_id,       UniqueID());
  attrs.ImpModify(TAG_XMI_version,  Sequence(L"0"));
  attrs.ImpModify(TAG_XMI_tag,      tag);
  attrs.ImpModify(TAG_XMI_value,    value);

  Map me_attrs;
  if (!pid.IsEmpty())
    me_attrs.ImpModify(TAG_XMI_idref, pid);

  return TYPE_XMI_UMLTaggedValue().Init(attrs,
                                        mk_UMLModelElement(me_attrs, Map(), Nil()));
}

TYPE_XMI_Document XmiAux::mk_Document(const TYPE_XMI_UMLModel & m,
                                      const Sequence & pr_l,
                                      int mode)
{
  Map attrs;
  attrs.ImpModify(Sequence(L"xmi.version"), Sequence(L"1.1"));
  attrs.ImpModify(Sequence(L"xmlns:UML"), Sequence(L"org.omg.xmi.namespace.UML"));

  return TYPE_XMI_Document().Init(attrs,
                                  mk_Header(mode),
                                  TYPE_XMI_Content().Init(Map(), m, pr_l, Sequence(), Nil()),
                                  Nil(),
                                  Nil());
}

TYPE_XMI_Header XmiAux::mk_Header(int mode)
{
  return TYPE_XMI_Header().Init(Map(),
                                mk_Documentation(mode),
                                Nil(),
                                Nil(),
                                mk_Metamodel(),
                                Nil());
}

TYPE_XMI_Documentation XmiAux::mk_Documentation(int mode)
{
  Sequence exporter;
  Sequence exporterversion;
  switch (mode) {
    case MODE_ASTAH: {
      exporter = Sequence(L"Jomt XMI writer");
      exporterversion = Sequence(L"1.2.0.28");
      break;
    }
    case MODE_EnterpriseArchitect: {
      exporter = Sequence(L"VDMTools XMI writer for EA");
      exporterversion = Sequence(L"1.0.0.0");
      break;
    }
    default: {
      exporter = Sequence(L"VDMTools XMI writer");
      exporterversion = Sequence(L"1.0.0.0");
      break;
    }
  }
  return TYPE_XMI_Documentation().Init(Map(),
                                       TYPE_XMI_Exporter().Init(Map(), exporter),
                                       TYPE_XMI_ExporterVersion().Init(Map(), exporterversion),
                                       Nil(),
                                       Nil(),
                                       Nil(),
                                       Nil());
}

TYPE_XMI_Metamodel XmiAux::mk_Metamodel()
{
  Map attrs;
  attrs.ImpModify(Sequence(L"xmi.name"), Sequence(L"UML"));
  attrs.ImpModify(Sequence(L"xmi.version"), Sequence(L"1.4"));

  return TYPE_XMI_Metamodel().Init(attrs);
}

Map XmiAux::createDefaultAttribute()
{
  Map res;
  res.ImpModify(Sequence(L"xmi.version"), Sequence(L"1.1"));
  res.ImpModify(Sequence(L"xmlns:UML"), Sequence(L"org.omg.xmi.namespace.UML"));
  return res;
}

TYPE_XMI_Header XmiAux::createDefaultHeader()
{
  TYPE_XMI_Header res;
  res.Init(Map(),
            createDefaultDocumentation(),
            Nil(),
            Nil(),
            createDefaultMetamodel(),
            Nil());
  return res;
}

TYPE_XMI_Documentation XmiAux::createDefaultDocumentation()
{
  TYPE_XMI_Documentation res;
  res.Init(Map(),
//           TYPE_XMI_Exporter().Init(Map(), Sequence(L"VDMTools XMI writer")),
//           TYPE_XMI_ExporterVersion().Init(Map(), Sequence(L"1.0.0")),
           TYPE_XMI_Exporter().Init(Map(), Sequence(L"Jomt XMI writer")),
           TYPE_XMI_ExporterVersion().Init(Map(), Sequence(L"1.2.0.28")),
           Nil(),
           Nil(),
           Nil(),
           Nil());
  return res;
}

TYPE_XMI_Metamodel XmiAux::createDefaultMetamodel()
{
  Map attrs;
  attrs.ImpModify(Sequence(L"xmi.name"), Sequence(L"UML"));
  attrs.ImpModify(Sequence(L"xmi.version"), Sequence(L"1.4"));
  TYPE_XMI_Metamodel res;
  res.Init(attrs);
  return res;
}

bool XmiAux::IsEAModel(const TYPE_XMI_UMLModel & m)
{
  Sequence elm_l (m.get_namespace().get_ownedElement());
  if (!elm_l.IsEmpty())
  {
    Record r (elm_l[1]);
    if (r.Is(TAG_TYPE_XMI_UMLClass))
    {
      Map attr (r.GetMap(pos_XMI_UMLClass_attrs));
      if (attr.DomExists(TAG_XMI_name))
        return (attr[TAG_XMI_name] == Sequence(L"EARootClass"));
    }
  }
  return false;
}

int XmiAux::GetModeOfAssociation(const TYPE_XMI_UMLAssociation & m)
{
  if (HasTagValue(TAG_XMI_ea_type, m.get_modelElement()))
    return MODE_EnterpriseArchitect;
  else 
    return MODE_ASTAH;
}

TYPE_XMI_UMLConstraint XmiAux::mk_UMLConstraint(const Sequence & name,
                                                const Sequence & prepost,
                                                const Sequence & pid)
{
  Map attrs;
  attrs.ImpModify(TAG_XMI_name, name);

  Sequence tv_l;
  tv_l.ImpAppend(mk_UMLTaggedValue(TAG_XMI_constraint_type, prepost, pid));
  tv_l.ImpAppend(mk_UMLTaggedValue(TAG_XMI_type, TAG_XMI_vdm, pid));

  return TYPE_XMI_UMLConstraint().Init(attrs,
                                       TYPE_XMI_UMLModelElement().Init(Map(),
                                                                       Sequence(),
                                                                       Nil(),
                                                                       Nil(),
                                                                       Sequence(),
                                                                       tv_l,
                                                                       Sequence()),
                                        Nil(),
                                        Nil());
}

TYPE_XMI_UMLGeneralization XmiAux::mk_UMLGeneralization(const Sequence & pid)
{
  Map attrs;
  attrs.ImpModify(TAG_XMI_id,           UniqueID());
  attrs.ImpModify(TAG_XMI_name,         Sequence(L""));
  attrs.ImpModify(TAG_XMI_version,      Sequence(L"0"));
  attrs.ImpModify(TAG_XMI_unSolvedFlag, TAG_XMI_FALSE);

  Map ns_attrs;
  ns_attrs.ImpModify(TAG_XMI_idref, pid);

  return TYPE_XMI_UMLGeneralization().Init(attrs,
                                           mk_UMLModelElement(Map(), ns_attrs, TAG_XMI_public),
                                           Nil(),  // Generalization.child
                                           Nil(),  // Generalization.parent
                                           Nil());
}

TYPE_XMI_UMLGeneralization XmiAux::mk_UMLGeneralizationRef(const Sequence & id)
{
  Map attrs;
  attrs.ImpModify(TAG_XMI_idref, id);

  return TYPE_XMI_UMLGeneralization().Init(attrs,
                                           mk_UMLModelElement(),
                                           Nil(),  // Generalization.child
                                           Nil(),  // Generalization.parent
                                           Nil());
}

TYPE_XMI_UMLAssociationEnd XmiAux::mk_UMLAssociationEnd()
{
  return mk_UMLAssociationEnd(Map());
}

TYPE_XMI_UMLAssociationEnd XmiAux::mk_UMLAssociationEnd(const Map & attrs)
{
  return TYPE_XMI_UMLAssociationEnd().Init(attrs, 
                                           mk_UMLModelElement(),
                                           mk_UMLFeature(),
                                           mk_UMLStructuralFeature(),
                                           Nil(), Nil(), Nil(), Nil(), Nil());
}

TYPE_XMI_UMLAssociationEnd XmiAux::mk_UMLAssociationEnd(const Sequence & name, const Sequence & pid)
{
  Map attrs; 
  attrs.ImpModify(TAG_XMI_id,            UniqueID());
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

  Sequence visibility (TAG_XMI_private);

  // for EA
  attrs.ImpModify(TAG_XMI_visibility, visibility);

  Map ns_attrs;
  ns_attrs.ImpModify(TAG_XMI_idref, pid);

  return TYPE_XMI_UMLAssociationEnd().Init(attrs, 
                                           mk_UMLModelElement(Map(), ns_attrs, visibility),
                                           mk_UMLFeature(visibility),
                                           mk_UMLStructuralFeature(),
                                           Nil(), Nil(), Nil(), visibility, Nil());
}

TYPE_XMI_UMLAssociationEnd XmiAux::mk_UMLAssociationEndRef(const Sequence & id)
{
  Map attrs;
  attrs.ImpModify(TAG_XMI_idref, id);

  return mk_UMLAssociationEnd(attrs);
}

TYPE_XMI_UMLMultiplicity XmiAux::mk_UMLMultiplicity(const TYPE_XMI_UMLMultiplicityRange & rg)
{
  Map attrs;
  attrs.ImpModify(TAG_XMI_id, UniqueID());

  return TYPE_XMI_UMLMultiplicity().Init(attrs, rg);
}

TYPE_XMI_UMLMultiplicityRange XmiAux::mk_UMLMultiplicityRange(const Sequence & lower,
                                                              const Sequence & upper)
{
  Map attrs;
  attrs.ImpModify(TAG_XMI_id,         UniqueID());
  attrs.ImpModify(TAG_XMI_lowerValue, lower);
  attrs.ImpModify(TAG_XMI_upperValue, upper);

  return TYPE_XMI_UMLMultiplicityRange().Init(attrs);
}

TYPE_XMI_UMLExpression XmiAux::mk_UMLExpression(const Sequence & val)
{
  Map attrs;
  attrs.ImpModify(TAG_XMI_id, UniqueID());

  return TYPE_XMI_UMLExpression().Init(attrs, val);
}

TYPE_XMI_UMLPrimitive XmiAux::mk_UMLPrimitive(const Sequence & tp)
{
  Map attrs;
  attrs.ImpModify(TAG_XMI_id,           UniqueID());
  attrs.ImpModify(TAG_XMI_name,         tp);
  attrs.ImpModify(TAG_XMI_version,      Sequence(L"0"));
  attrs.ImpModify(TAG_XMI_unSolvedFlag, TAG_XMI_FALSE);
  attrs.ImpModify(TAG_XMI_isRoot,       TAG_XMI_FALSE);
  attrs.ImpModify(TAG_XMI_isLeaf,       TAG_XMI_FALSE);
  attrs.ImpModify(TAG_XMI_isAbstract,   TAG_XMI_FALSE);
  attrs.ImpModify(TAG_XMI_isActive,     TAG_XMI_FALSE);

  return TYPE_XMI_UMLPrimitive().Init(attrs, Sequence()); // dummy
}

