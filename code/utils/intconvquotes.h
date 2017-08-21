/***
*  * WHAT
*  *    Header file for the intconvquotes filter
*  * FILE
*  *    $Source: /home/vdmtools/cvsroot/toolbox/code/utils/intconvquotes.h,v $
*  * VERSION
*  *    $Revision: 1.6 $
*  * DATE
*  *    $Date: 2006/03/14 07:15:23 $
*  * FORMAT
*  *    $State: Exp $
*  * PROJECT
*  *    Afrodite - ESPRIT III programme, project no. 6500.
*  * STATUS
*  *    Under development.
*  * AUTHOR
*  *    Henrik Voss
*  * COPYRIGHT
*  *    (C) Kyushu University
***/

#ifndef __intconvquotes_h__
#define __intconvquotes_h__

#include "metaiv.h"
#include "tagmap.h"

class INT2Q
{
public:
// Convert a handwritten AS to a generated AS
  static Generic h2gAS(const Generic & g);

// Convert a generated AS to a handwritten AS
  static Generic g2hAS(const Generic & g);

  static Generic TransformIntegers (const Generic & g);
  static Quote Integer2Quote(const Int & I);

  static Generic TransformIntegersBack(const Generic & g);
  static Int Quote2Integer(const Quote & Q);

private:
  static TagsTable tag_table;

  static bool IsCPPRec(const Record & r);
};

#endif // __intconvquotes_h__
