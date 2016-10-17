/***
*  * WHAT
*  *    Auxiliary functions for the implemetation
*  * FILE
*  *    $Source: /home/vdmtools/cvsroot/toolbox/code/statsem/mk.cc,v $
*  * VERSION
*  *    $Revision: 1.17 $
*  * DATE
*  *    $Date: 2006/03/07 06:36:45 $
*  * STATUS
*  *    $State: Exp $
*  * REFERENCES
*  *    
*  * PROJECT
*  *    Afrodite - ESPRIT programme, project no. 6500.
*  * AUTHOR
*  *    Poul Boegh Lassen (PBL) + $Author: vdmtools $
*  * COPYRIGHT
*  *    (C) Kyushu University
***/

#include "statsem.h"

#ifndef NOSS

const Record StatSem::btp_bool   = mk_REP_BooleanTypeRep();
const Record StatSem::btp_natone = mk_REP_NumericTypeRep((Int)(NATONE));
const Record StatSem::btp_nat    = mk_REP_NumericTypeRep((Int)(NAT));
const Record StatSem::btp_int    = mk_REP_NumericTypeRep((Int)(INTEGER));
const Record StatSem::btp_rat    = mk_REP_NumericTypeRep((Int)(RAT));
const Record StatSem::btp_real   = mk_REP_NumericTypeRep((Int)(REAL));
const Record StatSem::btp_token  = mk_REP_TokenTypeRep();
const Record StatSem::btp_char   = mk_REP_CharTypeRep();

const Record StatSem::rep_alltp  = mk_REP_AllTypeRep();
const Record StatSem::rep_tmptp  = mk_REP_TmpTypeRep();
const Record StatSem::rep_rettp  = mk_REP_RetTypeRep();
const Record StatSem::rep_unittp = mk_REP_UnitTypeRep();
const Record StatSem::rep_niltp  = mk_REP_NilTypeRep();
             
//const Record StatSem::rep_emptyset = mk_REP_EmptySetTypeRep();
//const Record StatSem::rep_emptyseq = mk_REP_EmptySeqTypeRep();
//const Record StatSem::rep_emptymap = mk_REP_EmptyMapTypeRep();
             
const Record StatSem::set_alltp = mk_SetOfAllType();
const Record StatSem::seq_alltp = mk_SeqOfAllType();
const Record StatSem::map_alltp = mk_MapOfAllType();

TYPE_REP_UnionTypeRep StatSem::mk_SetOfAllType()
{
  return mk_REP_UnionTypeRep(mk_set(mk_REP_EmptySetTypeRep(mk_REP_AllTypeRep()),
                                    mk_REP_SetTypeRep(mk_REP_AllTypeRep())));
}

TYPE_REP_UnionTypeRep StatSem::mk_SeqOfAllType ()
{
  return mk_REP_UnionTypeRep(mk_set(mk_REP_EmptySeqTypeRep(mk_REP_AllTypeRep()),
                             mk_REP_SeqTypeRep(mk_REP_AllTypeRep())));
}

TYPE_REP_UnionTypeRep StatSem::mk_MapOfAllType ()
{
  return mk_REP_UnionTypeRep(mk_set(mk_REP_EmptyMapTypeRep(mk_REP_AllTypeRep(), mk_REP_AllTypeRep()),
                                    mk_REP_GeneralMapTypeRep(mk_REP_AllTypeRep(), mk_REP_AllTypeRep()),
                                    mk_REP_InjectiveMapTypeRep(mk_REP_AllTypeRep(), mk_REP_AllTypeRep())));
}

#endif //!NOSS

