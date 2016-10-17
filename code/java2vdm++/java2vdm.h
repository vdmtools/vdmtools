#ifndef __java2vdm_h__
#define __java2vdm_h__

#include "Errors.h"
#include "contextinfo.h"
#include "tokenlist.h"

bool java_parse (wchar_t* fname,
                 bool POS_WANTED,           // include position information
                 vdm_Errors* _errs,         // List of errors generated
                 TYPE_CPP_File& res,        // the resulting AST
                 ContextInfo& ci, 
                 bool reset_ci, 
                 const TYPE_CI_FileId & fid);    // the file id

void SetInput(ifstream& inp);

#endif // __java2vdm_h__
