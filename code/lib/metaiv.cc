/***
*  * WHAT
*  *    VDM library interface implementation
*  * FILE
*  *    $Id: metaiv.cc,v 1.133 2006/06/23 07:28:17 vdmtools Exp $
*  * PROJECT
*  *    VDM Toolbox
*  * COPYRIGHT
*  *    (C) Kyushu University
***/

/**************************************************************************
*  This file contains the implementation of the definitions
*  in the files metaiv.v and basic.h
*  These functions implements the interface to the VDM C++ library.
*  In IPTES-IFAD-95 the interface is described and the structure
*  of the library is outlined.
**************************************************************************/

#include <iostream>
#include <fstream>
#include <stdio.h>
#include <typeinfo>

#include "metaiv.h"
#include "vdmlib_values.h"
#include <sstream>

#include <algorithm>

#include "main_tag.h"
#include "backslash.h"
#include "tb_wstring.h"

// Initialized to 0. Do not insert `= 0'.
#ifdef SHOW_MALLOC_STATS
int MetaivVal::BuildCount;
int MetaivVal::DestroyCount;
ShowMallocCount * showmalloccount; /*  */
#endif // SHOW_MALLOC_STATS

unsigned int VDMLibVersion = VDMLIBVERSION;

unsigned int GetVDMLibVersion()
{
  return VDMLibVersion;
}

//======================================================================
// Hash function for improving recursive comparison of structured
// VDM values.
//======================================================================

VDMFormatter VDM_default_formatter, VDM_default_formatter_PR_ASCII;

//======================================================================

/**************************************************************************
*  If ML_TRACE is on, it will be recorded in the file
*  m4trace, each time a constructor or destructor is called.
**************************************************************************/

Common Common::dummy;

VDMLibControl Common::vdm_lib_initialise2(VDMLIBVERSION);

#ifdef ML_TRACE
long MetaivVal::metaivval_object_create_count;
long Common::metaiv_object_create_count;
wstring traceHeader( MetaivVal *obj );
wstring traceHeader( Common *obj );

wofstream* m4trace;
//int m4DebugLevel = 0;
int m4DebugLevel = 3;

int getM4DebugLevel()
{
  return m4DebugLevel;
}

// setM4DebugLevel dictates the level of debugging:
//  0: Construction and destruction of MetaivVal
//  1: Construction and destruction of Common
//  2: Construction and destruction of internal containers
//  3: Construction and destruction of metaiv values
// Of course if x > y, the x includes debug messages for level y
void setM4DebugLevel(int i)
{
  if (i >= 0 && i < 4) {
    m4DebugLevel = i;
  }
  else {
    cerr << "setM4DebugLevel: argument must be between 0 and 3" << endl;
  }
}

wofstream* getM4trace()
{
  if (!m4trace) {
    m4trace = new wofstream("m4trace");
  }

  return m4trace;
}

void Common::traceObj( const wchar_t *method )
{
  if ( m4DebugLevel > 2 ) {
    (*getM4trace()) << ""
                    << traceHeader( this->p ) << " "
                    << method
                    << L" MetaivId=" << this->metaiv_object_id
                    << L" MetaivValId=" << this->p->metaivval_object_id
                    << endl;
    (*getM4trace()) << L""
                    << traceHeader( this ) << " "
                    << method
                    << L" MetaivId=" << this->metaiv_object_id
                    << L" MetaivValId=" << this->p->metaivval_object_id
                    << endl;
  }
}

void MetaivVal::traceObj( const wchar_t *method )
{
  if ( m4DebugLevel > 0 ) {
    (*getM4trace()) << L""
                    << traceHeader( this ) << " "
                    << method
                    << L" Type=" << PrintType( this )
                    << L" MetaivValId=" << this->metaivval_object_id
                    << L" BuildCount=" << this->BuildCount
                    << L" RefCount=" << this->RefCount
                    << L" Refs:";
    for( unsigned int i = 0; i < this->m_list.size(); i++ ) {
      long id = this->m_list.at(i);
      (*getM4trace()) << L" " << id;
    }
    (*getM4trace()) << endl;
  }
}

wstring traceHeader( MetaivVal *obj )
{
  wstring ret;
  wchar_t hebuf[20];
  if( obj->seq_count == 0 ) {
    swprintf( hebuf, 20, L" %08d\n", obj->metaivval_object_id );
    ret += hebuf;
  }
  swprintf( hebuf, 20, L" %08d-%06d", obj->metaivval_object_id, obj->seq_count );
  obj->seq_count++;
  ret += hebuf;
  return ret;
}

wstring traceHeader( Common *obj )
{
  wstring ret;
  wchar_t hebuf[20];
  if( obj->seq_count == 0 ) {
    swprintf( hebuf, 20, L"C%08d\n", obj->metaiv_object_id );
    ret += hebuf;
  }
  swprintf( hebuf, 20, L"C%08d-%06d", obj->metaiv_object_id, obj->seq_count );
  obj->seq_count++;
  ret += hebuf;
  return ret;
}
#endif //ML_TRACE

/**************************************************************************
*
*  Misc. Functions
*
**************************************************************************/

wstring ConvertChar(int e)
{
  wchar_t ch = '0' + e;
  return wstring() = ch;
}

wstring StringNumber(int i)
{
  if (i < 0) {
    return L"-" + StringNumber(-i);
  }
  else if (i / 10 == 0) {
    return ConvertChar(i);
  }
  else {
    return StringNumber(i / 10) +  ConvertChar(i % 10);
  }
}

void VDMFormatter::print_spaces(wostream&os, int indent)
{
  for (int i = 0; i < indent; i++) {
    os << L" ";
  }
}

std::wstring string2wstring(const std::string & s)
{
  return TBWSTR::string2wstring(s);
}

std::string wstring2string(const std::wstring & ws)
{
  return TBWSTR::wstring2string(ws);
}

std::wstring cinstr2wstring(const std::string & s)
{
  return TBWSTR::cinstr2wstring(s);
}

std::string wstring2coutstr(const std::wstring & ws)
{
  return TBWSTR::wstring2coutstr(ws);
}

std::wstring fsstr2wstring( const std::string & s )
{
  return TBWSTR::fsstr2wstring(s);
}

std::string wstring2fsstr(const std::wstring & ws)
{
  return TBWSTR::wstring2fsstr(ws);
}

std::wstring utf8str2wstring( const std::string & s )
{
  return TBWSTR::utf8str2wstring(s);
}

std::string wstring2utf8str( const std::wstring & ws )
{
  return TBWSTR::wstring2utf8str(ws);
}

/**************************************************************************
*
*  MetaivVal
*
*  MetaivVal is defined in basic.h
**************************************************************************/
/*
void MetaivVal::delete_this()
{
#ifdef ML_TRACE
  traceObj( L"MetaivVal::delete_this()" );
#endif // ML_TRACE
#ifdef SHOW_MALLOC_STATS
  MetaivVal::BuildCount--;
  MetaivVal::DestroyCount++;
#endif // SHOW_MALLOC_STATS

  delete this;
}
*/

// (Semi)dirty. Update cache fingerprint values of const object!
// Cast away const'ness to do this.
#ifdef VDM_FINGERPRINT
inline
void MetaivVal::setfingerprint(Fingerprint& fp) const
{
  MetaivVal &non_const = (MetaivVal&)*this;
  non_const.fp.value = fp.value;
  non_const.fp.setvalid(true);
}

void MetaivVal::updatefingerprint() const
{
  printf("MetaivVal::updatefingerprint -  This function should newer be called"); // LTO
}

inline
void MetaivVal::readfingerprint(Fingerprint& fingerprint) const
{
  if (! fp.valid()) {
    updatefingerprint();
  }
  fingerprint = fp;
}
#endif // VDM_FINGERPRINT

int MetaivVal::Compare(const MetaivVal & m) const
{
  cerr << L"base MetaivVal::Compare called\n";
  exit(1);
  return 0;
}

wstring MetaivVal::PrintType() const
{
  switch (this->type) {
    case mt_nil: return L"Nil";
    case mt_char: return L"Char";
    case mt_int: return L"Int";
    case mt_real: return L"Real";
    case mt_quote: return L"Quote";
    case mt_tuple: return L"Tuple";
    case mt_record: return L"Record";
    case mt_set: return L"Set";
    case mt_map: return L"Map";
    case mt_text: return L"Text";
    case mt_token: return L"Token";
    case mt_bool: return L"Bool";
    case mt_sequence: return L"Sequence";
    case mt_objectref: return L"ObjectRef";
    case mt_generic: return L"Generic";
    case mt_undef: return L"Undef";
    case mt_dlobject: return L"DLObject";
    case mt_drptr: return L"Pointer";
    case mt_time: return L"Time";
    case mt_function: return L"Function";
    default: {
      wstring unknown = L"UNKNOWN TYPE (typeid=";
      unknown += TBWSTR::string2wstring(typeid(this).name());
      unknown += L")";
      return unknown;
    }
  }
}

int MetaivVal::CompareMetaivType(MetaivVal * m) const
{
  MetaivVal::metaivType t1 (this->type);
  MetaivVal::metaivType t2 (m->type);
  if (t1 == t2) {
    return 0;
  }
  else {
    return ((t1 < t2) ? -1 : 1);
  }
}
//////////////////////////////////////////////////////////////////////

class VDMValCache
{
typedef std::map<RealVal::RealValueType, RealVal*> RealValMapType;
typedef std::map<wstring, SequenceVal*> StringValMapType;
typedef std::map<wstring, QuoteVal*> QuoteValMapType;

public:
  BoolVal * const falsev ;
  BoolVal * const truev ;
  CharVal ** const charv;
  NilVal * const nilv;
  GenericVal * const genericv;
  RecordVal * const recordv;
  TupleVal * const tuplev;
  QuoteVal * const quotev;
  TextVal * const textv;
  TokenVal * const tokenv;
  RealVal ** const realv;
  IntVal ** const intv;

private:
  RealValMapType rvm;
  StringValMapType svm;
  QuoteValMapType qvm;
public:
  VDMValCache() :
    falsev(new BoolVal(false)),
    truev(new BoolVal(true)),
    charv(new CharVal*[256]),
    nilv(new NilVal()),
    genericv(new GenericVal()),
    recordv(new RecordVal() ),
    tuplev(new TupleVal()),
    quotev(new QuoteVal()),
    textv(new TextVal()),
    tokenv(new TokenVal()),
    realv(new RealVal*[256]),
    intv(new IntVal*[256])
  {
    this->falsev->AddRef();
    this->truev->AddRef();

    for (unsigned int i = 0; i < 256; i++) {
      this->charv[i] = new CharVal(static_cast<CharVal::CharValueType>(i));
      this->charv[i]->AddRef();
      this->realv[i] = new RealVal(static_cast<RealVal::RealValueType>(i));
      this->realv[i]->AddRef();
      this->intv[i] = new IntVal(static_cast<IntVal::IntValueType>(i));
      this->intv[i]->AddRef();
    }

    this->nilv->AddRef();
    this->genericv->AddRef();
    this->recordv->AddRef();
    this->tuplev->AddRef();
    this->quotev->AddRef();
    this->textv->AddRef();
    this->tokenv->AddRef();
  };
  ~VDMValCache() {
    this->falsev->RemoveRef();
    this->truev->RemoveRef();

    for (unsigned int i = 0; i < 256; i++) {
      this->charv[i]->RemoveRef();
      this->realv[i]->RemoveRef();
      this->intv[i]->RemoveRef();
    }

    this->nilv->RemoveRef();
    this->genericv->RemoveRef();
    this->recordv->RemoveRef();
    this->tuplev->RemoveRef();
    this->quotev->RemoveRef();
    this->textv->RemoveRef();
    this->tokenv->RemoveRef();

    for (RealValMapType::const_iterator miter(this->rvm.begin()); miter != this->rvm.end(); miter++) {
      miter->second->RemoveRef();
    }
    this->rvm.clear();
    for (StringValMapType::const_iterator miter(this->svm.begin()); miter != this->svm.end(); miter++) {
      miter->second->RemoveRef();
    }
    this->svm.clear();
    for (QuoteValMapType::const_iterator miter(this->qvm.begin()); miter != this->qvm.end(); miter++) {
      miter->second->RemoveRef();
    }
    this->qvm.clear();
  }

  RealVal * GetCachedRealVal(RealVal::RealValueType i) {
    RealValMapType::const_iterator miter (this->rvm.find(i));
    if (miter != this->rvm.end()) {
      return miter->second;
    } else {
      RealVal * rv = new RealVal(i);
      if (this->rvm.size() < 10000) {
        rv->AddRef();
        this->rvm.insert(RealValMapType::value_type(i, rv));
      }
      return rv;
    }
  }

  SequenceVal * GetCachedStringVal(const wstring & str) {
    StringValMapType::const_iterator miter (this->svm.find(str));
    if (miter != this->svm.end()) {
      return miter->second;
    } else {
      SequenceVal * sv = new SequenceVal(str);
      if (this->svm.size() < 10000) {
        sv->AddRef();
        this->svm.insert(StringValMapType::value_type(str, sv));
      }
      return sv;
    }
  }

  QuoteVal * GetCachedQuoteVal(const wstring & str) {
    QuoteValMapType::const_iterator miter (this->qvm.find(str));
    if (miter != this->qvm.end()) {
      return miter->second;
    } else {
      QuoteVal * qv = new QuoteVal(str);
      if (this->qvm.size() < 10000) {
        qv->AddRef();
        this->qvm.insert(QuoteValMapType::value_type(str, qv));
      }
      return qv;
    }
  }
};

static VDMValCache *ValCache = NULL;

static VDMRecInfoMap * DefRecInfo = NULL;

static VDMSerializeInfoMap * DefSerializeInfo = NULL;

/**************************************************************************
*
*  Common
*
**************************************************************************/
Common::Common()
{
#ifdef ML_TRACE
  traceObj( L"Common::Common()" );
  this->metaiv_object_id = Common::metaiv_object_create_count++;
  this->seq_count = 0;
#endif // ML_TRACE

  this->mvp = ValCache->genericv;
  this->AddRef();
}

/**************************************************************************
*  Common::Link is used to link a metaiv variabel to a metaiv value
*  pointed to by mivp.
**************************************************************************/

/**************************************************************************
*  Common::ChangeComplex is called before a metaiv value is changed.
*  The function copies the value of other metaiv variables points
*  the value.
**************************************************************************/
/*
inline
void Common::ChangeComplex(int* CopyCount, bool force)
{
  if ((force || p->RefCount>1) && VDMLibControl::CopyOnModify) {
    MetaivVal *mivp = p->Copy();
    Link(mivp);
    (*CopyCount)++;
  }
#ifdef VDM_FINGERPRINT
  p->fp.setvalid(false);
#endif // VDM_FINGERPRINT
}

inline
void Common::ChangeSimple(int* CopyCount)
{
  if (p->RefCount >1) {
    MetaivVal *mivp = p->Copy();
    Link(mivp);
    (*CopyCount)++;
  }
#ifdef VDM_FINGERPRINT
  p->fp.setvalid(false);
#endif // VDM_FINGERPRINT
}
*/

/**************************************************************************
*  Common::Compare compares the values of two metaiv variables.
*  by first comparing the types and then if the types are the
*  same by comparing the values. The result is one of the following:
*
*  -1 : this < g
*   0 : this == g
*   1 : this > g
**************************************************************************/
int Common::Compare(const Common & g) const
{
  if (this->mvp == g.mvp) {
    return 0;
  }

  int cmt = this->mvp->CompareMetaivType(g.mvp);
  if (cmt == 0) {
    if (this->IsContainer()) {
      // Containers require special compare.
      typedef const type_info & TI;
      TI ti1(typeid(*this->mvp)), ti2(typeid(*g.mvp));
      if (ti1 == ti2) {
        return this->mvp->Compare(*g.mvp);
      }
      else {
        return (ti1.before(ti2)) ? -1 : 1;
      }
    }
    else {
      return this->mvp->Compare(*g.mvp);
    }
  }
  else if (this->IsInt() && g.IsReal()) {
    return this->mvp->Compare(*g.mvp);
  }
  else if (this->IsReal() && g.IsInt()) {
    return this->mvp->Compare(*g.mvp);
  }
  else {
    return cmt;
  }
}

wostream& operator<<(wostream & os, const Common & c)
{
  VDMFormatter vf = VDM_default_formatter_PR_ASCII;
  vf.SetFlag(vf.doublequoteontoken);
  vf.SetFlag(vf.prettyprint);
//  vf.SetIndent(2);
  vf.SetIndent(0);
  c.ostream_out(os, vf);
  return os;
}

wstring Common::ascii() const
{
  wostringstream os;
#ifdef _MSC_VER
  //
  // to tell wostream to consult the global locale if it cannot find the
  // requested facet.
  //
  // Toolbox sets neccesary locale, but generated code
  // do not, that's why there could be unknown exception thrown
  // while generated code outputs something to wide string streams
  //
  os.imbue(locale::empty());
#endif // _MSC_VER
  ostream_out(os, VDM_default_formatter);
  wstring s (os.str());
  return s;
}

void Common::pr_ascii(wostream & os, int ind) const
{
  VDMFormatter vf = VDM_default_formatter_PR_ASCII;
  if (ind >= 0) {
    vf.SetFlag(vf.prettyprint);
    vf.SetIndent(ind);
  }
  this->mvp->ostream_out(os, vf);
}

bool Common::Is(const int tag) const
{
  if (this->mvp->IsRecordVal()) {
    return static_cast<const RecordVal *>(mvp)->Is(tag);
  }
  else {
    return false;
  }
}

/**************************************************************************
*
*  Generic
*
**************************************************************************/

Generic::Generic() : Common(ValCache->genericv)
{
}

/**************************************************************************
*
*  MapVal
*
**************************************************************************/
MapVal & MapVal::Insert(const Common & gdom, const Common & gran)
{
  pair<MapValueType::iterator, bool> p (this->value.insert(MapValueType::value_type(gdom, gran)));

  if (!p.second) { // Existed!
    if (p.first->second.Compare(gran) != 0) {
      M4LibError::ReportError(ML_CONFLICTING_RNGVAL,L"Map::Insert: conflicting range values.");
    }
  }
  return *this;
}

MapVal & MapVal::ImpModify(const Common & gdom, const Common & gran)
{
  pair<MapValueType::iterator, bool> p (this->value.insert(MapValueType::value_type(gdom, gran)));
  if (!p.second) { // Existed!
    // This is reentrant safe since we keep a ref to the value to be overwritten.
    Generic keepref (p.first->second);
    p.first->second = gran;
  }
  return *this;
}

const Common & MapVal::operator[](const Common & gdom) const
{
  MapValueType::const_iterator miter (this->value.find(gdom));
  if (miter != this->value.end()) {
    return miter->second;
  } else {
    M4LibError::ReportError(ML_NOT_IN_DOM,L"Map::Get: key not in the domain of map.");
    return Common::dummy;
  }
}

MapVal & MapVal::ImpOverride(const MapVal & m)
{
  if( !m.value.empty() ) {
    for (MapValueType::const_iterator miter(m.value.begin()); miter != m.value.end(); miter++) {
      pair<MapValueType::iterator, bool> p (this->value.insert(*miter));
      if (!p.second) { // Existed!
        // This is reentrant safe since we keep a ref to the value to be overwritten.
        Generic keepref (p.first->second);
        p.first->second = miter->second;
      }
    }
  }
  return *this;
}

Map MapVal::Override(const MapVal & m) const
{
  Map tm;
  MapVal & mv (tm.SHAREDGETVAL(MapVal));
  mv.value.insert(this->value.begin(), this->value.end());
  if( !m.value.empty() ) {
    for (MapValueType::const_iterator miter(m.value.begin()); miter != m.value.end(); miter++) {
      pair<MapValueType::iterator, bool> p (mv.value.insert(*miter));
      if (!p.second) { // Existed!
        // This is reentrant safe since we keep a ref to the value to be overwritten.
        Generic keepref (p.first->second);
        p.first->second = miter->second;
      }
    }
  }
  return tm;
}

Set MapVal::Dom() const
{
  Set ts;
  if( !this->value.empty() ) {
    SetVal & sv (ts.SHAREDGETVAL(SetVal));
    for (MapValueType::const_iterator miter(this->value.begin()); miter != this->value.end(); miter++) {
      sv.value.insert(sv.value.end(), miter->first);
    }
  }
  return ts;
}

Set MapVal::Rng() const
{
  Set ts;
  if( !this->value.empty() ) {
    SetVal & sv (ts.SHAREDGETVAL(SetVal));
    for (MapValueType::const_iterator miter(this->value.begin()); miter != this->value.end(); miter++) {
      sv.value.insert(sv.value.end(), miter->second);
    }
  }
  return ts;
}

bool MapVal::DomExists(const Common & c) const
{
  return (this->value.find(c) != this->value.end());
}

bool MapVal::DomExists(const Common & c, Generic & res) const
{
  MapValueType::const_iterator miter (this->value.find(c));
  if (miter != this->value.end()) {
    res = miter->second;
    return true;
  } else {
    return false;
  }
}

bool MapVal::DomExists(const Common & c, Common ** res) 
{
  MapValueType::iterator miter (this->value.find(c));
  if (miter != this->value.end()) {
    *res = &(miter->second);
    return true;
  } else {
    return false;
  }
}

Common & MapVal::GetRngRef(const Common & c)
{
  MapValueType::iterator miter (this->value.find(c));
  if (miter != this->value.end()) {
    return miter->second;
  } else {
    M4LibError::ReportError(ML_NOT_IN_DOM, L"Map::RemElem: key not in the domain of map");
    return Common::dummy;
  }
}

MapVal & MapVal::RemElem(const Common & gdom)
{
  const MapValueType::iterator miter (value.find(gdom));
  if (miter != this->value.end()) {
    // This is reentrant safe since we keep a ref to the value to be erased.
    MapValueType::value_type keepref (*miter);
    this->value.erase(miter);
  } else {
    M4LibError::ReportError(ML_NOT_IN_DOM, L"Map::RemElem: key not in the domain of map");
  }
  return *this;
}

Map MapVal::Inverse() const
{
  Map tm;
  if( !this->value.empty() ) {
    MapVal & mv (tm.SHAREDGETVAL(MapVal));
    for (MapValueType::const_iterator miter(this->value.begin()); miter != this->value.end(); miter++) {
      mv.Insert(miter->second, miter->first);
    }
  }
  return tm;
}

bool MapVal::IsCompatible(const MapVal & m) const
{
  Set s (this->Dom().ImpIntersect(m.Dom()));
  const SetVal & sv (s.CGETVAL(SetVal));
  bool forall = true;
  if (!sv.IsEmpty()) {
    SetVal::SetValueType::const_iterator siter;
    for (siter = sv.value.begin(); siter != sv.value.end() && forall; siter++) {
      forall = (this->value.find(*siter)->second == m.value.find(*siter)->second); 
    }
  }
  return forall;
}

Map MapVal::DomRestrictedTo(const SetVal & s) const
{
  Map tm;
  MapVal & mv (tm.SHAREDGETVAL(MapVal));
  for (MapValueType::const_iterator miter(this->value.begin()); miter != this->value.end(); miter++) {
    if(s.InSet(miter->first)) {
      mv.value.insert(*miter);
    }
  }
  return tm;
}

Map MapVal::DomRestrictedBy(const SetVal & s) const
{
  Map tm;
  MapVal & mv (tm.SHAREDGETVAL(MapVal));
  for (MapValueType::const_iterator miter(this->value.begin()); miter != this->value.end(); miter++) {
    if(!s.InSet(miter->first)) {
      mv.value.insert(*miter);
    }
  }
  return tm;
}

Map MapVal::RngRestrictedTo(const SetVal & s) const
{
  Map tm;
  MapVal & mv (tm.SHAREDGETVAL(MapVal));
  for (MapValueType::const_iterator miter(this->value.begin()); miter != this->value.end(); miter++) {
    if(s.InSet(miter->second)) {
      mv.value.insert(*miter);
    }
  }
  return tm;
}

Map MapVal::RngRestrictedBy(const SetVal & s) const
{
  Map tm;
  MapVal & mv (tm.SHAREDGETVAL(MapVal));
  for (MapValueType::const_iterator miter(this->value.begin()); miter != this->value.end(); miter++) {
    if(!s.InSet(miter->second)) {
      mv.value.insert(*miter);
    }
  }
  return tm;
}

/*
bool MapVal::First(Common & gdom, Common & gran,
                   MapValueType::const_iterator & traverseIter) const
{
  if (!this->value.empty()) {
    traverseIter = this->value.begin();
    gdom = traverseIter->first;
    gran = traverseIter->second;
    return true;
  } else {
    return false;
  }
}

bool MapVal::Next(Common & gdom, Common & gran,
                  MapValueType::const_iterator & traverseIter) const
{
  traverseIter++;
  if (traverseIter != this->value.end()) {
    gdom = traverseIter->first;
    gran = traverseIter->second;
    return true;
  } else {
    return false;
  }
}
*/
bool MapVal::First(Common & gdom, Common & gran, MapValIterator & iterator) const
{
  if (!this->value.empty()) {
    if (iterator.traverseIterP != NULL) delete iterator.traverseIterP;
    iterator.traverseIterP = new MapVal::MapValueType::const_iterator(this->value.begin());
    gdom = (*iterator.traverseIterP)->first;
    gran = (*iterator.traverseIterP)->second;
    return true;
  } else {
    return false;
  }
}

bool MapVal::Next(Common & gdom, Common & gran, MapValIterator & iterator) const
{
  iterator.traverseIterP->operator++();
  if ((*iterator.traverseIterP) != this->value.end()) {
    gdom = (*iterator.traverseIterP)->first;
    gran = (*iterator.traverseIterP)->second;
    return true;
  } else {
    return false;
  }
}

int MapVal::Compare(const MetaivVal & mval) const
{
  const MapVal & m (static_cast<const MapVal &>(mval));
  if (this->value.size() < m.value.size()) {
    return -1;
  }
  else if (this->value.size() > m.value.size()) {
    return 1;
  }
  else {
    for (MapValueType::const_iterator miter1(this->value.begin()), miter2(m.value.begin());
         miter1 != this->value.end();
         miter1++, miter2++) {
      int cmp = miter1->first.Compare(miter2->first);
      if (cmp != 0) return cmp;
      int cmp2 = miter1->second.Compare(miter2->second);
      if (cmp2 != 0) return cmp2;
    }
    return 0;
  }
}

bool MapVal::IsSimple() const
{
  //return this->value.empty();
  bool is_simple = true;
  for (MapValueType::const_iterator miter(this->value.begin()); (miter != this->value.end()) && is_simple; miter++) {
    is_simple = miter->first.IsSimple() && miter->second.IsSimple();
  }
  return is_simple;
}

void MapVal::WriteVal(ostream & ostr) const
{
  ostr << 'a' << value.size() << ",";

  for (MapValueType::const_iterator miter(this->value.begin()); miter != this->value.end(); miter++) {
    miter->first.WriteVal (ostr);
    miter->second.WriteVal(ostr);
  }
}

void MapVal::ostream_out(wostream & os, const VDMFormatter & vf) const
{
  VDMFormatter vf2 = vf;

  bool is_simple_d = true;
  bool is_simple_r = true;
  bool multiline_d = false;
  bool multiline_r = false;
  if (vf.PrettyPrinting()) {
    vf2.IncrementIndent();
    for (MapValueType::const_iterator miter(this->value.begin()); (miter != this->value.end()); miter++) {
      is_simple_d = miter->first.IsSimple() && is_simple_d;
      is_simple_r = miter->second.IsSimple() && is_simple_r;
    }
    if (!is_simple_d) {
      multiline_d = true;
    }
    if (!is_simple_r) {
      multiline_r = true;
    }
  }

  os << L"{ ";

  if (this->value.empty()) {
    os << L"|->";
  }
  else {
    bool first = true;
    int indent = vf2.GetIndent();
    for (MapValueType::const_iterator miter(this->value.begin()); miter != this->value.end(); miter++) {
      vf2.SetIndent( indent );
      if (!first) {
        os << L",";
        //if (multiline_d) {
        if (multiline_d || multiline_r) {
          os << endl;
          vf2.print_spaces(os, vf2.GetIndent());
        }
        else {
          os << L" ";
        }
      }
      else {
        first = false;
      }

      miter->first.ostream_out(os, vf2);
      os << L" |-> ";
      if (multiline_r) {
        os << endl;
        vf2.IncrementIndent();
        vf2.print_spaces(os, vf2.GetIndent());
      }
      miter->second.ostream_out(os, vf2);
    }
  }
  os << L" }";
}

void MapVal::GC()
{
//wcout << L"GC: " << this->value.size() << L" : ";
  bool exists = true;
  while ( !this->value.empty() && exists ) {
    SetVal::SetValueType sv;
    for (MapValueType::const_iterator miter(this->value.begin()); miter != this->value.end(); miter++) {
      if (!(miter->first.get_mvp()->MultiLinked())) {
        sv.insert(sv.end(), miter->first);
      }
    }
    exists = !sv.empty();
    if( exists ) {
      SetVal::SetValueType::const_iterator siter;
      for (siter = sv.begin(); siter != sv.end(); siter++) {
        const MapValueType::iterator miter2 (this->value.find(*siter));
        MapValueType::value_type keepref (*miter2);
        this->value.erase(miter2);
      }
    }
  }
//wcout << this->value.size() << endl;
}

void MapVal::GC2(const Common & c)
{
  const MapValueType::iterator miter (this->value.find(c));
  if (miter != this->value.end()) {
    if (!(miter->first.get_mvp()->MultiLinked())) {
      // This is reentrant safe since we keep a ref to the value to be erased.
      MapValueType::value_type keepref (*miter);
      this->value.erase(miter);
    }
  }
}


/**************************************************************************
*
*  Map
*
**************************************************************************/
static inline
MapVal* GetMapVal()
{
  try {
    return new MapVal();
  }
  catch (bad_alloc &e) {
    M4LibError::ReportError(ML_NULL_POINTER, wstring(L"Memory Allocation failed."));
  }
  return NULL;
}

Map::Map() : Common(GetMapVal()), iteratorP(NULL)
{
}

Map::Map(const Generic & m) : Common(m), iteratorP(NULL)
{
  if (!this->IsMap()) {
    M4LibError::ReportError(ML_CAST_ERROR, m.PrintType() + L" casted to Map");
  }
}

Map::~Map()
{
  if (this->iteratorP) delete this->iteratorP;
}

Map & Map::operator=(const Generic & g)
{
  if (!g.IsMap()) {
    M4LibError::ReportError(ML_CAST_ERROR, g.PrintType() + L" casted to Map");
  }

  this->Link(g);
  return *this;
}

void Map::Clear()
{
  this->Clone();
  static_cast<MapVal *>(mvp)->Clear();
  if (this->iteratorP) delete this->iteratorP;
  this->iteratorP = NULL;
}

Map & Map::Insert(const Common & gdom, const Common & gran)
{
  this->Clone();
  static_cast<MapVal *>(mvp)->Insert(gdom, gran);
  return *this;
}

Map & Map::ImpModify(const Common & gdom, const Common & gran)
{
  this->Clone();
  static_cast<MapVal *>(mvp)->ImpModify(gdom, gran);
  return *this;
}

const Generic & Map::operator[](const Common & gdom) const
{
  return (const Generic &)static_cast<const MapVal *>(mvp)->operator[](gdom);
}

const Generic & Map::Apply(const Common & gdom) const
{
  return (const Generic &)static_cast<const MapVal *>(mvp)->operator[](gdom);
}

const Common & Map::CApply(const Common & gdom) const
{
  return static_cast<const MapVal *>(mvp)->operator[](gdom);
}

Common & Map::GetRngRef(const Common & gdom)
{
  return static_cast<MapVal *>(mvp)->GetRngRef(gdom);
}

Map & Map::ImpOverride(const Map & m)
{
  Map m_copy (m);
  this->Clone();
  static_cast<MapVal *>(mvp)->ImpOverride(m_copy.CGETVAL(MapVal));
  return *this;
}

Map Map::Override(const Map & m) const
{
  return static_cast<const MapVal *>(mvp)->Override(m.CGETVAL(MapVal));
}

int Map::Size() const
{
  return static_cast<const MapVal *>(mvp)->Size();
}

Set Map::Dom() const
{
  return static_cast<const MapVal *>(mvp)->Dom();
}

bool Map::DomExists(const Common & c) const
{
  return static_cast<const MapVal *>(mvp)->DomExists(c);
}

bool Map::DomExists(const Common & c, Generic & res) const
{
  return static_cast<const MapVal *>(mvp)->DomExists(c, res);
}

bool Map::IsEmpty() const
{
  return static_cast<const MapVal *>(mvp)->IsEmpty();
}

Set Map::Rng() const
{
  return static_cast<const MapVal *>(mvp)->Rng();
}

Map Map::Inverse() const
{
  return static_cast<const MapVal *>(mvp)->Inverse();
}

bool Map::IsCompatible(const Map & m) const
{
  return static_cast<const MapVal *>(mvp)->IsCompatible(m.CGETVAL(MapVal));
}

Map Map::DomRestrictedTo(const Set & s) const
{
  return static_cast<const MapVal *>(mvp)->DomRestrictedTo(s.CGETVAL(SetVal));
}

Map Map::DomRestrictedBy(const Set & s) const
{
  return static_cast<const MapVal *>(mvp)->DomRestrictedBy(s.CGETVAL(SetVal));
}

Map Map::RngRestrictedTo(const Set & s) const
{
  return static_cast<const MapVal *>(mvp)->RngRestrictedTo(s.CGETVAL(SetVal));
}

Map Map::RngRestrictedBy(const Set & s) const
{
  return static_cast<const MapVal *>(mvp)->RngRestrictedBy(s.CGETVAL(SetVal));
}

Map & Map::RemElem(const Common& gdom)
{
  this->Clone();
  static_cast<MapVal *>(mvp)->RemElem(gdom);
  return *this;
}

bool Map::First(Generic & gdom)
{
  if (this->iteratorP == NULL) this->iteratorP = new MapValIterator();

  Generic dummy;
  return static_cast<const MapVal *>(mvp)->First(gdom, dummy, *iteratorP);
}

bool Map::Next(Generic & gdom)
{
  Generic dummy;
  return static_cast<const MapVal *>(mvp)->Next(gdom, dummy, *iteratorP);
}

bool Map::First(Generic & gdom, Generic & gran)
{
  if (this->iteratorP == NULL) this->iteratorP = new MapValIterator();

  return static_cast<const MapVal *>(mvp)->First(gdom, gran, *iteratorP);
}

bool Map::Next(Generic & gdom, Generic & gran)
{
  return static_cast<const MapVal *>(mvp)->Next(gdom, gran, *iteratorP);
}

void Map::GC()
{
  static_cast<MapVal *>(mvp)->GC();
}

void Map::GC2(const Common & c)
{
  static_cast<MapVal *>(mvp)->GC2(c);
}

size_t Map::MaxSize() const
{
  return static_cast<const MapVal *>(mvp)->MaxSize();
}

/**************************************************************************
*
*  SequenceVal
*
**************************************************************************/
inline
SequenceVal::SequenceVal(const wstring & s) : MetaivVal(mt_sequence), isString(true)
{
#ifdef SHOW_MALLOC_STATS
    showmalloccount->log_insert(typeid(*this).name(), this, sizeof(*this));
#endif // SHOW_MALLOC_STATS

  for (wstring::const_iterator it = s.begin(); it != s.end(); it++) {
    this->value.push_back(Char(*it));
  } 
}

int SequenceVal::Compare(const MetaivVal & mval) const
{
  const SequenceVal & s ((static_cast<const SequenceVal &>(mval)));

  if (this->value.size() < (size_t)s.Length()) {
    return -1;
  }
  else if (this->value.size() > (size_t)s.Length()) {
    return 1;
  }
  else {
#ifdef VDM_FINGERPRINT
    Fingerprint fp1, fp2;
    readfingerprint(fp1);
    s.readfingerprint(fp2);
    int fp_cmp = fp1.Compare(fp2);
    if (fp_cmp != 0) {
      return fp_cmp;
    }
#endif // VDM_FINGERPRINT

    int res = 0;
    SequenceValueType::const_iterator i1,i2;
    for (i1 = this->value.begin(), i2 = s.value.begin();
         (i1 != this->value.end()) && (res == 0); i1++, i2++) {
      res = (*i1).Compare(*i2);
    }
    return res;
  }
}

