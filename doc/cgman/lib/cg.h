/***
*  * WHAT
*  *    Code generator auxiliary functions
*  * ID
*  *    $Id: cg.h,v 1.16 2005/05/27 00:21:34 vdmtools Exp $
*  * PROJECT
*  *    Toolbox
*  * COPYRIGHT
*  *    (C) 2005, CSK
***/

#ifndef _cg_h
#define _cg_h

#include <string>
#include "metaiv.h"

void PrintPosition();
void RunTime(wstring);
void NotSupported(wstring);
void PushPosInfo(int, int);
void PopPosInfo();
void PushFile(wstring);
void PopFile();
void AddRecordTag(const wstring&, const int&);
bool ParseVDMValue(const wstring& filename, Generic& res);

// OPTIONS

bool cg_OptionGenValues();
bool cg_OptionGenFctOps();
bool cg_OptionGenTpInv();
#endif
