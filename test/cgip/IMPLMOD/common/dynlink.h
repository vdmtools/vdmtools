/***
*  * WHAT
*  *    Dynamic linking extension in interpreter for implementation modules
*  * FILE
*  *    $Source: /home/vdmtools/cvsroot/toolbox/test/cgip/IMPLMOD/common/dynlink.h,v $
*  * VERSION
*  *    $Revision: 1.1.1.1 $
*  * DATE
*  *    $Date: 1997/02/06 12:04:40 $
*  * FORMAT
*  *    $State: Exp $
*  * PROJECT
*  *    IFAD VDM-SL Toolbox
*  * STATUS
*  *    Under development.
*  * AUTHOR
*  *    $Author: erik $
*  * COPYRIGHT
*  *   (C) 1992 IFAD, Denmark
***/
/****************************************************************************/

extern void * LoadDLImplModule(char * filename);
extern void * FindDLImplSymbol(void * libhandle, char *symbolname);
extern int DlLibClose(void * libhandle);
extern void * FindDLConstSymbol(void * libhandle, char *symbolname);
