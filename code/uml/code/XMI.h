#ifndef __XMI_h__
#define __XMI_h__

#include "main_tag.h"
//#define XMI_OFFSET        4000000

#define TAG_TYPE_XMI_Document                 (XMI_OFFSET + 0)
#define TAG_TYPE_XMI_Header                   (XMI_OFFSET + 1)
#define TAG_TYPE_XMI_Content                  (XMI_OFFSET + 2)
#define TAG_TYPE_XMI_Documentation            (XMI_OFFSET + 3)
#define TAG_TYPE_XMI_Exporter                 (XMI_OFFSET + 4)
#define TAG_TYPE_XMI_ExporterVersion          (XMI_OFFSET + 5)
#define TAG_TYPE_XMI_CurrentModelVersion      (XMI_OFFSET + 6)
#define TAG_TYPE_XMI_MaxModelVersion          (XMI_OFFSET + 7)
#define TAG_TYPE_XMI_CurrentModelProducer     (XMI_OFFSET + 8)
#define TAG_TYPE_XMI_SortedVersionHistories   (XMI_OFFSET + 9)
#define TAG_TYPE_XMI_Import                   (XMI_OFFSET + 10)
#define TAG_TYPE_XMI_Metametamodel            (XMI_OFFSET + 11)
#define TAG_TYPE_XMI_Metamodel                (XMI_OFFSET + 12)
#define TAG_TYPE_XMI_Model                    (XMI_OFFSET + 13)
#define TAG_TYPE_XMI_VersionEntry             (XMI_OFFSET + 14)
#define TAG_TYPE_XMI_Field                    (XMI_OFFSET + 15)
#define TAG_TYPE_XMI_Extension                (XMI_OFFSET + 16)
#define TAG_TYPE_XMI_Difference               (XMI_OFFSET + 17)
#define TAG_TYPE_XMI_Extensions               (XMI_OFFSET + 18)

#define TAG_TYPE_XMI_UMLNamespace             (XMI_OFFSET + 100)
#define TAG_TYPE_XMI_UMLStereotype            (XMI_OFFSET + 101)
#define TAG_TYPE_XMI_UMLTaggedValue           (XMI_OFFSET + 102)
#define TAG_TYPE_XMI_UMLModelElement          (XMI_OFFSET + 103)
#define TAG_TYPE_XMI_UMLModel                 (XMI_OFFSET + 104)
#define TAG_TYPE_XMI_UMLOperation             (XMI_OFFSET + 105)
#define TAG_TYPE_XMI_UMLPackage               (XMI_OFFSET + 106)
#define TAG_TYPE_XMI_UMLClassifier            (XMI_OFFSET + 107)
#define TAG_TYPE_XMI_UMLClass                 (XMI_OFFSET + 108)
#define TAG_TYPE_XMI_UMLGeneralizableElement  (XMI_OFFSET + 109)
#define TAG_TYPE_XMI_UMLGeneralization        (XMI_OFFSET + 110)
#define TAG_TYPE_XMI_UMLAssociation           (XMI_OFFSET + 111)
#define TAG_TYPE_XMI_UMLAssociationEnd        (XMI_OFFSET + 112)
#define TAG_TYPE_XMI_UMLFeature               (XMI_OFFSET + 113)
#define TAG_TYPE_XMI_UMLMultiplicity          (XMI_OFFSET + 114)
#define TAG_TYPE_XMI_UMLMultiplicityRange     (XMI_OFFSET + 115)
#define TAG_TYPE_XMI_UMLStructuralFeature     (XMI_OFFSET + 116)
#define TAG_TYPE_XMI_UMLInterface             (XMI_OFFSET + 117)
#define TAG_TYPE_XMI_UMLPrimitive             (XMI_OFFSET + 118)
#define TAG_TYPE_XMI_UMLParameter             (XMI_OFFSET + 119)
#define TAG_TYPE_XMI_UMLBehavioralFeature     (XMI_OFFSET + 120)
#define TAG_TYPE_XMI_UMLAttribute             (XMI_OFFSET + 121)
#define TAG_TYPE_XMI_UMLExpression            (XMI_OFFSET + 122)
#define TAG_TYPE_XMI_UMLConstraint            (XMI_OFFSET + 123)
#define TAG_TYPE_XMI_UMLBooleanExpression     (XMI_OFFSET + 124)
#define TAG_TYPE_XMI_UMLDataType              (XMI_OFFSET + 125)
#define TAG_TYPE_XMI_UMLDiagram               (XMI_OFFSET + 126)
#define TAG_TYPE_XMI_UMLDiagramElement        (XMI_OFFSET + 127)

#define TAG_TYPE_XMI_EAStub                   (XMI_OFFSET + 200)

