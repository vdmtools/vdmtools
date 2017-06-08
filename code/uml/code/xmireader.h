#ifndef __xmireader_h__
#define __xmireader_h__

#include "metaiv.h"
#include <libxml/xmlreader.h>
#include <string>

class XmiReader
{
public:
  XmiReader(int m); 
  ~XmiReader(); 

  Tuple ReadXMIDocument(const wstring & filename);
  void CleanUp();

  void SetMode(int m) { this->mode = m; };
  int GetMode() { return this->mode; };

private:
  int ReadNextLine(xmlTextReaderPtr reader);
  Generic ReadDocument(xmlTextReaderPtr reader);
  Generic ReadElement(xmlTextReaderPtr reader);
  Generic ReadText(xmlTextReaderPtr reader);

  Record CreateNewElement(xmlTextReaderPtr reader);
  Record SetElement(const Record & e, const Generic & element);
  TYPE_XMI_Document SetXMIDocument(const TYPE_XMI_Document & e, const Generic & element);
  TYPE_XMI_Header SetXMIHeader(const TYPE_XMI_Header & e, const Generic & element);
  TYPE_XMI_Content SetXMIContent(const TYPE_XMI_Content & e, const Generic & element);
  TYPE_XMI_Documentation SetXMIDocumentation(const TYPE_XMI_Documentation & e, const Generic & element);
  TYPE_XMI_SortedVersionHistories SetXMISortedVersionHistories(const TYPE_XMI_SortedVersionHistories & e,
                                                               const Generic & element);
  TYPE_XMI_Field SetXMIField(const TYPE_XMI_Field & e, const Generic & element);
  TYPE_XMI_Exporter SetXMIExporter(const TYPE_XMI_Exporter & e, const Generic & element);
  TYPE_XMI_ExporterVersion SetXMIExporterVersion(const TYPE_XMI_ExporterVersion & e, const Generic & element);

  TYPE_XMI_UMLModelElement SetUMLModelElement(const TYPE_XMI_UMLModelElement & e, const Generic & element);
  TYPE_XMI_UMLNamespace SetUMLNamespace(const TYPE_XMI_UMLNamespace & e, const Generic & element);
  TYPE_XMI_UMLModel SetUMLModel(const TYPE_XMI_UMLModel & e, const Generic & element);
  TYPE_XMI_UMLTaggedValue SetUMLTaggedValue(const TYPE_XMI_UMLTaggedValue & e, const Generic & element);
  TYPE_XMI_UMLClass SetUMLClass(const TYPE_XMI_UMLClass & e, const Generic & element);
  TYPE_XMI_UMLPackage SetUMLPackage(const TYPE_XMI_UMLPackage & e, const Generic & element);
  TYPE_XMI_UMLInterface SetUMLInterface(const TYPE_XMI_UMLInterface & e, const Generic & element);
  TYPE_XMI_UMLStereotype SetUMLStereotype(const TYPE_XMI_UMLStereotype & e, const Generic & element);
  TYPE_XMI_UMLClassifier SetUMLClassifier(const TYPE_XMI_UMLClassifier & e, const Generic & element);
  TYPE_XMI_UMLFeature SetUMLFeature(const TYPE_XMI_UMLFeature & e, const Generic & element);
  TYPE_XMI_UMLOperation SetUMLOperation(const TYPE_XMI_UMLOperation & e, const Generic & element);
  TYPE_XMI_UMLBehavioralFeature SetUMLBehavioralFeature(const TYPE_XMI_UMLBehavioralFeature & e,
                                                        const Generic & element);
  TYPE_XMI_UMLParameter SetUMLParameter(const TYPE_XMI_UMLParameter & e, const Generic & element);
  TYPE_XMI_UMLGeneralizableElement SetUMLGeneralizableElement(const TYPE_XMI_UMLGeneralizableElement & e,
                                                              const Generic & element);
  TYPE_XMI_UMLGeneralization SetUMLGeneralization(const TYPE_XMI_UMLGeneralization & e, const Generic & element);
  TYPE_XMI_UMLAssociation SetUMLAssociation(const TYPE_XMI_UMLAssociation & e, const Generic & element);
  TYPE_XMI_UMLAssociationEnd SetUMLAssociationEnd(const TYPE_XMI_UMLAssociationEnd & e, const Generic & element);
  TYPE_XMI_UMLAttribute SetUMLAttribute(const TYPE_XMI_UMLAttribute & e, const Generic & element);
  TYPE_XMI_UMLStructuralFeature SetUMLStructuralFeature(const TYPE_XMI_UMLStructuralFeature & e,
                                                        const Generic & element);
  TYPE_XMI_UMLMultiplicity SetUMLMultiplicity(const TYPE_XMI_UMLMultiplicity & e, const Generic & element);
  TYPE_XMI_UMLExpression SetUMLExpression(const TYPE_XMI_UMLExpression & e, const Generic & element);
  TYPE_XMI_UMLBooleanExpression SetUMLBooleanExpression(const TYPE_XMI_UMLBooleanExpression & e,
                                                        const Generic & element);
  TYPE_XMI_UMLConstraint SetUMLConstraint(const TYPE_XMI_UMLConstraint & e, const Generic & element);
  TYPE_XMI_UMLDiagram SetUMLDiagram(const TYPE_XMI_UMLDiagram & e, const Generic & element);

  string xmistr2str(const string & str, bool sjis);
  string astahstr2str(const string & str);
  int hexchar2int (const char c);

  Map GetAttribute(xmlTextReaderPtr reader);
  void DumpNodeInfo(xmlTextReaderPtr reader);
  wstring TypeNoToStr(int i);
  wstring GetEncoding(xmlTextReaderPtr reader);
  void ReportCanNotSet(const Record & e, const Generic & element);

  void CheckExporter(const Sequence & ep);
  void CheckExporterVersion(const Sequence & ep);
  bool IsASTAH();
  bool IsEA();
  bool IsShiftJIS(xmlTextReaderPtr reader);

  xmlTextReaderPtr reader;

  Map xmidef; // map seq of char to (set of seq of char * set of seq of char * [set of seq of char])
  Map umldef; // map seq of char to (set of seq of char * set of seq of char * [set of seq of char])
  Map idm;
  Set yet;
  Set yet_astah;
  Set not_set;

  int mode;  
};
#endif // __xmireader_h__
