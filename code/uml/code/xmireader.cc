#include "metaiv.h"
#include "XMI.h"
#include "xmireader.h"
#include "tb_wstring.h"
#include "tbutils.h"
#include "xmimodel.h"
#include "xmiaux.h"
#include <string>
using namespace std;

XmiReader::XmiReader(int m)
{
  this->mode = m;
}

XmiReader::~XmiReader()
{
  CleanUp();
}

bool XmiReader::IsASTAH()
{
  return (this->mode == MODE_ASTAH);
}

bool XmiReader::IsEA()
{
  return (this->mode == MODE_EnterpriseArchitect);
}

void XmiReader::CleanUp()
{
  xmlCleanupParser();
}

// ReadXMIDocument
// filename : seq of char
// ==> bool * [XMI`Document] * [map seq of char to token] * [seq of char] * seq of char
Tuple XmiReader::ReadXMIDocument(const wstring & filename)
{
  this->reader = xmlReaderForFile(TBWSTR::wstring2fsstr(filename).c_str(),
                                  NULL,
                                  XML_PARSE_RECOVER |
                                  XML_PARSE_DTDATTR |  /* default DTD attributes */
                                  XML_PARSE_NOENT);    /* substitute entities */
  if (reader != NULL) {
    Generic ret = ReadDocument(this->reader);
    Sequence encoding (GetEncoding(reader));

//vdm_log << ret << endl;
//vdm_log << L"yet:" << endl;
//vdm_log << yet << endl;
//vdm_log << yet_astah << endl;
//vdm_log << L"not_set:" << endl;
//vdm_log << not_set << endl;
//vdm_log << L"encoding: " << encoding << endl;

    xmlFreeTextReader(this->reader);
    if (ret.IsNil()) {
      return mk_(Bool(false), Nil(), Nil(), Nil(), Sequence(filename + L" : failed to parse"));
    }
    return mk_(Bool(true), ret, this->idm, encoding, Sequence());
  } else {
    return mk_(Bool(false), Nil(), Nil(), Nil(), Sequence(L"Unable to open " + filename));
  }
}

int XmiReader::ReadNextLine(xmlTextReaderPtr reader)
{
  int ret = xmlTextReaderRead(reader);

#ifdef _MSC_VER
  while(ret != 1)
  {
    ret = xmlTextReaderRead(reader);
  }
#endif // _MSC_VER

#ifdef DEBUG_DUMP
  if (ret == 1)
    DumpNodeInfo(reader);
#endif // DEBUG_DUMP

  return ret;
}

// SetElement
// e : XMI`Element
// element : XMI`Element | seq of char
// ==> XMI`Element
Record XmiReader::SetElement(const Record & e, const Generic & element)
{
//vdm_log << L"SetElement" << endl;
//vdm_log << e << endl;
//vdm_log << element << endl;
//wcout << L"SetElement" << endl;
//wcout << e << endl;
//wcout << element << endl;
  switch(e.GetTag()) {
    case TAG_TYPE_XMI_Exporter: {
      return SetXMIExporter(e, element);
      break;
    }
    case TAG_TYPE_XMI_ExporterVersion: {
      return SetXMIExporterVersion(e, element);
      break;
    }
    case TAG_TYPE_XMI_Document: {
      return SetXMIDocument(e, element);
      break;
    }
    case TAG_TYPE_XMI_SortedVersionHistories: {
      return SetXMISortedVersionHistories(e, element);
      break;
    }
    case TAG_TYPE_XMI_Documentation: {
      return SetXMIDocumentation(e, element);
      break;
    }
    case TAG_TYPE_XMI_Header: {
      return SetXMIHeader(e, element);
      break;
    }
    case TAG_TYPE_XMI_Content: {
      return SetXMIContent(e, element);
      break;
    }
    case TAG_TYPE_XMI_Field: {
      return SetXMIField(e, element);
      break;
    }
    case TAG_TYPE_XMI_CurrentModelVersion:
    case TAG_TYPE_XMI_MaxModelVersion:
    case TAG_TYPE_XMI_CurrentModelProducer: {
      Record res (e);
      if (element.IsSequence())
        res.SetField(2, element);
      return res;
      break;
    }
    case TAG_TYPE_XMI_UMLModelElement: {
      return SetUMLModelElement(e, element);
      break;
    }
    case TAG_TYPE_XMI_UMLModel: {
      return SetUMLModel(e, element);
      break;
    }
    case TAG_TYPE_XMI_UMLTaggedValue: {
      return SetUMLTaggedValue(e, element);
      break;
    }
    case TAG_TYPE_XMI_UMLNamespace: {
      return SetUMLNamespace(e, element);
      break;
    }
    case TAG_TYPE_XMI_UMLPackage: {
      return SetUMLPackage(e, element);
      break;
    }
    case TAG_TYPE_XMI_UMLInterface: {
      return SetUMLInterface(e, element);
      break;
    }
    case TAG_TYPE_XMI_UMLClass: {
      return SetUMLClass(e, element);
      break;
    }
    case TAG_TYPE_XMI_UMLStereotype: {
      return SetUMLStereotype(e, element);
      break;
    }
    case TAG_TYPE_XMI_UMLClassifier: {
      return SetUMLClassifier(e, element);
      break;
    }
    case TAG_TYPE_XMI_UMLFeature: {
      return SetUMLFeature(e, element);
      break;
    }
    case TAG_TYPE_XMI_UMLOperation: {
      return SetUMLOperation(e, element);
      break;
    }
    case TAG_TYPE_XMI_UMLBehavioralFeature: {
      return SetUMLBehavioralFeature(e, element);
      break;
    }
    case TAG_TYPE_XMI_UMLParameter: {
      return SetUMLParameter(e, element);
      break;
    }
    case TAG_TYPE_XMI_UMLGeneralizableElement: {
      return SetUMLGeneralizableElement(e, element);
      break;
    }
    case TAG_TYPE_XMI_UMLGeneralization: {
      return SetUMLGeneralization(e, element);
      break;
    }
    case TAG_TYPE_XMI_UMLAssociation: {
      return SetUMLAssociation(e, element);
      break;
    }
    case TAG_TYPE_XMI_UMLAssociationEnd: {
      return SetUMLAssociationEnd(e, element);
      break;
    }
    case TAG_TYPE_XMI_UMLAttribute: {
      return SetUMLAttribute(e, element);
      break;
    }
    case TAG_TYPE_XMI_UMLStructuralFeature: {
      return SetUMLStructuralFeature(e, element);
      break;
    }
    case TAG_TYPE_XMI_UMLMultiplicity: {
      return SetUMLMultiplicity(e, element);
      break;
    }
    case TAG_TYPE_XMI_UMLExpression: {
      return SetUMLExpression(e, element);
      break;
    }
    case TAG_TYPE_XMI_UMLBooleanExpression: {
      return SetUMLBooleanExpression(e, element);
      break;
    }
    case TAG_TYPE_XMI_UMLConstraint: {
      return SetUMLConstraint(e, element);
      break;
    }
    case TAG_TYPE_XMI_UMLDiagram: {
      return SetUMLDiagram(e, element);
      break;
    }
    default: {
      break;
    }
  }
  ReportCanNotSet(e, element);
  return e;
}

TYPE_XMI_Exporter XmiReader::SetXMIExporter(const TYPE_XMI_Exporter & e,
                                            const Generic & element)
{
  if (element.IsSequence())
  {
    TYPE_XMI_Exporter res (e);
    res.set_text(element);
    CheckExporter(element);
    return res;
  }
  ReportCanNotSet(e, element);
  return e;
}

