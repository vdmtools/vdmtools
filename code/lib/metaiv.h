/***
*  * WHAT
*  *    VDM library interface definitions
*  * ID
*  *    $Id: metaiv.h,v 1.91 2006/06/14 06:15:53 vdmtools Exp $
*  * PROJECT
*  *    Toolbox
*  * COPYRIGHT
*  *    (C) Kyushu University
***/

#ifndef __metaiv_h__
#define __metaiv_h__

#include <stdio.h>
#include <stddef.h>
#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <typeinfo>
#if __cplusplus >= 201103L
#include <unordered_map>
#endif
using namespace std;

#ifdef __linux__
#include "stdint.h"
#endif // __linux__

#ifdef _MSC_VER
typedef __int64 int64_t;
typedef unsigned __int64 uint64_t;
#endif // _MSC_VER

#ifndef VDMLIBVERSION
#define VDMLIBVERSION 102 // Major=VDMLIBVERSION/100 Minor=VDMLIBVERSION % 100
#endif // VDMLIBVERSION

#include "m4lib_errmsg.h"

std::wstring string2wstring(const std::string & s);
std::string wstring2string(const std::wstring & ws);
std::string wstring2coutstr(const std::wstring & ws);
std::string wstring2fsstr(const std::wstring & ws);

#ifdef ML_TRACE
extern wofstream* getM4trace();
extern int getM4DebugLevel();
extern void setM4DebugLevel(int i);
#endif //ML_TRACE

/**************************************************************************
*  Forward declaration of classes.
**************************************************************************/
class Common;
class Int;
class Map;
class Bool;
class Char;
class Quote;
class Text;
class Token;
class Real;
class Sequence;
class Record;
class Tuple;
class Set;
class Nil;
class ObjectRef;
class ObjectRefVal;
class MetaivVal;
class DRPtr;
class Time;
#ifdef VDM_FINGERPRINT
class Fingerprint;
#endif // VDM_FINGERPRINT

class VDMFormatter
{
private:
  long int flags;
  int indent;
  int indent_step;
public:
  enum Flags
  {
    prettyprint = 1,
    doublequoteontoken = 2
  };
  VDMFormatter() : flags(0), indent(0), indent_step(2) {}
  void SetIndentStep(int i) { this->indent_step = i; }
  void SetIndent(int i)     { this->indent = i; }
  void IncrementIndent(int i = 0) { this->indent += (i>0) ? i : this->indent_step; }
  int GetIndent() const { return indent; }
  bool GetFlag(long int flag) const { return (this->flags & flag) !=0; }
  bool PrettyPrinting() const { return GetFlag(prettyprint); }
  bool DoubleQuoteOnToken() const { return GetFlag(doublequoteontoken); }
  void SetFlag(long int flag) { this->flags |= flag; }
  void print_spaces(wostream& o, int indent);
};

/***************************************************************************
*
*  MetaivVal
*
*  MetaivVal is the base class for all the metaiv values (IntVal, SetVal ...).
*  It contains the following information:
*
*  BuildCount : contains the total number of existing MetaivVal's.
*  type : contains the type of the value.
*  up : contains a pointer to the actual ..Val class.
*  RefCount : records the number of references to this values.
*
***************************************************************************/

class MetaivVal
{
protected:
/**************************************************************************
*  All metaiv values have a type, which is recorded in MetaivVal.
**************************************************************************/
enum metaivType { mt_container = 100,
                  mt_nil ,
                  mt_char,
                  mt_int,
                  mt_real,
                  mt_quote,
                  mt_tuple,
                  mt_record,
                  mt_set,
                  mt_map,
                  mt_text,
                  mt_token,
                  mt_bool,
                  mt_sequence,
                  mt_objectref,
                  mt_common,
                  mt_generic,
                  mt_undef,
                  mt_dlobject,
                  mt_drptr,
                  mt_time,
                  mt_function
};

private:
  metaivType type;
  int RefCount;

public:
  MetaivVal(metaivType mt) : type(mt), RefCount(0) {
#ifdef SHOW_MALLOC_STATS
    MetaivVal::BuildCount++;
    showmalloccount->addval(type);
#endif // SHOW_MALLOC_STATS
#ifdef VDM_FINGERPRINT
    fp.setvalid(false);
#endif // VDM_FINGERPRINT
#ifdef ML_TRACE
    this->metaivval_object_id = MetaivVal::metaivval_object_create_count++;
    this->seq_count = 0;
#endif // ML_TRACE
  };
  virtual ~MetaivVal() {
#if 0
    DestroyCount++;
    if (DestroyCount > BuildCount) {
      cerr << L"Detected BuildCount=" << BuildCount
           << L" DestroyCount=" << DestroyCount << '\n';
      exit(1);
    }
#ifdef SHOW_MALLOC_STATS
    showmalloccount->remval(type);
#endif // SHOW_MALLOC_STATS
#endif // 0
  };

  void AddRef() { this->RefCount++; };
  void RemoveRef() {
    this->RefCount--;
#ifdef VDMLIB_SAFE_CHECK
    if (this->RefCount < 0)
     M4LibError::ReportError(ML_ZERO_REF,L"zero reference counter detected");
#endif // VDMLIB_SAFE_CHECK
    if ( this->RefCount == 0 ) delete_this();
  };
  void delete_this() {
#ifdef ML_TRACE
    traceObj( L"MetaivVal::delete_this()" );
#endif // ML_TRACE
#ifdef SHOW_MALLOC_STATS
    MetaivVal::BuildCount--;
    MetaivVal::DestroyCount++;
#endif // SHOW_MALLOC_STATS
    delete this;
  };

  virtual void ostream_out(wostream &, const VDMFormatter &) const = 0;
  virtual void WriteVal(ostream &) const = 0;
  virtual MetaivVal* Copy() const = 0;
  virtual int Compare(const MetaivVal &) const = 0;
  virtual bool IsSimple() const = 0;

  metaivType GetType() const { return this->type; };
  int GetRefCount() const { return this->RefCount; };
  bool MultiLinked() const { return (this->RefCount > 1); }
  wstring PrintType() const;

  bool IsContainerVal() const { return (this->type == mt_container); };
  bool IsNilVal() const { return (this->type == mt_nil); };
  bool IsCharVal() const { return (this->type == mt_char); };
  bool IsIntVal() const { return (this->type == mt_int); };
  bool IsRealVal() const { return (this->type == mt_real); };
  bool IsQuoteVal() const { return (this->type == mt_quote); };
  bool IsTupleVal() const { return (this->type == mt_tuple); };
  bool IsRecordVal() const { return (this->type == mt_record); };
  bool IsSetVal() const { return (this->type == mt_set); };
  bool IsMapVal() const { return (this->type == mt_map); };
  bool IsTextVal() const { return (this->type == mt_text); };
  bool IsTokenVal() const { return (this->type == mt_token); };
  bool IsBoolVal() const { return (this->type == mt_bool); };
  bool IsSequenceVal() const { return (this->type == mt_sequence); };
  bool IsObjectRefVal() const { return (this->type == mt_objectref); };
  bool IsCommonVal() const { return (this->type == mt_common); };
  bool IsGenericVal() const { return (this->type == mt_generic); };
  bool IsUndefVal() const { return (this->type == mt_undef); };
  bool IsDLObjectVal() const { return (this->type == mt_dlobject); };
  bool IsDRPtrVal() const { return (this->type == mt_drptr); };
  bool IsTimeVal() const { return (this->type == mt_time); };
  bool IsFunctionVal() const { return (this->type == mt_function); };

  int CompareMetaivType(MetaivVal *) const;

#ifdef VDM_FINGERPRINT
  void readfingerprint(Fingerprint&) const; // Inline.
  // LTO - virtual void updatefingerprint() const = 0;
  virtual void updatefingerprint() const;
  void setfingerprint(Fingerprint& fingerprint) const; // Not const!

  Fingerprint fp;
#endif // VDM_FINGERPRINT

public:
#ifdef SHOW_MALLOC_STATS
  static int BuildCount;
  static int DestroyCount;
#endif // SHOW_MALLOC_STATS

#ifdef ML_TRACE
public:
 long metaivval_object_id;
 static long metaivval_object_create_count;
 long seq_count;
 void traceObj( const wchar_t *method );
#endif // ML_TRACE
};

class VDM_RecInfo
{
public:
  typedef void (*vdm_pp_function_ptr)(wostream&, const Record&, VDMFormatter vf, bool simple);
private:
  uint64_t bits;
  int Tag;
  int Size;
  vdm_pp_function_ptr print_function;
  wstring symname;
  vector<bool> *bv;
  static const int BitLimit;

public:
  VDM_RecInfo();
  VDM_RecInfo(int tag, int size);
  ~VDM_RecInfo();

  bool AskDontCare(int field) const;
  void SetDontCare(int field);
  int GetTag() const { return this->Tag; };
  bool Is(const int tag) const { return (this->Tag == tag); };
  void SetSymTag(const wstring & s) { this->symname = s; };
  const wstring & GetSymTag() const { return this->symname; };
  int GetSize() const { return this->Size; };
  bool IsEmpty() const { return (this->Size == 0); };
  bool OutOfRange(int i) const { return ((i < 1 ) || (i > this->Size)); };

  void SetPrintFunction(vdm_pp_function_ptr f) { this->print_function = f; };
  bool HasPrintFunction() const { return (this->print_function != NULL); };
  void Print(wostream & os, const Record & r, VDMFormatter vf, bool simple) const
  { (*this->print_function)(os, r, vf, simple); };
  void dump(wostream & os) const
  {
    os << L"tag=" << this->Tag
       << L" size=" << this->Size
       << L" symtag=`" << this->symname << L"'";
  }
};

class VDMRecInfoMap
{
public:
  typedef void (*vdm_pp_function_ptr)(wostream&, const Record&, VDMFormatter vf, bool simple);

private:
#if __cplusplus >= 201103L
  typedef unordered_map<int, VDM_RecInfo *> VDMRecInfoMap_t;
  typedef unordered_map<wstring, VDM_RecInfo *> VDMRecInfoSymTagMap_t;
#else
  typedef map<int, VDM_RecInfo *> VDMRecInfoMap_t;
  typedef map<wstring, VDM_RecInfo *> VDMRecInfoSymTagMap_t;
#endif

  friend class RecordVal;
  VDMRecInfoMap_t mapvalue;
  VDMRecInfoSymTagMap_t symtagmap;

  VDM_RecInfo * Register(int tag, int size);
  VDM_RecInfo * Register(int tag, int size, const wstring & symtag, const vector<int> & dc);
public:
  VDMRecInfoMap() {};
  ~VDMRecInfoMap() { this->clear(); };

  void NewTag (int tag, int size) { this->Register(tag, size); };
  void NewTag (int tag, int size, const wstring & symtag, const vector<int> & dc)
  { this->Register(tag, size, symtag, dc); };
  bool AskDontCare(int /*tag*/, int field) const;
  void SetDontCare(int /*tag*/, int /*field*/);
  void SetSymTag(int /*tag*/, const wstring& /*symtag*/);
  void SetPrintFunction(int /*tag*/, VDM_RecInfo::vdm_pp_function_ptr f);
  int GetSize(int tag) const;
  bool GetSymTag(int tag, /* return parameter */ wstring & s) const;
  bool GetTag(const wstring &, /* var parameters */ int & tag, int & size) const;
  bool IsDecl(const wstring & tag) const;
  const VDM_RecInfo * GetInfo(const wstring & tag) const;
  unsigned int size() const { return this->mapvalue.size(); };
  void dump(wostream &o) const;
  void clear();
};

//
extern VDMRecInfoMap & VDMGetDefaultRecInfoMap();

////////////////////////////////////////////////////////////
// VDMSerializeInfoMap
////////////////////////////////////////////////////////////
class vdm_Seri
{
public:
  typedef Common (*vdm_DeSerialize_function)(istream&);
  typedef void (*vdm_Serialize_function)(ostream&, const MetaivVal *);

private:
  vdm_DeSerialize_function de_seri;
  vdm_Serialize_function seri;

public:
  vdm_Seri(vdm_DeSerialize_function d, vdm_Serialize_function s) : de_seri(d), seri(s) {};
  void Serialize(ostream & os, const MetaivVal * mv) const { (*this->seri)(os, mv); };
  Common DeSerialize(istream & is) const;
};

//class vdm_SerializeMap;

class VDMSerializeInfoMap
{
private:
  typedef map<wstring, vdm_Seri> VDMSerializeInfoMap_t;
  VDMSerializeInfoMap_t mapvalue;

public:
  VDMSerializeInfoMap() {};
  ~VDMSerializeInfoMap() { this->mapvalue.clear(); }
  void Register(const wstring & typeidname,
                vdm_Seri::vdm_DeSerialize_function deseri,
                vdm_Seri::vdm_Serialize_function seri);
  const vdm_Seri * LookUp(const wstring & typeidname);
};

//
extern VDMSerializeInfoMap & VDMGetDefaultSerializeInfoMap();

/***************************************************************************
 * SHOW_MALLOC_STATS
 ***************************************************************************/
#if defined(SHOW_MALLOC_STATS)
extern void insert(const wchar_t *name, MetaivVal *p, int size);
extern void remove(const wchar_t *name, MetaivVal *p);
#endif // SHOW_MALLOC_STATS

