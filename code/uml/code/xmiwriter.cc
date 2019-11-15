#include "xmiwriter.h"
#include "tb_wstring.h"
#include "XMI.h"
#include "xmiaux.h"
#include <fstream>
#include <time.h>

XmiWriter::XmiWriter(int m)
{
  this->mode = m;
}

XmiWriter::~XmiWriter()
{
}

bool XmiWriter::IsASTAH()
{
  return (this->mode == MODE_ASTAH);
}

bool XmiWriter::IsEA()
{
  return (this->mode == MODE_EnterpriseArchitect);
}

bool XmiWriter::SaveAs(const std::wstring & fname, const TYPE_XMI_Document & doc)
{
  std::ofstream out;
  out.open(TBWSTR::wstring2fsstr(fname).c_str(), ios::out);
  if (out.good()) {
    WriteFile(out, doc);
    out.close();
    return true;
  }
  else {
    return false;
  }
}

void XmiWriter::WriteFile(ofstream & o, const TYPE_XMI_Document & doc)
{
  o << "<?xml version = \"1.0\" encoding = \"UTF-8\" standalone = \"yes\"?>" << endl;
  WriteElement(o, 0, doc);
}

void XmiWriter::WriteElementSeq(ofstream & o, int indent, const Sequence & elm_l)
{
  size_t len_elm_l = elm_l.Length();
  for (size_t idx = 1; idx <= len_elm_l; idx++)
    WriteElement(o, indent, elm_l[idx]);
}

void XmiWriter::WriteElement(ofstream & o, int indent, const Record & elm)
{
  switch(elm.GetTag()) {
    case TAG_TYPE_XMI_Document: {
      WriteXMIDocument(o, indent, elm);
      break;
    }
    case TAG_TYPE_XMI_Header: {
      WriteXMIHeader(o, indent, elm);
      break;
    } 
    case TAG_TYPE_XMI_Documentation: {
      WriteXMIDocumentation(o, indent, elm);
      break;
    }
    case TAG_TYPE_XMI_Exporter: {
      WriteXMIExporter(o, indent, elm);
      break;
    }
    case TAG_TYPE_XMI_ExporterVersion: {
      WriteXMIExporterVersion(o, indent, elm);
      break;
    }
    case TAG_TYPE_XMI_CurrentModelVersion: {
      WriteXMICurrentModelVersion(o, indent, elm);
      break;
    }
    case TAG_TYPE_XMI_MaxModelVersion: {
      WriteXMIMaxModelVersion(o, indent, elm);
      break;
    }
    case TAG_TYPE_XMI_CurrentModelProducer: {
      WriteXMICurrentModelProducer(o, indent, elm);
      break;
    }
    case TAG_TYPE_XMI_SortedVersionHistories: {
      WriteXMISortedVersionHistories(o, indent, elm);
      break;
    }
    case TAG_TYPE_XMI_VersionEntry: {
      WriteXMIVersionEntry(o, indent, elm);
      break;
    }
    case TAG_TYPE_XMI_Import: {
      WriteXMIImport(o, indent, elm);
      break;
    }
    case TAG_TYPE_XMI_Metametamodel: {
      WriteXMIMetametamodel(o, indent, elm);
      break;
    }
    case TAG_TYPE_XMI_Metamodel: {
      WriteXMIMetamodel(o, indent, elm);
      break;
    }
    case TAG_TYPE_XMI_Model: {
      WriteXMIModel(o, indent, elm);
      break;
    }
    case TAG_TYPE_XMI_Content: {
      WriteXMIContent(o, indent, elm);
      break;
    }
    case TAG_TYPE_XMI_UMLModelElement: {
      WriteUMLModelElement(o, indent, elm);
      break;
    }
    case TAG_TYPE_XMI_UMLNamespace: {
      WriteUMLNamespace(o, indent, elm);
      break;
    }
    case TAG_TYPE_XMI_UMLFeature: {
      WriteUMLFeature(o, indent, elm);
      break;
    }
    case TAG_TYPE_XMI_UMLModel: {
      WriteUMLModel(o, indent, elm);
      break;
    }
    case TAG_TYPE_XMI_UMLPackage: {
      WriteUMLPackage(o, indent, elm);
      break;
    }
    case TAG_TYPE_XMI_UMLInterface: {
      WriteUMLInterface(o, indent, elm);
      break;
    }
    case TAG_TYPE_XMI_UMLClass: {
      WriteUMLClass(o, indent, elm); 
      break;
    }
    case TAG_TYPE_XMI_UMLAssociation: {
      WriteUMLAssociation(o, indent, elm);
      break;
    }
    case TAG_TYPE_XMI_UMLAssociationEnd: {
      WriteUMLAssociationEnd(o, indent, elm);
      break;
    }
    case TAG_TYPE_XMI_UMLGeneralization: {
      WriteUMLGeneralization(o, indent, elm);
      break;
    }
    case TAG_TYPE_XMI_UMLStereotype: {
      WriteUMLStereotype(o, indent, elm);
      break;
    }
    case TAG_TYPE_XMI_UMLPrimitive: {
      WriteUMLPrimitive(o, indent, elm);
      break;
    }
    case TAG_TYPE_XMI_UMLClassifier: {
      WriteUMLClassifier(o, indent, elm);
      break;
    }
    case TAG_TYPE_XMI_UMLAttribute: {
      WriteUMLAttribute(o, indent, elm);
      break;
    }
    case TAG_TYPE_XMI_UMLStructuralFeature: {
      WriteUMLStructuralFeature(o, indent, elm);
      break;
    }
    case TAG_TYPE_XMI_UMLExpression: {
      WriteUMLExpression(o, indent, elm);
      break;
    }
    case TAG_TYPE_XMI_UMLBooleanExpression: {
      WriteUMLBooleanExpression(o, indent, elm);
      break;
    }
    case TAG_TYPE_XMI_UMLOperation: {
      WriteUMLOperation(o, indent, elm);
      break;
    }
    case TAG_TYPE_XMI_UMLBehavioralFeature: {
      WriteUMLBehavioralFeature(o, indent, elm);
      break;
    }
    case TAG_TYPE_XMI_UMLParameter: {
      WriteUMLParameter(o, indent, elm);
      break;
    }
    case TAG_TYPE_XMI_UMLGeneralizableElement: {
      WriteUMLGeneralizableElement(o, indent, elm);
      break;
    }
    case TAG_TYPE_XMI_UMLMultiplicity: {
      WriteUMLMultiplicity(o, indent, elm);
      break;
    }
    case TAG_TYPE_XMI_UMLMultiplicityRange: {
      WriteUMLMultiplicityRange(o, indent, elm);
      break;
    }
    case TAG_TYPE_XMI_UMLTaggedValue: {
      WriteUMLTaggedValue(o, indent, elm);
      break;
    }
    case TAG_TYPE_XMI_UMLConstraint: {
      WriteUMLConstraint(o, indent, elm);
      break;
    }
    default: {
wcout << L"error: tag unknown: " << elm << endl;
    }
  }
}