TYPE_XMI_ExporterVersion XmiReader::SetXMIExporterVersion(
                                       const TYPE_XMI_ExporterVersion & e,
                                       const Generic & element)
{
  if (element.IsSequence())
  {
    TYPE_XMI_ExporterVersion res (e);
    res.set_text(element);
    CheckExporterVersion(element);
    return res;
  }
  ReportCanNotSet(e, element);
  return e;
}

// SetXMIDocument
// e : XMI`Document
// element : XMI`Element | seq of char
// ==> XMI`Document
TYPE_XMI_Document XmiReader::SetXMIDocument(const TYPE_XMI_Document & e,
                                            const Generic & element)
{
  if (element.IsRecord())
  {
    TYPE_XMI_Document res (e);
    switch(Record(element).GetTag()) {
      case TAG_TYPE_XMI_Header: {
        res.set_header(element);
        break;
      }
      case TAG_TYPE_XMI_Content: {
        res.set_content(element);
        break;
      }
      case TAG_TYPE_XMI_Difference: {
        res.set_difference(element);
        break;
      }
      case TAG_TYPE_XMI_Extensions: {
        res.set_extensions(element);
        break;
      }
    }
    return res;
  }
  ReportCanNotSet(e, element);
  return e;
}

TYPE_XMI_Header XmiReader::SetXMIHeader(const TYPE_XMI_Header & e,
                                        const Generic & element)
{
  if (element.IsRecord())
  {
    TYPE_XMI_Header res (e); 
    switch(Record(element).GetTag()) {
      case TAG_TYPE_XMI_Documentation: {
        res.set_documentation(element);
        break;
      }
      case TAG_TYPE_XMI_Import: {
        res.set_import(element);
        break;
      }
      case TAG_TYPE_XMI_Metametamodel: {
        res.set_documentation(element);
        break;
      }
      case TAG_TYPE_XMI_Metamodel: {
        res.set_metamodel(element);
        break;
      }
      case TAG_TYPE_XMI_Model: {
        res.set_model(element);
        break;
      }
    }
    return res;
  }
  ReportCanNotSet(e, element);
  return e;
}

TYPE_XMI_Content XmiReader::SetXMIContent(const TYPE_XMI_Content & e,
                                          const Generic & element)
{
  if (element.IsRecord())
  {
    TYPE_XMI_Content res (e); 
    switch(Record(element).GetTag()) {
      case TAG_TYPE_XMI_Extension: {
        Sequence extensions (res.get_extensions());
        extensions.ImpAppend(element);
        res.set_extensions(extensions);
        break;
      }
      case TAG_TYPE_XMI_UMLPrimitive: {
        Sequence primitives (res.get_primitives());
        primitives.ImpAppend(element);
        res.set_primitives(primitives);
        break;
      }
      case TAG_TYPE_XMI_UMLModel: {
        res.set_model(element);
        break;
      }
      case TAG_TYPE_XMI_UMLDiagram: {
        res.set_diagram(element);
        break;
      }
    }
    return res;
  }
  ReportCanNotSet(e, element);
  return e;
}

TYPE_XMI_Documentation XmiReader::SetXMIDocumentation(
                                     const TYPE_XMI_Documentation & e,
                                     const Generic & element)
{
  if (element.IsRecord())
  {
    TYPE_XMI_Documentation res (e); 
    switch(Record(element).GetTag()) {
      case TAG_TYPE_XMI_Exporter: {
        res.set_exporter(element);
        break;
      }
      case TAG_TYPE_XMI_ExporterVersion: {
        res.set_exporterVersion(element);
        break;
      }
      case TAG_TYPE_XMI_CurrentModelVersion: {
        res.set_currentModelVersion(element);
        break;
      }
      case TAG_TYPE_XMI_MaxModelVersion: {
        res.set_maxModelVersion(element);
        break;
      }
      case TAG_TYPE_XMI_CurrentModelProducer: {
        res.set_currentModelProducer(element);
        break;
      }
      case TAG_TYPE_XMI_SortedVersionHistories: {
        res.set_sortedVersionHistories(element);
        break;
      }
    }
    return res;
  }
  ReportCanNotSet(e, element);
  return e;
}

TYPE_XMI_SortedVersionHistories XmiReader::SetXMISortedVersionHistories(
                                              const TYPE_XMI_SortedVersionHistories & e,
                                              const Generic & element)
{
  if (element.IsRecord())
  {
    TYPE_XMI_SortedVersionHistories res (e);
    Sequence versionEntry (res.get_versionEntry());
    versionEntry.ImpAppend(element);
    res.set_versionEntry(versionEntry);
    return res;
  }
  ReportCanNotSet(e, element);
  return e;
}

TYPE_XMI_Field XmiReader::SetXMIField(const TYPE_XMI_Field & e,
                                      const Generic & element)
{
  if (element.IsSequence())
  {
    TYPE_XMI_Field res (e);
    res.set_field(element);
    return res;
  } 
  ReportCanNotSet(e, element);
  return e;
}


TYPE_XMI_UMLModelElement XmiReader::SetUMLModelElement (
                                       const TYPE_XMI_UMLModelElement & e,
                                       const Generic & element)
{
  if (element.IsRecord())
  {
    TYPE_XMI_UMLModelElement res (e);
    switch(Record(element).GetTag()) {
      case TAG_TYPE_XMI_UMLNamespace: {
        Sequence nspace (res.get_namespace()); 
        nspace.ImpAppend(element);
        res.set_namespace(nspace);
        return res;
        break;
      }
      case TAG_TYPE_XMI_UMLStereotype: {
        Sequence stereotype (res.get_stereotype()); 
        stereotype.ImpAppend(element);
        res.set_stereotype(stereotype);
        return res;
        break;
      }
      case TAG_TYPE_XMI_UMLTaggedValue: {
        Sequence taggedValue (res.get_taggedValue()); 
        taggedValue.ImpAppend(element);
        res.set_taggedValue(taggedValue);
        return res;
        break;
      }
      case TAG_TYPE_XMI_UMLConstraint: {
        Sequence constraint (res.get_constraint()); 
        constraint.ImpAppend(element);
        res.set_constraint(constraint);
        return res;
        break;
      }
      case TAG_TYPE_XMI_UMLModelElement: {
        TYPE_XMI_UMLModelElement me (element);
        if (!me.get_namespace().IsEmpty())
        {
          Sequence nspace (res.get_namespace());
          nspace.ImpConc(me.get_namespace());
          res.set_namespace(nspace);
        }
        if (!me.get_visibility().IsNil())
        {
          res.set_visibility(me.get_visibility());
        }
        if (!me.get_definition().IsNil())
        {
          res.set_definition(me.get_definition());
        }
        if (!me.get_stereotype().IsEmpty())
        {
          Sequence stereotype (res.get_stereotype());
          stereotype.ImpConc(me.get_stereotype());
          res.set_stereotype(stereotype);
        }
        if (!me.get_taggedValue().IsEmpty())
        {
          Sequence taggedValue (res.get_taggedValue());
          taggedValue.ImpConc(me.get_taggedValue());
          res.set_taggedValue(taggedValue);
        }
        if (!me.get_constraint().IsEmpty())
        {
          Sequence constraint (res.get_constraint());
          constraint.ImpConc(me.get_constraint());
          res.set_constraint(constraint);
        }
        return res;
        break;
      }
    }
  }
  not_set.Insert(mk_(e, element));
  return e;
}