/***************************************************************************
*
*  VDMContainerVal<T>
*
***************************************************************************/
template <class T> class VDMContainerVal : public MetaivVal
{
public:
  typedef VDMContainerVal<T> ValType;
  typedef T ContainedType;
  typedef VDMContainerVal<T> * ValPtrT;
public:
  ContainedType value;

public:
  VDMContainerVal();
  VDMContainerVal(const T & v);
  VDMContainerVal(const VDMContainerVal &);
  ~VDMContainerVal();

  ValType & upcast(MetaivVal &m) const { return static_cast<ValType &>(m); }
  const ValType & upcast(const MetaivVal &m) const { return static_cast<const ValType &>(m); }

  MetaivVal * Copy() const { return new VDMContainerVal<T>(*this); };

  void WriteVal(ostream&) const;
  void ostream_out(wostream&, const VDMFormatter&) const;

  virtual int Compare(const MetaivVal  &) const;
  bool IsSimple() const { return true; };

#ifdef VDM_FINGERPRINT
  virtual void updatefingerprint() const { }
#endif // VDM_FINGERPRINT
};

template <class T>
void VDMContainerVal<T>::WriteVal(ostream& o) const
{
  string id (typeid(ValPtrT).name());
  const vdm_Seri * p = VDMGetDefaultSerializeInfoMap().LookUp(string2wstring(id).c_str());
  if (p != NULL) {
    o << "n" << id.length() << "," << id << ",";
//    p->seri(o, this);
    p->Serialize(o, this);
  } else {
    wcerr << L"WriteVal: unknown (" << string2wstring(id) << L")\n";
  }
}

template <class T>
VDMContainerVal<T>::VDMContainerVal() : MetaivVal(mt_container)
{
#if defined(SHOW_MALLOC_STATS)
    insert(typeid(*this).name(), this, sizeof(*this));
#endif
}

template <class T>
VDMContainerVal<T>::VDMContainerVal(const T & v) : MetaivVal(mt_container), value(v)
{
#if defined(SHOW_MALLOC_STATS)
    insert(typeid(*this).name(), this, sizeof(*this));
#endif
}

template <class T>
VDMContainerVal<T>::VDMContainerVal(const VDMContainerVal<T> &c)
                   : MetaivVal(mt_container), value(c.value)
{
#if defined(SHOW_MALLOC_STATS)
    insert(typeid(*this).name(), this, sizeof(*this));
#endif
}

template <class T>
VDMContainerVal<T>::~VDMContainerVal()
{
#if defined(SHOW_MALLOC_STATS)
    remove(typeid(*this).name(), this);
#endif
}

// You can override Compare to avoid defining vdm_Compare.
template <class T>
int VDMContainerVal<T>::Compare(const MetaivVal &v) const
{
  return vdm_Compare(value, ((VDMContainerVal<T>&)v).value);
}

template <class T>
void VDMContainerVal<T>::ostream_out(wostream&o, const VDMFormatter&v) const
{
  o << L"containertype="
    << this->GetType()
    << L" (typeid="
    << string2wstring(string(typeid(*this).name()))
    << L")";
}

////////////////////
//// VDMLibControl
///////////////////
class VDMLibControl
{
private:
  static int initiated;
private:
  void init();
public:
  VDMLibControl();
  VDMLibControl(unsigned int hdr_vers);
  ~VDMLibControl();

public:
  // CopyOnModify (default true).
  static bool CopyOnModify;
  // Copy statistics.
  static int CopyCountSequence;
  static int CopyCountTuple;
  static int CopyCountRecord;
  static int CopyCountSet;
  static int CopyCountMap;
  static int CopyCountToken;
  static int CopyCountInt;
  static int CopyCountReal;
  static int CopyCountText;
  static int CopyCountQuote;
  static void print_copy_stats(wostream &o);
};

static VDMLibControl vdm_lib_initialise(VDMLIBVERSION);

/***************************************************************************
*
*  Common
*
*  Common is the common base class for all metaiv variables (Int, Set, ...).
*  It contains only the variabel p, which is a pointer to the value
*  for the metaiv variabel.
***************************************************************************/
wostream& operator<<(wostream&, const Common&);

class Common
{
 protected:
  MetaivVal *mvp;

 public:
  static Common dummy;
  static VDMLibControl vdm_lib_initialise2;

 protected:
  void Link(MetaivVal *);
  void Link(const Common & c) { this->Link(c.mvp); };
  void AddRef();
  void RemoveRef();

//  void ChangeSimple(int*CopyCount);
//  void ChangeComplex(int*CopyCount, bool force=false);

 public:
  Common();
  Common(MetaivVal*);
  Common(const Common & c);
  ~Common() { this->RemoveRef(); };

//  MetaivVal::metaivType MyValType() const { return this->mvp->GetType(); };
//  MetaivVal::metaivType gettype() const { return this->mvp->GetType(); };
  wstring PrintType () const { return this->mvp->PrintType(); };
  int GetRefCount() const { return this->mvp->GetRefCount(); };
  MetaivVal * get_mvp() const { return (this->mvp); };

  void Clone();
  int Compare(const Common &) const;
  void pr_ascii(wostream & str, int ind = 0) const;
  wstring ascii() const;
  friend wostream& operator<<(wostream &, const Common &);
  void ostream_out(wostream & os, const VDMFormatter & vf) const
  { this->mvp->ostream_out(os, vf); };
  void WriteVal(ostream & os) const { this->mvp->WriteVal (os); };

  Common & operator=(const Common & c) { this->Link(c); return *this; };

  bool operator==(const Common & c) const { return (this->Compare(c) == 0); };
  bool operator!=(const Common & c) const { return (this->Compare(c) != 0); };
  bool operator<(const Common & c) const { return (this->Compare(c) < 0); };

  bool IsContainer() const { return this->mvp->IsContainerVal(); };
  bool IsNil() const { return this->mvp->IsNilVal(); };
  bool IsChar() const { return this->mvp->IsCharVal(); };
  bool IsInt() const { return this->mvp->IsIntVal(); };
  bool IsReal() const { return this->mvp->IsRealVal(); };
  bool IsQuote() const { return this->mvp->IsQuoteVal(); };
  bool IsTuple() const { return this->mvp->IsTupleVal(); };
  bool IsRecord() const { return this->mvp->IsRecordVal(); };
  bool IsSet() const { return this->mvp->IsSetVal(); };
  bool IsMap() const { return this->mvp->IsMapVal(); };
  bool IsText() const { return this->mvp->IsTextVal(); };
  bool IsToken() const { return this->mvp->IsTokenVal(); };
  bool IsBool() const { return this->mvp->IsBoolVal(); };
  bool IsSequence() const { return this->mvp->IsSequenceVal(); };
  bool IsObjectRef() const { return this->mvp->IsObjectRefVal(); };
  bool IsCommon() const { return this->mvp->IsCommonVal(); };
  bool IsGeneric() const { return this->mvp->IsGenericVal(); };
  bool IsDLObject() const { return this->mvp->IsDLObjectVal(); };
  bool IsUndef() const { return this->mvp->IsUndefVal(); };
  bool IsDRPtr() const { return this->mvp->IsDRPtrVal(); };
  bool IsTime() const { return this->mvp->IsTimeVal(); };
  bool IsFunction() const { return this->mvp->IsFunctionVal(); };