void XmiWriter::WriteXMIDocument(ofstream & o, int indent,
                                 const TYPE_XMI_Document & elm)
{
  Map attrs (elm.GetMap(pos_XMI_Document_attrs));

  o << IND(indent) << "<XMI"
    << attributeStr(attrs, Sequence(L"xmi.version"))
    << attributeStr(attrs, Sequence(L"xmlns:UML"))
    << ">" << endl;
      
  WriteElement(o, indent + 1, elm.GetRecord(pos_XMI_Document_header));
  WriteElement(o, indent + 1, elm.GetRecord(pos_XMI_Document_content));
     
  o << IND(indent) << "</XMI>" << endl << flush;
}

void XmiWriter::WriteXMIHeader(ofstream & o, int indent,
                               const TYPE_XMI_Header & elm)
{
  Generic documentation (elm.GetField(pos_XMI_Header_documentation));
  Generic import        (elm.GetField(pos_XMI_Header_import));
  Generic metametamodel (elm.GetField(pos_XMI_Header_metametamodel));
  Generic metamodel     (elm.GetField(pos_XMI_Header_metamodel));
  Generic model         (elm.GetField(pos_XMI_Header_model));

  o << IND(indent) << "<XMI.header>" << endl;

  if (!documentation.IsNil()) {
    WriteElement(o, indent + 1, documentation);
  }
  if (!metamodel.IsNil()) {
    WriteElement(o, indent + 1, metamodel);
  }
  o << IND(indent) << "</XMI.header>" << endl << flush;
}

void XmiWriter::WriteXMIDocumentation(ofstream & o, int indent,
                                      const TYPE_XMI_Documentation & elm)
{
  Generic exporter               (elm.GetField(post_XMI_Documentation_exporter)); 
  Generic exporterVersion        (elm.GetField(post_XMI_Documentation_exporterVersion)); 
  Generic currentModelVersion    (elm.GetField(post_XMI_Documentation_currentModelVersion)); 
  Generic maxModelVersion        (elm.GetField(post_XMI_Documentation_maxModelVersion)); 
  Generic currentModelProducer   (elm.GetField(post_XMI_Documentation_currentModelProducer)); 
  Generic sortedVersionHistories (elm.GetField(post_XMI_Documentation_storedVersionHistories)); 

  o << IND(indent) << "<XMI.documentation>" << endl;

  if (!exporter.IsNil()) {
    WriteElement(o, indent + 1, exporter);
  }
  if (!exporterVersion.IsNil()) {
    WriteElement(o, indent + 1, exporterVersion);
  }
  o << IND(indent) << "</XMI.documentation>" << endl << flush;
}

void XmiWriter::WriteXMIExporter(ofstream & o, int indent,
                                 const TYPE_XMI_Exporter & elm)
{
  Generic text (elm.GetField(pos_XMI_Exporter_text));

  o << IND(indent) << "<XMI.exporter>";
  
  if (!text.IsNil()) {
    o << seq2mbstr(text);
  }
  o << "</XMI.exporter>" << endl << flush;
}

void XmiWriter::WriteXMIExporterVersion(ofstream & o, int indent,
                                        const TYPE_XMI_ExporterVersion & elm)
{
  Generic text (elm.GetField(pos_XMI_ExporterVersion_text));

  o << IND(indent) << "<XMI.exporterVersion>";
  
  if (!text.IsNil()) {
    o << seq2mbstr(text);
  }
  o << "</XMI.exporterVersion>" << endl << flush;
}

void XmiWriter::WriteXMICurrentModelVersion(ofstream & o, int indent,
                                            const TYPE_XMI_CurrentModelVersion & elm)
{
  Generic text (elm.GetField(pos_XMI_CurrentModelVersion_text));

  o << IND(indent) << "<XMI.currentModelVersion>";
  
  if (!text.IsNil()) {
    o << seq2mbstr(text);
  }
  o << "</XMI.currentModelVersion>" << endl << flush;
}

void XmiWriter::WriteXMIMaxModelVersion(ofstream & o, int indent,
                                        const TYPE_XMI_MaxModelVersion & elm)
{
  Generic text (elm.GetField(pos_XMI_MaxModelVersion_text));

  o << IND(indent) << "<XMI.maxModelVersion>";
  
  if (!text.IsNil()) {
    o << seq2mbstr(text);
  }
  o << "</XMI.maxModelVersion>" << endl << flush;
}

void XmiWriter::WriteXMICurrentModelProducer(ofstream & o, int indent,
                                             const TYPE_XMI_CurrentModelProducer & elm)
{
  Generic text (elm.GetField(pos_XMI_currentModelProducer_text));

  o << IND(indent) << "<XMI.currentModelProducer>";
      
  if (!text.IsNil()) {
    o << seq2mbstr(text);
  }
  o << "</XMI.currentModelProducer>" << endl << flush;
}

void XmiWriter::WriteXMISortedVersionHistories(ofstream & o, int indent,
                                               const TYPE_XMI_SortedVersionHistories & elm)
{
  Sequence versionEntry (elm.GetSequence(pos_XMI_SortedVersionHistories_versionEntry));

  o << IND(indent) << "<XMI.sortedVersionHistories>" << endl;
  
  WriteElementSeq(o, indent + 1, versionEntry);
    
  o << IND(indent) << "</XMI.sortedVersionHistories>" << endl << flush;
}

void XmiWriter::WriteXMIVersionEntry(ofstream & o, int indent,
                                     const TYPE_XMI_VersionEntry & elm)
{
  Map attrs (elm.GetMap(pos_XMI_VersionEntry_attrs));
  
  o << IND(indent) << "<XMI.versionEntry";
  o << attributeStr(attrs, Sequence(L"productVersion"));
  o << attributeStr(attrs, Sequence(L"modelVersion"));
  o << "/>" << endl << flush;
}

void XmiWriter::WriteXMIImport(ofstream & o, int indent,
                               const TYPE_XMI_Import & elm)
{
}

void XmiWriter::WriteXMIMetametamodel(ofstream & o, int indent,
                                      const TYPE_XMI_Metametamodel & elm)
{
  Map attrs (elm.GetMap(pos_XMI_Metametamodel_attrs));

  o << IND(indent) << "<XMI.metametamodel";
  o << attributeStr(attrs, Sequence(L"xmi.name"));
  o << attributeStr(attrs, Sequence(L"xmi.version"));
  o << "/>" << endl << flush;
}

void XmiWriter::WriteXMIMetamodel(ofstream & o, int indent,
                                  const TYPE_XMI_Metamodel & elm)
{
  Map attrs (elm.GetMap(pos_XMI_Metamodel_attrs));

  o << IND(indent) << "<XMI.metamodel";
  o << attributeStr(attrs, Sequence(L"xmi.name"));
  o << attributeStr(attrs, Sequence(L"xmi.version"));
  o << "/>" << endl << flush;
}

void XmiWriter::WriteXMIModel(ofstream & o, int indent,
                              const TYPE_XMI_Model & elm)
{
  Map attrs (elm.GetMap(pos_XMI_Model_attrs));

  o << IND(indent) << "<XMI.model";
  o << attributeStr(attrs, Sequence(L"xmi.name"));
  o << attributeStr(attrs, Sequence(L"xmi.version"));
  o << "/>" << endl << flush;
}

void XmiWriter::WriteXMIContent(ofstream & o, int indent,
                                const TYPE_XMI_Content & elm)
{
  Generic model       (elm.GetField(pos_XMI_Content_model));
  Sequence primitives (elm.GetSequence(pos_XMI_Content_primitives));
  Sequence extensions (elm.GetSequence(pos_XMI_Content_extensions));

  o << IND(indent) << "<XMI.content>" << endl;

  if (!model.IsNil())
    WriteElement(o, indent + 1, model);

  WriteElementSeq(o, indent + 1, primitives);
  WriteElementSeq(o, indent + 1, extensions);

  o << IND(indent) << "</XMI.content>" << endl << flush;
}

void XmiWriter::WriteUMLModelElement(ofstream & o, int indent,
                                     const TYPE_XMI_UMLModelElement & elm)
{
  Map attrs (elm.GetMap(pos_XMI_UMLModelElement_attrs));
  if (attrs.DomExists(TAG_XMI_idref)) {
    o << IND(indent) << "<UML:ModelElement";
    o << attributeStr(attrs, TAG_XMI_idref);
    o << "/>" << endl;
  }
  else {
    Sequence nspace (elm.GetSequence(pos_XMI_UMLModelElement_namespace));
    Generic visibility (elm.GetField(pos_XMI_UMLModelElement_visibility));
    Generic definition (elm.GetField(pos_XMI_UMLModelElement_definition));
    Sequence stereotype (elm.GetSequence(pos_XMI_UMLModelElement_stereotype));
    Sequence taggedValue (elm.GetSequence(pos_XMI_UMLModelElement_taggedValue));
    Sequence constraint (elm.GetSequence(pos_XMI_UMLModelElement_constraint));
        
    if (!nspace.IsEmpty()) {
      o << IND(indent) << "<UML:ModelElement.namespace>" << endl << flush;
      WriteElementSeq(o, indent + 1, nspace);
      o << IND(indent) << "</UML:ModelElement.namespace>" << endl << flush;
    }
    if (!visibility.IsNil()) {
      o << IND(indent) << "<UML:ModelElement.visibility";
      o << " xmi.value=\"" << seq2mbstr(visibility);
      o << "\"/>" << endl;
    }
    if (!definition.IsNil()) {
      o << IND(indent) << "<UML:ModelElement.definition";
      o << " xmi.value=\"" << str2xmistr(seq2mbstr(definition));
      o << "\"/>" << endl;
    }
    if (!taggedValue.IsEmpty()) {
      o << IND(indent) << "<UML:ModelElement.taggedValue>" << endl << flush;
      WriteElementSeq(o, indent + 1, taggedValue);
      o << IND(indent) << "</UML:ModelElement.taggedValue>" << endl << flush;
    }
    if (!constraint.IsEmpty()) {
      o << IND(indent) << "<UML:ModelElement.constraint>" << endl << flush;
      WriteElementSeq(o, indent + 1, constraint);
      o << IND(indent) << "</UML:ModelElement.constraint>" << endl << flush;
    }
    if (!stereotype.IsEmpty()) {
      o << IND(indent) << "<UML:ModelElement.stereotype>" << endl << flush;
      WriteElementSeq(o, indent + 1, stereotype);
      o << IND(indent) << "</UML:ModelElement.stereotype>" << endl << flush;
    }
  }
}

void XmiWriter::WriteUMLNamespace(ofstream & o, int indent,
                                  const TYPE_XMI_UMLNamespace & elm)
{
  Map attrs (elm.GetMap(pos_XMI_UMLNamespace_attrs));
  if (attrs.DomExists(TAG_XMI_idref)) {
    o << IND(indent) << "<UML:Namespace";
    o << attributeStr(attrs, TAG_XMI_idref);
    o << "/>" << endl;
  }
  else {
    Sequence ownedElement (elm.GetSequence(pos_XMI_UMLNamespace_ownedElement));
  
    if (!ownedElement.IsEmpty()) {
      o << IND(indent) << "<UML:Namespace.ownedElement>" << endl << flush;
      WriteElementSeq(o, indent + 1, ownedElement);
      o << IND(indent) << "</UML:Namespace.ownedElement>" << endl << flush;
    }
  }
}

