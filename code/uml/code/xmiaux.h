#ifndef __xmiaux_h__
#define __xmiaux_h__

#include "metaiv.h"
#include "AUML.h"
#include "XMI.h"


#define TAG_XMI_name           Sequence(L"name")
#define TAG_XMI_id             Sequence(L"xmi.id")
#define TAG_XMI_idref          Sequence(L"xmi.idref")
#define TAG_XMI_version        Sequence(L"version")
#define TAG_XMI_unSolvedFlag   Sequence(L"unSolvedFlag")
#define TAG_XMI_isRoot         Sequence(L"isRoot")
#define TAG_XMI_isLeaf         Sequence(L"isLeaf")
#define TAG_XMI_isAbstract     Sequence(L"isAbstract")
#define TAG_XMI_isActive       Sequence(L"isActive")
#define TAG_XMI_ownerScope     Sequence(L"ownerScope") 
#define TAG_XMI_targetScope    Sequence(L"targetScope") 
#define TAG_XMI_isQuery        Sequence(L"isQuery") 
#define TAG_XMI_concurrency    Sequence(L"concurrency") 
#define TAG_XMI_kind           Sequence(L"kind") 
#define TAG_XMI_changeability  Sequence(L"changeability") 
#define TAG_XMI_aggregation    Sequence(L"aggregation")
#define TAG_XMI_ordering       Sequence(L"ordering")
#define TAG_XMI_isOrdered      Sequence(L"isOrdered")
#define TAG_XMI_navigableType  Sequence(L"navigableType")
#define TAG_XMI_isNavigable    Sequence(L"isNavigable")
#define TAG_XMI_lowerValue     Sequence(L"lowerValue")
#define TAG_XMI_lower          Sequence(L"lower")
#define TAG_XMI_upperValue     Sequence(L"upperValue")
#define TAG_XMI_upper          Sequence(L"upper")
#define TAG_XMI_tag            Sequence(L"tag")
#define TAG_XMI_value          Sequence(L"value")
#define TAG_XMI_visibility     Sequence(L"visibility") 
#define TAG_XMI_body           Sequence(L"body") 
#define TAG_XMI_multiplicity   Sequence(L"multiplicity") 
#define TAG_XMI_subtype        Sequence(L"subtype") 
#define TAG_XMI_supertype      Sequence(L"supertype") 
#define TAG_XMI_qualifier      Sequence(L"qualifier") 
#define TAG_XMI_ea_end         Sequence(L"ea_end") 
#define TAG_XMI_ea_type        Sequence(L"ea_type") 
#define TAG_XMI_containment    Sequence(L"containment") 

#define TAG_XMI_type           Sequence(L"type")
#define TAG_XMI_sourcestyle    Sequence(L"sourcestyle")
#define TAG_XMI_deststyle      Sequence(L"deststyle")
#define TAG_XMI_constraint_type Sequence(L"constraint_type")
#define TAG_XMI_direction      Sequence(L"direction")

//#define TAG_XMI_funcJP         Sequence(L"\u6a3f\u80fd")
#define TAG_XMI_vdm            Sequence(L"vdm")

#define TAG_XMI_public         Sequence(L"public")
#define TAG_XMI_private        Sequence(L"private")
#define TAG_XMI_protected      Sequence(L"protected")
#define TAG_XMI_implementation Sequence(L"implementation")

#define TAG_XMI_TRUE           Sequence(L"true")
#define TAG_XMI_FALSE          Sequence(L"false")

#define TAG_XMI_instance       Sequence(L"instance")
#define TAG_XMI_classifier     Sequence(L"classifier")

#define TAG_XMI_sequential     Sequence(L"sequential")

#define TAG_XMI_changeable     Sequence(L"changeable")
#define TAG_XMI_frozen         Sequence(L"frozen")

#define TAG_XMI_return         Sequence(L"return")
#define TAG_XMI_in             Sequence(L"in")
#define TAG_XMI_out            Sequence(L"out")
#define TAG_XMI_inout          Sequence(L"inout")