  bool Is(const int tag) const;

  bool IsSimple() const { return this->mvp->IsSimple(); };

  template <class T> T & mgetval(T*)
  {
    Clone();
    return static_cast<T &>(*this->mvp);
  }
  template <class T> const T & cgetval(T*) const
  {
    return static_cast<const T &>(*this->mvp);
  }
  template <class T> T & sharedgetval(T*)
  {
    return static_cast<T &>(*this->mvp);
  }

#ifdef VDM_FINGERPRINT
  void readfingerprint(Fingerprint& fingerprint) const
  { this->mvp->readfingerprint(fingerprint); };
#endif // VDM_FINGERPRINT
#ifdef ML_TRACE
 public:
  long metaiv_object_id;
  long seq_count;
  static long metaiv_object_create_count;
  void traceObj( const wchar_t *method );
#endif // ML_TRACE
};

#define MGETVAL(t) mgetval((t*)0)
#define CGETVAL(t) cgetval((t*)0)
#define SHAREDGETVAL(t) sharedgetval((t*)0)

inline
void Common::RemoveRef()
{
#ifdef VDMLIB_SAFE_CHECK
  if (mvp != NULL) {
#endif // VDMLIB_SAFE_CHECK

    mvp->RemoveRef();

#ifdef VDMLIB_SAFE_CHECK
    mvp = NULL;
  }
  lse
    M4LibError::ReportError(ML_NULL_REF,L"null pointer detected");
#endif //VDMLIB_SAFE_CHECK
}

inline
void Common::AddRef()
{
#ifdef VDMLIB_SAFE_CHECK
  if (mvp != NULL) {
#endif // VDMLIB_SAFE_CHECK

    mvp->AddRef();

#ifdef VDMLIB_SAFE_CHECK
  } else
    M4LibError::ReportError(ML_NULL_REF,L"null pointer detected");
#endif // VDMLIB_SAFE_CHECK
}

inline
void Common::Link(MetaivVal *mivp)
{
  mivp->AddRef();
  this->RemoveRef();
  this->mvp = mivp;
}

inline
Common::Common(MetaivVal*mivp)
{
#ifdef VDMLIB_SAFE_CHECK
  if (mivp == NULL)
    M4LibError::ReportError(ML_NULL_REF,L"null pointer detected");
#endif // VDMLIB_SAFE_CHECK

#ifdef ML_TRACE
  this->metaiv_object_id = Common::metaiv_object_create_count++;
  this->seq_count = 0;
#endif // ML_TRACE

  this->mvp = mivp;
  this->AddRef();
}

inline
Common::Common(const Common& c)
{
#ifdef ML_TRACE
  this->metaiv_object_id = Common::metaiv_object_create_count++;
  this->seq_count = 0;
#endif // ML_TRACE

  this->mvp = c.mvp;
  this->AddRef();
}

inline
void Common::Clone()
{
  if (this->mvp->MultiLinked()) {
    this->Link(this->mvp->Copy());
  }
#ifdef VDM_FINGERPRINT
  mvp->fp.setvalid(false);
#endif // VDM_FINGERPRINT
}

#ifdef VDM_FINGERPRINT
class Fingerprint {
public:
  unsigned long value;
  bool valid_fingerprint;

  Fingerprint();
  void setvalid(bool);
  bool valid() const;
  int Compare(const Fingerprint&) const;
  void init_crc();
  void done_crc();
  void continue_crc(const void *p, int sz);
  static void crcgen( );

private:
  static unsigned long crcTable[256];
  void continue_crc_on_crc(const Fingerprint& fp);
  unsigned long getcrc(const wchar_t *p, int len);
  void continue_crc(unsigned long & crcinout, const wchar_t *p, int len);
  void continue_crc_on_crc(unsigned long & crc, unsigned long crcdata);
};
#endif // VDM_FINGERPRINT

/***************************************************************************
*
*  Generic
*
***************************************************************************/

class Generic : public Common {
public:
  Generic();
  ~Generic() {};
  Generic(const Generic & g) : Common(g) {};
  Generic(const Common & c) : Common(c) {};

  Generic& operator=(const Generic & g) { this->Link(g); return *this; };
  Generic& operator=(const Common & c) { this->Link(c); return *this; };
};

/**************************************************************************
*
*  VDMContainer<T>
*
**************************************************************************/
template <class T> class VDMContainer : public Common
{
public:
  VDMContainer() : Common(new VDMContainerVal<T>()) {};
  VDMContainer(const VDMContainer & c) : Common(c) {};
  VDMContainer(const T & v) : Common(new VDMContainerVal<T>(v)) {};
  VDMContainer(VDMContainerVal<T> * v) : Common(v) {};
  VDMContainer(const Generic & g) : Common(g)
  { if (!this->IsContainer()) M4LibError::ReportError(ML_CAST_ERROR, g.PrintType() + L" casted to VDMContainer"); };
  ~VDMContainer() {};

  void EnsureUnique() { Clone(); };
  T & get_oneref_ref() { Clone(); return (static_cast<VDMContainerVal<T> *>(mvp))->value; };
  T & get_shared_ref() { return ((VDMContainerVal<T> *)mvp)->value; };
  const T & get_const_ref() const { return (static_cast<VDMContainerVal<T> *>(mvp))->value; };
};

/**************************************************************************
*
*  Int
*
**************************************************************************/

class Int : public Common
{
public:
  Int();
  Int(const int64_t);
  Int(const Int & i) : Common(i) {};
  Int(const Real &);
  Int(const Generic &);
  ~Int() {};

  operator int() const;

  Int operator-() const;

  Int operator+(const Int &) const;
  Real operator+(const Real &) const;

  Int operator-(const Int &) const;
  Real operator-(const Real &) const;

  Int operator*(const Int &) const;
  Real operator*(const Real &) const;

  Real operator/(const Int &) const;
  Real operator/(const Real &) const;

  Real Exp(const Int &) const;
  Real Exp(const Real &) const;

  Int & Incr();

  Int& operator=(const int64_t);
  Int& operator=(const Generic &);
  Int& operator=(const Int &);
  Int& operator=(const Real &);
  int64_t GetValue() const;
};

/**************************************************************************
*
*  Nil
*
**************************************************************************/

class Nil : public Common
{
public:
  Nil();
  Nil(const Nil &);
  Nil(const Generic &);
  ~Nil() {};