void XmiWriter::WriteUMLFeature(ofstream & o, int indent,
                                const TYPE_XMI_UMLFeature & elm)
{
  Generic owner (elm.GetField(pos_XMI_UMLFeature_owner));
  Generic visibility (elm.GetField(pos_XMI_UMLFeature_visibility));
  if (!owner.IsNil()) {
    o << IND(indent) << "<UML:Feature.owner>" << endl << flush;
    WriteElement(o, indent + 1, owner);
    o << IND(indent) << "</UML:Feature.owner>" << endl << flush;
  }
  if (!visibility.IsNil()) {
    o << IND(indent) << "<UML:Feature.visibility";
    o << " xmi.value=\"" << seq2mbstr(visibility);
    o << "\"/>" << endl;
  }
}

void XmiWriter::WriteUMLModel(ofstream & o, int indent,
                              const TYPE_XMI_UMLModel & elm)
{
  Map attrs (elm.GetMap(pos_XMI_UMLModel_attrs));
  TYPE_XMI_UMLModelElement me (elm.GetRecord(pos_XMI_UMLModel_modelElement));
  TYPE_XMI_UMLNamespace ns (elm.GetRecord(pos_XMI_UMLModel_namespace));

  o << IND(indent) << "<UML:Model";
  o << attributeStr(attrs, TAG_XMI_id);
  o << attributeStr(attrs, TAG_XMI_name);
  o << attributeStr(attrs, TAG_XMI_version);
  o << attributeStr(attrs, TAG_XMI_unSolvedFlag);
  o << attributeStr(attrs, TAG_XMI_isRoot);
  o << attributeStr(attrs, TAG_XMI_isLeaf);
  o << attributeStr(attrs, TAG_XMI_isAbstract);
  o << attributeStr(attrs, Sequence(L"xmlns:UML"));
  o << ">" << endl;

  WriteElement(o, indent + 1, me);
  WriteElement(o, indent + 1, ns);
      
  o << IND(indent) << "</UML:Model>" << endl << flush;
}

void XmiWriter::WriteUMLPackage(ofstream & o, int indent,
                                const TYPE_XMI_UMLPackage & elm)
{
}

void XmiWriter::WriteUMLInterface(ofstream & o, int indent,
                                  const TYPE_XMI_UMLInterface & elm)
{
}

void XmiWriter::WriteUMLClass(ofstream & o, int indent,
                              const TYPE_XMI_UMLClass & elm)
{
  Map attrs (elm.GetMap(pos_XMI_UMLClass_attrs));
  TYPE_XMI_UMLModelElement me (elm.GetRecord(pos_XMI_UMLClass_modelElement));
  TYPE_XMI_UMLNamespace ns (elm.GetField(pos_XMI_UMLClass_namespace));
  TYPE_XMI_UMLGeneralizableElement ge (elm.GetField(pos_XMI_UMLClass_generalizableElement));
  Generic cf (elm.GetField(pos_XMI_UMLClass_classifier));

  o << IND(indent) << "<UML:Class";
  o << attributeStr(attrs, TAG_XMI_id);
  o << attributeStr(attrs, TAG_XMI_name);
  o << attributeStr(attrs, TAG_XMI_version);
  o << attributeStr(attrs, TAG_XMI_unSolvedFlag);
  o << attributeStr(attrs, TAG_XMI_isRoot);
  o << attributeStr(attrs, TAG_XMI_isLeaf);
  o << attributeStr(attrs, TAG_XMI_isAbstract);
  o << attributeStr(attrs, TAG_XMI_isActive);
  o << ">" << endl;

  WriteElement(o, indent + 1, me);

  WriteElement(o, indent + 1, ns);

  WriteElement(o, indent + 1, ge);

  if (!cf.IsNil()) {
    WriteElement(o, indent + 1, cf);
  }
      
  o << IND(indent) << "</UML:Class>" << endl << flush;
}

void XmiWriter::WriteUMLAssociation(ofstream & o, int indent,
                                    const TYPE_XMI_UMLAssociation & elm)
{
  Map attrs (elm.GetMap(pos_XMI_UMLAssociation_attrs));
  if (attrs.DomExists(TAG_XMI_idref)) {
    o << IND(indent) << "<UML:Association";
    o << attributeStr(attrs, TAG_XMI_idref);
    o << "/>" << endl;
  }
  else {
    TYPE_XMI_UMLModelElement me (elm.GetRecord(pos_XMI_UMLAssociation_modelElement));
    Sequence co (elm.GetSequence(pos_XMI_UMLAssociation_connection));

    o << IND(indent) << "<UML:Association";
    o << attributeStr(attrs, TAG_XMI_id);
    o << attributeStr(attrs, TAG_XMI_name);
    o << attributeStr(attrs, TAG_XMI_version);
    o << attributeStr(attrs, TAG_XMI_unSolvedFlag);
    o << ">" << endl;

    WriteElement(o, indent + 1, me);

    if (!co.IsEmpty()) {
      o << IND(indent + 1) << "<UML:Association.connection>" << endl << flush;
      WriteElementSeq(o, indent + 2, co);
      o << IND(indent + 1) << "</UML:Association.connection>" << endl << flush;
    }

    o << IND(indent) << "</UML:Association>" << endl << flush;
  }
}

void XmiWriter::WriteUMLAssociationEnd(ofstream & o, int indent,
                                       const TYPE_XMI_UMLAssociationEnd & elm)
{
  Map attrs (elm.GetMap(pos_XMI_UMLAssociationEnd_attrs));
  if (attrs.DomExists(TAG_XMI_idref)) {
    o << IND(indent) << "<UML:AssociationEnd";
    o << attributeStr(attrs, TAG_XMI_idref);
    o << "/>" << endl;
  }
  else {
    TYPE_XMI_UMLModelElement me (elm.GetRecord(pos_XMI_UMLAssociationEnd_modelElement));
    TYPE_XMI_UMLFeature fe (elm.GetRecord(pos_XMI_UMLAssociationEnd_feature));
    TYPE_XMI_UMLStructuralFeature sf (elm.GetRecord(pos_XMI_UMLAssociationEnd_structuralFeature));
    Generic pa (elm.GetField(pos_XMI_UMLAssociationEnd_participant));
    Generic iv (elm.GetField(pos_XMI_UMLAssociationEnd_initialValue));
    Generic as (elm.GetField(pos_XMI_UMLAssociationEnd_association));
    Generic visibility (elm.GetField(pos_XMI_UMLAssociationEnd_visibility));
    Generic qualifier (elm.GetField(pos_XMI_UMLAssociationEnd_qualifier));
 
    if (IsEA()) {
      if (attrs.DomExists(TAG_XMI_name)) {
        Sequence nm (attrs[TAG_XMI_name]);
        if (nm.IsEmpty()) {
          attrs.RemElem(TAG_XMI_name);
        }
      }
    }

    o << IND(indent) << "<UML:AssociationEnd";
    o << attributeStr(attrs, TAG_XMI_id);
    o << attributeStr(attrs, TAG_XMI_name);
    o << attributeStr(attrs, TAG_XMI_version);
    o << attributeStr(attrs, TAG_XMI_visibility);   // EA
    o << attributeStr(attrs, TAG_XMI_multiplicity); // EA
    o << attributeStr(attrs, TAG_XMI_unSolvedFlag);
    o << attributeStr(attrs, TAG_XMI_ownerScope);
    o << attributeStr(attrs, TAG_XMI_targetScope);
    o << attributeStr(attrs, TAG_XMI_changeability);
    o << attributeStr(attrs, TAG_XMI_aggregation);
    o << attributeStr(attrs, TAG_XMI_ordering);
    o << attributeStr(attrs, TAG_XMI_navigableType);
    o << attributeStr(attrs, TAG_XMI_isNavigable);
    o << attributeStr(attrs, TAG_XMI_type); // EA
    o << ">" << endl;

    WriteElement(o, indent + 1, me);
    WriteElement(o, indent + 1, fe);
    WriteElement(o, indent + 1, sf);

    if (!pa.IsNil()) {
      o << IND(indent + 1) << "<UML:AssociationEnd.participant>" << endl << flush;
      WriteElement(o, indent + 2, pa);
      o << IND(indent + 1) << "</UML:AssociationEnd.participant>" << endl << flush;
    }

    if (!iv.IsNil()) {
      o << IND(indent + 1) << "<UML:AssociationEnd.initialValue>" << endl << flush;
      WriteElement(o, indent + 2, iv);
      o << IND(indent + 1) << "</UML:AssociationEnd.initialValue>" << endl << flush;
    }
    if (!as.IsNil()) {
      o << IND(indent + 1) << "<UML:AssociationEnd.association>" << endl << flush;
      WriteElement(o, indent + 2, as);
      o << IND(indent + 1) << "</UML:AssociationEnd.association>" << endl << flush;
    }
    if (!visibility.IsNil()) {
      o << IND(indent + 1) << "<UML:AssociationEnd.visibility";
      o << " xmi.value=\"" << seq2mbstr(visibility);
      o << "\"/>" << endl;
    }
    if (!qualifier.IsNil()) {
      o << IND(indent + 1) << "<UML:AssociationEnd.qualifier>" << endl << flush;
      WriteElement(o, indent + 2, qualifier);
      o << IND(indent + 1) << "</UML:AssociationEnd.qualifier>" << endl << flush;
    }

    o << IND(indent) << "</UML:AssociationEnd>" << endl << flush;
  }
}

void XmiWriter::WriteUMLGeneralization(ofstream & o, int indent,
                                       const TYPE_XMI_UMLGeneralization & elm)
{
  Map attrs (elm.GetMap(pos_XMI_UMLGeneralization_attrs));
  if (attrs.DomExists(TAG_XMI_idref)) {
    o << IND(indent) << "<UML:Generalization";
    o << attributeStr(attrs, TAG_XMI_idref);
    o << "/>" << endl;
  }
  else {
    o << IND(indent) << "<UML:Generalization";
    o << attributeStr(attrs, TAG_XMI_id);
    o << attributeStr(attrs, TAG_XMI_name);
    o << attributeStr(attrs, TAG_XMI_version);
    o << attributeStr(attrs, TAG_XMI_unSolvedFlag);
    o << ">" << endl;

    TYPE_XMI_UMLModelElement me (elm.GetRecord(pos_XMI_UMLGeneralization_modelElement));
    Generic child (elm.GetField(pos_XMI_UMLGeneralization_child));
    Generic parent (elm.GetField(pos_XMI_UMLGeneralization_parent));

    WriteElement(o, indent + 1, me);

    if (!child.IsNil()) {
      o << IND(indent + 1) << "<UML:Generalization.child>" << endl << flush;
      WriteElement(o, indent + 2, child);
      o << IND(indent + 1) << "</UML:Generalization.child>" << endl << flush;
    }
    if (!parent.IsNil()) {
      o << IND(indent + 1) << "<UML:Generalization.parent>" << endl << flush;
      WriteElement(o, indent + 2, parent);
      o << IND(indent + 1) << "</UML:Generalization.parent>" << endl << flush;
    }

    o << IND(indent) << "</UML:Generalization>" << endl << flush;
  }
}

void XmiWriter::WriteUMLStereotype(ofstream & o, int indent,
                                   const TYPE_XMI_UMLStereotype & elm)
{
  Map attrs (elm.GetMap(pos_XMI_UMLStereotype_attrs));
  if (attrs.DomExists(TAG_XMI_idref)) {
    o << IND(indent) << "<UML:Stereotype";
    o << attributeStr(attrs, TAG_XMI_idref);
    o << "/>" << endl;
  }
  else {
    o << IND(indent) << "<UML:Stereotype";
    o << attributeStr(attrs, TAG_XMI_id);
    o << attributeStr(attrs, TAG_XMI_name);
    o << attributeStr(attrs, TAG_XMI_version);
    o << attributeStr(attrs, TAG_XMI_unSolvedFlag);
    o << attributeStr(attrs, TAG_XMI_isRoot);
    o << attributeStr(attrs, TAG_XMI_isLeaf);
    o << attributeStr(attrs, TAG_XMI_isAbstract);
    o << ">" << endl;

    Sequence bc (elm.GetSequence(pos_XMI_UMLStereotype_baseClass));
    Sequence ee (elm.GetSequence(pos_XMI_UMLStereotype_extendedElement));

    o << IND(indent + 1) << "<UML:Stereotype.baseClass>"
                         << seq2mbstr(bc)
                         << "</UML:Stereotype.baseClass>" << endl << flush;

    o << IND(indent + 1) << "<UML:Stereotype.extendedElement>" << endl << flush;
    WriteElementSeq(o, indent + 2, ee);
    o << IND(indent + 1) << "</UML:Stereotype.extendedElement>" << endl << flush;

    o << IND(indent) << "</UML:Stereotype>" << endl << flush;
  }
}