enum {
  length_XMI_Document = 3,
  pos_XMI_Document_attrs = 1,
  pos_XMI_Document_header = 2,
  pos_XMI_Document_content = 3,
  length_XMI_Header = 6,
  pos_XMI_Header_attrs = 1,
  pos_XMI_Header_documentation = 2,
  pos_XMI_Header_import = 3,
  pos_XMI_Header_metametamodel = 4,
  pos_XMI_Header_metamodel = 5,
  pos_XMI_Header_model = 6,
  length_XMI_Content = 4,
  pos_XMI_Content_attrs = 1,
  pos_XMI_Content_model = 2,
  pos_XMI_Content_primitives = 3,
  pos_XMI_Content_extensions = 4,
  length_XMI_Documentation = 7,
  post_XMI_Documentation_attrs = 1,
  post_XMI_Documentation_exporter = 2,
  post_XMI_Documentation_exporterVersion = 3,
  post_XMI_Documentation_currentModelVersion = 4,
  post_XMI_Documentation_maxModelVersion = 5,
  post_XMI_Documentation_currentModelProducer = 6,
  post_XMI_Documentation_storedVersionHistories = 7,
  length_XMI_Exporter = 2,
  pos_XMI_Exporter_attrs = 1,
  pos_XMI_Exporter_text = 2,
  length_XMI_ExporterVersion = 2,
  pos_XMI_ExporterVersion_attrs = 1,
  pos_XMI_ExporterVersion_text = 2,
  length_XMI_CurrentModelVersion = 2,
  pos_XMI_CurrentModelVersion_attrs = 1,
  pos_XMI_CurrentModelVersion_text = 2,
  length_XMI_MaxModelVersion = 2,
  pos_XMI_MaxModelVersion_attrs = 1,
  pos_XMI_MaxModelVersion_text = 2,
  length_XMI_currentModelProducer = 2,
  pos_XMI_currentModelProducer_attrs = 1,
  pos_XMI_currentModelProducer_text = 2,
  length_XMI_SortedVersionHistories = 2,
  pos_XMI_SortedVersionHistories_attrs = 1,
  pos_XMI_SortedVersionHistories_versionEntry = 2,
  length_XMI_VersionEntry = 1,
  pos_XMI_VersionEntry_attrs = 1,
  length_XMI_Import = 2,
  pos_XMI_Import_attrs = 1,
  pos_XMI_Import_data = 2,
  length_XMI_Metametamodel = 1,
  pos_XMI_Metametamodel_attrs = 1,
  length_XMI_Metamodel = 1,
  pos_XMI_Metamodel_attrs = 1,
  length_XMI_Model = 1,
  pos_XMI_Model_attrs = 1,
  length_XMI_UMLModel = 3,
  pos_XMI_UMLModel_attrs = 1,
  pos_XMI_UMLModel_modelElement = 2,
  pos_XMI_UMLModel_namespace = 3,
  length_XMI_UMLModelElement = 7,
  pos_XMI_UMLModelElement_attrs = 1,
  pos_XMI_UMLModelElement_namespace = 2,
  pos_XMI_UMLModelElement_visibility = 3,
  pos_XMI_UMLModelElement_definition = 4,
  pos_XMI_UMLModelElement_stereotype = 5,
  pos_XMI_UMLModelElement_taggedValue = 6,
  pos_XMI_UMLModelElement_constraint = 7,
  length_XMI_UMLNamespace = 2,
  pos_XMI_UMLNamespace_attrs = 1,
  pos_XMI_UMLNamespace_ownedElement = 2,
  length_XMI_UMLClass = 5,
  pos_XMI_UMLClass_attrs = 1,
  pos_XMI_UMLClass_modelElement = 2,
  pos_XMI_UMLClass_namespace = 3,
  pos_XMI_UMLClass_generalizableElement = 4,
  pos_XMI_UMLClass_classifier = 5,
  length_XMI_UMLClassifier = 2,
  pos_XMI_UMLClassifier_attrs = 1,
  pos_XMI_UMLClassifier_feature = 2,
  length_XMI_UMLAttribute =5,
  pos_XMI_UMLAttribute_attrs = 1,
  pos_XMI_UMLAttribute_modelElement = 2,
  pos_XMI_UMLAttribute_feature = 3,
  pos_XMI_UMLAttribute_structuralFeature = 4,
  pos_XMI_UMLAttribute_initialValue = 5,
  pos_XMI_UMLAttribute_associationEnd = 6,
  length_XMI_UMLFeature = 3,
  pos_XMI_UMLFeature_attrs = 1,
  pos_XMI_UMLFeature_owner = 2,
  pos_XMI_UMLFeature_visibility = 3,
  length_XMI_UMLStructuralFeature = 3,
  pos_XMI_UMLStructuralFeature_attrs = 1,
  pos_XMI_UMLStructuralFeature_multiplicity = 2,
  pos_XMI_UMLStructuralFeature_type = 3,
  length_XMI_UMLExpression = 2,
  pos_XMI_UMLExpression_attrs = 1,
  pos_XMI_UMLExpression_body = 2,
  length_XMI_UMLPrimitive = 2,
  pos_XMI_UMLPrimitive_attrs = 1,
  pos_XMI_UMLPrimitive_primitive = 2,
  length_XMI_UMLOperation = 4,
  pos_XMI_UMLOperation_attrs = 1,
  pos_XMI_UMLOperation_modelElement = 2,
  pos_XMI_UMLOperation_feature = 3,
  pos_XMI_UMLOperation_behavioralFeature = 4,
  length_XMI_UMLBehavioralFeature = 2,
  pos_XMI_UMLBehavioralFeature_attrs = 1,
  pos_XMI_UMLBehavioralFeature_parameter = 2,
  length_XMI_UMLParameter = 5,
  pos_XMI_UMLParameter_attrs = 1,
  pos_XMI_UMLParameter_modelElement = 2,
  pos_XMI_UMLParameter_behavioralFeature = 3,
  pos_XMI_UMLParameter_type = 4,
  pos_XMI_UMLParameter_defaultValue = 5,
  length_XMI_UMLStereotype = 3,
  pos_XMI_UMLStereotype_attrs = 1,
  pos_XMI_UMLStereotype_baseClass = 2,
  pos_XMI_UMLStereotype_extendedElement = 3,
  length_XMI_UMLGeneralizableElement = 2,
  pos_XMI_UMLGeneralizableElement_attrs = 1,
  pos_XMI_UMLGeneralizableElement_generalization = 2,
  length_XMI_UMLGeneralization = 5,
  pos_XMI_UMLGeneralization_attrs = 1,
  pos_XMI_UMLGeneralization_modelElement = 2,
  pos_XMI_UMLGeneralization_child = 3,
  pos_XMI_UMLGeneralization_parent = 4,
  pos_XMI_UMLGeneralization_flag = 5,
  length_XMI_UMLAssociationEnd = 7,
  pos_XMI_UMLAssociationEnd_attrs = 1,
  pos_XMI_UMLAssociationEnd_modelElement = 2,
  pos_XMI_UMLAssociationEnd_feature = 3,
  pos_XMI_UMLAssociationEnd_structuralFeature = 4,
  pos_XMI_UMLAssociationEnd_participant = 5,
  pos_XMI_UMLAssociationEnd_initialValue = 6,
  pos_XMI_UMLAssociationEnd_association = 7,
  pos_XMI_UMLAssociationEnd_visibility = 8,
  pos_XMI_UMLAssociationEnd_qualifier = 9,
  length_XMI_UMLAssociation = 3,
  pos_XMI_UMLAssociation_attrs = 1,
  pos_XMI_UMLAssociation_modelElement = 2,
  pos_XMI_UMLAssociation_connection = 3,
  length_XMI_UMLMultiplicity = 2,
  pos_XMI_UMLMultiplicity_attrs = 1,
  pos_XMI_UMLMultiplicity_range = 2,
  length_XMI_UMLMultiplicityRange = 1,
  pos_XMI_UMLMultiplicityRange_attrs = 1,
  length_XMI_UMLTaggedValue = 2,
  pos_XMI_UMLTaggedValue_attrs = 1,
  pos_XMI_UMLTaggedValue_modelElement = 2,
  length_XMI_UMLConstraint = 4,
  pos_XMI_UMLConstraint_attrs = 1,
  pos_XMI_UMLConstraint_modelElement = 2,
  pos_XMI_UMLConstraint_body = 3,
  pos_XMI_UMLConstraint_constrainedElement = 4,
  length_XMI_UMLBooleanExpression = 2,
  pos_XMI_UMLBooleanExpression_attrs = 1,
  pos_XMI_UMLBooleanExpression_body = 2,
  length_XMI_EAStub = 1,
  pos_XMI_EAStub_attrs = 1
};

class TYPE_XMI_Header : public Record {
public:
  TYPE_XMI_Header () : Record(TAG_TYPE_XMI_Header, 6) {}
  TYPE_XMI_Header &Init (const Map & p1, const Generic & p2, const Generic & p3,
                         const Generic & p4, const Generic & p5, const Generic & p6)
  { SetField(1, p1); SetField(2, p2); SetField(3, p3);
    SetField(4, p4); SetField(5, p5); SetField(6, p6); return *this; };
  TYPE_XMI_Header (const Common &c) : Record(c) {}

  const wchar_t * GetTypeName () const {
    return L"TYPE_XMI_Header";
  }
  
  const Map & get_attrs () const { return GetMap(1); };
  void set_attrs (const Map &p) { SetField(1, p); };
  const Generic  & get_documentation () const { return GetField(2); };
  void set_documentation (const Generic  &p) { SetField(2, p); };
  const Generic  & get_import () const { return GetField(3); };
  void set_import (const Generic  &p) { SetField(3, p); };
  const Generic  & get_metametamodel () const { return GetField(4); };
  void set_metametamodel (const Generic  &p) { SetField(4, p); };
  const Generic  & get_metamodel () const { return GetField(5); };
  void set_metamodel (const Generic  &p) { SetField(5, p); };
  const Generic  & get_model () const { return GetField(6); };
  void set_model (const Generic  &p) { SetField(6, p); };
};

class TYPE_XMI_Content : public Record {
public:
  TYPE_XMI_Content () : Record(TAG_TYPE_XMI_Content, 5) {}
  TYPE_XMI_Content &Init (const Map & p1, const Generic & p2,
                          const Sequence & p3, const Sequence & p4,
                          const Generic & p5)
  { SetField(1, p1); SetField(2, p2); SetField(3, p3);
    SetField(4, p4); SetField(5, p5); return *this; };
  TYPE_XMI_Content (const Common &c) : Record(c) {}