TYPE_XMI_UMLNamespace XmiReader::SetUMLNamespace(const TYPE_XMI_UMLNamespace & e,
                                                 const Generic & element)
{
  if (element.IsRecord())
  {
    TYPE_XMI_UMLNamespace res (e);
    switch(Record(element).GetTag()) {
      case TAG_TYPE_XMI_UMLStereotype:
      case TAG_TYPE_XMI_UMLPackage:
      case TAG_TYPE_XMI_UMLInterface:
      case TAG_TYPE_XMI_UMLGeneralization:
      case TAG_TYPE_XMI_UMLAssociation:
      case TAG_TYPE_XMI_UMLAssociationEnd:
      case TAG_TYPE_XMI_UMLConstraint:
      case TAG_TYPE_XMI_UMLDataType:
      case TAG_TYPE_XMI_UMLClass: {
        Sequence ownedElement (res.get_ownedElement());
        ownedElement.ImpAppend(element);
        res.set_ownedElement(ownedElement);
        break;
      }
      case TAG_TYPE_XMI_UMLNamespace: {
        TYPE_XMI_UMLNamespace ns (element);
        if (!ns.get_ownedElement().IsEmpty())
        {
          Sequence ownedElement (res.get_ownedElement());
          ownedElement.ImpConc(ns.get_ownedElement());
          res.set_ownedElement(ownedElement);
        }
        break;
      }
    }
    return res;
  }
  ReportCanNotSet(e, element);
  return e;
}


TYPE_XMI_UMLModel XmiReader::SetUMLModel (const TYPE_XMI_UMLModel & e,
                                          const Generic & element)
{
  if (element.IsRecord())
  {
    TYPE_XMI_UMLModel res (e);
    switch(Record(element).GetTag()) {
      case TAG_TYPE_XMI_UMLModelElement: {
        res.set_modelElement(SetElement(res.get_modelElement(), element));
        break;
      }
      case TAG_TYPE_XMI_UMLNamespace: {
        res.set_namespace(SetElement(res.get_namespace(), element));
        break;
      }
    }
    return res;
  }
  ReportCanNotSet(e, element);
  return e;
}

TYPE_XMI_UMLTaggedValue XmiReader::SetUMLTaggedValue(const TYPE_XMI_UMLTaggedValue & e,
                                                     const Generic & element)
{
  if (element.IsRecord())
  {
    TYPE_XMI_UMLTaggedValue res (e);
    switch(Record(element).GetTag()) {
      case TAG_TYPE_XMI_UMLModelElement: {
        res.set_modelElement(element);
        break;
      }
      case TAG_TYPE_XMI_UMLTaggedValue: {
        res.set_modelElement(TYPE_XMI_UMLTaggedValue(element).get_modelElement());
        break;
      }
    }
    return res;
  }
  ReportCanNotSet(e, element);
  return e;
}

TYPE_XMI_UMLClass XmiReader::SetUMLClass(const TYPE_XMI_UMLClass & e,
                                         const Generic & element)
{
  if (element.IsRecord())
  {
    TYPE_XMI_UMLClass res (e);
    switch(Record(element).GetTag()) {
      case TAG_TYPE_XMI_UMLModelElement: {
        res.set_modelElement(SetElement(res.get_modelElement(), element));
        break;
      }
      case TAG_TYPE_XMI_UMLNamespace: {
        if (res.get_namespace().IsNil())
          res.set_namespace(element);
        else
          res.set_namespace(SetElement(res.get_namespace(), element));
        break;
      }
      case TAG_TYPE_XMI_UMLGeneralizableElement: {
        if (res.get_generalizableElement().IsNil())
          res.set_generalizableElement(element);
        else
          res.set_generalizableElement(SetElement(res.get_generalizableElement(), element));
        break;
      }
      case TAG_TYPE_XMI_UMLClassifier: {
        res.set_classifier(element);
        break;
      }
    }
    return res;
  }
  ReportCanNotSet(e, element);
  return e;
}

TYPE_XMI_UMLPackage XmiReader::SetUMLPackage(const TYPE_XMI_UMLPackage & e,
                                             const Generic & element)
{
  if (element.IsRecord())
  {
    TYPE_XMI_UMLPackage res (e);
    switch(Record(element).GetTag()) {
      case TAG_TYPE_XMI_UMLModelElement: {
        res.set_modelElement(SetElement(res.get_modelElement(), element));
        break;
      }
      case TAG_TYPE_XMI_UMLNamespace: {
        res.set_namespace(SetElement(res.get_namespace(), element));
        break;
      }
    }
    return res;
  }
  ReportCanNotSet(e, element);
  return e;
}

TYPE_XMI_UMLInterface XmiReader::SetUMLInterface(const TYPE_XMI_UMLInterface & e,
                                                 const Generic & element)
{
  if (element.IsRecord())
  { 
    TYPE_XMI_UMLInterface res (e);
    switch(Record(element).GetTag()) {
      case TAG_TYPE_XMI_UMLModelElement: {
        res.set_modelElement(SetElement(res.get_modelElement(), element));
        break;
      }
    }
    return res;
  }
  ReportCanNotSet(e, element);
  return e;
}

TYPE_XMI_UMLStereotype XmiReader::SetUMLStereotype(const TYPE_XMI_UMLStereotype & e,
                                                    const Generic & element)
{
  if (element.IsRecord())
  {
    TYPE_XMI_UMLStereotype res (e);
    switch(Record(element).GetTag()) {
      case TAG_TYPE_XMI_UMLModelElement: {
        Sequence extendedElement (res.get_extendedElement());
        extendedElement.ImpAppend(element);
        res.set_extendedElement(extendedElement);
        break;
      }
      case TAG_TYPE_XMI_UMLStereotype: {
        TYPE_XMI_UMLStereotype st (element);
        if (!st.get_baseClass().IsEmpty())
          res.set_baseClass(st.get_baseClass());
        if (!st.get_extendedElement().IsEmpty())
        {
          Sequence extendedElement (res.get_extendedElement());
          extendedElement.ImpConc(st.get_extendedElement());
          res.set_extendedElement(extendedElement);
        }
        break;
      }
    }
    return res;
  }
  else if (element.IsSequence())
  {
    TYPE_XMI_UMLStereotype res (e);
    res.set_baseClass(element); 
    return res;
  }     
  ReportCanNotSet(e, element);
  return e;
}

TYPE_XMI_UMLClassifier XmiReader::SetUMLClassifier(const TYPE_XMI_UMLClassifier & e,
                                                   const Generic & element)
{
  if (element.IsRecord())
  {
    TYPE_XMI_UMLClassifier res (e);
    switch(Record(element).GetTag()) {
      case TAG_TYPE_XMI_UMLAttribute:
      case TAG_TYPE_XMI_UMLOperation: {
        Sequence feature (res.get_feature());
        feature.ImpAppend(element);
        res.set_feature(feature);
      }
    }
    return res;
  }
  ReportCanNotSet(e, element);
  return e;
}

TYPE_XMI_UMLFeature XmiReader::SetUMLFeature(const TYPE_XMI_UMLFeature & e,
                                             const Generic & element)
{
  if (element.IsRecord())
  {
    TYPE_XMI_UMLFeature res (e);
    switch(Record(element).GetTag()) {
      case TAG_TYPE_XMI_UMLClassifier: {
        res.set_owner(element);
        break;
      }
      case TAG_TYPE_XMI_UMLFeature: {
        TYPE_XMI_UMLFeature fe (element);
        if (!fe.get_owner().IsNil())
        {
          res.set_owner(fe.get_owner());
        }
        if (!fe.get_visibility().IsNil())
          res.set_visibility(fe.get_visibility());
        break;
      }
    }
    return res;
  }
  ReportCanNotSet(e, element);
  return e;
}

