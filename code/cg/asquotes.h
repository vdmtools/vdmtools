/***
*  * WHAT
*  *    Integer Quote definitions for module AS.
*  * FILE
*  *    $Source: /home/vdmtools/cvsroot/toolbox/code/cg/asquotes.h,v $
*  * VERSION
*  *    $Revision: 1.17 $
*  * DATE
*  *    $Date: 2006/01/10 01:27:45 $
*  * FORMAT
*  *    $State: Exp $
*  * PROJECT
*  *    VDM-SL Toolbox.
*  * AUTHOR
*  *    $Author: vdmtools $
*  * COPYRIGHT
*  *    (C) Kyushu University
***/

#ifndef __asquotes_h__
#define __asquotes_h__

#include "AS.h"
#include "REP.h"
#include "SEM.h"

enum {

  AS_TAGS_BEGIN = 1000,
  NOTYETSPEC = TAG_quote_NOTYETSPEC, /* Both SL and PP */
  DEFAULT_AS   = TAG_quote_DEFAULT_AS,

#ifdef VDMPP
  SUBRESP   = TAG_quote_SUBRESP,
#endif //VDMPP
  PRIVATE_AS   = TAG_quote_PRIVATE_AS,
  PROTECTED_AS = TAG_quote_PROTECTED_AS,
  PUBLIC_AS    = TAG_quote_PUBLIC_AS,
  NOT_INITIALISED_AS = TAG_quote_NOT_INITIALISED_AS,
  ALL = TAG_quote_ALL,
  STRUCT  = TAG_quote_STRUCT,
  NATONE  = TAG_quote_NATONE,
  NAT  = TAG_quote_NAT,
  INTEGER  = TAG_quote_INTEGER,
  RAT  = TAG_quote_RAT,
  REAL  = TAG_quote_REAL,
  READ  = TAG_quote_READ,
  READWRITE  = TAG_quote_READWRITE,
  NUMPLUS  = TAG_quote_NUMPLUS,
  NUMMINUS  = TAG_quote_NUMMINUS,
  NUMABS  = TAG_quote_NUMABS,
  FLOOR  = TAG_quote_FLOOR,
  NOT  = TAG_quote_NOT,
  SETCARD  = TAG_quote_SETCARD,
  SETDISTRUNION  = TAG_quote_SETDISTRUNION,
  SETDISTRINTERSECT  = TAG_quote_SETDISTRINTERSECT,
  SETPOWER  = TAG_quote_SETPOWER,
  SEQLEN  = TAG_quote_SEQLEN,
  SEQDISTRCONC  = TAG_quote_SEQDISTRCONC,
  SEQELEMS  = TAG_quote_SEQELEMS,
  SEQINDICES  = TAG_quote_SEQINDICES,
  SEQTAIL  = TAG_quote_SEQTAIL,
  SEQHEAD  = TAG_quote_SEQHEAD,
  SEQREVERSE  = TAG_quote_SEQREVERSE,
  MAPDOM  = TAG_quote_MAPDOM,
  MAPRNG  = TAG_quote_MAPRNG,
  MAPDISTRMERGE  = TAG_quote_MAPDISTRMERGE,
  MAPINVERSE = TAG_quote_MAPINVERSE,
  NUMMULT  = TAG_quote_NUMMULT,
  NUMDIV  = TAG_quote_NUMDIV,
  NUMREM  = TAG_quote_NUMREM,
  NUMMOD  = TAG_quote_NUMMOD,
  NUMEXP  = TAG_quote_NUMEXP,
  INTDIV  = TAG_quote_INTDIV,
  NUMLT  = TAG_quote_NUMLT,
  NUMLE  = TAG_quote_NUMLE,
  NUMGT  = TAG_quote_NUMGT,
  NUMGE  = TAG_quote_NUMGE,
  AND  = TAG_quote_AND,
  OR  = TAG_quote_OR,
  IMPLY  = TAG_quote_IMPLY,
  EQUIV  = TAG_quote_EQUIV,
  EQ  = TAG_quote_EQ,
  NE  = TAG_quote_NE,
  SETUNION  = TAG_quote_SETUNION,
  SETINTERSECT  = TAG_quote_SETINTERSECT,
  SETMINUS  = TAG_quote_SETMINUS,
  SUBSET  = TAG_quote_SUBSET,
  PROPERSUBSET  = TAG_quote_PROPERSUBSET,
  INSET  = TAG_quote_INSET,
  NOTINSET  = TAG_quote_NOTINSET,
  SEQCONC  = TAG_quote_SEQCONC,
  MAPMERGE  = TAG_quote_MAPMERGE,
  MAPDOMRESTTO  = TAG_quote_MAPDOMRESTTO,
  MAPDOMRESTBY  = TAG_quote_MAPDOMRESTBY,
  MAPRNGRESTTO  = TAG_quote_MAPRNGRESTTO,
  MAPRNGRESTBY  = TAG_quote_MAPRNGRESTBY,
  COMPOSE  = TAG_quote_COMPOSE,
  EXISTS  = TAG_quote_EXISTS,
  AS_INPUT  = TAG_quote_INPUT,
  DEF  = TAG_quote_DEF,
  POS  = TAG_quote_POS,
  OK  = TAG_quote_OK,
  EMPTYMAP  = TAG_quote_EMPTYMAP,
  AS_PURE  = TAG_quote_PURE,
  INV  = TAG_quote_INV,
  POST  = TAG_quote_POST,
  INIT  = TAG_quote_INIT,
  EXCEP  = TAG_quote_EXCEP,
  EQUALITY = TAG_quote_EQUALITY,
  ORDER  = TAG_quote_ORDER,
  PRE  = TAG_quote_PRE,
  REN  = TAG_quote_REN,
  OP  = TAG_quote_OP,
  FUN  = TAG_quote_FUN,
  VAL  = TAG_quote_VAL,
  PAR  = TAG_quote_PAR,
  NONE  = TAG_quote_NONE,
  CUR  = TAG_quote_CUR,
  OLD  = TAG_quote_OLD,
  VALUE  = TAG_quote_VALUE,
  IMPL  = TAG_quote_IMPL,
  NOTOVERLAP  = TAG_quote_NOTOVERLAP,
  TYPE  = TAG_quote_TYPE,
  VAR  = TAG_quote_VAR,
//  METHOD  = TAG_quote_METHOD,
  STATE  = TAG_quote_STATE,
  TAG  = TAG_quote_TAG,
  PERM  = TAG_quote_PERM,
//  SPECIAL  = TAG_quote_SPECIAL,
  PTHREAD  = TAG_quote_PTHREAD,
  PUREOP  = TAG_quote_PUREOP,

#ifdef VDMPP
  OBJECT = TAG_quote_OBJECT,
  LOCAL = TAG_quote_LOCAL,
  GLOBAL = TAG_quote_GLOBAL,
#endif // VDMPP

