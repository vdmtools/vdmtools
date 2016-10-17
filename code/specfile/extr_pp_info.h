/***
*  * WHAT
*  *    Header file the extr_pp_info file
*  * FILE
*  *    $Source: /home/vdmtools/cvsroot/toolbox/code/specfile/extr_pp_info.h,v $
*  * VERSION
*  *    $Revision: 1.3 $
*  * DATE
*  *    $Date: 2005/01/21 03:16:06 $
*  * FORMAT
*  *    $State: Exp $
*  * PROJECT
*  *    TB
*  * STATUS
*  *    Under development.
*  * AUTHOR
*  *    Lars T. Olsen
*  * COPYRIGHT
*  *    (C) Kyushu University
***/

#ifndef extr_pp_info_h_
#define extr_pp_info_h_

#include <fstream>
#include <stdlib.h>
#include "metaiv.h"
#include "tokenlist.h"
#include "contextinfo.h"

void extr_pp_info (
          const Generic& g,      // the AS tree to extr pp info from
          ContextInfo  & c,      // The context info table
          TokenList    & t,      // the token list to update
          bool         cov_on    // covering allowed 
                      );
#endif
