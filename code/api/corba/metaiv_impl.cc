/***
*  * WHAT
*  *    Implementation of the CORBA API defined in metaiv_idl.idl
*  *
*  * FILE
*  *    $Source: /home/vdmtools/cvsroot/toolbox/code/api/corba/metaiv_impl.cc,v $
*  * VERSION
*  *    $Revision: 1.14 $
*  * DATE
*  *    $Date: 2006/06/14 06:15:53 $
*  * STATUS
*  *    Under development
*  * REFERENCES
*  *
*  * PROJECT
*  *    INFORMA and Toolbox
*  * AUTHOR
*  *    Ole Storm + $Author: vdmtools $
*  * COPYRIGHT
*  *    (C) Kyushu University
***/

// NOTE: <windows.h> included in omnithread.h
#include "init_corba.h"
#include "metaiv_impl.h"
#include <sstream>
#include "tb_wstring.h"
#include "corba_api.h" // For ThrowAPIError
#include "m4lib_errmsg.h"

//ObjectMap obj_map(13);
//ObjectMap Generic_i::obj_map(13);

Generic_i::~Generic_i()
{
#ifdef _PRINT_DESTRUCTION
  cout << "Destroying object Generic_i\n";
#endif
}

void Generic_i::Destroy()
{
#ifndef DYN_CAST
  if(obj_map.Delete((VDM::VDMGeneric_ptr) this))
    CORBA::BOA::getBOA()->dispose((CORBA::Object*) this);
  else
    ThrowAPIError("Unable to locate object in the object table. Cannot destroy it.",
                  "Generic_i::Destroy");
#else
  //  CORBA::BOA::getBOA()->dispose(this);
#endif
}


VDM::bytes * Generic_i::ConvertToBytes(::Generic g)
  // Uses WriteVal to convert a Generic into a sequence of bytes.
{
  VDM::bytes *bts;
  ostringstream os;
  g.WriteVal(os);

  CORBA::Octet * buf = VDM::bytes::allocbuf(os.str().length());
  memcpy(buf, os.str().c_str(), os.str().length() * sizeof(CORBA::Octet));
  //  os.rdbuf()->freeze(0);
  bts = new VDM::bytes(os.str().length(), os.str().length(), buf, true);
  /*
  CORBA::Octet * buf = VDM::bytes::allocbuf(os.pcount());
  memcpy(buf, os.str(), os.pcount() * sizeof(CORBA::Octet));
  os.rdbuf()->freeze(0);
  bts = new VDM::bytes(os.pcount(), os.pcount(), buf, true);
  */
  return bts;
}

Numeric_i::~Numeric_i()
{
#ifdef _PRINT_DESTRUCTION
  cout << "Destroying object Numeric_i\n";
#endif // _PRINT_DESTRUCTION
}

CORBA::Double Numeric_i::GetValue()
{
  ::Real r(*_g);  // Possible int's are casted to real's...
  return r.GetValue();
}

Bool_i::~Bool_i()
{
#ifdef _PRINT_DESTRUCTION
  cout << "Destroying object Bool_i\n";
#endif // _PRINT_DESTRUCTION
}

CORBA::Boolean Bool_i::GetValue()
{
  ::Bool b(*_g);
  return b.GetValue();
}

Nil_i::~Nil_i()
{
#ifdef _PRINT_DESTRUCTION
  cout << "Destroying object Nil_i\n";
#endif // _PRINT_DESTRUCTION
}

Nil_i::Nil_i(VDM::ClientID id)
{
//  ::Nil* n = new ::Nil();  // 20060614
  _g = new Generic(Nil());
  _owner = id;
}

Quote_i::~Quote_i()
{
#ifdef _PRINT_DESTRUCTION
  cout << "Destroying object Quote_i\n";
#endif // _PRINT_DESTRUCTION
}

char * Quote_i::GetValue (  )
{
  ::Quote q(*_g);
  return CORBA::string_dup(TBWSTR::wstring2string(q.GetValue()).c_str());
}

