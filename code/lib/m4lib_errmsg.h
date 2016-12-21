/***
*  * WHAT
*  *    Integer Record Tag definitions and error messages for
*  *    metaiv libraries
*  * FILE
*  *    $Source: /home/vdmtools/cvsroot/toolbox/code/lib/m4lib_errmsg.h,v $
*  * VERSION
*  *    $Revision: 1.11 $
*  * DATE
*  *    $Date: 2005/12/08 05:59:10 $
*  * FORMAT
*  *    $State: Exp $
*  * PROJECT
*  *    The VDM-SL Toolbox.
*  * AUTHOR
*  *    $Author: vdmtools $
*  * COPYRIGHT
*  *    (C) Kyushu University
***/

#ifndef __m4lib_errmsg_h__
#define __m4lib_errmsg_h__ 1
#include <string>
//using namespace std;

class M4LibError
{
public:
  static void ReportError(int, const std::wstring &);
  static void Complain(const std::wstring & title, const std::wstring & message);
  static void SetGUIMode(bool);
  static bool GetGUIMode();
private:
  static bool guimode;
};

#define ML_CONFLICTING_RNGVAL 1
#define ML_NOT_IN_DOM 2
#define ML_CAST_ERROR 3
#define ML_INDEX_OUT_OF_RANGE 4
#define ML_OP_ON_EMPTY_SEQ 5
#define ML_OP_ON_EMPTY_SET 6
#define ML_NOT_IN_SET 7
#define ML_ASSIGN_ERROR 8
#define ML_TRAVERSE_CONFLICT 9
#define ML_HD_ON_EMPTY_SEQUENCE 10
#define ML_TL_ON_EMPTY_SEQUENCE 11
#define ML_RANGE_ERROR 12
#define ML_ZERO_REF 13
#define ML_NULL_REF 14
#define ML_DIV_BY_ZERO 15
#define ML_TAG_NOT_FOUND 16
#define ML_WRONG_RECORD_SIZE 17
#define ML_WRITEVAL_NOT_SUPPORTED 18
#define ML_NULL_POINTER 19

#endif // __m4lib_errmsg_h__