  const wchar_t * GetTypeName () const {
    return L"TYPE_XMI_Content";
  }
  
  const Map & get_attrs () const { return GetMap(1); };
  void set_attrs (const Map &p) { SetField(1, p); };
  const Generic & get_model () const { return GetField(2); };
  void set_model (const Generic &p) { SetField(2, p); };
  const Sequence & get_primitives () const { return GetSequence(3); };
  void set_primitives (const Sequence &p) { SetField(3, p); };
  const Sequence & get_extensions () const { return GetSequence(4); };
  void set_extensions (const Sequence &p) { SetField(4, p); };
  const Generic & get_diagram () const { return GetField(5); };
  void set_diagram (const Generic &p) { SetField(5, p); };
};

class TYPE_XMI_Documentation : public Record {
public:
  TYPE_XMI_Documentation () : Record(TAG_TYPE_XMI_Documentation, 7) {}
  TYPE_XMI_Documentation &Init (const Map & p1, const Generic & p2, const Generic & p3,
                                const Generic & p4, const Generic & p5, const Generic & p6,
                                const Generic & p7)
  { SetField(1, p1); SetField(2, p2); SetField(3, p3);
    SetField(4, p4); SetField(5, p5); SetField(6, p6); SetField(7, p7); return *this; };
  TYPE_XMI_Documentation (const Common &c) : Record(c) {}

  const wchar_t * GetTypeName () const {
    return L"TYPE_XMI_Documentation";
  }
  
  const Map & get_attrs () const { return GetMap(1); };
  void set_attrs (const Map &p) { SetField(1, p); };
  const Generic & get_exporter () const { return GetField(2); };
  void set_exporter (const Generic &p) { SetField(2, p); };
  const Generic & get_exporterVersion () const { return GetField(3); };
  void set_exporterVersion (const Generic &p) { SetField(3, p); };
  const Generic & get_currentModelVersion () const { return GetField(4); };
  void set_currentModelVersion (const Generic &p) { SetField(4, p); };
  const Generic & get_maxModelVersion () const { return GetField(5); };
  void set_maxModelVersion (const Generic &p) { SetField(5, p); };
  const Generic & get_currentModelProducer () const { return GetField(6); };
  void set_currentModelProducer (const Generic &p) { SetField(6, p); };
  const Generic & get_sortedVersionHistories () const { return GetField(7); };
  void set_sortedVersionHistories (const Generic &p) { SetField(7, p); };
};

class TYPE_XMI_Exporter : public Record {
public:
  TYPE_XMI_Exporter () : Record(TAG_TYPE_XMI_Exporter, 2) {}
  TYPE_XMI_Exporter &Init (const Map & p1, const Generic & p2)
  { SetField(1, p1); SetField(2, p2); return *this; };
  TYPE_XMI_Exporter (const Common &c) : Record(c) {}

  const wchar_t * GetTypeName () const {
    return L"TYPE_XMI_Exporter";
  }
  
  const Map & get_attrs () const { return GetMap(1); };
  void set_attrs (const Map &p) { SetField(1, p); };
  const Generic & get_text () const { return GetField(2); };
  void set_text (const Generic &p) { SetField(2, p); };
};

class TYPE_XMI_ExporterVersion : public Record {
public:
  TYPE_XMI_ExporterVersion () : Record(TAG_TYPE_XMI_ExporterVersion, 2) {}
  TYPE_XMI_ExporterVersion &Init (const Map & p1, const Generic & p2)
  { SetField(1, p1); SetField(2, p2); return *this; };
  TYPE_XMI_ExporterVersion (const Common &c) : Record(c) {}

  const wchar_t * GetTypeName () const {
    return L"TYPE_XMI_ExporterVersion";
  }
  
  const Map & get_attrs () const { return GetMap(1); };
  void set_attrs (const Map &p) { SetField(1, p); };
  const Generic & get_text () const { return GetField(2); };
  void set_text (const Generic &p) { SetField(2, p); };
};

class TYPE_XMI_CurrentModelVersion : public Record {
public:
  TYPE_XMI_CurrentModelVersion () : Record(TAG_TYPE_XMI_CurrentModelVersion, 2) {}
  TYPE_XMI_CurrentModelVersion &Init (const Map & p1, const Generic & p2)
  { SetField(1, p1); SetField(2, p2); return *this; };
  TYPE_XMI_CurrentModelVersion (const Common &c) : Record(c) {}

  const wchar_t * GetTypeName () const {
    return L"TYPE_XMI_CurrentModelVersion";
  }
  
  const Map & get_attrs () const { return GetMap(1); };
  void set_attrs (const Map &p) { SetField(1, p); };
  const Generic & get_text () const { return GetField(2); };
  void set_text (const Generic &p) { SetField(2, p); };
};

class TYPE_XMI_MaxModelVersion : public Record {
public:
  TYPE_XMI_MaxModelVersion () : Record(TAG_TYPE_XMI_MaxModelVersion, 2) {}
  TYPE_XMI_MaxModelVersion &Init (const Map & p1, const Generic & p2)
  { SetField(1, p1); SetField(2, p2); return *this; };
  TYPE_XMI_MaxModelVersion (const Common &c) : Record(c) {}

  const wchar_t * GetTypeName () const {
    return L"TYPE_XMI_MaxModelVersion";
  }
  
  const Map & get_attrs () const { return GetMap(1); };
  void set_attrs (const Map &p) { SetField(1, p); };
  const Generic & get_text () const { return GetField(2); };
  void set_text (const Generic &p) { SetField(2, p); };
};

class TYPE_XMI_CurrentModelProducer : public Record {
public:
  TYPE_XMI_CurrentModelProducer () : Record(TAG_TYPE_XMI_CurrentModelProducer, 2) {}
  TYPE_XMI_CurrentModelProducer &Init (const Map & p1, const Generic & p2)
  { SetField(1, p1); return *this; };
  TYPE_XMI_CurrentModelProducer (const Common &c) : Record(c) {}

  const wchar_t * GetTypeName () const {
    return L"TYPE_XMI_CurrentModelProducer";
  }
  
  const Map & get_attrs () const { return GetMap(1); };
  void set_attrs (const Map &p) { SetField(1, p); };
  const Generic & get_text () const { return GetField(2); };
  void set_text (const Generic &p) { SetField(2, p); };
};

class TYPE_XMI_SortedVersionHistories : public Record {
public:
  TYPE_XMI_SortedVersionHistories () : Record(TAG_TYPE_XMI_SortedVersionHistories, 2) {}
  TYPE_XMI_SortedVersionHistories &Init (const Map & p1, const Sequence & p2)
  { SetField(1, p1); SetField(2, p2); return *this; };
  TYPE_XMI_SortedVersionHistories (const Common &c) : Record(c) {}

  const wchar_t * GetTypeName () const {
    return L"TYPE_XMI_SortedVersionHistories";
  }
  
  const Map & get_attrs () const { return GetMap(1); };
  void set_attrs (const Map &p) { SetField(1, p); };
  const Sequence & get_versionEntry () const { return GetSequence(2); };
  void set_versionEntry (const Sequence &p) { SetField(2, p); };
};

class TYPE_XMI_Import : public Record {
public:
  TYPE_XMI_Import () : Record(TAG_TYPE_XMI_Import, 2) {}
  TYPE_XMI_Import &Init (const Map & p1, const Generic & p2)
  { SetField(1, p1); SetField(2, p2); return *this; };
  TYPE_XMI_Import (const Common &c) : Record(c) {}

  const wchar_t * GetTypeName () const {
    return L"TYPE_XMI_Import";
  }
  