Char_i::~Char_i()
{
#ifdef _PRINT_DESTRUCTION
  cout << "Destroying object Char_i\n";
#endif // _PRINT_DESTRUCTION
}

CORBA::Char Char_i::GetValue ()
{
  ::Char c(*_g);
  return c.GetValue();
}

Text_i::~Text_i()
{
#ifdef _PRINT_DESTRUCTION
  cout << "Destroying object Text_i\n";
#endif // _PRINT_DESTRUCTION
}

char * Text_i::GetValue ()
{
  ::Text t(*_g);
  return CORBA::string_dup(TBWSTR::wstring2string(t.GetValue()).c_str());
}

Token_i::~Token_i()
{
#ifdef _PRINT_DESTRUCTION
  cout << "Destroying object Token_i\n";
#endif // _PRINT_DESTRUCTION
}

char * Token_i::GetValue ()
{
  ::Token t(*_g);
  return CORBA::string_dup(TBWSTR::wstring2string(t.GetString()).c_str()); // 20100616
}

Set_i::~Set_i()
{
#ifdef _PRINT_DESTRUCTION
  cout << "Destroying object Set_i\n";
#endif // _PRINT_DESTRUCTION
}

Record_i::~Record_i()
{
#ifdef _PRINT_DESTRUCTION
  cout << "Destroying object Record_i\n";
#endif // _PRINT_DESTRUCTION
}

Map_i::~Map_i()
{
#ifdef _PRINT_DESTRUCTION
  cout << "Destroying object Map_i\n";
#endif // _PRINT_DESTRUCTION
}

Tuple_i::~Tuple_i()
{
#ifdef _PRINT_DESTRUCTION
  cout << "Destroying object Tuple_i\n";
#endif // _PRINT_DESTRUCTION
}

Sequence_i::~Sequence_i()
{
#ifdef _PRINT_DESTRUCTION
  cout << "Destroying object Sequence_i\n";
#endif // _PRINT_DESTRUCTION
}

//
// Implementation of operations on Map:
//

void Map_i::Insert ( VDM::VDMGeneric_ptr key, VDM::VDMGeneric_ptr val )
{
  Generic_i *key_p = dyn_cast(Generic_i, key);
  Generic_i *val_p = dyn_cast(Generic_i, val);
  if(!(key_p && val_p))
    ThrowApiError(L"Unable to cast from Generic_ptr to Generic_i",
                  L"Map_i::Insert");
  ::Generic k = key_p->GetMetaivValue(),
    v = val_p->GetMetaivValue();
  if( _m.DomExists(k) && v == _m[k] )
    throw VDM::VDMError(ML_CONFLICTING_RNGVAL);
  _m.Insert(k,v);
}

void Map_i::ImpModify ( VDM::VDMGeneric_ptr key, VDM::VDMGeneric_ptr val )
{
  Generic_i *key_p = dyn_cast(Generic_i, key);
  Generic_i *val_p = dyn_cast(Generic_i, val);
  if(!(key_p && val_p))
    ThrowApiError(L"Unable to cast from Generic_ptr to Generic_i",
                  L"Map_i::ImpModify");
  _m.ImpModify(key_p->GetMetaivValue(), val_p->GetMetaivValue());
}

VDM::VDMGeneric_ptr Map_i::Apply ( VDM::VDMGeneric_ptr  key )
{
  Generic_i *key_p = dyn_cast(Generic_i, key);
  if(!key_p)
    ThrowApiError(L"Unable to cast from Generic_ptr to Generic_i",
                  L"Map_i::Apply");

  ::Generic k = key_p->GetMetaivValue();
  if( !_m.DomExists(k) )
    throw VDM::VDMError(ML_NOT_IN_DOM);
  return metaiv_to_idl(_owner, _m[k]);
}

void Map_i::ImpOverride ( VDM::VDMMap_ptr m )
{
  Map_i *p = dyn_cast(Map_i, m);
  if(p)
    _m.ImpOverride(p->GetMetaivValue());
  else
    ThrowApiError(L"Unable to cast from Map_ptr to Map_i",
                  L"Map_i::ImpOverride");
}

