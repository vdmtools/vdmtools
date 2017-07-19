/////////////////////////////////////////////////////////////////////////////

// WHAT
//  VDM Library value definitions.
// ID
//    $Id: vdmlib_values.h,v 1.17 2006/06/14 06:15:53 vdmtools Exp $
// PROJECT
//   Toolbox
// COPYRIGHT
//   (C) Kyushu University
/////////////////////////////////////////////////////////////////////////////

#ifndef __vdmlib_values_h__
#define __vdmlib_values_h__

#include <map>
#include <iterator>
#include <set>
#include <vector>
#include <string>
#include <deque>
#include <time.h>
#include "stdarg.h"
#include <iomanip>
#include <math.h>
#include <cmath>

//
// NilVal
//
class NilVal : public MetaivVal
{
public:
  NilVal() : MetaivVal(mt_nil) {};

  void ostream_out(wostream&os, const VDMFormatter& /*vf*/) const
  { os << L"nil"; };
  void WriteVal (ostream& ostr) const
  { ostr << 'g'; };
  int Compare(const MetaivVal &) const { return 0; };
  bool IsSimple() const { return true; };
  MetaivVal * Copy() const { return new NilVal(); };
};

//
// CharVal
//
class CharVal : public MetaivVal
{
public:
  typedef wchar_t CharValueType;

private:
  const CharValueType value;

public:
  CharVal() : MetaivVal(mt_char), value(L'?') {};
  CharVal(CharValueType ch) : MetaivVal(mt_char), value(ch) {}

  CharValueType GetValue() const { return this->value; };

  void ostream_out(wostream & os, const VDMFormatter & /*vf*/) const;
  void WriteVal (ostream& ostr) const
  { ostr << 'j' << this->value; };
  int Compare(const MetaivVal & mval) const
  {
    CharValueType c = (static_cast<const CharVal &>(mval).GetValue());
    return ((this->value == c) ? 0 : ((this->value > c) ? 1 : -1));
  };
  MetaivVal * Copy() const { return new CharVal(this->value); };
  bool IsSimple() const { return true; };

  int ToHexNum() const
  {
    switch(this->value) {
      case L'0': return 0;
      case L'1': return 1;
      case L'2': return 2;
      case L'3': return 3;
      case L'4': return 4;
      case L'5': return 5;
      case L'6': return 6;
      case L'7': return 7;
      case L'8': return 8;
      case L'9': return 9;
      case L'a':
      case L'A': return 10;
      case L'b':
      case L'B': return 11;
      case L'c':
      case L'C': return 12;
      case L'd':
      case L'D': return 13;
      case L'e':
      case L'E': return 14;
      case L'f':
      case L'F': return 15;
      default: return 0;
    }
  };

  int ToDecNum() const
  {
    switch(this->value) {
      case L'0': return 0;
      case L'1': return 1;
      case L'2': return 2;
      case L'3': return 3;
      case L'4': return 4;
      case L'5': return 5;
      case L'6': return 6;
      case L'7': return 7;
      case L'8': return 8;
      case L'9': return 9;
      default: return 0;
    }
  };

  int ToBinNum() const
  {
    switch(this->value) {
      case L'0': return 0;
      case L'1': return 1;
      default: return 0;
    }
  };
};

//
// IntVal
//
class RealVal;
class IntVal : public MetaivVal
{
public:
  typedef int64_t IntValueType;

private:
  const IntValueType value;

public:
  IntVal() : MetaivVal(mt_int), value(0) {};
  IntVal(IntValueType i) : MetaivVal(mt_int), value(i) {};

  IntValueType GetValue() const { return this->value; };

  void ostream_out(wostream & o, const VDMFormatter & v) const
  { o << this->value; };

  void WriteVal(ostream & ostr) const
  { ostr << 'f' << this->value << ","; };

  int Compare(const MetaivVal & mval) const;
  bool IsSimple() const { return true; };

  MetaivVal * Copy() const { return new IntVal(this->value); };

  bool IsZero() const { return (this->value == 0); };