  const Map & get_attrs () const { return GetMap(1); };
  void set_attrs (const Map &p) { SetField(1, p); };
  const Generic & get_data () const { return GetField(2); };
  void set_data (const Sequence &p) { SetField(2, p); };
};

class TYPE_XMI_Metametamodel : public Record {
public:
  TYPE_XMI_Metametamodel () : Record(TAG_TYPE_XMI_Metametamodel, 1) {}
  TYPE_XMI_Metametamodel &Init (const Map & p1)
  { SetField(1, p1); return *this; };
  TYPE_XMI_Metametamodel (const Common &c) : Record(c) {}

  const wchar_t * GetTypeName () const {
    return L"TYPE_XMI_Metametamodel";
  }
  
  const Map & get_attrs () const { return GetMap(1); };
  void set_attrs (const Map &p) { SetField(1, p); };
};

class TYPE_XMI_Metamodel : public Record {
public:
  TYPE_XMI_Metamodel () : Record(TAG_TYPE_XMI_Metamodel, 1) {}
  TYPE_XMI_Metamodel &Init (const Map & p1)
  { SetField(1, p1); return *this; };
  TYPE_XMI_Metamodel (const Common &c) : Record(c) {}

  const wchar_t * GetTypeName () const {
    return L"TYPE_XMI_Metamodel";
  }
  
  const Map & get_attrs () const { return GetMap(1); };
  void set_attrs (const Map &p) { SetField(1, p); };
};

class TYPE_XMI_Model : public Record {
public:
  TYPE_XMI_Model () : Record(TAG_TYPE_XMI_Model, 1) {}
  TYPE_XMI_Model &Init (const Map & p1)
  { SetField(1, p1); return *this; };
  TYPE_XMI_Model (const Common &c) : Record(c) {}

  const wchar_t * GetTypeName () const {
    return L"TYPE_XMI_Model";
  }
  
  const Map & get_attrs () const { return GetMap(1); };
  void set_attrs (const Map &p) { SetField(1, p); };
};

class TYPE_XMI_VersionEntry : public Record {
public:
  TYPE_XMI_VersionEntry () : Record(TAG_TYPE_XMI_VersionEntry, 1) {}
  TYPE_XMI_VersionEntry &Init (const Map & p1)
  { SetField(1, p1); return *this; };
  TYPE_XMI_VersionEntry (const Common &c) : Record(c) {}

  const wchar_t * GetTypeName () const {
    return L"TYPE_XMI_VersionEntry";
  }
  
  const Map & get_attrs () const { return GetMap(1); };
  void set_attrs (const Map &p) { SetField(1, p); };
};

class TYPE_XMI_Difference : public Record {
public:
  TYPE_XMI_Difference () : Record(TAG_TYPE_XMI_Difference, 1) {}
  TYPE_XMI_Difference &Init (const Map & p1)
  { SetField(1, p1); return *this; };
  TYPE_XMI_Difference (const Common &c) : Record(c) {}

  const wchar_t * GetTypeName () const {
    return L"TYPE_XMI_Difference";
  }
  
  const Map & get_attrs () const { return GetMap(1); };
  void set_attrs (const Map &p) { SetField(1, p); };
};

class TYPE_XMI_Extensions : public Record {
public:
  TYPE_XMI_Extensions () : Record(TAG_TYPE_XMI_Extensions, 1) {}
  TYPE_XMI_Extensions &Init (const Map & p1)
  { SetField(1, p1); return *this; };
  TYPE_XMI_Extensions (const Common &c) : Record(c) {}

  const wchar_t * GetTypeName () const {
    return L"TYPE_XMI_Extensions";
  }
  
  const Map & get_attrs () const { return GetMap(1); };
  void set_attrs (const Map &p) { SetField(1, p); };
};

class TYPE_XMI_Document : public Record {
public:
  TYPE_XMI_Document () : Record(TAG_TYPE_XMI_Document, 5) {}
  TYPE_XMI_Document &Init (const Map & p1, const TYPE_XMI_Header & p2, const TYPE_XMI_Content & p3,
                           const Generic &p4, const Generic & p5)
  { SetField(1, p1); SetField(2, p2); SetField(3, p3);
    SetField(4, p4); SetField(5, p5); return *this; };
  TYPE_XMI_Document (const Common &c) : Record(c) {}

  const wchar_t * GetTypeName () const {
    return L"TYPE_XMI_Donument";
  }
  
  const Map & get_attrs() const { return GetMap(1); };
  void set_attrs (const Map &p) { SetField(1, p); };
  TYPE_XMI_Header get_header () const { return GetRecord(2); };
  void set_header (const TYPE_XMI_Header &p) { SetField(2, p); };
  TYPE_XMI_Content get_content () const { return GetRecord(3); };
  void set_content (const TYPE_XMI_Content &p) { SetField(3, p); };
  Generic get_difference () const { return GetField(4); };
  void set_difference (const Generic &p) { SetField(4, p); };
  Generic get_extensions () const { return GetField(5); };
  void set_extensions (const Generic &p) { SetField(5, p); };
};

class TYPE_XMI_Field : public Record {
public:
  TYPE_XMI_Field () : Record(TAG_TYPE_XMI_Field, 2) {}
  TYPE_XMI_Field &Init (const Map & p1, const Generic & p2)
  { SetField(1, p1); SetField(2, p2); return *this; };
  TYPE_XMI_Field (const Common &c) : Record(c) {}

  const wchar_t * GetTypeName () const {
    return L"TYPE_XMI_Field";
  }
  
  const Map & get_attrs() const { return GetMap(1); };
  void set_attrs (const Map &p) { SetField(1, p); };
  const Generic & get_field () const { return GetField(2); };
  void set_field (const Generic &p) { SetField(2, p); };
};

class TYPE_XMI_Extension : public Record {
public:
  TYPE_XMI_Extension () : Record(TAG_TYPE_XMI_Extension, 2) {}
  TYPE_XMI_Extension &Init (const Map & p1, const Sequence & p2)
  { SetField(1, p1); SetField(2, p2); return *this; };
  TYPE_XMI_Extension (const Common &c) : Record(c) {}

  const wchar_t * GetTypeName () const {
    return L"TYPE_XMI_Extension";
  }
  
  const Map & get_attrs() const { return GetMap(1); };
  void set_attrs (const Map &p) { SetField(1, p); };
  const Sequence & get_extension () const { return GetSequence(2); };
  void set_extension (const Sequence &p) { SetField(2, p); };
};

class TYPE_XMI_UMLNamespace : public Record {
public:
  TYPE_XMI_UMLNamespace () : Record(TAG_TYPE_XMI_UMLNamespace, 2) {}
  TYPE_XMI_UMLNamespace &Init (const Map &p1, const Sequence & p2)
  { SetField(1, p1); SetField(2, p2); return *this; };
  TYPE_XMI_UMLNamespace (const Common &c) : Record(c) {}

  const wchar_t * GetTypeName () const {
    return L"TYPE_XMI_UMLNamespace";
  }
  
  const Map & get_attrs () const { return GetMap(1); };
  void set_attrs (const Map &p) { SetField(1, p); };
  const Sequence & get_ownedElement () const { return GetSequence(2); };
  void set_ownedElement (const Sequence &p) { SetField(2, p); };
};

class TYPE_XMI_UMLStereotype : public Record {
public:
  TYPE_XMI_UMLStereotype () : Record(TAG_TYPE_XMI_UMLStereotype, 3) {}
  TYPE_XMI_UMLStereotype &Init (const Map & p1, const Sequence &p2, const Sequence & p3)
  { SetField(1, p1); SetField(2, p2); SetField(3, p3); return *this; };
  TYPE_XMI_UMLStereotype (const Common &c) : Record(c) {}

