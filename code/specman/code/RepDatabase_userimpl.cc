/***
*  * WHAT
*  *    
*  * FILE
*  *    $Source: /home/vdmtools/cvsroot/toolbox/code/specman/code/RepDatabase_userimpl.cc,v $
*  * VERSION
*  *    $Revision: 1.6 $
*  * DATE
*  *    $Date: 2006/04/19 08:11:25 $
*  * STATUS
*  *    $State: Exp $
*  * REFERENCES
*  *    IDERS-
*  * PROJECT
*  *    IDERS Project EP8593
*  * AUTHOR
*  *    $Author: vdmtools $
*  * LOG
*  *    $Log: RepDatabase_userimpl.cc,v $
*  *    Revision 1.6  2006/04/19 08:11:25  vdmtools
*  *    win32
*  *
*  *    Revision 1.5  2006/03/09 01:57:32  vdmtools
*  *    tempfile
*  *
*  *    Revision 1.4  2005/12/05 02:24:08  vdmtools
*  *    tempnam -> mkstemp
*  *
*  *    Revision 1.3  2001/06/12 15:04:56  paulm
*  *    Updates for wchar_t/wstring/string literals (i18n)
*  *
*  *    Revision 1.2  2000/05/02 15:27:55  blackie
*  *    removed a compiler warning
*  *
*  *    Revision 1.1  1998/09/18 15:09:56  jeppe
*  *    Update wrt. cg naming revision: 'external' changed to 'userdef' or 'userimpl',
*  *    'global' changed to 'anonym'.
*  *
*  *    Revision 1.1  1998/08/26 14:03:36  jeppe
*  *    Added prelim operations CreateTempFile and RemoveTempFile to RepDatabase
*  *
*  * COPYRIGHT
*  *    (C) Kyushu University
***/

#ifndef _MSC_VER
#include <unistd.h>
#endif // _MSC_VER

#include <stdlib.h>

#include "projectval.h" 
#include "tbutils.h"
#include "tb_pthread.h"
 
MUTEX_T mutex_repdb;

TYPE_ProjectTypes_TmpFileName vdm_RepDatabase::vdm_CreateTempFile()
{
  wstring tmpfile(TBUTILS::tb_tempnam (L"vdmd6"));
  return PTAUX::mk_FileName (tmpfile);
}

void vdm_RepDatabase::vdm_RemoveTempFile (const TYPE_ProjectTypes_TmpFileName &tmpnm)
{
  wstring tmpfile = PTAUX::ExtractFileName (tmpnm);
  if (tmpfile.c_str()) {
    TBUTILS::remove_file(tmpfile);
  }
}

void vdm_RepDatabase::vdm_Init()
{
  MUTEX_INIT(mutex_repdb);
}

void vdm_RepDatabase::vdm_Finish()
{
  MUTEX_DESTROY(mutex_repdb);
}

void vdm_RepDatabase::vdm_Lock()
{
  MUTEX_LOCK(mutex_repdb);
}

void vdm_RepDatabase::vdm_UnLock()
{
  MUTEX_UNLOCK(mutex_repdb);
}