  Nil& operator=(const Nil & i);
  Nil& operator=(const Generic &);
};

/**************************************************************************
*
*  Bool
*
**************************************************************************/

class Bool : public Common
{
public:
  Bool();
  Bool(bool);
  Bool(const Bool & b) : Common(b) {};
  Bool(const Generic &);
  ~Bool() {};

  operator bool() const;
  Bool& operator=(const Bool &);
  Bool& operator=(const bool);
  Bool& operator=(const Generic &);

  Bool operator!() const;
  Bool operator&&(bool) const;
  Bool operator||(bool) const;

  Bool& operator&=(bool);
  Bool& operator|=(bool);

  Bool mnot() const { return this->operator!(); };
  Bool mand(const Bool & b) const { return this->operator&&(b); };
  Bool mor(const Bool & b) const { return this->operator||(b); };

  bool GetValue() const;
};

/**************************************************************************
*
*  Real
*
**************************************************************************/

class Real : public Common {
public:
  Real();
  Real(const double);
  Real(const Int & i);
  Real(const Real & r) : Common(r) {};
  Real(const Generic &);
  ~Real() {};

  operator double() const;

  Real operator-() const;

  Real operator+(const Int  &) const;
  Real operator+(const Real &) const;

  Real operator-(const Int  &) const;
  Real operator-(const Real &) const;

  Real operator*(const Int  &) const;
  Real operator*(const Real &) const;

  Real operator/(const Int  &) const;
  Real operator/(const Real &) const;

  Real Exp(const Int  &) const;
  Real Exp(const Real &) const;

  Real& operator=(const Int&);
  Real& operator=(const double);
  Real& operator=(const Generic &);
  Real& operator=(const Real &);
  double GetValue() const;
  int64_t GetIntValue() const;

  bool IsInt() const;
  bool IsNat() const;
  bool IsNatOne() const;

  bool IsZero() const;

  Real Abs() const;
  Real Floor() const;
  Real Ceil() const;
  Real Minus() const;

  Real Add(const Real & r) const;
  Real Sub(const Real & r) const;
  Real Mult(const Real & r) const;
  Real Div(const Real & r ) const;
  Real IntDiv(const Real & r ) const;
  Real Rem(const Real & r ) const;
  Real Mod(const Real & r ) const;
  Real Pow(const Real & r ) const;
  Bool GreaterEqual(const Real & r) const;
  Bool GreaterThan(const Real & r) const;
  Bool LessEqual(const Real & r) const;
  Bool LessThan(const Real & r) const;

  Real HexAnd(const Real & r) const;
  Real HexOr(const Real & r) const;
  Real HexExor(const Real & r) const;
  Real HexShiftRight(const Real & r) const;
  Real HexByteShiftRight(const Real & r) const;
  Real HexShiftLeft(const Real & r) const;
  Real HexByteShiftLeft(const Real & r) const;
};

/**************************************************************************
*
*  Quote
*
**************************************************************************/

class Quote : public Common
{
 public:
  Quote();
  Quote(const wchar_t *);
  Quote(const wstring &);
  Quote(const Quote & q) : Common(q) {};
  Quote(const Generic &);
  ~Quote() {};

  Quote& operator=(const Quote &);
  Quote& operator=(const wchar_t *);
  Quote& operator=(const wstring &);
  Quote& operator=(const Generic &);

  // Be carefull about this function. Se the comment on
  // Text::GetValue().
  wstring GetValue() const;
};

/**************************************************************************
*
*  Char
*
**************************************************************************/

class Char : public Common
{
public:
  Char();
  Char(wchar_t);
  Char(const Char & c) : Common(c) {} ;
  Char(const Generic &);
  ~Char() {};

  operator wchar_t();
  Char& operator=(const Char &);
  Char& operator=(const wchar_t);
  Char& operator=(const Generic &);
  wchar_t GetValue() const;

  int ToHexNum() const;
  int ToDecNum() const;
  int ToBinNum() const;
};

/**************************************************************************
*
*  Text
*
**************************************************************************/

class Text : public Common
{
public:
  Text();
  Text(const wchar_t *);
  Text(const wstring &);
  Text(const Text & t): Common(t) {};
  Text(const Generic &);
  ~Text() {};

  Text& operator=(const Text &);
  Text& operator=(const wchar_t *);
  Text& operator=(const wstring &);
  Text& operator=(const Generic &);
  wstring GetValue() const;
};

/**************************************************************************
*
*  Token
*
**************************************************************************/
class Token : public Common
{
public:
  Token();
  Token(const wchar_t*);
  Token(const wstring &);
  Token(const Generic &);
  Token(const Token & t) : Common(t) {};
  ~Token() {};

  Token& operator=(const Token &);
  Token& operator=(const wchar_t *);
  Token& operator=(const wstring &);
  Token& operator=(const Generic &);
  const Generic & GetValue() const;
  wstring GetString() const;
};

Token mk_token(const Common & c);

/**************************************************************************
*
*  Map
*
**************************************************************************/

class MapValIterator;

class Map : public Common
{
private:
  MapValIterator *iteratorP;
public:

  Map();
  Map(const Map & m) : Common(m), iteratorP(NULL) {};
  Map(const Generic &);
  ~Map();

  Map & operator=(const Map & m) { this->Link(m.mvp); return *this; };
  Map & operator=(const Generic &);

  const Generic & operator[](const Common &) const;
  const Generic & Apply(const Common &) const;

  void Clear();
  Map & Insert(const Common &, const Common &);
  Map & ImpModify(const Common &, const Common &);
  Map & ImpOverride(const Map &);
  Map Override(const Map &) const;
  Map & RemElem(const Common &);

  int Size() const;
  bool IsEmpty() const;
  Set Dom() const;
  bool DomExists(const Common & g) const;
  bool DomExists(const Common & g, Generic &) const;
  Set Rng() const;
  Map Inverse() const;
  bool IsCompatible(const Map &) const;

  Map DomRestrictedTo(const Set &) const;
  Map DomRestrictedBy(const Set &) const;
  Map RngRestrictedTo(const Set &) const;
  Map RngRestrictedBy(const Set &) const;

  bool First(Generic &);
  bool Next(Generic &);
  bool First(Generic &, Generic &);
  bool Next(Generic &, Generic &);

  size_t MaxSize() const;

protected:
  void GC();
  void GC2(const Common & c);
  const Common & CApply(const Common &) const;
  Common & GetRngRef(const Common &);
};

/**************************************************************************
*
*  Sequence
*
**************************************************************************/
class SequenceValIterator;

class Sequence : public Common
{
private:
  SequenceValIterator *IteratorP;

public:
  Sequence();
  Sequence(const Sequence & m) : Common(m), IteratorP(NULL) {};
  Sequence(const wchar_t *);
  Sequence(const wstring &);
  Sequence(const Generic &);
  ~Sequence();