TYPE_XMI_UMLOperation XmiReader::SetUMLOperation(const TYPE_XMI_UMLOperation & e,
                                                 const Generic & element)
{
  if (element.IsRecord())
  {
    TYPE_XMI_UMLOperation res (e);
    switch(Record(element).GetTag()) {
      case TAG_TYPE_XMI_UMLModelElement: {
        res.set_modelElement(SetElement(res.get_modelElement(), element));
        break;
      }
      case TAG_TYPE_XMI_UMLFeature: {
        res.set_feature(SetElement(res.get_feature(), element));
        break;
      }
      case TAG_TYPE_XMI_UMLBehavioralFeature: {
        res.set_behavioralFeature(SetElement(res.get_behavioralFeature(), element));
        break;
      }
    }
    return res;
  }
  ReportCanNotSet(e, element);
  return e;
}

TYPE_XMI_UMLBehavioralFeature XmiReader::SetUMLBehavioralFeature(
                                            const TYPE_XMI_UMLBehavioralFeature & e,
                                            const Generic & element)
{
  if (element.IsRecord())
  {
    TYPE_XMI_UMLBehavioralFeature res (e);
    switch(Record(element).GetTag()) {
      case TAG_TYPE_XMI_UMLParameter: {
        Sequence parameter (res.get_parameter());
        parameter.ImpAppend(element);
        res.set_parameter(parameter);
        break;
      }
      case TAG_TYPE_XMI_UMLBehavioralFeature: {
        TYPE_XMI_UMLBehavioralFeature bf (element);
        if (!bf.get_parameter().IsEmpty()) 
        {
          Sequence parameter (res.get_parameter());
          parameter.ImpConc(bf.get_parameter());
          res.set_parameter(parameter);
        }
        break;
      }
    }
    return res;
  }
  ReportCanNotSet(e, element);
  return e;
}

TYPE_XMI_UMLParameter XmiReader::SetUMLParameter(const TYPE_XMI_UMLParameter & e,
                                                 const Generic & element)
{
  if (element.IsRecord())
  {
    TYPE_XMI_UMLParameter res (e);
    switch(Record(element).GetTag()) {
      case TAG_TYPE_XMI_UMLModelElement: {
        res.set_modelElement(SetElement(res.get_modelElement(), element));
        break;
      }
      case TAG_TYPE_XMI_UMLBehavioralFeature: {
        res.set_behavioralFeature(element);
        break;
      }
      case TAG_TYPE_XMI_UMLClassifier: {
        res.set_type(element);
        break;
      }
      case TAG_TYPE_XMI_UMLExpression: {
        res.set_defaultValue(element);
        break;
      }
      case TAG_TYPE_XMI_UMLParameter: {
        TYPE_XMI_UMLParameter pa (element);
        res.set_modelElement(SetElement(res.get_modelElement(),
                                        pa.get_modelElement()));
        if (!pa.get_behavioralFeature().IsNil())
        {
          res.set_behavioralFeature(pa.get_behavioralFeature());
        } 
        if (!pa.get_type().IsNil())
        {
          res.set_type(pa.get_type());
        }
        if (!pa.get_defaultValue().IsNil())
        {
          res.set_defaultValue(pa.get_defaultValue());
        }
        break;
      }
    }
    return res;
  }
  ReportCanNotSet(e, element);
  return e;
}

TYPE_XMI_UMLGeneralizableElement XmiReader::SetUMLGeneralizableElement(
                                               const TYPE_XMI_UMLGeneralizableElement & e,
                                               const Generic & element)
{
  if (element.IsRecord())
  {
    TYPE_XMI_UMLGeneralizableElement res (e);
    switch(Record(element).GetTag()) {
      case TAG_TYPE_XMI_UMLGeneralization: {
        Sequence generalization (res.get_generalization());
        generalization.ImpAppend(element);
        res.set_generalization(generalization);
        break;
      }
      case TAG_TYPE_XMI_UMLGeneralizableElement: {
        TYPE_XMI_UMLGeneralizableElement ge (element);
        if (!ge.get_generalization().IsEmpty())
        {
          Sequence generalization (res.get_generalization());
          generalization.ImpConc(ge.get_generalization());
          res.set_generalization(generalization);
        } 
      }
    }
    return res;
  }
  ReportCanNotSet(e, element);
  return e;
}

TYPE_XMI_UMLGeneralization XmiReader::SetUMLGeneralization(const TYPE_XMI_UMLGeneralization & e,
                                                           const Generic & element)
{
  if (element.IsRecord())
  {
    TYPE_XMI_UMLGeneralization res (e);
    switch(Record(element).GetTag()) {
      case TAG_TYPE_XMI_UMLModelElement: {
        res.set_modelElement(SetElement(res.get_modelElement(), element));
        break;
      }
      case TAG_TYPE_XMI_UMLGeneralization: {
        TYPE_XMI_UMLGeneralization ge (element);
        res.set_modelElement(SetElement(res.get_modelElement(),
                                        ge.get_modelElement()));
        if (!ge.get_child().IsNil())
        {
          res.set_child(ge.get_child());
        }
        if (!ge.get_parent().IsNil())
        {
          res.set_parent(ge.get_parent());
        }
        break;
      }
      case TAG_TYPE_XMI_UMLGeneralizableElement: {
        if (res.get_flag() == Bool(true))
        {
          res.set_child(element);
        }
        else if (res.get_flag() == Bool(false))
        {
          res.set_parent(element);
        }
        break;
      }
    }
    return res;
  }
  ReportCanNotSet(e, element);
  return e;
}

TYPE_XMI_UMLAssociation XmiReader::SetUMLAssociation(const TYPE_XMI_UMLAssociation & e,
                                                     const Generic & element)
{
  if (element.IsRecord())
  {
    TYPE_XMI_UMLAssociation res (e);
    switch(Record(element).GetTag()) {
      case TAG_TYPE_XMI_UMLModelElement: {
        res.set_modelElement(SetElement(res.get_modelElement(), element));
        break;
      }
      case TAG_TYPE_XMI_UMLAssociationEnd: {
        Sequence connection (res.get_connection());
        connection.ImpAppend(element);
        res.set_connection(connection);
        break;
      }
      case TAG_TYPE_XMI_UMLAssociation: {
        TYPE_XMI_UMLAssociation as (element);
        res.set_modelElement(SetElement(res.get_modelElement(),
                                        as.get_modelElement()));
        if (!as.get_connection().IsEmpty())
        {
          Sequence connection (res.get_connection());
          connection.ImpConc(as.get_connection());
          res.set_connection(connection);
        }
        break;
      }
    }
    return res;
  }
  ReportCanNotSet(e, element);
  return e;
}

TYPE_XMI_UMLAssociationEnd XmiReader::SetUMLAssociationEnd(const TYPE_XMI_UMLAssociationEnd & e,
                                                           const Generic & element)
{
  if (element.IsRecord())
  {
    TYPE_XMI_UMLAssociationEnd res (e);
    switch(Record(element).GetTag()) {
      case TAG_TYPE_XMI_UMLModelElement: {
        res.set_modelElement(SetElement(res.get_modelElement(), element));
        break;
      }
      case TAG_TYPE_XMI_UMLFeature: {
        res.set_feature(SetElement(res.get_feature(), element));
        break;
      }
      case TAG_TYPE_XMI_UMLStructuralFeature: {
        res.set_structuralFeature(SetElement(res.get_structuralFeature(), element));
        break;
      }
      case TAG_TYPE_XMI_UMLClassifier: {
        res.set_participant(element);
        break;
      }
      case TAG_TYPE_XMI_UMLExpression: {
        res.set_initialValue(element);
        break;
      }
      case TAG_TYPE_XMI_UMLAssociation: {
        res.set_association(element);
        break;
      }
      case TAG_TYPE_XMI_UMLAttribute: {
        res.set_qualifier(element);
        break;
      }
      case TAG_TYPE_XMI_UMLAssociationEnd: {
        TYPE_XMI_UMLAssociationEnd ae (element);
        res.set_modelElement(SetElement(res.get_modelElement(),
                                        ae.get_modelElement()));
        res.set_feature(SetElement(res.get_feature(),
                                   ae.get_feature()));
        res.set_structuralFeature(SetElement(res.get_structuralFeature(),
                                             ae.get_structuralFeature()));
        if (!ae.get_participant().IsNil())
        {
          res.set_participant(ae.get_participant());
        }
        if (!ae.get_initialValue().IsNil())
        {
          res.set_initialValue(ae.get_initialValue());
        }
        if (!ae.get_association().IsNil())
        {
          res.set_association(ae.get_association());
        }
        if (!ae.get_visibility().IsNil())
        {
          res.set_visibility(ae.get_visibility());
        }
        if (!ae.get_qualifier().IsNil())
        {
          res.set_qualifier(ae.get_qualifier());
        }
        break;
      }
    }
    return res;
  }
  ReportCanNotSet(e, element);
  return e;
}

TYPE_XMI_UMLAttribute XmiReader::SetUMLAttribute(const TYPE_XMI_UMLAttribute & e,
                                                 const Generic & element)
{
  if (element.IsRecord())
  {
    TYPE_XMI_UMLAttribute res (e);
    switch(Record(element).GetTag()) {
      case TAG_TYPE_XMI_UMLModelElement: {
        res.set_modelElement(SetElement(res.get_modelElement(), element));
        break;
      }
      case TAG_TYPE_XMI_UMLFeature: {
        res.set_feature(SetElement(res.get_feature(), element));
        break;
      }
      case TAG_TYPE_XMI_UMLStructuralFeature: {
        res.set_structuralFeature(SetElement(res.get_structuralFeature(), element));
        break;
      }
      case TAG_TYPE_XMI_UMLExpression: {
        res.set_initialValue(element);
        break;
      }
      case TAG_TYPE_XMI_UMLAssociationEnd: {
        res.set_associationEnd(element);
        break;
      }
      case TAG_TYPE_XMI_UMLAttribute: {
        TYPE_XMI_UMLAttribute at (element);
//
        if (!at.get_attrs().IsEmpty())
          res.set_attrs(at.get_attrs());
//
        res.set_modelElement(SetElement(res.get_modelElement(),
                                        at.get_modelElement()));
        res.set_feature(SetElement(res.get_feature(),
                                   at.get_feature()));
        res.set_structuralFeature(SetElement(res.get_structuralFeature(),
                                             at.get_structuralFeature()));
        if (!at.get_initialValue().IsNil())
        {
          res.set_initialValue(at.get_initialValue());
        }

        if (!at.get_associationEnd().IsNil())
        {
          res.set_associationEnd(at.get_associationEnd());
        }
        break;
      }
    }
    return res;
  }
  ReportCanNotSet(e, element);
  return e;
}

TYPE_XMI_UMLStructuralFeature XmiReader::SetUMLStructuralFeature(
                                   const TYPE_XMI_UMLStructuralFeature & e,
                                   const Generic & element)
{
  if (element.IsRecord())
  {
    TYPE_XMI_UMLStructuralFeature res (e);
    switch(Record(element).GetTag()) {
      case TAG_TYPE_XMI_UMLMultiplicity: {
        res.set_multiplicity(element);
        break;
      }
      case TAG_TYPE_XMI_UMLClassifier: {
        res.set_type(element);
        break;
      }
      case TAG_TYPE_XMI_UMLStructuralFeature: {
        TYPE_XMI_UMLStructuralFeature sf (element);
        if (!sf.get_multiplicity().IsNil())
        {
          res.set_multiplicity(sf.get_multiplicity());
        }
        if (!sf.get_type().IsNil())
        {
          res.set_type(sf.get_type());
        }
        break;
      }
    }
    return res;
  }
  ReportCanNotSet(e, element);
  return e;
}

TYPE_XMI_UMLMultiplicity XmiReader::SetUMLMultiplicity(const TYPE_XMI_UMLMultiplicity & e,
                                                       const Generic & element)
{
  if (element.IsRecord())
  {
    TYPE_XMI_UMLMultiplicity res (e);
    switch(Record(element).GetTag()) {
      case TAG_TYPE_XMI_UMLMultiplicityRange: {
        res.set_range(element);
        break;
      }
      case TAG_TYPE_XMI_UMLMultiplicity: {
        TYPE_XMI_UMLMultiplicity mu (element);
        if (!mu.get_range().IsNil())
        {
          res.set_range(mu.get_range());
        }
        break;
      }
    }
    return res;
  }
  ReportCanNotSet(e, element);
  return e;
}

TYPE_XMI_UMLExpression XmiReader::SetUMLExpression(const TYPE_XMI_UMLExpression & e,
                                                   const Generic & element)
{
  if (element.IsRecord())
  {
    TYPE_XMI_UMLExpression res (e);
    switch(Record(element).GetTag()) {
      case TAG_TYPE_XMI_UMLExpression: {
        TYPE_XMI_UMLExpression ex (element);
        if (!ex.get_body().IsNil())
          res.set_body(ex.get_body());
        break;
      }
    }
    return res;
  }
  else if (element.IsSequence())
  { 
    TYPE_XMI_UMLExpression res (e);
    res.set_body(element);
    return res;
  }
  ReportCanNotSet(e, element);
  return e;
}

TYPE_XMI_UMLBooleanExpression XmiReader::SetUMLBooleanExpression(
                                            const TYPE_XMI_UMLBooleanExpression & e,
                                            const Generic & element)
{
  if (element.IsRecord())
  {
    TYPE_XMI_UMLBooleanExpression res (e);
    switch(Record(element).GetTag()) {
      case TAG_TYPE_XMI_UMLExpression: {
        TYPE_XMI_UMLExpression ex (element);
        if (!ex.get_body().IsNil())
          res.set_body(ex.get_body());
        break;
      }
    }
    return res;
  }
  ReportCanNotSet(e, element);
  return e;
}

TYPE_XMI_UMLConstraint XmiReader::SetUMLConstraint(const TYPE_XMI_UMLConstraint & e,
                                                   const Generic & element)
{
  if (element.IsRecord())
  {
    TYPE_XMI_UMLConstraint res (e);
    switch(Record(element).GetTag()) {
      case TAG_TYPE_XMI_UMLModelElement: {
        res.set_modelElement(SetElement(res.get_modelElement(), element));
        break;
      }
      case TAG_TYPE_XMI_UMLConstraint: {
        TYPE_XMI_UMLConstraint ct (element);
        res.set_modelElement(SetElement(res.get_modelElement(),
                                        ct.get_modelElement()));
        if (!ct.get_body().IsNil())
          res.set_body(ct.get_body());
        if (!ct.get_constrainedElement().IsNil())
          res.set_constrainedElement(ct.get_constrainedElement());
        break;
      }
      case TAG_TYPE_XMI_UMLBooleanExpression: {
        res.set_body(element);
        break;
      }
    }
    return res;
  }
  ReportCanNotSet(e, element);
  return e;
}

TYPE_XMI_UMLDiagram XmiReader::SetUMLDiagram(const TYPE_XMI_UMLDiagram & e,
                                             const Generic & element)
{
  if (element.IsRecord())
  {
    TYPE_XMI_UMLDiagram res (e);
    switch(Record(element).GetTag()) {
      case TAG_TYPE_XMI_UMLModelElement: {
        res.set_modelElement(SetElement(res.get_modelElement(), element));
        break;
      }
      case TAG_TYPE_XMI_UMLDiagram: {
        TYPE_XMI_UMLDiagram dm (element);
        res.set_modelElement(SetElement(res.get_modelElement(),
                                        dm.get_modelElement()));
        if (!dm.get_element().IsEmpty())
        {
          Sequence delement (res.get_element());
          delement.ImpConc(dm.get_element());
          res.set_element(delement);
        }
        break;
      }
      case TAG_TYPE_XMI_UMLDiagramElement: {
        Sequence delement (res.get_element());
        delement.ImpAppend(element);
        res.set_element(delement);
        break;
      }
    }
    return res;
  }
  ReportCanNotSet(e, element);
  return e;
}




/*
{
  ReportCanNotSet(e, element);
  return e;
}

*/


Record XmiReader::CreateNewElement(xmlTextReaderPtr reader)
{
  Map attrs (GetAttribute(reader));
  const xmlChar *name = xmlTextReaderConstName(reader);
  if (name == NULL)
  {
    vdm_log << L"error " << endl;
    return Record();
  }

  wstring nm (TBWSTR::utf8str2wstring((const char *)name).c_str());

  if (nm == L"XMI")
    return TYPE_XMI_Document().Init(attrs, 
                                    TYPE_XMI_Header().Init(Map(), Nil(), Nil(), Nil(), Nil(), Nil()),
                                    TYPE_XMI_Content().Init(Map(), Nil(), Sequence(), Sequence(), Nil()),
                                    Nil(),
                                    Nil());
  else if (nm == L"XMI.header")
    return TYPE_XMI_Header().Init(attrs, Nil(), Nil(), Nil(), Nil(), Nil());
  else if (nm == L"XMI.content")
    return TYPE_XMI_Content().Init(attrs, Nil(), Sequence(), Sequence(), Nil());
  else if (nm == L"XMI.difference")
    return TYPE_XMI_Difference().Init(attrs);
  else if (nm == L"XMI.extensions")
    return TYPE_XMI_Extensions().Init(attrs);
  else if (nm == L"XMI.documentation")
    return TYPE_XMI_Documentation().Init(attrs, Nil(), Nil(), Nil(), Nil(), Nil(), Nil());
  else if (nm == L"XMI.exporter")
    return TYPE_XMI_Exporter().Init(attrs, Nil());
  else if (nm == L"XMI.exporterVersion")
    return TYPE_XMI_ExporterVersion().Init(attrs, Nil());
  else if (nm == L"XMI.currentModelVersion")
    return TYPE_XMI_CurrentModelVersion().Init(attrs, Nil());
  else if (nm == L"XMI.maxModelVersion")
    return TYPE_XMI_MaxModelVersion().Init(attrs, Nil());
  else if (nm == L"XMI.currentModelProducer")
    return TYPE_XMI_CurrentModelProducer().Init(attrs, Nil());
  else if (nm == L"XMI.sortedVersionHistories")
    return TYPE_XMI_SortedVersionHistories().Init(attrs, Sequence());
  else if (nm == L"XMI.import")
    return TYPE_XMI_Import().Init(attrs, Nil());
  else if (nm == L"XMI.metametamodel")
    return TYPE_XMI_Metametamodel().Init(attrs);
  else if (nm == L"XMI.metamodel")
    return TYPE_XMI_Metamodel().Init(attrs);
  else if (nm == L"XMI.model")
    return TYPE_XMI_Model().Init(attrs);
  else if (nm == L"XMI.versionEntry")
    return TYPE_XMI_VersionEntry().Init(attrs);
  else if (nm == L"XMI.field")
    return TYPE_XMI_Field().Init(attrs, Nil());
  else if (nm == L"XMI.extension")
    return TYPE_XMI_Extension().Init(attrs, Sequence());
  else if (nm == L"UML:Model")
    return TYPE_XMI_UMLModel().Init(attrs,
                                    XmiAux::mk_UMLModelElement(),
                                    XmiAux::mk_UMLNamespace());
  else if (nm == L"UML:Package")
    return TYPE_XMI_UMLPackage().Init(attrs, 
                                      XmiAux::mk_UMLModelElement(),
                                      XmiAux::mk_UMLNamespace());
  else if (nm == L"UML:Interface")
    return TYPE_XMI_UMLInterface().Init(attrs, XmiAux::mk_UMLModelElement());
  else if (nm == L"UML:Class")
    return TYPE_XMI_UMLClass().Init(attrs,
                                    XmiAux::mk_UMLModelElement(),
                                    XmiAux::mk_UMLNamespace(),
                                    TYPE_XMI_UMLGeneralizableElement().Init(Map(), Sequence()),
                                    Nil());

  else if ((nm == L"UML:TaggedValue") ||
           (nm == L"UML:TaggedValue.modelElement"))
    return TYPE_XMI_UMLTaggedValue().Init(attrs, XmiAux::mk_UMLModelElement());
  else if ((nm == L"UML:Stereotype") ||
           (nm == L"UML:Stereotype.baseClass") ||
           (nm == L"UML:Stereotype.extendedElement"))
    return TYPE_XMI_UMLStereotype().Init(attrs, Sequence(), Sequence());
  else if ((nm == L"UML:Namespace") ||
           (nm == L"UML:Namespace.ownedElement"))
    return XmiAux::mk_UMLNamespace(attrs);
  else if ((nm == L"UML:ModelElement") ||
           (nm == L"UML:ModelElement.taggedValue") ||
           (nm == L"UML:ModelElement.namespace") ||
           (nm == L"UML:ModelElement.constraint") ||
           (nm == L"UML:ModelElement.stereotype"))
    return XmiAux::mk_UMLModelElement(attrs, Map(), TAG_XMI_public);
  else if (nm == L"UML:ModelElement.visibility") 
  {
    TYPE_XMI_UMLModelElement res (XmiAux::mk_UMLModelElement());
    if (attrs.DomExists(Sequence(L"xmi.value")))
      res.set_visibility(attrs[Sequence(L"xmi.value")]);
    return res;
  }
  else if (nm == L"UML:ModelElement.definition") 
  {
    TYPE_XMI_UMLModelElement res (XmiAux::mk_UMLModelElement());
    if (attrs.DomExists(Sequence(L"xmi.value")))
      res.set_definition(attrs[Sequence(L"xmi.value")]);
    return res;
  }
  else if ((nm == L"UML:Classifier") ||
           (nm == L"UML:Classifier.feature"))
    return TYPE_XMI_UMLClassifier().Init(attrs, Sequence());
  else if (nm == L"UML:Primitive")
    return TYPE_XMI_UMLPrimitive().Init(attrs, Sequence());
  else if ((nm == L"UML:Parameter") ||
           (nm == L"UML:Parameter.behavioralFeature") ||
           (nm == L"UML:Parameter.type") ||
           (nm == L"UML:Parameter.defaultValue"))
    return TYPE_XMI_UMLParameter().Init(attrs,
                                        XmiAux::mk_UMLModelElement(),
                                        Nil(), Nil(), Nil());
  else if ((nm == L"UML:BehavioralFeature") ||
           (nm == L"UML:BehavioralFeature.parameter"))
    return TYPE_XMI_UMLBehavioralFeature().Init(attrs, Sequence());
  else if ((nm == L"UML:Feature") ||
           (nm == L"UML:Feature.owner"))
    return XmiAux::mk_UMLFeature(attrs, Nil());
  else if (nm == L"UML:Feature.visibility")
  {
    TYPE_XMI_UMLFeature res (XmiAux::mk_UMLFeature());
    if (attrs.DomExists(Sequence(L"xmi.value")))
      res.set_visibility(attrs[Sequence(L"xmi.value")]);
    return res;
  }
  else if (nm == L"UML:Operation")
    return TYPE_XMI_UMLOperation().Init(attrs,
                                        XmiAux::mk_UMLModelElement(),
                                        XmiAux::mk_UMLFeature(),
                                        TYPE_XMI_UMLBehavioralFeature().Init(Map(), Sequence()));
                                     
  else if  ((nm == L"UML:Generalization") ||
            (nm == L"UML:Generalization.child") ||
            (nm == L"UML:Generalization.parent"))
  { 
    TYPE_XMI_UMLGeneralization res;
    res.Init(attrs, XmiAux::mk_UMLModelElement(), Nil(), Nil(), Nil());
    if (nm == L"UML:Generalization.child")
      res.set_flag(Bool(true));
    else if (nm == L"UML:Generalization.parent")
      res.set_flag(Bool(false));
    return res;
  }
  else if ((nm == L"UML:GeneralizableElement") ||
           (nm == L"UML:GeneralizableElement.generalization"))
    return TYPE_XMI_UMLGeneralizableElement().Init(attrs, Sequence());
  else if ((nm == L"UML:Association") ||
           (nm == L"UML:Association.connection"))
    return TYPE_XMI_UMLAssociation().Init(attrs,
                                          XmiAux::mk_UMLModelElement(),
                                          Sequence());
  else if ((nm == L"UML:AssociationEnd") ||
           (nm == L"UML:AssociationEnd.association") ||
           (nm == L"UML:AssociationEnd.participant"))
    return XmiAux::mk_UMLAssociationEnd(attrs);
  else if (nm == L"UML:AssociationEnd.initialValue") 
  {
    return TYPE_XMI_UMLExpression().Init(attrs, Nil());
  }
  else if (nm == L"UML:AssociationEnd.visibility") 
  {
    TYPE_XMI_UMLAssociationEnd res (XmiAux::mk_UMLAssociationEnd());
    if (attrs.DomExists(Sequence(L"xmi.value")))
      res.set_visibility(attrs[Sequence(L"xmi.value")]);
    return res;
  }
  else if (nm == L"UML:AssociationEnd.qualifier") 
  {
    return XmiAux::mk_UMLAttribute(attrs);
  }
  else if ((nm == L"UML:StructuralFeature") ||
           (nm == L"UML:StructuralFeature.type") ||
           (nm == L"UML:StructuralFeature.multiplicity"))
    return XmiAux::mk_UMLStructuralFeature(attrs);
  else if ((nm == L"UML:Multiplicity") ||
           (nm == L"UML:Multiplicity.range"))
    return TYPE_XMI_UMLMultiplicity().Init(attrs, Nil());
  else if (nm == L"UML:MultiplicityRange")
    return TYPE_XMI_UMLMultiplicityRange().Init(attrs);
  else if ((nm == L"UML:Attribute") ||
           (nm == L"UML:Attribute.initialValue"))
  {
//    return TYPE_XMI_UMLAttribute().Init(attrs,
//                                        XmiAux::mk_UMLModelElement(),
//                                        XmiAux::mk_UMLFeature(),
//                                        XmiAux::mk_UMLStructuralFeature(),
//                                        Nil(), Nil());
    return XmiAux::mk_UMLAttribute(attrs);
  }
  else if (nm == L"UML:Attribute.associationEnd")
//    return TYPE_XMI_UMLAssociationEnd().Init(Map(),
//                                             XmiAux::mk_UMLModelElement(),
//                                             XmiAux::mk_UMLFeature(),
//                                             XmiAux::mk_UMLStructuralFeature(),
//                                             Nil(), Nil(), Nil(), Nil());
    return XmiAux::mk_UMLAssociationEnd();
  else if ((nm == L"UML:Constraint") ||
           (nm == L"UML:Constraint.body") ||
           (nm == L"UML:Constraint.constrainedElement"))
    return TYPE_XMI_UMLConstraint().Init(attrs, XmiAux::mk_UMLModelElement(), Nil(), Nil());
  else if ((nm == L"UML:Expression") ||
           (nm == L"UML:Expression.body"))
    return TYPE_XMI_UMLExpression().Init(attrs, Nil());
  else if (nm == L"UML:BooleanExpression")
    return TYPE_XMI_UMLBooleanExpression().Init(attrs, Nil());
  else if (nm == L"UML:DataType")
    return TYPE_XMI_UMLDataType().Init(attrs);
  else if ((nm == L"UML:Diagram") ||
           (nm == L"UML:Diagram.element"))
    return TYPE_XMI_UMLDiagram().Init(attrs, XmiAux::mk_UMLModelElement(), Sequence());
  else if (nm == L"UML:DiagramElement")
    return TYPE_XMI_UMLDiagramElement().Init(attrs);
  else if (nm == L"EAStub") {
    return TYPE_XMI_EAStub().Init(attrs);
  }
  else
  {
    Sequence n (nm);
    if (n.SubSequence(1, 5) == Sequence(L"JUDE:"))
      yet_astah.Insert(n);
    else
      yet.Insert(n);
  }
  return Record();
}

Generic XmiReader::ReadElement(xmlTextReaderPtr reader)
{ 
  Map attrs (GetAttribute(reader));
  Record e (CreateNewElement(reader));

  if (xmlTextReaderIsEmptyElement(reader) != 1)
  {
    int depth = xmlTextReaderDepth(reader);
    Generic element = ReadDocument(reader);
    while(depth + 1 == xmlTextReaderDepth(reader)) 
    {
      e = SetElement(e, element);
      element = ReadDocument(reader);
    }
  }

  if (attrs.DomExists(Sequence(L"xmi.id"))) {
    if (Record() != e) {
      this->idm.ImpModify(attrs[Sequence(L"xmi.id")], e);
    }
  }

  return e;
}

Generic XmiReader::ReadText(xmlTextReaderPtr reader)
{
  const xmlChar * value = xmlTextReaderConstValue(reader);
  if (value != NULL)
    return Sequence(TBWSTR::utf8str2wstring((const char *)value));
  else
    return Nil();
}

Generic XmiReader::ReadDocument(xmlTextReaderPtr reader)
{
  int ret = ReadNextLine(reader);
  while (ret == 1)
  {
    string name ((const char *)xmlTextReaderConstName(reader));
    switch (xmlTextReaderNodeType(reader))
    {
      case XML_READER_TYPE_ELEMENT: {
        return ReadElement(reader);
        break;
      }
      case XML_READER_TYPE_END_ELEMENT: {
        return Nil();
	break;
      }
      case XML_READER_TYPE_TEXT: {
        return ReadText(reader);
        break;
      }
      case XML_READER_TYPE_SIGNIFICANT_WHITESPACE: {
        break;
      }
      case XML_READER_TYPE_COMMENT: {
        break;
      }

      case XML_READER_TYPE_NONE:
      case XML_READER_TYPE_ATTRIBUTE:
      case XML_READER_TYPE_CDATA:
      case XML_READER_TYPE_ENTITY_REFERENCE:
      case XML_READER_TYPE_ENTITY:
      case XML_READER_TYPE_PROCESSING_INSTRUCTION:
      case XML_READER_TYPE_DOCUMENT:
      case XML_READER_TYPE_DOCUMENT_TYPE:
      case XML_READER_TYPE_DOCUMENT_FRAGMENT:
      case XML_READER_TYPE_NOTATION:
      case XML_READER_TYPE_WHITESPACE:
      case XML_READER_TYPE_END_ENTITY:
      case XML_READER_TYPE_XML_DECLARATION:
      default: {
        vdm_log << L"Unsupported: " << TypeNoToStr(xmlTextReaderNodeType(reader)) << endl;
        break;
      }
    }
    ret = ReadNextLine(reader);
  }
  return Nil();
}

void XmiReader::DumpNodeInfo(xmlTextReaderPtr reader)
{
  const xmlChar * name = xmlTextReaderConstName(reader);
  if (name == NULL)
    name = BAD_CAST "--";

  vdm_log << xmlTextReaderDepth(reader) << L" "
          << TypeNoToStr(xmlTextReaderNodeType(reader)) << L" "
          << TBWSTR::string2wstring((const char *)name) << L" "
          << xmlTextReaderAttributeCount(reader) << L" "
          << xmlTextReaderIsEmptyElement(reader) << L" "
          << xmlTextReaderHasValue(reader);

  const xmlChar * value = xmlTextReaderConstValue(reader);

  if (value == NULL)
    vdm_log << endl;
  else {
//    if (xmlStrlen(value) > 40)
//        printf(" %.40s...\n", value);
//    else
//      printf(" %s\n", value);
    vdm_log << L" " << TBWSTR::string2wstring((const char *)value) << endl;
  }
}

wstring XmiReader::TypeNoToStr(int i)
{
  switch(i) {
    case XML_READER_TYPE_NONE:
      return L"NONE";
    case XML_READER_TYPE_ELEMENT:
      return L"ELEMENT";
    case XML_READER_TYPE_ATTRIBUTE:
      return L"ATTRIBUTE";
    case XML_READER_TYPE_TEXT:
      return L"TEXT";
    case XML_READER_TYPE_CDATA:
      return L"CDATA";
    case XML_READER_TYPE_ENTITY_REFERENCE:
      return L"REFERENCE";
    case XML_READER_TYPE_ENTITY:
      return L"ENTITY";
    case XML_READER_TYPE_PROCESSING_INSTRUCTION:
      return L"INSTRUCTION";
    case XML_READER_TYPE_COMMENT:
      return L"COMMENT";
    case XML_READER_TYPE_DOCUMENT:
      return L"DOCUMENT";
    case XML_READER_TYPE_DOCUMENT_TYPE:
      return L"DOCUMENT_TYPE";
    case XML_READER_TYPE_DOCUMENT_FRAGMENT:
      return L"DOCUMENT_FRAGMENT";
    case XML_READER_TYPE_NOTATION:
      return L"NOTATION";
    case XML_READER_TYPE_WHITESPACE:
      return L"WHITESPACE";
    case XML_READER_TYPE_SIGNIFICANT_WHITESPACE:
      return L"SIGNIFICANT_WHITESPACE";
    case XML_READER_TYPE_END_ELEMENT:
      return L"END_ELEMENT";
    case XML_READER_TYPE_END_ENTITY:
      return L"END_ENTITY";
    case XML_READER_TYPE_XML_DECLARATION:
      return L"XML_DECLARATION";
    default:
      return L"UNDEF";
  }
}

Map XmiReader::GetAttribute(xmlTextReaderPtr reader)
{
  Map res;
  int ac = xmlTextReaderAttributeCount(reader);
  if (ac > 0)
  {
    for (int i = 0; i < ac; i++)
    {
      xmlTextReaderMoveToAttributeNo(reader, i);
      const xmlChar * anm = xmlTextReaderName(reader);
      if (anm != NULL)
      {
        xmlTextReaderMoveToElement(reader);
        const xmlChar * avl = xmlTextReaderGetAttributeNo(reader, i);
        if (avl != NULL)
        {
          string anms ((const char *)anm);
          string avls ((const char *)avl);
          res.ImpModify(Sequence(TBWSTR::utf8str2wstring(anms)),
                        Sequence(TBWSTR::utf8str2wstring(xmistr2str(avls, IsShiftJIS(reader)))));
        }
      }
    }
  }
  return res;
}

string XmiReader::xmistr2str(const string & str, bool sjis)
{
  string res (str);
  if (IsASTAH())
    res = astahstr2str(str);

  if (sjis && !res.empty())
  {
    // for SHIFT-JIS 
    int len = res.length();
    int index = 0;
    string ns;
    while(index < len)
    {
      char c (res[index]);
      switch (c) {
        case '\xc2': {
          if (index < len - 1) 
          {
            if ('\xa5' == res[index + 1])
            {
              // 0xc2, 0xa5 : yen sign
              ns += '\x5c'; // back slash
              index += 1;
              break;
            }
          }
          ns += c;
          break;
        }
        case '\xe2': {
          if (index < len - 2) 
          {
            if (('\x80' == res[index + 1]) && ('\xbe' == res[index + 2]))
            {
              // 0xe2, 0x80, 0xbe : overline
              ns += '\x7e'; // tilde
              index += 2;
              break;
            }
          }
          ns += c;
          break;
        }
        default: {
          ns += c;
          break;
        }
      }
      index++;
    } 
    res = ns;
  }
  return res;
}

string XmiReader::astahstr2str(const string & str)
{
  string res;
  int str_len = (int)str.size();
  int index = 0;
  while (index < str_len) {
    unsigned char c = str[index];
    switch(c) {
      case '+': {
        res += ' ';
        break;
      }
      case '%': {
        if (index < str_len - 2)
        {
          unsigned int x = hexchar2int(str[index + 1]) * 16 +
                           hexchar2int(str[index + 2]);
          res += x;
          index += 2; 
        }
        else
        {
          // err
          res += ' ';
        }
        break;
      }
      default: {
        res += c;
        break;
      }
    }
    index++;
  }
  return res;
}

int XmiReader::hexchar2int(const char c)
{
  if (('0' <= c) && ('9' >= c))
    return c - '0';

  if (('a' <= c) && ('f' >= c))
    return c - 'a' + 10;

  if (('A' <= c) && ('F' >= c))
    return c - 'A' + 10;

// error
  return 0;
}

bool XmiReader::IsShiftJIS(xmlTextReaderPtr reader)
{
  const xmlChar * e = xmlTextReaderConstEncoding(reader);
  if (e != NULL)
  {
    string estr ((const char *)e);
    return (estr == "SHIFT_JIS");
  }
  return false;
}

wstring XmiReader::GetEncoding(xmlTextReaderPtr reader)
{
  wstring encoding;
  const xmlChar * e = xmlTextReaderConstEncoding(reader);
  if (e != NULL)
  {
    string estr ((const char *)e);
    encoding = TBWSTR::string2wstring(estr);
  }
  return encoding;
}

void XmiReader::ReportCanNotSet(const Record & e, const Generic & element)
{
  this->not_set.Insert(mk_(e, element));
}

void XmiReader::CheckExporter(const Sequence & ep)
{
// Jomt XMI writer
// Enterprise Architect
  if (ep.SubSequence(1, 15) == Sequence(L"Jomt XMI writer"))
  {
    this->SetMode(MODE_ASTAH);
  }
  else if (ep == Sequence(L"Enterprise Architect"))
  {
    this->SetMode(MODE_EnterpriseArchitect);
  }
}

void XmiReader::CheckExporterVersion(const Sequence & ep)
{
}