  const wchar_t * GetTypeName () const {
    return L"TYPE_XMI_UMLStereotype";
  }
  
  const Map & get_attrs() const { return GetMap(1); };
  void set_attrs (const Map &p) { SetField(1, p); };
  const Sequence & get_baseClass () const { return GetSequence(2); };
  void set_baseClass (const Sequence &p) { SetField(2, p); };
  const Sequence & get_extendedElement () const { return GetSequence(3); };
  void set_extendedElement (const Sequence &p) { SetField(3, p); };
};

class TYPE_XMI_UMLModelElement : public Record {
public:
  TYPE_XMI_UMLModelElement () : Record(TAG_TYPE_XMI_UMLModelElement, 7) {}
  TYPE_XMI_UMLModelElement &Init (const Map & p1,
                                  const Sequence & p2,
                                  const Generic & p3,
                                  const Generic & p4,
                                  const Sequence & p5,
                                  const Sequence & p6,
                                  const Sequence & p7)
  { SetField(1, p1); SetField(2, p2); SetField(3, p3);
    SetField(4, p4); SetField(5, p5); SetField(6, p6);
    SetField(7, p7); return *this; };
  TYPE_XMI_UMLModelElement (const Common &c) : Record(c) {}

  const wchar_t * GetTypeName () const {
    return L"TYPE_XMI_UMLModelElement";
  }
  
  const Map & get_attrs () const { return GetMap(1); };
  void set_attrs (const Map &p) { SetField(1, p); };
  const Sequence & get_namespace () const { return GetSequence(2); };
  void set_namespace (const Sequence &p) { SetField(2, p); };
  const Generic & get_visibility () const { return GetField(3); };
  void set_visibility (const Generic &p) { SetField(3, p); };
  const Generic & get_definition () const { return GetField(4); };
  void set_definition (const Generic &p) { SetField(4, p); };
  const Sequence & get_stereotype () const { return GetSequence(5); };
  void set_stereotype (const Sequence &p) { SetField(5, p); };
  const Sequence & get_taggedValue () const { return GetSequence(6); };
  void set_taggedValue (const Sequence &p) { SetField(6, p); };
  const Sequence & get_constraint () const { return GetSequence(7); };
  void set_constraint (const Sequence &p) { SetField(7, p); };
};

class TYPE_XMI_UMLTaggedValue : public Record {
public:
  TYPE_XMI_UMLTaggedValue () : Record(TAG_TYPE_XMI_UMLTaggedValue, 2) {}
  TYPE_XMI_UMLTaggedValue &Init (const Map & p1, const TYPE_XMI_UMLModelElement &p2)
  { SetField(1, p1); SetField(2, p2); return *this; };
  TYPE_XMI_UMLTaggedValue (const Common &c) : Record(c) {}

  const wchar_t * GetTypeName () const {
    return L"TYPE_XMI_UMLTaggedValue";
  }
  
  const Map & get_attrs () const { return GetMap(1); };
  void set_attrs (const Map &p) { SetField(1, p); };
  TYPE_XMI_UMLModelElement get_modelElement () const { return GetRecord(2); };
  void set_modelElement (const TYPE_XMI_UMLModelElement &p) { SetField(2, p); };
};

class TYPE_XMI_UMLModel : public Record {
public:
  TYPE_XMI_UMLModel () : Record(TAG_TYPE_XMI_UMLModel, 3) {}
  TYPE_XMI_UMLModel &Init (const Map &p1,
                           const TYPE_XMI_UMLModelElement &p2,
                           const TYPE_XMI_UMLNamespace & p3)
  { SetField(1, p1); SetField(2, p2); SetField(3, p3); return *this; };
  TYPE_XMI_UMLModel (const Common &c) : Record(c) {}

  const wchar_t * GetTypeName () const {
    return L"TYPE_XMI_UMLModel";
  }
  
  const Map & get_attrs() const { return GetMap(1); };
  void set_attrs (const Map &p) { SetField(1, p); };
  TYPE_XMI_UMLModelElement get_modelElement () const { return GetRecord(2); };
  void set_modelElement (const TYPE_XMI_UMLModelElement &p) { SetField(2, p); };
  TYPE_XMI_UMLNamespace get_namespace () const { return GetRecord(3); };
  void set_namespace (const TYPE_XMI_UMLNamespace &p) { SetField(3, p); };
};

class TYPE_XMI_UMLMultiplicityRange : public Record {
public:
  TYPE_XMI_UMLMultiplicityRange () : Record(TAG_TYPE_XMI_UMLMultiplicityRange, 1) {}
  TYPE_XMI_UMLMultiplicityRange &Init (const Map & p1)
  { SetField(1, p1); return *this; };
  TYPE_XMI_UMLMultiplicityRange (const Common &c) : Record(c) {}

  const wchar_t * GetTypeName () const {
    return L"TYPE_XMI_UMLMultiplicityRange";
  }
  
  const Map & get_attrs() const { return GetMap(1); };
  void set_attrs (const Map &p) { SetField(1, p); };
};

class TYPE_XMI_UMLMultiplicity : public Record {
public:
  TYPE_XMI_UMLMultiplicity () : Record(TAG_TYPE_XMI_UMLMultiplicity, 2) {}
  TYPE_XMI_UMLMultiplicity &Init (const Map & p1, const Generic & p2)
  { SetField(1, p1); SetField(2, p2); return *this; };
  TYPE_XMI_UMLMultiplicity (const Common &c) : Record(c) {}

  const wchar_t * GetTypeName () const {
    return L"TYPE_XMI_UMLMultiplicity";
  }
  
  const Map & get_attrs() const { return GetMap(1); };
  void set_attrs (const Map &p) { SetField(1, p); };
  const Generic & get_range () const { return GetField (2); };
  void set_range (const TYPE_XMI_UMLMultiplicityRange &p) { SetField(2, p); };
};

class TYPE_XMI_UMLPackage : public Record {
public:
  TYPE_XMI_UMLPackage () : Record(TAG_TYPE_XMI_UMLPackage, 3) {}
  TYPE_XMI_UMLPackage &Init (const Map & p1,
                             const TYPE_XMI_UMLModelElement & p2,
                             const TYPE_XMI_UMLNamespace & p3)
  { SetField(1, p1); SetField(2, p2); SetField(3, p3); return *this; };
  TYPE_XMI_UMLPackage (const Common &c) : Record(c) {}

  const wchar_t * GetTypeName () const {
    return L"TYPE_XMI_UMLPackage";
  }
  
  const Map & get_attrs() const { return GetMap(1); };
  void set_attrs (const Map &p) { SetField(1, p); };
  TYPE_XMI_UMLModelElement get_modelElement () const { return GetRecord(2); };
  void set_modelElement (const TYPE_XMI_UMLModelElement &p) { SetField(2, p); };
  TYPE_XMI_UMLNamespace get_namespace () const { return GetRecord(3); };
  void set_namespace (const TYPE_XMI_UMLNamespace &p) { SetField(3, p); };
};

class TYPE_XMI_UMLClassifier : public Record {
public:
  TYPE_XMI_UMLClassifier () : Record(TAG_TYPE_XMI_UMLClassifier, 2) {}
  TYPE_XMI_UMLClassifier &Init (const Map & p1, const Sequence & p2)
  { SetField(1, p1); SetField(2, p2); return *this; };
  TYPE_XMI_UMLClassifier (const Common &c) : Record(c) {}

  const wchar_t * GetTypeName () const {
    return L"TYPE_XMI_UMLClassifier";
  }
  