  Sequence & operator=(const Sequence & s) { this->Link(s.mvp); return *this; };
  Sequence & operator=(const wchar_t * s)
  { return this->operator=((s == NULL) ? wstring(L"") : wstring(s)); };

  Sequence & operator=(const wstring &);
  Sequence & operator=(const Generic &);

  const Generic & Index(int64_t i) const;
  const Generic & operator[](int64_t i) const;
  const Generic & Hd() const;
  Sequence Tl() const;
  Sequence & ImpTl();
  Sequence & ImpConc(const Sequence &);
  Sequence & operator^=(const Sequence & s) { return this->ImpConc(s); };
  Sequence Conc(const Sequence &) const;
  Sequence operator^(const Sequence & s) const { return this->Conc(s); };
  const Generic & Last() const;

  int64_t Length() const;
  bool IsEmpty() const ;
  bool GetString(wstring &) const;
  wstring GetString() const;
  bool IsString() const;

  void Clear();

  Sequence & ImpAppend(const Common &);
  Sequence & ImpPrepend(const Common &);
  Sequence & RemElem(int64_t i);
  Sequence & ImpModify(int64_t j, const Common &);
  Set Elems() const;
  Set Inds() const;
  Sequence Reverse() const;
  Sequence SubSequence(int64_t from, int64_t to) const;
  bool StartsWith(const Sequence & l) const;
  bool EndsWith(const Sequence & l) const;
  Sequence Product(const Set & s) const;
  Set Permute() const;
  Sequence DistrSetProduct() const;
  bool OutOfRange(const Real & r) const;
  int64_t Find(const Common &) const;

  bool First(Generic &) const;
  bool Next(Generic &) const;

  size_t MaxSize() const;

protected:
  Common & GetIndexRef(int64_t i);
  const Common & CIndex(int64_t i) const;
  const Common & CLast() const;
};

Sequence mk_sequence(const Generic & p1);
Sequence mk_sequence(const Generic & p1, const Generic & p2);
Sequence mk_sequence(const Generic & p1, const Generic & p2, const Generic & p3);
Sequence mk_sequence(const Generic & p1, const Generic & p2, const Generic & p3, const Generic & p4);
Sequence mk_sequence(const Generic & p1, const Generic & p2, const Generic & p3, const Generic & p4,
                     const Generic & p5);
Sequence mk_sequence(const Generic & p1, const Generic & p2, const Generic & p3, const Generic & p4,
                     const Generic & p5, const Generic & p6);
Sequence mk_sequence(const Generic & p1, const Generic & p2, const Generic & p3, const Generic & p4,
                     const Generic & p5, const Generic & p6, const Generic & p7);
Sequence mk_sequence(const Generic & p1, const Generic & p2, const Generic & p3, const Generic & p4,
                     const Generic & p5, const Generic & p6, const Generic & p7, const Generic & p8);

/**************************************************************************
*
*  Stack
*
**************************************************************************/
class Stack : public Sequence
{
public:
  Stack() {};
  Stack(const Stack & s) : Sequence(s) {};
  ~Stack() {};

  Stack & Push(const Common & c);
  Generic Pop();
  Sequence Pop(int64_t n);

  const Generic & Head() const { return Sequence::Hd(); };
  const Generic & GetNth(int64_t n) const { return Sequence::Index(n); };
  Stack & RemoveNth(int64_t n);

  Stack & Copy();
  Stack & Swap();

  int64_t Length() const { return Sequence::Length(); };
  bool IsEmpty() const { return Sequence::IsEmpty(); };
  void Clear() { Sequence::Clear(); };
};

/**************************************************************************
*
*  Set
*
**************************************************************************/

class SetValIterator;

class Set : public Common
{
 private:
  SetValIterator *IteratorP;
 public:
  Set();
  Set(const Set & m) : Common(m), IteratorP(NULL) {};
  Set(const Generic &);
  ~Set();

  Set & operator=(const Set & s) { this->Link(s.mvp); this->IteratorP = NULL; return *this; };
  Set & operator=(const Generic &);
  Set & Insert(const Common &);
  int64_t Card() const;
  Set Power() const;
  bool IsEmpty() const;
  bool InSet(const Common &) const ;
  void Clear();
  Set & ImpUnion(const Set &);
  Set & ImpIntersect(const Set &);
  Set & ImpDiff(const Set &);
  Set Union(const Set &) const;
  Set Intersect(const Set &) const;
  Set Diff(const Set &) const;
  const Generic & GetElem() const;
  const Generic & GetElem(bool reverse) const;
  Set & RemElem(const Common &);
  bool SubSet(const Set &) const;

  bool First(Generic &) const;
  bool Next(Generic &) const;

  Sequence ToSequence() const;
  Sequence ToSequence(bool reverse) const;
  Set DirectProduct(const Set & s) const;
  Set DistrDirectProduct() const;
  Set DUnion() const;
  Set DInter() const;

  size_t MaxSize() const;
};

Set mk_set(const Generic & p1);
Set mk_set(const Generic & p1, const Generic & p2);
Set mk_set(const Generic & p1, const Generic & p2, const Generic & p3);
Set mk_set(const Generic & p1, const Generic & p2, const Generic & p3, const Generic & p4);
Set mk_set(const Generic & p1, const Generic & p2, const Generic & p3, const Generic & p4,
           const Generic & p5);
Set mk_set(const Generic & p1, const Generic & p2, const Generic & p3, const Generic & p4,
           const Generic & p5, const Generic & p6);
Set mk_set(const Generic & p1, const Generic & p2, const Generic & p3, const Generic & p4,
           const Generic & p5, const Generic & p6, const Generic & p7);
Set mk_set(const Generic & p1, const Generic & p2, const Generic & p3, const Generic & p4,
           const Generic & p5, const Generic & p6, const Generic & p7, const Generic & p8);

/**************************************************************************
*
*  DLObject
*
**************************************************************************/
class DlClass;

class DLObject : public Common
{
public:
  DLObject(const wstring & name, DlClass *object);
  DLObject(const DLObject & dlobj) : Common(dlobj) {};
  DLObject(const Generic & dlobj);
  DLObject & operator=(const DLObject  &);
  DLObject & operator=(const Generic &);
  wstring GetName() const;
  DlClass * GetPtr() const;
  ~DLObject() {};
};

/**************************************************************************
*
*  Record
*
**************************************************************************/
class Record : public Common
{
public:
  Record();
  Record(int tag, int size);
  Record(int tag, int size, VDMRecInfoMap & tagspace);
  Record(const wstring & symtag, const VDMRecInfoMap & tagspace);
  Record(const Record & m) : Common(m) {};
  Record(const Generic &);
  Record(const Common &);
  ~Record() {};

  Record & operator=(const Record & r) { this->Link(r.mvp); return *this; };
  Record & operator=(const Generic &);
  Record & SetField(const int i, const Common& gdom);
  const Generic & GetField(const int i) const;
  int GetTag() const;
  wstring GetTextTag() const;
  int64_t Length() const;
  Sequence GetFields() const;
  Record & SetFields(const Sequence &);
  bool IsDontCare(int filed) const;

  Common & GetFieldRef(int i);

  const Int & GetInt(int i) const;
  const Real & GetReal(int i) const;
  const Map & GetMap(int i) const;
  const Set & GetSet(int i) const;
  const Sequence & GetSequence(int i) const;
  const Tuple & GetTuple(int i) const;
  const Bool & GetBool(int i) const;
  const Record & GetRecord(int i) const;
  const Char & GetChar(int i) const;

  int64_t GetIntValue(int i) const;
  double GetRealValue(int i) const;
  bool GetBoolValue(int i) const;
};

/**************************************************************************
*
*  Tuple
*
**************************************************************************/
class Tuple : public Common
{
public:
  Tuple();
  Tuple(int size);
  Tuple(const Tuple & t) : Common(t) {};
  Tuple(const Generic &);
  ~Tuple() {};

  Tuple & operator=(const Tuple &);
  Tuple & operator=(const Generic &);
  Tuple & SetField(int i, const Common & gdom);
  const Generic & GetField(int i) const;
  int64_t Length() const;
  Sequence GetFields() const;

  Common & GetFieldRef(int i);

  const Int & GetInt(int i) const;
  const Real & GetReal(int i) const;
  const Map & GetMap(int i) const;
  const Set & GetSet(int i) const;
  const Sequence & GetSequence(int i) const;
  const Tuple & GetTuple(int i) const;
  const Bool & GetBool(int i) const;
  const Record & GetRecord(int i) const;

  int64_t GetIntValue(int i) const;
  double GetRealValue(int i) const;
  bool GetBoolValue(int i) const;
};

Tuple mk_(const Generic & p1, const Generic & p2);
Tuple mk_(const Generic & p1, const Generic & p2, const Generic & p3);
Tuple mk_(const Generic & p1, const Generic & p2, const Generic & p3, const Generic & p4);
Tuple mk_(const Generic & p1, const Generic & p2, const Generic & p3, const Generic & p4,
          const Generic & p5);
Tuple mk_(const Generic & p1, const Generic & p2, const Generic & p3, const Generic & p4,
          const Generic & p5, const Generic & p6);
Tuple mk_(const Generic & p1, const Generic & p2, const Generic & p3, const Generic & p4,
          const Generic & p5, const Generic & p6, const Generic & p7);
Tuple mk_(const Generic & p1, const Generic & p2, const Generic & p3, const Generic & p4,
          const Generic & p5, const Generic & p6, const Generic & p7, const Generic & p8);

/**************************************************************************
*
*  ObjectRef
*
**************************************************************************/

class ObjectRef;

class ObjectRefVal : public MetaivVal {
private:
  friend class ObjectRef;
protected:
  virtual bool has_cg_base() const { return false; }

public:
  virtual int ctype() const = 0;
  // ObjectRefVal(vdmBase* mp);
  ObjectRefVal();
  virtual ~ObjectRefVal();
  void WriteVal(ostream&) const;
  void ostream_out(wostream&, const VDMFormatter&) const;
  virtual int Compare(const MetaivVal &) const;
  virtual MetaivVal* Copy() const {return NULL;}; // this should never be executed
  virtual void updatefingerprint() const;
  bool IsSimple() const { return false; };
};

class vdmBase : public ObjectRefVal {

private:
  int clstype;
  Set base_s;
  Set class_s;

protected:
  void RegisterAsBase(int id);
  void RegisterAsDerived(int id);

public:
  vdmBase() {}
  virtual ~vdmBase();
public:
  int ctype() const { return clstype; }
  Bool SameBaseClass(const ObjectRef&) const;
  Bool IsOfClass(int) const ;
  Bool IsOfBaseClass(int) const ;
  Set GetBaseClasses() const { return base_s; }
};


class ObjectRef : public Common {
 public:
  ObjectRef(vdmBase* = NULL);
  ObjectRef(const ObjectRef &);
  ObjectRef(const Generic &);
  virtual ~ObjectRef();

  ObjectRef& operator=(const ObjectRef &);
  ObjectRef& operator=(const Generic &);
  int MyObjectId () const;
  vdmBase* GetRef () const;
  bool IsInitialized() const;

  Bool SameBaseClass(const ObjectRef &) const;
  Bool IsOfClass(int) const;
  Bool IsOfBaseClass(int) const;
};

//
// DRPtr
//
class DRPtr : public Common {
public:
  DRPtr(MetaivVal * p);
  DRPtr(const DRPtr & p) { this->Link(p); };
  DRPtr(const Common & c) { this->operator=(c); };
  DRPtr & operator=(const Common & g);
  Common GetVal() const { return Common(GetPtr()); };
  int GetRC() const { return GetPtr()->GetRefCount(); };
private:
  MetaivVal * GetPtr() const;
};

//
// Time
//
class Time : public Common {
public:
  Time();
  Time(const Time & t) { this->Link(t); };
  Time(const Common & c) { this->operator=(c); };
  Time & operator=(const Common & g);
  wstring GetString(const string & format) const;
};

//
// Function
//
class Function : public Common {
public:
  typedef Sequence (*vdm_function_ptr)(const Sequence & args_l);

public:
  Function();
  Function(const wchar_t* fnm, vdm_function_ptr);
  Function(const wstring & fnm, vdm_function_ptr);
  Function(const Function & t) { this->Link(t); };
  Function(const Common & c) { this->operator=(c); };
  Function & operator=(const Common & g);
  Sequence Apply(const Sequence & args) const;
  wstring GetName() const;
};

//
unsigned int GetVDMLibVersion();

//
//
//
Generic ReadVal(istream&);
int64_t ReadInteger(istream&);
wstring StringNumber(int);

// VDMSetTagMap(const Map& m) is no longer used.
// VDMTagDump(ostream& os) is no longer used.

//////////////////////////////////////////////////
// Error Handling
typedef void (*VDMErrorHandler)(int error, int exi);

VDMErrorHandler
VDMSetErrorHandler(VDMErrorHandler fct);

void
VDMErrorHandle(int error, int exi);
// Exits if no handler has been installed with VDMSetErrorHandler.

enum VDMMajorError {
  VDM_ERR_LIB = 1
};

//
struct vdmdlfunctions {
  unsigned int (*vdmgetvdmlibversion)();
  VDMRecInfoMap * (*vdmgetdefaultrecinfomapptr)();
  bool (*vdmdlgettag)(VDMRecInfoMap *, const wchar_t *s, int *tag, int *size);
  const wchar_t * (*vdmdlgetsymtag)(VDMRecInfoMap *, int tag);
};

