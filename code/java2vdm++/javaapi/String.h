#ifndef __String_h__
#define __String_h__

#include "String_anonym.h"

#include <dlclass.h>
//#include <m_apm.h>
#include <metaiv.h>

#include <string>
//using namespace std;

class vdm_String: public DlClass {

  friend class vdm_StringBuffer;
  
  public:
  static const wchar_t * name;

  protected:
  wstring value;

  private:
  static vdm_String* GetDLPtr(const DLObject& obj);

  public:
  Generic DlMethodCall (const wchar_t* name, const Sequence &p);

  virtual const wchar_t * GetTypeName () const { return vdm_String::name; }

  vdm_String ();
  vdm_String (const wstring& );
  vdm_String (const type_cL &seqOfChar);
  vdm_String (const wchar_t* str);
  vdm_String (const vdm_String &);

  const vdm_String& operator=(const vdm_String &s);

  virtual Int vdm_compareTo (const Sequence &);
//  virtual DLObject vdm_concat (const Sequence &);
  virtual void vdm_concat (const Sequence &);
  virtual Int vdm_hashCode ();
  virtual Int vdm_indexOf(const Sequence &);
  virtual Int vdm_indexOfcLI(const Sequence &);
  virtual Int vdm_lastIndexOf(const Sequence &);
  virtual Int vdm_lastIndexOfcLI(const Sequence &);
  virtual Int vdm_length ();
  virtual DLObject vdm_replace (const Sequence &);
  virtual type_icM vdm_toCharArray ();
  virtual DLObject vdm_toLowerCase ();
  virtual Sequence vdm_toStringVDM ();
  virtual DLObject vdm_toUpperCase ();
  virtual Char vdm_charAt (const Int &vdm_index);
  virtual DLObject vdm_valueOfBool(const Sequence &);
  virtual DLObject vdm_valueOfChar(const Sequence &);
  virtual DLObject vdm_valueOfSeqOfChar(const Sequence &);
  virtual DLObject vdm_valueOfInt(const Sequence &);
  virtual DLObject vdm_valueOfReal(const Sequence &);

  wstring getValue() const { return value; }

  friend class vdm_J2VUTIL;
  friend class vdm_Integer;
  friend class vdm_FileOutputStream;
  friend class vdm_FileInputStream;
  friend class vdm_DriverManager;
  friend class vdm_SqlStatement;
  friend class vdm_SqlResultSet;
//  friend class vdm_StringBuffer;
  friend class vdm_File;

};

#endif // __String_h__