  IntValueType Incr() const { return (this->value + 1); };
  IntValueType Minus() const { return -(this->value); };

  IntValueType Add(IntValueType i) const { return (value + i); };
  IntValueType Sub(IntValueType i) const { return (value - i); };
  IntValueType Mult(IntValueType i) const { return (value * i); };
  IntValueType Div(IntValueType i) const { return (value / i); };
};

#define LONGDOUBLE 1
#if defined(LONGDOUBLE) && !defined(_MSC_VER) && !defined(__Cygwin__)
#define VLFLOOR floorl
#define VLFABS std::fabs
#define VLCEIL ceill
#ifdef __FreeBSD__
#define VLPOW pow
#else
#define VLPOW powl
#endif // __FreeBSD__
#define VLSTRTOD strtold
#else
#define VLFLOOR floor
#define VLFABS fabs
#define VLCEIL ceil
#define VLPOW pow
#define VLSTRTOD strtod
#endif // defined(LONGDOUBLE) && !defined(_MSC_VER)
//
// RealVal
//
class RealVal : public MetaivVal
{
public:
#ifdef LONGDOUBLE
  typedef long double RealValueType;
#define VLFORMAT "%.16Lg"
#else
  typedef double RealValueType;
#define VLFORMAT "%.16lg"
#endif // LONGDOUBLE

private:
  const RealValueType value;

public:
  RealVal() : MetaivVal(mt_real), value(0.0) {};
  RealVal(RealValueType d) : MetaivVal(mt_real), value(d) {};

  RealValueType GetValue() const { return this->value; };

  void ostream_out(wostream & os, const VDMFormatter & /*vf*/) const;

  void WriteVal (ostream & ostr) const
  { ostr << 'i' << setprecision(20) << this->value << ","; };

  int Compare(const MetaivVal & mval) const
  {
    switch(mval.GetType()) {
      case mt_real: {
        RealValueType r ((static_cast<const RealVal &>(mval)).GetValue());
        return ((this->value == r) ? 0 : ((this->value > r) ? 1 : -1));
      }
      case mt_int: {
        IntVal::IntValueType i ((static_cast<const IntVal &>(mval)).GetValue());
        return ((this->value == i) ? 0 : ((this->value > i) ? 1 : -1));
      }
      default: {
        return this->GetType() < mval.GetType() ? -1 : 1;
      }
    }
  };
  bool IsSimple() const { return true; };

  MetaivVal * Copy() const { return new RealVal(this->value); };

  bool IsInt() const { return (this->value == VLFLOOR(this->value)); };
  bool IsNat() const { return ((this->value == VLFLOOR(this->value)) && (this->value >= 0)); };
  bool IsNatOne() const { return ((this->value == VLFLOOR(this->value)) && (this->value > 0)); };

  bool IsZero() const { return (this->value == 0); };

  RealValueType Minus() const { return -(this->value); };
  RealValueType Abs() const { return VLFABS(this->value); };
  RealValueType Floor() const { return VLFLOOR(this->value); };
  RealValueType Ceil() const { return VLCEIL(this->value); };

  RealValueType Add (RealValueType d) const { return (this->value + d); };
  RealValueType Sub (RealValueType d) const { return (this->value - d); };
  RealValueType Mult (RealValueType d) const { return (this->value * d); };
  RealValueType Div (RealValueType d) const
  { if (d == 0.0) { M4LibError::ReportError(ML_DIV_BY_ZERO, L"Division by zero"); return 0; }
    else          { return (this->value / d); } };
  RealValueType IntDiv (RealValueType d) const
  { if (d == 0.0) { M4LibError::ReportError(ML_DIV_BY_ZERO, L"Division by zero"); return 0; }
    else          { RealValueType n = this->value / d; return ((n < 0) ? -VLFLOOR(-n) : VLFLOOR(n)); } };
  RealValueType Rem (RealValueType d) const
  { if (d == 0.0) { M4LibError::ReportError(ML_DIV_BY_ZERO, L"Division by zero"); return 0; }
    else          { return (this->value - d * IntDiv(d)); } };
  RealValueType Mod (RealValueType d) const
  { if (d == 0.0) { M4LibError::ReportError(ML_DIV_BY_ZERO, L"Division by zero"); return 0; }
    else          { return (this->value - d * VLFLOOR(this->value/d)); } };
  RealValueType Pow (RealValueType d) const { return VLPOW(this->value, d); };

