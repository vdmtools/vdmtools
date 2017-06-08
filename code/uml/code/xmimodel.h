#ifndef __xmimodel_h__
#define __xmimodel_h__

#include "metaiv.h"
#include "XMI.h"

class IXmiParameter : public TYPE_XMI_UMLParameter
{
public:
  IXmiParameter(const Sequence & name, const Sequence & id,
                const Sequence & opid, const Sequence & clid);
  IXmiParameter(const TYPE_XMI_UMLParameter & p) : TYPE_XMI_UMLParameter(p) {};

  Bool IsReturn() const;
  void SetReturn();
  Generic GetTypeRef() const;
  void SetTypeRef(const TYPE_XMI_UMLClassifier & t);
};

class IXmiMultiplicityRange : public TYPE_XMI_UMLMultiplicityRange
{
public:
  IXmiMultiplicityRange(const Sequence & id);
  IXmiMultiplicityRange(const TYPE_XMI_UMLMultiplicityRange & r) : TYPE_XMI_UMLMultiplicityRange(r) {};

  Sequence GetLowerValue();
  void SetLowerValue(const Sequence & v);
  Sequence GetUpperValue();
  void SetUpperValue(const Sequence & v);
};

class IXmiClass : public TYPE_XMI_UMLClass
{
public:
  IXmiClass(const Map & attrs, const Map & ns_attrs);
  IXmiClass(const Sequence & name, const Sequence & id, const Sequence & pid);
  IXmiClass(const TYPE_XMI_UMLClass & c) : TYPE_XMI_UMLClass(c) {};

  Set GetSuperclasses(); 
  void SetAbstract(const Bool & b);
  Sequence GetFeatures() const;
  Sequence GetAttributes() const;
  Sequence GetOperations() const;
  void AppendGeneralization(const TYPE_XMI_UMLGeneralization & g);
  void AddTaggedValue(const TYPE_XMI_UMLTaggedValue & tv);
};

class IXmiAttribute : public TYPE_XMI_UMLAttribute
{
public:
  IXmiAttribute(const Sequence & name, const Sequence & id, const Sequence & pid);
  IXmiAttribute(const TYPE_XMI_UMLAttribute & o) : TYPE_XMI_UMLAttribute(o) {};

  void SetStatic(const Bool & b);
  void SetChangeable(const Bool & b);
  void SetVisibility(const Sequence & v);
  Generic GetTypeRef() const;
  void SetTypeRef(const TYPE_XMI_UMLClassifier & t);
};

class IXmiOperation : public TYPE_XMI_UMLOperation
{
public:
  IXmiOperation(const Sequence & name, const Sequence & id, const Sequence & pid);
  IXmiOperation(const TYPE_XMI_UMLOperation & o) : TYPE_XMI_UMLOperation(o) {};

  Sequence GetParameters() const;
  IXmiParameter GetReturnType() const;
  Sequence GetParam() const;
  Sequence GetStereotypes() const;
  void AppendStereotype(const TYPE_XMI_UMLStereotype & st);
  void SetStatic(const Bool & b);
  Bool IsAbstract();
  void SetAbstract(const Bool& b);
  Sequence GetVisibility();
  void SetVisibility(const Sequence & v);
  void AddTaggedValue(const TYPE_XMI_UMLTaggedValue & tv);
  void AddConstraint(const TYPE_XMI_UMLConstraint & co);
};

class IXmiAssociation;
class IXmiAssociationEnd : public TYPE_XMI_UMLAssociationEnd
{
public:
  IXmiAssociationEnd(const Map & attrs, const Map & ns_attrs);
  IXmiAssociationEnd(const Sequence & name, const Sequence & id, const Sequence & pid);
  IXmiAssociationEnd(const TYPE_XMI_UMLAssociationEnd & p) : TYPE_XMI_UMLAssociationEnd(p) {};

  void SetStatic(const Bool & b);
  void SetVisibility(const Sequence & v);
  
  void SetOwner(const TYPE_XMI_UMLClassifier & c);
  void SetMultiplicity(const TYPE_XMI_UMLMultiplicity & m);
  void SetMultiplicityEA(const Sequence & m);

  Bool IsNavigable();
  void SetNavigable(const Bool & b);
  void SetOrdered(const Bool & b);
};

class IXmiAssociation : public TYPE_XMI_UMLAssociation
{
public:
  IXmiAssociation(const Map & attrs, const Map & ns_attrs);
  IXmiAssociation(const Sequence & name, const Sequence & id, const Sequence & pid);
  IXmiAssociation(const TYPE_XMI_UMLAssociation & p) : TYPE_XMI_UMLAssociation(p) {};
};

#endif // __xmimodel_h__
