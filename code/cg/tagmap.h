/***
 *  * WHAT
 *  *    
 *  * FILE
 *  *    $Source: /home/vdmtools/cvsroot/toolbox/code/cg/tagmap.h,v $
 *  * VERSION
 *  *    $Revision: 1.8 $
 *  * DATE
 *  *    $Date: 2006/03/14 07:15:37 $
 *  *    
 *  * AUTHOR
 *  *    $Author: vdmtools $
 *  * COPYRIGHT
 *  *    (C) Kyushu University
 ***/

#ifndef __tagmap_h__
#define __tagmap_h__

#include "metaiv.h"

class TagsTable {

private:
  MAP<Quote, Int> q2imap;
  MAP<Int, Quote> i2qmap;

  void Insert(const Quote & key, const Int & tagv);

public:
  TagsTable();
  ~TagsTable();

  Int Compute(const Quote & key) const;
  Quote GetKey(const Int & tag) const; // Get the key corresponding to the int value
};

#endif // __tagmap_h__