bool SequenceVal::IsSimple() const
{
  if (this->value.empty() || IsString()) {
    return true;
  }

  bool forall = true;
  SequenceVal::SequenceValueType::const_iterator siter;
  for (siter = this->value.begin(); (siter != this->value.end()) && forall; siter++) {
    forall = siter->IsSimple();
  }
  return forall;
}

void SequenceVal::ostream_out(wostream & os, const VDMFormatter & vf) const
{
  VDMFormatter vf2 (vf);

  bool all_char = (this->isString || this->value.size() > 0);
  bool is_simple = true;
  if (!this->isString) {
    SequenceValueType::const_iterator i1;
    for (i1 = this->value.begin(); i1 != this->value.end(); i1++) {
      all_char = all_char && i1->IsChar();
      is_simple = is_simple && i1->IsSimple();
    }
  }

  if (all_char) {
    os << L"\"";
    wstring s;
    this->GetString(s);
    Backslashed::convert_internal_to_printed_backslash(s, os, true);
    os << L"\"";
  }
  else {
    bool multiline = false;
    if (vf.PrettyPrinting()) {
      vf2.IncrementIndent();
      if (!is_simple) {
        multiline = true;
      }
    }

    os << L"[ ";
    bool first = true;
    SequenceValueType::const_iterator i1;
    for (i1 = this->value.begin(); i1 != this->value.end(); i1++) {
      if (!first) {
        os << L",";
        if (multiline) {
          os << endl;
          vf2.print_spaces(os, vf2.GetIndent());
        }
        else {
          os << L" ";
        }
      }
      else {
        first = false;
      }
      (*i1).ostream_out(os, vf2);
    }
    os << L" ]";
  }
}

void SequenceVal::WriteVal (ostream& ostr) const
{
  ostr << 'b' << (int) this->value.size() << ",";

  SequenceValueType::const_iterator i1;
  for (i1 = this->value.begin(); i1 != this->value.end(); i1++) {
    (*i1).WriteVal (ostr);
  }
}

Common & SequenceVal::refIndex(int64_t j) //const
{
  if (OutOfRange(j)) {
    M4LibError::ReportError(ML_INDEX_OUT_OF_RANGE,L"Sequence::Index");
    return Common::dummy; // Only executed if ReportError is changed.
  }
  return this->value[j-1];
}

const Common & SequenceVal::operator[](int64_t j) const
{
  if (OutOfRange(j)) {
    M4LibError::ReportError(ML_INDEX_OUT_OF_RANGE,L"Sequence::Index");
    return Common::dummy; // Only executed if ReportError is changed.
  }
  return this->value[j-1];
}

const Common & SequenceVal::Hd() const
{
  if (!this->value.empty()) {
    return this->value.front();
  }
  else {
    M4LibError::ReportError(ML_OP_ON_EMPTY_SEQ,L"'Hd' on empty Sequence");
    return Common::dummy;
  }
}

const Common & SequenceVal::Last() const
{
  if (!this->value.empty()) {
    return this->value.back();
  }
  else {
    M4LibError::ReportError(ML_OP_ON_EMPTY_SEQ,L"'Last' on empty Sequence");
    return Common::dummy;
  }
}

Sequence SequenceVal::Tl() const
{
  Sequence ts;
  if (!this->value.empty()) {
    SequenceVal & sv (ts.SHAREDGETVAL(SequenceVal));

//    sv.value = this->value;
//    sv.isString = this->isString;
//    sv.ImpTl();
    sv.value.insert(sv.value.begin(), this->value.begin() + 1, this->value.end());
    sv.isString = this->isString;
  }
  else {
    M4LibError::ReportError(ML_OP_ON_EMPTY_SEQ,L"'Tl' on empty Sequence");
  }
  return ts;
}

SequenceVal& SequenceVal::ImpTl()
{
  if (!this->value.empty()) {
    // This is reentrant safe since we keep a ref to the value to be erased.
    Generic keepref (*this->value.begin()); //
#ifdef USE_DEQUE
    this->value.pop_front();
#else
    this->value.erase(this->value.begin());
#endif // USE_DEQUE
  }
  else {
    M4LibError::ReportError(ML_OP_ON_EMPTY_SEQ,L"'ImpTl' on empty Sequence");
  }
  return *this;
}

Generic SequenceVal::ImpPop()
{
  if (!this->value.empty()) {
    Generic g (*this->value.begin()); //
#ifdef USE_DEQUE
    this->value.pop_front();
#else
    this->value.erase(this->value.begin());
#endif // USE_DEQUE
    return g;
  }
  else {
    M4LibError::ReportError(ML_OP_ON_EMPTY_SEQ,L"'Pop' on empty Sequence");
  }

  return Generic();
}

Sequence SequenceVal::ImpPop(int64_t n)
{
  if (static_cast<int64_t>(this->value.size()) < n) {
    M4LibError::ReportError(ML_OP_ON_EMPTY_SEQ,L"'Pop' on empty Sequence");
    return Sequence();
  }

  Sequence s;
  SequenceVal & sv (s.SHAREDGETVAL(SequenceVal));

  SequenceValueType::reverse_iterator rend = this->value.rend();
  sv.value.insert(sv.value.begin(), rend - n, rend);
  sv.isString = this->isString;
  SequenceValueType::iterator begin = this->value.begin();
  this->value.erase(begin, begin + n);
  return s;
}

SequenceVal & SequenceVal::ImpCopy()
{
  if (!this->value.empty()) {
    Generic item (*this->value.begin()); //
    this->value.insert(this->value.begin(), item);
  }
  else {
    M4LibError::ReportError(ML_OP_ON_EMPTY_SEQ,L"'ImpCopy' on empty Sequence");
  }
  return *this;
}

SequenceVal & SequenceVal::ImpSwap()
{
  if (this->value.size() > 1) {
    Generic item (*this->value.begin()); //
    this->value[0] = this->value[1];
    this->value[1] = item;
  }
  else {
    M4LibError::ReportError(ML_OP_ON_EMPTY_SEQ,L"'ImpSwap' on empty Sequence");
  }
  return *this;
}

wstring SequenceVal::GetString() const
{
  wstring s;
  GetString(s);
  return s;
}

bool SequenceVal::GetString(wstring & str) const
{
  wstring res;
  bool forall = true;
  SequenceVal::SequenceValueType::const_iterator i1;
  for (i1 = this->value.begin(); (i1 != this->value.end()) && forall; i1++) {
    forall = i1->IsChar();
    if (forall) {
      res += Char(*i1).GetValue();
    }
  }
  str = (forall ? res : wstring(L""));
  return forall;
}

bool SequenceVal::IsString() const
{
  if(!this->isString) {
    if(this->value.empty()) return false;
    SequenceVal::SequenceValueType::const_iterator siter;
    bool forall = true;
    for (siter = this->value.begin(); (siter != this->value.end()) && forall; siter++) {
      forall = siter->IsChar();
    }
    return forall;
  }
  return true;
}

SequenceVal & SequenceVal::ImpConc(const SequenceVal & v2)
{
  if ( !v2.value.empty() ) {
    this->value.insert(this->value.end(), v2.value.begin(), v2.value.end());
    this->isString &= v2.isString;
  }
  return *this;
}

Sequence SequenceVal::Conc(const SequenceVal & v2) const
{
  Sequence ts;
  SequenceVal & sv (ts.SHAREDGETVAL(SequenceVal));
  sv.value.insert(sv.value.end(), this->value.begin(), this->value.end());
  sv.isString = this->isString;
  if ( !v2.value.empty() ) {
    sv.value.insert(sv.value.end(), v2.value.begin(), v2.value.end());
    sv.isString &= v2.isString;
  }
  return ts;
}

SequenceVal & SequenceVal::RemElem(int64_t i)
{
  if (OutOfRange(i)) {
    M4LibError::ReportError(ML_RANGE_ERROR, L"Sequence::RemElem");
  }
  else {
    // This is reentrant safe //since we keep a ref to the value
    // to be erased.
    Generic keepref (*(this->value.begin()+(i-1)));
    this->value.erase(this->value.begin()+(i-1));
  }
  return *this;
}

SequenceVal & SequenceVal::ImpModify(int64_t j,  const Common& g)
{
  if (OutOfRange(j)) {
    M4LibError::ReportError(ML_INDEX_OUT_OF_RANGE,
                L"index out of range in 'Sequence::ImpModify'");
  }
  else {
    if (!g.IsChar()) {
      this->isString = false;
    }
    // This is reentrant safe since we keep a ref to the value
    // to be overwritten.
    Generic keepref (this->value[j-1]);
    this->value[j-1] = g;
  }
  return *this;
}

Set SequenceVal::Elems() const
{
  Set ts;
  ts.SHAREDGETVAL(SetVal).value.insert(this->value.begin(), this->value.end());
  return ts;
}

Set SequenceVal::Inds() const
{
  Set ts;
  if (!this->value.empty()) {
    SetVal & sv (ts.SHAREDGETVAL(SetVal));
    int len = this->value.size();
    for (int i = 1; i <= len; i++)
      sv.Insert(Int(i));
  }
  return ts;
}

Sequence SequenceVal::Reverse() const
{
  Sequence ts;
  SequenceVal & sv (ts.SHAREDGETVAL(SequenceVal));
  sv.value.insert(sv.value.begin(), this->value.rbegin(), this->value.rend());
  sv.isString = this->isString;
  return ts;
}

Set SequenceVal::Permute() const
{
  Set ts;
  if ( this->value.empty() ) {
    return ts;
  }

  SetVal & sv (ts.SHAREDGETVAL(SetVal));
  if ( this->value.size() == 1 ) {
    Sequence s;
    SequenceVal & tsv (s.SHAREDGETVAL(SequenceVal));
    tsv.value.insert(tsv.value.end(), this->value.begin(), this->value.end());
    tsv.isString = this->isString;
    sv.Insert(s);
    return ts;
  }

  SequenceVal::SequenceValueType::const_iterator i1;
  for (i1 = this->value.begin(); i1 != this->value.end(); i1++) {
    SequenceVal tsv;
    SequenceVal::SequenceValueType::const_iterator i2;
    for (i2 = this->value.begin(); i2 != this->value.end(); i2++) {
      if (i1 != i2) {
       tsv.value.push_back(*i2);
      }
    }
    tsv.isString = this->isString;
    Set perm_s (tsv.Permute());

    SetVal & perm_sv (perm_s.SHAREDGETVAL(SetVal));
    SetVal::SetValueType::const_iterator i3;
    for (i3 = perm_sv.value.begin(); i3 != perm_sv.value.end(); i3++) {
      Sequence tbd_l (*i3);
      tbd_l.SHAREDGETVAL(SequenceVal).ImpPrepend(*i1);
      sv.Insert(tbd_l);
    }
  }
  return ts;
}

Sequence SequenceVal::DistrSetProduct() const
{
  if (this->value.empty()) {
    return mk_sequence(Sequence());
  }

  Sequence tmp_l;
  SequenceVal & tmp_lv (tmp_l.SHAREDGETVAL(SequenceVal));
  tmp_lv.value.insert(tmp_lv.value.end(), this->value.begin(), this->value.end());

  Sequence ts (mk_sequence(Sequence())); // [ [] ]

  while (!tmp_lv.value.empty()) {
    Set elem (tmp_lv.value.front()); // Hd
    SetVal & elemv (elem.SHAREDGETVAL(SetVal));
#ifdef USE_DEQUE
    tmp_lv.value.pop_front(); // ImpTl
#else
    tmp_lv.value.erase(tmp_lv.value.begin()); // ImpTl
#endif // USE_DEQUE

    Sequence lts;
    SequenceVal & ltsv (lts.SHAREDGETVAL(SequenceVal));
    const SequenceVal & tsv (ts.SHAREDGETVAL(SequenceVal));
    for (SequenceVal::SequenceValueType::const_iterator i1 = tsv.value.begin(); i1 != tsv.value.end(); i1++)
    {
      for (SetVal::SetValueType::const_iterator i2 = elemv.value.begin(); i2 != elemv.value.end(); i2++) {
        Sequence l (*i1);
        ltsv.ImpAppend(l.ImpAppend(*i2));
      }
    }
    ts = lts;
  }
  return ts;
}

Sequence SequenceVal::SubSequence(int64_t from, int64_t to) const
{
  Sequence ts;
  SequenceVal & sv (ts.SHAREDGETVAL(SequenceVal));
  SequenceVal::SequenceValueType::const_iterator i1;
  int64_t index = 1;
  for (i1 = this->value.begin(); i1 != this->value.end(); i1++) {
    if( from <= index && index <= to ) {
      sv.value.push_back(*i1);
    }
    index++;
  }
  sv.isString = this->isString;
  return ts;
}

bool SequenceVal::StartsWith(const SequenceVal & l) const
{
  if (l.value.empty()) return true;
  if (l.value.size() > this->value.size()) return false;
  int res = 0;
  SequenceValueType::const_iterator i1, i2;
  for (i1 = l.value.begin(), i2 = this->value.begin();
       (i1 != l.value.end()) && (res == 0); i1++, i2++) {
    res = (*i1).Compare(*i2);
  }
  return (res == 0);
}

bool SequenceVal::EndsWith(const SequenceVal & l) const
{
  if (l.value.empty()) return true;
  if (l.value.size() > this->value.size()) return false;
  int res = 0;
  SequenceValueType::const_reverse_iterator i1, i2;
  for (i1 = l.value.rbegin(), i2 = this->value.rbegin();
       (i1 != l.value.rend()) && (res == 0); i1++, i2++) {
    res = (*i1).Compare(*i2);
  }
  return (res == 0);
}

Sequence SequenceVal::Product(const SetVal & s) const
{
  Sequence ts;
  SequenceVal & sv (ts.SHAREDGETVAL(SequenceVal));
  SetVal::SetValueType::const_iterator siter;
  for (siter = s.value.begin(); siter != s.value.end(); siter++) {
    Sequence tts;
    SequenceVal & tsv (tts.SHAREDGETVAL(SequenceVal));
    tsv.value.insert(tsv.value.end(), this->value.begin(), this->value.end());
    tsv.value.push_back(*siter);
    sv.value.push_back(tts);
  }
  return ts;
}

int64_t SequenceVal::Find(const Common & c) const
{
  if (!this->value.empty()) {
    int64_t index = 0;
    bool exists = false;
    SequenceVal::SequenceValueType::const_iterator i1;
    for (i1 = this->value.begin(); (i1 != this->value.end()) && !exists; i1++) {
      index++;
      exists = (c == *i1);
    }
    return (exists ? index : 0);
  }
  return 0;
}

bool SequenceVal::First(Common & g, SequenceValueType::const_iterator& traverseIter) const
{
  if ( !this->value.empty() ) {
    traverseIter = this->value.begin();
    g = (*traverseIter);
    return true;
  }
  else {
    return false;
  }
}

bool SequenceVal::Next(Common & g, SequenceValueType::const_iterator& traverseIter) const
{
  traverseIter++;
  if (traverseIter != this->value.end()) {
    g = (*traverseIter);
    return true;
  }
  else {
    return false;
  }
}

MetaivVal * SequenceVal::mk_sequenceval(int num, ...)
{
  va_list list;
  va_start (list, num);
  SequenceVal * sv = new SequenceVal();
  for (int i = 0; i < num; i++) {
    const Generic & g = *static_cast<const Generic *>(va_arg(list, const Generic *));
    sv->value.push_back(g);
  }
  va_end(list);
  return sv;
}

/**************************************************************************
*
*  Sequence
*
**************************************************************************/
static inline
SequenceVal* GetSequenceVal(const wstring & c)
{
  try {
    return ValCache->GetCachedStringVal (c);
  }
  catch (bad_alloc &e) {
    M4LibError::ReportError(ML_NULL_POINTER, wstring(L"Memory Allocation failed."));
  }
  return NULL;
}

static inline
SequenceVal* GetSequenceVal()
{
  try {
    return new SequenceVal();
  }
  catch (bad_alloc &e) {
    M4LibError::ReportError(ML_NULL_POINTER, wstring(L"Memory Allocation failed."));
  }
  return NULL;
}

Sequence::Sequence() : Common(GetSequenceVal()), IteratorP(NULL)
{
}

Sequence::Sequence(const wchar_t * st) : Common(GetSequenceVal(NULL == st ? wstring(L"") : wstring(st))), IteratorP(NULL)
{
}

Sequence::Sequence(const wstring & st) : Common(GetSequenceVal(st)), IteratorP(NULL)
{
}

Sequence::~Sequence()
{
  if (this->IteratorP) delete this->IteratorP;
}

Sequence::Sequence(const Generic & m) : Common(m), IteratorP(NULL)
{
  if (!this->IsSequence()) {
    M4LibError::ReportError(ML_CAST_ERROR, m.PrintType() + L" casted to Sequence");
  }
}

Sequence & Sequence::operator=(const wstring & s)
{
  this->Link(new SequenceVal(s));
  this->IteratorP = NULL;
  return *this;
}

Sequence & Sequence::operator=(const Generic & g)
{
  if (!g.IsSequence()) {
    M4LibError::ReportError(ML_CAST_ERROR, g.PrintType() + L" casted to Sequence");
  }
  this->IteratorP = NULL;
  this->Link(g);
  return *this;
}

void Sequence::Clear()
{
  this->Clone();
  static_cast<SequenceVal *>(mvp)->Clear();
  if (this->IteratorP) delete this->IteratorP;
  this->IteratorP = NULL;
}

Sequence & Sequence::ImpAppend(const Common & g)
{
  this->Clone();
  static_cast<SequenceVal *>(mvp)->ImpAppend(g);
  return *this;
}

const Generic & Sequence::operator[](int64_t j) const
{
  return (const Generic &)static_cast<const SequenceVal *>(mvp)->operator[](j);
}