void XmiWriter::WriteUMLPrimitive(ofstream & o, int indent,
                                  const TYPE_XMI_UMLPrimitive & elm)
{
  Map attrs (elm.GetMap(pos_XMI_UMLPrimitive_attrs));

  o << IND(indent) << "<UML:Primitive";
  o << attributeStr(attrs, TAG_XMI_id);
  o << attributeStr(attrs, TAG_XMI_name);
  o << attributeStr(attrs, TAG_XMI_version);
  o << attributeStr(attrs, TAG_XMI_unSolvedFlag);
  o << attributeStr(attrs, TAG_XMI_isRoot);
  o << attributeStr(attrs, TAG_XMI_isLeaf);
  o << attributeStr(attrs, TAG_XMI_isAbstract);
  o << attributeStr(attrs, TAG_XMI_isActive);
  o << ">";
  o << "</UML:Primitive>" << endl;
}

void XmiWriter::WriteUMLClassifier(ofstream & o, int indent,
                                   const TYPE_XMI_UMLClassifier & elm)
{
  Map attrs (elm.GetMap(pos_XMI_UMLClassifier_attrs));
  if (attrs.DomExists(TAG_XMI_idref)) {
    o << IND(indent) << "<UML:Classifier";
    o << attributeStr(attrs, TAG_XMI_idref);
    o << "/>" << endl;
  }
  else {
    o << IND(indent) << "<UML:Classifier.feature>" << endl << flush;
    Sequence f_l (elm.GetSequence(pos_XMI_UMLClassifier_feature));
    WriteElementSeq(o, indent + 1, f_l);
    o << IND(indent) << "</UML:Classifier.feature>" << endl << flush;
  }
}

void XmiWriter::WriteUMLAttribute(ofstream & o, int indent,
                                  const TYPE_XMI_UMLAttribute & elm)
{
  Map attrs (elm.GetMap(pos_XMI_UMLAttribute_attrs));
  TYPE_XMI_UMLModelElement me (elm.GetRecord(pos_XMI_UMLAttribute_modelElement));
  TYPE_XMI_UMLFeature fe (elm.GetRecord(pos_XMI_UMLAttribute_feature));
  TYPE_XMI_UMLStructuralFeature sf (elm.GetRecord(pos_XMI_UMLAttribute_structuralFeature));
  Generic iv (elm.GetField(pos_XMI_UMLAttribute_initialValue));
  Generic ae (elm.GetField(pos_XMI_UMLAttribute_associationEnd));

  o << IND(indent) << "<UML:Attribute";
  o << attributeStr(attrs, TAG_XMI_id);
  o << attributeStr(attrs, TAG_XMI_name);
  o << attributeStr(attrs, TAG_XMI_version);
  o << attributeStr(attrs, TAG_XMI_visibility); // EA
  o << attributeStr(attrs, TAG_XMI_unSolvedFlag);
  o << attributeStr(attrs, TAG_XMI_ownerScope);
  o << attributeStr(attrs, TAG_XMI_targetScope);
  o << attributeStr(attrs, TAG_XMI_changeability);
  o << ">" << endl;

  WriteElement(o, indent + 1, me);
  WriteElement(o, indent + 1, fe);
  WriteElement(o, indent + 1, sf);

  if (!iv.IsNil()) {
    o << IND(indent + 1) << "<UML:Attribute.initialValue>" << endl << flush;
    WriteElement(o, indent + 2, iv);
    o << IND(indent + 1) << "</UML:Attribute.initialValue>" << endl << flush;
  }
  if (!ae.IsNil()) {
    o << IND(indent + 1) << "<UML:Attribute.associationEnd>" << endl << flush;
    WriteElement(o, indent + 2, ae);
    o << IND(indent + 1) << "</UML:Attribute.associationEnd>" << endl << flush;
  }
  o << IND(indent) << "</UML:Attribute>" << endl << flush;
}

void XmiWriter::WriteUMLStructuralFeature(ofstream & o, int indent,
                                          const TYPE_XMI_UMLStructuralFeature & elm)
{
  Generic multiplicity (elm.GetField(pos_XMI_UMLStructuralFeature_multiplicity));
  Generic type (elm.GetField(pos_XMI_UMLStructuralFeature_type));
  if (!multiplicity.IsNil()) {
    o << IND(indent) << "<UML:StructuralFeature.multiplicity>" << endl << flush;
    WriteElement(o, indent + 1, multiplicity);
    o << IND(indent) << "</UML:StructuralFeature.multiplicity>" << endl << flush;
  }
  if (!type.IsNil()) {
    o << IND(indent) << "<UML:StructuralFeature.type>" << endl << flush;
    WriteElement(o, indent + 1, type);
    o << IND(indent) << "</UML:StructuralFeature.type>" << endl << flush;
  }
}

void XmiWriter::WriteUMLExpression(ofstream & o, int indent,
                                   const TYPE_XMI_UMLExpression & elm)
{
  Map attrs (elm.GetMap(pos_XMI_UMLExpression_attrs));
  Generic body (elm.GetField(pos_XMI_UMLExpression_body));
  if (!body.IsNil()) {
    o << IND(indent) << "<UML:Expression";
    o << attributeStr(attrs, TAG_XMI_id);
    o << ">" << endl;

    o << IND(indent + 1) << "<UML:Expression.body>";
    o << str2webstr(seq2mbstr(body));
    o << "</UML:Expression.body>" << endl;
   
    o << IND(indent) << "</UML:Expression>" << endl << flush;
  }
}

