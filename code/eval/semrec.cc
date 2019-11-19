/***
*  * WHAT
*  *    Implementation of semantic record values
*  * FILE
*  *    $Source: /mnt/cvsroot/toolbox/code/eval/semrec.cc,v $
*  * VERSION
*  *    $Revision: 1.49 $
*  * DATE
*  *    $Date: 2006/09/22 11:14:06 $
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

//#include "semrec.h"
#include "semvalues.h"
#include "astaux.h"
#include "rt_errmsg.h"

void SemRecTable::Decl_SEM_REC(const TYPE_AS_Name & astag,
                               int size,
                               const SEQ<Bool> & dc_s)
{
  //TYPE_AS_Name qnm (UTIL::QualiName(this->currentmodcls, astag));
  TYPE_AS_Name qnm (AUX::ConstructDoubleName(this->currentmodcls, astag));

  int tag = this->next_tag;
  this->next_tag++;

  this->DefSemRecInfo.NewTag(tag, size);
  this->DefSemRecInfo.SetSymTag(tag, ASName2String(qnm));

  size_t len = dc_s.Length();
  for (size_t i = 1; i <= len; i++) {
    if (dc_s[i].GetValue()) {
      this->DefSemRecInfo.SetDontCare(tag, i);
    }
  }
}

TYPE_DYNSEM_SEM_SemRecord SemRecTable::mk_SEM_REC(const TYPE_AS_Name & astag, const SEQ<TYPE_SEM_VAL> & v_l)
{
  int tag = 0;
  int size = 0;
  if( !this->DefSemRecInfo.GetTag(ASName2String(astag), tag, size ) ) {
    RTERR::Error(L"SemRecord::mk_SEM_REC", RTERR_TAG_UNKNOWN, Nil(), Nil(), Sequence());
  }
  Record inner_rec(tag, size, this->DefSemRecInfo);
  inner_rec.SetFields(v_l);

  return TYPE_DYNSEM_SEM_SemRecord().Init(astag, inner_rec);
}

// The function decides if the record has been defined in the
// the semrec map before. (The function is used by the back-end
// of the intepreter).
bool SemRecTable::IsDecl(const TYPE_AS_Name & astag) const
{
  int tag = 0;
  int size = 0;
  return this->DefSemRecInfo.GetTag(ASName2String(astag), tag, size );
}

