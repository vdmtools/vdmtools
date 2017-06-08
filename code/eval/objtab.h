/***
*  * WHAT
*  *    object definition of vdm_InnerObjRef used in semantic object
*  *    references.
*  *    Definition of class vdm_ObjectTab
*  * FILE
*  *    $Id: objtab.h,v 1.16 2006/06/13 07:02:12 vdmtools Exp $
*  * COPYRIGHT
*  *    (C) Kyushu University
***/

#ifndef __objtab_h__
#define __objtab_h__

#include "metaiv.h"
#include "SEM.h"
#include "GLOBAL.h"

// Use private inheritance so we can define our own needed subset of
// the normal Map operations.

class ObjectTab : private MAP<TYPE_SEM_OBJ_uRef, TYPE_GLOBAL_OBJ_uDesc>
{
public:
  ObjectTab() {} ;
   ~ObjectTab() { this->reset(); };
 
  const TYPE_GLOBAL_OBJ_uDesc & operator[](const TYPE_SEM_OBJ_uRef & key) const
  { return (const TYPE_GLOBAL_OBJ_uDesc &) Map::CApply(key); };

  ObjectTab & Insert(const TYPE_SEM_OBJ_uRef & key, const TYPE_GLOBAL_OBJ_uDesc & desc)
  { Map::Insert(key, desc); return *this; };

  ObjectTab & ImpModify(const TYPE_SEM_OBJ_uRef & key, const TYPE_GLOBAL_OBJ_uDesc & desc)
  { Map::ImpModify(key, desc); return *this; };

  ObjectTab & RemElem(const TYPE_SEM_OBJ_uRef & key)
  { Map::RemElem(key); return *this; };
 
  int Size() const { return Map::Size(); }; 
  bool IsEmpty() const { return Map::IsEmpty(); }; 

  SET<TYPE_SEM_OBJ_uRef> Dom() const
  { return SET<TYPE_SEM_OBJ_uRef>(Map::Dom()); };

  bool DomExists(const TYPE_SEM_OBJ_uRef & g) const
  { return Map::DomExists(g); };
  bool DomExists(const TYPE_SEM_OBJ_uRef & g, Generic & res) const
  { return Map::DomExists(g, res); };

  void reset() { Map::Clear();} ;

  Map GetMap() const
  {
    Map m(*this);
    m.Clone();
    return m;
  };

  void GC() { Map::GC(); };
  void GC2(const Common & c) { Map::GC2(c); };

private:
  // Not exported. Should not be needed by clients
  ObjectTab(const Map & m ) : MAP<TYPE_SEM_OBJ_uRef, TYPE_GLOBAL_OBJ_uDesc>(m) {};
  ObjectTab(const Generic & g) : MAP<TYPE_SEM_OBJ_uRef, TYPE_GLOBAL_OBJ_uDesc>(g) {}; 

  ObjectTab & operator=(const ObjectTab & o)
  { Map::operator=((Map&)o); return *this; };

  ObjectTab & operator=(const Generic & g)
  { Map::operator=((Map&)g); return *this; };
}; 
#endif // __OBJTAB_H__

// Force emacs to use C++ mode for this file.
//// Local Variables:
//// mode: c++
//// End:

