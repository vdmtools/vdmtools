/***
*  * WHAT
*  *    A simple hash table used to store all objects created through
*  *    the API. The table is used to lookup a CORBA object and return the
*  *    corresponding implementation object. This is necessary because
*  *    not all compilers support dynamic casts. 
*  *    Furthermore, the table is used to tag all objects created in
*  *    the API, so that objects can be freed/destroyed by using the
*  *    function VDMApplication::PushTag() and VDMApplication::DestroyTag()
*  *    
*  * FILE
*  *    $Source: /home/vdmtools/cvsroot/toolbox/code/api/corba/object_map.h,v $
*  * VERSION
*  *    $Revision: 1.8 $
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

#ifndef __object_map_h__
#define __object_map_h__

#include <vector>
#include <list>
#include <map>
//using namespace std;

#include "metaiv_idl.hh"
#include "corba_api.hh"

#define ObjectMap_HASH_SIZE 1013

enum {
  INITIAL_CLIENT_ID = 1,
  INITIAL_TAG_VALUE = 1
};

typedef std::map <VDM::ClientID, int> ObjectTagMap;

// An item of the object table:
//
struct item {
  VDM::VDMGeneric_var obj_ref;
  void * impl_obj;
  int tag;
  int clientID;
  item() : obj_ref(VDM::VDMGeneric::_nil()), 
    impl_obj(NULL), tag(0), clientID(0) { }
  item(VDM::VDMGeneric_var obj, void * imp, int tg, int id) : 
    obj_ref(obj), impl_obj(imp), tag(tg), clientID(id) {}
  item(VDM::VDMGeneric_var obj, void * imp, int tg) : 
    obj_ref(obj), impl_obj(imp), tag(tg), clientID(INITIAL_CLIENT_ID-1) {}

};


class ObjectMap{
public:
  ObjectMap(){ 
    tbl = std::vector< std::list<item> >(tbl_sz = ObjectMap_HASH_SIZE);
  }
  ObjectMap(int hash_sz){ tbl = std::vector< std::list<item> >(tbl_sz = hash_sz); }
  ~ObjectMap(){};
  void Insert(VDM::VDMGeneric_var, void *);
  void Insert(VDM::VDMGeneric_var, void *, VDM::ClientID);
  void * Find(VDM::VDMGeneric_var);
  bool Delete(VDM::VDMGeneric_var);

  void Tag(VDM::ClientID id);
  void DestroyTag(VDM::ClientID id);

  void CleanUpClient(VDM::ClientID);
  void Register(VDM::ClientID);

  void Echo();

private:
  //int k;
  std::vector< std::list<item> > tbl;
  int tbl_sz;

  // This map is used to keep track of the current tag of a given
  // client. That is, it is a map from ClientID to tag value. The methode
  // Tag() and DestroyTag() modifies the mapping.:
  //
  ObjectTagMap tagmap;
};

typedef std::list<item> ItemsList;
typedef std::list<item>::iterator ItemsIterator;

#endif // __object_map_h__



