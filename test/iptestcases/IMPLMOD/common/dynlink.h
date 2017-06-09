/***
*  * WHAT
*  *    Dynamic linking extension in interpreter for implementation modules
*  * FILE
*  *    $Source: /home/vdmtools/cvsroot/toolbox/test/iptestcases/IMPLMOD/common/dynlink.h,v $
*  * VERSION
*  *    $Revision: 1.1 $
*  * DATE
*  *    $Date: 2000/07/24 18:46:17 $
*  * FORMAT
*  *    $State: Exp $
*  * PROJECT
*  *    IFAD VDM-SL Toolbox
*  * STATUS
*  *    Under development.
*  * AUTHOR
*  *    $Author: blackie $
*  * COPYRIGHT
*  *   (C) 1992 IFAD, Denmark
***/
/****************************************************************************/

extern void * LoadDLImplModule(char * filename);
extern void * FindDLImplSymbol(void * libhandle, char *symbolname);
extern int DlLibClose(void * libhandle);
extern void * FindDLConstSymbol(void * libhandle, char *symbolname);