VDM::VDMSet_ptr Map_i::Dom (  )
{
  VDM::VDMSet_ptr p = VDM::VDMSet::_narrow(metaiv_to_idl(_owner, _m.Dom()));
  CORBA::release(p); // necessary because _narrow() increments the
                     // reference count
  return p;
}

VDM::VDMSet_ptr Map_i::Rng (  )
{
  VDM::VDMSet_ptr p = VDM::VDMSet::_narrow(metaiv_to_idl(_owner, _m.Rng()));
  CORBA::release(p); // necessary because _narrow() increments the
                     // reference count
  return p;
}

CORBA::Boolean Map_i::DomExists ( VDM::VDMGeneric_ptr g )
{
  Generic_i *p = dyn_cast(Generic_i, g);
  if(!p)
    ThrowApiError(L"Unable to cast from Generic_ptr to Generic_i",
                  L"Map_i::DomExists");
  return _m.DomExists(p->GetMetaivValue());
}

void Map_i::RemElem ( VDM::VDMGeneric_ptr g )
{
  Generic_i *p = dyn_cast(Generic_i, g);
  if(!p)
    ThrowApiError(L"Unable to cast from Generic_ptr to Generic_i",
                  L"Map_i::RemElem");
  ::Generic k = p->GetMetaivValue();
  if(!_m.DomExists(k))
    throw VDM::VDMError(ML_NOT_IN_DOM);
  _m.RemElem(k);
}

CORBA::Boolean Map_i::First(VDM::VDMGeneric_out g)
{
  bool rtval;
  if((rtval = _m.First(_iter)))
    g = metaiv_to_idl(_owner, _iter);
  return rtval;
}

CORBA::Boolean Map_i::Next(VDM::VDMGeneric_out g)
{
  bool rtval;
  if((rtval = _m.Next(_iter)))
    g = metaiv_to_idl(_owner, _iter);
  return rtval;
}

//
// Implementation of operations on Sequence:
//

CORBA::Long Sequence_i::Length()
{
  return _s.Length();
}

VDM::VDMGeneric_ptr Sequence_i::Index(CORBA::Long i)
{
  if(i<1 || i>(int)_s.Length())
    throw VDM::VDMError(ML_INDEX_OUT_OF_RANGE);
  return metaiv_to_idl(_owner, _s.Index(i));
}

VDM::VDMGeneric_ptr Sequence_i::Hd()
{
  if(_s.IsEmpty())
    throw VDM::VDMError(ML_HD_ON_EMPTY_SEQUENCE);
  return metaiv_to_idl(_owner, _s.Hd());
}

VDM::VDMSequence_ptr Sequence_i::Tl()
{
  if(_s.IsEmpty())
    throw VDM::VDMError(ML_TL_ON_EMPTY_SEQUENCE);
  VDM::VDMSequence_ptr p = VDM::VDMSequence::_narrow(metaiv_to_idl(_owner, _s.Tl()));
  CORBA::release(p); // necessary because _narrow() increments the
                     // reference count
  return p;
}

void Sequence_i::ImpTl()
{
  if(_s.IsEmpty())
    throw VDM::VDMError(ML_TL_ON_EMPTY_SEQUENCE);
  _s.ImpTl();
}

void Sequence_i::RemElem(CORBA::Long i)
{
  if(i<1 || i>(int)_s.Length())
    throw VDM::VDMError(ML_INDEX_OUT_OF_RANGE);
  _s.RemElem(i);
}

VDM::VDMSet_ptr Sequence_i::Elems ()
{
  VDM::VDMSet_ptr p = VDM::VDMSet::_narrow(metaiv_to_idl(_owner, _s.Elems()));
  CORBA::release(p); // necessary because _narrow() increments the
                     // reference count
  return p;
}

CORBA::Boolean Sequence_i::GetString(CORBA::String_out s)
{
  wstring str;
  int rtval = _s.GetString(str);
  s = CORBA::string_dup(TBWSTR::wstring2string(str).c_str());
  return rtval;
}

