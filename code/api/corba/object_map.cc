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
*  *    $Source: /home/vdmtools/cvsroot/toolbox/code/api/corba/object_map.cc,v $
*  * VERSION
*  *    $Revision: 1.12 $
*  * DATE
*  *    $Date: 2005/05/27 00:28:34 $
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

#include <iostream>
#include "object_map.h"
#include "corba_api.hh"

#include "metaiv.h"
#include "metaiv_impl.h"

#include "metaiv.h"
#include "metaiv_impl.h"
#include "corba_api.h"

void ObjectMap::Insert(VDM::VDMGeneric_var obj_ref, void * v)
  // Inserts the new object in the slot computed by
  // CORBA::Object::_hash(). The object is inserted in the front of
  // the list.
  // Objects inserted with this version of Insert are not owned by any
  // clients!
{
  tbl[obj_ref->_hash(tbl_sz)].push_front(item(obj_ref, v, INITIAL_TAG_VALUE));
}

void ObjectMap::Insert(VDM::VDMGeneric_var obj_ref, void * v, VDM::ClientID id)
  // Inserts the new object in the slot computed by
  // CORBA::Object::_hash(). The inserted object is owned by client
  // id.
{
  /*
  cout << "Inserting " << obj_ref << endl;
  cout << "metaiv value is " << ((Generic_i*) v)->GetMetaivValue() << endl;
  */

  ObjectTagMap::iterator i = tagmap.find(id);
  if(i != tagmap.end()) {
    int tag = (*i).second;
    tbl[obj_ref->_hash(tbl_sz)].push_front(item(obj_ref, v, tag, id));
  }
  else {
    ThrowApiError(L"ObjectMap::Insert(): The ClientID passed does not exist. Need to call VDMAppliction::Register() to get a valid ClientID.",
		  L"ObjectMap::Insert");
  }
}


void * ObjectMap::Find(VDM::VDMGeneric_var obj)
  // Finds the implementation object associated with the object
  // reference obj. The implementation uses a move-to-front heuristic,
  // i.e. if found, the object is moved to the front of the list.
{
  cerr << L"Entering ObjectMap::Find" << endl;
  Echo();
  
  ItemsIterator i,j;
  ItemsList* l = &tbl[obj->_hash(tbl_sz)];
  i = l->begin();
  j = l->end();
  while( i!= l->end() && ! (*i).obj_ref->_is_equivalent(obj)) {
    ++i;
    //    cerr << L"i is " << (Generic*) (i->impl_obj) << endl;
    cerr << L"i is " << (Generic*) ((*i).impl_obj) << endl;
  }
  if(i==l->end()) {
    cerr << L"Leaving ObjectMap::Find" << endl;
    return NULL;
  }
  
  else {
    void *v;
    v = (*i).impl_obj;
    // Move object to front of list:
    cout << (*i).obj_ref << endl;
    l->push_front(*i);
    l->erase(i);
    cerr << L"Leaving ObjectMap::Find" << endl;
    return v;
  }
}


bool ObjectMap::Delete(VDM::VDMGeneric_var obj)
  // Removes the object from the hash table.
{
  ItemsIterator i;
  ItemsList* l = &tbl[obj->_hash(tbl_sz)];
  i = l->begin();
  while( i!= l->end() && ! (*i).obj_ref->_is_equivalent(obj)) {
    ++i;
  }
  if(i==l->end()) {
    return false;
  }
  else {
    l->erase(i);
    return true;
  }
}


void ObjectMap::Tag(VDM::ClientID id)
{ 
  ObjectTagMap::iterator i = tagmap.find(id);
  if(i != tagmap.end()) {
    // The client id is known already, increment the tag value:
    (*i).second++;
  }
  else {
    ThrowApiError(L"ObjectMap::Tag(): The ClientID passed does not exist. Need to call VDMAppliction::Register() to get a valid ClientID.",
		  L"ObjectMap::Tag");
  }
}

void ObjectMap::DestroyTag(VDM::ClientID id)
  // Disposes all CORBA objects tagged with the value contained in
  // tagmap[id].
{
  ObjectTagMap::iterator tm_i = tagmap.find(id);
  if(tm_i != tagmap.end()) {
    int tag = (*tm_i).second;
    if(tag){
      // Remove objects that are owned by id _and_ tagged by tag:
      for(int j=0; j<tbl_sz; j++) {
        ItemsIterator i;
        ItemsList* l = &tbl[j];
        i = l->begin();
        while( i!= l->end() ) {
          if( (*i).tag == tag && (*i).clientID == id ) {
            CORBA::BOA::getBOA()->dispose((*i).obj_ref);
            i = l->erase(i);
          }
          else {
            ++i;
          }
        }
      }
    }
    else {
      ThrowApiError(L"DestroyTag() failed. The tag stack was empty.",
		    L"ObjectMap::DestroyTag");
    }
    // Finally decrement the tag value of this client.
    (*tm_i).second--;
  }
  else {
    ThrowApiError(L"ObjectMap::DestroyTag(): The ClientID passed does not exist. Need to call VDMAppliction::Register() to get a valid ClientID.",
		  L"ObjectMap::DestroyTag");
  }
}


void ObjectMap::Register(VDM::ClientID id)
  // This function should be called prior to inserting any objects for
  // client id. The function creates an initial entry in the
  // ObjectTagMap for client id.
{
  tagmap[id] = INITIAL_TAG_VALUE;
}
  

void ObjectMap::CleanUpClient(VDM::ClientID id)
  // Disposes all CORBA objects owned by the client identified by id
{
  for(int j=0; j<tbl_sz; j++) {
    ItemsIterator i;
    ItemsList* l = &tbl[j];
    i = l->begin();
    while( i!= l->end() ) {
      if( (*i).clientID == id ) {
        CORBA::BOA::getBOA()->dispose((*i).obj_ref);
        i = l->erase(i);
      }
      else {
        ++i;
      }
    }
  }
  // Remove the client from the tagmap:
  ObjectTagMap::iterator i = tagmap.find(id);
  if(i != tagmap.end()) {
    tagmap.erase(i);
  }
}

void ObjectMap::Echo()
{
  cout << "\nCurrent Object Hash Table:\n";
  for(int j=0;j<tbl_sz; j++) {
    cout << tbl[j].size() << ": ";
    ItemsIterator i;
    ItemsList* l = &tbl[j];
    i = l->begin();
    while( i!= l->end() ) {
      cout << "(" << (*i).clientID << ", " << (*i).tag << "," <<
        (*i).obj_ref << "," <<
	//        ((Generic_i*) (*i).impl_obj)->GetMetaivValue() << 
	")";
      ++i;
      if(i!= l->end()) {
        cout << ", ";
      }
      else {
        cout << endl;
      }
    }
    cout << endl;
  }
  cout << "\nFinished Echo" << endl;
  
}


