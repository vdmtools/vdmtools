#ifndef __Character_h__
#define __Character_h__

#include <dlclass.h>
#include <metaiv.h>
#include <string>

class vdm_Character: public DlClass {

public:
  static const wchar_t * name;

protected:
  wchar_t value;

private:
  static vdm_Character *GetDLPtr(const DLObject& obj);

public:
  Generic DlMethodCall (const wchar_t* name, const Sequence &p);

  vdm_Character ();
  vdm_Character (const Sequence &);

  const vdm_Character& operator=(const vdm_Character &s);
  virtual Char vdm_charValue ();
  virtual Int vdm_compareTo (const Sequence &);
  virtual Int vdm_hashCode ();
  virtual Sequence vdm_toStringVDM ();
  virtual Int vdm_digit(const Sequence &);
  virtual Bool vdm_isDigit(const Sequence &);
  virtual Bool vdm_isLetter(const Sequence &);
  virtual Bool vdm_isLetterOrDigit(const Sequence &);
  virtual Bool vdm_isSpaceChar(const Sequence &);
  virtual Bool vdm_isWhiteSpace(const Sequence &);
};

#endif // __Character_h__