extern "C" {
  void vdmdlgetfunctions(vdmdlfunctions *);

#ifdef _USRDLL // User wants to build a DLModule for NT
  __declspec(dllexport) void dll_vdmdlgetfunctions (vdmdlfunctions *f)
  {
    vdmdlgetfunctions (f);
  }
#endif

};

//////////////////////////////////////////////////
//////////////////////////////////////////////////

template <class T> class SET : public Set
{
public:
  SET<T>(): Set() {};
  SET<T>(const SET<T> & g) : Set(g) {};
  SET<T>(const Generic & g) : Set(g) {};
  SET<T>(const Set & g) : Set(g) {}; // ???

  SET<T> & operator=(const SET<T> & s) { return (SET<T> &)Set::operator=(s); };
  SET<T> & operator=(const Generic & s) { return (SET<T> &)Set::operator=(s); };

  SET<T> & Insert(const T & e) { return (SET<T> &) Set::Insert(e); };
  bool InSet(const T & e) const { return Set::InSet(e); };
  SET<T> & ImpUnion(const SET<T> & s) { return (SET<T> &) Set::ImpUnion(s); };
  SET<T> & ImpIntersect(const SET<T> & s) { return (SET<T> &) Set::ImpIntersect(s); };
  SET<T> & ImpDiff(const SET<T> & s) { return (SET<T> &) Set::ImpDiff(s); };
  T GetElem() const { return (T) Set::GetElem(); };
  SET<T> & RemElem(const T & e) { return (SET<T> &) Set::RemElem(e); };
  bool SubSet(const SET<T> & s) const { return Set::SubSet(s); };

//  bool First(Generic & e) const { return Set::First(e); };
//  bool Next(Generic & e) const { return Set::Next(e); };
//???  bool First(const T & e) const { return Set::First(e);};
//???  bool Next(const T & e) const { return Set::Next(e);};
};

///////////////////////////////////////////
// THE SEQUENCE TEMPLATE CLASS
///////////////////////////////////////////

template <class T> class SEQ : public Sequence
{
public:
  SEQ<T>() : Sequence() {};
  SEQ<T>(const SEQ<T> & s) : Sequence(s) {};
  SEQ<T>(const Generic & g) : Sequence(g) {};
  SEQ<T>(const Sequence & g) : Sequence(g) {};

  SEQ<T> & operator=(const SEQ<T> & s)
  { return (SEQ<T> &) Sequence::operator=(s); };
  SEQ<T> & operator=(const Generic & s)
  { return (SEQ<T> &) Sequence::operator=(s); };

  T Hd() const { return (T) Sequence::Hd(); };
  const T & Index(int64_t i) const { return (const T &) Sequence::CIndex(i); };
  const T & operator[](int64_t i) const { return (const T &) Sequence::CIndex(i); };

  SEQ<T> Tl() const { return (SEQ<T>) Sequence::Tl(); };
  SEQ<T> & ImpTl() { return (SEQ<T> &) Sequence::ImpTl(); };
  SEQ<T> & ImpConc(const SEQ<T> & s)
  { return (SEQ<T> &) Sequence::ImpConc(s); };
  SEQ<T> & ImpAppend(const T & e)
  { return (SEQ<T> &) Sequence::ImpAppend(e); };
  SEQ<T> operator^(const SEQ<T> & s) const
  { return (SEQ<T>) Sequence::operator^(s); };
  const T & Last() const { return (const T &) Sequence::CLast(); };

  SEQ<T> & ImpPrepend(const T & e)
  { return (SEQ<T> &) Sequence::ImpPrepend(e); };
  SEQ<T> & RemElem(int64_t i) { return (SEQ<T> &) Sequence::RemElem(i); };
  SEQ<T> & ImpModify(int64_t j, const T & g)
  { return (SEQ<T> &) Sequence::ImpModify(j, g); };

  SET<T> Elems() const { return (SET<T>) Sequence::Elems(); };

  SEQ<T> SubSequence(int64_t from, int64_t to) const
  { return (SEQ<T>) Sequence::SubSequence(from, to); };

//  bool First(Generic & e) const { return Sequence::First(e);};
//  bool Next(Generic & e) const { return Sequence::Next(e);};
//  bool First(T & e) const { return Sequence::First((Generic &) e);};
//  bool Next(T & e) const { return Sequence::First((Generic &) e);};

  T & GetIndexRef(int64_t i) { return (T &) Sequence::GetIndexRef(i); };
};

/////////////////////////////////////////////////////////
// MAP
/////////////////////////////////////////////////////////

template <class D,class R> class MAP : public  Map {
public:
  MAP<D,R>() : Map() {};
  MAP<D,R>(const Generic & c) : Map(c) {};
  MAP<D,R>(const Map & c) : Map(c) {}; // ???
  MAP<D,R>(const MAP<D,R> & m) : Map(m) {};

  MAP<D,R> & operator=(const MAP<D,R> & m)
  { return (MAP<D,R> &) Map::operator=(m); };
  MAP<D,R> & operator=(const Generic & m)
  { return (MAP<D,R> &) Map::operator=(m); };

  MAP<D,R> Insert(const D & d, const R & r)
  { return (MAP<D,R>&) Map::Insert(d,r); };
  MAP<D,R> ImpModify(const D & d, const R & r)
  { return (MAP<D,R>&) Map::ImpModify(d,r); };

  const R & operator[](const D & d) const {return (const R &) Map::CApply(d); };
  MAP<D,R> ImpOverride(const MAP<D,R> & m )
  { return (MAP<D,R>&) Map::ImpOverride(m); };

  SET<D> Dom() const { return (SET<D>) Map::Dom(); };
  SET<R> Rng() const { return (SET<R>) Map::Rng(); };

  bool DomExists(const D & d) const { return Map::DomExists(d); };
  bool DomExists(const D & d, R & r) const { return Map::DomExists(d,r); };

  MAP<D,R> & RemElem(const D & c) { return (MAP<D,R> &) Map::RemElem(c); };

  R & GetRngRef(const D & c) { return (R &) Map::GetRngRef(c); };

//  bool First(Generic & d) const { return Map::First(d); };
//  bool Next(Generic & d) const { return Map::Next(d); };
//  bool First(Generic & d, Generic & r) const { return Map::First(d, r); };
//  bool Next(Generic & d, Generic & r)const { return Map::Next(d, r); };
//  bool First(D& d ) const { return Map::First(d);};
//  bool Next(D& d ) const { return Map::Next(d);};
//  bool First(D& d,R&r ) const { return Map::First(d,r);};
//  bool Next(D& d, R& r ) const { return Map::Next(d,r);};
};
#endif // __metaiv_h__

// Force emacs to use C++ mode for this file.
//// Local Variables:
//// mode: c++
//// End:

