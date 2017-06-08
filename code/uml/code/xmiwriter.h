#ifndef __xmiwriter_h__
#define __xmiwriter_h__

#include "metaiv.h"
#include "XMI.h"
#include <string>

class XmiWriter
{
public:
  XmiWriter(int m);
  virtual ~XmiWriter();
  bool SaveAs(const std::wstring & fname,
              const TYPE_XMI_Document & doc);

  void SetMode(int m) { this->mode = m; };

private:
  void WriteFile(ofstream & o, const TYPE_XMI_Document & doc);

  void WriteElementSeq(ofstream & o, int indent, const Sequence & elm_l);
  void WriteElement(ofstream & o, int indent, const Record & elm);

  void WriteXMIDocument(ofstream & o, int indent, const TYPE_XMI_Document & elm);
  void WriteXMIHeader(ofstream & o, int indent, const TYPE_XMI_Header & elm);
  void WriteXMIDocumentation(ofstream & o, int indent, const TYPE_XMI_Documentation & elm);
  void WriteXMIExporter(ofstream & o, int indent, const TYPE_XMI_Exporter & elm);
  void WriteXMIExporterVersion(ofstream & o, int indent, const TYPE_XMI_ExporterVersion & elm);
  void WriteXMICurrentModelVersion(ofstream & o, int indent, const TYPE_XMI_CurrentModelVersion & elm);
  void WriteXMIMaxModelVersion(ofstream & o, int indent, const TYPE_XMI_MaxModelVersion & elm);
  void WriteXMICurrentModelProducer(ofstream & o, int indent, const TYPE_XMI_CurrentModelProducer & elm);
  void WriteXMISortedVersionHistories(ofstream & o, int indent, const TYPE_XMI_SortedVersionHistories & elm);
  void WriteXMIVersionEntry(ofstream & o, int indent, const TYPE_XMI_VersionEntry & elm);
  void WriteXMIImport(ofstream & o, int indent, const TYPE_XMI_Import & elm);
  void WriteXMIMetametamodel(ofstream & o, int indent, const TYPE_XMI_Metametamodel & elm);
  void WriteXMIMetamodel(ofstream & o, int indent, const TYPE_XMI_Metamodel & elm);
  void WriteXMIModel(ofstream & o, int indent, const TYPE_XMI_Model & elm);
  void WriteXMIContent(ofstream & o, int indent, const TYPE_XMI_Content & elm);

  void WriteUMLModelElement(ofstream & o, int indent, const TYPE_XMI_UMLModelElement & elm);
  void WriteUMLNamespace(ofstream & o, int indent, const TYPE_XMI_UMLNamespace & elm);
  void WriteUMLFeature(ofstream & o, int indent, const TYPE_XMI_UMLFeature & elm);
  void WriteUMLModel(ofstream & o, int indent, const TYPE_XMI_UMLModel & elm);
  void WriteUMLPackage(ofstream & o, int indent, const TYPE_XMI_UMLPackage & elm);
  void WriteUMLInterface(ofstream & o, int indent, const TYPE_XMI_UMLInterface & elm);
  void WriteUMLClass(ofstream & o, int indent, const TYPE_XMI_UMLClass & elm);
  void WriteUMLAssociation(ofstream & o, int indent, const TYPE_XMI_UMLAssociation & elm);
  void WriteUMLAssociationEnd(ofstream & o, int indent, const TYPE_XMI_UMLAssociationEnd & elm);
  void WriteUMLGeneralization(ofstream & o, int indent, const TYPE_XMI_UMLGeneralization & elm);
  void WriteUMLStereotype(ofstream & o, int indent, const TYPE_XMI_UMLStereotype & elm);
  void WriteUMLPrimitive(ofstream & o, int indent, const TYPE_XMI_UMLPrimitive & elm);

  void WriteUMLClassifier(ofstream & o, int indent, const TYPE_XMI_UMLClassifier & elm);
  void WriteUMLAttribute(ofstream & o, int indent, const TYPE_XMI_UMLAttribute & elm);
  void WriteUMLStructuralFeature(ofstream & o, int indent, const TYPE_XMI_UMLStructuralFeature & elm);
  void WriteUMLExpression(ofstream & o, int indent, const TYPE_XMI_UMLExpression & elm);
  void WriteUMLBooleanExpression(ofstream & o, int indent, const TYPE_XMI_UMLBooleanExpression & elm);
  void WriteUMLOperation(ofstream & o, int indent, const TYPE_XMI_UMLOperation & elm);
  void WriteUMLBehavioralFeature(ofstream & o, int indent, const TYPE_XMI_UMLBehavioralFeature & elm);
  void WriteUMLParameter(ofstream & o, int indent, const TYPE_XMI_UMLParameter & elm);
  void WriteUMLGeneralizableElement(ofstream & o, int indent, const TYPE_XMI_UMLGeneralizableElement & elm);
  void WriteUMLMultiplicity(ofstream & o, int indent, const TYPE_XMI_UMLMultiplicity & elm);
  void WriteUMLMultiplicityRange(ofstream & o, int indent, const TYPE_XMI_UMLMultiplicityRange & elm);
  void WriteUMLTaggedValue(ofstream & o, int indent, const TYPE_XMI_UMLTaggedValue & elm);
  void WriteUMLConstraint(ofstream & o, int indent, const TYPE_XMI_UMLConstraint & elm);

  string attributeStr(const Map & m, const Sequence & key);
  string IND(int level);
  string seq2mbstr(const Sequence & seq);
  string str2xmistr(const string & str);
  string str2webstr(const string & str);
  string str2astahstr(const string & str);

  bool IsASTAH();
  bool IsEA();

  int mode;
};

#endif // __xmiwriter_h__
