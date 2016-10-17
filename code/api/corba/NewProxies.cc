#include "NewProxies.h"

CORBA::Object_ptr 
_new_Generic_proxyObjectFactory::newProxyObject(Rope *r,
                                                CORBA::Octet *key,
                                                size_t keysize,
                                                IOP::TaggedProfileList *profiles,
                                                CORBA::Boolean release)
{
  _new_proxy_Generic *p = new _new_proxy_Generic(r,key,keysize,profiles,release);
  return p;
}

CORBA::Object_ptr 
_new_Int_proxyObjectFactory::newProxyObject(Rope *r,
                                                CORBA::Octet *key,
                                                size_t keysize,
                                                IOP::TaggedProfileList *profiles,
                                                CORBA::Boolean release)
{
  _new_proxy_Int *p = new _new_proxy_Int(r,key,keysize,profiles,release);
  return p;
}

CORBA::Object_ptr 
_new_Real_proxyObjectFactory::newProxyObject(Rope *r,
                                                CORBA::Octet *key,
                                                size_t keysize,
                                                IOP::TaggedProfileList *profiles,
                                                CORBA::Boolean release)
{
  _new_proxy_Real *p = new _new_proxy_Real(r,key,keysize,profiles,release);
  return p;
}


CORBA::Object_ptr 
_new_Sequence_proxyObjectFactory::newProxyObject(Rope *r,
                                                CORBA::Octet *key,
                                                size_t keysize,
                                                IOP::TaggedProfileList *profiles,
                                                CORBA::Boolean release)
{
  _new_proxy_Sequence *p = new _new_proxy_Sequence(r,key,keysize,profiles,release);
  return p;
}