#define TAG_XMI_none           Sequence(L"none")
#define TAG_XMI_unordered      Sequence(L"unordered")
#define TAG_XMI_ordered        Sequence(L"ordered")
#define TAG_XMI_navigable      Sequence(L"navigable")
#define TAG_XMI_non_navigable  Sequence(L"non navigable")
#define TAG_XMI_unspecified_navigable      Sequence(L"unspecified navigable")

#define TAG_XMI_operation      Sequence(L"operation")
#define TAG_XMI_function       Sequence(L"function")

#define TAG_XMI_partial        Sequence(L"partial")
#define TAG_XMI_total          Sequence(L"total")

#define TAG_XMI_expl           Sequence(L"expl")
#define TAG_XMI_impl           Sequence(L"impl")
#define TAG_XMI_extexpl        Sequence(L"extexpl")

#define TAG_XMI_precondition   Sequence(L"precondition")
#define TAG_XMI_postcondition  Sequence(L"postcondition")

#define TAG_XMI_fntype         Sequence(L"fntype")
#define TAG_XMI_deftype        Sequence(L"deftype")

#define TAG_XMI_isClass        Sequence(L"isClass")

#define QUOTE_PUBLIC           Quote(L"PUBLIC")
#define QUOTE_PRIVATE          Quote(L"PRIVATE")
#define QUOTE_PROTECTED        Quote(L"PROTECTED")
#define QUOTE_ORDERED          Quote(L"ORDERED")
#define QUOTE_NORMAL           Quote(L"NORMAL")

#define QUOTE_EXPL             Quote(L"EXPL")
#define QUOTE_IMPL             Quote(L"IMPL")
#define QUOTE_EXT              Quote(L"EXT")

#define QUOTE_TOTAL            Quote(L"TOTAL")
#define QUOTE_PARTIAL          Quote(L"PARTIAL")

#define QUOTE_ONE              Quote(L"ONE")
#define QUOTE_MANY             Quote(L"MANY")
#define QUOTE_ONE_TO_MANY      Quote(L"ONE_TO_MANY")
#define QUOTE_ZERO_ONE         Quote(L"ZERO_ONE")

enum {
  MODE_ASTAH,
  MODE_EnterpriseArchitect,
  MODE_UNKNOWN
};

class XmiAux
{
public:
  static Sequence UniqueID();
  static Sequence GetName(const Generic & g);
  static bool IsRef(const Generic & g);
  static Sequence GetID(const Generic & g);

  static Sequence GetElementName(const Generic & r, const Map & idm);
  static bool IsPrimitive(const Sequence & t); 
  static bool IsRealClassName(const Sequence & nm);
  static Sequence GetClasses(const TYPE_XMI_UMLModel & m);

  static SEQ<TYPE_XMI_UMLClass> GetAllClasses(const TYPE_XMI_UMLModel & m);
  static SEQ<TYPE_XMI_UMLGeneralization> GetAllGeneralizations(const TYPE_XMI_UMLModel & m);
  static SEQ<TYPE_XMI_UMLAssociationEnd> GetAllAssociationEnds(const TYPE_XMI_UMLModel & m);
  static SEQ<TYPE_XMI_UMLAssociation> GetAllAssociation(const TYPE_XMI_UMLModel & m);
  static SEQ<TYPE_XMI_UMLAttribute> GetAllAttributes(const TYPE_XMI_UMLClass & c);
  static SEQ<TYPE_XMI_UMLOperation> GetAllOperations(const TYPE_XMI_UMLClass & c);

  static bool IsFunction(const TYPE_XMI_UMLOperation & o, const Map & idm);
  static bool IsOperation(const TYPE_XMI_UMLOperation & o, const Map & idm);
  static bool IsImpl(const TYPE_XMI_UMLOperation & o, const Map & idm);
  static bool IsExtExpl(const TYPE_XMI_UMLOperation & o, const Map & idm);
  static bool IsPartial(const TYPE_XMI_UMLOperation & o, const Map & idm);