void Sequence_i::ImpAppend(VDM::VDMGeneric_ptr g)
{
  Generic_i *p = dyn_cast(Generic_i, g);
  if(p)
    _s.ImpAppend(p->GetMetaivValue());
  else
    ThrowApiError(L"Unable to cast from Generic_ptr to Generic_i",
                  L"Sequence_i::ImpAppend");

}

void Sequence_i::ImpModify(CORBA::Long i, VDM::VDMGeneric_ptr g)
{
  if(i < 1 || i > (int)_s.Length())
    throw VDM::VDMError(ML_INDEX_OUT_OF_RANGE);
  else {
    Generic_i *p = dyn_cast(Generic_i, g);
    if(p)
      _s.ImpModify(i, p->GetMetaivValue());
    else
      ThrowApiError(L"Unable to cast from Generic_ptr to Generic_i",
                    L"Sequence_i::ImpModify");
  }
}

void Sequence_i::ImpPrepend(VDM::VDMGeneric_ptr g)
{
  Generic_i *p = dyn_cast(Generic_i, g);
  if(p)
    _s.ImpPrepend(p->GetMetaivValue());
  else
    ThrowApiError(L"Unable to cast from Generic_ptr to Generic_i",
                  L"Sequence_i::ImpPrepend");
}

void Sequence_i::ImpConc(VDM::VDMSequence_ptr s)
{
  Generic_i *p = dyn_cast(Sequence_i, s);
  if(p)
    _s.ImpConc(p->GetMetaivValue());
  else
    ThrowApiError(L"Unable to cast from Sequence_ptr to Sequence_i",
                  L"Sequence_i::ImpConc");
}

CORBA::Boolean Sequence_i::First(VDM::VDMGeneric_out g)
{
  bool rtval;
  if((rtval = _s.First(_iter)))
    g = metaiv_to_idl(_owner, _iter);
  return rtval;
}

CORBA::Boolean Sequence_i::Next(VDM::VDMGeneric_out g)
{
  bool rtval;
  if((rtval = _s.Next(_iter)))
    g = metaiv_to_idl(_owner, _iter);
  return rtval;
}

//VDM::bytes *
//Sequence_i::GetCPPValue()
//{
//  bytes *bts;
//  ostrstream os;
//  _s.WriteVal(os);
//  CORBA::Octet * buf = bytes::allocbuf(os.pcount());
//  memcpy(buf, os.str(), os.pcount() * sizeof(CORBA::Octet));
//  os.rdbuf()->freeze(0);
//  bts = new bytes(os.pcount(), os.pcount(), buf, true);
//  return bts;
//}

//
// Implementation of operations on Set:
//

void Set_i::Insert ( VDM::VDMGeneric_ptr g )
{
  Generic_i *p = dyn_cast(Generic_i, g);
  if(p)
    _s.Insert(p->GetMetaivValue());
  else
    ThrowApiError(L"Unable to cast from Generic_ptr to Generic_i",
                  L"Set_i::Insert");
}

CORBA::Boolean Set_i::InSet ( VDM::VDMGeneric_ptr g )
{
  Generic_i *p = dyn_cast(Generic_i, g);
  if(!p)
    ThrowApiError(L"Unable to cast from Generic_ptr to Generic_i",
                  L"Set_i::InSet");
  return _s.InSet(p->GetMetaivValue());
}

void Set_i::ImpUnion ( VDM::VDMSet_ptr s )
{
  Set_i *p = dyn_cast(Set_i, s);
  if(!p)
    ThrowApiError(L"Unable to cast from Set_ptr to Set_i",
                  L"Set_i::ImpUnion");
  _s.ImpUnion(p->GetMetaivValue());
}

void Set_i::ImpIntersect ( VDM::VDMSet_ptr s )
{
  Set_i *p = dyn_cast(Set_i, s);
  if(!p)
    ThrowApiError(L"Unable to cast from Set_ptr to Set_i",
                  L"Set_i::ImpIntersect");
  _s.ImpIntersect(p->GetMetaivValue());
}

