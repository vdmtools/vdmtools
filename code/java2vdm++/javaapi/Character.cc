#include "Character.h"

#include <math.h>
//#include <ctype.h>
#include <metaiv.h>

#include <string>
//using namespace std;

const wchar_t* vdm_Character::name = L"JavaLangCharacter";

vdm_Character *vdm_Character::GetDLPtr(const DLObject &obj) {
        vdm_Character *objPtr = (vdm_Character*) obj.GetPtr();
        return objPtr;
}

Generic vdm_Character::DlMethodCall (const wchar_t* name, const Sequence &p) {
  Generic res;
  if (!wcscmp(name, L"JavaLangCharacter'")) {
    if (p.Length()==1) {
      *this=vdm_Character(p);
//      DLObject res(L"Character", this);
//      return res;
    }
  } else if (!wcscmp(name,L"charValue") && p.Length() == 0) {
    res = this->vdm_charValue();
  } else if (!wcscmp(name,L"compareTo'") && p.Length() == 1) {
    res = this->vdm_compareTo(p);
  } else if (!wcscmp(name,L"hashCode") && p.Length() == 0) {
    res = this->vdm_hashCode();
  } else if (!wcscmp(name,L"toSeqOfChar") && p.Length() == 0) {
    res = this->vdm_toStringVDM();
  } else if (!wcscmp(name,L"digit'") && p.Length() == 2) {
    res = this->vdm_digit(p);
  } else if (!wcscmp(name,L"isDigit'") && p.Length() == 1) {
    res = this->vdm_isDigit(p);
  } else if (!wcscmp(name,L"isLetter'") && p.Length() == 1) {
    res = this->vdm_isLetter(p);
  } else if (!wcscmp(name,L"isLetterOrDigit'") && p.Length() == 1) {
    res = this->vdm_isLetterOrDigit(p);
  } else if (!wcscmp(name,L"isSpaceChar'") && p.Length() == 1) {
    res = this->vdm_isSpaceChar(p);
  } else if (!wcscmp(name,L"isWhiteSpace'") && p.Length() == 1) {
    res = this->vdm_isWhiteSpace(p);
  } else {
    throw 1;
  }
  return res;
}

vdm_Character::vdm_Character () {
  value = 0;
  return;
}

vdm_Character::vdm_Character (const Sequence &p) {
  if (p[1].IsChar()) {
    value = ((Char) p[1]).GetValue();
  } else {
    value = 0;
  }
}

const vdm_Character& vdm_Character::operator=(const vdm_Character &s) {
  value = s.value;
  return *this;
}

Char vdm_Character::vdm_charValue() {
        return Char(value);
}

Int vdm_Character::vdm_compareTo (const Sequence &p) {
  int res;
  vdm_Character *argChar = GetDLPtr(p[1]);
  res = value - argChar->value;
  return Int(res);
}

Int vdm_Character::vdm_hashCode () {
        return Int(value);
}

Sequence vdm_Character::vdm_toStringVDM() {
  Sequence res;
  res.ImpAppend(Char(value));
  return res;
}

Int vdm_Character::vdm_digit(const Sequence &p) {
  wchar_t ch = ((Char) p[1]).GetValue();
  int v;
  int rdx = ((Int) p[2]).GetValue();
  if (rdx < 2 || rdx > 36) {
    return Int(-1);
  } else if (ch >= 48 && ch <= 57) {
    v = ch - 48;
    if (v < rdx) {
      return Int(v);
    } else {
      return Int(-1);
    }
  } else if (ch >= 65 && ch <= 90) {
    v = ch - 55;
    if (v < rdx) {
      return Int(v);
    } else {
      return Int(-1);
    }
  } else if (ch >= 97 && ch <= 122) {
    v = ch - 87;
    if (v < rdx) {
      return Int(v);
    } else {
      return Int(-1);
    }
  } else {
    return Int(-1);
  }
}

Bool vdm_Character::vdm_isDigit(const Sequence &p) {
  wchar_t v = ((Char) p[1]).GetValue();
  if (isdigit(v))
    return Bool(true);
  else
    return Bool(false);
}

Bool vdm_Character::vdm_isLetter(const Sequence &p) {
  wchar_t v = ((Char) p[1]).GetValue();
  if (isalpha(v))
    return Bool(true);
  else
    return Bool(false);
}

Bool vdm_Character::vdm_isLetterOrDigit(const Sequence &p) {
  wchar_t v = ((Char) p[1]).GetValue();
  if (isalnum(v))
    return Bool(true);
  else
    return Bool(false);
}

Bool vdm_Character::vdm_isSpaceChar(const Sequence &p) {
  wchar_t v = ((Char) p[1]).GetValue();
  if (isspace(v))
    return Bool(true);
  else
    return Bool(false);
}

Bool vdm_Character::vdm_isWhiteSpace(const Sequence &p) {
  wchar_t v = ((Char) p[1]).GetValue();
  switch(v) {
    case '\011': // HORIZONTAL TABULATION.
    case '\012': // LINE FEED.
    case '\013': // VERTICAL TABULATION.
    case '\014': // FORM FEED.
    case '\015': // CARRIAGE RETURN.
    case '\034': // FILE SEPARATOR.
    case '\035': // GROUP SEPARATOR.
    case '\036': // RECORD SEPARATOR.
    case '\037': // UNIT SEPARATOR.
      return Bool(true);
  }
//  if (isblank(v))
//    return Bool(true);
//  else
  return Bool(false);
}