  static TYPE_XMI_UMLClassifier GetTypeRef(const TYPE_XMI_UMLAttribute & a);
  static TYPE_XMI_UMLClassifier GetTypeRef(const TYPE_XMI_UMLParameter & p);
  static SEQ<TYPE_XMI_UMLParameter> GetParam(const TYPE_XMI_UMLOperation & o);
  static TYPE_XMI_UMLParameter GetReturnType(const TYPE_XMI_UMLOperation & o);
  static Sequence GetPreCondition(const TYPE_XMI_UMLOperation & o);
  static Sequence GetPostCondition(const TYPE_XMI_UMLOperation & o);
  static Sequence GetFnType(const TYPE_XMI_UMLOperation & o);
  static Sequence GetDefType(const TYPE_XMI_UMLOperation & o);
  static bool IsClass(const TYPE_XMI_UMLClass & c);
  static bool IsAbstract(const TYPE_XMI_UMLClass & c);
  static Sequence SearchTagValue(const Sequence & key, const TYPE_XMI_UMLModelElement & me);
  static bool HasTagValue(const Sequence & key, const TYPE_XMI_UMLModelElement & me);
  static Bool IsStatic(const TYPE_XMI_UMLOperation & o);
  static Sequence GetVisibility(const TYPE_XMI_UMLOperation & o);
  static bool IsReturnType(const TYPE_XMI_UMLParameter & t); 
  static bool IsVoid(const TYPE_XMI_UMLParameter & p, const Map & idm);

  static Bool IsStatic(const TYPE_XMI_UMLAttribute & a);
  static Sequence GetVisibility(const TYPE_XMI_UMLAttribute & a);
  static bool IsChangeable(const TYPE_XMI_UMLAttribute & a);

  static Bool IsStatic(const TYPE_XMI_UMLAssociationEnd & ae, const Map & idm);
  static Sequence GetVisibility(const TYPE_XMI_UMLAssociationEnd & ae, const Map & idm);
  static Sequence GetMultiplicity(const TYPE_XMI_UMLAssociationEnd & ae, const Map & idm);
  static bool IsOrdered(const TYPE_XMI_UMLAssociationEnd & ae, const Map & idm);
  static Generic GetQualifer(const TYPE_XMI_UMLAssociationEnd & se,
                             const TYPE_XMI_UMLAssociationEnd & ce,
                             const Map & idm,
                             int mode);
  static Generic GetQualiferSeq(const TYPE_XMI_UMLAssociationEnd & se,
                                const TYPE_XMI_UMLAssociationEnd & ce,
                                const Map & idm,
                                int mode);
  static Generic GetQualifer(const TYPE_XMI_UMLAssociation & a, const Map & idm);
  static Generic GetInitValue(const TYPE_XMI_UMLAssociationEnd & ae, const Map & idm);
  static Generic GetInitType(const TYPE_XMI_UMLAssociationEnd & ae,
                             const Sequence & server,
                             const Quote & cd,
                             const Sequence & qualifer,
                             const Quote & role_t,
                             const Map & idm);

  static Sequence GetChildId(const TYPE_XMI_UMLGeneralization & ge);
  static Sequence GetParentId(const TYPE_XMI_UMLGeneralization & ge);

  static TYPE_XMI_UMLAssociationEnd GetRoleSource(const TYPE_XMI_UMLAssociation & a, const Map & idm);
  static TYPE_XMI_UMLAssociationEnd GetRoleDest(const TYPE_XMI_UMLAssociation & a, const Map & idm);
//  static Sequence GetServerClassName(const TYPE_XMI_UMLAssociation & a, const Map & idm);
//  static Sequence GetClientClassName(const TYPE_XMI_UMLAssociation & a, const Map & idm);
  static Sequence GetClassNameFromAssoc(const TYPE_XMI_UMLAssociationEnd & ae, const Map & idm);

  static Sequence PrintAttribute(const TYPE_XMI_UMLAttribute & a, const Map & idm);
  static Sequence PrintOperation(const TYPE_XMI_UMLOperation & o, const Map & idm);

