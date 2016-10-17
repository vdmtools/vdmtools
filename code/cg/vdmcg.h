/***  
*  * WHAT
*  *    VDM SL/++ Code Generator.
*  * FILE
*  *    $Source: /home/vdmtools/cvsroot/toolbox/code/cg/vdmcg.h,v $
*  * VERSION
*  *    $Revision: 1.24 $
*  * DATE
*  *    $Date: 2006/03/24 08:31:51 $
*  * STATUS
*  *    Under development
*  * REFERENCES
*  *    
*  * PROJECT
*  *    AFRODITE
*  * AUTHOR
*  *    Henrik Voss + $Author: vdmtools $
*  *
*  * COPYRIGHT
*  *    (C) Kyushu University
***/

#ifndef __vdmcg_h__
#define __vdmcg_h__

#include "metaiv.h"
#include "AS.h"
#include "CPP.h"
#include "CGMAIN.h"
#ifdef VDMPP
#include "CONC.h"
#endif // VDMPP

////////////
// VDMCG
////////////

class vdmcg {

private:
  const Nil nil;
  const Set eset;

public:

// VDM++ sources
  
#ifdef VDMPP

/** module CLASS **************************************************/
/** Reference : ***************************************************/
/******************************************************************/

#include "mod_class.h"

/** module CONC  **************************************************/
/** Reference : ***************************************************/
/******************************************************************/

#include "mod_conc.h"
  
/* module FVD *****************************************************/
/** Reference : ***************************************************/
/******************************************************************/

#include "mod_fvd.h"

#endif // VDMPP

// VDM-SL sources

#ifdef VDMSL
  
/** module MOD ****************************************************/
/** Reference : ***************************************************/
/******************************************************************/

#include "mod_mod.h"

/** module SD ****************************************************/
/** Reference : ***************************************************/
/******************************************************************/

#include "mod_statedef.h"

#endif // VDMSL

// Common sources


/** module CGMAIN  *****************************************************/
/** Reference : ***************************************************/
/******************************************************************/

#include "mod_cgmain.h"
  
/** module CGAUX ****************************************************/
/** Reference : ***************************************************/
/******************************************************************/
        
#include "mod_cgaux.h"
    

/** module DS ****************************************************/
/** Reference : ***************************************************/
/******************************************************************/

#include "mod_ds.h"

/** module STMT ****************************************************/
/** Reference : ***************************************************/
/******************************************************************/

#include "mod_stmt.h"

/** module EXPR ***************************************************/
/** Reference : ***************************************************/
/******************************************************************/

#include "mod_expr.h"

/** module FD *****************************************************/
/** Reference : ***************************************************/
/******************************************************************/

#include "mod_fctdef.h"

/** module PM *****************************************************/
/** Reference : ***************************************************/
/******************************************************************/

#include "mod_patmat.h"

/** module VD *****************************************************/
/** Reference : ***************************************************/
/******************************************************************/

#include "mod_valdef.h"

     
/** module TPGEN **************************************************/
/** Reference : ***************************************************/
/******************************************************************/

#include "mod_tpgen.h"

     
/** module TD *****************************************************/
/** Reference : ***************************************************/
/******************************************************************/

#include "mod_typedef.h"

/** Common Definitions ********************************************/
/******************************************************************/

  bool rti;
#ifdef VDMSL
  wstring (*GetSourceFile) (int fileid);
#endif // VDMSL

#ifdef VDMSL
  int cur_file;
#endif // VDMSL

  type_dL cur_file_name_l;

  TYPE_CGMAIN_VT mk_CG_VT(const TYPE_CPP_Expr &, const Generic &);

  SEQ<TYPE_CPP_Stmt> GenUpdatePosInfo(const Record &);
  TYPE_CPP_Stmt GenPushFile();
  TYPE_CPP_Stmt GenPopFile();
  TYPE_CPP_Stmt GenPushPosInfo(const SEQ<TYPE_CPP_Expr> &);
  TYPE_CPP_Stmt GenPopPosInfo();
  void SetCurFileName(const TYPE_CPP_Expr & nm);
  SEQ<TYPE_CPP_Expr> GetCurFileName();

public:

// Error reporting 

  void ReportUndefined(const wstring & fct);
  void ReportError(const wstring & fct);

#ifdef VDMSL
  void cg(const TYPE_AS_Document &,
          const Map &, const Set &,
          wstring (*_GetSourceFile) (int fileid),
          const Generic &, const Bool &, const Bool &,
          const Bool &, const Bool &, const Generic &,
          const Bool &);
#endif //VDMSL

#ifdef VDMPP
  void cg(const TYPE_AS_Document &,
          const SET<TYPE_AS_Name> &,
          const Generic &, const Bool &, const Bool &,
          const Bool &, const Bool &, const Generic &,
          const Bool &, const Bool &);
#endif //VDMPP
  void SetRTI(bool i) { rti = i; }

  void Reset(const TYPE_AS_Document &);
  
  vdmcg() {}
  ~vdmcg() {}
};

#endif // __vdmcg_h__
