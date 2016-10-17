#ifndef __StringBuffer_h__
#define __StringBuffer_h__

#include "StringBuffer_anonym.h"

#include <dlclass.h>
#include <metaiv.h>
#include <string>

//using namespace std;

//#include L"metaiv.h"
//#include L"Object.h"

class vdm_StringBuffer: public DlClass
{
public:
  static const wchar_t * name;

protected:
  int buffer_size;
  wchar_t *strbuffer;

public:
  Generic DlMethodCall (const wchar_t* name, const Sequence &p);

  vdm_StringBuffer ();
  vdm_StringBuffer (const Int &capacity);
  vdm_StringBuffer (const type_cL &seqOfChar);
  vdm_StringBuffer (const wstring &aString);
  ~vdm_StringBuffer ();

  const vdm_StringBuffer& operator= (const vdm_StringBuffer &s);

//  virtual DLObject vdm_appendMICIntInt(const type_icM &, const Int &, const Int &);
  virtual DLObject vdm_append(const Sequence &);  
  virtual Int vdm_capacity();
  virtual DLObject vdm_deleteCharAt(const Sequence &);
  virtual void vdm_ensureCapacity(const Sequence &);
  virtual DLObject vdm_insert(const Sequence &);
  virtual Int vdm_length();

  virtual Sequence vdm_toStringVDM();

private:
  static vdm_StringBuffer* GetDLPtr(const DLObject& obj);
};

#endif // __StringBuffer_h__