  bool GreaterEqual (RealValueType d) const { return (this->value >= d); };
  bool GreaterThan (RealValueType d) const { return (this->value > d); };
  bool LessEqual (RealValueType d) const { return (this->value <= d); };
  bool LessThan (RealValueType d) const { return (this->value < d); };

  IntVal::IntValueType HexAnd (RealValueType d) const
  { return (static_cast<IntVal::IntValueType>(this->value) & static_cast<IntVal::IntValueType>(d)); };
  IntVal::IntValueType HexOr (RealValueType d) const
  { return (static_cast<IntVal::IntValueType>(this->value) | static_cast<IntVal::IntValueType>(d)); };
  IntVal::IntValueType HexExor (RealValueType d) const
  { return (static_cast<IntVal::IntValueType>(this->value) ^ static_cast<IntVal::IntValueType>(d)); };
  IntVal::IntValueType HexShiftRight (RealValueType d) const
  { return (static_cast<IntVal::IntValueType>(this->value) >> static_cast<IntVal::IntValueType>(d)); };
  IntVal::IntValueType HexByteShiftRight (RealValueType d) const
  { return (static_cast<IntVal::IntValueType>(this->value) >> static_cast<IntVal::IntValueType>(d)) & 0xFF; };
  IntVal::IntValueType HexShiftLeft (RealValueType d) const
  { return (static_cast<IntVal::IntValueType>(this->value) << static_cast<IntVal::IntValueType>(d)); };
  IntVal::IntValueType HexByteShiftLeft (RealValueType d) const
  { return (static_cast<IntVal::IntValueType>(this->value) << static_cast<IntVal::IntValueType>(d)) & 0xFF; };
  IntVal::IntValueType HexAdd (RealValueType d, RealValueType c) const
  { return (static_cast<IntVal::IntValueType>(value + d + c)); };
  IntVal::IntValueType HexSub (RealValueType d, RealValueType c) const
  { return (static_cast<IntVal::IntValueType>(value - d - c)); };
};

//
// QuoteVal
//
class QuoteVal : public MetaivVal
{
public:
  typedef std::wstring QuoteValueType;

private:
  const QuoteValueType value;

public:
  QuoteVal() : MetaivVal(mt_quote) {};
  QuoteVal(const QuoteValueType & q) : MetaivVal(mt_quote), value(q) {};

  QuoteValueType GetValue() const { return this->value; };

  void ostream_out(wostream & os, const VDMFormatter &) const
  { os << L"<" << this->value << L">"; };

  void WriteVal (ostream & ostr) const;

  int Compare(const MetaivVal & mval) const
  {
    QuoteValueType q (static_cast<const QuoteVal &>(mval).GetValue());
    return ((this->value == q) ? 0 : ((this->value < q) ? -1 : 1));
  }
  bool IsSimple() const { return true; };
  MetaivVal * Copy() const { return new QuoteVal(this->value); };
};

//
// TupleVal
//
class TupleVal : public MetaivVal
{
public:
  typedef std::vector<Common> TupleValueType;

private:
  friend class Tuple;
  TupleValueType value;
  int max;

public:
  TupleVal() : MetaivVal(mt_tuple), max(0) {};
  TupleVal(int size) : MetaivVal(mt_tuple), value(size), max(size) {};
  TupleVal(const TupleVal & t) : MetaivVal(mt_tuple), value(t.value), max(t.max) {};
  ~TupleVal() { this->value.clear(); };
  static MetaivVal * mk_tupleval(int num, ...);

  TupleVal & SetField(int i, const Common & gdom);
  const Common & GetField(int i) const;
  Sequence GetFields() const;
  Common & refField(int i);
  int Length() const { return this->value.size(); };
  bool IsEmpty() const { return this->value.empty(); };
  bool OutOfRange(int i) const { return ((i < 1 ) || (i > (int)this->value.size())); };

  void ostream_out(wostream & os, const VDMFormatter & vf) const;
  int Compare(const MetaivVal & t) const;
  bool IsSimple() const;
  void WriteVal (ostream & ostr) const;
  MetaivVal * Copy() const { return new TupleVal(*this); };
};

//
// RecordVal
//
class RecordVal : public MetaivVal
{
public:
  typedef std::vector<Common> RecordValueType;

private:
  friend class Record;
  friend class Generic;

  RecordValueType value;
  const VDM_RecInfo * recinfop;

private:
  int GetTag() const { return this->recinfop->GetTag(); };
  std::wstring GetTextTag() const { return this->recinfop->GetSymTag(); };
  int GetSize() const { return this->recinfop->GetSize(); };
  bool IsDontCare(int i) const { return this->recinfop->AskDontCare(i - 1); };

public:
  bool Is(const int tag) const { return this->recinfop->Is(tag); };
  RecordVal() : MetaivVal(mt_record), recinfop(VDMGetDefaultRecInfoMap().Register(0, 0)) {};
  RecordVal(int tag, int size, VDMRecInfoMap & tagspace)
    : MetaivVal(mt_record), value(size), recinfop(tagspace.Register(tag, size)) {};
  RecordVal(const wstring & symtag, const VDMRecInfoMap & tagspace)
    : MetaivVal(mt_record), recinfop(tagspace.GetInfo(symtag))
  { this->value.reserve(recinfop->GetSize()); };
  RecordVal(const RecordVal & r)
    : MetaivVal(mt_record), value(r.value), recinfop(r.recinfop) {};
  ~RecordVal() { this->value.clear(); };

  void WriteVal(ostream &) const;
  void ostream_out(wostream &, const VDMFormatter &) const;
  int Compare(const MetaivVal &) const;
  bool IsSimple() const;
  MetaivVal * Copy() const { return new RecordVal(*this); };

  Sequence GetFields() const;
  void SetFields(const Sequence & s);
  bool IsEmpty() const { return (this->recinfop->IsEmpty()); };
  bool OutOfRange(int i) const { return this->recinfop->OutOfRange(i); };

  void SetField(int i, const Common & gdom);
  const Common & GetField(int i) const;
  Common & refField(int i);
};

//
// SetVal
//
class SetValIterator;
class SetVal : public MetaivVal
{
public:
  typedef std::set<Common, less<Common> > SetValueType;

private:
  friend class Set;
  friend class MapVal;
  friend class SequenceVal;
  SetValueType value;

public:
  SetVal() : MetaivVal(mt_set) {};
  SetVal(const SetVal & s) : MetaivVal(mt_set), value(s.value) {};
  ~SetVal() { this->value.clear(); };
  static MetaivVal * mk_setval(int num, ...);

  int Compare(const MetaivVal &) const;
  bool IsSimple() const;
  void WriteVal(ostream &) const;
  void ostream_out(wostream &, const VDMFormatter &) const;
  MetaivVal * Copy() const { return (value.empty() ? new SetVal() : new SetVal(*this)); };
  size_t MaxSize() const { return (size_t)this->value.max_size(); };

  void Clear() { this->value.clear(); } ;
  SetVal & Insert(const Common & g)
  { if (this->value.find(g) == this->value.end()) this->value.insert(g); return *this; };
  IntVal::IntValueType Card() const { return static_cast<int64_t>(this->value.size()); };
  bool IsEmpty() const { return (this->value.empty()); };
  bool InSet(const Common & g) const
  { return (this->value.find(g) != this->value.end()); };
  Set Power() const;

  SetVal & ImpUnion(const SetVal & s);
  SetVal & ImpIntersect(const SetVal & s);
  SetVal & ImpDiff(const SetVal & s);
  Set Union(const SetVal & s) const;
  Set Intersect(const SetVal & s) const;
  Set Diff(const SetVal & s) const;
  const Common & GetElem(bool reverse) const;
  SetVal & RemElem(const Common & g) ;
  bool SubSet(const SetVal & s) const;
  bool First(Common & g, SetValueType::const_iterator & setIter) const;
  bool Next(Common & g, SetValueType::const_iterator & setIter) const ;
  Sequence ToSequence(bool reverse) const;
  Set DirectProduct(const SetVal & s) const;
  Set DistrDirectProduct() const;
  Set DUnion() const;
  Set DInter() const;
};

class SetValIterator
{
public:
  SetVal::SetValueType::const_iterator traverseIter;
};

//
// MapVal
//
class MapVal : public MetaivVal
{
public:
  typedef std::map<Common, Common, less<Common> > MapValueType;

private:
  friend class Map;
  friend class MapValIterator;
  MapValueType value;

public:
  MapVal() : MetaivVal(mt_map) {};
  MapVal(const MapVal & m) : MetaivVal(mt_map), value(m.value) {};
  ~MapVal() { this->value.clear(); };

  int Compare(const MetaivVal & m) const;
  bool IsSimple() const;
  void WriteVal(ostream & ostr) const;
  void ostream_out(wostream & os, const VDMFormatter & vf) const;
  MetaivVal * Copy() const { return (value.empty() ? new MapVal() : new MapVal(*this)); };
  size_t MaxSize() const { return (size_t)this->value.max_size(); };

  void Clear() { this->value.clear(); };
  MapVal & Insert(const Common & gdom, const Common & gran);
  MapVal & ImpModify(const Common & gdom, const Common & gran);
  const Common & operator[](const Common & gdom) const;
  MapVal & ImpOverride(const MapVal & m);
  Map Override(const MapVal & m) const;
  int Size() const { return this->value.size(); };
  Set Dom() const;
  Set Rng() const;
  bool DomExists(const Common & c) const;
  bool DomExists(const Common & c, Generic & res) const;
  bool DomExists(const Common & c, Common ** res);
  Common & GetRngRef(const Common & c);
  bool IsEmpty() const { return this->value.empty(); };
  MapVal & RemElem(const Common & gdom);
//  bool First(Common & gdom, Common & gran,
//             MapValueType::const_iterator * traverseIter) const;
//  bool Next(Common & gdom, Common & gran,
//            MapValueType::const_iterator * traverseIter) const;
  bool First(Common & gdom, Common & gran, MapValIterator & iterator) const;
  bool Next(Common & gdom, Common & gran, MapValIterator & iterator) const;
  Map Inverse() const;
  bool IsCompatible(const MapVal &) const;
  Map DomRestrictedTo(const SetVal &) const;
  Map DomRestrictedBy(const SetVal &) const;
  Map RngRestrictedTo(const SetVal &) const;
  Map RngRestrictedBy(const SetVal &) const;

  void GC();
  void GC2(const Common & c);
};

class MapValIterator
{
public:
//  MapVal::MapValueType::const_iterator traverseIter;
//
//  MapValIterator(MapVal * m) : traverseIter(m->value.begin()) {};

  MapVal::MapValueType::const_iterator * traverseIterP;
  MapValIterator() { this->traverseIterP = NULL; };
  ~MapValIterator()
  {
    if (this->traverseIterP != NULL) delete this->traverseIterP;
  }
};

//
// TextVal
//
class TextVal : public MetaivVal
{
public:
  typedef std::wstring TextValueType;

private:
  const TextValueType value;

public:
  TextVal() : MetaivVal(mt_text) {};
  TextVal(const TextValueType & s) : MetaivVal(mt_text), value(s) {};

  TextValueType GetValue() const { return this->value; };

  void ostream_out(wostream & os, const VDMFormatter &) const
  { os << L"\"" << this->value << L"\""; };
  void WriteVal (ostream & ostr) const;

  int Compare(const MetaivVal & mval) const
  {
    TextValueType t (static_cast<const TextVal &>(mval).GetValue());
    return ((this->value == t) ? 0 : ((this->value > t) ? 1 : -1));
  }
  bool IsSimple() const { return true; };
  MetaivVal * Copy() const { return new TextVal(this->value); };
};

//
// TokenVal
//
class TokenVal : public MetaivVal
{
public:
//  typedef std::wstring TokenValueType;
  typedef Common TokenValueType;

private:
  const TokenValueType value;

private:
  bool IsString() const
  {
    if (this->value.IsSequence())
      return ((const Sequence &)(this->value)).IsString();
    else 
      return false;
  }

public:
  std::wstring GetString() const
  {
    return (this->value.IsSequence() ? ((const Sequence &)(this->value)).GetString() : L"not_string");
  }

  TokenVal() : MetaivVal(mt_token), value(Sequence()) {};
  TokenVal(const std::wstring & s) : MetaivVal(mt_token), value(Sequence(s)) {}
  TokenVal(const Common & c) : MetaivVal(mt_token), value(c) {}

  const TokenValueType & GetValue() const { return this->value; };

  void ostream_out(wostream & os, const VDMFormatter & vf) const
  {
    os << L"mk_token( " << this->value << L" )";
  };
  void WriteVal (ostream & ostr) const;

  int Compare(const MetaivVal & mval) const
  {
    TokenValueType t (static_cast<const TokenVal &>(mval).GetValue());
    return this->value.Compare(t);
  }
  bool IsSimple() const { return true; };
  MetaivVal * Copy() const { return new TokenVal(this->value); };
};

//
// BoolVal
//
class BoolVal : public MetaivVal
{
public:
  typedef bool BoolValueType;

private:
  const BoolValueType value;

public:
  BoolVal() : MetaivVal(mt_bool), value(false) {};
  BoolVal(const BoolVal & b) : MetaivVal(mt_bool), value(b.GetValue()) {};
  BoolVal(BoolValueType b) : MetaivVal(mt_bool), value(b) {};

  BoolValueType GetValue() const { return this->value; };

  bool And(bool b) const { return (this->value && b); };
  bool Or(bool b) const { return (this->value || b); };
  bool Not() const { return !(this->value); };

  void ostream_out(wostream & os, const VDMFormatter & /*vf*/) const
  { os << (this->value ? L"true" : L"false"); };
  void WriteVal (ostream& ostr) const
  { ostr << 'h' << (int)this->value << ","; };
  int Compare(const MetaivVal & mval) const
  {
    BoolValueType b = (static_cast<const BoolVal &>(mval).GetValue());
    return ((this->value == b) ? 0 : (this->value ? 1 : -1));
  }
  bool IsSimple() const { return true; };
  MetaivVal * Copy() const { return new BoolVal(this->value); };
};

#define USE_DEQUE
//
// SequenceVal
//
class SequenceVal : public MetaivVal
{
public:
#ifdef USE_DEQUE
  typedef std::deque<Common> SequenceValueType;
#else
  typedef std::vector<Common> SequenceValueType;
#endif // USE_DEQUE

private:
  friend class Sequence;
  friend class RecordVal;
  friend class SetVal;
  friend class TupleVal;

private:
  SequenceValueType value;
  bool isString;

public:
  SequenceVal() : MetaivVal(mt_sequence), isString(false) {};
  SequenceVal(const std::wstring & s);
  SequenceVal(const SequenceVal & s)
    : MetaivVal(mt_sequence), value(s.value), isString(s.isString) {};
  ~SequenceVal() { this->value.clear(); };

  static MetaivVal * mk_sequenceval(int num, ...);

  SequenceVal & ImpAppend( const Common & g)
  { this->value.push_back(g); this->isString &= g.IsChar(); return *this; };
  SequenceVal & ImpPrepend( const Common & g)
#ifdef USE_DEQUE
  { this->value.push_front(g); this->isString &= g.IsChar(); return *this; };
#else
  { this->value.insert(this->value.begin(), g); this->isString &= g.IsChar(); return *this; };
#endif // USE_DEQUE

  int Compare(const MetaivVal & s) const;

  const Common & Index(int64_t i) const { return this->operator[](i); };
  Common & refIndex(int64_t i);
  const Common & operator[](int64_t j) const;
  const Common & Hd() const;
  Sequence Tl() const;
  SequenceVal & ImpTl();
  const Common & Last() const;
  
  void Clear() { this->value.clear(); };
  int64_t Length() const { return static_cast<int64_t>(this->value.size()); };
  bool IsEmpty() const { return this->value.empty(); };
  bool GetString(std::wstring & str) const;
  std::wstring GetString() const;
  bool IsString() const;
  SequenceVal & ImpConc(const SequenceVal & v2);
  Sequence Conc(const SequenceVal & v2) const;
  SequenceVal & RemElem(int64_t i);
  SequenceVal & ImpModify(int64_t j, const Common & g);
  Set Elems() const;
  Set Inds() const;
  Sequence Reverse() const;
  Sequence SubSequence(int64_t from, int64_t to) const;
  bool StartsWith(const SequenceVal & l) const;
  bool EndsWith(const SequenceVal & l) const;
  Sequence Product(const SetVal & s) const;
  Set Permute() const;
  Sequence DistrSetProduct() const;
  bool OutOfRange(int64_t i) const { return ((i < 1 ) || (i > static_cast<int64_t>(value.size()))); };
  int64_t Find(const Common & c) const;
  bool First(Common & g, SequenceValueType::const_iterator & traverseIter) const;
  bool Next(Common & g, SequenceValueType::const_iterator & traverseIter) const;

//  Generic Pop();
  Generic ImpPop();
  Sequence ImpPop(int64_t n);
  SequenceVal & ImpCopy();
  SequenceVal & ImpSwap();
  void WriteVal(ostream &) const;
  void ostream_out(wostream &, const VDMFormatter &) const;
  bool IsSimple() const;

  MetaivVal * Copy() const { return new SequenceVal(*this); };
  size_t MaxSize() const { return (size_t)this->value.max_size(); };
};

class SequenceValIterator
{
public:
  SequenceVal::SequenceValueType::const_iterator traverseIter;
};

//
// GenericVal
//
class GenericVal : public MetaivVal
{
public:
  GenericVal() : MetaivVal(mt_generic) {};

  void ostream_out(wostream & os, const VDMFormatter & /*vf*/) const
  { os << L"<<UNDEFINED>>"; };
  void WriteVal (ostream& ostr) const
  { M4LibError::ReportError(ML_WRITEVAL_NOT_SUPPORTED, wstring(L"WriteVal not supported for Generic")); };
  int Compare(const MetaivVal &) const { return 0; };
  MetaivVal * Copy() const { return new GenericVal(); };
  bool IsSimple() const { return false; };
};

//
// DLObjectVal
//
class DLObjectVal : public MetaivVal
{
private:
  std::wstring name;
  DlClass * dlclass;

public:
  DLObjectVal(const std::wstring & name_, DlClass *p)
    : MetaivVal(mt_dlobject), name(name_), dlclass(p) {}
  std::wstring GetName() const { return this->name; };
  DlClass * GetDLClass() const { return this->dlclass; };

  void ostream_out(wostream & os, const VDMFormatter & /*vf*/) const
  { os << L"<<DLOBJECT=" << this->name << L"," << (void*)this->dlclass << L">>"; };
  void WriteVal (ostream& ostr) const
  { M4LibError::ReportError(ML_WRITEVAL_NOT_SUPPORTED, wstring(L"WriteVal not supported for DLObject")); };
  int Compare(const MetaivVal & mval) const
  {
    DlClass * v = static_cast<const DLObjectVal &>(mval).GetDLClass();
    return (((long)this->dlclass == (long)v) ? 0 : (((long)this->dlclass > (long)v) ? 1 : -1));
  }
  MetaivVal * Copy() const { return new DLObjectVal(name, dlclass); };
  bool IsSimple() const { return true; };
};

//
// UndefVal
//
class UndefVal : public MetaivVal
{
  UndefVal() : MetaivVal(mt_undef) {};

  void ostream_out(wostream & os, const VDMFormatter & /*vf*/) const
  { os << L"<<UNDEFINED>>"; };
  void WriteVal (ostream& ostr) const
  { M4LibError::ReportError(ML_WRITEVAL_NOT_SUPPORTED, wstring(L"WriteVal not supported for UndefVal")); };
  int Compare(const MetaivVal &) const { return 0; };
  MetaivVal * Copy() const { return new UndefVal(); };
  bool IsSimple() const { return true; };
};

//
// DRPPtrVal
//
class DRPtrVal : public MetaivVal
{
private:
  MetaivVal * ptr;

public:
  DRPtrVal() : MetaivVal(mt_drptr), ptr(NULL) {};
  DRPtrVal(MetaivVal * p) : MetaivVal(mt_drptr), ptr(p) {};
  ~DRPtrVal() {};

  MetaivVal * GetValue() const { return this->ptr; };

  void ostream_out(wostream&os, const VDMFormatter& /*vf*/) const
  { os << L"<<POINTER>>"; };
  void WriteVal (ostream& ostr) const
  { M4LibError::ReportError(ML_WRITEVAL_NOT_SUPPORTED, wstring(L"WriteVal not supported for Pointer")); };
  int Compare(const MetaivVal &) const { return 0; };
  MetaivVal * Copy() const { return new DRPtrVal(this->ptr); };
  bool IsSimple() const { return true; };
};

//
// TimeVal
//
class TimeVal : public MetaivVal
{
public:
  typedef time_t TimeValueType;

private:
  TimeValueType value;

public:
  TimeVal() : MetaivVal(mt_time) { time(&value); };
  TimeVal(TimeValueType t) : MetaivVal(mt_time), value(t) {};
  ~TimeVal() {};

  TimeValueType GetValue() const { return this->value; };
  wstring GetString(const string & format) const;

  void ostream_out(wostream&os, const VDMFormatter& /*vf*/) const;
  void WriteVal (ostream& ostr) const;
  int Compare(const MetaivVal &) const;
  MetaivVal * Copy() const { return new TimeVal(this->value); };
  bool IsSimple() const { return true; };
};

class FunctionVal : public MetaivVal
{
public:
  typedef Sequence (*vdm_functionval_ptr)(const Sequence & args_l);
  typedef std::wstring FunctionNameType;

private:
  vdm_functionval_ptr value;
  FunctionNameType fnm;

public:
  FunctionVal() : MetaivVal(mt_function), value(NULL), fnm(L"") {};
  FunctionVal(const FunctionNameType & nm, vdm_functionval_ptr p) : MetaivVal(mt_function), value(p), fnm(nm) {};

  void SetFunction(const FunctionNameType & nm, vdm_functionval_ptr p) { this->fnm = nm; this->value = p; };
  vdm_functionval_ptr GetFunction() const { return this->value; };
  FunctionNameType GetName() const { return this->fnm; };

  Sequence Apply (const Sequence & args) const
  { if (this->value == NULL) {
      M4LibError::ReportError(ML_NULL_POINTER, wstring(L"Function pointer is NULL"));
    }
    return (*this->value)(args);
  };

  void ostream_out(wostream&os, const VDMFormatter& /*vf*/) const
  { os << L"<<FUNCTION:" << fnm << L">>"; };
  void WriteVal (ostream& ostr) const
  { M4LibError::ReportError(ML_WRITEVAL_NOT_SUPPORTED, wstring(L"WriteVal not supported for Function")); };
  int Compare(const MetaivVal & mval) const
  {
    vdm_functionval_ptr fp ((static_cast<const FunctionVal &>(mval)).GetFunction());
    FunctionNameType fname ((static_cast<const FunctionVal &>(mval)).GetName());
    return ((this->value == fp) ? ((this->fnm == fname) ? 0 : ((this->fnm > fname) ? 1 : -1))
                                : ((this->value > fp) ? 1 : -1));
  }
  MetaivVal * Copy() const { return new FunctionVal(this->fnm, this->value); };
  bool IsSimple() const { return true; };
};

#endif // __vdmlib_values_h__