const Generic & Sequence::Index(int64_t i) const
{
//  return this->operator[](i);
  return (const Generic &)static_cast<const SequenceVal *>(mvp)->operator[](i);
}

const Common & Sequence::CIndex(int64_t i) const
{
  return static_cast<const SequenceVal *>(mvp)->operator[](i);
}

const Common & Sequence::CLast() const
{
  return static_cast<const SequenceVal *>(mvp)->Last();
}

Common & Sequence::GetIndexRef(int64_t i)
{
  return static_cast<SequenceVal *>(mvp)->refIndex(i);
}

const Generic & Sequence::Hd() const
{
  return (const Generic &)static_cast<const SequenceVal *>(mvp)->Hd();
}

Sequence Sequence::Tl() const
{
  return static_cast<const SequenceVal *>(mvp)->Tl();
}

Sequence& Sequence::ImpTl()
{
  this->Clone();
  static_cast<SequenceVal *>(mvp)->ImpTl();
  return *this;
}

const Generic & Sequence::Last() const
{
  return (const Generic &)static_cast<const SequenceVal *>(mvp)->Last();
}

int64_t Sequence::Length() const
{
  return static_cast<const SequenceVal *>(mvp)->Length();
}

bool Sequence::IsEmpty() const
{
  return static_cast<const SequenceVal *>(mvp)->IsEmpty();
}

/**************************************************************************
*  Sequence::GetString converts a Sequence to a wstring.
*  if all elements in the sequence is of type Char, GetString will return
*  true and the result will be available in the wstring str.
*  If all elements in the Sequence are not of type Char, GetString will
*  return false and the wstring str will be empty.
**************************************************************************/

wstring Sequence::GetString() const
{
  return static_cast<const SequenceVal *>(mvp)->GetString();
}

bool Sequence::GetString(wstring & str) const
{
  return static_cast<const SequenceVal *>(mvp)->GetString(str);
}

bool Sequence::IsString() const
{
  return static_cast<const SequenceVal *>(mvp)->IsString();
}

Sequence& Sequence::ImpConc(const Sequence & s)
{
  this->Clone();
  static_cast<SequenceVal *>(mvp)->ImpConc(s.CGETVAL(SequenceVal));
  return *this;
}

Sequence Sequence::Conc(const Sequence & s) const
{
  return static_cast<const SequenceVal *>(mvp)->Conc(s.CGETVAL(SequenceVal));
}

Sequence & Sequence::ImpPrepend(const Common & g)
{
  this->Clone();
  static_cast<SequenceVal *>(mvp)->ImpPrepend(g);
  return *this;
}

Sequence & Sequence::RemElem(int64_t i)
{
  this->Clone();
  static_cast<SequenceVal *>(mvp)->RemElem(i);
  return *this;
}

Sequence & Sequence::ImpModify(int64_t j, const Common & g)
{
  this->Clone();
  static_cast<SequenceVal *>(mvp)->ImpModify(j, g);
  return *this;
}

Set Sequence::Elems() const
{
  return static_cast<const SequenceVal *>(mvp)->Elems();
}

Set Sequence::Inds() const
{
  return static_cast<const SequenceVal *>(mvp)->Inds();
}

Sequence Sequence::Reverse() const
{
  return static_cast<const SequenceVal *>(mvp)->Reverse();
}

Set Sequence::Permute() const
{
  return static_cast<const SequenceVal *>(mvp)->Permute();
}

Sequence Sequence::DistrSetProduct() const
{
  return static_cast<const SequenceVal *>(mvp)->DistrSetProduct();
}

bool Sequence::OutOfRange(const Real & r) const
{
  return (!r.IsNatOne() || static_cast<const SequenceVal *>(mvp)->OutOfRange(r.GetIntValue()));
}

int64_t Sequence::Find(const Common & c) const
{
  return static_cast<const SequenceVal *>(mvp)->Find(c);
}

Sequence Sequence::SubSequence(int64_t from, int64_t to) const
{
  return static_cast<const SequenceVal *>(mvp)->SubSequence(from, to);
}

bool Sequence::StartsWith(const Sequence & l) const
{
  return static_cast<const SequenceVal *>(mvp)->StartsWith(l.CGETVAL(SequenceVal));
}

bool Sequence::EndsWith(const Sequence & l) const
{
  return static_cast<const SequenceVal *>(mvp)->EndsWith(l.CGETVAL(SequenceVal));
}

Sequence Sequence::Product(const Set & s) const
{
  return static_cast<const SequenceVal *>(mvp)->Product(s.CGETVAL(SetVal));
}

bool Sequence::First(Generic & g) const
{
  //if (this->IteratorP == NULL) this->IteratorP = new SequenceValIterator();
  if (this->IteratorP == NULL) {
    ((Sequence &)*this).IteratorP = new SequenceValIterator();
  }
  return static_cast<const SequenceVal *>(mvp)->First(g, this->IteratorP->traverseIter);
}

bool Sequence::Next(Generic & g) const
{
  return static_cast<const SequenceVal *>(mvp)->Next(g, this->IteratorP->traverseIter);
}

size_t Sequence::MaxSize() const
{
  return static_cast<const SequenceVal *>(mvp)->MaxSize();
}

Sequence mk_sequence(const Generic & p1)
{
  return Sequence(Common(SequenceVal::mk_sequenceval(1,&p1)));
}

Sequence mk_sequence(const Generic & p1, const Generic & p2)
{
  return Sequence(Common(SequenceVal::mk_sequenceval(2,&p1,&p2)));
}

Sequence mk_sequence(const Generic & p1, const Generic & p2, const Generic & p3)
{
  return Sequence(Common(SequenceVal::mk_sequenceval(3,&p1,&p2,&p3)));
}

Sequence mk_sequence(const Generic & p1, const Generic & p2, const Generic & p3, const Generic & p4)
{
  return Sequence(Common(SequenceVal::mk_sequenceval(4,&p1,&p2,&p3,&p4)));
}

Sequence mk_sequence(const Generic & p1, const Generic & p2, const Generic & p3, const Generic & p4,
                     const Generic & p5)
{
  return Sequence(Common(SequenceVal::mk_sequenceval(5,&p1,&p2,&p3,&p4,&p5)));
}

Sequence mk_sequence(const Generic & p1, const Generic & p2, const Generic & p3, const Generic & p4,
                     const Generic & p5, const Generic & p6)
{
  return Sequence(Common(SequenceVal::mk_sequenceval(6,&p1,&p2,&p3,&p4,&p5,&p6)));
}

Sequence mk_sequence(const Generic & p1, const Generic & p2, const Generic & p3, const Generic & p4,
                     const Generic & p5, const Generic & p6, const Generic & p7)
{
  return Sequence(Common(SequenceVal::mk_sequenceval(7,&p1,&p2,&p3,&p4,&p5,&p6,&p7)));
}

Sequence mk_sequence(const Generic & p1, const Generic & p2, const Generic & p3, const Generic & p4,
                     const Generic & p5, const Generic & p6, const Generic & p7, const Generic & p8)
{
  return Sequence(Common(SequenceVal::mk_sequenceval(8,&p1,&p2,&p3,&p4,&p5,&p6,&p7,&p8)));
}

/**************************************************************************
*
*  Stack
*
**************************************************************************/
Stack & Stack::Push(const Common & c)
{
  this->Clone();
  static_cast<SequenceVal *>(mvp)->ImpPrepend(c);
  return *this;
}

Generic Stack::Pop()
{
  this->Clone();
  return static_cast<SequenceVal *>(mvp)->ImpPop();
}

Sequence Stack::Pop(int64_t n)
{
  if (n > 0) {
    this->Clone();
    return static_cast<SequenceVal *>(mvp)->ImpPop(n);
  }
  else {
    return Sequence();
  }
}

Stack & Stack::RemoveNth(int64_t n)
{
  this->Clone();
  static_cast<SequenceVal *>(mvp)->RemElem(n);
  return *this;
}

Stack & Stack::Copy()
{
  this->Clone();
  static_cast<SequenceVal *>(mvp)->ImpCopy();
  return *this;
}

Stack & Stack::Swap()
{
  this->Clone();
  static_cast<SequenceVal *>(mvp)->ImpSwap();
  return *this;
}

/**************************************************************************
*
*  SetVal
*
**************************************************************************/
Set SetVal::Power() const
{
  Set ts;
  SetVal & sv ((SetVal &)*ts.get_mvp());
  sv.Insert(Set()); 
  for (SetVal::SetValueType::const_iterator siter = this->value.begin(); siter != this->value.end(); siter++)
  {
    SetVal svp (sv);
    SetVal::SetValueType::const_iterator svpiter;
    for (svpiter = svp.value.begin(); svpiter != svp.value.end(); svpiter++) {
      Set s (*svpiter);
      s.Insert(*siter);
      sv.Insert(s);
    }
  }
  return ts;
}

SetVal & SetVal::ImpUnion(const SetVal & s)
{
  if ( !s.value.empty() ) {
    SetVal::SetValueType sval;
    insert_iterator<SetVal::SetValueType> ii (sval, sval.begin()) ;
    set_union(this->value.begin(), this->value.end(), s.value.begin(), s.value.end(), ii);
    this->value.clear();
    this->value = sval;
  }
  return *this;
}

Set SetVal::Union(const SetVal & s) const
{
  Set ts;
  SetVal & sv (ts.SHAREDGETVAL(SetVal));
  insert_iterator<SetVal::SetValueType> ii (sv.value, sv.value.begin()) ;
  set_union(this->value.begin(), this->value.end(), s.value.begin(), s.value.end(), ii);
  return ts;
}

SetVal & SetVal::ImpIntersect(const SetVal & s)
{
  if( !s.value.empty() ) {
    SetVal::SetValueType sval;
    insert_iterator<SetVal::SetValueType> ii (sval, sval.begin()) ;
    set_intersection(this->value.begin(), this->value.end(), s.value.begin(), s.value.end(), ii);
    this->value.clear();
    this->value = sval;
  }
  else {
    this->value.clear();
  }
  return *this;
}

Set SetVal::Intersect(const SetVal & s) const
{
  Set ts;
  SetVal & sv (ts.SHAREDGETVAL(SetVal));
  insert_iterator<SetVal::SetValueType> ii (sv.value, sv.value.begin()) ;
  set_intersection(this->value.begin(), this->value.end(), s.value.begin(), s.value.end(), ii);
  return ts;
}

SetVal & SetVal::ImpDiff(const SetVal& s)
{
  if( !s.value.empty() ) {
    SetVal::SetValueType sval;
    const SetVal::SetValueType & s_value = s.value;
    insert_iterator<SetVal::SetValueType> ii (sval, sval.begin()) ;
    set_difference(this->value.begin(), this->value.end(), s_value.begin(), s_value.end(), ii);
    this->value.clear();
    this->value = sval;
  }
  return *this;
}

Set SetVal::Diff(const SetVal & s) const
{
  Set ts;
  SetVal & sv (ts.SHAREDGETVAL(SetVal));
  insert_iterator<SetVal::SetValueType> ii (sv.value, sv.value.begin()) ;
  set_difference(this->value.begin(), this->value.end(), s.value.begin(), s.value.end(), ii);
  return ts;
}

const Common & SetVal::GetElem(bool reverse) const
{
  if (!this->value.empty()) {
    if (reverse) {
      return *(this->value.rbegin());   // Last element
    }
    else {
      return *(this->value.begin());    // First element
    }
  }
  else {
    M4LibError::ReportError(ML_OP_ON_EMPTY_SET,L"vdm_SetVal::GetElem: on empty Set");
    return Common::dummy;
  }
}

SetVal & SetVal::RemElem(const Common & g)
{
  const SetValueType::iterator siter = value.find(g);
  if (siter != this->value.end()) {
    // This is reentrant safe since we keep a ref to the value to be erased.
    SetValueType::value_type keepref (*siter);
    this->value.erase(siter);
  } else {
    M4LibError::ReportError(ML_NOT_IN_SET,L"vdm_SetVal::RemElem: on empty Set");
  }
  return *this;
}

bool SetVal::SubSet(const SetVal & s) const
{
  if( !this->value.empty() ) {
    return includes(s.value.begin(), s.value.end(), this->value.begin(), this->value.end());
  }
  else {
    return true;
  }
}

bool SetVal::First(Common & g, SetValueType::const_iterator & setIter) const
{
  if ( !this->value.empty() ) {
    setIter = this->value.begin();
    g = (*setIter);
    return true;
  } else {
    return false;
  }
}

bool SetVal::Next(Common & g, SetValueType::const_iterator & setIter) const
{
  setIter++;
  if (setIter != this->value.end()) {
    g = (*setIter);
    return true;
  } else {
    return false;
  }
}

int SetVal::Compare(const MetaivVal & mval) const
{
  const SetVal & s (static_cast<const SetVal &>(mval));
  if (this->value.size() < s.value.size()) {
    return -1;
  }
  else if (this->value.size() > s.value.size()) {
    return 1;
  }
  else {
    int res = 0;
    SetValueType::const_iterator miter1, miter2;
    for (miter1 = this->value.begin(), miter2 = s.value.begin();
         (miter1 != this->value.end()) && (res == 0);
         miter1++, miter2++) {
      res = (*miter1).Compare((*miter2));
    }
    return res;
  }
}

bool SetVal::IsSimple() const
{
  if (this->value.empty()) {
    return true;
  }
  bool forall = true;
  SetVal::SetValueType::const_iterator siter;
  for (siter = this->value.begin(); (siter != this->value.end()) && forall; siter++) {
    forall = siter->IsSimple();
  }
  return forall;
}

Sequence SetVal::ToSequence(bool reverse) const
{
  Sequence ts;
  SequenceVal & sv (ts.SHAREDGETVAL(SequenceVal));
  if (reverse) {
    sv.value.insert(sv.value.end(), this->value.rbegin(), this->value.rend());
  }
  else {
    sv.value.insert(sv.value.end(), this->value.begin(), this->value.end());
  }
  return ts;
}

Set SetVal::DirectProduct(const SetVal & s) const
{
  Set ts;
  SetVal & sv (ts.SHAREDGETVAL(SetVal));
  SetVal::SetValueType::const_iterator siter;
  for (siter = this->value.begin(); siter != this->value.end(); siter++) {
    SetVal::SetValueType::const_iterator siter2;
    for (siter2 = s.value.begin(); siter2 != s.value.end(); siter2++) {
      Set tts;
      SetVal & tsv (tts.SHAREDGETVAL(SetVal));
      tsv.value.insert(*siter);
      tsv.value.insert(*siter2);
      sv.value.insert(tts);
    }
  }
  return ts;
}

Set SetVal::DistrDirectProduct() const
{
  if (this->value.empty() || this->InSet(Set())) {
    return Set();
  }

  Set tmp_s;
  SetVal & tmpsv (tmp_s.SHAREDGETVAL(SetVal));
  tmpsv.value.insert(this->value.begin(), this->value.end());

  Set ts (mk_set(Set()));
  
  while (!tmpsv.value.empty()) {
    Set elem (*tmpsv.value.begin()); // GetElem
    const SetVal & elemv (elem.CGETVAL(SetVal));
    tmpsv.value.erase(tmpsv.value.begin()); // RemElem

    Set lts;
    SetVal & ltsv (lts.SHAREDGETVAL(SetVal));
    const SetVal & tsv (ts.SHAREDGETVAL(SetVal));
    for (SetVal::SetValueType::const_iterator s1 = tsv.value.begin(); s1 != tsv.value.end(); s1++) {
      for (SetVal::SetValueType::const_iterator s2 = elemv.value.begin(); s2 != elemv.value.end(); s2++) {
        Set s (*s1);
        ltsv.Insert(s.Insert(*s2));
      }
    }
    ts = lts;
  }
  return ts; 
}

Set SetVal::DUnion() const
{
  Set ts;
  SetVal & sv (ts.SHAREDGETVAL(SetVal));
  SetVal::SetValueType::const_iterator siter;
  for (siter = this->value.begin(); siter != this->value.end(); siter++) {
    if (!(*siter).IsSet()) {
      M4LibError::ReportError(ML_CAST_ERROR, (*siter).PrintType() + L" casted to Set");
    }
    SetValueType::value_type ref (*siter);
    sv.ImpUnion(ref.CGETVAL(SetVal));
  }
  return ts;
}

Set SetVal::DInter() const
{
  if (this->value.empty() || this->InSet(Set())) {
    return Set();
  }
  Set ts;
  SetVal & sv (ts.SHAREDGETVAL(SetVal));

  Set tmp_s;
  SetVal & tmpsv (tmp_s.SHAREDGETVAL(SetVal));
  tmpsv.value.insert(this->value.begin(), this->value.end());

  Set elem (*tmpsv.value.begin());      // GetElem
  const SetVal & elemv (elem.CGETVAL(SetVal));

  tmpsv.value.erase(tmpsv.value.begin());   // RemElem

  sv.value.insert(elemv.value.begin(), elemv.value.end());

  SetVal::SetValueType::const_iterator siter;
  for (siter = tmpsv.value.begin(); siter != tmpsv.value.end(); siter++) {
    if (!(*siter).IsSet()) {
      M4LibError::ReportError(ML_CAST_ERROR, (*siter).PrintType() + L" casted to Set");
    }
    SetValueType::value_type ref (*siter);
    sv.ImpIntersect(ref.CGETVAL(SetVal));
  }
  return ts;
}

/**************************************************************************
*  SetVal::ostream_out writes an ascii representation of the Set to
*  os in a L"pretty" print format.
**************************************************************************/
void SetVal::ostream_out(wostream & os, const VDMFormatter & vf) const
{
  VDMFormatter vf2 (vf);

  bool is_simple = true;
  SetValueType::const_iterator i1;
  for (i1 = this->value.begin(); (i1 != this->value.end()) && is_simple; i1++) {
    is_simple = (*i1).IsSimple();
  }

  bool multiline = false;
  if (vf.PrettyPrinting()) {
    vf2.IncrementIndent();
    if (!is_simple) {
      multiline = true;
    }
  }

  os << L"{ ";

  bool first = true;
  for (i1 = this->value.begin(); i1 != this->value.end(); i1++) {
    if (!first) {
      os << L",";
      if (multiline) {
        os << endl;
        vf2.print_spaces(os, vf2.GetIndent());
      }
      else {
        os << L" ";
      }
    }
    else {
      first = false;
    }
    (*i1).ostream_out(os, vf2);
  }

  os << L" }";
}

void SetVal::WriteVal (ostream& ostr) const
{
  ostr << 'c' << this->value.size() << ",";

  SetValueType::const_iterator i1;
  for (i1 = this->value.begin(); i1 != this->value.end(); i1++) {
    (*i1).WriteVal (ostr);
  }
}

MetaivVal * SetVal::mk_setval(int num, ...)
{
  va_list list;
  va_start (list, num);
  SetVal * sv = new SetVal();
  for (int i = 0; i < num; i++) {
    const Generic & g = *static_cast<const Generic *>(va_arg(list, const Generic *));
    if (sv->value.find(g) == sv->value.end()) {
      sv->value.insert(g);
    }
  }
  va_end(list);
  return sv;
}

/**************************************************************************
*
*  Set
*
**************************************************************************/
static inline
SetVal* GetSetVal()
{
  try {
    return new SetVal();
  }
  catch (bad_alloc &e) {
    M4LibError::ReportError(ML_NULL_POINTER, wstring(L"Memory Allocation failed."));
  }
  return NULL;
}

Set::Set() : Common(GetSetVal()), IteratorP(NULL)
{
}

Set::Set( const Generic & m) : Common(m), IteratorP(NULL)
{
  if (!this->IsSet()) {
    M4LibError::ReportError(ML_CAST_ERROR, m.PrintType() + L" casted to Set");
  }
}

Set::~Set()
{
  if (this->IteratorP != NULL) delete this->IteratorP;
}

Set & Set::operator=(const Generic & g)
{
  if (!g.IsSet()) {
    M4LibError::ReportError(ML_CAST_ERROR, g.PrintType() + L" casted to Set");
  }
  this->IteratorP = NULL;
  this->Link(g);
  return *this;
}

void Set::Clear()
{
  this->Clone();
  static_cast<SetVal *>(mvp)->Clear();
  if (this->IteratorP) {
    delete this->IteratorP;
  }
  this->IteratorP = NULL;
}

bool Set::First(Generic & g) const
{
  // If only we could use 'mutable' on the IteratorP member variable
  // but DEC cxx does not understand this keyword. So we have to cast
  // away const'ness.
  //if (this->IteratorP == NULL) this->IteratorP = new SetValIterator();
  if (this->IteratorP == NULL) {
    ((Set &)*this).IteratorP = new SetValIterator();
  }
  return static_cast<const SetVal *>(mvp)->First(g, this->IteratorP->traverseIter);
}

bool Set::Next(Generic & g) const
{
  return static_cast<const SetVal *>(mvp)->Next(g, this->IteratorP->traverseIter);
}

Set& Set::Insert(const Common & g)
{
  // FIXME (selfinsert).
  this->Clone();
  static_cast<SetVal *>(mvp)->Insert(Common(g));
  return *this;
}

int64_t Set::Card() const
{
  return static_cast<const SetVal *>(mvp)->Card();
}

Set Set::Power() const
{
  return static_cast<const SetVal *>(mvp)->Power();
}

bool Set::IsEmpty() const
{
  return static_cast<const SetVal *>(mvp)->IsEmpty();
}

bool Set::InSet(const Common & g) const
{
  return static_cast<const SetVal *>(mvp)->InSet(g);
}

Set & Set::ImpUnion(const Set & s)
{
  Set s_copy(s);
  this->Clone();
  static_cast<SetVal *>(mvp)->ImpUnion(s_copy.CGETVAL(SetVal));
  return *this;
}

Set & Set::ImpIntersect(const Set & s)
{
  Set s_copy(s);
  this->Clone();
  static_cast<SetVal *>(mvp)->ImpIntersect(s_copy.CGETVAL(SetVal));
  return *this;
}

Set & Set::ImpDiff(const Set & s)
{
  Set s_copy(s);
  this->Clone();
  static_cast<SetVal *>(mvp)->ImpDiff(s_copy.CGETVAL(SetVal));
  return *this;
}

Set Set::Union(const Set & s) const
{
  return static_cast<SetVal *>(mvp)->Union(s.CGETVAL(SetVal));
}

Set Set::Intersect(const Set & s) const
{
  return static_cast<SetVal *>(mvp)->Intersect(s.CGETVAL(SetVal));
}

Set Set::Diff(const Set & s) const
{
  return static_cast<SetVal *>(mvp)->Diff(s.CGETVAL(SetVal));
}

const Generic & Set::GetElem() const
{
  return (const Generic &)static_cast<const SetVal *>(mvp)->GetElem(true);
}

const Generic & Set::GetElem(bool reverse) const
{
  return (const Generic &)static_cast<const SetVal *>(mvp)->GetElem(reverse);
}

Set & Set::RemElem(const Common& g)
{
  this->Clone();
  static_cast<SetVal *>(mvp)->RemElem(g);
  return *this;
}

bool Set::SubSet(const Set & s) const
{
  return static_cast<const SetVal *>(mvp)->SubSet(s.CGETVAL(SetVal));
}

Sequence Set::ToSequence() const
{
  return static_cast<const SetVal *>(mvp)->ToSequence(false);
}

Sequence Set::ToSequence(bool reverse) const
{
  return static_cast<const SetVal *>(mvp)->ToSequence(reverse);
}

Set Set::DirectProduct(const Set & s) const
{
  return static_cast<const SetVal *>(mvp)->DirectProduct(s.CGETVAL(SetVal));
}

Set Set::DistrDirectProduct() const
{
  return static_cast<const SetVal *>(mvp)->DistrDirectProduct();
}

Set Set::DUnion() const
{
  return static_cast<const SetVal *>(mvp)->DUnion();
}

Set Set::DInter() const
{
  return static_cast<const SetVal *>(mvp)->DInter();
}

Set mk_set(const Generic & p1)
{
  return Set(Common(SetVal::mk_setval(1,&p1)));
}

Set mk_set(const Generic & p1, const Generic & p2)
{
  return Set(Common(SetVal::mk_setval(2,&p1,&p2)));
}

Set mk_set(const Generic & p1, const Generic & p2, const Generic & p3)
{
  return Set(Common(SetVal::mk_setval(3,&p1,&p2,&p3)));
}

Set mk_set(const Generic & p1, const Generic & p2, const Generic & p3, const Generic & p4)
{
  return Set(Common(SetVal::mk_setval(4,&p1,&p2,&p3,&p4)));
}

Set mk_set(const Generic & p1, const Generic & p2, const Generic & p3, const Generic & p4, 
           const Generic & p5)
{
  return Set(Common(SetVal::mk_setval(5,&p1,&p2,&p3,&p4,&p5)));
}

Set mk_set(const Generic & p1, const Generic & p2, const Generic & p3, const Generic & p4,
           const Generic & p5, const Generic & p6)
{
  return Set(Common(SetVal::mk_setval(6,&p1,&p2,&p3,&p4,&p5,&p6)));
}

Set mk_set(const Generic & p1, const Generic & p2, const Generic & p3, const Generic & p4,
           const Generic & p5, const Generic & p6, const Generic & p7)
{
  return Set(Common(SetVal::mk_setval(7,&p1,&p2,&p3,&p4,&p5,&p6,&p7)));
}

Set mk_set(const Generic & p1, const Generic & p2, const Generic & p3, const Generic & p4,
           const Generic & p5, const Generic & p6, const Generic & p7, const Generic & p8)
{
  return Set(Common(SetVal::mk_setval(8,&p1,&p2,&p3,&p4,&p5,&p6,&p7,&p8)));
}

/**************************************************************************
*
*  RecordVal
*
**************************************************************************/
int RecordVal::Compare(const MetaivVal & mval) const
{
  const RecordVal & r (static_cast<const RecordVal &>(mval));
  //if (this->GetTag() == r.GetTag())
  if (this->recinfop->GetTag() == r.recinfop->GetTag()) {
    //int len = this->GetSize();
    int len = this->recinfop->GetSize();
    //if (len == r.GetSize())
    if (len == r.recinfop->GetSize()) {
      int res = 0;
      for (int i = 0; (i < len) && (res == 0); i++) {
        if (this->recinfop->AskDontCare(i))
          continue;           // Skip don't care field.
        res = this->value[i].Compare(r.value[i]);
      }
      return res;
    }
    else {
      //return ((len < r.GetSize()) ? -1 : 1);
      return ((len < r.recinfop->GetSize()) ? -1 : 1);
    }
  } else {
    //return ((this->GetTag() < r.GetTag() ? -1 : 1));
    return ((this->recinfop->GetTag() < r.recinfop->GetTag() ? -1 : 1));
  }
}

bool RecordVal::IsSimple() const
{
  return this->value.empty();
}

size_t Set::MaxSize() const
{
  return static_cast<const SetVal *>(mvp)->MaxSize();
}

/**************************************************************************
*  RecordVal::ostream_out writes an ascii representation of the Record to
*  os in a L"pretty" print format.
**************************************************************************/
void RecordVal::ostream_out(wostream & os, const VDMFormatter & vf) const
{
  VDMFormatter vf2 (vf);

  if (vf.PrettyPrinting()) {
    vf2.IncrementIndent(3);
  }
  // Why did we indent with +3 in this case?

  bool is_simple = true;
  for (int i = 1; (i <= (int) GetSize()) && is_simple; i++) {
    is_simple = value[i-1].IsSimple();
  }

  if (this->recinfop->HasPrintFunction()) {
    // Make a temporay Record object which contain `this'.
    Record r = Record(Common(new RecordVal(*this)));
    this->recinfop->Print(os, r, vf, is_simple);
    return;
  }

  const wstring symname (this->recinfop->GetSymTag());
  if ( !symname.empty() ) {
    if ( (symname == L"AS`Name") ||
         (symname == L"AS`OldName") ||
         (symname == L"AS`TypeName") ||
         (symname == L"AS`TypeVar") ||
         (symname == L"AS`NewExpr") ||
         (symname == L"AS`BoolLit") ||
         (symname == L"AS`NilLit") ||
         (symname == L"AS`NumLit") ||
         (symname == L"AS`RealLit") ||
         (symname == L"AS`CharLit") ||
         (symname == L"AS`TextLit") ||
         (symname == L"AS`QuoteLit") ||
         (symname == L"AS`QuoteType") ||
         ((symname == L"AS`PatternName") && (this->value[1].IsNil())) ||
         (symname == L"REP`QuoteTypeRep") ||
         (symname == L"REP`TypeNameRep") ||
         (symname == L"REP`ObjRefTypeRep") ||
         ((symname == L"STKM`PatternName") && (this->value[1].IsNil())) ||
         (symname == L"ProjectTypes`FileName") ||
         (symname == L"INSTRTP`NEWOBJ") ||
         (symname == L"INSTRTP`INITCLASS") ||
         (symname == L"INSTRTP`FIELDSEL") ||
         (symname == L"INSTRTP`LOOKUP") ||
         (symname == L"INSTRTP`PUSH") ||
         (symname == L"INSTRTP`ERRINST") ||
         (symname == L"SEM`OBJ_uRef") ||
         (symname == L"SEM`QUOTE_V") ) {
      is_simple = true;
    }
    os << L"mk_" << symname << L"(";
  }
  else {
    os << L"mk_unknown" << this->GetTag() << L"(";
  }

  bool multiline = false;
  if (vf2.PrettyPrinting()) {
    if (!is_simple) {
      multiline = true;
    }
  }

  bool first = true;
  for (int j = 1; j <= (int) GetSize(); j++) {
    if (!first) {
      os << L",";
    }
    else {
      first = false;
    }

    if (multiline) {
      os << endl;
      vf2.print_spaces(os, vf2.GetIndent());
    }
    else {
      os << L" ";
    }
    value[j-1].ostream_out(os, vf2);
  }
  if (GetSize() > 0) {
    os << L" ";
  }
  os << L")";
}

void RecordVal::WriteVal(ostream & ostr) const
{
  ostr << 'd' << this->GetTag() << "," << this->GetSize() << ",";

  for (int i = 1; i<=(int) this->GetSize(); i++) {
    this->value[i-1].WriteVal(ostr);
  }
}

void RecordVal::SetField(int i, const Common & gdom)
{
  if (OutOfRange(i)) {
    wcerr << L"tag=" << GetTextTag() << L" size=" << GetSize() << L" field=" << i << endl;
    M4LibError::ReportError(ML_INDEX_OUT_OF_RANGE, L"index out of range in 'SetField'");
  }
  else {
    // This is reentrant safe since we keep a ref to the value
    // to be overwritten.
    Generic keepref(this->value[i-1]);
    this->value[i-1] = gdom;
  }
}

const Common & RecordVal::GetField(int i) const
{
  if (OutOfRange(i)) {
    wcerr << L"tag=" << GetTextTag() << L" size=" << GetSize() << L" field=" << i << endl;
    //ostream_out(wcerr, VDM_default_formatter);
    M4LibError::ReportError(ML_INDEX_OUT_OF_RANGE, L"index out of range in 'GetField'");
    return Common::dummy;
  }
  return this->value[i-1];
}

Common & RecordVal::refField(int i)
{
  if (OutOfRange(i)) {
    wcerr << L"tag=" << GetTextTag() << L" size=" << GetSize() << L" field=" << i << endl;
    M4LibError::ReportError(ML_INDEX_OUT_OF_RANGE,L"index out of range in 'refField'");
  }
  return this->value[i-1];
}

Sequence RecordVal::GetFields() const
{
  Sequence ts;
  SequenceVal & sv (ts.SHAREDGETVAL(SequenceVal));
  sv.value.insert(sv.value.end(), this->value.begin(), this->value.end());
  return ts;
}

void RecordVal::SetFields(const Sequence & s)
{
  if ( (int)s.Length() != this->GetSize() ) {
    wcerr << L"tag=" << GetTextTag() << L" size=" << GetSize()
          << L" new size=" << s.Length()
          << endl;
    M4LibError::ReportError(ML_INDEX_OUT_OF_RANGE,L"index out of range in 'SetFields'");
  }
  else {
    const SequenceVal & sv (s.CGETVAL(SequenceVal));
    this->value.clear();
    this->value.insert(this->value.end(), sv.value.begin(), sv.value.end());
  }
}

/**************************************************************************
*
*  Record
*
**************************************************************************/
static inline
RecordVal * GetRecordVal(const wstring & symtag, const VDMRecInfoMap & tagspace)
{
  try {
    return new RecordVal(symtag, tagspace);
  }
  catch (bad_alloc &e) {
    M4LibError::ReportError(ML_NULL_POINTER, wstring(L"Memory Allocation failed."));
  }
  return NULL;
}

static inline
RecordVal * GetRecordVal(int tag, int size, VDMRecInfoMap & tagspace)
{
  try {
    return new RecordVal(tag, size, tagspace);
  }
  catch (bad_alloc &e) {
    M4LibError::ReportError(ML_NULL_POINTER, wstring(L"Memory Allocation failed."));
  }
  return NULL;
}

Record::Record() : Common(ValCache->recordv)
{
}

Record::Record(const Generic & m) : Common(m)
{
  if (!this->IsRecord()) {
    M4LibError::ReportError(ML_CAST_ERROR, m.PrintType() + L" casted to Record");
  }
}

Record::Record(const Common & m) : Common(m)
{
  if (!this->IsRecord()) {
    M4LibError::ReportError(ML_CAST_ERROR, m.PrintType() + L" casted to Record");
  }
}

Record::Record(int tag, int size, VDMRecInfoMap& tagspace) : Common(GetRecordVal(tag, size, tagspace))
{
}

Record::Record(const wstring & symtag, const VDMRecInfoMap & tagspace) : Common(GetRecordVal(symtag, tagspace))
{
}

Record::Record(int tag, int size) : Common(GetRecordVal(tag, size, VDMGetDefaultRecInfoMap()))
{
}

Record & Record::operator=(const Generic & g)
{
  if (!g.IsRecord()) {
    M4LibError::ReportError(ML_CAST_ERROR, g.PrintType() + L" casted to Record");
  }
  this->Link(g);
  return *this;
}

Record & Record::SetField(const int i, const Common & c)
{
  this->Clone();
  static_cast<RecordVal *>(mvp)->SetField(i, c);
  return *this;
}

const Generic & Record::GetField(const int i) const
{
  return static_cast<const Generic &>(static_cast<const RecordVal *>(mvp)->GetField(i));
}

const Int & Record::GetInt(int i) const
{
  return static_cast<const Int &>(static_cast<const RecordVal *>(mvp)->GetField(i));
}

const Real & Record::GetReal(int i) const
{
  return static_cast<const Real &>(static_cast<const RecordVal *>(mvp)->GetField(i));
}

const Map & Record::GetMap(int i) const
{
  return static_cast<const Map &>(static_cast<const RecordVal *>(mvp)->GetField(i));
}

const Set & Record::GetSet(int i) const
{
  return static_cast<const Set &>(static_cast<const RecordVal *>(mvp)->GetField(i));
}

const Sequence & Record::GetSequence(int i) const
{
  return static_cast<const Sequence &>(static_cast<const RecordVal *>(mvp)->GetField(i));
}

const Tuple & Record::GetTuple(int i) const
{
  return static_cast<const Tuple &>(static_cast<const RecordVal *>(mvp)->GetField(i));
}

const Bool & Record::GetBool(int i) const
{
  return static_cast<const Bool &>(static_cast<const RecordVal *>(mvp)->GetField(i));
}

const Record & Record::GetRecord(int i) const
{
  return static_cast<const Record &>(static_cast<const RecordVal *>(mvp)->GetField(i));
}

const Char & Record::GetChar(int i) const
{
  return static_cast<const Char &>(static_cast<const RecordVal *>(mvp)->GetField(i));
}

int64_t Record::GetIntValue(int i) const
{
  return (static_cast<const Int &>(static_cast<const RecordVal *>(mvp)->GetField(i))).GetValue();
}

double Record::GetRealValue(int i) const
{
  return (static_cast<const Real &>(static_cast<const RecordVal *>(mvp)->GetField(i))).GetValue();
}

bool Record::GetBoolValue(int i) const
{
  return (static_cast<const Bool &>(static_cast<const RecordVal *>(mvp)->GetField(i))).GetValue();
}

int Record::GetTag() const
{
  return static_cast<const RecordVal *>(mvp)->GetTag();
}

wstring Record::GetTextTag() const
{
  return static_cast<const RecordVal *>(mvp)->GetTextTag();
}

int64_t Record::Length() const
{
  return static_cast<const RecordVal *>(mvp)->GetSize();
}

Sequence Record::GetFields() const
{
  return static_cast<const RecordVal *>(mvp)->GetFields();
}

Record & Record::SetFields(const Sequence & s)
{
  this->Clone();
  static_cast<RecordVal *>(mvp)->SetFields(s);
  return *this;
}

bool Record::IsDontCare(int i) const
{
  return static_cast<const RecordVal *>(mvp)->IsDontCare(i);
}

Common & Record::GetFieldRef(int i)
{
  return static_cast<RecordVal *>(mvp)->refField(i);
}

/**************************************************************************
*
*  TupleVal
*
**************************************************************************/
int TupleVal::Compare(const MetaivVal & mval) const
{
  const TupleVal & t (static_cast<const TupleVal &>(mval));

  if (value.size() < t.value.size()) {
    return -1;
  }
  else if (value.size() > t.value.size()) {
    return 1;
  }
  else {
#ifdef VDM_FINGERPRINT
    Fingerprint fp1, fp2;
    readfingerprint(fp1);
    t.readfingerprint(fp2);
    int fp_cmp = fp1.Compare(fp2);
    if (fp_cmp != 0) {
      return fp_cmp;
    }
#endif // VDM_FINGERPRINT

    int res = 0;
    //TupleValueType::const_iterator i1, i2;
    for (TupleValueType::const_iterator i1 = this->value.begin(), i2 = t.value.begin();
         (i1 != this->value.end()) && (res == 0); i1++, i2++) {
      res = (*i1).Compare(*i2);
    }
    return res;
  }
}

bool TupleVal::IsSimple() const
{
  //return this->value.empty();
  bool is_simple = true;
  for (TupleValueType::const_iterator itr = this->value.begin(); (itr != this->value.end()) && is_simple; itr++) {
    is_simple = itr->IsSimple();
  }
  return is_simple;
}

TupleVal & TupleVal::SetField(int i, const Common & gdom)
{
  if (OutOfRange(i)) {
    M4LibError::ReportError(ML_INDEX_OUT_OF_RANGE,L"index out of range in 'SetField'");
  }
  this->value[i-1] = gdom;
  return *this;
}

const Common & TupleVal::GetField(int i) const
{
  if (OutOfRange(i)) {
    M4LibError::ReportError(ML_INDEX_OUT_OF_RANGE,L"index out of range in 'GetField'");
    return Common::dummy;
  }
  return this->value[i-1];
}

Sequence TupleVal::GetFields() const
{
  Sequence ts;
  SequenceVal & sv (ts.SHAREDGETVAL(SequenceVal));
  sv.value.insert(sv.value.end(), this->value.begin(), this->value.end());
  return ts;
}

Common & TupleVal::refField(int i)
{
  if (OutOfRange(i)) {
    M4LibError::ReportError(ML_INDEX_OUT_OF_RANGE,L"index out of range in 'refField'");
  }
  return this->value[i-1];
}

void TupleVal::ostream_out(wostream & os, const VDMFormatter & vf) const
{
  VDMFormatter vf2 = vf;

  bool is_simple = true;
  //for (int i = 1; (i <= (int) this->value.size()) && is_simple; i++) {
  //  is_simple = this->value[i-1].IsSimple();
  //}
  for (TupleValueType::const_iterator itr = this->value.begin(); (itr != this->value.end()) && is_simple; itr++) {
    is_simple = itr->IsSimple();
  }

  if( !is_simple ) {
    if( ( this->value.size() == 2 ) && this->value[1].IsInt() ) {
      if( this->value[0].IsRecord() ) {
        Record r (this->value[0]);
        wstring tag (r.GetTextTag());
        if ((tag == L"SEM`NUM") ||
            (tag == L"SEM`BOOL") ||
            (tag == L"SEM`TOKEN") ||
            (tag == L"SEM`QUOTE") ||
            (tag == L"SEM`OBJ_Ref")) {
            is_simple = true;
        }
        else if (tag == L"SEM`SEQ") {
          //Sequence s (r.GetField( 1 ));
          //if (s.IsEmpty()) is_simple = true;
          is_simple = r.GetSequence(1).IsEmpty();
        }
        else if (tag == L"SEM`SET") {
          //Set s (r.GetField( 1 ));
          //if (s.IsEmpty()) is_simple = true;
          is_simple = r.GetSet(1).IsEmpty();
        }
        else if (tag == L"SEM`MAP") {
          //Map m (r.GetField( 1 ));
          //if (m.IsEmpty()) is_simple = true;
          is_simple = r.GetMap(1).IsEmpty();
        }
        else if (tag == L"SEM`TUPLE") {
          //Sequence t (r.GetField( 1 ));
          //if (t.IsEmpty()) is_simple = true;
          is_simple = r.GetSequence(1).IsEmpty();
        }
//
      }
    }
  }

  bool multiline = false;
  if (vf2.PrettyPrinting()) {
    vf2.IncrementIndent(5);
    if (!is_simple) {
      multiline = true;
    }
  }

  os << L"mk_( ";
  bool first = true;
  for (int j = 1; j <= (int) value.size(); j++) {
    if (!first) {
      os << L",";
      if (multiline) {
        os << endl;
        vf2.print_spaces(os, vf2.GetIndent());
      }
      else {
       os << L" ";
      }
    }
    else {
      first = false;
    }
    value[j-1].ostream_out(os, vf2);
  }
  os << L" )";
}

void TupleVal::WriteVal (ostream& ostr) const
{
  ostr << 'e' << (int) this->value.size() << ",";

  for (int i = 1; i <= (int) this->value.size(); i++) {
    this->value[i-1].WriteVal(ostr);
  }
}

MetaivVal * TupleVal::mk_tupleval(int num, ...)
{
  va_list list;
  va_start (list, num);
  TupleVal * tv = new TupleVal(num);
  for (int i = 0; i < num; i++) {
    tv->value[i] = *static_cast<const Generic *>(va_arg(list, const Generic *));
  }
  va_end(list);
  return tv;
}

/**************************************************************************
*
*  Tuple
*
**************************************************************************/
static inline
TupleVal* GetTupleVal(int size)
{
  try {
    return ((size == 0) ? ValCache->tuplev : new TupleVal(size));
  }
  catch (bad_alloc &e) {
    M4LibError::ReportError(ML_NULL_POINTER, wstring(L"Memory Allocation failed."));
  }
  return NULL;
}

Tuple::Tuple() : Common(ValCache->tuplev)
{
}

Tuple::Tuple(const Generic & m) : Common(m)
{
  if (!this->IsTuple()) {
    M4LibError::ReportError(ML_CAST_ERROR, m.PrintType() + L" casted to Tuple");
  }
}

Tuple::Tuple(int size) : Common(GetTupleVal(size))
{
}

Tuple mk_(const Generic & p1, const Generic & p2)
{
  return Tuple(Common(TupleVal::mk_tupleval(2,&p1,&p2)));  
}

Tuple mk_(const Generic & p1, const Generic & p2, const Generic & p3)
{
  return Tuple(Common(TupleVal::mk_tupleval(3,&p1,&p2,&p3)));  
}

Tuple mk_(const Generic & p1, const Generic & p2, const Generic & p3, const Generic & p4)
{
  return Tuple(Common(TupleVal::mk_tupleval(4,&p1,&p2,&p3,&p4)));  
}

Tuple mk_(const Generic & p1, const Generic & p2, const Generic & p3, const Generic & p4,
          const Generic & p5)
{
  return Tuple(Common(TupleVal::mk_tupleval(5,&p1,&p2,&p3,&p4,&p5)));  
}

Tuple mk_(const Generic & p1, const Generic & p2, const Generic & p3, const Generic & p4,
          const Generic & p5, const Generic & p6)
{
  return Tuple(Common(TupleVal::mk_tupleval(6,&p1,&p2,&p3,&p4,&p5,&p6)));  
}

Tuple mk_(const Generic & p1, const Generic & p2, const Generic & p3, const Generic & p4,
          const Generic & p5, const Generic & p6, const Generic & p7)
{
  return Tuple(Common(TupleVal::mk_tupleval(7,&p1,&p2,&p3,&p4,&p5,&p6,&p7)));  
}

Tuple mk_(const Generic & p1, const Generic & p2, const Generic & p3, const Generic & p4,
          const Generic & p5, const Generic & p6, const Generic & p7, const Generic & p8)
{
  return Tuple(Common(TupleVal::mk_tupleval(8,&p1,&p2,&p3,&p4,&p5,&p6,&p7,&p8)));  
}

Tuple & Tuple::operator=(const Tuple & t)
{
  this->Link(t.mvp);
  return *this;
}

Tuple & Tuple::operator=(const Generic & g)
{
  if (!g.IsTuple()) {
    M4LibError::ReportError(ML_CAST_ERROR, g.PrintType() + L" casted to Tuple");
  }
  this->Link(g);
  return *this;
}

int64_t Tuple::Length() const
{
  return static_cast<const TupleVal *>(mvp)->Length();
}

Sequence Tuple::GetFields() const
{
  return static_cast<const TupleVal *>(mvp)->GetFields();
}

Tuple& Tuple::SetField(int i, const Common & gdom)
{
  this->Clone();
  static_cast<TupleVal *>(mvp)->SetField(i, gdom);
  return *this;
}

const Generic & Tuple::GetField(int i) const
{
  return static_cast<const Generic &>(static_cast<const TupleVal *>(mvp)->GetField(i));
}

const Int & Tuple::GetInt(int i) const
{
  return static_cast<const Int &>(static_cast<const TupleVal *>(mvp)->GetField(i));
}

const Real & Tuple::GetReal(int i) const
{
  return static_cast<const Real &>(static_cast<const TupleVal *>(mvp)->GetField(i));
}

const Map & Tuple::GetMap(int i) const
{
  return static_cast<const Map &>(static_cast<const TupleVal *>(mvp)->GetField(i));
}

const Set & Tuple::GetSet(int i) const
{
  return static_cast<const Set &>(static_cast<const TupleVal *>(mvp)->GetField(i));
}

const Sequence & Tuple::GetSequence(int i) const
{
  return static_cast<const Sequence &>(static_cast<const TupleVal *>(mvp)->GetField(i));
}

const Tuple & Tuple::GetTuple(int i) const
{
  return static_cast<const Tuple &>(static_cast<const TupleVal *>(mvp)->GetField(i));
}

const Bool & Tuple::GetBool(int i) const
{
  return static_cast<const Bool &>(static_cast<const TupleVal *>(mvp)->GetField(i));
}

const Record & Tuple::GetRecord(int i) const
{
  return static_cast<const Record &>(static_cast<const TupleVal *>(mvp)->GetField(i));
}

int64_t Tuple::GetIntValue(int i) const
{
  return (static_cast<const Int &>(static_cast<const TupleVal *>(mvp)->GetField(i))).GetValue();
}

double Tuple::GetRealValue(int i) const
{
  return (static_cast<const Real &>(static_cast<const TupleVal *>(mvp)->GetField(i))).GetValue();
}

bool Tuple::GetBoolValue(int i) const
{
  return (static_cast<const Bool &>(static_cast<const TupleVal *>(mvp)->GetField(i))).GetValue();
}

Common & Tuple::GetFieldRef(int i)
{
  return static_cast<TupleVal *>(mvp)->refField(i);
}

/**************************************************************************
*
*  ObjectRefVal
*
**************************************************************************/
ObjectRefVal::ObjectRefVal() : MetaivVal(mt_objectref)
{
#if defined(SHOW_MALLOC_STATS)
  showmalloccount->log_insert(typeid(*this).name(), this, sizeof(*this));
#endif
}

void
ObjectRefVal::updatefingerprint() const
{
#ifdef VDM_FINGERPRINT
  Fingerprint fp_res;
  fp_res.init_crc();
  setfingerprint(fp_res);
#endif // VDM_FINGERPRINT
}

int ObjectRefVal::Compare(const MetaivVal & mval) const
{
  const ObjectRefVal& t (static_cast<const ObjectRefVal &>(mval));
  return ((this == &t) ? 0 : ((this < &t) ? -1 : 1));
}

ObjectRefVal::~ObjectRefVal() {
#if defined(SHOW_MALLOC_STATS)
  showmalloccount->log_remove(typeid(*this).name(), this);
#endif
}

/**************************************************************************
*  ObjectRefVal::ostream_out writes an ascii representation of the ObjectRef to
*  os in a L"pretty" print format.
**************************************************************************/
void ObjectRefVal::ostream_out(wostream & os, const VDMFormatter & /*vf*/) const
{
  os << L"@(" << ctype() << L", " << this << L")";
}

void ObjectRefVal::WriteVal (ostream& ostr) const
{
  cerr << "ERROR : WriteVal not defined for ObjectRefVal\n";
  ostr << 'f';
  ostr << 0 << L",";
}

/*************************************************************************
 * vdmBase
 ************************************************************************/

Bool vdmBase::SameBaseClass(const ObjectRef & cls) const
{
  Set s (cls.GetRef()->GetBaseClasses());
  s.ImpIntersect(GetBaseClasses());
  return Bool(!s.IsEmpty());
}

Bool vdmBase::IsOfClass(int cls) const
{
  return Bool(class_s.InSet (Int (cls)));
}

Bool vdmBase::IsOfBaseClass(int cls) const
{
  return Bool(base_s.InSet(Int(cls)));
}

void
vdmBase::RegisterAsBase(int cls)
{
  clstype = cls;
  base_s.Insert(Int(cls));
  class_s.Insert(Int(cls));
}

void
vdmBase::RegisterAsDerived(int cls)
{
  clstype = cls;
  class_s.Insert(Int(cls));
}

vdmBase::~vdmBase()
{
  // Do nothing.
}
/**************************************************************************
*
*  ObjectRef
*
**************************************************************************/
static vdmBase* vdmBaseP;
static
vdmBase* dummy_vdmBase()
{
  if (!vdmBaseP) {
    vdmBaseP = new vdmBase;
  }
  vdmBaseP->AddRef();
  return vdmBaseP;
}

ObjectRef::ObjectRef(vdmBase* mp) : Common(mp ? mp : dummy_vdmBase())
{
}

ObjectRef::ObjectRef(const ObjectRef&m) : Common((const Common&)m)
{
}

ObjectRef::ObjectRef(const Generic & m) : Common((const Common&)m)
{
  if (!this->IsObjectRef()) {
    M4LibError::ReportError(ML_CAST_ERROR, m.PrintType() + L" casted to ObjectRef");
  }
}

ObjectRef::~ObjectRef()
{
}

ObjectRef & ObjectRef::operator=(const ObjectRef & t)
{
  this->Link(t.mvp);
  return *this;
}

ObjectRef & ObjectRef::operator=(const Generic & g)
{
  if (!g.IsObjectRef()) {
    M4LibError::ReportError(ML_CAST_ERROR, g.PrintType() + L" casted to ObjectRef");
  }
  this->Link(g);
  return *this;
}

int ObjectRef::MyObjectId () const
{
  return this->GetRef()->ctype();
}

vdmBase* ObjectRef::GetRef () const
{
  if (((ObjectRefVal *)mvp)->has_cg_base()) {
    return (vdmBase*)mvp;
  } else {
    M4LibError::ReportError(ML_NULL_REF, L"object reference in ObjectRef not initialised");
    return (vdmBase*)NULL;
  }
}

bool ObjectRef::IsInitialized() const
{
  return (((ObjectRefVal *)mvp)->has_cg_base());
}

Bool ObjectRef::SameBaseClass(const ObjectRef& o) const
{
  return this->GetRef()->SameBaseClass(o);
}

Bool ObjectRef::IsOfClass(int c) const
{
  return this->GetRef()->IsOfClass(c);
}

Bool ObjectRef::IsOfBaseClass(int c) const
{
  return this->GetRef()->IsOfBaseClass(c);
}

/**************************************************************************
*
*  IntVal
*
**************************************************************************/
int IntVal::Compare(const MetaivVal & mval) const
{
  switch(mval.GetType()) {
    case mt_int: {
      IntValueType i ((static_cast<const IntVal &>(mval).GetValue()));
      return ((this->value == i) ? 0 : ((this->value > i) ? 1 : -1 ));
    }
    case mt_real: {
      RealVal::RealValueType r ((static_cast<const RealVal &>(mval).GetValue()));
      return ((this->value == r) ? 0 : ((this->value > r) ? 1 : -1 ));
    }
    default: {
      return this->GetType() < mval.GetType() ? -1 : 1; // Should never be executed.
    }
  }
}

/**************************************************************************
*
*  Int
*
**************************************************************************/
static inline
IntVal* GetIntVal(int64_t i)
{
  try {
    return (((i >= 0) && (i < 256)) ? ValCache->intv[i] : new IntVal(static_cast<IntVal::IntValueType>(i)));
  }
  catch (bad_alloc &e) {
    M4LibError::ReportError(ML_NULL_POINTER, wstring(L"Memory Allocation failed."));
  }
  return NULL;
}

Int::Int() : Common(ValCache->intv[0])
{
}

Int::Int(const int64_t i) : Common(GetIntVal(i))
{
}

Int::Int(const Real & r) : Common(GetIntVal(static_cast<int64_t>(r.GetValue())))
{
}

Int::Int(const Generic & g) : Common(g)
{
  if (g.IsInt()) {
  }
  else if (g.IsReal()) {
    this->Link(GetIntVal(static_cast<int64_t>(Real(g).GetValue())));
  }
  else {
    M4LibError::ReportError(ML_CAST_ERROR, g.PrintType() + L" casted to Int");
  }
}

Int::operator int() const
{
  return (int)(static_cast<const IntVal *>(mvp)->GetValue());
}

Int Int::operator-() const
{
  return Int(static_cast<const IntVal *>(mvp)->Minus());
}

Int Int::operator+(const Int & i) const
{
  return Int(static_cast<const IntVal *>(mvp)->Add(i.GetValue()));
}

Real Int::operator+(const Real & r) const
{
  return Real(static_cast<const RealVal>(GetValue()).Add(r.GetValue()));
}

Int Int::operator-(const Int & i) const
{
  return Int(static_cast<const IntVal *>(mvp)->Sub(i.GetValue()));
}

Real Int::operator-(const Real & r) const
{
  return Real(static_cast<const RealVal>(GetValue()).Sub(r.GetValue()));
}

Int Int::operator*(const Int & i) const
{
  return Int(static_cast<const IntVal *>(mvp)->Mult(i.GetValue()));
}

Real Int::operator*(const Real & r) const
{
  return Real(static_cast<const RealVal>(GetValue()).Mult(r.GetValue()));
}

Real Int::operator/(const Int & i) const
{
  return Real(static_cast<const RealVal>(GetValue()).Div(static_cast<RealVal::RealValueType>(i.GetValue())));
}

Real Int::operator/(const Real & r) const
{
  return Real(static_cast<const RealVal>(GetValue()).Div(r.GetValue()));
}

Real Int::Exp(const Int & i) const
{
  return Real(static_cast<const RealVal>(GetValue()).Pow(static_cast<RealVal::RealValueType>(i.GetValue ())));
}

Real Int::Exp(const Real & r) const
{
  return Real(static_cast<const RealVal>(GetValue()).Pow(r.GetValue ()));
}

Int& Int::Incr()
{
  this->Link(GetIntVal(static_cast<const IntVal *>(mvp)->Incr()));
  return *this;
}

Int& Int::operator=(const int64_t i)
{
  this->Link(GetIntVal(i));
  return *this;
}

Int & Int::operator=(const Generic & g)
{
  if (g.IsInt()) {
    this->Link(g);
    return *this;
  }
  else if (g.IsReal()) {
    this->Link(GetIntVal(static_cast<int64_t>(Real(g).GetValue())));
    return *this;
  }
  else {
    M4LibError::ReportError(ML_CAST_ERROR, g.PrintType() + L" casted to Int");
    return *this;
  }
}

Int & Int::operator=(const Int & i)
{
  this->Link(i.mvp);
  return *this;
}

Int & Int::operator=(const Real & r)
{
  this->Link(GetIntVal(static_cast<int64_t>(r.GetValue())));
  return *this;
}

int64_t Int::GetValue() const
{
  return static_cast<int64_t>(static_cast<const IntVal *>(mvp)->GetValue());
}

/**************************************************************************
*
* Nil
*
**************************************************************************/
Nil::Nil() : Common(ValCache->nilv)
{
}

Nil::Nil(const Nil& /*par*/) : Common(ValCache->nilv)
{
}

Nil::Nil(const Generic & I) : Common(I)
{
  if (!this->IsNil()) {
    M4LibError::ReportError(ML_CAST_ERROR, I.PrintType() + L" casted to Nil");
  }
}

Nil & Nil::operator=(const Nil & /*unused*/)
{
  // No need to do anything.
  return *this;
}

Nil & Nil::operator=(const Generic & g)
{
  if (!g.IsNil()) {
    M4LibError::ReportError(ML_CAST_ERROR, g.PrintType() + L" casted to Nil");
  }
  this->Link(g);
  return *this;
}

/**************************************************************************
*
*  Bool
*
**************************************************************************/
static inline
BoolVal* GetBoolVal(bool b)
{
  return (b ? ValCache->truev : ValCache->falsev);
}

Bool::Bool() : Common(ValCache->falsev)
{
}

Bool::Bool(bool b) : Common(GetBoolVal(b))
{
}

Bool::Bool(const Generic & B) : Common(B)
{
  if (!IsBool()) {
    M4LibError::ReportError(ML_CAST_ERROR, B.PrintType() + L" casted to Bool");
  }
}

Bool::operator bool() const
{
  return static_cast<const BoolVal *>(mvp)->GetValue();
}

Bool & Bool::operator=(const Bool & i)
{
  this->Link(i.mvp);
  return *this;
}

Bool & Bool::operator=(const bool b)
{
  this->Link(GetBoolVal(b));
  return *this;
}

Bool & Bool::operator=(const Generic & g)
{
  if (!g.IsBool()) {
    M4LibError::ReportError(ML_CAST_ERROR, g.PrintType() + L" casted to Bool");
  }

  this->Link(g);
  return *this;
}

bool Bool::GetValue() const
{
  return static_cast<const BoolVal *>(mvp)->GetValue();
}

Bool Bool::operator!() const
{
  return Bool(static_cast<const BoolVal *>(mvp)->Not());
}

Bool Bool::operator&&(bool b) const
{
  return Bool(static_cast<const BoolVal *>(mvp)->And(b));
}

Bool Bool::operator||(bool b) const
{
  return Bool(static_cast<const BoolVal *>(mvp)->Or(b));
}

Bool& Bool::operator&=(bool b)
{
  this->Link(GetBoolVal(static_cast<const BoolVal *>(mvp)->And(b)));
  return *this;
}

Bool& Bool::operator|=(bool b)
{
  this->Link(GetBoolVal(static_cast<const BoolVal *>(mvp)->Or(b)));
  return *this;
}

/**************************************************************************
*
*  RealVal
*
**************************************************************************/
void RealVal::ostream_out(wostream & os, const VDMFormatter & /*vf*/) const
{
  char buf[100];
  ::sprintf(buf, VLFORMAT, this->value);
  //  int n = os.precision();
  //  printf("string is %s\n", buf);
  //  os << setprecision(16) << value.GetVal() << setprecision(n);
  os << TBWSTR::string2wstring(buf);
}

/**************************************************************************
*
*  Real
*
**************************************************************************/
static inline
RealVal* GetRealVal(IntVal::IntValueType i)
{
  try {
#ifdef _MSC_VER
  return (((i >= 0) && (i < 256)) ? ValCache->realv[i] : new RealVal(static_cast<RealVal::RealValueType>(i)));
#else
  return (((i >= 0) && (i < 256)) ? ValCache->realv[i]
                                  : ValCache->GetCachedRealVal(static_cast<RealVal::RealValueType>(i)));
#endif // _MSC_VER
  }
  catch (bad_alloc &e) {
    M4LibError::ReportError(ML_NULL_POINTER, wstring(L"Memory Allocation failed."));
  }
  return NULL;
}

static inline
RealVal* GetRealVal(RealVal::RealValueType d)
{
  if ((d == VLFLOOR(d)) && (d <= 2147483647)) {
    return GetRealVal(static_cast<IntVal::IntValueType>(d));
  }
  return new RealVal(static_cast<RealVal::RealValueType>(d));
}

Real::Real() : Common(ValCache->realv[0])
{
}

Real::Real(const double d) : Common(GetRealVal(static_cast<RealVal::RealValueType>(d)))
{
}

Real::Real(const Int & i) : Common(GetRealVal(i.GetValue()))
{
}

Real::Real(const Generic & r) : Common(r)
{
  if (r.IsReal()) {
  }
  else if (r.IsInt()) {
    this->Link(GetRealVal(Int(r).GetValue()));
  }
  else {
    M4LibError::ReportError(ML_CAST_ERROR, r.PrintType() + L" casted to Real");
  }
}

Real::operator double() const
{
  return static_cast<const RealVal *>(mvp)->GetValue();
}

Real Real::operator-() const
{
  return Real(static_cast<const RealVal *>(mvp)->Minus());
}

Real Real::operator+(const Int & i) const
{
  return Real(static_cast<const RealVal *>(mvp)->Add(static_cast<RealVal::RealValueType>(i.GetValue ())));
}

Real Real::operator+(const Real & r) const
{
  return Real(static_cast<const RealVal *>(mvp)->Add(r.GetValue()));
}

Real Real::operator-(const Int & i) const
{
  return Real(static_cast<const RealVal *>(mvp)->Sub(static_cast<RealVal::RealValueType>(i.GetValue ())));
}

Real Real::operator-(const Real & r) const
{
  return Real(static_cast<const RealVal *>(mvp)->Sub(r.GetValue ()));
}

Real Real::operator*(const Int & i) const
{
  return Real(static_cast<const RealVal *>(mvp)->Mult(static_cast<RealVal::RealValueType>(i.GetValue ())));
}

Real Real::operator*(const Real & r) const
{
  return Real(static_cast<const RealVal *>(mvp)->Mult(r.GetValue ()));
}

Real Real::operator/(const Int & i) const
{
  return Real(static_cast<const RealVal *>(mvp)->Div(static_cast<RealVal::RealValueType>(i.GetValue())));
}

Real Real::operator/(const Real & r) const
{
  return Real(static_cast<const RealVal *>(mvp)->Div(r.GetValue()));
}

Real Real::Exp(const Int & i) const
{
  return Real(static_cast<const RealVal *>(mvp)->Pow(static_cast<RealVal::RealValueType>(i.GetValue ())));
}

Real Real::Exp(const Real & r) const
{
  return Real(static_cast<const RealVal *>(mvp)->Pow(r.GetValue ()));
}

Real& Real::operator=(const Real& i)
{
  this->Link(i.mvp);
  return *this;
}

Real& Real::operator=(const Int & i)
{
  this->Link(GetRealVal(static_cast<IntVal::IntValueType>(i.GetValue())));
  return *this;
}

Real& Real::operator=(const double r)
{
  this->Link(new RealVal(static_cast<RealVal::RealValueType>(r)));
  return *this;
}

Real& Real::operator=(const Generic & g)
{
  if (g.IsReal()) {
    this->Link(g);
    return *this;
  }
  else if (g.IsInt()) {
    this->Link(GetRealVal(static_cast<IntVal::IntValueType>(Int(g).GetValue())));
    return *this;
  }
  else {
    M4LibError::ReportError(ML_CAST_ERROR, g.PrintType() + L" casted to Real");
    return *this;
  }
}

double Real::GetValue() const
{
  return static_cast<double>(static_cast<const RealVal *>(mvp)->GetValue());
}

int64_t Real::GetIntValue() const
{
  return static_cast<int64_t>(static_cast<const RealVal *>(mvp)->GetValue());
}

bool Real::IsInt() const
{
  return static_cast<const RealVal *>(mvp)->IsInt();
}

bool Real::IsNat() const
{
  return static_cast<const RealVal *>(mvp)->IsNat();
}

bool Real::IsNatOne() const
{
  return static_cast<const RealVal *>(mvp)->IsNatOne();
}

bool Real::IsZero() const
{
  return static_cast<const RealVal *>(mvp)->IsZero();
}

Real Real::Abs() const
{
  return Real(static_cast<const RealVal *>(mvp)->Abs());
}

Real Real::Floor() const
{
  return Real(static_cast<const RealVal *>(mvp)->Floor());
}

Real Real::Ceil() const
{
  return Real(static_cast<const RealVal *>(mvp)->Ceil());
}

Real Real::Minus() const
{
  return Real(static_cast<const RealVal *>(mvp)->Minus());
}

Real Real::Add(const Real & r) const
{
  return Real(static_cast<const RealVal *>(mvp)->Add(r.GetValue()));
}

Real Real::Sub(const Real & r) const
{
  return Real(static_cast<const RealVal *>(mvp)->Sub(r.GetValue()));
}

Real Real::Mult(const Real & r) const
{
  return Real(static_cast<const RealVal *>(mvp)->Mult(r.GetValue()));
}

Real Real::Div(const Real & r ) const
{
  return Real(static_cast<const RealVal *>(mvp)->Div(r.GetValue()));
}

Real Real::IntDiv(const Real & r ) const
{
  return Real(static_cast<const RealVal *>(mvp)->IntDiv(r.GetValue()));
}

Real Real::Rem(const Real & r ) const
{
  return Real(static_cast<const RealVal *>(mvp)->Rem(r.GetValue()));
}

Real Real::Mod(const Real & r ) const
{
  return Real(static_cast<const RealVal *>(mvp)->Mod(r.GetValue()));
}

Real Real::Pow(const Real & r ) const
{
  return Real(static_cast<const RealVal *>(mvp)->Pow(r.GetValue()));
}

Bool Real::GreaterEqual(const Real & r) const
{
  return Bool(static_cast<const RealVal *>(mvp)->GreaterEqual(r.GetValue()));
}

Bool Real::GreaterThan(const Real & r) const
{
  return Bool(static_cast<const RealVal *>(mvp)->GreaterThan(r.GetValue()));
}

Bool Real::LessEqual(const Real & r) const
{
  return Bool(static_cast<const RealVal *>(mvp)->LessEqual(r.GetValue()));
}

Bool Real::LessThan(const Real & r) const
{
  return Bool(static_cast<const RealVal *>(mvp)->LessThan(r.GetValue()));
}

Real Real::HexAnd(const Real & r) const
{
  return Real(static_cast<const RealVal *>(mvp)->HexAnd(r.GetValue()));
}

Real Real::HexOr(const Real & r) const
{
  return Real(static_cast<const RealVal *>(mvp)->HexOr(r.GetValue()));
}

Real Real::HexExor(const Real & r) const
{
  return Real(static_cast<const RealVal *>(mvp)->HexExor(r.GetValue()));
}

Real Real::HexShiftRight(const Real & r) const
{
  return Real(static_cast<const RealVal *>(mvp)->HexShiftRight(r.GetValue()));
}

Real Real::HexByteShiftRight(const Real & r) const
{
  return Real(static_cast<const RealVal *>(mvp)->HexByteShiftRight(r.GetValue()));
}

Real Real::HexShiftLeft(const Real & r) const
{
  return Real(static_cast<const RealVal *>(mvp)->HexShiftLeft(r.GetValue()));
}

Real Real::HexByteShiftLeft(const Real & r) const
{
  return Real(static_cast<const RealVal *>(mvp)->HexByteShiftLeft(r.GetValue()));
}

/**************************************************************************
*
*  CharVal
*
**************************************************************************/
void CharVal::ostream_out(wostream & os, const VDMFormatter & /*vf*/) const
{
  wstring s;
  s += this->value;
  os << L"'";
  Backslashed::convert_internal_to_printed_backslash(s, os, false);
  os << L"'";
};

/**************************************************************************
*
*  Char
*
**************************************************************************/
static inline
CharVal* GetCharVal(wchar_t ch)
{
  try {
    return ((ch < 256) ? ValCache->charv[(wchar_t)ch] : new CharVal(ch));
  }
  catch (bad_alloc &e) {
    M4LibError::ReportError(ML_NULL_POINTER, wstring(L"Memory Allocation failed."));
  }
  return NULL;
}

Char::Char() : Common(ValCache->charv[L'?'])
{
}

Char::Char(wchar_t ch) : Common(GetCharVal(ch))
{
}

Char::Char(const Generic & G) : Common(G)
{
  if (!this->IsChar()) {
    M4LibError::ReportError(ML_CAST_ERROR, G.PrintType() + L" casted to Char");
  }
}

Char::operator wchar_t()
{
  return static_cast<const CharVal *>(mvp)->GetValue();
}

Char & Char::operator=(const Char & i)
{
  this->Link(i.mvp);
  return *this;
}

Char& Char::operator=(const wchar_t i)
{
  this->Link(GetCharVal(i));
  return *this;
}

Char & Char::operator=(const Generic & g)
{
  if (!g.IsChar()) {
    M4LibError::ReportError(ML_CAST_ERROR, g.PrintType() + L" casted to Char");
  }

  this->Link(g);
  return *this;
}

wchar_t Char::GetValue() const
{
  return static_cast<const CharVal *>(mvp)->GetValue();
}

int Char::ToHexNum() const
{
  return static_cast<const CharVal *>(mvp)->ToHexNum();
}

int Char::ToDecNum() const
{
  return static_cast<const CharVal *>(mvp)->ToDecNum();
}

int Char::ToBinNum() const
{
  return static_cast<const CharVal *>(mvp)->ToBinNum();
}

/**************************************************************************
*
*  QuoteVal
*
**************************************************************************/
void QuoteVal::WriteVal (ostream & ostr) const
{
  string tmpstr (TBWSTR::wstring2string(this->value));
  ostr << 'k' << (int) tmpstr.length() << "," << tmpstr;
}

/**************************************************************************
*
*  Quote
*
**************************************************************************/
static inline
QuoteVal* GetQuoteVal(const wstring & c)
{
  try {
    return (c.empty() ? ValCache->quotev : ValCache->GetCachedQuoteVal(c));
  }
  catch (bad_alloc &e) {
    M4LibError::ReportError(ML_NULL_POINTER, wstring(L"Memory Allocation failed."));
  }
  return NULL;
}

Quote::Quote() : Common(ValCache->quotev)
{
}

Quote::Quote(const wstring & c) : Common(GetQuoteVal(c))
{
}

Quote::Quote(const wchar_t * c) : Common(GetQuoteVal(NULL == c ? wstring(L"") : wstring(c)))
{
}

Quote::Quote(const Generic & g) : Common(g)
{
  if (!this->IsQuote()) {
    M4LibError::ReportError(ML_CAST_ERROR, g.PrintType() + L" casted to Quote");
  }
}

Quote & Quote::operator=(const Quote & i)
{
  this->Link(i.mvp);
  return *this;
}

Quote & Quote::operator=(const wchar_t * c)
{
  this->Link(GetQuoteVal(c));
  return *this;
}

Quote & Quote::operator=(const wstring & c)
{
  this->Link(new QuoteVal(c));
  return *this;
}

Quote & Quote::operator=(const Generic & g)
{
  if (!g.IsQuote()) {
    M4LibError::ReportError(ML_CAST_ERROR, g.PrintType() + L" casted to Quote");
  }
  this->Link(g);
  return *this;
}

wstring Quote::GetValue() const
{
  return static_cast<const QuoteVal *>(mvp)->GetValue();
}

/**************************************************************************
*
*  DLObject
*
**************************************************************************/
DLObject::DLObject(const wstring & name, DlClass *p) : Common(new DLObjectVal(name, p))
{
}

DLObject::DLObject(const Generic & g) : Common(g)
{
  if (!this->IsDLObject()) {
    M4LibError::ReportError(ML_CAST_ERROR, g.PrintType() + L" casted to DLObject");
  }
}

DLObject & DLObject::operator=(const DLObject & i)
{
  this->Link(i.mvp);
  return *this;
}

DLObject & DLObject::operator=(const Generic & g)
{
  if (!g.IsDLObject()) {
    M4LibError::ReportError(ML_CAST_ERROR, g.PrintType() + L" casted to DLObject");
  }
  this->Link(g);
  return *this;
}

wstring DLObject::GetName() const
{
  return static_cast<const DLObjectVal *>(mvp)->GetName();
}

DlClass * DLObject::GetPtr() const
{
  return static_cast<const DLObjectVal *>(mvp)->GetDLClass();
}

/**************************************************************************
*
*  Text
*
**************************************************************************/
void TextVal::WriteVal (ostream & ostr) const
{
  string tmpstr (TBWSTR::wstring2string(this->value));
  ostr << 'l' << (int) tmpstr.length() << "," << tmpstr;
}

/**************************************************************************
*
*  Text
*
**************************************************************************/
static inline
TextVal* GetTextVal(const wchar_t* c)
{
  return ((c == NULL) ? ValCache->textv : new TextVal(wstring(c)));
}

Text::Text() : Common(ValCache->textv)
{
}

Text::Text(const wstring & cr) : Common(new TextVal(cr))
{
}

Text::Text(const wchar_t* cr) : Common(GetTextVal(cr))
{
}

Text::Text(const Generic & m) : Common(m)
{
  if (!this->IsText()) {
    M4LibError::ReportError(ML_CAST_ERROR, m.PrintType() + L" casted to Text");
  }
}

wstring Text::GetValue() const
{
  return static_cast<const TextVal *>(mvp)->GetValue();
}

Text & Text::operator=(const Text & i)
{
  this->Link(i.mvp);
  return *this;
}

Text & Text::operator=(const wchar_t * i)
{
  this->Link(GetTextVal(i));
  return *this;
}

Text & Text::operator=(const wstring & i)
{
  this->Link(new TextVal(i));
  return *this;
}

Text & Text::operator=(const Generic & g)
{
  if (!g.IsText()) {
    M4LibError::ReportError(ML_CAST_ERROR, g.PrintType() + L" casted to Text");
  }
  this->Link(g);
  return *this;
}

/**************************************************************************
*
*  TokenVal
*
**************************************************************************/
void TokenVal::WriteVal (ostream & ostr) const
{
  if (IsString()) {
    string tmpstr (TBWSTR::wstring2string(GetString()));
    ostr << 'm' << (int) tmpstr.length() << "," << tmpstr;
  }
  else {
    ostringstream o;
    this->value.WriteVal(o);
    string tmpstr (o.str());
    ostr << 'o' << (int) tmpstr.length() << "," << tmpstr;
  }
}

/**************************************************************************
*
*  Token
*
**************************************************************************/
static inline
TokenVal* GetTokentVal(const wchar_t* c)
{
  try {
    return ((c == NULL) ? ValCache->tokenv : new TokenVal(wstring(c)));
  }
  catch (bad_alloc &e) {
    M4LibError::ReportError(ML_NULL_POINTER, wstring(L"Memory Allocation failed."));
  }
  return NULL;
}

Token::Token() : Common(ValCache->tokenv)
{
}

Token::Token(const wstring & cr) : Common(new TokenVal(cr))
{
}

Token::Token(const wchar_t* cr) : Common(GetTokentVal(cr))
{
}

Token::Token(const Generic & m) : Common(m)
{
  if (!this->IsToken()) {
    M4LibError::ReportError(ML_CAST_ERROR, m.PrintType() + L" casted to Token");
  }
}

const Generic & Token::GetValue() const
{
  return (const Generic &)static_cast<const TokenVal *>(mvp)->GetValue();
}

wstring Token::GetString() const
{
  return static_cast<const TokenVal *>(mvp)->GetString();
}

Token & Token::operator=(const Token & i)
{
  this->Link(i.mvp);
  return *this;
}

Token & Token::operator=(const wchar_t * i)
{
  this->Link(GetTokentVal(i));
  return *this;
}

Token & Token::operator=(const wstring & i)
{
  this->Link(new TokenVal(i));
  return *this;
}

Token & Token::operator=(const Generic & g)
{
  if (!g.IsToken()) {
    M4LibError::ReportError(ML_CAST_ERROR, g.PrintType() + L" casted to Token");
  }

  this->Link(g);
  return *this;
}

Token mk_token(const Common & c)
{
  return Token(Common(new TokenVal(c)));
}

/**************************************************************************
*
*  DRPtr
*
**************************************************************************/
DRPtr::DRPtr(MetaivVal * p) : Common(new DRPtrVal(p))
{
}

MetaivVal * DRPtr::GetPtr() const
{
  return static_cast<const DRPtrVal *>(mvp)->GetValue();
}

DRPtr & DRPtr::operator=(const Common & g)
{
  if (g.IsDRPtr()) {
    this->Link(g);
    return *this;
  }
  else {
    M4LibError::ReportError(ML_CAST_ERROR, g.PrintType() + L" casted to DRPtr");
    return *this;
  }
}

/**************************************************************************
*
*  Time
*
**************************************************************************/
void TimeVal::ostream_out(wostream & os, const VDMFormatter & /*vf*/) const
{
#ifdef _MSC_VER
  char timebuf[30];
  ctime_s(timebuf, 26, &value);
  string timestr (timebuf);
  os << string2wstring(timestr.substr(0, 24));
#else
  string timestr (ctime(&value));
//  char timebuf[30];
//  strftime(timebuf, 29, "%a %b %d %H:%M:%S %Z %Y", localtime(&value));
//  string timestr (timebuf);
  os << string2wstring(timestr.substr(0, 24));
#endif // _MSC_VER
}

void TimeVal::WriteVal (ostream & ostr) const
{
}

int TimeVal::Compare(const MetaivVal & mval) const
{
  switch(mval.GetType()) {
    case mt_time: {
      TimeValueType t ((static_cast<const TimeVal &>(mval).GetValue()));
      return ((this->value == t) ? 0 : ((this->value > t) ? 1 : -1 ));
    }
    default: {
      return this->GetType() < mval.GetType() ? -1 : 1; // Should never be executed.
    }
  }
  return 0;
}

wstring TimeVal::GetString(const string & format) const
{
  char buf [50];
#ifdef _MSC_VER
  setlocale( LC_TIME, "English" );
#else
  setlocale( LC_TIME, "en_US" );
#endif // _MSC_VER
  strftime (buf, 49, format.c_str(), localtime (&value));
  return string2wstring(std::string (buf));
}

Time::Time() : Common(new TimeVal())
{
}

Time & Time::operator=(const Common & g)
{
  if (g.IsTime()) {
    this->Link(g);
    return *this;
  }
  else {
    M4LibError::ReportError(ML_CAST_ERROR, g.PrintType() + L" casted to Time");
    return *this;
  }
}

wstring Time::GetString(const string & format) const
{
  return static_cast<const TimeVal *>(mvp)->GetString(format);
}

/**************************************************************************
*
*  Function
*
**************************************************************************/
Function::Function() : Common(new FunctionVal())
{
}

Function::Function(const wstring & nm, vdm_function_ptr p) : Common(new FunctionVal(nm,p))
{
}

Function::Function(const wchar_t* nm, vdm_function_ptr p) : Common(new FunctionVal(wstring(nm),p))
{
}

Function & Function::operator=(const Common & c)
{
  if (c.IsFunction()) {
    this->Link(c);
    return *this;
  }
  else {
    M4LibError::ReportError(ML_CAST_ERROR, c.PrintType() + L" casted to Function");
    return *this;
  }
}

Sequence Function::Apply(const Sequence & args) const
{
  return static_cast<const FunctionVal *>(mvp)->Apply(args);
}

wstring Function::GetName() const
{
  return static_cast<const FunctionVal *>(mvp)->GetName();
}
//////////////////////////////////////////////////////////////////////
//======================================================================
#ifdef SHOW_MALLOC_STATS
static ofstream *liblog;
#endif // SHOW_MALLOC_STATS

int VDMLibControl::initiated = 0; // Inititialised to zero by compiler.

void VDMLibControl::init()
{
  if (VDMLibControl::initiated == 0) {
    CopyOnModify = true;
#ifdef VDM_FINGERPRINT
    Fingerprint::crcgen();
#endif // VDM_FINGERPRINT
#ifdef SHOW_MALLOC_STATS
    liblog = new ofstream(L"mem.log");
    showmalloccount = new ShowMallocCount;
#endif // SHOW_MALLOC_STATS
    DefRecInfo = new VDMRecInfoMap();
    ValCache = new VDMValCache();
  }
  VDMLibControl::initiated++;
}

bool vdmdlgettag(VDMRecInfoMap *m, const wchar_t *s, int *tag, int *size)
{
  wstring tags(s);
  return m->GetTag(tags, *tag, *size);
}

static wstring * res; // must be static since we return its c_str().

const wchar_t * vdmdlgetsymtag(VDMRecInfoMap *m, int tag)
{
  if (!res) {
    res = new wstring;
  }
  m->GetSymTag(tag, *res);
  return res->c_str();
}

static VDMRecInfoMap * VDMGetDefaultRecInfoMapPtr()
{
  return DefRecInfo;
}

static unsigned int vdmgetvdmlibversion()
{
  return VDMLibVersion;
}

void vdmdlgetfunctions(vdmdlfunctions *f)
{
  f->vdmgetvdmlibversion = vdmgetvdmlibversion;
  f->vdmgetdefaultrecinfomapptr = VDMGetDefaultRecInfoMapPtr;
  f->vdmdlgettag = vdmdlgettag;
  f->vdmdlgetsymtag = vdmdlgetsymtag;
}

bool VDMLibControl::CopyOnModify;
int  VDMLibControl::CopyCountSequence;
int  VDMLibControl::CopyCountTuple;
int  VDMLibControl::CopyCountRecord;
int  VDMLibControl::CopyCountSet;
int  VDMLibControl::CopyCountMap;
int  VDMLibControl::CopyCountToken;
int  VDMLibControl::CopyCountInt;
int  VDMLibControl::CopyCountReal;
int  VDMLibControl::CopyCountText;
int  VDMLibControl::CopyCountQuote;

void VDMLibControl::print_copy_stats(wostream &o)
{
  o << L"CopyOnModify=" << CopyOnModify << endl
    << L"CopyCountSequence= " << CopyCountSequence  << endl
    << L"CopyCountTuple=    " << CopyCountTuple     << endl
    << L"CopyCountRecord=   " << CopyCountRecord    << endl
    << L"CopyCountSet=      " << CopyCountSet       << endl
    << L"CopyCountMap=      " << CopyCountMap       << endl
    << L"CopyCountToken=    " << CopyCountToken     << endl
    << L"CopyCountInt=      " << CopyCountInt       << endl
    << L"CopyCountReal=     " << CopyCountReal      << endl
    << L"CopyCountText=     " << CopyCountText      << endl
    << L"CopyCountQuote=    " << CopyCountQuote     << endl
    ;
}

VDMLibControl::VDMLibControl()
{
  init();
}

VDMLibControl::VDMLibControl(unsigned int hdr_vers)
{
  init();
  if (hdr_vers != VDMLibVersion && hdr_vers > 0) {
    wcerr << L"Warning (VDM Library): library version is: "
          << VDMLibVersion
          << L". Header file version is: "
          << hdr_vers
          << endl;
  }
}

VDMLibControl::~VDMLibControl()
{
  // Empty.
  VDMLibControl::initiated--;
  if (VDMLibControl::initiated == 0)
  {
    delete DefRecInfo;
    delete ValCache;
    delete DefSerializeInfo;

#ifdef SHOW_MALLOC_STATS
    cout << L"BuildCount=" << MetaivVal::BuildCount << endl;
    cout << L"DestrCount=" << MetaivVal::DestroyCount << endl;
    showmalloccount->Report(*liblog);
    liblog->close();
#endif // SHOW_MALLOC_STATS
  }
}

//////////////////////////////////////////////////
static
void VDMDefaultEH(int i)
{
  exit(i);
}

static VDMErrorHandler vdm_eh;

VDMErrorHandler VDMSetErrorHandler(VDMErrorHandler fct)
{
  VDMErrorHandler f = vdm_eh;
  vdm_eh = fct;
  return f;
}

void VDMErrorHandle(int error, int exi)
{
  if (vdm_eh != 0) {
    (*vdm_eh)(error, exi);
  } else {
    VDMDefaultEH(exi);
  }
}

////////////////////////////////////////////////////////////
// VDMRecInfoMap
////////////////////////////////////////////////////////////

VDMRecInfoMap & VDMGetDefaultRecInfoMap()
{
  return *DefRecInfo;
}

////////////////////////////////////////////////////////////
// VDMSerializeInfoMap
////////////////////////////////////////////////////////////

VDMSerializeInfoMap & VDMGetDefaultSerializeInfoMap()
{
  if (! DefSerializeInfo) {
    DefSerializeInfo = new VDMSerializeInfoMap();
  }
  return *DefSerializeInfo;
}

Common vdm_Seri::DeSerialize(istream & is) const
{
  return (*this->de_seri)(is);
};

void VDMSerializeInfoMap::Register(const wstring & typeidname,
                                   vdm_Seri::vdm_DeSerialize_function deseri,
                                   vdm_Seri::vdm_Serialize_function seri)
{
  typedef pair<VDMSerializeInfoMap_t::iterator,bool>  pair_ib_t;
  vdm_Seri vs(deseri, seri);
  pair_ib_t p = this->mapvalue.insert(VDMSerializeInfoMap_t::value_type(typeidname, vs));
  if (p.second == false) { // Existed
    cerr << "Register: duplicate typeinfo" << TBWSTR::wstring2string(typeidname) << endl;
  }
}

const vdm_Seri * VDMSerializeInfoMap::LookUp(const wstring & typeidname)
{
  VDMSerializeInfoMap_t::iterator i = this->mapvalue.find(typeidname);
  if (i != this->mapvalue.end()) { // Found.
    return & (*i).second;
  } else {
    return NULL;
  }
}

string ReadToken(istream& istr)
{
  string ret;
  while(!istr.eof()) {
    char c;
    istr.get(c);
    if( c == ',' ) break;
    ret += c;
  }
  return ret;
}

int64_t ReadInteger(istream& istr)
{
  string s = ReadToken(istr);
  return atol(s.c_str());
}

double ReadDouble(istream& istr)
{
  string s = ReadToken(istr);
  return atof(s.c_str());
}

int ReadChar(istream& istr)
{
  string s;
  while(!istr.eof()) {
    int n = istr.peek();
    if( (n < '0') || (n > '9') ) break;
    char c;
    istr.get(c);
    s += c;
  }
  int val = atoi(s.c_str());
  return val;
}

wstring ReadString(istream& istr)
{
  int length = ReadInteger(istr);
  string s;
  for(int i = 1; i <= length; i++ ) {
    char c;
    istr.get(c);
    s += c;
  }
  return TBWSTR::string2wstring(s);
}

Generic ReadVal(istream& istr)
{
  Generic res;
  char ch;
  istr.get(ch);
  switch (ch){
    case 'a': {// Map
      int length = ReadInteger(istr);
      Map r;
      for (int i = 1; i <= length; i++) {
        Generic dom (ReadVal (istr));
        Generic rng (ReadVal (istr));
        r.Insert (dom, rng);
      }
      res = r;
      break;
    }
    case 'b': {// Sequence
      int length = ReadInteger(istr);
      Sequence r;
      for (int i = 1; i <= length; i++) {
        r.ImpAppend (ReadVal (istr));
      }
      res = r;
      break;
    }
    case 'c': {// Set
      int length = ReadInteger(istr);
      Set r;
      for (int i = 1; i <= length; i++) {
        r.Insert (ReadVal (istr));
      }
      res = r;
      break;
    }
    case 'd': {// Record
      int tag = ReadInteger(istr);
      int length = ReadInteger(istr);
      Record r (tag, length);

      for (int i = 1; i <= length; i++) {
        r.SetField (i, ReadVal (istr));
      }
      res = r;
      break;
    }
    case 'e': {// Tuple
      int length = ReadInteger(istr);
      Tuple r (length);
      for (int i = 1; i <= length; i++) {
        r.SetField (i, ReadVal (istr));
      }
      res = r;
      break;
    }
    case 'f': {// Int
      int val = ReadInteger(istr);
      Int r(val);
      res = r;
      break;
    }
    case 'g': {// Nil
      Nil r;
      res = r;
      break;
    }
    case 'h': {// Bool
      int val = ReadInteger(istr);
      Bool r(val != 0);
      res = r;
      break;
    }
    case 'i': {// Real
      double val = ReadDouble(istr);
      Real r(val);
      res = r;
      break;
    }
    case 'j': {// Char
      Char r(ReadChar(istr));
      res = r;
      break;
    }
    case 'k': {// Quote
      Quote r (ReadString(istr));
      res = r;
      break;
    }
    case 'l': {// Text
      Text r (ReadString(istr));
      res = r;
      break;
    }
    case 'm': {// Token
      Token r (ReadString(istr));
      res = r;
      break;
    }
    case 'n': {// Container types
      wstring id = ReadString(istr);
      char dummy;istr.get(dummy); // skip ','
      const vdm_Seri * vs = VDMGetDefaultSerializeInfoMap().LookUp(id);
      if (vs != NULL) {
        return vs->DeSerialize(istr);
      } else {
        cerr << "ReadVal: unknown container("
             << TBWSTR::wstring2string(id) << ")" << endl << flush;
      }
      break;
    }
    case 'o': {
      int length = ReadInteger(istr);
      string s;
      for (int i = 1; i <= length; i++) {
        char c;
        istr.get(c);
        s += c;
      }
      istringstream is (s);
      Generic g (ReadVal(is));
      res = mk_token(g) ;
      break;
    }
    default: {
      cerr << L"Unknown key: " << ch << endl << flush;
      break;
    }
  }
  return res;
}