  const Map & get_attrs() const { return GetMap(1); };
  void set_attrs (const Map &p) { SetField(1, p); };
  const Sequence & get_feature() const { return GetSequence(2); };
  void set_feature (const Sequence &p) { SetField(2, p); };
};

class TYPE_XMI_UMLGeneralizableElement : public Record {
public:
  TYPE_XMI_UMLGeneralizableElement () : Record(TAG_TYPE_XMI_UMLGeneralizableElement, 2) {}
  TYPE_XMI_UMLGeneralizableElement &Init (const Map & p1, const Sequence & p2)
  { SetField(1, p1); SetField(2, p2); return *this; };
  TYPE_XMI_UMLGeneralizableElement (const Common &c) : Record(c) {}
 
  const wchar_t * GetTypeName () const {
    return L"TYPE_XMI_UMLGeneralizableElement";
  }
  
  const Map & get_attrs() const { return GetMap(1); };
  void set_attrs (const Map &p) { SetField(1, p); };
  const Sequence & get_generalization() const { return GetSequence(2); };
  void set_generalization (const Sequence &p) { SetField(2, p); };
};

class TYPE_XMI_UMLClass : public Record {
public:
  TYPE_XMI_UMLClass () : Record(TAG_TYPE_XMI_UMLClass, 5) {}
  TYPE_XMI_UMLClass &Init (const Map & p1,
                           const TYPE_XMI_UMLModelElement & p2,
                           const TYPE_XMI_UMLNamespace & p3,
                           const TYPE_XMI_UMLGeneralizableElement & p4,
                           const Generic & p5)
  { SetField(1, p1); SetField(2, p2); SetField(3, p3);
    SetField(4, p4); SetField(5, p5); return *this; };
  TYPE_XMI_UMLClass (const Common &c) : Record(c) {}

  const wchar_t * GetTypeName () const {
    return L"TYPE_XMI_UMLClass";
  }
  
  const Map & get_attrs() const { return GetMap(1); };
  void set_attrs (const Map &p) { SetField(1, p); };
  TYPE_XMI_UMLModelElement get_modelElement () const { return GetRecord(2); };
  void set_modelElement (const TYPE_XMI_UMLModelElement &p) { SetField(2, p); };
  TYPE_XMI_UMLNamespace get_namespace () const { return GetRecord(3); };
  void set_namespace (const TYPE_XMI_UMLNamespace &p) { SetField(3, p); };
  TYPE_XMI_UMLGeneralizableElement get_generalizableElement () const { return GetRecord(4); };
  void set_generalizableElement (const TYPE_XMI_UMLGeneralizableElement &p) { SetField(4, p); };
  const Generic & get_classifier () const { return GetField(5); };
  void set_classifier (const Generic &p) { SetField(5, p); };
};

class TYPE_XMI_UMLInterface : public Record {
public:
  TYPE_XMI_UMLInterface () : Record(TAG_TYPE_XMI_UMLInterface, 2) {}
  TYPE_XMI_UMLInterface &Init (const Map & p1, const TYPE_XMI_UMLModelElement &p2)
  { SetField(1, p1); SetField(2, p2); return *this; };
  TYPE_XMI_UMLInterface (const Common &c) : Record(c) {}

  const wchar_t * GetTypeName () const {
    return L"TYPE_XMI_UMLInterface";
  }
  
  const Map & get_attrs() const { return GetMap(1); };
  void set_attrs (const Map &p) { SetField(1, p); };
  TYPE_XMI_UMLModelElement get_modelElement () const { return GetRecord(2); };
  void set_modelElement (const TYPE_XMI_UMLModelElement &p) { SetField(2, p); };
};

class TYPE_XMI_UMLPrimitive : public Record {
public:
  TYPE_XMI_UMLPrimitive () : Record(TAG_TYPE_XMI_UMLPrimitive, 2) {}
  TYPE_XMI_UMLPrimitive &Init (const Map &p1, const Sequence & p2)
  { SetField(1, p1); SetField(2, p2); return *this; };
  TYPE_XMI_UMLPrimitive (const Common &c) : Record(c) {}

  const wchar_t * GetTypeName () const {
    return L"TYPE_XMI_UMLPrimitive";
  }
  
  const Map & get_attrs() const { return GetMap(1); };
  void set_attrs (const Map &p) { SetField(1, p); };
  const Sequence & get_primitive () const { return GetSequence (2); };
  void set_primitive (const Sequence &p) { SetField(2, p); };
};

class TYPE_XMI_UMLBehavioralFeature : public Record {
public:
  TYPE_XMI_UMLBehavioralFeature () : Record(TAG_TYPE_XMI_UMLBehavioralFeature, 2) {}
  TYPE_XMI_UMLBehavioralFeature &Init (const Map &p1, const Sequence & p2)
  { SetField(1, p1); SetField(2, p2); return *this; };
  TYPE_XMI_UMLBehavioralFeature (const Common &c) : Record(c) {}

  const wchar_t * GetTypeName () const {
    return L"TYPE_XMI_UMLBehavioralFeature";
  }
  
  const Map & get_attrs () const { return GetMap (1); };
  void set_attrs (const Map &p) { SetField(1, p); };
  const Sequence & get_parameter () const { return GetSequence(2); };
  void set_parameter  (const Sequence &p) { SetField(2, p); };
};

class TYPE_XMI_UMLParameter : public Record {
public:
  TYPE_XMI_UMLParameter () : Record(TAG_TYPE_XMI_UMLParameter, 5) {}
  TYPE_XMI_UMLParameter &Init (const Map & p1,
                               const TYPE_XMI_UMLModelElement &p2,
                               const Generic &p3,
                               const Generic &p4,
                               const Generic &p5)
  { SetField(1, p1); SetField(2, p2); SetField(3, p3);
    SetField(4, p4); SetField(5, p5); return *this; };
  TYPE_XMI_UMLParameter (const Common &c) : Record(c) {}

  const wchar_t * GetTypeName () const {
    return L"TYPE_XMI_UMLParameter";
  }
  
  const Map & get_attrs() const { return GetMap(1); };
  void set_attrs (const Map &p) { SetField(1, p); };
  TYPE_XMI_UMLModelElement get_modelElement () const { return GetField (2); };
  void set_modelElement (const TYPE_XMI_UMLModelElement &p) { SetField(2, p); };
  const Generic & get_behavioralFeature () const { return GetField (3); };
  void set_behavioralFeature (const TYPE_XMI_UMLBehavioralFeature &p) { SetField(3, p); };
  const Generic & get_type () const { return GetField (4); };
  void set_type (const TYPE_XMI_UMLClassifier &p) { SetField(4, p); };
  const Generic & get_defaultValue () const { return GetField (5); };
  void set_defaultValue (const Generic &p) { SetField(5, p); };
};

class TYPE_XMI_UMLFeature : public Record {
public:
  TYPE_XMI_UMLFeature () : Record(TAG_TYPE_XMI_UMLFeature, 3) {}
  TYPE_XMI_UMLFeature &Init (const Map & p1,
                             const Generic &p2,
                             const Generic &p3)
  { SetField(1, p1); SetField(2, p2); SetField(3, p3); return *this; };
  TYPE_XMI_UMLFeature (const Common &c) : Record(c) {}

  const wchar_t * GetTypeName () const {
    return L"TYPE_XMI_UMLFeature";
  }
  
  const Map & get_attrs() const { return GetMap(1); };
  void set_attrs (const Map &p) { SetField(1, p); };
  const Generic & get_owner () const { return GetField (2); };
  void set_owner (const TYPE_XMI_UMLClassifier &p) { SetField(2, p); };
  const Generic & get_visibility () const { return GetField (3); };
  void set_visibility (const Generic &p) { SetField(3, p); };
};

class TYPE_XMI_UMLOperation : public Record {
public:
  TYPE_XMI_UMLOperation () : Record(TAG_TYPE_XMI_UMLOperation, 4) {}
  TYPE_XMI_UMLOperation &Init (const Map & p1,
                               const TYPE_XMI_UMLModelElement &p2,
                               const TYPE_XMI_UMLFeature &p3,
                               const TYPE_XMI_UMLBehavioralFeature &p4)
  { SetField(1, p1); SetField(2, p2); SetField(3, p3); SetField(4, p4); return *this; };
  TYPE_XMI_UMLOperation (const Common &c) : Record(c) {}

  const wchar_t * GetTypeName () const {
    return L"TYPE_XMI_UMLOperation";
  }
  
  const Map & get_attrs() const { return GetMap(1); };
  void set_attrs (const Map &p) { SetField(1, p); };
  TYPE_XMI_UMLModelElement get_modelElement () const { return GetField (2); };
  void set_modelElement (const TYPE_XMI_UMLModelElement &p) { SetField(2, p); };
  TYPE_XMI_UMLFeature get_feature () const { return GetRecord (3); };
  void set_feature (const TYPE_XMI_UMLFeature &p) { SetField(3, p); };
  TYPE_XMI_UMLBehavioralFeature get_behavioralFeature () const { return GetField (4); };
  void set_behavioralFeature (const TYPE_XMI_UMLBehavioralFeature &p) { SetField(4, p); };
};

class TYPE_XMI_UMLGeneralization : public Record {
public:
  TYPE_XMI_UMLGeneralization () : Record(TAG_TYPE_XMI_UMLGeneralization, 5) {}
  TYPE_XMI_UMLGeneralization &Init (const Map & p1,
                                    const TYPE_XMI_UMLModelElement &p2,
                                    const Generic &p3,
                                    const Generic &p4,
                                    const Generic &p5)
  { SetField(1, p1); SetField(2, p2); SetField(3, p3);
    SetField(4, p4); SetField(5, p5); return *this; };
  TYPE_XMI_UMLGeneralization (const Common &c) : Record(c) {}

  const wchar_t * GetTypeName () const {
    return L"TYPE_XMI_UMLGeneralization";
  }
  
  const Map & get_attrs() const { return GetMap(1); };
  void set_attrs (const Map &p) { SetField(1, p); };
  TYPE_XMI_UMLModelElement get_modelElement () const { return GetField (2); };
  void set_modelElement (const TYPE_XMI_UMLModelElement &p) { SetField(2, p); };
  const Generic & get_child () const { return GetField(3); };
  void set_child (const TYPE_XMI_UMLGeneralizableElement &p) { SetField(3, p); };
  const Generic & get_parent () const { return GetField(4); };
  void set_parent (const TYPE_XMI_UMLGeneralizableElement &p) { SetField(4, p); };
  const Generic & get_flag () const { return GetField (5); };
  void set_flag (const Generic &p) { SetField(5, p); };
};

class TYPE_XMI_UMLStructuralFeature : public Record {
public:
  TYPE_XMI_UMLStructuralFeature () : Record(TAG_TYPE_XMI_UMLStructuralFeature, 3) {}
  TYPE_XMI_UMLStructuralFeature &Init (const Map & p1, const Generic &p2, const Generic &p3)
  { SetField(1, p1); SetField(2, p2); SetField(3, p3); return *this; };
  TYPE_XMI_UMLStructuralFeature (const Common &c) : Record(c) {}

  const wchar_t * GetTypeName () const {
    return L"TYPE_XMI_UMLStructuralFeature";
  }
  
  const Map & get_attrs() const { return GetMap(1); };
  void set_attrs (const Map &p) { SetField(1, p); };
  const Generic & get_multiplicity () const { return GetField (2); };
  void set_multiplicity (const TYPE_XMI_UMLMultiplicity &p) { SetField(2, p); };
  const Generic & get_type () const { return GetField (3); };
  void set_type (const TYPE_XMI_UMLClassifier &p) { SetField(3, p); };
};

class TYPE_XMI_UMLAssociation : public Record {
public:
  TYPE_XMI_UMLAssociation () : Record(TAG_TYPE_XMI_UMLAssociation, 3) {}
  TYPE_XMI_UMLAssociation &Init (const Map & p1,
                                    const TYPE_XMI_UMLModelElement &p2,
                                    const Sequence &p3)
  { SetField(1, p1); SetField(2, p2); SetField(3, p3); return *this; };
  TYPE_XMI_UMLAssociation (const Common &c) : Record(c) {}

  const wchar_t * GetTypeName () const {
    return L"TYPE_XMI_UMLAssociation";
  }
  
  const Map & get_attrs() const { return GetMap(1); };
  void set_attrs (const Map &p) { SetField(1, p); };
  TYPE_XMI_UMLModelElement get_modelElement () const { return GetField (2); };
  void set_modelElement (const TYPE_XMI_UMLModelElement &p) { SetField(2, p); };
  const Sequence & get_connection () const { return GetSequence (3); };
  void set_connection (const Sequence &p) { SetField(3, p); };
};

class TYPE_XMI_UMLAssociationEnd : public Record {
public:
  TYPE_XMI_UMLAssociationEnd () : Record(TAG_TYPE_XMI_UMLAssociationEnd, 9) {}
  TYPE_XMI_UMLAssociationEnd &Init (const Map & p1,
                                    const TYPE_XMI_UMLModelElement &p2,
                                    const TYPE_XMI_UMLFeature &p3,
                                    const TYPE_XMI_UMLStructuralFeature &p4,
                                    const Generic & p5,
                                    const Generic & p6,
                                    const Generic & p7,
                                    const Generic & p8,
                                    const Generic & p9)
  { SetField(1, p1); SetField(2, p2); SetField(3, p3);
    SetField(4, p4); SetField(5, p5); SetField(6, p6);
    SetField(7, p7); SetField(8, p8); SetField(9, p9); return *this; };
  TYPE_XMI_UMLAssociationEnd (const Common &c) : Record(c) {}

  const wchar_t * GetTypeName () const {
    return L"TYPE_XMI_UMLAssociationEnd";
  }
  
  const Map & get_attrs() const { return GetMap(1); };
  void set_attrs (const Map &p) { SetField(1, p); };
  TYPE_XMI_UMLModelElement get_modelElement () const { return GetField (2); };
  void set_modelElement (const TYPE_XMI_UMLModelElement &p) { SetField(2, p); };
  TYPE_XMI_UMLFeature get_feature () const { return GetSequence (3); };
  void set_feature (const TYPE_XMI_UMLFeature &p) { SetField(3, p); };
  TYPE_XMI_UMLStructuralFeature get_structuralFeature () const { return GetSequence (4); };
  void set_structuralFeature (const TYPE_XMI_UMLStructuralFeature &p) { SetField(4, p); };
  const Generic & get_participant () const { return GetField (5); };
  void set_participant (const TYPE_XMI_UMLClassifier &p) { SetField(5, p); };
  const Generic & get_initialValue () const { return GetField (6); };
  void set_initialValue (const Generic &p) { SetField(6, p); };
  const Generic & get_association () const { return GetField (7); };
  void set_association (const TYPE_XMI_UMLAssociation &p) { SetField(7, p); };
  const Generic & get_visibility () const { return GetField (8); };
  void set_visibility (const Generic &p) { SetField(8, p); };
  const Generic & get_qualifier () const { return GetField (9); };
  void set_qualifier (const Generic &p) { SetField(9, p); };
};