  static TYPE_XMI_UMLModel mk_UMLModel(const Sequence & name, const Sequence & id);
  static TYPE_XMI_UMLClass mk_UMLClass(const Sequence & name,
                                       const Sequence & id,
                                       const Sequence & pid,
                                       const Bool & abstract); 
  static TYPE_XMI_UMLNamespace mk_UMLNamespace();
  static TYPE_XMI_UMLNamespace mk_UMLNamespace(const Map & attrs);
  static TYPE_XMI_UMLModelElement mk_UMLModelElement();
  static TYPE_XMI_UMLModelElement mk_UMLModelElement(const Generic & visibility);
  static TYPE_XMI_UMLModelElement mk_UMLModelElement(const Map & attrs,
                                                     const Map & ns_attrs,
                                                     const Generic & visibility);
  static TYPE_XMI_UMLStereotype mk_UMLStereotype(const Sequence & name,
                                                 const Sequence & id, 
                                                 const Sequence & fnopid);
  static TYPE_XMI_UMLTaggedValue mk_UMLTaggedValue(const Sequence & tag, const Sequence & value);
  static TYPE_XMI_UMLTaggedValue mk_UMLTaggedValue(const Sequence & tag,
                                                   const Sequence & value,
                                                   const Sequence & pid);
  static TYPE_XMI_Document mk_Document(const TYPE_XMI_UMLModel & m, const Sequence & pr_l, int mode);
  static TYPE_XMI_Header mk_Header(int mode);
  static TYPE_XMI_Documentation mk_Documentation(int mode);
  static TYPE_XMI_Metamodel mk_Metamodel();
  static TYPE_XMI_UMLFeature mk_UMLFeature();
  static TYPE_XMI_UMLFeature mk_UMLFeature(const Generic & visibility);
  static TYPE_XMI_UMLFeature mk_UMLFeature(const Map & attrs, const Generic & visibility);
  static TYPE_XMI_UMLStructuralFeature mk_UMLStructuralFeature();
  static TYPE_XMI_UMLStructuralFeature mk_UMLStructuralFeature(const Map & attrs);

  static TYPE_XMI_UMLAttribute mk_UMLAttribute(const Map & attrs);

  static TYPE_XMI_UMLConstraint mk_UMLConstraint(const Sequence & name,
                                                 const Sequence & prepost,
                                                 const Sequence & pid);
  static TYPE_XMI_UMLGeneralization mk_UMLGeneralization(const Sequence & pid);
  static TYPE_XMI_UMLGeneralization mk_UMLGeneralizationRef(const Sequence & id);
  static TYPE_XMI_UMLAssociationEnd mk_UMLAssociationEnd();
  static TYPE_XMI_UMLAssociationEnd mk_UMLAssociationEnd(const Map & attrs);
  static TYPE_XMI_UMLAssociationEnd mk_UMLAssociationEnd(const Sequence & name, const Sequence & pid);
  static TYPE_XMI_UMLAssociationEnd mk_UMLAssociationEndRef(const Sequence & id);

  static TYPE_XMI_UMLMultiplicity mk_UMLMultiplicity(const TYPE_XMI_UMLMultiplicityRange & rg);
  static TYPE_XMI_UMLMultiplicityRange mk_UMLMultiplicityRange(const Sequence & lower,
                                                               const Sequence & upper);
  static TYPE_XMI_UMLExpression mk_UMLExpression(const Sequence & val);
  static TYPE_XMI_UMLPrimitive mk_UMLPrimitive(const Sequence & tp);

  static bool IsEAModel(const TYPE_XMI_UMLModel & m);
  static int GetModeOfAssociation(const TYPE_XMI_UMLAssociation & m);

private:
  static Map GetAttribute(const Record & r);
  static Sequence GetAllElements(const TYPE_XMI_UMLModel & m);

  static Map createDefaultAttribute();
  static TYPE_XMI_Header createDefaultHeader();
  static TYPE_XMI_Documentation createDefaultDocumentation();
  static TYPE_XMI_Metamodel createDefaultMetamodel();

  static unsigned long uid;
};

#endif // __xmiaux_h__
