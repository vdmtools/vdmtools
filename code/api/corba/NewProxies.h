#ifndef _NewProxies_h
#define _NewProxies_h

#include "metaiv_idl.hh"

class _new_proxy_Generic : public virtual VDM::_proxy_Generic
{
public:
  _new_proxy_Generic (Rope *r,
                      CORBA::Octet *key,
                      size_t keysize,
                      IOP::TaggedProfileList *profiles,
                      CORBA::Boolean release) 
    : _proxy_Generic(r,key,keysize,profiles,release),
    omniObject(VDM_Generic_IntfRepoID,r,key,keysize,profiles,release) {}
  
  virtual ~_new_proxy_Generic() { destroy(); }
  
};

class _new_Generic_proxyObjectFactory : public VDM::Generic_proxyObjectFactory
{
public:
  _new_Generic_proxyObjectFactory () {}
  virtual ~_new_Generic_proxyObjectFactory () {}
  virtual CORBA::Object_ptr newProxyObject(Rope *r,
                                           CORBA::Octet *key,
                                           size_t keysize,
                                           IOP::TaggedProfileList *profiles,
                                           CORBA::Boolean release);
};


class _new_proxy_Int : //public virtual _new_proxy_Generic,
                       public virtual VDM::_proxy_Int
{
public:
  _new_proxy_Int (Rope *r,
                  CORBA::Octet *key,
                  size_t keysize,
                  IOP::TaggedProfileList *profiles,
                  CORBA::Boolean release) 
    : //_new_proxy_Generic(r,key,keysize,profiles,release),
    _proxy_Int(r,key,keysize,profiles,release),
    omniObject(VDM_Int_IntfRepoID,r,key,keysize,profiles,release) {}
  
  virtual ~_new_proxy_Int() { destroy(); }
  
};

class _new_Int_proxyObjectFactory : public VDM::Int_proxyObjectFactory
{
public:
  _new_Int_proxyObjectFactory () {}
  virtual ~_new_Int_proxyObjectFactory () {}
  virtual CORBA::Object_ptr newProxyObject(Rope *r,
                                           CORBA::Octet *key,
                                           size_t keysize,
                                           IOP::TaggedProfileList *profiles,
                                           CORBA::Boolean release);
};




class _new_proxy_Real : //public virtual _new_proxy_Generic,
                        public virtual VDM::_proxy_Real
{
public:
  _new_proxy_Real (Rope *r,
                   CORBA::Octet *key,
                   size_t keysize,
                   IOP::TaggedProfileList *profiles,
                   CORBA::Boolean release) 
    : //_new_proxy_Generic(r,key,keysize,profiles,release),
    _proxy_Real(r,key,keysize,profiles,release),
    omniObject(VDM_Real_IntfRepoID,r,key,keysize,profiles,release) {}
  
  virtual ~_new_proxy_Real() { destroy(); }
  
};

class _new_Real_proxyObjectFactory : public VDM::Real_proxyObjectFactory
{
public:
  _new_Real_proxyObjectFactory () {}
  virtual ~_new_Real_proxyObjectFactory () {}
  virtual CORBA::Object_ptr newProxyObject(Rope *r,
                                           CORBA::Octet *key,
                                           size_t keysize,
                                           IOP::TaggedProfileList *profiles,
                                           CORBA::Boolean release);
};


class _new_proxy_Sequence : //public virtual _new_proxy_Generic,
                            public virtual VDM::_proxy_Sequence
{
public:
  _new_proxy_Sequence (Rope *r,
                     CORBA::Octet *key,
                     size_t keysize,
                     IOP::TaggedProfileList *profiles,
                     CORBA::Boolean release) 
    : //_new_proxy_Generic(r,key,keysize,profiles,release),
    _proxy_Sequence(r,key,keysize,profiles,release),
    omniObject(VDM_Sequence_IntfRepoID,r,key,keysize,profiles,release) {}
  
  virtual ~_new_proxy_Sequence() { destroy(); }
  //  virtual void ImpAppend ( VDM::Generic_ptr  g ) { _proxy_Sequence::ImpAppend(g); }
  
};

class _new_Sequence_proxyObjectFactory : public VDM::Sequence_proxyObjectFactory
{
public:
  _new_Sequence_proxyObjectFactory () {}
  virtual ~_new_Sequence_proxyObjectFactory () {}
  virtual CORBA::Object_ptr newProxyObject(Rope *r,
                                           CORBA::Octet *key,
                                           size_t keysize,
                                           IOP::TaggedProfileList *profiles,
                                           CORBA::Boolean release);
};


#endif;
