/***
*  * WHAT
*  *    Implementation of the CORBA API defined in metaiv_idl.idl
*  *    
*  * FILE
*  *    $Source: /home/vdmtools/cvsroot/toolbox/code/api/corba/metaiv_impl.h,v $
*  * VERSION
*  *    $Revision: 1.13 $
*  * DATE
*  *    $Date: 2006/04/18 07:45:41 $
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

#ifndef __metaiv_impl_h__
#define __metaiv_impl_h__

#include "metaiv_idl.hh"

#include "metaiv.h"
//#include "object_map.h"
#include "tb_wstring.h"

// #define _PRINT_DESTRUCTION
// #define DYN_CAST

//extern ObjectMap obj_map;

#ifdef DYN_CAST
//#define dyn_cast(type, ptr)  dynamic_cast<type*>(poa->reference_to_servant(ptr))
#define dyn_cast(type, ptr)  dynamic_cast<type*>(VDMCORBA::GetPOAvar()->reference_to_servant(ptr))
#else
#define dyn_cast(type, ptr) (type *) obj_map.Find(ptr)
#endif

class Generic_i : public POA_VDM::VDMGeneric, public PortableServer::RefCountServantBase {
public:
  Generic_i(VDM::ClientID id, ::Generic g) :  _owner(id) { _g = new Generic(g);};
  Generic_i() {};
  virtual ~Generic_i();
  VDM::bytes * ConvertToBytes(::Generic);
  static VDM::VDMGeneric_ptr metaiv_to_idl(VDM::ClientID id, ::Generic g);
  
  virtual CORBA::Boolean IsNil() { return false; }     
  virtual CORBA::Boolean IsChar() { return false; }     
  //  virtual CORBA::Boolean IsInt() { return false; }      
  //  virtual CORBA::Boolean IsReal() { return false; }     
  virtual CORBA::Boolean IsNumeric() { return false; }     
  virtual CORBA::Boolean IsQuote() { return false; }    
  virtual CORBA::Boolean IsTuple() { return false; }    
  virtual CORBA::Boolean IsRecord() { return false; }   
  virtual CORBA::Boolean IsSet() { return false; }      
  virtual CORBA::Boolean IsMap() { return false; }      
  virtual CORBA::Boolean IsText() { return false; }     
  virtual CORBA::Boolean IsToken() { return false; }    
  virtual CORBA::Boolean IsBool() { return false; }     
  virtual CORBA::Boolean IsSequence() { return false; } 
  virtual CORBA::Boolean IsObjectRef() { return false; }
  virtual void Destroy();
  virtual VDM::bytes * GetCPPValue () { return ConvertToBytes(*_g); }

  virtual ::Generic GetMetaivValue() { return *(new Generic(*_g)); };
  virtual char* ToAscii() { return CORBA::string_dup(TBWSTR::wstring2string(_g->ascii()).c_str()); }
  virtual VDM::VDMGeneric_ptr  Clone (  ) { return metaiv_to_idl(_owner, *_g); }
  
  ::Generic* _g;

protected:
  VDM::ClientID _owner;
};


//class Int_i : public POA_VDM::VDMInt, public Generic_i {
//public:
//  ~Int_i();
//  Int_i(Int i) { _g = i; }
//  CORBA::Boolean IsInt() { return true; }
//  CORBA::Long GetValue();
//};
//
//class Real_i : public POA_VDM::VDMReal, public Generic_i {
//public:
//  ~Real_i();
//  Real_i(Real r) { _g = r; }
//  CORBA::Boolean IsReal() { return true; }
//  CORBA::Double GetValue();
//};

class Numeric_i : public POA_VDM::VDMNumeric, public Generic_i {
public:
  ~Numeric_i();
  Numeric_i(VDM::ClientID id, ::Real r) : Generic_i(id, r) {}
  Numeric_i(VDM::ClientID id, ::Int i) : Generic_i(id, i) {}
  CORBA::Boolean IsNumeric() { return true; }
  CORBA::Double GetValue();
};

class Bool_i : public POA_VDM::VDMBool, public Generic_i {
public:
  ~Bool_i();
  Bool_i(VDM::ClientID id, ::Bool b) : Generic_i(id, b) {}
  CORBA::Boolean IsBool() { return true; }
  CORBA::Boolean  GetValue();
};

class Nil_i : public POA_VDM::VDMNil, public Generic_i {
public:
  ~Nil_i();
  Nil_i(VDM::ClientID);
  CORBA::Boolean IsNil() { return true; }
};

class Quote_i : public POA_VDM::VDMQuote, public Generic_i {
public:
  ~Quote_i();
  Quote_i(VDM::ClientID id, ::Quote q) : Generic_i(id, q) {}
  CORBA::Boolean IsQuote() { return true; }
  virtual char *  GetValue (  );
};

class Text_i : public POA_VDM::VDMText, public Generic_i {
public:
  ~Text_i();
  Text_i(VDM::ClientID id, ::Text q) :  Generic_i(id, q) {}
  CORBA::Boolean IsText() { return true; }
  virtual char *  GetValue (  );
};

class Token_i : public POA_VDM::VDMToken, public Generic_i {
public:
  ~Token_i();
  Token_i(VDM::ClientID id, ::Token q) : Generic_i(id, q) {}
  CORBA::Boolean IsToken() { return true; }
  virtual char *  GetValue (  );
};

class Char_i : public POA_VDM::VDMChar, public Generic_i {
public:
  ~Char_i();
  Char_i(VDM::ClientID id, ::Char c) : Generic_i(id, c) {}
  CORBA::Boolean IsChar() { return true; }
  virtual CORBA::Char  GetValue (  );
};

// All composite types hold their own copy of the MetaIv value, and
// override the ToAscii(), GetMetaivValue(), Clone(), and
// GetCPPValue() methods.

class Sequence_i : public POA_VDM::VDMSequence, public Generic_i {
public:
  Sequence_i(VDM::ClientID id, ::Sequence s) : _s(s) { _owner = id; }
  ~Sequence_i();
  CORBA::Boolean IsSequence() { return true; }

  VDM::VDMGeneric_ptr Index(CORBA::Long i);
  VDM::VDMGeneric_ptr Hd();
  VDM::VDMSequence_ptr Tl();
  void ImpTl();
  void RemElem(CORBA::Long i);
  VDM::VDMSet_ptr  Elems (  );
  CORBA::Long Length();
  CORBA::Boolean GetString(CORBA::String_out);
  CORBA::Boolean IsEmpty() { return _s.IsEmpty(); }
  void ImpAppend(VDM::VDMGeneric_ptr g);
  void ImpModify(CORBA::Long i, VDM::VDMGeneric_ptr g);
  void ImpPrepend(VDM::VDMGeneric_ptr g);
  void ImpConc(VDM::VDMSequence_ptr s);
  CORBA::Boolean First(VDM::VDMGeneric_out);
  CORBA::Boolean Next(VDM::VDMGeneric_out);

  char* ToAscii() { return CORBA::string_dup(TBWSTR::wstring2string(_s.ascii()).c_str()); }
  VDM::bytes * GetCPPValue() { return ConvertToBytes(_s); }
  ::Generic GetMetaivValue() { return _s; }

  VDM::VDMGeneric_ptr  Clone (  ) { return metaiv_to_idl(_owner, _s); }

private:
  ::Sequence _s;
  ::Generic _iter;
};


class Map_i : public POA_VDM::VDMMap, public Generic_i {    
public:
  Map_i(VDM::ClientID id, ::Map m) : _m(m) { _owner = id; }
  ~Map_i();
  CORBA::Boolean IsMap() { return true; }

  void Insert ( VDM::VDMGeneric_ptr, VDM::VDMGeneric_ptr );
  void ImpModify ( VDM::VDMGeneric_ptr, VDM::VDMGeneric_ptr );
  VDM::VDMGeneric_ptr  Apply ( VDM::VDMGeneric_ptr );
  void ImpOverride ( VDM::VDMMap_ptr );
  CORBA::ULong  Size (  ) { return _m.Size(); }
  CORBA::Boolean  IsEmpty (  ) { return _m.IsEmpty(); }
  VDM::VDMSet_ptr  Dom (  );
  VDM::VDMSet_ptr  Rng (  );
  CORBA::Boolean  DomExists ( VDM::VDMGeneric_ptr );
  void RemElem ( VDM::VDMGeneric_ptr );
  CORBA::Boolean First(VDM::VDMGeneric_out);
  CORBA::Boolean Next(VDM::VDMGeneric_out);

  char* ToAscii() { return CORBA::string_dup(TBWSTR::wstring2string(_m.ascii()).c_str()); }
  VDM::bytes * GetCPPValue() { return ConvertToBytes(_m); }
  ::Generic GetMetaivValue() { return _m; }
  VDM::VDMGeneric_ptr  Clone (  ) { return metaiv_to_idl(_owner, _m); }

private:
  ::Map _m;
  ::Generic _iter;
};

class Set_i : public POA_VDM::VDMSet, public Generic_i {    
public:
  Set_i(VDM::ClientID id, ::Set s) : _s(s) { _owner = id; }
  ~Set_i();
  CORBA::Boolean IsSet() { return true; }

  void Insert ( VDM::VDMGeneric_ptr  g );
  CORBA::ULong  Card (  ) { return _s.Card(); }
  CORBA::Boolean  IsEmpty () { return _s.IsEmpty(); }
  CORBA::Boolean  InSet ( VDM::VDMGeneric_ptr  g );
  void ImpUnion ( VDM::VDMSet_ptr  s );
  void ImpIntersect ( VDM::VDMSet_ptr  s );
  VDM::VDMGeneric_ptr  GetElem (  );
  void RemElem ( VDM::VDMGeneric_ptr  g );
  CORBA::Boolean  SubSet ( VDM::VDMSet_ptr  s );
  void ImpDiff ( VDM::VDMSet_ptr  s );
  CORBA::Boolean First(VDM::VDMGeneric_out);
  CORBA::Boolean Next(VDM::VDMGeneric_out);
    
  char* ToAscii() { return CORBA::string_dup(TBWSTR::wstring2string(_s.ascii()).c_str()); }
  VDM::bytes * GetCPPValue() { return ConvertToBytes(_s); }
  ::Generic GetMetaivValue() { return _s; }
  VDM::VDMGeneric_ptr  Clone (  ) { return metaiv_to_idl(_owner, _s); }

private:
  ::Set _s;
  ::Generic _iter;
};

class Record_i : public POA_VDM::VDMRecord, public Generic_i {    
public:
  Record_i(VDM::ClientID id, ::Record r) : _r(r) { _owner = id; }
  ~Record_i();
  CORBA::Boolean IsRecord() { return true; }

  void SetField ( CORBA::ULong  i, VDM::VDMGeneric_ptr  g );
  VDM::VDMGeneric_ptr  GetField ( CORBA::ULong  i );
  char *  GetTag ();
  CORBA::Boolean  Is ( const char *sym_tag );
  CORBA::ULong  Length () { return _r.Length(); }

  char* ToAscii() { return CORBA::string_dup(TBWSTR::wstring2string(_r.ascii()).c_str()); }
  VDM::bytes * GetCPPValue() { return ConvertToBytes(_r); }
  ::Generic GetMetaivValue() { return _r; }
  VDM::VDMGeneric_ptr  Clone (  ) { return metaiv_to_idl(_owner, _r); }

private:
  ::Record _r;
};

class Tuple_i : public POA_VDM::VDMTuple, public Generic_i {    
public:
  Tuple_i(VDM::ClientID id, ::Tuple t) : _t(t) { _owner = id; }
  ~Tuple_i();
  CORBA::Boolean IsTuple() { return true; }

  void SetField ( CORBA::ULong  i, VDM::VDMGeneric_ptr  g );
  VDM::VDMGeneric_ptr  GetField ( CORBA::ULong  i );
  CORBA::ULong  Length () { return _t.Length(); }

  char* ToAscii() { return CORBA::string_dup(TBWSTR::wstring2string(_t.ascii()).c_str()); }
  VDM::bytes * GetCPPValue() { return ConvertToBytes(_t); }
  ::Generic GetMetaivValue() { return _t; }
  VDM::VDMGeneric_ptr  Clone (  ) { return metaiv_to_idl(_owner, _t); }

private:
  ::Tuple _t;
};

class VDMFactory_i : public POA_VDM::VDMFactory, public PortableServer::RefCountServantBase {
public:
  VDMFactory_i(){};
  ~VDMFactory_i();
//  VDM::Int_ptr MkInt ( CORBA::Long );
//  VDM::Real_ptr MkReal ( CORBA::Double );
  VDM::VDMNumeric_ptr MkNumeric ( VDM::ClientID, CORBA::Double );
  VDM::VDMBool_ptr MkBool ( VDM::ClientID, CORBA::Boolean );
  VDM::VDMNil_ptr MkNil ( VDM::ClientID );
  VDM::VDMQuote_ptr MkQuote ( VDM::ClientID, const char * );
  VDM::VDMChar_ptr MkChar ( VDM::ClientID, CORBA::Char );
  VDM::VDMText_ptr MkText ( VDM::ClientID, const char * );
  VDM::VDMToken_ptr MkToken ( VDM::ClientID, const char * );
  VDM::VDMMap_ptr MkMap ( VDM::ClientID );
  VDM::VDMSequence_ptr MkSequence ( VDM::ClientID );
  VDM::VDMSet_ptr MkSet ( VDM::ClientID );
  //  VDM::Record_ptr MkRecord ( VDM::ClientID, const char *sym_tag, CORBA::ULong );
  VDM::VDMTuple_ptr MkTuple ( VDM::ClientID, CORBA::ULong );
  VDM::VDMGeneric_ptr FromCPPValue ( VDM::ClientID, const VDM::bytes & );

};

#endif // __metaiv_impl_h__
