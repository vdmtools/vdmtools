/***
*  * WHAT
*  *    Implementation of semantic record values
*  * FILE
*  *    $Source: /home/vdmtools/cvsroot/toolbox/code/eval/semrec.h,v $
*  * VERSION
*  *    $Revision: 1.14 $
*  * DATE
*  *    $Date: 2006/04/10 06:11:12 $
*  * FORMAT
*  *    $State: Exp $
*  * PROJECT
*  *    VDM-SL Toolbox
*  * STATUS
*  *    Under development.
*  * AUTHOR
*  *    $Author: vdmtools $
*  * COPYRIGHT
*  *    (C) Kyushu University
***/

#ifndef INCL_semrec_h
#define INCL_semrec_h

#include "metaiv.h"
#include "AS.h"
#include "semvalues.h"

/**************************************************************/
/* The SemRec class implements the semantic value of records. */
/* The specification of SEM`Rec is:                           */
/*     REC::                                                  */
/*       tag: AS`Name                                         */
/*         v : map nat to VAL                                 */
/*         v_dc:- map nat to VAL;                             */
/* The implementation of the semantic records is based on the */
/* don't care support in the metaiv-library. The record tag info*/
/* table used is defined locally in the SemRec class (DefSemRecInfo) */
/*                                                            */
/* The semantic record is implemented as a Record with the    */
/* SEM`REC tag. This record contains one field, that contains */
/* the record that implements the semantic record             */
/**************************************************************/

class SemRecTable
{
private:
  VDMRecInfoMap DefSemRecInfo;
  int next_tag;
  TYPE_AS_Name currentmodcls; // Current Module or Class Name.

public:
  SemRecTable() { Reset(); };
  ~SemRecTable() { Reset(); };
  void Reset() { this->DefSemRecInfo.clear(); this->next_tag = 1; };

  void SetCurrentModClass(const TYPE_AS_Name & name)
  { this->currentmodcls = name; };

  void Decl_SEM_REC(const TYPE_AS_Name & tag,
                    int size,
                    const SEQ<Bool> & dc_s);
  TYPE_DYNSEM_SEM_SemRecord mk_SEM_REC(const TYPE_AS_Name & tag,
                                       const SEQ<TYPE_SEM_VAL> & v_l);
  bool IsDecl(const TYPE_AS_Name & tag) const;
};

#endif // INCL_semrec_h
