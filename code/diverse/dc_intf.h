/***
*  * WHAT
*  *    header file for functions for the domain compiler interface.
*  * FILE
*  *    $Source: /home/vdmtools/cvsroot/toolbox/code/diverse/dc_intf.h,v $
*  * VERSION
*  *    $Revision: 1.3 $
*  * DATE
*  *    $Date: 2001/06/12 15:04:05 $
*  * STATUS
*  *    $State: Exp $
*  * PROJECT
*  *    Afrodite - ESPRIT programme, project no. 6500.
*  * AUTHOR
*  *    Poul Boegh Lassen (PBL) + $Author: paulm $
*  * LOG
*  *    $Log: dc_intf.h,v $
*  *    Revision 1.3  2001/06/12 15:04:05  paulm
*  *    Updates for wchar_t/wstring/string literals (i18n)
*  *
*  *    Revision 1.2  1997/06/02 12:09:47  erik
*  *    Changed String to wstring.
*  *
*  *    Revision 1.1.1.1  1995/03/21 13:25:43  henrik
*  *
 * Revision 1.1  1995/01/05  12:43:53  henrik
 * Initial revision
 *
 * Revision 1.1  1995/01/05  12:43:53  henrik
 * Initial revision
 *
*  * COPYRIGHT
*  *    (C) Kyushu University
***/

int GenerateImpl (ifstream& istr, int count);
void WriteImplFile (ofstream& ostr, wstring filename);
void InitImplScanner ();
