/***
*  * WHAT
*  *  
*  * FILE
*  *    $Source: /home/vdmtools/cvsroot/toolbox/code/specman/code/StateStore_userdef.h,v $
*  * VERSION
*  *    $Revision: 1.2 $
*  * DATE
*  *    $Date: 2005/01/19 05:01:29 $
*  * FORMAT
*  *    $State: Exp $
*  * PROJECT
*  *    VDM-SL Toolbox.
*  * AUTHOR
*  *    $Author: vdmtools $
*  * COPYRIGHT
*  *    (C) Kyushu University
***/

#ifndef __StateStore_userdef_h__
#define __StateStore_userdef_h__

#include "main_tag.h"
#include "metaiv.h"
#include <string>

#define DEF_StateStore_EXTERNAL

class StateStoreAux
{
public:
  enum stasto_type {stasto_read, stasto_write};
public:
  static int ExtractVersion(const Generic & project, int current_version);
  static Sequence ConvertProject (int version, Generic project, int cur_ver);
  static Sequence AbsToRelPath(const Sequence & store, const std::wstring & pname);
  static Sequence RelToAbsPath(const Sequence & store, const std::wstring & pname);
  static Generic ReadAST (const Generic & store);
  static Generic WriteAST (const Generic & AST);

  static void AppendItem(const Generic & e);
  static Generic GetHeadItem();
  static void ClearItems();
  static void SetItems(const Sequence & s);
  static Sequence GetItems();

  static void SetProjectFilename(const std::wstring & fnm);
  static std::wstring GetProjectFilename();

  static void SetReadMode();
  static void SetWriteMode();
  static bool IsWriteMode();

private:
  static Sequence ProjectStore;
  static std::wstring projectfile;
  static int current_ast_version;
  static stasto_type stype;
};

#endif // __StateStore_userdef_h__