VDM::VDMGeneric_ptr Set_i::GetElem ()
{
  if(_s.IsEmpty())
    throw VDM::VDMError(ML_OP_ON_EMPTY_SET);
  return metaiv_to_idl(_owner, _s.GetElem());
}

void Set_i::RemElem ( VDM::VDMGeneric_ptr g )
{
  Generic_i *p = dyn_cast(Generic_i, g);
  if(!p)
    ThrowApiError(L"Unable to cast from Generic_ptr to Generic_i",
                  L"Set_i::RemElem");
  if(!_s.InSet(p->GetMetaivValue()))
    throw VDM::VDMError(ML_NOT_IN_SET);
  _s.RemElem(p->GetMetaivValue());
}

CORBA::Boolean Set_i::SubSet ( VDM::VDMSet_ptr s )
{
  Set_i *p = dyn_cast(Set_i, s);
  if(!p)
    ThrowApiError(L"Unable to cast from Set_ptr to Set_i",
                  L"Set_i::SubSet");
  return _s.SubSet(p->GetMetaivValue());
}

void Set_i::ImpDiff ( VDM::VDMSet_ptr s )
{
  Set_i *p = dyn_cast(Set_i, s);
  if(!p)
    ThrowApiError(L"Unable to cast from Set_ptr to Set_i",
                  L"Set_i::ImpDiff");
  _s.ImpDiff(p->GetMetaivValue());
}

CORBA::Boolean Set_i::First(VDM::VDMGeneric_out g)
{
  bool rtval;
  if((rtval = _s.First(_iter)))
    g = metaiv_to_idl(_owner, _iter);
  return rtval;
}

CORBA::Boolean Set_i::Next(VDM::VDMGeneric_out g)
{
  bool rtval;
  if((rtval = _s.Next(_iter)))
    g = metaiv_to_idl(_owner, _iter);
  return rtval;
}

void Record_i::SetField ( CORBA::ULong  i, VDM::VDMGeneric_ptr g )
{
  if (i < 1 || (CORBA::Long) i > (int)_r.Length())
    throw VDM::VDMError(ML_RANGE_ERROR);
  Generic_i *p = dyn_cast(Generic_i, g);
  if(!p)
    ThrowApiError(L"Unable to cast from Generic_ptr to Generic_i",
                  L"Record_i::SetField");
  _r.SetField(i, p->GetMetaivValue());
}

VDM::VDMGeneric_ptr Record_i::GetField ( CORBA::ULong i )
{
  if (i < 1 || (CORBA::Long) i > (int)_r.Length())
    throw VDM::VDMError(ML_RANGE_ERROR);
  return metaiv_to_idl(_owner, _r.GetField(i));
}

char * Record_i::GetTag ()
{
  wstring sym_tag = _r.GetTextTag();
  return CORBA::string_dup(TBWSTR::wstring2string(sym_tag).c_str());
}

CORBA::Boolean Record_i::Is(const char *sym_tag)
{
  wstring tag = _r.GetTextTag();
  return tag.compare(TBWSTR::string2wstring(sym_tag)) == 0;
}

void Tuple_i::SetField ( CORBA::ULong i, VDM::VDMGeneric_ptr g )
{
  if (i < 1 || (CORBA::Long) i > (int)_t.Length())
    throw VDM::VDMError(ML_RANGE_ERROR);

  Generic_i *p = dyn_cast(Generic_i, g);
  if(!p)
    ThrowApiError(L"Unable to cast from Generic_ptr to Generic_i",
                  L"Tuple_i::SetField");
  _t.SetField(i, p->GetMetaivValue());
}

VDM::VDMGeneric_ptr Tuple_i::GetField ( CORBA::ULong i )
{
  if (i < 1 || (CORBA::Long) i > (int)_t.Length())
    throw VDM::VDMError(ML_RANGE_ERROR);
  return metaiv_to_idl(_owner, _t.GetField(i));
}

VDM::VDMGeneric_ptr Generic_i::metaiv_to_idl(VDM::ClientID id, ::Generic g)
{
  Generic_i* g_i = NULL;
  if (g.IsInt())
    g_i = new Numeric_i(id, Int(g));
  else if (g.IsReal())
    g_i = new Numeric_i(id, Real(g));
  else if (g.IsBool())
    g_i = new Bool_i(id, Bool(g));
  else if (g.IsNil())
    g_i = new Nil_i(id);
  else if (g.IsQuote())
    g_i = new Quote_i(id, Quote(g));
  else if (g.IsChar())
    g_i = new Char_i(id, Char(g));
  else if (g.IsText())
    g_i = new Text_i(id, Text(g));
  else if (g.IsToken())
    g_i = new Token_i(id, Token(g));
  else if (g.IsMap())
    g_i = new Map_i(id, Map(g));
  else if (g.IsSequence())
    g_i = new Sequence_i(id, Sequence(g));
  else if (g.IsSet())
    g_i = new Set_i(id, Set(g));
  else if (g.IsRecord())
    g_i = new Record_i(id, Record(g));
  else if (g.IsTuple())
    g_i = new Tuple_i(id, Tuple(g));
  else
  {
    cout << "APIError; Unknown type passed to metaiv_to_idl" << endl;
    ThrowApiError(L"Unknown type passed to metaiv_to_idl", L"metaiv_to_idl");
  }
  //  g_i->_obj_is_ready(CORBA::BOA::getBOA());

  PortableServer::ObjectId_var v = VDMCORBA::GetPOAvar()->activate_object(g_i);

  // FIXME: What happens to ownership of g_i's memory here?
  VDM::VDMGeneric_var g_ptr = g_i->_this();
#ifndef DYN_CAST
  obj_map.Insert(VDM::VDMGeneric::_duplicate(g_ptr), g_i, id);
  obj_map.Echo();
#endif
  return g_ptr._retn();
}

VDMFactory_i::~VDMFactory_i()
{
#ifdef _PRINT_DESTRUCTION
  cout << "Destroying object Factory_i" << endl;
#endif // _PRINT_DESTRUCTION
}

//VDM::Int_ptr
//VDMFactory_i::mkInt ( CORBA::Long  i )
//{
//  VDM::Int_ptr p = VDM::Int::_narrow(metaiv_to_idl(Int(i)));
//  CORBA::release(p); // necessary because _narrow() increments the
//                     // reference count
//  return p;
//}
//
//
//VDM::Real_ptr
//VDMFactory_i::MkReal ( CORBA::Double  r )
//{
//  VDM::Real_ptr p = VDM::Real::_narrow(metaiv_to_idl(Real(r)));
//  CORBA::release(p); // necessary because _narrow() increments the
//                     // reference count
//  return p;
//}

VDM::VDMNumeric_ptr VDMFactory_i::MkNumeric ( VDM::ClientID id, CORBA::Double  r )
{
  VDM::VDMNumeric_ptr p = VDM::VDMNumeric::_narrow(Generic_i::metaiv_to_idl (id, Real (r)));
  //  CORBA::release(p); // necessary because _narrow() increments the
                     // reference count
  return p;
}

VDM::VDMSequence_ptr VDMFactory_i::MkSequence (VDM::ClientID id)
{
  VDM::VDMSequence_ptr p = VDM::VDMSequence::_narrow(Generic_i::metaiv_to_idl(id, Sequence()));
  //  CORBA::release(p); // necessary because _narrow() increments the
                     // reference count
  return p;
}

VDM::VDMBool_ptr VDMFactory_i::MkBool ( VDM::ClientID id, CORBA::Boolean b )
{
  VDM::VDMBool_ptr p = VDM::VDMBool::_narrow(Generic_i::metaiv_to_idl(id, Bool(b)));
  CORBA::release(p); // necessary because _narrow() increments the
                     // reference count
  return p;
}

VDM::VDMNil_ptr VDMFactory_i::MkNil (VDM::ClientID id)
{
  VDM::VDMNil_ptr p = VDM::VDMNil::_narrow(Generic_i::metaiv_to_idl(id, Nil()));
  CORBA::release(p); // necessary because _narrow() increments the
                     // reference count
  return p;
}

VDM::VDMQuote_ptr VDMFactory_i::MkQuote ( VDM::ClientID id, const char *c )
{
  VDM::VDMQuote_ptr p = VDM::VDMQuote::_narrow(Generic_i::metaiv_to_idl(id, Quote(TBWSTR::string2wstring(string(c)))));
  //  CORBA::release(p); // necessary because _narrow() increments the
                     // reference count
  return p;
}

VDM::VDMText_ptr VDMFactory_i::MkText ( VDM::ClientID id, const char *c )
{
  VDM::VDMText_ptr p = VDM::VDMText::_narrow(Generic_i::metaiv_to_idl(id, Text(TBWSTR::string2wstring(string(c)))));
  //  CORBA::release(p); // necessary because _narrow() increments the
                     // reference count
  return p;
}

VDM::VDMToken_ptr VDMFactory_i::MkToken ( VDM::ClientID id, const char *c )
{
  VDM::VDMToken_ptr p = VDM::VDMToken::_narrow(Generic_i::metaiv_to_idl(id, Token(TBWSTR::string2wstring(string(c)))));
  CORBA::release(p); // necessary because _narrow() increments the
                     // reference count
  return p;
}

VDM::VDMChar_ptr VDMFactory_i::MkChar ( VDM::ClientID id, CORBA::Char c )
{
  VDM::VDMChar_ptr p = VDM::VDMChar::_narrow(Generic_i::metaiv_to_idl(id, Char(c)));
  CORBA::release(p); // necessary because _narrow() increments the
                     // reference count
  return p;
}

VDM::VDMMap_ptr VDMFactory_i::MkMap (VDM::ClientID id)
{
  VDM::VDMMap_ptr p = VDM::VDMMap::_narrow(Generic_i::metaiv_to_idl(id, Map()));
  //  CORBA::release(p); // necessary because _narrow() increments the
                     // reference count
  return p;
}

VDM::VDMSet_ptr VDMFactory_i::MkSet (VDM::ClientID id)
{
  VDM::VDMSet_ptr p = VDM::VDMSet::_narrow(Generic_i::metaiv_to_idl(id, Set()));
  //  CORBA::release(p); // necessary because _narrow() increments the
                     // reference count
  return p;
}

#if 0
VDM::VDMRecord_ptr
VDMFactory_i::MkRecord ( VDM::ClientID id, const char *sym_tag, CORBA::ULong len )
{
  // First, translate the sybolic tag to an integer value;
  int tag;
  if( semrec->GetTagVal(sym_tag, tag) ){
    VDM::VDMRecord_ptr p = VDM::VDMRecord::_narrow(Generic_i::metaiv_to_idl(id, Record(tag, len)));
    CORBA::release(p); // necessary because _narrow() increments the
                       // reference count
  }
  else{
    wstring err("MkRecord(): Unknown symbolic tag: ");
    err += wstring(sym_tag) + L"\n";
    ThrowApiError(err.c_str(), "VDMFactory_i::MkRecord");
  }
  return p;
}
#endif

VDM::VDMTuple_ptr VDMFactory_i::MkTuple ( VDM::ClientID id, CORBA::ULong len )
{
  VDM::VDMTuple_ptr p = VDM::VDMTuple::_narrow(Generic_i::metaiv_to_idl(id, Tuple(len)));
  CORBA::release(p); // necessary because _narrow() increments the
                     // reference count
  return p;
}

VDM::VDMGeneric_ptr VDMFactory_i::FromCPPValue( VDM::ClientID id, const VDM::bytes &b )
  // Constructs a VDM::Generic based of the binary representation of a
  // 'real' meta-iv value in b.
{
  istringstream is((char *)b.NP_data()); //, b.length());
  Generic g = ReadVal(is);
  return Generic_i::metaiv_to_idl(id, g);
}