class TYPE_XMI_UMLAttribute : public Record {
public:
  TYPE_XMI_UMLAttribute () : Record(TAG_TYPE_XMI_UMLAttribute, 6) {}
  TYPE_XMI_UMLAttribute &Init (const Map & p1,
                               const TYPE_XMI_UMLModelElement &p2,
                               const TYPE_XMI_UMLFeature &p3,
                               const TYPE_XMI_UMLStructuralFeature &p4,
                               const Generic & p5,
                               const Generic & p6)
  { SetField(1, p1); SetField(2, p2); SetField(3, p3);
    SetField(4, p4); SetField(5, p5); SetField(6, p6); return *this; };
  TYPE_XMI_UMLAttribute (const Common &c) : Record(c) {}

  const wchar_t * GetTypeName () const {
    return L"TYPE_XMI_UMLAttribute";
  }
  
  const Map & get_attrs() const { return GetMap(1); };
  void set_attrs (const Map &p) { SetField(1, p); };
  TYPE_XMI_UMLModelElement get_modelElement () const { return GetField (2); };
  void set_modelElement (const TYPE_XMI_UMLModelElement &p) { SetField(2, p); };
  TYPE_XMI_UMLFeature get_feature () const { return GetRecord (3); };
  void set_feature (const TYPE_XMI_UMLFeature &p) { SetField(3, p); };
  TYPE_XMI_UMLStructuralFeature get_structuralFeature () const { return GetRecord (4); };
  void set_structuralFeature (const TYPE_XMI_UMLStructuralFeature &p) { SetField(4, p); };
  const Generic & get_initialValue () const { return GetField (5); };
  void set_initialValue (const Generic &p) { SetField(5, p); };
  const Generic & get_associationEnd () const { return GetField (6); };
  void set_associationEnd (const Generic &p) { SetField(6, p); };
};

class TYPE_XMI_UMLConstraint : public Record {
public:
  TYPE_XMI_UMLConstraint () : Record(TAG_TYPE_XMI_UMLConstraint, 4) {}
  TYPE_XMI_UMLConstraint &Init (const Map & p1,
                                const TYPE_XMI_UMLModelElement &p2,
                                const Generic &p3,
                                const Generic &p4)
  { SetField(1, p1); SetField(2, p2); SetField(3, p3);
    SetField(4, p4); return *this; };
  TYPE_XMI_UMLConstraint (const Common &c) : Record(c) {}

  const wchar_t * GetTypeName () const {
    return L"TYPE_XMI_UMLConstraint";
  }
  
  const Map & get_attrs() const { return GetMap(1); };
  void set_attrs (const Map &p) { SetField(1, p); };
  TYPE_XMI_UMLModelElement get_modelElement () const { return GetField (2); };
  void set_modelElement (const TYPE_XMI_UMLModelElement &p) { SetField(2, p); };
  Generic get_body () const { return GetField(3); };
  void set_body (const TYPE_XMI_UMLFeature &p) { SetField(3, p); };
  Generic get_constrainedElement () const { return GetField(4); };
  void set_constrainedElement (const Generic &p) { SetField(4, p); };
};

class TYPE_XMI_UMLExpression : public Record {
public:
  TYPE_XMI_UMLExpression () : Record(TAG_TYPE_XMI_UMLExpression, 2) {}
  TYPE_XMI_UMLExpression &Init (const Map & p1, const Generic & p2)
  { SetField(1, p1); SetField(2, p2); return *this; };
  TYPE_XMI_UMLExpression (const Common &c) : Record(c) {}

  const wchar_t * GetTypeName () const {
    return L"TYPE_XMI_UMLExpression";
  }
  
  const Map & get_attrs() const { return GetMap(1); };
  void set_attrs (const Map &p) { SetField(1, p); };
  const Generic & get_body () const { return GetField (2); };
  void set_body (const Generic &p) { SetField(2, p); };
};

class TYPE_XMI_UMLBooleanExpression : public Record {
public:
  TYPE_XMI_UMLBooleanExpression () : Record(TAG_TYPE_XMI_UMLBooleanExpression, 2) {}
  TYPE_XMI_UMLBooleanExpression &Init (const Map & p1, const Generic & p2)
  { SetField(1, p1); SetField(2, p2); return *this; };
  TYPE_XMI_UMLBooleanExpression (const Common &c) : Record(c) {}

  const wchar_t * GetTypeName () const {
    return L"TYPE_XMI_UMLBooleanExpression";
  }
  
  const Map & get_attrs() const { return GetMap(1); };
  void set_attrs (const Map &p) { SetField(1, p); };
  const Generic & get_body () const { return GetField (2); };
  void set_body (const Generic &p) { SetField(2, p); };
};

class TYPE_XMI_UMLDataType : public Record {
public:
  TYPE_XMI_UMLDataType () : Record(TAG_TYPE_XMI_UMLDataType, 1) {}
  TYPE_XMI_UMLDataType &Init (const Map & p1)
  { SetField(1, p1); return *this; };
  TYPE_XMI_UMLDataType (const Common &c) : Record(c) {}

  const wchar_t * GetTypeName () const {
    return L"TYPE_XMI_UMLDataType";
  }
  
  const Map & get_attrs() const { return GetMap(1); };
  void set_attrs (const Map &p) { SetField(1, p); };
};

class TYPE_XMI_UMLDiagramElement : public Record {
public:
  TYPE_XMI_UMLDiagramElement () : Record(TAG_TYPE_XMI_UMLDiagramElement, 1) {}
  TYPE_XMI_UMLDiagramElement &Init (const Map & p1)
  { SetField(1, p1); return *this; };
  TYPE_XMI_UMLDiagramElement (const Common &c) : Record(c) {}

  const wchar_t * GetTypeName () const {
    return L"TYPE_XMI_UMLDiagramElement";
  }
  
  const Map & get_attrs() const { return GetMap(1); };
  void set_attrs (const Map &p) { SetField(1, p); };
};

class TYPE_XMI_UMLDiagram : public Record {
public:
  TYPE_XMI_UMLDiagram () : Record(TAG_TYPE_XMI_UMLDiagram, 3) {}
  TYPE_XMI_UMLDiagram &Init (const Map & p1, const TYPE_XMI_UMLModelElement & p2,
                             const Sequence & p3)
  { SetField(1, p1); SetField(2, p2); SetField(3, p3); return *this; };
  TYPE_XMI_UMLDiagram (const Common &c) : Record(c) {}

  const wchar_t * GetTypeName () const {
    return L"TYPE_XMI_UMLDiagram";
  }
  
  const Map & get_attrs() const { return GetMap(1); };
  void set_attrs (const Map &p) { SetField(1, p); };
  TYPE_XMI_UMLModelElement get_modelElement () const { return GetField (2); };
  void set_modelElement (const TYPE_XMI_UMLModelElement &p) { SetField(2, p); };
  const Sequence & get_element() const { return GetSequence(3); };
  void set_element (const Sequence &p) { SetField(3, p); };
};

class TYPE_XMI_EAStub : public Record {
public:
  TYPE_XMI_EAStub () : Record(TAG_TYPE_XMI_EAStub, 1) {}
  TYPE_XMI_EAStub &Init (const Map & p1)
  { SetField(1, p1); return *this; };
  TYPE_XMI_EAStub (const Common &c) : Record(c) {}

  const wchar_t * GetTypeName () const {
    return L"TYPE_XMI_EAStub";
  }
  
  const Map & get_attrs () const { return GetMap(1); };
  void set_attrs (const Map &p) { SetField(1, p); };
};

void init_XMI();

#endif // __XMI_h__