  ANY = TAG_quote_ANY,
  ATLEASTONE = TAG_quote_ATLEASTONE,
  POSSIBLY = TAG_quote_POSSIBLY,

  PER  = TAG_quote_PER,
  DECL = TAG_quote_DECL,
  
  // Quotes form test.vdm
//  MET  = TAG_quote_MET,
//  INST  = TAG_quote_INST,
//  DUMMY  = TAG_quote_DUMMY,
  
  // Quotes from the dynamin semantics
  READ_ONLY  = TAG_quote_READ_ONLY, //SEM_Permission
  READ_WRITE  = TAG_quote_READ_WRITE, //SEM_Permission

//  APPLY = TAG_quote_APPLY, //
//  CALL  = TAG_quote_CALL, //
  
  // Qoutes from the static semantics
  REM  = TAG_quote_REM,// REM and ADD are used and defined in env.cc
  ADD  = TAG_quote_ADD,

  MACRO_FILE = TAG_quote_MACRO_FILE,
  MACRO_LINE = TAG_quote_MACRO_LINE,
  MACRO_COLUMN = TAG_quote_MACRO_COLUMN,
  MACRO_CLMOD = TAG_quote_MACRO_CLMOD,
  MACRO_FNOP = TAG_quote_MACRO_FNOP,

  // TO BE DELETED
//  APPROX  = TAG_quote_APPROX, // 20060105
#ifdef VDMPP
  FULLY  = 1000000000, //Full method spec.
#endif //VDMPP
  
};

#endif // __asquotes_h__