void XmiWriter::WriteUMLBooleanExpression(ofstream & o, int indent,
                                          const TYPE_XMI_UMLBooleanExpression & elm)
{
  Map attrs (elm.GetMap(pos_XMI_UMLBooleanExpression_attrs));
  Generic body (elm.GetField(pos_XMI_UMLBooleanExpression_body));
  if (!body.IsNil()) {
    o << IND(indent) << "<UML:BooleanExpression";
    o << attributeStr(attrs, TAG_XMI_id);
    o << ">" << endl;

    o << IND(indent + 1) << "<UML:Expression.body>";
    o << str2webstr(seq2mbstr(body));
    o << "</UML:Expression.body>" << endl;
   
    o << IND(indent) << "</UML:BooleanExpression>" << endl << flush;
  }
}

void XmiWriter::WriteUMLOperation(ofstream & o, int indent,
                                  const TYPE_XMI_UMLOperation & elm)
{
  Map attrs (elm.GetMap(pos_XMI_UMLOperation_attrs));
  TYPE_XMI_UMLModelElement me (elm.GetRecord(pos_XMI_UMLOperation_modelElement));
  TYPE_XMI_UMLFeature fe (elm.GetRecord(pos_XMI_UMLOperation_feature));
  TYPE_XMI_UMLBehavioralFeature bf (elm.GetRecord(pos_XMI_UMLOperation_behavioralFeature));

  o << IND(indent) << "<UML:Operation";
  o << attributeStr(attrs, TAG_XMI_id);
  o << attributeStr(attrs, TAG_XMI_name);
  o << attributeStr(attrs, TAG_XMI_version);
  o << attributeStr(attrs, TAG_XMI_visibility); // EA
  o << attributeStr(attrs, TAG_XMI_unSolvedFlag);
  o << attributeStr(attrs, TAG_XMI_ownerScope);
  o << attributeStr(attrs, TAG_XMI_isQuery);
  o << attributeStr(attrs, TAG_XMI_concurrency);
  o << attributeStr(attrs, TAG_XMI_isRoot);
  o << attributeStr(attrs, TAG_XMI_isLeaf);
  o << attributeStr(attrs, TAG_XMI_isAbstract);
  o << ">" << endl;

  WriteElement(o, indent + 1, me);
  WriteElement(o, indent + 1, fe);
  WriteElement(o, indent + 1, bf);

  o << IND(indent) << "</UML:Operation>" << endl << flush;
}

void XmiWriter::WriteUMLBehavioralFeature(ofstream & o, int indent,
                                          const TYPE_XMI_UMLBehavioralFeature & elm)
{
  Map attrs (elm.GetMap(pos_XMI_UMLBehavioralFeature_attrs));
  if (attrs.DomExists(TAG_XMI_idref)) {
    o << IND(indent) << "<UML:BehavioralFeature";
    o << attributeStr(attrs, TAG_XMI_idref);
    o << "/>" << endl;
  }
  else {
    Sequence p_l (elm.GetSequence(pos_XMI_UMLBehavioralFeature_parameter));
    o << IND(indent) << "<UML:BehavioralFeature.parameter>" << endl << flush;
    WriteElementSeq(o, indent + 1, p_l);
    o << IND(indent) << "</UML:BehavioralFeature.parameter>" << endl << flush;
  }
}

void XmiWriter::WriteUMLParameter(ofstream & o, int indent,
                                  const TYPE_XMI_UMLParameter & elm)
{
  Map attrs (elm.GetMap(pos_XMI_UMLParameter_attrs));
  TYPE_XMI_UMLModelElement me (elm.GetRecord(pos_XMI_UMLParameter_modelElement));
  Generic bf (elm.GetField(pos_XMI_UMLParameter_behavioralFeature));
  Generic tp (elm.GetField(pos_XMI_UMLParameter_type));
  Generic dv (elm.GetField(pos_XMI_UMLParameter_defaultValue));

  o << IND(indent) << "<UML:Parameter";
  o << attributeStr(attrs, TAG_XMI_id);
  o << attributeStr(attrs, TAG_XMI_name);
  o << attributeStr(attrs, TAG_XMI_version);
  o << attributeStr(attrs, TAG_XMI_unSolvedFlag);
  o << attributeStr(attrs, TAG_XMI_kind);
  o << ">" << endl;

  WriteElement(o, indent + 1, me);

  if (!bf.IsNil()) {
    o << IND(indent + 1) << "<UML:Parameter.behavioralFeature>" << endl << flush;
    WriteElement(o, indent + 2, bf);
    o << IND(indent + 1) << "</UML:Parameter.behavioralFeature>" << endl << flush;
  }
  if (!tp.IsNil()) {
    o << IND(indent + 1) << "<UML:Parameter.type>" << endl << flush;
    WriteElement(o, indent + 2, tp);
    o << IND(indent + 1) << "</UML:Parameter.type>" << endl << flush;
  }
  if (!dv.IsNil()) {
    o << IND(indent + 1) << "<UML:Parameter.defaultValue>" << endl << flush;
    WriteElement(o, indent + 2, dv);
    o << IND(indent + 1) << "</UML:Parameter.defaultValue>" << endl << flush;
  }

  o << IND(indent) << "</UML:Parameter>" << endl << flush;
}

void XmiWriter::WriteUMLGeneralizableElement(ofstream & o, int indent,
                                             const TYPE_XMI_UMLGeneralizableElement & elm)
{
  Map attrs (elm.GetMap(pos_XMI_UMLGeneralizableElement_attrs));
  if (attrs.DomExists(TAG_XMI_idref)) {
    o << IND(indent) << "<UML:GeneralizableElement";
    o << attributeStr(attrs, TAG_XMI_idref);
    o << "/>" << endl;
  }
  else {
    Sequence g_l (elm.GetSequence(pos_XMI_UMLGeneralizableElement_generalization));
    if (!g_l.IsEmpty()) {
      o << IND(indent) << "<UML:GeneralizableElement.generalization>" << endl << flush;
      WriteElementSeq(o, indent + 1, g_l);
      o << IND(indent) << "</UML:GeneralizableElement.generalization>" << endl << flush;
    }
  }
}

void XmiWriter::WriteUMLMultiplicity(ofstream & o, int indent,
                                     const TYPE_XMI_UMLMultiplicity & elm)
{
  Map attrs (elm.GetMap(pos_XMI_UMLMultiplicity_attrs));
  Generic rg (elm.GetField(pos_XMI_UMLMultiplicity_range));

  o << IND(indent) << "<UML:Multiplicity";
  o << attributeStr(attrs, TAG_XMI_id);
  o << ">" << endl;

  if (!rg.IsNil()) {
    o << IND(indent + 1) << "<UML:Multiplicity.range>" << endl << flush;
    WriteElement(o, indent + 2, rg);
    o << IND(indent + 1) << "</UML:Multiplicity.range>" << endl << flush;
  }

  o << IND(indent) << "</UML:Multiplicity>" << endl << flush;
}

void XmiWriter::WriteUMLMultiplicityRange(ofstream & o, int indent,
                                          const TYPE_XMI_UMLMultiplicityRange & elm)
{
  Map attrs (elm.GetMap(pos_XMI_UMLMultiplicityRange_attrs));

  o << IND(indent) << "<UML:MultiplicityRange";
  o << attributeStr(attrs, TAG_XMI_id);
  o << attributeStr(attrs, TAG_XMI_lowerValue);
  o << attributeStr(attrs, TAG_XMI_upperValue);
  o << "/>" << endl;
}

void XmiWriter::WriteUMLTaggedValue(ofstream & o, int indent,
                                    const TYPE_XMI_UMLTaggedValue & elm)
{
  Map attrs (elm.GetMap(pos_XMI_UMLTaggedValue_attrs));
  TYPE_XMI_UMLModelElement me (elm.GetRecord(pos_XMI_UMLTaggedValue_modelElement));

  if (IsASTAH()) {
    o << IND(indent) << "<UML:TaggedValue";
    o << attributeStr(attrs, TAG_XMI_id);
    o << attributeStr(attrs, TAG_XMI_version);
    o << attributeStr(attrs, TAG_XMI_tag);
    o << attributeStr(attrs, TAG_XMI_value);
    o << ">" << endl;

    o << IND(indent + 1) << "<UML:TaggedValue.modelElement>" << endl << flush;
    WriteElement(o, indent + 2, me);
    o << IND(indent + 1) << "</UML:TaggedValue.modelElement>" << endl << flush;

    o << IND(indent) << "</UML:TaggedValue>" << endl << flush;
  }
  if (IsEA()) {
    o << IND(indent) << "<UML:TaggedValue";
    o << attributeStr(attrs, TAG_XMI_tag);
    o << attributeStr(attrs, TAG_XMI_value);
    o << "/>" << endl << flush;
  }
}

void XmiWriter::WriteUMLConstraint(ofstream & o, int indent,
                                   const TYPE_XMI_UMLConstraint & elm)
{
  Map attrs (elm.GetMap(pos_XMI_UMLConstraint_attrs));
  TYPE_XMI_UMLModelElement me (elm.GetRecord(pos_XMI_UMLConstraint_modelElement));
  Generic body (elm.GetField(pos_XMI_UMLConstraint_body));
  Generic ce (elm.GetField(pos_XMI_UMLConstraint_constrainedElement));

  if (attrs.DomExists(TAG_XMI_idref)) {
    o << IND(indent) << "<UML:Constraint";
    o << attributeStr(attrs, TAG_XMI_idref);
    o << "/>" << endl;
  }
  else {
    o << IND(indent) << "<UML:Constraint";
    o << attributeStr(attrs, TAG_XMI_id);
    o << attributeStr(attrs, TAG_XMI_name);
    o << attributeStr(attrs, TAG_XMI_version);
    o << ">" << endl;

    WriteElement(o, indent + 1, me);

    if (!body.IsNil()) {
      o << IND(indent + 1) << "<UML:Constraint.body>" << endl << flush;
      WriteElement(o, indent + 2, body);
      o << IND(indent + 1) << "</UML:Constraint.body>" << endl << flush;
    }
    if (!ce.IsNil()) {
      o << IND(indent + 1) << "<UML:Constraint.constrainedElement>" << endl << flush;
      WriteElement(o, indent + 2, ce);
      o << IND(indent + 1) << "</UML:Constraint.constrainedElement>" << endl << flush;
    }

    o << IND(indent) << "</UML:Constraint>" << endl << flush;
  }
}

string XmiWriter::attributeStr(const Map & attrs, const Sequence & key)
{
  string res;
  if (attrs.DomExists(key)) {
    res += " ";
    res += seq2mbstr(key);
    res += "=\"";
    if ((key == TAG_XMI_name) ||
        (key == TAG_XMI_value)) {
      res += str2xmistr(seq2mbstr(attrs[Sequence(key)])); 
    }
    else {
      res += seq2mbstr(attrs[Sequence(key)]); 
    }
    res += "\"";
  }
  return res;
}

string XmiWriter::IND(int level)
{
  string res;
  for (int i = 0; i < level; i++) {
    res += "  ";
  }
  return res;
}

string XmiWriter::seq2mbstr(const Sequence & seq)
{
  return TBWSTR::wstring2utf8str(seq.GetString());
}

string XmiWriter::str2xmistr(const string & str)
{
  if (IsASTAH()) {
    return str2astahstr(str);
  }
  else {
    return str2webstr(str);
  }
}

string XmiWriter::str2astahstr(const string & str)
{
  string res;
  for (int i = 0; i < (int)str.size(); i++) {
    unsigned char c = str[i];
    if  ((('0' <= c) && ('9' >= c)) ||
         (('a' <= c) && ('z' >= c)) ||
         (('A' <= c) && ('Z' >= c))) {
      res += c;
    }
    else {
      switch (c) {
        case '*':
        case '-':
        case '_':
        case '.': {
          res += c;
          break;
        }
        case ' ': {
          res += '+';
          break;
        }
        default: {
          char buf [4];
          sprintf(buf, "%02X", c);
          res += '%';
          res += string(buf);
          break;
        }
      }
    }
  } 
  return res;
}

string XmiWriter::str2webstr(const string & str)
{
  string res;
  for (int i = 0; i < (int)str.size(); i++) {
    unsigned char c = str[i];
    switch(c) {
      case '&': {
        res += "&amp;";
        break;
      }
      case '<': {
        res += "&lt;";
        break;
      }
      case '>': {
        res += "&gt;";
        break;
      }
      case '"': {
        res += "&quot;";
        break;
      }
      default: {
        res += c;
        break;
      }
    }
  } 
  return res;
}
